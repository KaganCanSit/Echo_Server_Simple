#include "Server.h"

SocketServer::SocketServer(int portNumber)
    : listenerSocket(-1), clientSocket(-1), portNumber(portNumber) {
    memset(buffer, 0, BUFF_LEN);
}

SocketServer::~SocketServer() {
    Stop();
}

void SocketServer::Start() {

#ifdef _WIN32
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "Failed to WSAData initialize." << std::endl;
        return;
    }
#endif

    if (CreateListener() != 0) {
        std::cerr << "Failed to create listener socket." << std::endl;
        return;
    }

    AcceptConnection();
    HandleCommunication();
}

void SocketServer::Stop() {
    CloseSocket(listenerSocket);
    CloseSocket(clientSocket);

#ifdef _WIN32
    WSACleanup();
#endif
}

int SocketServer::CreateListener() {
    listenerSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (listenerSocket < 0) {
        std::cerr << "Failed to create listener socket." << std::endl;
        return -1;
    }

    struct sockaddr_in serverAddress;
    memset(&serverAddress, 0, sizeof(serverAddress));
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(portNumber);
    serverAddress.sin_addr.s_addr = INADDR_ANY;

    if (bind(listenerSocket,reinterpret_cast<struct sockaddr*>(& serverAddress), sizeof(serverAddress)) < 0) {
        std::cerr << "Binding failed." << std::endl;
        CloseSocket(listenerSocket);
        return -1;
    }

    if (listen(listenerSocket, 5) < 0) {
        std::cerr << "Socket listening failed." << std::endl;
        CloseSocket(listenerSocket);
        return -1;
    }

    std::cout << "Server is listening on port " << portNumber << ". Waiting for connections." << std::endl;

    return 0;
}

void SocketServer::AcceptConnection() {
    struct sockaddr_in clientAddress;
    socklen_t clientAddressLength = sizeof(clientAddress);
    clientSocket = accept(listenerSocket, reinterpret_cast<struct sockaddr*>(&clientAddress), &clientAddressLength);

    if (clientSocket < 0) {
        std::cerr << "Connection failed." << std::endl;
        return;
    }

    char clientIp[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(clientAddress.sin_addr), clientIp, INET_ADDRSTRLEN);

    int remotePort = ntohs(clientAddress.sin_port);
    std::cout << "New connection established. IP: " << clientIp << " Port:" << remotePort << std::endl;
}

inline void trim(std::string& str) {
    str.erase(str.begin(), std::find_if(str.begin(), str.end(), [](unsigned char ch) {
        return !std::isspace(ch);
        }));

    str.erase(std::find_if(str.rbegin(), str.rend(), [](unsigned char ch) {
        return !std::isspace(ch);
        }).base(), str.end());
}

void SocketServer::HandleCommunication() {
    while (true) {
        memset(buffer, 0, BUFF_LEN);
        int bytesReceived = recv(clientSocket, buffer, BUFF_LEN - 1, 0);

        if (bytesReceived == SOCKET_ERROR) {
            std::cerr << "Error reading from socket." << std::endl;
            break;
        }
        else if (bytesReceived == 0) {
            std::cerr << "Client disconnected." << std::endl;
            break;
        }

        // Handle the received data as needed
        std::string receivedMessage(buffer);
        trim(receivedMessage);
        std::cout << "Client Message: " << receivedMessage << std::endl;

        // Respond to the client
        std::string response = receivedMessage + " content received.\n\n";
        if (send(clientSocket, response.c_str(), response.length(), 0) == SOCKET_ERROR) {
            std::cerr << "Error sending response." << std::endl;
            break;
        }
    }

    closesocket(clientSocket);
}


void SocketServer::CloseSocket(int socket) {
#ifdef _WIN32
    closesocket(socket);
#else
    close(socket);
#endif
}