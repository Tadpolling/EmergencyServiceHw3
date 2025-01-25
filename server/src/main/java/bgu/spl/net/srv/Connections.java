package bgu.spl.net.srv;

import java.io.IOException;

public interface Connections<T> {

    boolean send(int connectionId, T msg);

    void send(String channel, T msg);

    void disconnect(int connectionId);

    void connect(int connectionId, ConnectionHandler<T> handler);

    void subscribe(int connectionId, String channel,int id);

    void unsubscribe(int connectionId, int id);
}
