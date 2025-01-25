package bgu.spl.net.impl.stomp;

public class StompMessageDetails {
    String messageType;
    String destination;
    int id;
    int receipt;
    String message;
    public StompMessageDetails(String fullMessage) {
        String[] parts = fullMessage.split("\n");
        System.out.println(fullMessage);
        for (String part : parts) {
            System.out.println(part+", ");
        }
        messageType = parts[0].trim();
        for(int i=1; i<parts.length-2; i++) {
            String[] lineHalfs = parts[i].trim().split(":");
            if(lineHalfs[0].equals("destination")) {
                destination = lineHalfs[1];
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
        System.out.println(message);
    }

}
