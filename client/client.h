#ifndef CLIENT_H
#define CLIENT_H

#include <array>
#include <fstream>
#include <filesystem>
#include <boost/asio.hpp>

namespace fs = std::filesystem;

class Client
{
public:
    using IoContext = boost::asio::io_context;
    using TcpResolver = boost::asio::ip::tcp::resolver;
    using TcpResolverIterator = TcpResolver::iterator;
    using TcpSocket = boost::asio::ip::tcp::socket;

    Client(IoContext& t_IoContext, TcpResolverIterator t_endpointIterator, 
        std::string const& t_path);
    void sendList();
    void handleServerRequest();
private:
    std::string generateFileList();
    void waitForServerRequest();
    void openFile(std::string const& t_path);
    void doConnect();
    void doWriteFile(const boost::system::error_code& t_ec);
    
    template<typename Buffer>
    void writeBuffer(Buffer& t_buffer);


    TcpResolver m_TcpResolver;
    TcpSocket m_mainSocket;
    TcpSocket m_fileSocket;
    TcpResolverIterator m_endpointIterator;
    enum { MessageSize = 1024 };
    std::array<char, MessageSize> m_buf;
    boost::asio::streambuf m_request;
    std::ifstream m_sourceFile;
    std::string m_path;
    std::string m_listOfFiles;
};

template<typename Buffer>
void Client::writeBuffer(Buffer& t_buffer)
{
    boost::asio::async_write(m_mainSocket,
        t_buffer,
        [this](boost::system::error_code ec, size_t /*length*/)
        {
            doWriteFile(ec);
        });
}

#endif // !CLIENT_H
