#ifndef SERVER_H
#define SERVER_H

#include"pch.h"

class Server
{
public:
    Server();
    ~Server();

    void AcceptClient();

    Server(const Server& other) = delete;
    Server& operator=(const Server& other) = delete;

private:
    void CreateSocket();
    void BindSocket();
    void Listen();

private:
    int serverSock;
    int clientSock;
    sockaddr_in serverAddr;
    sockaddr_in clientAddr;
    socklen_t clientAddrLen;
};

#endif // !SERVER_H
