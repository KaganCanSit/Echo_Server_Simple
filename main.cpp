#include <iostream>

#include "Server.h"

int main(int argc, char** argv)
{
    if (argc < 2) {
        std::cout << "Usage: " << argv[0] << " <Port Number>" << std::endl;
        return 0;
    }

    int portNumber = atoi(argv[1]);
    
    if (portNumber < 1024) {
        std::cerr << "Usually ports 0-1024 are used by the operating system. Choose a more secure port number." << std::endl;
        return -1;
    }

    SocketServer server(portNumber);
    server.Start();
}