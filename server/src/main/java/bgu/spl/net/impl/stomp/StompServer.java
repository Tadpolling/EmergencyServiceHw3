package bgu.spl.net.impl.stomp;

import bgu.spl.net.api.StompMessagingProtocolImpl;
import bgu.spl.net.impl.echo.LineMessageEncoderDecoder;
import bgu.spl.net.srv.Server;

public class StompServer {

    public static void main(String[] args) {
        // you can use any server...
        Server.threadPerClient(
                15000, //port
                ()->new StompMessagingProtocolImpl(), //protocol factory
                LineMessageEncoderDecoder::new //message encoder decoder factory
        ).serve();
    }
}
