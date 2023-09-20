#include "Server.h"

Server::Server(uint16_t port)
    : m_asioAcceptor(m_asioContext, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port))
{}

Server::~Server()
{
    Stop();
}

bool Server::Start()
{
    try
    {
        WaitForClientConnection();
        m_threadContext = std::thread([this]() { m_asioContext.run(); });
    }
    catch(const std::exception& e)
    {
        std::cerr << "[SERVER] Exception: " << e.what() << '\n';
        return false;
    }

    std::cout << "[SERVER] Started!\n";
    return true;
}

void Server::WaitForClientConnection()
{
    m_asioAcceptor.async_accept(
        [this](std::error_code ec, boost::asio::ip::tcp::socket socket)
        {
            if(!ec)
            {
                std::cout << "[SERVER] New Connection: " << socket.remote_endpoint() << "\n";

                // Create a new connection to handle this client 
                m_connection = 
                    std::make_shared<Connection>(Connection::Owner::server, m_asioContext, std::move(socket));
                
                m_connection->ConnectToClient();

                std::cout << "Connection Approved\n";  
            }
            else
			{
				// Error has occurred during acceptance
				std::cout << "[SERVER] New Connection Error: " << ec.message() << "\n";
			}

            WaitForClientConnection();
        }
    );
}

void Server::Update(size_t nMaxMessages)
{
    size_t nMessageCount = 0;
    while(nMessageCount < nMaxMessages && !m_qMsgIn.empty())
    {
        //Grab the fron message
        std::scoped_lock lock(m_mtx);
        auto msg = m_qMsgIn.front();
        m_qMsgIn.pop_front();

        std::cout << msg << std::endl;

        // //Pass to message handler
        // OnMessage(msg.remote, msg.msg); // - interface for file requesting?

        nMessageCount++;
    }
}

void Server::Stop()
{
    //Request the context to close
    m_asioContext.stop();

    //Tidy up the context thread
    if (m_threadContext.joinable()) m_threadContext.join();

    //Inform someone
    std::cout << "[SERVER] Stopped!\n";
}

void Server::MessageClient(std::shared_ptr<Connection> client, const std::string &msg)
{
    if(client && client->IsConnected())
    {
        client->Send(msg);
    }
}
