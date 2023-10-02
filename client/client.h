#ifndef CLIENT_H
#define CLIENT_H

namespace fs = std::filesystem; // Will be public
// It is better to use such in cpp files, while std::filesystem used in headers

class Client
{
public:
    using IoContext = boost::asio::io_context;
    using TcpResolver = boost::asio::ip::tcp::resolver;
    using TcpResolverIterator = TcpResolver::iterator;
    using TcpSocket = boost::asio::ip::tcp::socket;

    Client(IoContext& t_IoContext, TcpResolverIterator t_mainEndpointIterator, 
    TcpResolverIterator t_fileEndpointIterator, std::string const& t_path);
    void sendList();
    void handleServerRequest();
    // Good incapsulation
private:
    std::string generateFileList();
    void waitForServerRequest();
    void openFile(std::string const& t_path);
    void doConnect();
    void doFileTransferConnect();
    void doWriteFile(const boost::system::error_code& t_ec);
    
    template<typename Buffer>
    void writeBuffer(Buffer& t_buffer);


    TcpResolver m_TcpResolver;
    IoContext& m_IoContext;
    TcpSocket m_mainSocket;
    TcpSocket m_fileSocket;
    TcpResolverIterator m_mainEndpointIterator;
    TcpResolverIterator m_fileEndpointIterator;
    enum { MessageSize = 1024 };
    std::array<char, MessageSize> m_buf;
    boost::asio::streambuf m_request;
    boost::asio::streambuf m_fileRequestBuf;
    std::ifstream m_sourceFile; // Shouldn't be a class member
    std::string m_path;
    std::string m_listOfFiles;
    std::string pathToFiles;
};

template<typename Buffer>
void Client::writeBuffer(Buffer& t_buffer)
{
    boost::asio::async_write(m_fileSocket,
        t_buffer,
        [this](boost::system::error_code ec, size_t /*length*/)
        {
            doWriteFile(ec);
        });
}

#endif // !CLIENT_H
