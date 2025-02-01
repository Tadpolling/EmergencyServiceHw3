package bgu.spl.net.impl.stomp;

import bgu.spl.net.api.StompMessagingProtocolImpl;
import bgu.spl.net.impl.echo.LineMessageEncoderDecoder;
import bgu.spl.net.srv.Server;

public class StompServer {

    public static void main(String[] args) {

        int port = Integer.parseInt(args[0]);
        String serverType = args[1];
        switch (serverType) {
            case "tpc":
                Server.threadPerClient(
                port, //port
                ()->new StompMessagingProtocolImpl(), //protocol factory
                LineMessageEncoderDecoder::new //message encoder decoder factory
            ).serve();
                break;
            case "reactor":
                Server.reactor(5,
                        port,
                        ()->new StompMessagingProtocolImpl(),
                        LineMessageEncoderDecoder::new
                ).serve();
                break;
        }


    }
}
