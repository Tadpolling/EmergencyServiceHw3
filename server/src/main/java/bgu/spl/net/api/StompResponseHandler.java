package bgu.spl.net.api;

public class StompResponseHandler {
    public static StompResponse createConnectedSuccessfullyResponse(String version)
    {
        String response = "CONNECTED\n";
        response += "version:"+ version + "\n\n";
        response += "\u0000";
        return new StompResponse(response,false);
    }

    public static StompResponse createErrorResponse(String errorMessage,String extraInformation,String details)
    {
        String response = "ERROR\n";
        response += "message:"+ errorMessage + "\n\n";
        if(extraInformation != null)
            response+=extraInformation+"\n" ;
        if(details != null)
            response +="\n"+details+"\n";
        response += "\u0000";
        return new StompResponse(response,true);
    }

    public static StompResponse createReceipt(int receiptId)
    {
        String response = "RECEIPT\n";
        response+="receipt-id:"+receiptId+"\n\n";
        response += "\u0000";
        return new StompResponse(response,false);
    }

    public static StompResponse createMessageResponse(int subscriptionId,int messageId,String destination,String message)
    {
        String response = "MESSAGE\n";
        response += "subscription:"+subscriptionId+"\n";
        response += "message-id:"+messageId+"\n";
        response += "destination:"+destination+"\n\n";
        response += message+"\n";
        response += "\u0000";
        return new StompResponse(response,false);
    }
}
