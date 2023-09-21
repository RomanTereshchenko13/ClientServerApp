#pragma once

#include <array>
#include <fstream>
#include <string>
#include <memory>
#include <boost/asio.hpp>
#include <filesystem>



class Connection
    : public std::enable_shared_from_this<Connection>
{
public:
    using TcpSocket = boost::asio::ip::tcp::socket;

    Connection(TcpSocket t_socket);

    void start()
    {
        doRead();
    }

    void handleFileList(size_t bytes);

private:
    void doRead();
    void processRead(size_t t_bytesTransferred);
    void createFile();
    void readData(std::istream &stream);
    void doReadFileContent(size_t t_bytesTransferred);
    void handleError(std::string const& t_functionName, boost::system::error_code const& t_ec);
    void requestSpecificFile(const std::string& fileList);
    void readUntilFileListDelimiter();

    TcpSocket m_socket;
    enum { MaxLength = 40960 };
    std::array<char, MaxLength> m_buf;
    boost::asio::streambuf m_requestBuf_;
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
    using IoContext = boost::asio::io_context;

    Server(IoContext& t_IoContext, short t_port, std::string const& t_workDirectory);

private:
    void doAccept();
    void createWorkDirectory();

    TcpSocket m_socket;
    TcpAcceptor m_acceptor;

    std::string m_workDirectory;
};