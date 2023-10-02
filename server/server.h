#ifndef SERVER_H
#define SERVER_H

// Where is all headers???
// boost used here, how is it possible?

class Connection
    : public std::enable_shared_from_this<Connection>
{
public:
    using TcpSocket = boost::asio::ip::tcp::socket;

    Connection(TcpSocket t_socket);

    void start()
    {
        doFileListRead();
    }

    void handleFileList(size_t bytes);
    void requestFile();
    std::atomic<bool>& getBoolSent();

private:
    void doRead();
    void processRead(size_t t_bytesTransferred);
    void createFile();
    void readData(std::istream &stream);
    void doReadFileContent(size_t t_bytesTransferred);
    void handleError(std::string const& t_functionName, boost::system::error_code const& t_ec);
    void doFileListRead();

    TcpSocket m_socket;
    enum { MaxLength = 40960 }; // What is this?
    std::array<char, MaxLength> m_buf;
    boost::asio::streambuf m_requestBuf;
    std::ofstream m_outputFile; // Should it be member of a class?
    size_t m_fileSize;
    std::string m_fileName;
    std::string m_fileList; // list of file in string? vector or hash set of strings would be better
};


class Server
{
public:
    // Great!!!
    using TcpSocket = boost::asio::ip::tcp::socket;
    using TcpAcceptor = boost::asio::ip::tcp::acceptor;
    using TcpEndPoint = boost::asio::ip::tcp::endpoint;
    using TcpIp = boost::asio::ip::tcp;
    using IoContext = boost::asio::io_context;
    using Strand = boost::asio::strand<IoContext::executor_type>;


    Server(IoContext& t_IoContext);
    void doFileRequestAccept();

private:
    void doAccept();
    void createWorkDirectory();

    // Naming is good!
    TcpSocket m_socket; // accept socket
    TcpSocket m_fileSocket;

    TcpAcceptor m_acceptor;
    TcpAcceptor m_fileRequestAcceptor;
    
    IoContext& m_ioContext;
    std::string m_workDirectory; // I don't see need to store it here
    Strand strand;
};

#endif // !SERVER_H
