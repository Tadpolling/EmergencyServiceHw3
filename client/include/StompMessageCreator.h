#pragma once

#include <string>

class StompMessageCreator{
    public:
        static std::string createConnectMessage(std::string version, std::string host, std::string login, std::string passcode);
        static std::string createSendMessage(std::string destination, std::string messageBody);
        static std::string createSubscribeMessage(std::string destination, int id);
        static std::string createUnsubscribeMessage(int id);
        static std::string createDisconnectMessage(int receipt);
};