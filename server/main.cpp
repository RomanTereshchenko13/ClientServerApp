#include <iostream>

#include "server.h"
#include "../shared/pch.h"
#include "../shared/logger.h"

int main(int argc, char* argv[])
{
    Logger::instance().setOptions("server_%3N.log", 1 * 1024 * 1024, 10 * 1024 * 1024);

    try {
        boost::asio::io_context IoContext;

        Server server(IoContext);

        IoContext.run();

    } catch (std::exception& e) {
        std::cerr << "Exception: " << e.what() << "\n";
    }

    return 0;
}