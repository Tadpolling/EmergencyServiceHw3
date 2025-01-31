#pragma once

#include <stdlib.h>
#include <vector>
#include <map>
#include "../include/ConnectionHandler.h"
#include "../include/StompMessageCreator.h"


std::vector<std::string> split(std::string& string, char delimiter);

void split_str(std::string& string, char delimiter, std::vector<std::string>& strings);

bool check_if_response_is_error(ConnectionHandler& connectionHandler,std::string& response_buff);

void handle_login(std::vector<std::string>& input_parameters, ConnectionHandler& connectionHandler, bool& loggedIn,std::string& currentUser);

void handle_join(std::vector<std::string>& input_parameters, ConnectionHandler& connectionHandler,
 std::map<std::string,int> &subscriptionToSubId,unsigned int& subscriptionId);

void handle_exit(std::vector<std::string>& input_parameters, ConnectionHandler& connectionHandler,
 std::map<std::string,int> &subscriptionToSubId);

void handle_logout(std::vector<std::string>& input_parameters, ConnectionHandler& connectionHandler, bool& loggedIn, unsigned int& receiptCount);

