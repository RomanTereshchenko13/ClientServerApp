#include "Connection.h"

Connection::Connection(Owner parent, asio::io_context &asioContext, asio::ip::tcp::socket socket)
: m_asioContext(asioContext), m_socket(std::move(socket))
{
    m_nOwner = parent;
}

void Connection::ConnectToClient()
{
    if(m_socket.is_open())
    {
        Read();
    }
}

void Connection::ConnectToServer(const asio::ip::tcp::resolver::results_type& endpoints)
{
    if(m_nOwner == Owner::client)
    {
        asio::async_connect(m_socket, endpoints,
            [this](std::error_code ec, asio::ip::tcp::endpoint endpoint)
            {
                if(!ec)
                {
                    std::cout << "Succesfully connected to server\n";
                }
                else
                {
                    std::cout << "Fail to connect to server\n";
                    m_socket.close();
                }
            }
        );
    }
}

void Connection::Disconnect()
{
    if(IsConnected())
    {
        boost::asio::post(m_asioContext, [this]() { m_socket.close(); });
    }
}

bool Connection::IsConnected() const
{
    return m_socket.is_open();
}

void Connection::Send(const std::string &msgOut)
{
     asio::async_write(m_socket, asio::buffer(msgOut),
        [this](std::error_code ec, std::size_t length)
        {
            if (!ec)
            {
                std::cout << "List of files succesfully sent\n";
            }
            else
            {
                std::cout << "Fail\n";
				m_socket.close();
            }
        }
     );
}

void Connection::Read()
{
    asio::async_read(m_socket, asio::buffer(msgIn),
        [this](std::error_code ec, size_t length)
        {
            if(!ec)
            {
                std::cout << "File Recieved!\n";
            }
            else
            {
                std::cout << "Error!\n";
            }
        }
    );

}
