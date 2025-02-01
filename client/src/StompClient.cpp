#include <stdlib.h>
#include <vector>
#include <map>
#include <string>
#include <fstream>
#include <ctime>
#include <thread>
#include <mutex>
#include <condition_variable>
#include "../include/ConnectionHandler.h"
#include "../include/event.h"
#include "../include/utils.h"

struct SocketHandlerInput{
    ConnectionHandler& connectionHandler;
    std::map<std::pair<std::string,std::string>,std::vector<Event>>& reports_by_user_and_channel;
    bool& isError;
    std::string& body;
    std::mutex& m;
    std::condition_variable& cv;
};

void socketHandler(SocketHandlerInput socketInput);

int main (int argc, char *argv[]) {
    if (argc < 3) {
        std::cerr << "Usage: " << argv[0] << " host port" << std::endl << std::endl;
        return -1;
    }
    std::string host = argv[1];
    short port = atoi(argv[2]);
    
    ConnectionHandler connectionHandler(host, port);
    if (!connectionHandler.connect()) {
        std::cerr << "Cannot connect to " << host << ":" << port << std::endl;
        return 1;
    }

	bool loggedIn = false;
    unsigned int subscriptionId = 0;
    unsigned int receiptCount=0;
    std::map<std::string, int> subscriptionToSubId = std::map<std::string,int>();
    // Key- Channel name, value- place holder bool
    std::map<std::string, bool> subscribedChannels = std::map<std::string,bool>();
    std::string current_user="";
    // Key- User,ChannelName, Value- list of reports
    std::map<std::pair<std::string,std::string>,std::vector<Event>> reports_by_user_and_channel
     = std::map<std::pair<std::string,std::string>,std::vector<Event>>();
    bool isError=false;
    std::string body="";
    std::mutex m;
    std::condition_variable cv;
    

    SocketHandlerInput socketInput {connectionHandler,reports_by_user_and_channel,isError,body,m,cv};
    std::thread socketThread(socketHandler,socketInput);

    while (1) {
        const short bufsize = 1024;
        char buf[bufsize];
        std::cin.getline(buf, bufsize);
		std::string line(buf);
        std::vector<std::string> input_parameters = split(line,' ');
        std::string command = input_parameters[0];
        socketInput.isError=false;
        socketInput.body="";
        if(command.compare("login")==0)
        {
            handle_login(input_parameters,connectionHandler,loggedIn,current_user,socketInput.isError,socketInput.body,cv,m);
            continue;
        }
        if(!loggedIn && !socketInput.isError)
        {
            std::cout << "please login first" <<std::endl;
            continue;
        }
        if(command.compare("join")==0)
        {
            handle_join(input_parameters,connectionHandler,subscriptionToSubId,subscriptionId,subscribedChannels,receiptCount);
            continue;
        }
        if(command.compare("exit")==0)
        {
            handle_exit(input_parameters,connectionHandler,subscriptionToSubId,subscribedChannels,receiptCount);
            continue;
        }
        if(command.compare("report")==0)
        {
            handle_report(input_parameters,connectionHandler,subscribedChannels,current_user,reports_by_user_and_channel);
            continue;
        }
        if(command.compare("summary")==0)
        {
            handle_summary(input_parameters,reports_by_user_and_channel);
            continue;
        }
        if(command.compare("logout")==0)
        {
            handle_logout(input_parameters,connectionHandler,loggedIn,receiptCount,subscribedChannels);
            continue;
        }
    }
    return 0;
}

void socketHandler(SocketHandlerInput socketInput)
{
        std::string response_str="";
        while(1)
        {
            if (socketInput.connectionHandler.getFrameAscii(response_str,'\0')) {
                // Have message to parse here
                int index=0;
                for(;response_str[index]<'A'|| response_str[index]>'Z';index++);
                response_str=response_str.substr(index);

                // Seperating headers and body
                std::string response_type = response_str.substr(0,response_str.find('\n'));
                int bodyIndex = response_str.find("\n\n");
                std::string headers = response_str.substr(0,bodyIndex);
                std::string body = bodyIndex< (int)response_str.size() ? response_str.substr(bodyIndex): "";
                std::vector<std::string> headers_seperated = split(headers,'\n');
                std::vector<std::pair<std::string,std::string>> headersKeyValuePairs = std::vector<std::pair<std::string,std::string>>();
                // Need to remove message type from list
                headers_seperated.erase(headers_seperated.begin());
                // Parsing headers
                for(std::string line: headers_seperated)
                {
                    std::vector<std::string> pairs = split(line,':');
                    headersKeyValuePairs.push_back(std::pair<std::string,std::string>(pairs[0],pairs[1]));
                }

                if(response_type.compare("ERROR")==0)
                {
                    socketInput.isError=true;
                }
                if(response_type.compare("MESSAGE")==0)
                {
                    // finding channel name
                    std::string channelName="";
                    for(auto keyValuePair: headersKeyValuePairs)
                    {
                        if(keyValuePair.first.compare("destination")==0)
                        {
                            channelName = keyValuePair.second;
                            break;
                        }
                    }
                    Event e = Event(body);
                    e.setChannelName(channelName);
                    std::pair<std::string,std::string> key= std::pair<std::string,std::string>(e.getEventOwnerUser(),e.get_channel_name());
                    if(socketInput.reports_by_user_and_channel.find(key)==socketInput.reports_by_user_and_channel.end())
                    {
                        socketInput.reports_by_user_and_channel[key]=std::vector<Event>();
                    }
                    socketInput.reports_by_user_and_channel[key].push_back(e);
                }
                else
                {
                    // Not MESSAGE type
                    socketInput.body=body;
                    if(response_type.compare("ERROR")==0)
                    {
                        // finding error message
                        std::string errorMessage="";
                        for(auto keyValuePair: headersKeyValuePairs)
                        {
                            if(keyValuePair.first.compare("message")==0)
                            {
                                errorMessage = keyValuePair.second;
                                break;
                            }
                        }
                        socketInput.body=errorMessage;
                    }
                    socketInput.cv.notify_all();
                }
                response_str="";
            }
        }
}