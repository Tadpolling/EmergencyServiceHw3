package bgu.spl.net.srv;

import java.util.HashMap;

public class ConnectionsImpl<T> implements Connections<T> {

    HashMap<Integer,ConnectionHandler<T>> connectionIdToClient;
    HashMap<String,HashMap<Integer,Boolean>> channelToSubscribedClients;

    public ConnectionsImpl() {
        connectionIdToClient = new HashMap<>();
        channelToSubscribedClients = new HashMap<>();
    }

    @Override
    public boolean send(int connectionId, T msg) {
        if(!connectionIdToClient.containsKey(connectionId))
            return false;
        connectionIdToClient.get(connectionId).send(msg);
        return true;
    }

    @Override
    public void send(String channel, T msg) {
        for(int connectionId :  channelToSubscribedClients.get(channel).keySet())
        {
            send(connectionId, msg);
        }
    }

    @Override
    public void disconnect(int connectionId) {
        connectionIdToClient.remove(connectionId);
        for(String channel:channelToSubscribedClients.keySet())
        {
            channelToSubscribedClients.get(channel).remove(connectionId);
        }
    }

    public void connect(int connectionId, ConnectionHandler<T> handler) {
        connectionIdToClient.put(connectionId, handler);
    }

    public void subscribe(int connectionId, String channel) {
        if (!channelToSubscribedClients.containsKey(channel))
            channelToSubscribedClients.put(channel, new HashMap<>());
        channelToSubscribedClients.get(channel).put(connectionId, true);
        
    }

}
