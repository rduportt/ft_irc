#include "../../includes/command.hpp"

void command::join(Client *client, const message &msg, std::map<std::string, channel*> &channels)
{
    if (!client->is_registered())
    {
        client->appendToSendBuffer(":server 451 You have not registered\r\n");
        return;
    }
    if (msg.params.empty())
    {
        client->appendToSendBuffer(":server 461 " + client->getNickname() + " JOIN: Not enough parameters\r\n");
        return;
    }
    std::string key;
    std::string chanName = msg.params[0];
    if(msg.params.size() >= 2)
        key = msg.params[1];
    else
        key = "";
    

    if (chanName.empty() || chanName[0] != '#')
    {
        client->appendToSendBuffer(":server 403 " + client->getNickname() + " " + chanName + " :No such channel\r\n");
        return;
    }

if (channels.find(chanName) == channels.end())
{
    channels[chanName] = new channel(chanName);
    if (!key.empty())
        channels[chanName]->setKey(key);
}

    channel *Channel = channels[chanName];
    if (Channel->isMember(client->getFd()))
        return;
    
    if (Channel->isInviteOnly() && !Channel->isInvated(client->getFd()))
    {
        client->appendToSendBuffer(":server 473 " + client->getNickname() + " " + chanName + " :Cannot join channel (+i)\r\n");
        return;
    }

    if(!Channel->getKey().empty() && Channel->getKey() != key)
    {
        client->appendToSendBuffer(":server 475 " + client->getNickname() + " " + chanName + " :Cannot join channel (+k)\r\n");
        return;
    }

    if (Channel->getUserlimit() > 0 && Channel->getUserlimit() <= Channel->getMemberCount())
    {
        client->appendToSendBuffer(":server 471 " + client->getNickname() + " " + chanName + " :Cannot join channel (+l)\r\n");
        return;
    }

    Channel->addMember(client);
    Channel->removeInvated(client->getFd());

    std::string joinChannel =  clientPrefix(client) + " JOIN " + chanName +"\r\n";
    Channel->broadcast(joinChannel, -1);

    if (!Channel->getTopic().empty())
    {
        client->appendToSendBuffer(":server 332 " + client->getNickname() + " " + chanName + " :" + Channel->getTopic() + "\r\n");
    }
    else
        client->appendToSendBuffer(":server 331 " + client->getNickname() + " " + chanName + " :No topic is set\r\n");
    
    client->appendToSendBuffer(":server 353 " + client->getNickname() + " " + chanName + " :" + Channel->getMembersList() + "\r\n");
    client->appendToSendBuffer(":server 366 " + client->getNickname() + " " + chanName + " :End of /NAMES list\r\n");
}