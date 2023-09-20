#include"Client.h"

#define PORT 7500

int main(int argc, char* argv[])
{
    if (argc != 2) 
    {
        std::cerr << "Usage: " << argv[0] << " <server_ip>\n";
        return 1;
    }

    Client client;
    client.Connect(argv[1], PORT);
}