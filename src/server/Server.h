#ifndef SERVER_H
#define SERVER_H

#include"../shared/Connection.h"
#include"../shared/Serialize.h"

class Server 
{
public:
    Server(uint16_t port);
    ~Server();

    bool Start();
    void WaitForClientConnection();
    void Update(size_t nMaxMessages = -1);
    void Stop();
    void MessageClient(std::shared_ptr<Connection> client, const std::string& msg);
   
private:
    asio::io_context m_asioContext;

    std::shared_ptr<Connection> m_connection;

    std::thread m_threadContext;
    std::mutex m_mtx;
    
    asio::ip::tcp::acceptor m_asioAcceptor;

    std::deque<std::string> m_qMsgIn;
};

#endif // !SERVER_H
