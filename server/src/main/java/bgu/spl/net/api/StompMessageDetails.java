package bgu.spl.net.api;

public class StompMessageDetails {
    public String messageType;
    public String destination;
    public int id;
    public int receipt;
    public String message;

    // fields for connecting
    public String version;
    public String host;
    public String username;
    public String password;
    private boolean isConnectingMessage;
    public StompMessageDetails(String fullMessage) {
        String[] parts = fullMessage.split("\n");
        System.out.println(fullMessage);
        for (String part : parts) {
            System.out.println(part+", ");
        }
        messageType = parts[0].trim();
        if (messageType.equals("CONNECT")) {
            isConnectingMessage = true;
        }
        for(int i=1; i<parts.length; i++) {
            String[] lineHalfs = parts[i].trim().split(":");
            if(lineHalfs[0].equals("destination")) {
                destination = lineHalfs[1].trim();
                if(destination.charAt(0)=='/')
                    destination = destination.substring(1);
                continue;
            }
            if(lineHalfs[0].equals("id")) {
                id = Integer.parseInt(lineHalfs[1].trim());
                continue;
            }
            if(lineHalfs[0].equals("receipt")) {
                receipt = Integer.parseInt(lineHalfs[1].trim());
                continue;
            }
            if(isConnectingMessage) {

                if(lineHalfs[0].equals("accept-version")) {
                    version = lineHalfs[1].trim();
                    continue;
                }
                if(lineHalfs[0].equals("host")) {
                    host = lineHalfs[1].trim();
                    continue;
                }
                if(lineHalfs[0].equals("login")) {
                    username = lineHalfs[1].trim();
                    continue;
                }
                if(lineHalfs[0].equals("passcode")) {
                    password = lineHalfs[1].trim();
                    continue;
                }

            }
        }
        message = parts[parts.length-2].trim();
    }

    public void printMessageDetails()
    {
        System.out.println("Printing Message Details:");
        System.out.println(messageType);
        System.out.println(destination);
        System.out.println(id);
        System.out.println(receipt);

        System.out.println(version);
        System.out.println(host);
        System.out.println(username);
        System.out.println(password);
        System.out.println(message);
    }

}
