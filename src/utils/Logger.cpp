#include "Logger.h"

#include <iostream>
#include <chrono>
#include <sstream>
#include <iomanip>

std::ofstream Logger::s_logfile;
bool Logger::s_useFile = false;

void Logger::info(const std::string& msg) { log(LogLevel::INFO, msg); }
void Logger::warn(const std::string& msg) { log(LogLevel::WARN, msg); }
void Logger::error(const std::string& msg) { log(LogLevel::ERROR, msg); }

void Logger::log(LogLevel level, const std::string& msg)
{
    auto now = std::chrono::system_clock::now();
    auto t = std::chrono::system_clock::to_time_t(now);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch() % std::chrono::seconds(1)
    ).count();

    std::ostringstream oss;
    oss << std::put_time(std::localtime(&t), "%Y-%m-%d %H:%M:%S")
        << "." << std::setw(3) << std::setfill('0') << ms;
    std::string timestamp = oss.str();

    // 2. 로그 레벨 문자열 선택
    const char* level_str = nullptr;
    switch (level)
    {
        case LogLevel::INFO:  level_str = "INFO ";  break;
        case LogLevel::WARN:  level_str = "WARN ";  break;
        case LogLevel::ERROR: level_str = "ERROR"; break;
    }

    // 3. 최종 로그 메시지 조합
    std::string line = "[" + timestamp + "] [" + level_str + "] " + msg;

    if (level == LogLevel::ERROR)
        std::cerr << line << "\n";
    else
        std::cout << line << "\n";

    if (s_useFile && s_logfile.is_open())
        s_logfile << line << std::endl;
}
