#ifndef CLIENT_HPP_
#define CLIENT_HPP_

#include <iostream>
#include <cstring>
#include <csignal>
#include <netinet/in.h>
#include <string>
#include <vector>


class Client
{
    private :
        int _fd;
        std::string _username;
        std::string _nickname;
        std::string _realname;
        std::string _recvBuffer;
        std::string _sendBuffer;
        int _authorized;
        int _register;
        bool _disconnected;
    public:
        Client(int fd);
        int getFd() const;
        const std::string& getUsername() const;
        const std::string& getNickname() const;
        const std::string& getRealname() const;
        const std::string& getRecvBuffer() const;
        const std::string& getSendBuffer() const;
        bool is_authorized();
        bool is_registered();
        bool isDisconnected() const;

        void setUsername(const std::string &nick);
        void setNickname(const std::string &nick);
        void setRealname(const std::string &name);
        void setAuthenticated(bool val);
        void setRegistered(bool val);
        void setDisconnected(bool val);

        void appendToRecvBuffer(const std::string &data);
        void appendToSendBuffer(const std::string &data);
        void consumeSendBuffer(size_t n);
        bool getNextLine(std::string &out);
        ~Client();
};

#endif