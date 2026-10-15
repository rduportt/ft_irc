#include "../../includes/command.hpp"

void command::topic(channel *chan, Client *client, const message &msg)
{
    if (msg.params.empty())
    {
        client->appendToSendBuffer(":server 461 " + client->getNickname() + " " + msg.command + " :Not enough parameters\r\n");
        return;
    }

    if (!chan)
    {
        client->appendToSendBuffer(":server 403 " + client->getNickname() + " " + msg.params[0] + " :No such channel\r\n");
        return;
    }

    if (!chan->isMember(client->getFd()))
    {
        client->appendToSendBuffer(":server 442 " + client->getNickname() + " " + chan->getName() + " :You're not on that channel\r\n");
        return;
    }

    if (msg.params.size() == 1)
    {
        if (chan->getTopic().empty())
        {
            client->appendToSendBuffer(":server 331 " + client->getNickname() + " " 
                + chan->getName() + " :No topic is set\r\n");
        }
        else
        {
            // RPL_TOPIC (332)
            client->appendToSendBuffer(":server 332 " + client->getNickname() + " " 
                + chan->getName() + " :" + chan->getTopic() + "\r\n");

            // RPL_TOPICWHOTIME (333)
            std::stringstream ss;
            ss << chan->getTopicSetAt();
            client->appendToSendBuffer(":server 333 " + client->getNickname() + " " 
                + chan->getName() + " " + chan->getTopicSetter() + " " + ss.str() + "\r\n");
        }
        return;
    }

    if (chan->isTopicRestricted() && !chan->isOperator(client->getFd()))
    {
        client->appendToSendBuffer(":server 482 " + client->getNickname() + " " + chan->getName() + " :You're not channel operator\r\n");
        return;
    }
    chan->setTopic(msg.params[1], client->getNickname(), std::time(NULL));
    std::string topicMsg = ":" + client->getNickname() + "!" + client->getUsername() + "@localhost TOPIC " + chan->getName() + " :" + msg.params[1] + "\r\n";
    chan->broadcast(topicMsg, -1);
}