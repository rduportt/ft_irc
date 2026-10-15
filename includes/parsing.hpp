#ifndef PARSING_CPP
#define PARSING_CPP

#include <iostream>
#include <vector>

struct message
{
    std::string prefix;
    std::string command;
    std::vector<std::string> params; 
};

class Parser
{

    private :
        static std::string extractPrefix(std::string &line);
        static std::string extractCommand(std::string &line);
        static std::vector<std::string> excratParams(std::string &line);
    public : 
        static message line(const std::string &line);
};

#endif