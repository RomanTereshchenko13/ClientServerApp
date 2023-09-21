#include <iostream>

#include <boost/asio/io_context.hpp>

#include "client.h"
#include "../shared/pch.h"
#include "../shared/logger.h"

int main(int argc, char* argv[])
{
    if (argc != 2) {
        std::cerr << "Usage: client <address>\n";
        return 1;
    }

    Logger::instance().setOptions("client_%3N.log", 1 * 1024 * 1024, 10 * 1024 * 1024);

    auto address = argv[1];
    auto filePath = "files_to_send/Raven.txt";

    try {
        boost::asio::io_context IoContext;

        boost::asio::ip::tcp::resolver resolver(IoContext);
        auto mainEndpntIterator = resolver.resolve({ address, "7500" });
        auto fileEndpntIterator = resolver.resolve({ address, "7505" });
        Client client(IoContext, mainEndpntIterator, fileEndpntIterator, filePath);
        IoContext.run();
    } catch (std::fstream::failure& e) {
        std::cerr << e.what() << "\n";
    } catch (std::exception& e) {
        std::cerr << "Exception: " << e.what() << "\n";
    }

    return 0;
}