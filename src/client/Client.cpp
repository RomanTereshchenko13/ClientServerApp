#include "Client.h"

Client::Client() : m_socket(m_context)
{
    FillVector();
    std::cout << "Client created!\n!";
}

Client::~Client()
{
    Disconnect();
}

void Client::Connect(std::string host, const uint16_t port)
{
    //Resolve hostname/ip-address into tangiable physical address
    asio::ip::tcp::resolver resolver(m_context);
    asio::ip::tcp::resolver::results_type endpoints = resolver.resolve(host, std::to_string(port));

    //Create connection
    m_connection = std::make_unique<Connection>(m_context, asio::ip::tcp::socket(m_context));

    //Tell connection object to connect to server
    m_connection->ConnectToServer(endpoints);

    // Start Context Thread
	m_thrContext = std::thread([this]() { m_context.run(); });

    if(m_connection->IsConnected())
    {
        std::cout << "Connected!\n";
    }
    else
    {
        std::cout << "Error, can`t connect!\n";
    }

    //Serialize the list of files
    std::string serializedData = srlz::serialize(m_vectorOfFiles);

    //Send serialized data to server
    m_connection->Send(serializedData);
}

void Client::Disconnect()
{
    // If connection exists, and it's connected then...
	if(IsConnected())
	{
	// ...disconnect from server gracefully
		m_connection->Disconnect();
    }

	// Either way, we're also done with the asio context...				
	m_context.stop();
	// ...and its thread
	if (m_thrContext.joinable())
		m_thrContext.join();

	// Destroy the connection object
	m_connection.release();
}

bool Client::IsConnected()
{
    if(m_connection)
        return m_connection->IsConnected();
    else 
        return false;
}

void Client::FillVector()
{
    std::string pathToFiles = fs::current_path().string() + "/files";
    try
    {
        for (const auto& entry : fs::directory_iterator(pathToFiles))
        {
            if(entry.is_regular_file())
            {
                m_vectorOfFiles.push_back(entry.path().filename());
            }
        }
    }
    catch(const std::exception& ec)
    {
        std::cerr << "Error during creation of list of files: " << ec.what() << '\n';
    }
    
}
