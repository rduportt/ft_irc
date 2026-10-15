#include "../includes/server.hpp"

volatile sig_atomic_t v_signal = 0;

void server::start(char **argv)
{
    int             ready;
    if(initSocket(argv) == -1)
        return ;
    while(_nfds > 0) {
        ready = poll(_fds, _nfds, -1);
        if (ready == -1)
        {
            if (v_signal)
                break;
            stop();
            return;
        }    
        for(nfds_t i = 0; i < _nfds; i++)
        {
            if(_fds[i].revents & POLLIN)
            {
                if(_fds[i].fd == _serverSocket) // accepting new client
                {
                    if(handleConnection() == -1)
                    {
                        stop();
                        return ;
                    }
                }
                else //Client message
                {
                    handleClientMessage(i);
                }
            }
            else if (_fds[i].revents & (POLLHUP | POLLERR))
            {
                std::cout << "closing fd " << _fds[i].fd << std::endl;
                removeClient(i);
            }
            if(_fds[i].revents & POLLOUT)
            {
                handleClientWrite(i);
            }
        }
        updatePolloutEvents();
    }
    stop();
    std::cout << "Server shutting down." << std::endl;
    exit(EXIT_SUCCESS);

}

int server::initSocket(char **argv)
{
    int opt = 1;

    char *end;
    long port = strtol(argv[1], &end, 10);

    if (*end != '\0')
    {
        return -1;
    }
    if (port < 1 || port > 65535)
    {
        return -1;
    }
    struct sigaction sa;
    sa.sa_handler = handleSignal;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(SIGINT, &sa, NULL);
    _nfds = 1;
    _fds = static_cast<struct pollfd *>(calloc(_nfds, sizeof(struct pollfd)));
    if(!_fds)
        return -1;

    _serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    _serverAddress.sin_family = AF_INET;
    _serverAddress.sin_port = htons(atoi(argv[1]));
    _serverAddress.sin_addr.s_addr = INADDR_ANY;
    _pass = std::string(argv[2]);
    if(setsockopt(_serverSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1)
        return -1;
    if(bind(_serverSocket, (struct sockaddr*)&_serverAddress, sizeof(_serverAddress)) == -1)
        return -1;
    if(listen(_serverSocket, 5) == -1)
        return -1;
    _fds[0].fd = _serverSocket;
    _fds[0].events = POLLIN;
    return 0;
}

int server::handleConnection()
{
    int clientSocket = accept(_serverSocket, NULL, NULL);
    if(clientSocket == -1)
    {
        std::cout << "accept failed" << std::endl;
        return 0;
    }
    struct pollfd *tmp = static_cast<struct pollfd *>(realloc(_fds, sizeof(struct pollfd) * (_nfds+1)));
    if(!tmp)
    {
        close(clientSocket);
        return -1;
    }
    _fds = tmp;
    _fds[_nfds].fd = clientSocket;
    _fds[_nfds].events = POLLIN;
    _fds[_nfds].revents = 0;
    _nfds++;
    Client *client = new Client(clientSocket);
    _clients.insert(std::make_pair(clientSocket, client));
    std::cout << "New client in !" << std::endl;  
    return 0;
}

void server::handleClientMessage(nfds_t &i)
{
    message msg;

    int fd = _fds[i].fd;
    std::map<int, Client*>::iterator it = _clients.find(fd);
    if(it == _clients.end())
    {
        std::cout << "Warning: no Client found for fd " << fd << std::endl;
    }
    else
    {
        char buffer[1024] = {0};    
        int r = recv(_fds[i].fd, buffer, sizeof(buffer), 0);
        if(r > 0)
        {
            // std::cout << "[recv " << r << " octets] \"";
            // for (int k = 0; k < r; k++) {
            //     if (buffer[k] == '\r') std::cout << "\\r";
            //     else if (buffer[k] == '\n') std::cout << "\\n";
            //     else std::cout << buffer[k];
            // }
            // std::cout << "\"" << std::endl;
            it->second->appendToRecvBuffer(std::string(buffer, r));
            std::string line;
            while (it->second->getNextLine(line))
            {
                if (line.empty())
                    continue;
                msg = Parser::line(line);
                dispatchCommand(*it->second, msg);
                if (it->second->isDisconnected())
                {
                    removeClient(i);
                    return;
                }
            }
        }
            else if (r == 0)
        {
            std::cout << "Client on fd " << _fds[i].fd << " disconnected" << std::endl;
            removeClient(i);
        }
        else // r = -1
        {
            std::cout << "recv error on fd " << _fds[i].fd << std::endl;
            removeClient(i);
        } 
    }

}

void    server::handleClientWrite(nfds_t &i)
{
    int fd = _fds[i].fd;
    std::map<int, Client*>::iterator it = _clients.find(fd);
    if(it == _clients.end())
    {
        std::cout << "Warning: no Client found for fd " << fd << std::endl;
    }
    else
    {
        const std::string &buf = it->second->getSendBuffer();
        if (buf.empty())
        {
            _fds[i].events = POLLIN;   // plus rien à envoyer, désactive POLLOUT
            return;
        }
        int s = send(_fds[i].fd, buf.c_str(), buf.size(), 0);
        if(s > 0)
        {
            it->second->consumeSendBuffer(s);
        }
        else // s = -1
        {
            std::cout << "send error on fd " << _fds[i].fd << std::endl;
            removeClient(i);
        } 
    }

}

//a modif + test
void server::dispatchCommand(Client &client, const message &msg)
{
    if(msg.command == "PASS")
        command::pass(&client, msg, _pass);
    else if(msg.command == "NICK")
        command::nick(&client, msg, _clients);
    else if(msg.command == "USER")
        command::user(&client, msg);
    else if(msg.command == "PING")
        command::ping(&client, msg);
    else if(msg.command == "JOIN")
        command::join(&client, msg, _channels);
    else if(msg.command == "PART")
        command::part(&client, msg, _channels);
    else if(msg.command == "PRIVMSG")
        command::privmsg(&client, msg, _channels, _clients);
    else if(msg.command == "KICK")
    {
        channel *chan = NULL;
        if (!msg.params.empty())
        {
            std::map<std::string, channel*>::iterator it = _channels.find(msg.params[0]);
            if (it != _channels.end())
                chan = it->second;
        }
        command::kick(chan, &client, msg, _clients);
    }
    else if(msg.command == "INVITE")
    {
        channel *chan = NULL;
        if (msg.params.size() >= 2)
        {
            std::map<std::string, channel*>::iterator it = _channels.find(msg.params[1]);
            if (it != _channels.end())
                chan = it->second;
        }
        command::invite(chan, &client, msg, _clients);
    }
    else if(msg.command == "TOPIC")
    {
        channel *chan = NULL;
        if (!msg.params.empty())
        {
            std::map<std::string, channel*>::iterator it = _channels.find(msg.params[0]);
            if (it != _channels.end())
                chan = it->second;
        }
        command::topic(chan, &client, msg);
    }
    else if(msg.command == "MODE")
        command::mode(&client, msg, _channels, _clients);
    else if(msg.command == "QUIT")
        command::quit(&client, msg, _channels);
    else if (msg.command == "CAP")
    {
        if (!msg.params.empty() && msg.params[0] == "LS")
            client.appendToSendBuffer("CAP * LS :\r\n");
        else if (!msg.params.empty() && msg.params[0] == "END")
        {
        }
    }
    else
    {
        std::string nick = client.getNickname().empty() ? "*" : client.getNickname();
        client.appendToSendBuffer(":server 421 " + nick + " " + msg.command + " :Unknown command\r\n");
    }

    for(nfds_t i = 0; i < _nfds; i++)
    {
        if(_fds[i].fd == _serverSocket)
            continue;
        std::map<int, Client*>::iterator it = _clients.find(_fds[i].fd);
        if (it != _clients.end() && !it->second->getSendBuffer().empty())
            _fds[i].events |= POLLOUT;
    }
}



void server::removeClient(nfds_t &i)
{
    std::map<int, Client*>::iterator it = _clients.find(_fds[i].fd);
    if(it != _clients.end())
    {
        delete it->second;
        _clients.erase(it);
    }
    close(_fds[i].fd);
    _fds[i] = _fds[_nfds - 1];
    _nfds--;
    i--;
}

void server::updatePolloutEvents()
{
    for (nfds_t i = 0; i < _nfds; i++)
    {
        if (_fds[i].fd == _serverSocket)
            continue;
        std::map<int, Client*>::iterator it = _clients.find(_fds[i].fd);
        if (it != _clients.end() && !it->second->getSendBuffer().empty())
            _fds[i].events |= POLLOUT;
    }
}

void handleSignal(int sig)
{
    (void)sig;
    v_signal = 1;
}

server::server()
{
    _fds = NULL;
    _nfds = 0;
}

server::~server()
{
    if(_fds)
        stop();
}

void server::stop()
{
    for (nfds_t j = 0; j < _nfds; j++)
        close(_fds[j].fd);
    free(_fds);
    _fds = NULL;
    _nfds = 0;
    for (std::map<int, Client*>::iterator it = _clients.begin(); it != _clients.end(); ++it)
        delete it->second;
    _clients.clear();
    for (std::map<std::string, channel*>::iterator it = _channels.begin(); it != _channels.end(); ++it)
        delete it->second;
    _channels.clear();
}