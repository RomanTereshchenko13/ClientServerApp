
#include "server.h"

#include"../shared/pch.h"

Connection::Connection(TcpSocket t_socket)
    : m_socket(std::move(t_socket))
{
}

void Connection::doRead()
{
    // Send the request to the client.
    auto self = shared_from_this();
    async_read_until(m_socket, m_requestBuf, "\n\n",
        [this, self](boost::system::error_code ec, size_t bytes)
        {
            if (!ec)
                processRead(bytes);
            else
                handleError(__FUNCTION__, ec);
        });
}

void Connection::doFileListRead()
{
    auto self = shared_from_this();
    boost::asio::async_read_until(m_socket, m_requestBuf, "\n\n",
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
    << ", in_avail = " << m_requestBuf.in_avail() << ", size = " 
    << m_requestBuf.size() << ", max_size = " << m_requestBuf.max_size() << ".";
    std::string fileList;
    std::istream requestStream(&m_requestBuf);
    while (std::getline(requestStream, fileList, '\t')) 
    {
        m_fileList += fileList + '\n';
        BOOST_LOG_TRIVIAL(trace) << "Read line: " << fileList;
        if (fileList.empty()) {
            break;
        }
    }
    
    std::cout << "List of files received: " << m_fileList << std::endl;
}

void Connection::processRead(size_t t_bytesTransferred)
{
   BOOST_LOG_TRIVIAL(trace) << __FUNCTION__ << "(" << t_bytesTransferred << ")" 
        << ", in_avail = " << m_requestBuf.in_avail() << ", size = " 
        << m_requestBuf.size() << ", max_size = " << m_requestBuf.max_size() << ".";

    std::istream requestStream(&m_requestBuf);
    readData(requestStream);

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

Server::Server(IoContext& t_IoContext)
    : m_socket(t_IoContext), m_ioContext(t_IoContext), m_fileSocket(t_IoContext),
    m_acceptor(t_IoContext, TcpEndPoint(boost::asio::ip::tcp::v4(), 7500)),
    m_fileRequestAcceptor(t_IoContext, TcpEndPoint(boost::asio::ip::tcp::v4(), 7505))
{
    std::cout << "Server started\n";

    createWorkDirectory();
    doAccept();
    doFileRequestAccept();
}

//1
void Server::createWorkDirectory()
{
    namespace fs = std::filesystem;
    m_workDirectory = fs::current_path().parent_path().string() + "/server/files"; 
    auto currentPath = fs::path(m_workDirectory);
    if (!exists(currentPath) && !create_directory(currentPath))
        BOOST_LOG_TRIVIAL(error) << "Coudn't create working directory: " << m_workDirectory;
    current_path(currentPath);
}

void Connection::requestSpecificFile()
{
    std::string name;
    std::cout << "Enter the name of the file to request: ";
    std::cin >> name;
    m_fileName = name;
    // Send the request to the client.
    boost::asio::async_write(m_socket,
    boost::asio::buffer(name + "\n\n"),
    [this](boost::system::error_code ec, size_t /*length*/)
    {
        if (!ec) 
        {
        } 
        else 
        {
            handleError(__FUNCTION__, ec);
        }
    });

    doRead();
}

//Server methods
void Server::doAccept()
{
    m_acceptor.async_accept(m_socket,
        [this](boost::system::error_code ec)
    {
        BOOST_LOG_TRIVIAL(trace) << "Accepted client: " << m_socket.remote_endpoint() << " on :" << m_socket.local_endpoint().port();
        if (!ec)
            std::make_shared<Connection>(std::move(m_socket))->start();
        doAccept();
    });
}

void Server::doFileRequestAccept()
{
    m_fileRequestAcceptor.async_accept(m_fileSocket, 
        [this](boost::system::error_code ec) 
        {
            BOOST_LOG_TRIVIAL(trace) << "Accepted client: " << m_fileSocket.remote_endpoint() << " on :" << m_fileSocket.local_endpoint().port();
            if (!ec)
                std::make_shared<Connection>(std::move(m_fileSocket))->requestSpecificFile();
            doFileRequestAccept();
        });
}
