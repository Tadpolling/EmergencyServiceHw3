package bgu.spl.net.api;

public class StompResponseHandler {
    public static StompResponse createConnectedSuccessfullyResponse(String version)
    {
        String response = "CONNECTED\n";
        response += "version:"+ version + "\n";
        response += "\u0000";
        return new StompResponse(response,false);
    }

    public static StompResponse createErrorResponse(String errorMessage,String extraInformation,String details)
    {
        String response = "ERROR\n";
        response += "message:"+ errorMessage + "\n";
        if(extraInformation != null)
            response+=extraInformation+"\n";
        if(details != null)
            response +=details+"\n";
        response += "\u0000";
        return new StompResponse(response,true);
    }

    public static StompResponse createReceipt(int receiptId)
    {
        String response = "RECEIPT\n";
        response+="receipt-id:"+receiptId+"\n";
        response += "\u0000";
        return new StompResponse(response,false);
    }

}
