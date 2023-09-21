#include"../shared/pch.h"

#include "client.h"

Client::Client(IoContext& t_IoContext, TcpResolverIterator t_mainEndpointIterator, TcpResolverIterator t_fileEndpointIterator,
    std::string const& t_path)
    : m_TcpResolver(t_IoContext), m_IoContext(t_IoContext), m_mainSocket(t_IoContext), m_fileSocket(t_IoContext),
    m_mainEndpointIterator(t_mainEndpointIterator), m_fileEndpointIterator(t_fileEndpointIterator), m_path(t_path)
{
    pathToFiles = fs::current_path().parent_path().string() + "/client/files";
    doConnect();
}

//1
void Client::doConnect()
{
    boost::asio::async_connect(m_mainSocket, m_mainEndpointIterator, 
        [this](boost::system::error_code ec, TcpResolverIterator)
        {
            if (!ec) {
                sendList();
                BOOST_LOG_TRIVIAL(info) << "[Port: 7500] Connected to server...";
            } else {
                std::cout << "[Port: 7500]  Coudn't connect to host. Please run server "
                    "or check network connection.\n";
                BOOST_LOG_TRIVIAL(error) << "Error: " << ec.message();
            }
        });
}
//2
void Client::doFileTransferConnect()
{
    boost::asio::async_connect(m_fileSocket, m_fileEndpointIterator,
        [this](boost::system::error_code ec, TcpResolverIterator)
        {
            if (!ec) {
                waitForServerRequest();
                BOOST_LOG_TRIVIAL(info) << "[Port: 7505] Connected to server...";
            } else {
                std::cout << "[Port: 7505] Coudn't connect to host. Please run server "
                    "or check network connection.\n";
                BOOST_LOG_TRIVIAL(error) << "Error: " << ec.message();
            }
        });
}

//3
void Client::sendList()
{
    m_listOfFiles = generateFileList();
    std::ostream os(&m_request);
    os << m_listOfFiles;
    os << m_listOfFiles << "\n" << m_listOfFiles.size();
    BOOST_LOG_TRIVIAL(trace) << "Request size: " << m_request.size();
    boost::asio::async_write(m_mainSocket, boost::asio::buffer(m_listOfFiles.data(), m_listOfFiles.size()),
    [this](boost::system::error_code ec, size_t /*length*/) {
        if (!ec) 
        {
            BOOST_LOG_TRIVIAL(info) << "[Port: 7500] Sent list of files:\n" << m_listOfFiles;
        } else {
            BOOST_LOG_TRIVIAL(error) << "[Port: 7505] Error sending list of files: " << ec.message();
        }
    });
    doFileTransferConnect();
}

//4
std::string Client::generateFileList() {
    std::stringstream fileList;
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

//5
// Wait for the server's request for a specific file
void Client::waitForServerRequest() {
    boost::asio::async_read_until(m_fileSocket, m_fileRequestBuf, "\n",
        [this](boost::system::error_code ec, size_t /*length*/)
        {
            if (!ec) {
                std::istream is(&m_fileRequestBuf);
                std::string requestedFile;
                std::getline(is, requestedFile, '\n');
                std::cout << "Requested file: " << requestedFile << std::endl;
                
                std::string fullPath = pathToFiles + "/" +  requestedFile;
                //Begin working with file
                writeBuffer(m_request);
                openFile(fullPath);
            } else {
                BOOST_LOG_TRIVIAL(error) << "Error: " << ec.message();
            }
        }
    );
}

//6
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
    BOOST_LOG_TRIVIAL(trace) << "[Port: 7505] Request size: " << m_request.size();
}

//7
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
            ss << "[Port: 7505] Send " << m_sourceFile.gcount() << " bytes, total: "
                << m_sourceFile.tellg() << " bytes";
            BOOST_LOG_TRIVIAL(trace) << ss.str();
            std::cout << ss.str() << std::endl;

            auto buf = boost::asio::buffer(m_buf.data(), static_cast<size_t>(m_sourceFile.gcount()));
            writeBuffer(buf);
        }
    } else {
        BOOST_LOG_TRIVIAL(error) << "[Port: 7505] Error: " << t_ec.message();
    }
}