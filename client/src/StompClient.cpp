#include <stdlib.h>
#include <vector>
#include <map>
#include <string>
#include <fstream>
#include <ctime>
#include "../include/ConnectionHandler.h"
#include "../include/event.h"
#include "../include/utils.h"
/**
* This code assumes that the server replies the exact text the client sent it (as opposed to the practical session example)
*/

std::string formatNumber(int number);
bool lessThanEventComparator(const Event& e1, const Event& e2);

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
    bool wait_response=true;
    std::map<std::string, int> subscriptionToSubId = std::map<std::string,int>();
    std::string current_user="";
    // Key- User,ChannelName, Value- list of reports
    std::map<std::pair<std::string,std::string>,std::vector<Event>> reports_by_user_and_channel
     = std::map<std::pair<std::string,std::string>,std::vector<Event>>();
	//From here we will see the rest of the ehco client implementation:
    while (1) {
        const short bufsize = 1024;
        char buf[bufsize];
        std::cin.getline(buf, bufsize);
		std::string line(buf);
        std::vector<std::string> input_parameters = split(line,' ');
        wait_response=true;
        // for(int i=0;i<input_parameters.size();i++)
        // {
        //     std::cout<<input_parameters[i]<<std::endl;
        // }
        std::string command = input_parameters[0];
        if(command.compare("login")==0)
        {
            std::cout<<"Starting login as: " <<loggedIn <<std::endl;
            handle_login(input_parameters,connectionHandler,loggedIn,current_user);
            wait_response=false;
        }
        if(!loggedIn)
        {
            std::cout << "please login first" <<std::endl;
            wait_response=false;
        }
        if(command.compare("join")==0)
        {
            handle_join(input_parameters,connectionHandler,subscriptionToSubId,subscriptionId);
        }
        if(command.compare("exit")==0)
        {
            handle_exit(input_parameters,connectionHandler,subscriptionToSubId);
        }
        if(command.compare("report")==0)
        {
            if(input_parameters.size()!=2)
            {
                std::cout << "report command needs 1 arg: {file}"<<std::endl;
            }


            std::string file_path = input_parameters[1];
            names_and_events result = parseEventsFile(file_path);
            std::pair<std::string,std::string> key =std::pair<std::string,std::string>(current_user,result.channel_name);
            if(reports_by_user_and_channel.find(key)==reports_by_user_and_channel.end())
            {
                reports_by_user_and_channel[key]=std::vector<Event>();
            }
            for(size_t i=0;i<result.events.size();i++)
            {
                reports_by_user_and_channel[std::pair<std::string,std::string>(current_user,result.channel_name)].push_back(result.events[i]);
                std::string reportMessage = StompMessageCreator::createSendMessage(result.channel_name,result.events[i].get_description());
                if(!connectionHandler.sendFrameAscii(reportMessage,'\0'))
                {
                    std::cout<<"Could not connect to server"<<std::endl;
                    //return;
                }
            }


        }
        if(command.compare("summary")==0)
        {
            if(input_parameters.size()!=4)
            {
                std::cout << "summary command needs 3 args: {channel_name} {user} {file}"<<std::endl;
            }
            std::string channel_name = input_parameters[1];
            std::string user = input_parameters[2];
            std::string file_path = input_parameters[3];
            std::pair<std::string,std::string> currentKey =std::pair<std::string,std::string>(user,channel_name);
            std::ofstream file;
            file.open(file_path,std::fstream::trunc);
            int active_count =0;
            int forces_arrival_at_scene_count =0;
            std::string active_str ="active";
            std::string forces_arrival_str = "forces_arrival_at_scene";
            for(Event e : reports_by_user_and_channel[currentKey])
            {
   
                if(e.get_general_information().at(active_str).compare("true"))
                    active_count++;
                if(e.get_general_information().at(forces_arrival_str).compare("true"))
                    forces_arrival_at_scene_count++;
            }
            file<<"Channel "<<channel_name<<std::endl;
            file<<"Stats:"<<std::endl;
            file<<"Total: "<<reports_by_user_and_channel[std::pair<std::string,std::string>(user,channel_name)].size()<<std::endl;
            file<<"active: " <<active_count<< std::endl;
            file<<"forces arrival at scene: " <<forces_arrival_at_scene_count<<std::endl;
            file<< std::endl;
            file<<"Event Reports:"<<std::endl;
            file<<std::endl;
            std::sort(reports_by_user_and_channel[currentKey].begin(),reports_by_user_and_channel[currentKey].end(),lessThanEventComparator);
            for(size_t i=0;i<reports_by_user_and_channel[currentKey].size();i++)
            {
                Event& e = reports_by_user_and_channel[currentKey][i];
                file<< "Report_"<<i+1<<":"<<std::endl;
                file<<"\tcity: "<<e.get_city()<<std::endl;
                time_t time = e.get_date_time()+320*60;
                tm* timeinfo = localtime(&time);

                file<<"\tdate time: "<<formatNumber(timeinfo->tm_mday)<<"/"<<formatNumber(timeinfo->tm_mon+1)<<"/"<<formatNumber((timeinfo->tm_year+1900)%100) <<" "<<
                formatNumber(timeinfo->tm_hour) <<":"<<formatNumber(timeinfo->tm_min)<<std::endl;
                file<<"\tevent name: "<<e.get_name()<<std::endl;
                std::string summary_string = e.get_description().size() <=27 ? e.get_description() : e.get_description().substr(0,27)+"...";
                file<<"\tsummary: "<<summary_string<<std::endl; 
            }
            file.close();

        }
        if(command.compare("logout")==0)
        {
            handle_logout(input_parameters,connectionHandler,loggedIn,receiptCount);
        }
        if(wait_response)
        {
            std::string response;
            check_if_response_is_error(connectionHandler,response);
        }

    }
    return 0;
}

std::string formatNumber(int number)
{
   return ((number <10)? "0"+std::to_string(number): std::to_string(number));
}

bool lessThanEventComparator(const Event& e1, const Event& e2)
{
    if(e1.get_date_time()<e2.get_date_time())
        return true;
    if(e1.get_date_time()>e2.get_date_time())
        return false;
    return e1.get_name().compare(e2.get_name());
}