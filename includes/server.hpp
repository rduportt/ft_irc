#ifndef SERVER_HPP_
#define SERVER_HPP_

#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <iostream>
#include <csignal>
#include <netinet/in.h>
#include <poll.h>
#include <map>
#include "parsing.hpp"
#include "command.hpp"
#include "channel.hpp"
#include "client.hpp"

extern volatile sig_atomic_t v_signal;

class server
{
    private :
        int                                 _port;
        sockaddr_in                         _serverAddress;
        std::string                         _pass;
        struct pollfd                       *_fds; //array of pollfd (1 entry per fd to watch)
        nfds_t                              _nfds; //number of entries in that array
        int                                 _serverSocket;
        std::map<int, Client*>              _clients;
        std::map<std::string, channel*>     _channels;
    public :
        server();
        ~server();
        void    start(char **argv);
        int     initSocket(char **argv);
        int     handleConnection();
        void    handleClientMessage(nfds_t &i);
        void    handleClientWrite(nfds_t &i);
        void    removeClient(nfds_t &i);
        void    dispatchCommand(Client &client, const message &msg);
        void    updatePolloutEvents();
        void    stop();
};

void handleSignal(int sig);

#endif