#include "common/Utils.h"
#include <random>

namespace utils
{
    std::string generateRandomCode(int length)
    {
        static const char chars[] =
            "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
            "abcdefghijklmnopqrstuvwxyz"
            "0123456789";

        static std::random_device rd;
        static std::mt19937 gen(rd());
        static std::uniform_int_distribution<> dis(0, sizeof(chars) - 2);

        std::string code;
        code.reserve(length);

        for (int i = 0; i < length; ++i)
            code += chars[dis(gen)];

        return code;
    }
}