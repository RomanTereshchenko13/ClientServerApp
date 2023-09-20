#ifndef CONNECTION_H
#define CONNECTION_H

#include"pch.h"

class Connection : public std::enable_shared_from_this<Connection>
{
public:
    enum class Owner
    {
        server,
        client
    };

public:
    Connection(Owner parent, asio::io_context& asioContext, asio::ip::tcp::socket socket);
    void ConnectToClient();
    void ConnectToServer(const asio::ip::tcp::resolver::results_type& endpoints);
    void Disconnect();
    bool IsConnected() const;

    void Send(const std::string& msg);
    void Write();
    /// @brief 
    void Read();

private:
    //Each connection has unique socket to a remote
    boost::asio::ip::tcp::socket m_socket;
    //This context is shared with whole asio instance
    boost::asio::io_context& m_asioContext;
    std::string msgOut;
    std::string msgIn;
    Owner m_nOwner = Owner::server;
};

#endif // !NET_CONNECTION_H

