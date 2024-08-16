/**
  ******************************************************************************
  * @file           : log.cpp
  * @author         : toastoffee
  * @brief          : None
  * @attention      : None
  * @date           : 2024/8/15
  ******************************************************************************
  */



#include <cstdio>
#include <iostream>

#include "log.h"


void log(LogLevel logLevel, const char *fileName, int line, const char *format, va_list args) {

    char desc[128];
    vsnprintf(desc, sizeof(desc), format, args);

    char logContent[512];
    snprintf(logContent, sizeof(logContent), "[%s] - %s:line %d - [%s] %s\n",
             getCurrentTimeStamp().c_str(), fileName, line, convertLogLevelToStr(logLevel), desc);

    va_end(args);

    std::cout << logContent;
}

std::string getCurrentTimeStamp(){

    std::chrono::system_clock::time_point now = std::chrono::system_clock::now();

    std::time_t now_time_t = std::chrono::system_clock::to_time_t(now);
    std::tm* now_tm = std::localtime(&now_time_t);

    char buffer[128] = {0};
    strftime(buffer, sizeof(buffer), "%F %T:", now_tm);

    const auto duration_in_millis = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;
    const int milliseconds = duration_in_millis.count();

    std::string msStr = std::to_string(milliseconds);
    strcpy(buffer + strlen(buffer), msStr.c_str());

    return buffer;
}

const char* convertLogLevelToStr(const LogLevel &logLevel){
    switch (logLevel) {
        case LogLevel::Fatal:
            return "Fatal";
            break;
        case LogLevel::Critical:
            return "Critical";
            break;
        case LogLevel::Error:
            return "Error";
            break;
        case LogLevel::Warn:
            return "Warn";
            break;
        case LogLevel::Info:
            return "Info";
            break;
        case LogLevel::Debug:
            return "Debug";
            break;
    }
}

void Fatal(const char *fileName, int line, const char *format, ...) {
    va_list args;
    va_start(args, format);

    log(LogLevel::Fatal, fileName, line, format, args);

    va_end(args);
}

void Critical(const char *fileName, int line, const char *format, ...) {
    va_list args;
    va_start(args, format);

    log(LogLevel::Critical, fileName, line, format, args);

    va_end(args);
}

void Error(const char *fileName, int line, const char *format, ...) {
    va_list args;
    va_start(args, format);

    log(LogLevel::Error, fileName, line, format, args);

    va_end(args);
}

void Warn(const char *fileName, int line, const char *format, ...) {
    va_list args;
    va_start(args, format);

    log(LogLevel::Warn, fileName, line, format, args);

    va_end(args);
}

void Info(const char *fileName, int line, const char *format, ...) {
    va_list args;
    va_start(args, format);

    log(LogLevel::Info, fileName, line, format, args);

    va_end(args);
}

void Debug(const char *fileName, int line, const char *format, ...) {
    va_list args;
    va_start(args, format);

    log(LogLevel::Debug, fileName, line, format, args);

    va_end(args);
}