#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include "client.hpp"
#include <iostream>
#include <cstring>
#include <csignal>
#include <ctime>
#include <netinet/in.h>
#include <string>
#include <vector>
#include <map>

class channel
{
    private :
        std::string _name;
        std::string _topic;
        std::string _key;
        std::string _topicSetter;
        time_t      _topicSetAt;
        int userLimit;
        bool _inviteOnly;
        bool _topicRestricted;

        std::map<int, Client*> members;
        std::map<int, bool> _operator;
        std::vector<int>    invated;
    public:
        channel(const std::string &name);

        const std::string& getName();
        const std::string& getTopic();
        const std::string& getKey();
        int getUserlimit();
        bool isInviteOnly();
        bool isTopicRestricted();
        int getMemberCount();
        const std::string& getTopicSetter() const;
        time_t getTopicSetAt() const;



        void setTopic(const std::string& topic, const std::string &setter, time_t setAt);
        void setKey(const std::string &key);
        void setUserlimit(int limit);
        void setInviteOnly(bool val);
        void setTopicRestricted(bool val);

        void addMember(Client *client);
        void removeMember(int fd);
        bool isMember(int fd) const;
        Client* getMember(int fd) const;

        void setOperator(int fd, bool val);
        bool isOperator(int fd) const;

        void addInvated(int fd);
        bool isInvated(int fd) const;
        void removeInvated(int fd);

        void broadcast(const std::string& message, int exludeFd);
        std::string getMembersList() const;
        ~channel();

};

#endif