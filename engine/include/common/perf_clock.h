#ifndef _PERF_CLOCK_H
#define _PERF_CLOCK_H
#define SPDLOG_FMT_EXTERNAL

#include "spdlog/spdlog.h"
#include "spdlog/sinks/daily_file_sink.h"
#include <memory>
#include <chrono>

class LogRunTime
{
public:
    explicit LogRunTime(const std::string& function_name)
        : function_name_(function_name)
        , start_time_(std::chrono::system_clock::now())
    {}

    void log(const std::string& str)
    {
        auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now() - start_time_).count();
        LogRunTime::get_logger()->info("{} {} ran in {}", function_name_, str, elapsed);
    }

protected:
    static spdlog::logger* get_logger()
    {
        static spdlog::logger *curr_logger = new spdlog::logger{"runtime_logger", std::make_shared<spdlog::sinks::daily_file_sink_mt>("runtime.log", 23, 59)};
        return curr_logger;
    }

protected:
    std::string function_name_;
    std::chrono::time_point<std::chrono::system_clock> start_time_;
};

class LogRunTimeRAII : private LogRunTime
{
public:
    explicit LogRunTimeRAII(const std::string& function_name)
        : LogRunTime(function_name)
    {}

    ~LogRunTimeRAII()
    {
        log("");
    }
};

#endif
