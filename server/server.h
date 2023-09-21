#ifndef SERVER_H
#define SERVER_H

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
    void requestSpecificFile();

private:
    void doRead();
    void processRead(size_t t_bytesTransferred);
    void createFile();
    void readData(std::istream &stream);
    void doReadFileContent(size_t t_bytesTransferred);
    void handleError(std::string const& t_functionName, boost::system::error_code const& t_ec);
    void doFileListRead();

    TcpSocket m_socket;
    enum { MaxLength = 40960 };
    std::array<char, MaxLength> m_buf;
    boost::asio::streambuf m_requestBuf;
    std::ofstream m_outputFile;
    size_t m_fileSize;
    std::string m_fileName;
    std::string m_fileList;
};


class Server
{
public:
    using TcpSocket = boost::asio::ip::tcp::socket;
    using TcpAcceptor = boost::asio::ip::tcp::acceptor;
    using TcpEndPoint = boost::asio::ip::tcp::endpoint;
    using IoContext = boost::asio::io_context;

    Server(IoContext& t_IoContext);
    void doFileRequestAccept();

private:
    void doAccept();
    void createWorkDirectory();

    TcpSocket m_socket;
    TcpSocket m_fileSocket;
    TcpAcceptor m_acceptor;
    TcpAcceptor m_fileRequestAcceptor;
    IoContext& m_ioContext;
    std::string m_workDirectory;
};

#endif // !SERVER_H
