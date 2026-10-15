#ifndef COMMAND_HPP
#define COMMAND_HPP


#include <string>
#include <sstream>
#include "client.hpp"
#include "channel.hpp"
#include "server.hpp"
#include "parsing.hpp"
#include <cstdlib>

class command
{
    public :  
        static void pass(Client *client, const message &msg, const std::string &serverPassword);
        static void kick(channel *chan ,Client *client, const message &msg, std::map<int, Client*> &clients);
        static void join(Client *client, const message &msg, std::map<std::string, channel*> &channels);
        static void nick(Client *client, const message &msg, std::map<int, Client*> &clients);
        static void invite(channel *chan, Client *client, const message &msg, std::map<int, Client*> &clients);
        static void user(Client *client, const message &msg);
        static void privmsg(Client *client, const message &msg, std::map<std::string, channel*> &channels, std::map<int, Client*> &clients);
        static void topic(channel *chan, Client *client, const message &msg);        
        static void mode(Client *client, const message &msg, std::map<std::string, channel*> &channels, std::map<int, Client*> &clients);
        static void quit(Client *client, const message &msg, std::map<std::string, channel*> &channels);
        static void ping(Client *client, const message &msg);
        static void part(Client *client, const message &msg, std::map<std::string, channel*> &channels);
        static std::string getChannelModes(channel *Channel);
};

std::string clientPrefix(Client *client);


#endif