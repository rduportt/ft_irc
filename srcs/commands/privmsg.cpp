#include "../../includes/command.hpp"

void command::privmsg(Client *client, const message &msg, std::map<std::string, channel*> &channels, std::map<int, Client*> &clients)
{
    if (!client->is_registered())
    {
        client->appendToSendBuffer(":server 451: You have not registered\r\n");
        return;
    }
    if (msg.params.size() < 2)
    {
        client->appendToSendBuffer(":server 461: " + client->getNickname() + " PRIVMSG: Not enough parameters\r\n");
        return;
    }
    std::string target = msg.params[0];
    std::string content   = msg.params[1];

    if (content.empty())
    {
        client->appendToSendBuffer(":server 412: " + client->getNickname() + " :No text to send\r\n");   
        return;
    }

    std::string fullMsg = clientPrefix(client) + " PRIVMSG " + target + " :" + content + "\r\n";

    if (target[0] == '#')
    {
        if (channels.find(target) == channels.end())
        {
            client->appendToSendBuffer(":server 403 " + client->getNickname() + " " + target + " :No such channel\r\n");
            return;
        }
        channel *Channel = channels[target];
        if (!Channel->isMember(client->getFd()))
        {
            client->appendToSendBuffer(":server 404 " + client->getNickname() + " " + target + " :Cannot send to channel\r\n");
            return;
        }
        Channel->broadcast(fullMsg, client->getFd());
        return;
    }
    Client *dest = NULL;
    std::map<int, Client*>::const_iterator it = clients.begin();
    while (it != clients.end())
    {
        if (it->second->getNickname() == target)
        {
            dest = it->second;
            break;
        }
        it++;
    }
    if (!dest)
    {
        client->appendToSendBuffer(":server 401 " + client->getNickname() + " " + target + " :No such nick\r\n");
        return;
    }
    dest->appendToSendBuffer(fullMsg);
}