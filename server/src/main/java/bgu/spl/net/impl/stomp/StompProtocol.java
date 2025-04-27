package bgu.spl.net.impl.stomp;

import bgu.spl.net.api.StompMessagingProtocol;
import bgu.spl.net.srv.Connections;
import bgu.spl.net.srv.User;
import java.time.LocalDateTime;
import java.util.*;


public class StompProtocol implements StompMessagingProtocol<String> {
    private Hashtable<String,String> stompFrame;
    private int connectionId;
    private String thisMessage;
    private Connections<String> connections;

    @Override
    public void start(int connectionId, Connections<String> connections) {
        stompFrame = new Hashtable<>();
        this.connectionId=connectionId;
        this.connections=connections;
    }

    @Override
    public void process(String message) {
        System.out.println("["+ LocalDateTime.now() + "]:"+Thread.currentThread().getName() +
                " is currently starting to process message:\n"+message);
        thisMessage=message.substring(0,message.length()-1);
        String frameType = message.substring(0,message.indexOf("\n"));
        String stompBody = message.substring(message.indexOf("\n")+1);
        String MessageOrNay = readStompHeaders(stompBody);
        int has_receipt=0;
        if (stompFrame.containsKey("receipt")) has_receipt=1;
        if(!Objects.equals(MessageOrNay, "\n")){
            stompFrame.put("game_updates",exctractMsg(MessageOrNay.substring(1)));
        }
        stompFrame.put("FrameType",frameType);
        if(connections.getUserFromConnectionId(connectionId)==null&& !frameType.equals("CONNECT"))
            generateError("Not logged in error" , "client cannot make any action before logging in user.",has_receipt);
        else{
        switch (frameType){
            case "CONNECT":{
                checkConnectMsg(has_receipt);
                if(!Objects.equals(stompFrame.get("FrameType"), "ERROR"))
                    tryToLogin(has_receipt);
                break;
                }
            case "SEND":{
                checkSendMsg(has_receipt);
                if(!Objects.equals(stompFrame.get("FrameType"), "ERROR"))
                    buildMessage(has_receipt);
                break;
            }
            case "SUBSCRIBE": {
                checkSubscribeMsg(has_receipt);
                if(!Objects.equals(stompFrame.get("FrameType"), "ERROR"))
                    tryToSubscribeUser(has_receipt);
                break;
            }
            case "UNSUBSCRIBE": {
                checkUnsubscribeMsg(has_receipt);
                if(!Objects.equals(stompFrame.get("FrameType"), "ERROR"))
                    tryToUnsubscribeUser(has_receipt);
                break;
                }
            case "DISCONNECT":{
                checkDisconnectMsg(has_receipt);
                if(!Objects.equals(stompFrame.get("FrameType"), "ERROR"))
                    tryToDisconnect();
                break;
            }
            default:generateError("Stomp Syntax Error","Illegal STOMP frame type.",has_receipt);
        }
        }
        if (stompFrame!=null){
            String s=hashmapToStomp(stompFrame);
            connections.send(connectionId,s);
            System.out.println("["+ LocalDateTime.now() + "]:"+Thread.currentThread().getName() +
                    " has sent back message:\n"+s);
            if(frameType.equals("DISCONNECT")) connections.disconnect(connectionId);}
    }

    private String hashmapToStomp(Hashtable<String,String> stompFrame) {
        String STOMP="";
        switch (stompFrame.get("FrameType")){
            case "ERROR":{
                STOMP=STOMP+"ERROR\n";
                if(stompFrame.containsKey("receipt-id")){
                    STOMP=STOMP+"receipt-id:" +stompFrame.get("receipt-id")+"\n";
                }
                STOMP=STOMP+"message:" +stompFrame.get("errorType")+"\n\n"+"The message:\n-----\n" +stompFrame.get("originalMessage")+"\n"+"-----"+
                        stompFrame.get("errorDescription");
                return STOMP;
            }
            case "RECEIPT":{
                STOMP=STOMP+"RECEIPT\nreceipt-id:" + stompFrame.get("receipt-id")+"\n\n";
                return STOMP;
            }
            case "MESSAGE":{
                STOMP=STOMP+"MESSAGE\nsubscription:" + stompFrame.get("subscription") +"\n" +"message-id:" +
                        stompFrame.get("message-id") +"\ndestination:"+stompFrame.get("destination")+"\n\n"+stompFrame.get("message");
                return STOMP;
            }
            case "CONNECTED":{
                STOMP=STOMP+"CONNECTED\n"+"version:"+stompFrame.get("accept-version")+"\n\n";
            }
        }
        return STOMP;
    }

