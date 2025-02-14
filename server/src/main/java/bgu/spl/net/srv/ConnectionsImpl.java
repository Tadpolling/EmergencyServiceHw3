package bgu.spl.net.srv;

import bgu.spl.net.api.StompResponse;
import bgu.spl.net.api.StompResponseHandler;

import java.util.HashMap;
import java.util.concurrent.ConcurrentHashMap;
import java.util.concurrent.atomic.AtomicInteger;
import java.util.function.Function;


public class ConnectionsImpl<T> implements Connections<T> {

    // Key- connectionId,value ConnectionHandler for that client.
    ConcurrentHashMap<Integer,ConnectionHandler<T>> connectionIdToClient;
    // First Key - channel/Topic, Second Key- connectionId, Value- Subscription id.
    ConcurrentHashMap<String,ConcurrentHashMap<Integer,Integer>> channelToSubscribedClients;
    // Key- (connectionId,subscription id), value - channel/topic

    ConcurrentHashMap<String, String>  subscriptionToTopic;

    public ConnectionsImpl() {
        connectionIdToClient = new ConcurrentHashMap<>();
        channelToSubscribedClients = new ConcurrentHashMap<>();
        subscriptionToTopic = new ConcurrentHashMap<>();
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
            int subId = channelToSubscribedClients.get(channel).get(connectionId);
            connectionIdToClient.get(connectionId).send(msg);
        }
    }

    @Override
    public void send(String channel,int senderConnectionId, Function<Integer,T> createMessageFunc) {
        synchronized (channelToSubscribedClients) {
            synchronized(connectionIdToClient) {
                for (int connectionId : channelToSubscribedClients.get(channel).keySet()) {
                    if (senderConnectionId == connectionId)
                        continue;
                    int subId = channelToSubscribedClients.get(channel).get(connectionId);
                    connectionIdToClient.get(connectionId).send(createMessageFunc.apply(subId));
                }
            }
        }
    }

    @Override
    public void disconnect(int connectionId) {
        synchronized (connectionIdToClient) {
            synchronized(channelToSubscribedClients) {
                synchronized(subscriptionToTopic) {

                    connectionIdToClient.remove(connectionId);
                    for (String channel : channelToSubscribedClients.keySet()) {
                        Integer subscriptionId = channelToSubscribedClients.get(channel).remove(connectionId);
                        if (subscriptionId != null) {
                            subscriptionToTopic.remove(combineConnecitonIdAndSubscriptionId(connectionId, subscriptionId));
                        }
                    }
                }
            }
        }
    }

    @Override
    public void connect(int connectionId, ConnectionHandler<T> handler) {
        synchronized (connectionIdToClient) {
            connectionIdToClient.put(connectionId, handler);
        }
    }

    @Override
    public void subscribe(int connectionId, String channel, int subscriptionId) {
        synchronized (channelToSubscribedClients) {
            synchronized (subscriptionToTopic) {
                if (!channelToSubscribedClients.containsKey(channel))
                    channelToSubscribedClients.put(channel, new ConcurrentHashMap<>());
                channelToSubscribedClients.get(channel).put(connectionId, subscriptionId);
                subscriptionToTopic.put(combineConnecitonIdAndSubscriptionId(connectionId, subscriptionId), channel);
            }
        }
    }

    @Override
    public  void unsubscribe(int connectionId, int subscriptionId){
        synchronized (channelToSubscribedClients) {
            synchronized (subscriptionToTopic) {
                String topic = subscriptionToTopic.remove(combineConnecitonIdAndSubscriptionId(connectionId, subscriptionId));
                channelToSubscribedClients.get(topic).remove(connectionId);
            }
        }
    }

    private String combineConnecitonIdAndSubscriptionId(int connectionId, int subscriptionId) {
        return connectionId + ":" + subscriptionId;
    }
}
