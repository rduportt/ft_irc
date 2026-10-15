#include "../../includes/command.hpp"

void command::quit(Client *client, const message &msg, std::map<std::string, channel*> &channels)
{
    std::string reason = msg.params.empty() ? "Leaving" : msg.params[0];
    std::string quitMsg = ":" + client->getNickname() + "!" + client->getUsername() + "@localhost QUIT :" + reason + "\r\n";
    client->appendToSendBuffer(quitMsg);
    std::map<std::string, channel*>::iterator it = channels.begin();
    while(it != channels.end())
    {
        if(it->second->isMember(client->getFd()))
        {
            it->second->broadcast(quitMsg, -1);
            it->second->removeMember(client->getFd());
            
            if(it->second->getMemberCount() == 0)
            {
                delete it->second;
                std::map<std::string, channel*>::iterator toErase = it;
                ++it;
                channels.erase(toErase);
                continue;
            }
        }
        it++;
    }
    client->setDisconnected(true);
}
