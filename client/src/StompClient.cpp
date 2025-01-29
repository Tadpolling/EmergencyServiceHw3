#include <stdlib.h>
#include <vector>
#include "../include/ConnectionHandler.h"

static std::vector<std::string> split(std::string& string, char delimiter);
/**
* This code assumes that the server replies the exact text the client sent it (as opposed to the practical session example)
*/
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
	//From here we will see the rest of the ehco client implementation:
    while (1) {
        const short bufsize = 1024;
        char buf[bufsize];
        std::cin.getline(buf, bufsize);
		std::string line(buf);
		int len=line.length();
        std::cout<<line<<std::endl;
        std::vector<std::string> input_parameters = split(line,' ');

        std::string command = input_parameters[0];
        if(command.compare("login")==0)
        {
            if(input_parameters.size()!=4)
            {
                std::cout << "login command needs 3 args: {host:port} {username} {password}"<<std::endl;
            }
            loggedIn = true;
            std::vector<std::string> host_port = split(input_parameters[1],':');
            std::string host=host_port[0];
            int port= atoi(host_port[1].c_str());
            std::string username = input_parameters[2];
            std::string password = input_parameters[3];
        }
        if(!loggedIn)
        {
            std::cout << "please login first" <<std::endl;
        }
        if(command.compare("join")==0)
        {
            if(input_parameters.size()!=2)
            {
                std::cout << "join command needs 1 arg: {channel_name}"<<std::endl;
            }
            std::string channel_name = input_parameters[1];
        }
        if(command.compare("exit")==0)
        {
            if(input_parameters.size()!=2)
            {
                std::cout << "exit command needs 1 arg: {channel_name}"<<std::endl;
            }
            std::string channel_name = input_parameters[1];
        }
        if(command.compare("report")==0)
        {
            if(input_parameters.size()!=2)
            {
                std::cout << "report command needs 1 arg: {file}"<<std::endl;
            }
            std::string file_path = input_parameters[1];
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
        }
        if(command.compare("logout")==0)
        {
            if(input_parameters.size()!=1)
            {
                std::cout << "logout command needs 0 args"<<std::endl;
            }
            loggedIn=false;
        }
        if (!connectionHandler.sendLine(line)) {
            std::cout << "Disconnected. Exiting...\n" << std::endl;
            break;
        }
		// connectionHandler.sendLine(line) appends '\n' to the message. Therefor we send len+1 bytes.
        std::cout << "Sent " << len+1 << " bytes to server" << std::endl;

 
        // We can use one of three options to read data from the server:
        // 1. Read a fixed number of characters
        // 2. Read a line (up to the newline character using the getline() buffered reader
        // 3. Read up to the null character
        std::string answer;
        // Get back an answer: by using the expected number of bytes (len bytes + newline delimiter)
        // We could also use: connectionHandler.getline(answer) and then get the answer without the newline char at the end
        if (!connectionHandler.getLine(answer)) {
            std::cout << "Disconnected. Exiting...\n" << std::endl;
            break;
        }
        
		len=answer.length();
		// A C string must end with a 0 char delimiter.  When we filled the answer buffer from the socket
		// we filled up to the \n char - we must make sure now that a 0 char is also present. So we truncate last character.
        answer.resize(len-1);
        std::cout << "Reply: " << answer << " " << len << " bytes " << std::endl << std::endl;
        if (answer == "bye") {
            std::cout << "Exiting...\n" << std::endl;
            break;
        }
    }
    return 0;
}

static std::vector<std::string> split(std::string& string, char delimiter)
{
    std::vector<std::string> strings= std::vector<std::string>();
    size_t index;
    while((index= string.find(delimiter)) != std::string::npos)
    {
        std::string string_part = string.substr(0,index);
        string = string.substr(index+1);
        strings.push_back(string_part);
    }
    return strings;
}