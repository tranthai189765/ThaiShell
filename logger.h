#pragma once
#include <string>

enum LogLevel {
    INFO = 0,
    LOG_ERROR = 1,
    WARN = 2,
    SUCCESS = 3,
    PROCESS = 4
};


void log(LogLevel level, const std::string& message);
void printBanner();
