#include "../../includes/command.hpp"

void command::ping(Client *client, const message &msg)
{
    if(msg.params.empty())
    {
        client->appendToSendBuffer(":server 409 :No origin specified");
        return;
    }
    client->appendToSendBuffer(":server PONG server :" + msg.params[0] + "\r\n");
}