#include "Env.h"

#include "Logger.h"
#include <fstream>

Env& Env::getInstance()
{
    static Env instance;
    return instance;
}

Env::Env()
{
    load();
}

void Env::load()
{
    std::ifstream inFile(envPath);

    if (!inFile.is_open())
    {
        Logger::warn("[Env] Warning: Could not open .env file.");
        return;
    }

    std::string line;
    while (std::getline(inFile, line))
    {
        if (!line.empty() && line.back() == '\r')
        {
            line.pop_back();
        }

        if (line.empty() || line[0] == '#')
        {
            continue;
        }

        auto delimiterPos = line.find('=');
        if (delimiterPos == std::string::npos || delimiterPos == 0)
        {
            continue;
        }

        std::string key = line.substr(0, delimiterPos);
        std::string value = line.substr(delimiterPos + 1);
        
        m_envMap[key] = value;
        Logger::info(key + " = " + value);
    }
}

std::string Env::get(const std::string& key)
{
    auto it = m_envMap.find(key);
    if (it != m_envMap.end())
        return it->second;
    return "";
}