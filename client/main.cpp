#include <iostream>

#include <boost/asio/io_context.hpp>

#include "client.h"
#include "../shared/pch.h"
#include "../shared/logger.h"

#include <thread>
#include <chrono>


int main(int argc, char* argv[])
{
    if (argc != 3) {
        std::cerr << "Usage: client <address> <port>\n";
        return 1;
    }

    Logger::instance().setOptions("client_%3N.log", 1 * 1024 * 1024, 10 * 1024 * 1024);

    auto address = argv[1];
    auto port = argv[2];
    auto filePath = "files_to_send/helloworld.txt";

    try {
        boost::asio::io_context IoContext;

        boost::asio::ip::tcp::resolver resolver(IoContext);
        auto endpointIterator = resolver.resolve({ address, port });
        Client client(IoContext, endpointIterator, filePath);
        std::this_thread::sleep_for(std::chrono::seconds(5));
        IoContext.run();
    } catch (std::fstream::failure& e) {
        std::cerr << e.what() << "\n";
    } catch (std::exception& e) {
        std::cerr << "Exception: " << e.what() << "\n";
    }

    return 0;
}