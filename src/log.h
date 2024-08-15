/**
  ******************************************************************************
  * @file           : log.h
  * @author         : toastoffee
  * @brief          : None
  * @attention      : None
  * @date           : 2024/8/15
  ******************************************************************************
  */



#ifndef GAMEBOY_EMULATOR_LOG_H
#define GAMEBOY_EMULATOR_LOG_H

#include <cstdarg>

enum class LogLevel : unsigned int {

    Fatal       = 0,
    Critical    = 100,
    Error       = 200,
    Warn        = 300,
    Info        = 400,
    Debug       = 500,
};

void log(LogLevel logLevel, const char* fileName, int line, const char *loggerName, const char *format, va_list args);

#define FATAL(loggerName, format, ...)      log(LogLevel::Fatal, __FILE_NAME__, __LINE__, loggerName, format, ##__VA_ARGS__)
#define CRITICAL(loggerName, format, ...)   log(LogLevel::Critical, __FILE_NAME__, __LINE__, loggerName, format, ##__VA_ARGS__)
#define ERROR(loggerName, format, ...)      log(LogLevel::Error, __FILE_NAME__, __LINE__, loggerName, format, ##__VA_ARGS__)
#define WARN(loggerName, format, ...)       log(LogLevel::Warn, __FILE_NAME__, __LINE__, loggerName, format, ##__VA_ARGS__)
#define INFO(loggerName, format, ...)       log(LogLevel::Info, __FILE_NAME__, __LINE__, loggerName, format, ##__VA_ARGS__)
#define DEBUG(loggerName, format, ...)      log(LogLevel::Debug, __FILE_NAME__, __LINE__, loggerName, format, ##__VA_ARGS__)

#endif //GAMEBOY_EMULATOR_LOG_H
