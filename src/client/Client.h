#ifndef CLIENT_H
#define CLIENT_H

#include"../shared/Connection.h"
#include"../shared/Serialize.h"

using namespace boost;

namespace fs = std::filesystem;

class Client
{
public:
    Client();
    ~Client();
    void Connect(std::string host, const uint16_t port);
    void Disconnect();
    bool IsConnected();
    
    void SendListofFiles();
    void SendFile();

private:
    void FillVector();

private:
    //asio context handles data transfer
    asio::io_context m_context;
    //...but it needs a thread of its own to execute its work commands
    std::thread m_thrContext;
    //hardware socket that is connected to the server
    asio::ip::tcp::socket m_socket;
    //The client has a single instance of a "connection" object. which handles data transfer
    std::unique_ptr<Connection> m_connection;
    //Vector containing list of files in it root directory
    std::vector<std::string> m_vectorOfFiles;
};

#endif // CLIENT_H
