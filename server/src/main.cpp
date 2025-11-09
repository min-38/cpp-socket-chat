#include "application/Server.h"
#include "logger/Logger.h"

int main()
{
    try
    {
        Server server;
        server.run();
    }
    catch (std::exception& e)
    {
        Logger::warn(e.what());
    }

    return 0;
}
