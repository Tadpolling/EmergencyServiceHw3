#include "../include/StompMessageCreator.h"

std::string StompMessageCreator::createConnectMessage(std::string version, std::string host, std::string login, std::string passcode)
{
    std::string message = "CONNECT\n";
    message+="accept-version:"+version+"\n";
    message+="host:"+host+"\n";
    message+="login:"+login+"\n";
    message+="passcode:"+passcode+"\n";
    message+="\n";
    return message;
}

std::string StompMessageCreator::createSendMessage(std::string destination, std::string messageBody)
{
    std::string message = "SEND\n";
    message+="destination:"+destination+"\n";
    message+="\n"+messageBody+"\n";
    return message;
}

std::string StompMessageCreator::createSubscribeMessage(std::string destination, int id)
{
    std::string message = "SUBSCRIBE\n";
    message+="destination:"+destination+"\n";
    message+="id:"+std::to_string(id)+"\n";
    message+="\n";
    return message;
}

std::string StompMessageCreator::createUnsubscribeMessage(int id)
{
    std::string message = "UNSUBSCRIBE\n";
    message+="id:"+std::to_string(id)+"\n";
    message+="\n";
    return message;
}

std::string StompMessageCreator::createDisconnectMessage(int receipt)
{
    std::string message = "DISCONNECT\n";
    message+="receipt:"+std::to_string(receipt)+"\n";
    message+="\n";
    return message;
}
