#pragma once

#include <stdlib.h>
#include <vector>
#include <map>
#include <condition_variable>
#include <mutex>
#include "../include/ConnectionHandler.h"
#include "../include/StompMessageCreator.h"


std::vector<std::string> split(std::string& string, char delimiter);

void handle_login(std::vector<std::string>& input_parameters, ConnectionHandler& connectionHandler, bool& loggedIn,std::string& currentUser,
bool& isError, std::string& errorBody,std::condition_variable& cv,std::mutex& m);

void handle_join(std::vector<std::string>& input_parameters, ConnectionHandler& connectionHandler,
 std::map<std::string,int> &subscriptionToSubId,unsigned int& subscriptionId,std::map<std::string, bool>& subscribedChannels,unsigned int& receipt);

void handle_exit(std::vector<std::string>& input_parameters, ConnectionHandler& connectionHandler,
 std::map<std::string,int> &subscriptionToSubId,std::map<std::string, bool>& subscribedChannels,unsigned int& receipt);

void handle_report(std::vector<std::string>& input_parameters, ConnectionHandler& connectionHandler,std::map<std::string, bool>& subscribedChannels,
std::string& current_user,std::map<std::pair<std::string,std::string>,std::vector<Event>>& reports_by_user_and_channel);

void handle_summary(std::vector<std::string>& input_parameters,std::map<std::pair<std::string,std::string>,std::vector<Event>>& reports_by_user_and_channel);

void handle_logout(std::vector<std::string>& input_parameters, ConnectionHandler& connectionHandler, bool& loggedIn, unsigned int& receiptCount,std::map<std::string, bool>& subscribedChannels);

