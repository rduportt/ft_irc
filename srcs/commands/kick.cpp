#include "../../includes/command.hpp"

void command::kick(channel *chan ,Client *client, const message &msg, std::map<int, Client*> &clients)
{
    if(msg.params.size() < 2)
    {
        client->appendToSendBuffer(":server 461 " + client->getNickname() + " " + msg.command + " :Not enough parameters\r\n");
        return;
    }
    if(!chan)
    {
        client->appendToSendBuffer(":server 403 " + client->getNickname() + " " + msg.params[0] + " :No such channel\r\n");
        return ;
    }
    if (!chan->isMember(client->getFd()))
    {
        client->appendToSendBuffer(":server 442 " + client->getNickname() + " " 
            + chan->getName() + " :You're not on that channel\r\n");
        return;
    }
    if(!chan->isOperator(client->getFd()))
    {
        client->appendToSendBuffer(":server 482 " + client->getNickname() + " " + msg.params[0] + " :You're not channel operator\r\n");
        return ;
    }

    std::string targetNick = msg.params[1];
    Client *target = NULL;
    std::map<int, Client*>::iterator it = clients.begin();
    while (it != clients.end())
    {
        if (it->second->getNickname() == targetNick)
        {
            target = it->second;
            break;
        }
        ++it;
    }
    if (!target)
    {
        client->appendToSendBuffer(":server 401 " + client->getNickname() + " " + targetNick + " :No such nick\r\n");
        return;
    }
    if (!chan->isMember(target->getFd()))
    {
        client->appendToSendBuffer(":server 441 " + client->getNickname() + " " + targetNick + " " + chan->getName() + " :They aren't on that channel\r\n");
        return;
    }
    std::string kickMsg = ":" + client->getNickname() + "!" + client->getUsername() + "@localhost KICK " + chan->getName() + " " + targetNick;
    if (msg.params.size() > 2)
        kickMsg += " :" + msg.params[2];  // raison du kick
    else
        kickMsg += " :" + client->getNickname();  // raison par défaut
    kickMsg += "\r\n";
    chan->broadcast(kickMsg, -1);
    chan->removeMember(target->getFd());
}
