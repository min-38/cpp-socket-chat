#include "common/pch.h"
#include "ui/Lobby.h"

int main()
{
    try
    {
        Lobby lobby;
        lobby.Run();
    }
    catch (const std::exception& e)
    {
        std::cerr << "예외 발생: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
