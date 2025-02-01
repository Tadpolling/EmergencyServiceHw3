package bgu.spl.net.api;
import bgu.spl.net.api.StompMessageDetails;
import bgu.spl.net.srv.ConnectionHandler;
import bgu.spl.net.srv.Connections;
import utils.Pair;

import java.util.concurrent.atomic.AtomicInteger;
import java.util.function.Function;


public class StompMessagingProtocolImpl implements StompMessagingProtocol<String> {

    private static AtomicInteger messageIdCounter = new AtomicInteger(0);

    private int connectionId;
    private Connections<String> connections;
    private boolean shouldTerminate;
    private ConnectionHandler<String> connectionHandler;


    public StompMessagingProtocolImpl() {
        this.shouldTerminate = false;
    }

    @Override
    public void start(int connectionId, Connections<String> connections, ConnectionHandler<String> connectionHandler) {
        this.connectionId = connectionId;
        this.connections = connections;
        this.connectionHandler = connectionHandler;
    }

    @Override
    public void process(String message) {
        //System.out.println(message);
        //connections.send(connectionId, message.toUpperCase());
        //message = "SEND\ndestination:/topic/a\nHello topic a\n\0";
        StompMessageDetails details = new StompMessageDetails(message);
        details.printMessageDetails();
        StompResponse response;
        switch (details.messageType)
        {
            case "CONNECT":
                Pair<Boolean, StompResponse> loginResponse= LoginHandler.login(
                        connectionId,details.version,details.host,details.username,details.password);
                if(loginResponse.getFirst())
                    connections.connect(connectionId,connectionHandler);
                if(loginResponse.getSecond().isError())
                    shouldTerminate=false;
                connectionHandler.send(loginResponse.getSecond().getResponseMessage());

            break;
            case "SEND":
                int messageId = messageIdCounter.incrementAndGet();
                Function<Integer,String> subIdToMessageFunc= (subId)->StompResponseHandler.createMessageResponse(subId,
                        messageId, details.destination, details.message).getResponseMessage();
                connections.send(details.destination,connectionId, subIdToMessageFunc);
                break;
            case "SUBSCRIBE":
                connections.subscribe(connectionId, details.destination, details.id);
                response= StompResponseHandler.createReceipt(details.receipt);
                connectionHandler.send(response.getResponseMessage());
                System.out.println(response.getResponseMessage());
                break;
            case "UNSUBSCRIBE":
                connections.unsubscribe(connectionId,details.id);
                response= StompResponseHandler.createReceipt(details.receipt);
                connectionHandler.send(response.getResponseMessage());
                break;
            case "DISCONNECT":
                response= LoginHandler.logout(connectionId,details.receipt);
                connections.disconnect(connectionId);
                connectionHandler.send(response.getResponseMessage());
                System.out.println(response.getResponseMessage());

            break;

        }

    }

    /**
     * @return true if the connection should be terminated
     */
    @Override
    public boolean shouldTerminate() {
        return shouldTerminate;
    }

    @Override
    public int getConnectionId() {
        return connectionId;
    }



}
