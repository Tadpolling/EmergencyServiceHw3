package bgu.spl.net.api;
import bgu.spl.net.api.StompMessageDetails;
import bgu.spl.net.srv.ConnectionHandler;
import bgu.spl.net.srv.Connections;
import utils.Pair;


public class StompMessagingProtocolImpl implements StompMessagingProtocol<String> {

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

        switch (details.messageType)
        {
            case "CONNECT":
                Pair<Boolean, StompResponse> loginResponse= LoginHandler.login(
                        connectionId,details.version,details.host,details.username,details.password);
                if(loginResponse.getFirst())
                    connections.connect(connectionId,connectionHandler);
                if(loginResponse.getSecond().isError())
                    shouldTerminate=true;
                connectionHandler.send(loginResponse.getSecond().getResponseMessage());

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
                System.out.println("Disconnecting");
                StompResponse response= LoginHandler.logout(connectionId,details.id);
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
