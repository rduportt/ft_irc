#include "../../includes/command.hpp"
#include <sstream>


std::string clientPrefix(Client *client)
{
    return ":" + client->getNickname() + "!"
               + client->getUsername() + "@localhost";
}

static Client* findByNick(const std::string nick, std::map<int, Client*> &clients)
{
    std::map<int, Client*>::iterator it;
    for (it = clients.begin(); it != clients.end(); it++)   
    {
        if(it->second->getNickname() == nick)
            return it->second;
    }
    return NULL;
}

std::string command::getChannelModes(channel *Channel)
{
    std::string modes = "+";
    std::string params;

    if (Channel->isInviteOnly())
        modes += "i";
    if (Channel->isTopicRestricted())
        modes += "t";
    if (!Channel->getKey().empty())
    {
        modes += "k";
        params += " " + Channel->getKey();
    }
    if (Channel->getUserlimit() > 0)
    {
        modes += "l";
        std::ostringstream oss;
        oss << Channel->getUserlimit();
        params += " " + oss.str();
    }
    if (modes == "+")
        return "";
    return modes + params;
}

void command::mode(Client *client, const message &msg, std::map<std::string, channel*> &channels, std::map<int, Client*> &clients)
{
    if (!client->is_registered())
    {
        client->appendToSendBuffer(":server 451 * :You have not registered\r\n");
        return;
    }
    if (msg.params.empty())
    {
        client->appendToSendBuffer(":server 461 " + client->getNickname() + " MODE :Not enough parameters\r\n");
        return;
    }

    std::string chanName = msg.params[0];
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
    if (!Channel->isOperator(client->getFd()))
    {
        client->appendToSendBuffer(":server 482 " + client->getNickname() + " " + chanName + " :You're not channel operator\r\n");
        return;
    }
    if (msg.params.size() < 2)
    {
        client->appendToSendBuffer(":server 324 " + client->getNickname() + " " + chanName + " " + getChannelModes(Channel) + "\r\n");
        return;
    }
    std::string modeStr = msg.params[1];
    size_t paramIndex = 2;

    if (modeStr.empty())
        return;
    bool signe = true;
    if (modeStr[0] == '+')
        signe = true;
    else if(modeStr[0] == '-')
        signe = false;
    else
    {
        client->appendToSendBuffer(":server 472 " + client->getNickname() + " " + modeStr + " :Unknown mode\r\n");
        return;
    }
    // Commencer à 1 pour sauter le + ou -
    size_t i = 1;
    while (i < modeStr.size())
    {
        char mode = modeStr[i];

        if (mode == 'i')
        {
            Channel->setInviteOnly(signe);
            std::string modeMsg = clientPrefix(client) + " MODE "
                                + chanName + " "
                                + (signe ? "+i" : "-i") + "\r\n";
            Channel->broadcast(modeMsg, -1);
        }
        else if (mode == 't')
        {
            Channel->setTopicRestricted(signe);
            std::string modeMsg = clientPrefix(client) + " MODE "
                                + chanName + " "
                                + (signe ? "+t" : "-t") + "\r\n";
            Channel->broadcast(modeMsg, -1);
        }
        else if (mode == 'k')
        {
            if (signe)
            {
                if (paramIndex >= msg.params.size())
                {
                    client->appendToSendBuffer(":server 461 " + client->getNickname()
                                            + " MODE :Not enough parameters\r\n");
                    return;
                }
                std::string key = msg.params[paramIndex++];
                Channel->setKey(key);
                std::string modeMsg = clientPrefix(client) + " MODE "
                                    + chanName + " +k " + key + "\r\n";
                Channel->broadcast(modeMsg, -1);
            }
            else
            {
                Channel->setKey("");
                std::string modeMsg = clientPrefix(client) + " MODE "
                                    + chanName + " -k\r\n";
                Channel->broadcast(modeMsg, -1);
            }
        }
        else if (mode == 'o')
        {
            if (paramIndex >= msg.params.size())
            {
                client->appendToSendBuffer(":server 461 " + client->getNickname()
                                        + " MODE :Not enough parameters\r\n");
                return;
            }
            std::string targetNick = msg.params[paramIndex++];
            Client *target = findByNick(targetNick, clients);
            if (!target)
            {
                client->appendToSendBuffer(":server 401 " + client->getNickname()
                                        + " " + targetNick + " :No such nick\r\n");
                i++;
                continue;
            }
            if (!Channel->isMember(target->getFd()))
            {
                client->appendToSendBuffer(":server 441 " + client->getNickname()
                                        + " " + targetNick + " " + chanName
                                        + " :They aren't on that channel\r\n");
                i++;
                continue;
            }
            Channel->setOperator(target->getFd(), signe);
            std::string modeMsg = clientPrefix(client) + " MODE "
                                + chanName + " "
                                + (signe ? "+o " : "-o ") + targetNick + "\r\n";
            Channel->broadcast(modeMsg, -1);
        }
        else if (mode == 'l')
        {
            if (signe)
            {
                if (paramIndex >= msg.params.size())
                {
                    client->appendToSendBuffer(":server 461 " + client->getNickname()
                                            + " MODE :Not enough parameters\r\n");
                    return;
                }
                int limit = std::atoi(msg.params[paramIndex++].c_str());
                if (limit <= 0)
                {
                    client->appendToSendBuffer(":server 461 " + client->getNickname()
                                            + " MODE :Invalid limit\r\n");
                    return;
                }
                Channel->setUserlimit(limit);
                std::string modeMsg = clientPrefix(client) + " MODE "
                                    + chanName + " +l "
                                    + msg.params[paramIndex - 1] + "\r\n";
                Channel->broadcast(modeMsg, -1);
            }
            else
            {
                Channel->setUserlimit(0);
                std::string modeMsg = clientPrefix(client) + " MODE "
                                    + chanName + " -l\r\n";
                Channel->broadcast(modeMsg, -1);
            }
        }
        i++;
    }
}


