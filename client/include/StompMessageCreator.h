#pragma once

#include <string>
#include "../include/event.h"


class StompMessageCreator{
    public:
        static std::string createConnectMessage(std::string version, std::string host, std::string login, std::string passcode);
        static std::string createSendMessage(std::string destination,const Event& e);
        static std::string createSubscribeMessage(std::string destination, int id,int receipt);
        static std::string createUnsubscribeMessage(int id,int receipt);
        static std::string createDisconnectMessage(int receipt);
};