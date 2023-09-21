#include <string>
#include <iostream>

#include <boost/log/trivial.hpp>

#include "client.h"

Client::Client(IoContext& t_IoContext, TcpResolverIterator t_endpointIterator, 
    std::string const& t_path)
    : m_TcpResolver(t_IoContext), m_mainSocket(t_IoContext), m_fileSocket(t_IoContext),
    m_endpointIterator(t_endpointIterator), m_path(t_path)
{
    doConnect();
}

//1
void Client::doConnect()
{
    boost::asio::async_connect(m_mainSocket, m_endpointIterator, 
        [this](boost::system::error_code ec, TcpResolverIterator)
        {
            if (!ec) {
                sendList();
                BOOST_LOG_TRIVIAL(info) << "Connected to server...";
            } else {
                std::cout << "Coudn't connect to host. Please run server "
                    "or check network connection.\n";
                BOOST_LOG_TRIVIAL(error) << "Error: " << ec.message();
            }
        });
}

//2
void Client::sendList()
{
    m_listOfFiles = generateFileList(); // Assuming m_path is the directory path
    std::ostream os(&m_request);
    os << m_listOfFiles;
    os << m_listOfFiles << "\n" << m_listOfFiles.size() << "\n\n";
    BOOST_LOG_TRIVIAL(trace) << "Request size: " << m_request.size();
    boost::asio::async_write(m_mainSocket, boost::asio::buffer(m_listOfFiles.data(), m_listOfFiles.size()),
    [this](boost::system::error_code ec, size_t /*length*/) {
        if (!ec) 
        {
            // Successful write
            BOOST_LOG_TRIVIAL(info) << "Send list of files:\n" << m_listOfFiles;
        } else {
            // Handle error
            BOOST_LOG_TRIVIAL(error) << "Error sending list of files: " << ec.message();
        }
    });
}

//3
std::string Client::generateFileList() {
    std::stringstream fileList;
    std::string pathToFiles = fs::current_path().string() + "/files_to_send";
    try
    {
        for (const auto& entry : fs::directory_iterator(pathToFiles)) 
        {
            if (entry.is_regular_file()) 
            {
                fileList << entry.path().filename().string() + "\n";
            }
        }
    }
    catch(const std::exception& ec)
    {
        std::cerr << "Error during creation of list of files: " << ec.what() << '\n';
    }
    fileList << "\n\n";
    return fileList.str();
}

//4
// Wait for the server's request for a specific file
void Client::waitForServerRequest() {
    boost::asio::async_read(m_mainSocket,
        boost::asio::buffer(m_buf),
        [this](boost::system::error_code ec, size_t /*length*/)
        {
            if (!ec) {
                std::string requestedFile = std::string(m_buf.data());
                std::cout << requestedFile << "Requested\n";
                // Send the requested file to the server on a different port
                openFile(requestedFile);
                writeBuffer(m_request);
            } else {
                BOOST_LOG_TRIVIAL(error) << "Error: " << ec.message();
            }
        }
    );
}

//5
void Client::openFile(std::string const &t_path)
{
    m_sourceFile.open(t_path, std::ios_base::binary | std::ios_base::ate);
    if (m_sourceFile.fail())
        throw std::fstream::failure("Failed while opening file " + t_path);
    
    m_sourceFile.seekg(0, m_sourceFile.end);
    auto fileSize = m_sourceFile.tellg();
    m_sourceFile.seekg(0, m_sourceFile.beg);

    std::ostream requestStream(&m_request);
    std::filesystem::path p(t_path);
    requestStream << p.filename().string() << "\n" << fileSize << "\n\n";
    BOOST_LOG_TRIVIAL(trace) << "Request size: " << m_request.size();
}

//6
void Client::doWriteFile(const boost::system::error_code& t_ec)
{
    if (!t_ec) {
        if (m_sourceFile) {
            m_sourceFile.read(m_buf.data(), m_buf.size()); 
            if (m_sourceFile.fail() && !m_sourceFile.eof()) {
                auto msg = "Failed while reading file";
                BOOST_LOG_TRIVIAL(error) << msg;
                throw std::fstream::failure(msg);
            }
            std::stringstream ss;
            ss << "Send " << m_sourceFile.gcount() << " bytes, total: "
                << m_sourceFile.tellg() << " bytes";
            BOOST_LOG_TRIVIAL(trace) << ss.str();
            std::cout << ss.str() << std::endl;

            auto buf = boost::asio::buffer(m_buf.data(), static_cast<size_t>(m_sourceFile.gcount()));
            writeBuffer(buf);
        }
    } else {
        BOOST_LOG_TRIVIAL(error) << "Error: " << t_ec.message();
    }
}