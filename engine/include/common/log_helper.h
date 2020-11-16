#ifndef _LOG_HELPER_H
#define _LOG_HELPER_H
#define SPDLOG_FMT_EXTERNAL

#include "spdlog/spdlog.h"
#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/sinks/daily_file_sink.h"
#include <string>


class RegisterUnitTestLogger
{
public:
    explicit RegisterUnitTestLogger(const std::string &test_name)
    {
        auto file_logger = spdlog::basic_logger_mt("basic_logger", test_name + "_log.txt", true);
        spdlog::set_default_logger(file_logger);
    }
};

#endif
