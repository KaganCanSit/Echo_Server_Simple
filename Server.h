#ifndef SERVER_H
#define SERVER_H

#include <iostream>
#include <string>

#ifdef _WIN32
#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")
#include <WS2tcpip.h>
#else
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#endif

#define BUFF_LEN 1024

#pragma once
class SocketServer {
public:
    explicit SocketServer(int portNumber);
    ~SocketServer();

    void Start();
    void Stop();

private:
    int listenerSocket;
    int clientSocket;
    int portNumber;
    char buffer[BUFF_LEN];

    int CreateListener();
    void CloseSocket(int socket);

    void AcceptConnection();
    void HandleCommunication();
};

#endif