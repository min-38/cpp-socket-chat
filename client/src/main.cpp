#include "common/pch.h"
#include "ui/ChatClient.h"

int main()
{
    try
    {
        ChatClient client;
        client.Run();
    }
    catch (const std::exception& e)
    {
        std::cerr << "예외 발생: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
