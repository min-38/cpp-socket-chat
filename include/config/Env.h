#pragma once

#include <string>
#include <unordered_map>

class Env
{
public:
    static Env& getInstance();

    std::string get(const std::string& key);

    Env(const Env&) = delete;
    Env& operator=(const Env&) = delete;
    Env(Env&&) = delete;
    Env& operator=(Env&&) = delete;

private:
    Env();

    void load();

    const std::string envPath = ".env";

    std::unordered_map<std::string, std::string> m_envMap;
};
