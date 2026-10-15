#include "../includes/parsing.hpp"

message Parser::line(const std::string &line)
{
    message msg;
    std::string raw = line;
    if (!raw.empty() && raw[raw.size() - 1] == '\r')
        raw.erase(line.size() - 1);
    if (!raw.empty() && raw[raw.size() - 1] == '\n')
        raw.erase(line.size() - 1);

    if (line.empty())
        return msg;
    msg.prefix = extractPrefix(raw);
    msg.command = extractCommand(raw);
    msg.params = excratParams(raw);

    return msg;
}

std::string Parser::extractPrefix(std::string &line)
{
    std::string prefix;

    if (line.empty() || line[0] != ':')
        return "";    

    size_t spacePos = line.find(' ');
    if (spacePos == std::string::npos)
    {
        prefix = line.substr(1);
        line.clear();
        return prefix;
    }
    prefix = line.substr(1, spacePos - 1);
    line = line.substr(spacePos + 1);
    return prefix;
}

std::string Parser::extractCommand(std::string &line)
{
    std::string command;
    size_t spacePos = line.find(' ');
    if (spacePos == std::string::npos)
    {
        command = line;
        line.clear();
        return command;
    }
    command = line.substr(0, spacePos);
    line = line.substr(spacePos + 1);
    return command;
}

std::vector<std::string> Parser::excratParams(std::string &line)
{
    std::vector<std::string> params;
    while (!line.empty())
    {
        if (line[0] == ':')
        {
            params.push_back(line.substr(1));
            break;
        }
        size_t spacePos = line.find(' ');
        if(spacePos == std::string::npos)
        {
            params.push_back(line);
            break;
        }
        else 
        {
            params.push_back(line.substr(0, spacePos));
            line = line.substr(spacePos + 1);
        }
    }
    return params;
}