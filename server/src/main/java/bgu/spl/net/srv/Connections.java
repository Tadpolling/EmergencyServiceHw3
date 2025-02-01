package bgu.spl.net.srv;

import java.io.IOException;
import java.util.function.Function;

public interface Connections<T> {

    boolean send(int connectionId, T msg);

    void send(String channel, T msg);

    void send(String channel,int senderConnectionId, Function<Integer,T> createMessageFunc);

    void disconnect(int connectionId);

    void connect(int connectionId, ConnectionHandler<T> handler);

    void subscribe(int connectionId, String channel,int id);

    void unsubscribe(int connectionId, int id);
}
