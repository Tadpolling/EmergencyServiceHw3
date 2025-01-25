package bgu.spl.net.impl.stomp;

import bgu.spl.net.api.StompMessagingProtocol;
import bgu.spl.net.srv.ConnectionHandler;
import bgu.spl.net.srv.Connections;

public class StompMesssagingProtocolImpl implements StompMessagingProtocol<String> {

    private int connectionId;
    private Connections<String> connections;
    private boolean shouldTerminate;
    private ConnectionHandler<String> connectionHandler;

    StompMesssagingProtocolImpl() {
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

        switch (details.messageType)
        {
            case "CONNECT":
                connections.connect(connectionId,connectionHandler);
            break;
            case "SEND":
                connections.send(details.destination,details.message);
                break;
            case "SUBSCRIBE":
                connections.subscribe(connectionId, details.destination, details.id);
                break;
            case "UNSUBSCRIBE":
                connections.unsubscribe(connectionId,details.id);
                break;
            case "DISCONNECT":
                connections.disconnect(connectionId);
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