    private void buildMessage(int has_receipt) {
        User s=connections.getUserFromConnectionId(connectionId);
        String dest_id=s.getIDfromTopicName(stompFrame.get("destination"));
        if (dest_id==null) generateError("SEND command error","destination not found.",has_receipt);
        else{
            Hashtable<String,String> msg=new Hashtable<>();
            msg.put("FrameType","MESSAGE");
            msg.put("subscription",dest_id);
            msg.put("message-id",connections.getMsgID());
            msg.put("message",stompFrame.get("game_updates"));
            msg.put("destination",stompFrame.get("destination"));
            String send = hashmapToStomp(msg);
            connections.send(stompFrame.get("destination"),send);
            System.out.println("["+LocalDateTime.now()+"]:"+Thread.currentThread().getName()
                    +" has processed the frame and sent back a message:\n" + send);
            if (has_receipt==1){
                generateReceipt();
            }
            else stompFrame=null;
        }
    }

    private void tryToDisconnect() {
        User s=connections.getUserFromConnectionId(connectionId);
        for(String topic_name:s.getTopicNames()){
            connections.getTopics().get(topic_name).remove(s);
        }
        generateReceipt();
    }

    private void tryToUnsubscribeUser(int has_receipt) {
        User s=connections.getUserFromConnectionId(connectionId);
        String topic_name=s.getTopicNameFromSubID(stompFrame.get("id"));
        if(topic_name!=null){
            s.unsubscribe(stompFrame.get("id"));
            connections.getTopics().get(topic_name).remove(s);
            if (has_receipt==1){
                generateReceipt();
            }
            else stompFrame=null;
        }
        else generateError("Unsubscribe Error","the user is trying to unsubscribe from non-existing id.",has_receipt);
    }

    private void tryToSubscribeUser(int has_receipt) {
        User s=connections.getUserFromConnectionId(connectionId);
        if (s.subscribe(stompFrame.get("destination"),stompFrame.get("id"))){
            if (!connections.getTopics().containsKey(stompFrame.get("destination")))
                connections.getTopics().put(stompFrame.get("destination"),new ArrayList<>());
            connections.getTopics().get(stompFrame.get("destination")).add(s);
            if (has_receipt==1){
                generateReceipt();
            }
            else stompFrame=null;
        }
        else generateError("Subscription Error","User is already subscribed to this subscription id.",has_receipt);
    }

    private void generateReceipt() {
        Hashtable<String,String> rec=new Hashtable<>();
        rec.put("FrameType","RECEIPT");
        rec.put("receipt-id",stompFrame.get("receipt"));
        stompFrame=rec;
    }

    private void tryToLogin(int receipt) {
        String username =stompFrame.get("login");
        if(!connections.getUsers().containsKey(username)){
            if (!Objects.equals(stompFrame.get("host"), "stomp.cs.bgu.ac.il\n"))
                if (!Objects.equals(stompFrame.get("accept-version"), "1.2\n"))
                    {
                        User s=new User(stompFrame.get("host"),username,stompFrame.get("passcode"),true);
                        connections.getUsers().put(username,s);
                        connections.attachCIDtoUser(connectionId,s);
                        Hashtable<String,String> connected=new Hashtable<>();
                        connected.put("FrameType","CONNECTED");
                        connected.put("accept-version","1.2");
                        stompFrame=connected;
                    }
                else generateError("Login Error","The accept-version is wrong!",receipt);
            else generateError("Login Error","The host is illegal.",receipt);
        }
        else if(connections.getUsers().get(username).isConnected) {
            generateError("Login Error", "The user is already Logged in", receipt);
            }
        else if(!connections.getUsers().get(username).matchPassword(stompFrame.get("passcode"))){
            generateError("Login Error","The user password is incorrect",receipt);
        }
        else {
            Hashtable<String,String> connected=new Hashtable<>();
            connected.put("FrameType","CONNECTED");
            connected.put("accept-version","1.2");
            stompFrame=connected;
            connections.attachCIDtoUser(connectionId,connections.getUsers().get(username));
            connections.getUsers().get(username).connectUser();
        }
    }

