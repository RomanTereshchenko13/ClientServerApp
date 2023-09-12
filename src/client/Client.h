#ifndef CLIENT_H
#define CLIENT_H

#include"pch.h"

class Client
{
public:
    Client();
    void SendMessage();

private:
    void CreateSocket();
    void Connect();

private:
    int clientSocket;
    sockaddr_in serverAddr;
};


#endif // CLIENT_H
