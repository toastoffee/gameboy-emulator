/**
  ******************************************************************************
  * @file           : log.hpp
  * @author         : toastoffee
  * @brief          : a header-only file that supports log system
  * @attention      : None
  * @date           : 2024/6/18
  ******************************************************************************
  */



#ifndef TOAST_LOG_HPP
#define TOAST_LOG_HPP

#include <string>
#include <sstream>
#include <fstream>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <thread>

#define DEFAULT_BUF_SIZE 1024


/**************************** [Buffer] ****************************/

/*
 * container of string that helps store and get the buffer
 */
class Buffer
{
public:
    explicit Buffer(int bufSize = DEFAULT_BUF_SIZE);    // initialization with buf size
    ~Buffer();                                          // delete _buf

    bool Write(const char *str, int len);               // write the str to _buf
    void GetData(char*& buf, int& len);                 // get the _buf data and size
    bool IsEmpty() const;

private:
    char *_buf;                                         // where data stores
    int _bufLen;                                        // current data length
    int _bufSize;                                       // the data buffer maximum size
};

Buffer::Buffer(int bufSize) : _buf(new char[bufSize]), _bufSize(bufSize), _bufLen(0) { }

bool Buffer::Write(const char *str, int len)
{
    if (_bufLen + len > _bufSize)
    {
        return false;
    }

    memcpy(_buf + _bufLen, str, len);
    _bufLen += len;

    return true;
}

void Buffer::GetData(char*& buffer, int& len)
{
    buffer = _buf;
    len = _bufLen;
}

bool Buffer::IsEmpty() const{
    return _bufLen == 0;
}

Buffer::~Buffer() {
    delete[] _buf;
}



/**************************** [LogAppender] ****************************/
/**************************** [Base] ****************************/

/*
 * base class of Appender
 */
class LogAppender
{
public:
    virtual void Log(const char* content) = 0;
};




/*
 * the sync abstract version of LogAppender
 */
class SyncLogAppender : public LogAppender
{
public:
    virtual void Log(const char* content);

private:
    virtual void Output(const char* content) = 0;
};

void SyncLogAppender::Log(const char *content) {
    Output(content);
}



/*
 * the async abstract version of
 *
 *
 *
 *      LogAppender
 */
class AsyncLogAppender : public LogAppender
{
public:

    void Log(const char* content) override;

    void JoinThread();

private:
    void WriteThread();

    virtual void Output(const char* content) = 0;

private:
    bool _isRunning = false;
    bool _isClosing = false;
    std::mutex _mu;
    std::condition_variable _cond;

    std::thread _writeThread;

    Buffer *_currentBuffer;
    std::queue<Buffer *> _buffers;
};

void AsyncLogAppender::JoinThread() {

    if(!_currentBuffer->IsEmpty()){
        _buffers.push(_currentBuffer);
        _currentBuffer = new Buffer();
    }

    _isClosing = true;
    _cond.notify_one();
    _writeThread.join();
}

void AsyncLogAppender::Log(const char* content) {
    if(!_isRunning){
        _isRunning = true;
        _isClosing = false;
        _currentBuffer = new Buffer();
        _writeThread = std::thread(&AsyncLogAppender::WriteThread, this);
    }

    std::lock_guard<std::mutex> lock(_mu);
    if (!_currentBuffer->Write(content, strlen(content)))
    {
        _buffers.push(_currentBuffer);

        _currentBuffer = new Buffer();

        _currentBuffer->Write(content, strlen(content));
        _cond.notify_one();
    }
}

void AsyncLogAppender::WriteThread() {
    std::queue<Buffer *> buffersToWrite;
    while (_isRunning)
    {
        {
            std::unique_lock<std::mutex> lock(_mu);
            if (_buffers.empty())
            {
                _cond.wait_for(lock, std::chrono::milliseconds(3000));
            }

            _buffers.push(_currentBuffer);
            _currentBuffer = new Buffer();

            buffersToWrite.swap(_buffers);
        }

        while (!buffersToWrite.empty())
        {
            if(!_currentBuffer->IsEmpty()){
                _buffers.push(_currentBuffer);
                _currentBuffer = new Buffer();
            }

            Buffer *buffer = buffersToWrite.front();
            char *data;
            int len;
            buffer->GetData(data, len);

            Output(data);

            delete buffer;
            buffersToWrite.pop();
        }

        if(_isClosing){
            _isRunning = false;
        }
    }
}


/**************************** [LogAppender] ****************************/
/**************************** [some implementation] ****************************/

class SyncOstreamAppender : public SyncLogAppender
{
public:
    explicit SyncOstreamAppender(std::ostream &ostream) : _ostream(ostream){};

private:
    void Output(const char* content) override;

private:
    std::ostream &_ostream;
};

void SyncOstreamAppender::Output(const char *content) {
    _ostream << content;
}



class AsyncOstreamAppender : public AsyncLogAppender
{
public:
    explicit AsyncOstreamAppender(std::ostream &ostream) : _ostream(ostream){};

private:
    void Output(const char* content) override;

private:
    std::ostream &_ostream;
};

