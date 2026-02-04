#include "SerialLogger.h"

UART_HandleTypeDef huart2;
ILogger *logger = new SerialLogger(&huart2);

SerialLogger::SerialLogger(UART_HandleTypeDef* huart)
    : _huart(huart) {
}

void SerialLogger::Log(const ELogLevel level, const char *file, const int line, std::string &message) {
    char header[128];
    const char *levelStr = GetLevelString(level);

    // Format: [LEVEL] [File:Line] Message
    int headerLen = snprintf(header, sizeof(header), "[%s] [%s:%d] ",
                             levelStr, file, line);

    // Send Header
    HAL_UART_Transmit(_huart, (uint8_t *) header, headerLen, HAL_MAX_DELAY);

    // Send Message Content
    HAL_UART_Transmit(_huart, (uint8_t *) message.c_str(), message.length(), HAL_MAX_DELAY);

    // Send Newline
    HAL_UART_Transmit(_huart, (uint8_t *) "\r\n", 2, HAL_MAX_DELAY);
}

const char *SerialLogger::GetLevelString(const ELogLevel level) {
    switch (level) {
        case ELogLevel::Trace:
            return "TRACE";
        case ELogLevel::Debug:
            return "DEBUG";
        case ELogLevel::Info:
            return "INFO";
        case ELogLevel::Warning:
            return "WARN";
        case ELogLevel::Error:
            return "ERROR";
        case ELogLevel::Fatal:
            return "FATAL";
        default:
            return "UNKNOWN";
    }
}

