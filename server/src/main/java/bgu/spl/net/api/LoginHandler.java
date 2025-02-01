package bgu.spl.net.api;

import utils.Pair;

import java.util.HashMap;

public class LoginHandler {
    // Key- username, Value - (password, isConnected)
    private static HashMap<String, Pair<String,Boolean>> users = new HashMap<>();
    private static HashMap<Integer,String>  clientIdToUsername= new HashMap<>();

    public static Pair<Boolean, StompResponse> login(int clientId,String version, String host, String user, String pass ) {
        if(isNewUser(user)) {
            // new user
            users.put(user,new Pair<>(pass,true));
            clientIdToUsername.put(clientId,user);
            return new Pair<>(true,StompResponseHandler.createConnectedSuccessfullyResponse(version));
        }
        // This is not a new user
        if(checkIsConnected(user))
        {
            // User is already logged in
            return new Pair<>(false,StompResponseHandler.createErrorResponse("User is already logged in",null,null));
        }
        if(isCorrectLoginInfo(user,pass))
        {
            // Login
            users.get(user).setSecond(true);
            clientIdToUsername.put(clientId,user);
            return new Pair<>(true,StompResponseHandler.createConnectedSuccessfullyResponse(version));
        }
        else
        {
            // Wrong Password
            return new Pair<>(false,StompResponseHandler.createErrorResponse("Wrong password",null,null));
        }
    }

    public static StompResponse logout(int connectionId,int receiptId) {
        disconnect(connectionId);
        return StompResponseHandler.createReceipt(receiptId);
    }

    private static boolean isNewUser(String user)
    {
        return !users.containsKey(user);
    }

    private static boolean checkIsConnected(String user)
    {
        return users.get(user).getSecond();
    }

    private static boolean isCorrectLoginInfo(String user, String pass)
    {
        return users.get(user).getFirst().equals(pass);
    }

    private static void disconnect(int clientId)
    {
        String user = clientIdToUsername.remove(clientId);
        users.get(user).setSecond(false);
    }
}
