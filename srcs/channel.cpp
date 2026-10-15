#include "../includes/channel.hpp"

channel::channel(const std::string& name)
    : _name(name),
      _topic(""),
      _key(""),
      _topicSetter(""),
      _topicSetAt(0),
      userLimit(0),         
      _inviteOnly(false),   
      _topicRestricted(false)
{}

const std::string& channel::getName()
{
    return _name;
}

const std::string& channel::getTopic()
{
    return _topic;
}

const std::string& channel::getKey()
{
    return _key;
}

int channel::getUserlimit()
{
    return userLimit;
}

bool channel::isTopicRestricted()
{
    return _topicRestricted;
}

bool channel::isInviteOnly()
{
    return _inviteOnly;
}

const std::string& channel::getTopicSetter() const 
{
    return _topicSetter;
}

time_t channel::getTopicSetAt() const 
{
    return _topicSetAt;
}

void channel::setTopic(const std::string& topic, const std::string &setter, time_t setAt)
{
    _topic = topic;
    _topicSetter = setter;
    _topicSetAt = setAt;
}

void channel::setKey(const std::string& key)
{
    _key = key;
}

void channel::setUserlimit(int limit)
{
    userLimit = limit;
}

void channel::setInviteOnly(bool val)
{
    _inviteOnly = val;
}
void channel::setTopicRestricted(bool val)
{
    _topicRestricted = val;
}

void channel::addMember(Client *client)
{
    members[client->getFd()] = client;
    if (members.size() == 1)
        _operator[client->getFd()] = true;
}

void channel::removeMember(int fd)
{
        members.erase(fd);
        _operator.erase(fd);
        removeInvated(fd);
}

bool channel::isMember(int fd) const
{
    return (members.find(fd) != members.end());
}

Client* channel::getMember(int fd) const
{
    std::map<int, Client*>::const_iterator it = members.find(fd);
    if (it != members.end())
        return it->second;
    return NULL;
}

void channel::setOperator(int fd, bool val)
{
    _operator[fd] = val;
}

int channel::getMemberCount()
{
    int count = 0;
    std::map<int, Client*>::const_iterator it = members.begin();
    while (it != members.end())
    {
        count++;
        it++;
    }
    return count;
}

bool channel::isOperator(int fd) const
{
    std::map<int, bool>::const_iterator it = _operator.find(fd);
    if (it != _operator.end())
        return it->second;
    return (false);
}

void channel::addInvated(int fd)
{
    if (!isInvated(fd))
        invated.push_back(fd);
}

bool channel::isInvated(int fd) const
{
    size_t i = 0;
    while (i < invated.size())
    {
        if (invated[i] == fd)
            return true;
        i++;
    }
    return false;
}

void channel::removeInvated(int fd)
{
    size_t i = 0;
    while (i < invated.size())
    {
        if (invated[i] == fd)
            invated.erase(invated.begin() + i);
        i++;
    }    
}

void channel::broadcast(const std::string& message, int excludeFd)
{
    std::map<int, Client*>::const_iterator it;
    it = members.begin();
    while(it != members.end())
    {
        if (it->first != excludeFd)
            it->second->appendToSendBuffer(message);
        it++;
    }
}

std::string channel::getMembersList() const
{
    std::string list;
    std::map<int, Client*>::const_iterator it;

    it = members.begin();
    while (it != members.end())
    {
        if (!list.empty())
            list += " ";
        if (isOperator(it->first))
            list += "@";
        list += it->second->getNickname();
        it++;
    }
    return list;
}

channel::~channel() {}