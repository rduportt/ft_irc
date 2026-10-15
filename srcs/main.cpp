#include "../includes/client.hpp"
#include "../includes/server.hpp"

int main(int argc, char **argv)
{
    if (argc != 3)
    {
        std::cout << "please put the right parameters (./a.out 'port' 'pass')!" << std::endl; 
        return 1;
    }
    v_signal = 0;
    server Server;
    Server.start(argv);
}