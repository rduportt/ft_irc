#include "../../includes/command.hpp"

void command::invite(channel *chan, Client *client, const message &msg, std::map<int, Client*> &clients)
{
    if (msg.params.size() < 2)
    {
        client->appendToSendBuffer(":server 461 " + client->getNickname() + " " + msg.command + " :Not enough parameters\r\n");
        return;
    }

    if (!chan)
    {
        client->appendToSendBuffer(":server 403 " + client->getNickname() + " " + msg.params[1] + " :No such channel\r\n");
        return;
    }

    if (!chan->isMember(client->getFd()))
    {
        client->appendToSendBuffer(":server 442 " + client->getNickname() + " " + chan->getName() + " :You're not on that channel\r\n");
        return;
    }

    if (chan->isInviteOnly() && !chan->isOperator(client->getFd()))
    {
        client->appendToSendBuffer(":server 482 " + client->getNickname() + " " + chan->getName() + " :You're not channel operator\r\n");
        return;
    }

    std::string targetNick = msg.params[0];
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
        client->appendToSendBuffer(":server 401 " + client->getNickname() + " " 
            + targetNick + " :No such nick\r\n");
        return;
    }

    if (chan->isMember(target->getFd()))
    {
        client->appendToSendBuffer(":server 443 " + client->getNickname() + " " + targetNick + " " + chan->getName() + " :is already on channel\r\n");
        return;
    }

    chan->addInvated(target->getFd());
    std::string inviteMsg = ":" + client->getNickname() + "!" + client->getUsername() + "@localhost INVITE " + targetNick + " :" + chan->getName() + "\r\n";
    target->appendToSendBuffer(inviteMsg);
    client->appendToSendBuffer(":server 341 " + client->getNickname() + " " + targetNick + " " + chan->getName() + "\r\n");
}