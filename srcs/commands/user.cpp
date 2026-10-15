#include "../../includes/command.hpp"

void command::user(Client *client, const message &msg)
{
    if(!client->is_authorized())
    {
        client->appendToSendBuffer(":server 451 * :You have not registered\r\n");
        return;
    }
    if (client->is_registered())
    {
        client->appendToSendBuffer(":server 462 * :You may not reregister\r\n");
        return;
    }

    if (msg.params.size() < 4)
    {
        client->appendToSendBuffer(":server 461 * USER :not enough parameters\r\n");
        return;
    }

    client->setUsername(msg.params[0]);
    client->setRealname(msg.params[3]);

    if (!client->getNickname().empty() && !client->is_registered())
    {
        client->setRegistered(true);
        client->appendToSendBuffer(":server 001 " + client->getNickname() + " :Welcome to the IRC network\r\n");
    }
}