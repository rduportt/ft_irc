#include "../../includes/command.hpp"

void command::pass(Client *client, const message &msg, const std::string &serverPassword)
{
    if (client->is_authorized())
    {
        client->appendToSendBuffer(":server 462 * :You may not reregister\r\n");
        return;
    }

    if (msg.params.empty())
    {
        client->appendToSendBuffer(":server 461 * :Not enough parameters\r\n");
        return;
    }
    if (msg.params[0] != serverPassword)
    {
        client->appendToSendBuffer(":server 464 * :Password incorrect\r\n");
        return;
    }
    client->setAuthenticated(true);
}