#include "../includes/client.hpp"


Client::Client(int fd) : _fd(fd), _authorized(false), _register(false), _disconnected(false)
{

}

Client::~Client()
{
    
}

int Client::getFd() const
{
    return this->_fd;
}

const std::string& Client::getNickname() const
{
    return this->_nickname;
}

const std::string& Client::getRealname() const
{
    return this->_realname;
}

const std::string& Client::getUsername() const
{
    return this->_username;
}

const std::string& Client::getRecvBuffer() const
{
    return this->_recvBuffer;
}

const std::string& Client::getSendBuffer() const
{
    return this->_sendBuffer;
}

bool Client::is_authorized()
{
    return this->_authorized;
}

bool Client::is_registered()
{
    return this->_register;
}

void Client::setNickname(const std::string& nick)
{
    this->_nickname = nick;
}

void Client::setRealname(const std::string& real)
{
    this->_realname = real;
}

void Client::setUsername(const std::string& user)
{
    this->_username = user;
}

void Client::setAuthenticated(bool val)
{
    this->_authorized = val;
}

void Client::setRegistered(bool val)
{
    this->_register = val;
}

void Client::appendToRecvBuffer(const std::string& data)
{
    this->_recvBuffer += data;
}

void Client::appendToSendBuffer(const std::string& data)
{
    this->_sendBuffer += data;
}

void Client::consumeSendBuffer(size_t n)
{
    _sendBuffer.erase(0, n);
}

// Client.cpp
bool Client::getNextLine(std::string &out)
{
    size_t pos = _recvBuffer.find('\n');
    if (pos == std::string::npos)
        return false;
    out = _recvBuffer.substr(0, pos);
    _recvBuffer.erase(0, pos + 1);
    // enlève le \r éventuel en fin de ligne
    if (!out.empty() && out[out.size() - 1] == '\r')
        out.erase(out.size() - 1);
    return true;
}

void Client::setDisconnected(bool val)
{
    _disconnected = val;
}

bool Client::isDisconnected() const
{
    return _disconnected;
}