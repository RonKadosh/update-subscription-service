package bgu.spl.net.srv;

import java.util.ArrayList;
import java.util.concurrent.ConcurrentHashMap;

public class WorldCupConnections<T> implements Connections<T>{
    private int message_id;
    private ConcurrentHashMap<Integer,ConnectionHandler<T>> currConnections;
    private ConcurrentHashMap<String, ArrayList<User>> currTopics;
    private ConcurrentHashMap<String,User> users;
    private ConcurrentHashMap<Integer,User> mapCIDtoUser;
    WorldCupConnections(ConcurrentHashMap<Integer,ConnectionHandler<T>> currConnections){
        message_id=0;
        this.currConnections=currConnections;
        mapCIDtoUser = new ConcurrentHashMap<>();
        currTopics=new ConcurrentHashMap<>();
        users=new ConcurrentHashMap<>();
    }
    @Override
    public boolean send(int connectionId, T msg) {
        currConnections.get(connectionId).send(msg);
        return false;
    }


    @Override
    public void send(String channel, T msg) {
        for (User s:currTopics.get(channel)){
            send(s.getCID(),msg);
        }
    }

    @Override
    public void disconnect(int connectionId) {
        mapCIDtoUser.get(connectionId).disconnectUser();
        mapCIDtoUser.remove(connectionId);
        currConnections.remove(connectionId);
    }

    @Override
    public ConcurrentHashMap<String, User> getUsers() {
        return users;
    }

    @Override
    public ConcurrentHashMap<String, ArrayList<User>> getTopics() {
        return currTopics;
    }

    @Override
    public void attachCIDtoUser(int connectionId, User s) {
        s.setConnectionID(connectionId);
        mapCIDtoUser.put(connectionId,s);
    }

    @Override
    public User getUserFromConnectionId(int connectionId) {
        return mapCIDtoUser.get(connectionId);
    }

    @Override
    public synchronized String getMsgID() {
        String ret= String.valueOf(message_id);
        message_id++;
        return ret;
    }

    public boolean connectUser(String username,String password){
        return true;
    }
    public boolean subscribeToChannel(String cId,String topic){
        return true;
    }

}
