#include "../include/utils.h"


std::vector<std::string> split(std::string& string, char delimiter)
{
    std::vector<std::string> strings= std::vector<std::string>();
    size_t index;
    while((index= string.find(delimiter)) != std::string::npos)
    {
        std::string string_part = string.substr(0,index);
        string = string.substr(index+1);
        strings.push_back(string_part);
    }
    strings.push_back(string);
    return strings;
}

void split_str(std::string& string, char delimiter, std::vector<std::string>& strings)
{
    size_t index;
    while((index= string.find(delimiter)) != std::string::npos)
    {
        std::string string_part = string.substr(0,index);
        string = string.substr(index+1);
        strings.push_back(string_part);
    }
    strings.push_back(string);
}

bool check_if_response_is_error(ConnectionHandler& connectionHandler,std::string& response_buff)
{
        // Get back an answer: by using the expected number of bytes (len bytes + newline delimiter)
        // We could also use: connectionHandler.getline(answer) and then get the answer without the newline char at the end
        if (connectionHandler.getFrameAscii(response_buff,'\0')) {
            // Have message to parse here
            int index=0;
            for(;response_buff[index]<'A'|| response_buff[index]>'Z';index++);
            response_buff=response_buff.substr(index);
            std::cout << "Reply: " << response_buff<< std::endl << std::endl;

            std::string response_type = response_buff.substr(0,response_buff.find('\n'));
            std::cout<<response_type<<std::endl;
            if(response_type.compare("ERROR")==0)
            {
                std::cout<<"ERROR response"<<std::endl;
                return true;
            }
            return false;
        }
    return true;
}

void handle_login(std::vector<std::string>& input_parameters, ConnectionHandler& connectionHandler, bool& loggedIn,std::string& currentUser)
{
if(input_parameters.size()!=4)
            {
                std::cout << "login command needs 3 args: {host:port} {username} {password}"<<std::endl;
                return;
            }
            if(loggedIn)
            {
                std::cout<< "The client is already logged in, log out before trying again" <<std::endl;
                return;
            }
            std::vector<std::string> host_port = split(input_parameters[1],':');
            // std::string host=host_port[0];
            // int port= atoi(host_port[1].c_str());
            std::string username = input_parameters[2];
            std::string password = input_parameters[3];            
            std::string login_frame= StompMessageCreator::createConnectMessage("1.2","stomp.cs.bgu.ac.il",username,password);
            std::cout<<"sending log in message " <<std::endl;
            if(!connectionHandler.sendFrameAscii(login_frame,'\0'))
            {
                std::cout<<"Could not connect to server"<<std::endl;
                return;
            }
            std::string response;
            if(!check_if_response_is_error(connectionHandler,response))
            {
                loggedIn = true;
                currentUser= username;
            }
               
}

void handle_join(std::vector<std::string>& input_parameters, ConnectionHandler& connectionHandler,
 std::map<std::string,int> &subscriptionToSubId,unsigned int& subscriptionId)
{
            if(input_parameters.size()!=2)
            {
                std::cout << "join command needs 1 arg: {channel_name}"<<std::endl;
                return;
            }
            std::string channel_name = input_parameters[1];
            subscriptionToSubId[channel_name]=subscriptionId;
            std::string join_message = StompMessageCreator::createSubscribeMessage(channel_name,subscriptionId++);
            if(!connectionHandler.sendFrameAscii(join_message,'\0'))
            {
                std::cout<<"Issue connecting to the server"<<std::endl;
                return;
            }
}

void handle_exit(std::vector<std::string>& input_parameters, ConnectionHandler& connectionHandler,
 std::map<std::string,int> &subscriptionToSubId)
 {
            if(input_parameters.size()!=2)
            {
                std::cout << "exit command needs 1 arg: {channel_name}"<<std::endl;
                return;
            }
            std::string channel_name = input_parameters[1];
            int subId = subscriptionToSubId[channel_name];
            std::string exit_message = StompMessageCreator::createUnsubscribeMessage(subId);
            if(!connectionHandler.sendFrameAscii(exit_message,'\0'))
            {
                std::cout<<"Issue connecting to the server"<<std::endl;
                return;
            }
 }

void handle_logout(std::vector<std::string>& input_parameters, ConnectionHandler& connectionHandler, bool& loggedIn, unsigned int& receiptCount)
{
        if(input_parameters.size()!=1)
        {
            std::cout << "logout command needs 0 args"<<std::endl;
            return;
        }

        std::string logout_message = StompMessageCreator::createDisconnectMessage(receiptCount++);
        if(!connectionHandler.sendFrameAscii(logout_message,'\0'))
        {
            std::cout<<"Issue connecting to the server"<<std::endl;
            return;
        }
        loggedIn=false;
}