void AsyncOstreamAppender::Output(const char *content) {
    _ostream << content;
}



class SyncFileAppender : public SyncLogAppender
{
public:
    explicit SyncFileAppender(const std::string &logFilePath);

private:
    void Output(const char *content) override;

private:
    std::string _logFilePath;
    std::ofstream _fileStream;
};

SyncFileAppender::SyncFileAppender(const std::string &logFilePath) : _logFilePath(logFilePath) { }

void SyncFileAppender::Output(const char *content) {
    _fileStream.open(_logFilePath, std::ios::out | std::ios::app);
    _fileStream << content;
    _fileStream.close();
}



class AsyncFileAppender : public AsyncLogAppender
{
public:
    explicit AsyncFileAppender(const std::string &logFilePath);

private:
    void Output(const char *content) override;

private:
    std::string _logFilePath;
    std::ofstream _fileStream;
};

AsyncFileAppender::AsyncFileAppender(const std::string &logFilePath) : _logFilePath(logFilePath) { }

void AsyncFileAppender::Output(const char *content) {
    _fileStream.open(_logFilePath, std::ios::out | std::ios::app);
    _fileStream << content;
    _fileStream.close();
}



/**************************** [logLevel] ****************************/

enum class LogLevel : unsigned int {

    Fatal       = 0,
    Critical    = 100,
    Error       = 200,
    Warn        = 300,
    Info        = 400,
    Debug       = 500,

};

const char* toCString(const LogLevel &logLevel){
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



/**************************** [TimeStamp] ****************************/

std::string GetCurrentTimeStamp(){

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

/**************************** [Logger] ****************************/

class Logger {
public:

    Logger(const char *name, LogAppender &logAppender, LogLevel logLevel = LogLevel::Debug) :
            _name(name), _logAppender(logAppender), _logLevel(logLevel) { }

    void SetPriority(LogLevel logLevel);

    void Fatal(const char* fileName, int line, const char *format, ...);
    void Critical(const char* fileName, int line, const char *format, ...);
    void Error(const char* fileName, int line, const char *format, ...);
    void Warn(const char* fileName, int line, const char *format, ...);
    void Info(const char* fileName, int line, const char *format, ...);
    void Debug(const char* fileName, int line, const char *format, ...);

    void Log(LogLevel logLevel, const char* fileName, int line, const char *format, va_list args);

private:
    const char *_name;
    LogAppender &_logAppender;
    LogLevel _logLevel;
};

void Logger::SetPriority(LogLevel logLevel) {
    _logLevel = logLevel;
}

void Logger::Log(LogLevel logLevel, const char* fileName, int line, const char *format, va_list args) {
    if(logLevel > _logLevel) return;

    char desc[128];
    vsnprintf(desc, sizeof(desc), format, args);

    char logContent[512];
    snprintf(logContent, sizeof(logContent), "[%s] - %s:line %d - [%s] %s\n",
             GetCurrentTimeStamp().c_str(), fileName, line, toCString(logLevel), desc);

    va_end(args);

    _logAppender.Log(logContent);
}

void Logger::Fatal(const char *fileName, int line, const char *format, ...) {
    va_list args;
    va_start(args, format);

    Log(LogLevel::Fatal, fileName, line, format, args);

    va_end(args);
}

void Logger::Critical(const char *fileName, int line, const char *format, ...) {
    va_list args;
    va_start(args, format);

    Log(LogLevel::Fatal, fileName, line, format, args);

    va_end(args);
}

void Logger::Error(const char *fileName, int line, const char *format, ...) {
    va_list args;
    va_start(args, format);

    Log(LogLevel::Error, fileName, line, format, args);

    va_end(args);
}

void Logger::Warn(const char *fileName, int line, const char *format, ...) {
    va_list args;
    va_start(args, format);

    Log(LogLevel::Warn, fileName, line, format, args);

    va_end(args);
}

void Logger::Info(const char *fileName, int line, const char *format, ...) {
    va_list args;
    va_start(args, format);

    Log(LogLevel::Info, fileName, line, format, args);

    va_end(args);
}

void Logger::Debug(const char *fileName, int line, const char *format, ...) {
    va_list args;
    va_start(args, format);

    Log(LogLevel::Debug, fileName, line, format, args);

    va_end(args);
}

#define FATAL(format, ...)      Fatal(__FILE_NAME__, __LINE__, format, ##__VA_ARGS__)
#define CRITICAL(format, ...)   Critical(__FILE_NAME__, __LINE__, format, ##__VA_ARGS__)
#define ERROR(format, ...)      Error(__FILE_NAME__, __LINE__, format, ##__VA_ARGS__)
#define WARN(format, ...)       Warn(__FILE_NAME__, __LINE__, format, ##__VA_ARGS__)
#define INFO(format, ...)       Info(__FILE_NAME__, __LINE__, format, ##__VA_ARGS__)
#define DEBUG(format, ...)      Debug(__FILE_NAME__, __LINE__, format, ##__VA_ARGS__)

#endif // TOAST_LOG_HPP
