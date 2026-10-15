#include "../../includes/command.hpp"

void command::nick(Client *client, const message &msg, std::map<int, Client*> &clients)
{
    if(!client->is_authorized())
    {
        client->appendToSendBuffer(":server 451 * :You have not registered\r\n");
        return;
    }
    if(msg.params.empty())
    {
        client->appendToSendBuffer(":server 431 * :No nickname given\r\n");
        return;
    }

    std::string newNick = msg.params[0];
    std::map<int, Client*>::const_iterator it;
    it = clients.begin();
    while (it != clients.end())
    {
        if (it->second->getNickname() == newNick && it->first != client->getFd())
        {
            client->appendToSendBuffer(":server 433 " + newNick + " " + newNick + " :Nickname is already in use\r\n");
            return;
        }

        it++;
    }
    std::string oldNick = client->getNickname();
    client->setNickname(newNick);
    if (!oldNick.empty())
        client->appendToSendBuffer(":" + oldNick + " NICK " + newNick + "\r\n");
    if (!client->is_registered() && !client->getNickname().empty() && !client->getUsername().empty())
    {
        client->setRegistered(true);
        client->appendToSendBuffer(":server 001 " + newNick + " :Welcome to the IRC network\r\n");
    }
}
