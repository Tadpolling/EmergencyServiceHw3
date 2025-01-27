package bgu.spl.net.api;

public class StompResponse {
    private String response;
    private boolean isError;
    StompResponse(String response, boolean isError) {
        this.response = response;
        this.isError = isError;
    }
    public String getResponseMessage() {
        return response;
    }
    public boolean isError() {
        return isError;
    }
}
