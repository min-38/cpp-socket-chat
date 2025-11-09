#pragma once

#include <string>
#include <fstream>

enum class LogLevel { INFO, WARN, ERROR };

class Logger {
public:
    // 복사, 이동 금지
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;
    Logger(Logger&&) = delete;
    Logger& operator=(Logger&&) = delete;

    static void init();

    static void info(const std::string &msg);
    static void warn(const std::string &msg);
    static void error(const std::string &msg);

private:
    Logger()    = default;
    ~Logger()   = default;

    static void log(LogLevel level, const std::string &msg);

private:
    static std::ofstream s_logfile;
    static bool s_useFile;
};
