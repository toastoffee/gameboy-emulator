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
#include <string>

enum class LogLevel : unsigned int {

    Fatal       = 0,
    Critical    = 100,
    Error       = 200,
    Warn        = 300,
    Info        = 400,
    Debug       = 500,
};

void log(LogLevel logLevel, const char* fileName, int line, const char *format, va_list args);

void Fatal(const char *fileName, int line, const char *format, ...);
void Critical(const char *fileName, int line, const char *format, ...);
void Error(const char *fileName, int line, const char *format, ...);
void Warn(const char *fileName, int line, const char *format, ...);
void Info(const char *fileName, int line, const char *format, ...);
void Debug(const char *fileName, int line, const char *format, ...);

std::string getCurrentTimeStamp();

const char* convertLogLevelToStr(const LogLevel &logLevel);

#define FATAL(format, ...)      Fatal(__FILE_NAME__, __LINE__, format, ##__VA_ARGS__)
#define CRITICAL(format, ...)   Critical(__FILE_NAME__, __LINE__, format, ##__VA_ARGS__)
#define ERROR(format, ...)      Error( __FILE_NAME__, __LINE__, format, ##__VA_ARGS__)
#define WARN(format, ...)       Warn(__FILE_NAME__, __LINE__, format, ##__VA_ARGS__)
#define INFO(format, ...)       Info(__FILE_NAME__, __LINE__, format, ##__VA_ARGS__)
#define DEBUG(format, ...)      Debug(__FILE_NAME__, __LINE__, format, ##__VA_ARGS__)

#endif //GAMEBOY_EMULATOR_LOG_H
