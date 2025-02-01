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

void handle_login(std::vector<std::string>& input_parameters, ConnectionHandler& connectionHandler, bool& loggedIn,
std::string& currentUser,bool& isError, std::string& errorBody,std::condition_variable& cv,std::mutex& m)
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
            if(!connectionHandler.sendFrameAscii(login_frame,'\0'))
            {
                std::cout<<"Could not connect to server"<<std::endl;
                return;
            }
            std::string response;
            std::unique_lock<std::mutex> lk(m);
            cv.wait(lk);
            if(!isError)
            {
                loggedIn = true;
                currentUser= username;
                std::cout<<"Login successful"<<std::endl;
            }
            else
            {
                std::cout<<"Error: "<< errorBody<<std::endl;
            }
               
}

void handle_join(std::vector<std::string>& input_parameters, ConnectionHandler& connectionHandler,
 std::map<std::string,int> &subscriptionToSubId,unsigned int& subscriptionId,std::map<std::string, bool>& subscribedChannels,unsigned int& receipt)
{
    if(input_parameters.size()!=2)
    {
        std::cout << "join command needs 1 arg: {channel_name}"<<std::endl;
        return;
    }
    std::string channel_name = input_parameters[1];
    // Check if already subscribed
    if(subscribedChannels.find(channel_name)!=subscribedChannels.end())
    {
        // Print error
        std::cout<< "Already subscribed to channel "<<channel_name<<std::endl;
        return;
    }

    subscriptionToSubId[channel_name]=subscriptionId;
    std::string join_message = StompMessageCreator::createSubscribeMessage(channel_name,subscriptionId++,receipt++);
    if(!connectionHandler.sendFrameAscii(join_message,'\0'))
    {
        std::cout<<"Issue connecting to the server"<<std::endl;
        return;
    }
    std::cout<<"Joined channel "<<channel_name<<std::endl;
    subscribedChannels[channel_name]=true;
}

void handle_exit(std::vector<std::string>& input_parameters, ConnectionHandler& connectionHandler,
 std::map<std::string,int> &subscriptionToSubId,std::map<std::string, bool>& subscribedChannels,unsigned int& receipt)
 {
        if(input_parameters.size()!=2)
        {
            std::cout << "exit command needs 1 arg: {channel_name}"<<std::endl;
            return;
        }
        std::string channel_name = input_parameters[1];
        if(subscribedChannels.find(channel_name)==subscribedChannels.end())
        {
            std::cout<<"you are not subscribed to channel "<<channel_name<<std::endl;
            return;
        }
        int subId = subscriptionToSubId[channel_name];
        std::string exit_message = StompMessageCreator::createUnsubscribeMessage(subId,receipt++);
        if(!connectionHandler.sendFrameAscii(exit_message,'\0'))
        {
            std::cout<<"Issue connecting to the server"<<std::endl;
            return;
        }
        std::cout<<"Exited channel "<<channel_name<<std::endl;
        subscribedChannels.erase(channel_name);

 }

void handle_report(std::vector<std::string>& input_parameters, ConnectionHandler& connectionHandler,std::map<std::string, bool>& subscribedChannels,
std::string& current_user,std::map<std::pair<std::string,std::string>,std::vector<Event>>& reports_by_user_and_channel)
{
                if(input_parameters.size()!=2)
            {
                std::cout << "report command needs 1 arg: {file}"<<std::endl;
            }
            std::string file_path = input_parameters[1];
            names_and_events result = parseEventsFile(file_path);
            if(subscribedChannels.find(result.channel_name)==subscribedChannels.end())
            {
                // Not subscribed to channel
                std::cout<<"You are not registered to channel "<<result.channel_name<<std::endl;
                return;
            }
            std::pair<std::string,std::string> key =std::pair<std::string,std::string>(current_user,result.channel_name);
            if(reports_by_user_and_channel.find(key)==reports_by_user_and_channel.end())
            {
                reports_by_user_and_channel[key]=std::vector<Event>();
            }
            for(size_t i=0;i<result.events.size();i++)
            {
                result.events[i].setEventOwnerUser(current_user);
                reports_by_user_and_channel[std::pair<std::string,std::string>(current_user,result.channel_name)].push_back(result.events[i]);
                std::string reportMessage = StompMessageCreator::createSendMessage(result.channel_name,result.events[i]);
                if(!connectionHandler.sendFrameAscii(reportMessage,'\0'))
                {
                    std::cout<<"Could not connect to server"<<std::endl;
                    //return;
                }
            }
            std::cout<< "reported"<<std::endl;
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

void handle_summary(std::vector<std::string>& input_parameters,std::map<std::pair<std::string,std::string>,std::vector<Event>>& reports_by_user_and_channel)
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
                if(e.get_general_information().at(active_str).compare("true")==0)
                    active_count++;
                if(e.get_general_information().at(forces_arrival_str).compare("true")==0)
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

void handle_logout(std::vector<std::string>& input_parameters, ConnectionHandler& connectionHandler, bool& loggedIn, unsigned int& receiptCount,std::map<std::string, bool>& subscribedChannels)
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
        subscribedChannels.clear();

}

