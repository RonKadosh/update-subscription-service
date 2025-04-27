package bgu.spl.net.srv;

import java.util.Collection;
import java.util.Hashtable;
import java.util.Objects;

public class User {
        public final String host;
        public final String userName;
        public final String password;
        public boolean isConnected;
        public int CurrConnectionId;
        private int cID;
        private Hashtable<String, String> mapIDtoTopic;

        public User(String _host, String _userName, String _password, boolean _isConnected){
            this.host = _host;
            this.userName = _userName;
            this.password = _password;
            this.isConnected = _isConnected;
            mapIDtoTopic =new Hashtable<>();
        }

        public void connectUser(){
            this.isConnected = true;
        }
        public void disconnectUser(){
            this.isConnected = false;
            mapIDtoTopic=new Hashtable<>();
            cID=-1;
        }
        public boolean matchPassword(String password){
            return Objects.equals(this.password, password);
        }

    public boolean subscribe(String topic,String subId) {
            if(mapIDtoTopic.containsKey(subId)) return false;
            else mapIDtoTopic.put(subId,topic);
            return true;
    }

    public String getTopicNameFromSubID(String id) {
            return mapIDtoTopic.get(id);
    }

    public void unsubscribe(String id) {
            mapIDtoTopic.remove(id);
    }

    public Collection<String> getTopicNames() {
            return mapIDtoTopic.values();
    }

    public String getIDfromTopicName(String destination) {
            for(String ID:mapIDtoTopic.keySet()){
                if (Objects.equals(mapIDtoTopic.get(ID), destination)) return ID;
            }
            return null;
    }

    public void setConnectionID(int connectionId) {
            cID=connectionId;
    }

    public int getCID() {
            return cID;
    }
}
