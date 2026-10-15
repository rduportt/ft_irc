#include "../../includes/command.hpp"

void command::part(Client *client, const message &msg, std::map<std::string, channel*> &channels)
{
    if (!client->is_registered())
    {
        client->appendToSendBuffer(":server 451: You have not registered\r\n");
        return;
    }
    if (msg.params.empty())
    {
        client->appendToSendBuffer(":server 461: " + client->getNickname() + " PART: Not enough parameters\r\n");
        return;
    }

    std::string chanName = msg.params[0];
    std::string reason   = msg.params.size() > 1 ? msg.params[1] : "Leaving";
    if (channels.find(chanName) == channels.end())
    {
        client->appendToSendBuffer(":server 403 " + client->getNickname() + " " + chanName + " :No such channel\r\n");
        return;
    }
    channel *Channel = channels[chanName];
    if (!Channel->isMember(client->getFd()))
    {
        client->appendToSendBuffer(":server 442 " + client->getNickname() + " " + chanName + " :You're not on that channel\r\n");
        return;
    }
    std::string PartMsg = clientPrefix(client) + " PART " + chanName + " " + reason + "\r\n";
    Channel->broadcast(PartMsg, -1);
    Channel->removeMember(client->getFd());

    if (Channel->getMemberCount() == 0)
    {
        delete Channel;
        channels.erase(chanName);
    }
}
