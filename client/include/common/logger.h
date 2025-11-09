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
    static void debug(const std::string &msg);  // 디버그 모드에서만 출력

private:
    Logger()    = default;
    ~Logger()   = default;

    static void log(LogLevel level, const std::string &msg);

private:
    static std::ofstream s_logfile;
    static bool s_useFile;
};
