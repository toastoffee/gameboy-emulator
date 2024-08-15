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
#include "log.h"


void log(LogLevel logLevel, const char *fileName, int line, const char *loggerName, const char *format, va_list args) {

    char desc[128];
    vsnprintf(desc, sizeof(desc), format, args);

    char logContent[512];
    snprintf(logContent, sizeof(logContent), "[%s] - %s:line %d - [%s] %s\n",
             GetCurrentTimeStamp().c_str(), fileName, line, toCString(logLevel), desc);

    va_end(args);

    _logAppender.Log(logContent);
}
