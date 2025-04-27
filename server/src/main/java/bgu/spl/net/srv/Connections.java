package bgu.spl.net.srv;

import java.util.ArrayList;
import java.util.concurrent.ConcurrentHashMap;

public interface Connections<T> {

    boolean send(int connectionId, T msg);

    void send(String channel, T msg);

    void disconnect(int connectionId);
    ConcurrentHashMap<String,User> getUsers();
    ConcurrentHashMap<String, ArrayList<User>> getTopics();

    void attachCIDtoUser(int connectionId, User s);

    User getUserFromConnectionId(int connectionId);

    String getMsgID();
}
