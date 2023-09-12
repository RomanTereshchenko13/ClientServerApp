#include "Client.h"

#define SERVER_ADDRESS "127.0.0.1"
#define SERVER_PORT 7500
#define FILE_TRANSFER_PORT 7505
#define MAX_BUFFER_SIZE 1024

Client::Client()
{
    CreateSocket();
    Connect();
}

void Client::SendMessage()
{
    std::string file_list = "file1.txt;file2.txt;file3.txt";
    if (send(clientSocket, file_list.c_str(), file_list.size(), 0) == -1) 
    {
        perror("Error sending file list to server");
        exit(EXIT_FAILURE);
    }
}

void Client::CreateSocket()
{
    clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (clientSocket < 0)
    {
        std::cerr << "Error: " << strerror(errno) << std::endl;
        exit(1);
    }

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(SERVER_PORT);
    serverAddr.sin_addr.s_addr = inet_addr(SERVER_ADDRESS);
}

void Client::Connect()
{
    if (connect(clientSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == -1) 
    {
        perror("Error connecting to server");
        exit(EXIT_FAILURE);
    }
}
