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

std::string convertBoolToString(bool b)
{
    return b? "true":"false";
}

std::string StompMessageCreator::createSendMessage(std::string destination,const Event& e)
{
    std::string message = "SEND\n";
    message+="destination:"+destination+"\n\n";
    // Body starts here
    message+="user: "+e.getEventOwnerUser()+"\n";
    message+="city: "+e.get_city()+"\n";
    message+="event name: "+e.get_name()+"\n";
    message+="date time: "+std::to_string(e.get_date_time())+"\n";
    message+="general information:\n";
    message+="\tactive: "+convertBoolToString(e.get_general_information().at("active").compare("true")==0)+"\n";
    message+="\tforces_arrival_at_scene: "+convertBoolToString(e.get_general_information()
    .at("forces_arrival_at_scene").compare("true")==0)+"\n";
    message+="description:\n";
    message+=e.get_description()+"\n";
    return message;
}

std::string StompMessageCreator::createSubscribeMessage(std::string destination, int id,int receipt)
{
    std::string message = "SUBSCRIBE\n";
    message+="destination:"+destination+"\n";
    message+="id:"+std::to_string(id)+"\n";
    message+="receipt:"+std::to_string(receipt)+"\n";
    message+="\n";
    return message;
}

std::string StompMessageCreator::createUnsubscribeMessage(int id,int receipt)
{
    std::string message = "UNSUBSCRIBE\n";
    message+="id:"+std::to_string(id)+"\n";
    message+="receipt:"+std::to_string(receipt)+"\n";
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
