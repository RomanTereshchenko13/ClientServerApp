#include"Server.h"

#define PORT 7500

int main()
{
    Server server(PORT);
    if(server.Start())
    {
        while(true)
        {
            server.Update();
        }
    }

    return 0;
}