    private String exctractMsg(String messageOrNay) {
        return messageOrNay;
    }

    private void checkDisconnectMsg(int receipt) {
        if(stompFrame.containsKey("game_updates"))
            generateError("Stomp Syntax Error","DISCONNECT frame must not have a body.",receipt);
        else if(stompFrame.size()==2){
            if (receipt==0){
                generateError("Stomp Syntax Error","DISCONNECT frame must include receipt header",receipt);
            }
        }
        else generateError("Stomp Syntax Error","DISCONNECT frame must have 1 header, instead got " + (stompFrame.size()-1)+".",receipt);
    }

    private void checkUnsubscribeMsg(int receipt) {
        if(stompFrame.containsKey("game_updates"))
            generateError("Stomp Syntax Error","UNSUBSCRIBE frame must not have a body.",receipt);
        else if(stompFrame.size()==2+receipt){
            if(!stompFrame.containsKey("id")){
                generateError("Stomp Syntax Error","UNSUBSCRIBE frame must have id header",receipt);
            }
        }
        else generateError("Stomp Syntax Error","UNSUBSCRIBE frame must have 1 header(not including optional receipt) instead got "+(stompFrame.size()-1)+".",receipt);
    }

    private void checkSubscribeMsg(int receipt) {
        if(stompFrame.containsKey("game_updates"))
            generateError("Stomp Syntax Error","SUBSCRIBE frame must not have a body.",receipt);
        else if(stompFrame.size()==3+receipt){
            if(!stompFrame.containsKey("id")|!stompFrame.containsKey("destination")){
                generateError("Stomp Syntax Error","SUBSCRIBE frame must have the next headers: destination,id.",receipt);
            }
        }
        else generateError("Stomp Syntax Error","SUBSCRIBE frame must have 2 headers(not including optinal receipt) instead got " + (stompFrame.size()-1)+".",receipt);
    }

    private void checkSendMsg(int receipt) {
        if(!stompFrame.containsKey("game_updates"))
            generateError("Stomp Syntax Error","SEND frame must have a body.",receipt);
        else if(stompFrame.size()==3+receipt){
            if(!stompFrame.containsKey("destination")){
                generateError("Stomp Syntax Error","SEND frame must have a destination header.",receipt);
            }
        }
        else generateError("Stomp Syntax Error","SEND frame must have 1 header(not including optional receipt) instead got " + (stompFrame.size()-2) + ".",receipt);
    }

    private void checkConnectMsg(int receipt) {
        if(stompFrame.containsKey("game_updates"))
            generateError("Stomp Syntax Error","CONNECT frame must not have a body.",receipt);
        else if(stompFrame.size()==5+receipt){
            if(!stompFrame.containsKey("accept-version")|!stompFrame.containsKey("host")|!stompFrame.containsKey("login")|!stompFrame.containsKey("passcode")){
                generateError("Stomp Syntax Error","CONNECT frame must have the headers:accept-version,host,login,passcode",receipt);
            }
        }
        else generateError("Stomp Syntax Error","CONNECT frame must have 4 headers(not including optional receipt) instead got " + (stompFrame.size()-1) + ".",receipt);
    }

    private String readStompHeaders(String cutMessage) {
        while (!Objects.equals(cutMessage.substring(0,1), "\n")){
                int indexofhozez=cutMessage.indexOf(":");
                int indexofshura=cutMessage.indexOf("\n");
                stompFrame.put(cutMessage.substring(0,indexofhozez),cutMessage.substring(indexofhozez+1,indexofshura));
                cutMessage=cutMessage.substring(indexofshura+1);
        }
        return cutMessage;
    }

    @Override
    public boolean shouldTerminate() {
        return false;
    }
    public void generateError(String errorType,String errorDescription,int has_receipt){
        Hashtable<String,String> error=new Hashtable<>();
        error.put("FrameType","ERROR");
        error.put("errorType",errorType);
        error.put("errorDescription",errorDescription);
        error.put("originalMessage",thisMessage);
        if (has_receipt==1) error.put("receipt-id",stompFrame.get("receipt"));
        stompFrame=error;
    }
}
