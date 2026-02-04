#ifndef ILOGGER_H
#define ILOGGER_H

extern void InitializeLogger();

#ifdef ENABLE_LOGGING

#include <string>
#include "ELogLevel.h"

// Forward declaration
class ILogger;
extern ILogger *logger;

// Define log levels as constants
#define LOG_LEVEL_TRACE 0
#define LOG_LEVEL_DEBUG 1
#define LOG_LEVEL_INFO 2
#define LOG_LEVEL_WARNING 3
#define LOG_LEVEL_ERROR 4
#define LOG_LEVEL_FATAL 5

#ifndef LOG_LEVEL
#define LOG_LEVEL LOG_LEVEL_FATAL
#endif

#if LOG_LEVEL <= LOG_LEVEL_TRACE
#define LOG_TRACE(...) logger->LogMessage(ELogLevel::Trace, __FILE__, __LINE__, __VA_ARGS__)
#else
#define LOG_TRACE(...)
#endif

#if LOG_LEVEL <= LOG_LEVEL_DEBUG
#define LOG_DEBUG(...) logger->LogMessage(ELogLevel::Debug, __FILE__, __LINE__, __VA_ARGS__)
#else
#define LOG_DEBUG(...)
#endif

#if LOG_LEVEL <= LOG_LEVEL_INFO
#define LOG_INFO(...) logger->LogMessage(ELogLevel::Info, __FILE__, __LINE__, __VA_ARGS__)
#else
#define LOG_INFO(...)
#endif

#if LOG_LEVEL <= LOG_LEVEL_WARNING
#define LOG_WARNING(...) logger->LogMessage(ELogLevel::Warning, __FILE__, __LINE__, __VA_ARGS__)
#else
#define LOG_WARNING(...)
#endif

#if LOG_LEVEL <= LOG_LEVEL_ERROR
#define LOG_ERROR(...) logger->LogMessage(ELogLevel::Error, __FILE__, __LINE__, __VA_ARGS__)
#else
#define LOG_ERROR(...)
#endif

#if LOG_LEVEL <= LOG_LEVEL_FATAL
#define LOG_FATAL(...) logger->LogMessage(ELogLevel::Fatal, __FILE__, __LINE__, __VA_ARGS__)
#else
#define LOG_FATAL(...)
#endif

class ILogger {
public:
    template<typename... Args>
    void LogMessage(const ELogLevel level, const char *file, const int line, Args... args) {
        std::string output;
        LogInternal(output, args...);
        Log(level, file, line, output);
    }

protected:
    virtual void Log(ELogLevel level, const char *file, int line, std::string &message) = 0;

private:
    template<typename T>
    void LogInternal(std::string &logMessage, T arg) {
        logMessage += arg;
        logMessage.append(" ");
    }

    template<typename T, typename... Args>
    void LogInternal(std::string &logMessage, T firstArg, Args... args) {
        LogInternal(logMessage, firstArg);
        LogInternal(logMessage, args...);
    }
};

#else

#define LOG_TRACE(...)
#define LOG_DEBUG(...)
#define LOG_INFO(...)
#define LOG_WARNING(...)
#define LOG_ERROR(...)
#define LOG_FATAL(...)

#include <string>
#include "ELogLevel.h"

class ILogger {
public:
    virtual void Log(ELogLevel level, const char *file, int line, std::string &message) {};
};

#endif

#endif //ILOGGER_H
