#include <iostream>

#include <boost/asio/read_until.hpp>
#include <boost/log/trivial.hpp>
#include "server.h"


Connection::Connection(TcpSocket t_socket)
    : m_socket(std::move(t_socket))
{
}


void Connection::doRead()
{
    readUntilFileListDelimiter();
    // auto self = shared_from_this();
    // async_read_until(m_socket, m_requestBuf_, "\n\n",
    //     [this, self](boost::system::error_code ec, size_t bytes)
    //     {
    //         if (!ec)
    //             processRead(bytes);
    //         else
    //             handleError(__FUNCTION__, ec);
    //     });
}

void Connection::processRead(size_t t_bytesTransferred)
{
   BOOST_LOG_TRIVIAL(trace) << __FUNCTION__ << "(" << t_bytesTransferred << ")" 
        << ", in_avail = " << m_requestBuf_.in_avail() << ", size = " 
        << m_requestBuf_.size() << ", max_size = " << m_requestBuf_.max_size() << ".";

    std::istream requestStream(&m_requestBuf_);
    readData(requestStream);

    auto pos = m_fileName.find_last_of('\\');
    if (pos != std::string::npos)
        m_fileName = m_fileName.substr(pos + 1);

    createFile();

    // write extra bytes to file
    do {
        requestStream.read(m_buf.data(), m_buf.size());
        BOOST_LOG_TRIVIAL(trace) << __FUNCTION__ << " write " << requestStream.gcount() << " bytes.";
        m_outputFile.write(m_buf.data(), requestStream.gcount());
    } while (requestStream.gcount() > 0);

    auto self = shared_from_this();
    m_socket.async_read_some(boost::asio::buffer(m_buf.data(), m_buf.size()),
        [this, self](boost::system::error_code ec, size_t bytes)
        {
            if (!ec)
                doReadFileContent(bytes);
            else
                handleError(__FUNCTION__, ec);
        }
    );
}


void Connection::readData(std::istream &stream)
{
    stream >> m_fileName;
    stream >> m_fileSize;
    stream.read(m_buf.data(), 2);

    BOOST_LOG_TRIVIAL(trace) << m_fileName << " size is " << m_fileSize
        << ", tellg = " << stream.tellg();
}


void Connection::createFile()
{
    m_outputFile.open(m_fileName, std::ios_base::binary);
    if (!m_outputFile) {
        BOOST_LOG_TRIVIAL(error) << __LINE__ << ": Failed to create: " << m_fileName;
        return;
    }
}


void Connection::doReadFileContent(size_t t_bytesTransferred)
{
    if (t_bytesTransferred > 0) {
        m_outputFile.write(m_buf.data(), static_cast<std::streamsize>(t_bytesTransferred));

        BOOST_LOG_TRIVIAL(trace) << __FUNCTION__ << " recv " << m_outputFile.tellp() << " bytes";

        if (m_outputFile.tellp() >= static_cast<std::streamsize>(m_fileSize)) {
            std::cout << "Received file: " << m_fileName << std::endl;
            return;
        }
    }
    auto self = shared_from_this();
    m_socket.async_read_some(boost::asio::buffer(m_buf.data(), m_buf.size()),
        [this, self](boost::system::error_code ec, size_t bytes)
        {
            doReadFileContent(bytes);
        });
}


void Connection::handleError(std::string const& t_functionName, boost::system::error_code const& t_ec)
{
    BOOST_LOG_TRIVIAL(error) << __FUNCTION__ << " in " << t_functionName << " due to " 
        << t_ec << " " << t_ec.message() << std::endl;
}


Server::Server(IoContext& t_IoContext, short t_port, std::string const& t_workDirectory)
    : m_socket(t_IoContext),
    m_acceptor(t_IoContext, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), t_port)),
    m_workDirectory(t_workDirectory)
{
    std::cout << "Server started\n";

    createWorkDirectory();

    doAccept();
}


void Server::doAccept()
{
    m_acceptor.async_accept(m_socket,
        [this](boost::system::error_code ec)
    {
        if (!ec)
            std::make_shared<Connection>(std::move(m_socket))->start();

        doAccept();
    });
}

void Server::createWorkDirectory()
{
    namespace fs = std::filesystem;
    auto currentPath = fs::path(m_workDirectory);
    if (!exists(currentPath) && !create_directory(currentPath))
        BOOST_LOG_TRIVIAL(error) << "Coudn't create working directory: " << m_workDirectory;
    current_path(currentPath);
}


void Connection::readUntilFileListDelimiter()
{
    auto self = shared_from_this();
    boost::asio::async_read_until(m_socket, m_requestBuf_, "\n\n",
        [this, self](boost::system::error_code ec, size_t bytes)
        {
            if (!ec) {
                handleFileList(bytes);
            } else {
                handleError(__FUNCTION__, ec); 
            }
        });
}

void Connection::handleFileList(size_t bytes)
{
    BOOST_LOG_TRIVIAL(trace) << __FUNCTION__ << "(" << bytes << ")" 
    << ", in_avail = " << m_requestBuf_.in_avail() << ", size = " 
    << m_requestBuf_.size() << ", max_size = " << m_requestBuf_.max_size() << ".";
    std::istream requestStream(&m_requestBuf_);
    std::string line;
    while (std::getline(requestStream, line)) {
        if (line.empty()) {
            break;
        }
        std::cout << "List of files received: " << line << std::endl;
    }

    requestSpecificFile(line);

}

void Connection::requestSpecificFile(const std::string& fileList)
{
    std::cout << "Enter the name of the file you wish to request: ";
    std::string fileName;
    std::cin >> fileName;

    boost::asio::write(m_socket, boost::asio::buffer(fileName + "\n"));
}
