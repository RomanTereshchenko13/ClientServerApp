#include "Server.h"
#include "Server.h"

#define SERVER_PORT 7500
#define FILE_TRANSFER_PORT 7505
#define MAX_BUFFER_SIZE 1024

Server::Server()
{
    CreateSocket();
    BindSocket();
    Listen();
}

Server::~Server()
{
    std::cout << "Server Destructor called\n";
    close(serverSock);
}

void Server::AcceptClient()
{
    clientAddrLen = sizeof(clientAddr);
    clientSock = accept(serverSock, (sockaddr*)&clientAddr, &clientAddrLen);
    if (clientSock < 0)
    {
        std::cerr << "Error: " << strerror(errno) << std::endl;
        exit(1);
    }

    // Receive the list of files from the client
    char file_list[MAX_BUFFER_SIZE];
    memset(file_list, 0, sizeof(file_list));
    ssize_t bytesReceived = recv(clientSock, file_list, sizeof(file_list), 0);

    if (bytesReceived == -1) {
        perror("Error receiving file list from client");
        exit(EXIT_FAILURE);
    }

    std::cout << "Received file list from client: " << file_list << std::endl;
}

//Private methods to create serverSocket
void Server::CreateSocket()
{
    serverSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (serverSock < 0)
    {
        std::cerr << "Error: " << strerror(errno) << std::endl;
        exit(1);
    }

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(SERVER_PORT);
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
}

void Server::BindSocket()
{
    if (bind(serverSock, (struct sockaddr*) &serverAddr, sizeof(serverAddr)))
    {
        std::cerr << "Error: " << strerror(errno) << std::endl;
        exit(1);
    }
}

void Server::Listen()
{
    if (listen(serverSock, 5) < 0)
    {
        std::cerr << "Error: " << strerror(errno) << std::endl;
        exit(1);
    }

    std::cout << "Server is listening on port " << SERVER_PORT << std::endl;

}
