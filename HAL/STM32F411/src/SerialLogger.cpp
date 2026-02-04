#include "SerialLogger.h"
#include "LogEntry.h"
#include <cstring>

static constexpr size_t kLogQueueSize = 8;
static volatile bool s_txBusy = false;
static LogEntry s_queue[kLogQueueSize];
static size_t s_head = 0;
static size_t s_tail = 0;
static size_t s_count = 0;

UART_HandleTypeDef huart1;
ILogger *logger = new SerialLogger(&huart1);

static bool EnqueueLog(const char *data, uint16_t len);
static void StartNextTx(UART_HandleTypeDef *huart);
static uint16_t BuildLogLine(char *dst, size_t dstSize, const char *levelStr,
                             const char *file, int line, const std::string &message);

void InitializeLogger() {
    huart1.Instance = USART1;
    huart1.Init.BaudRate = 115200;
    huart1.Init.WordLength = UART_WORDLENGTH_8B;
    huart1.Init.StopBits = UART_STOPBITS_1;
    huart1.Init.Parity = UART_PARITY_NONE;
    huart1.Init.Mode = UART_MODE_TX_RX;
    huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    huart1.Init.OverSampling = UART_OVERSAMPLING_16;

    if (HAL_UART_Init(&huart1) != HAL_OK) {
        while (1) {
        }
    }
}

SerialLogger::SerialLogger(UART_HandleTypeDef* huart)
    : _huart(huart) {
}

static bool EnqueueLog(const char *data, uint16_t len) {
    if (s_count >= kLogQueueSize) {
        return false;
    }
    LogEntry &entry = s_queue[s_tail];
    if (len > kLogBufferSize) {
        len = kLogBufferSize;
    }
    memcpy(entry.buf, data, len);
    entry.len = len;
    s_tail = (s_tail + 1) % kLogQueueSize;
    ++s_count;
    return true;
}

static void StartNextTx(UART_HandleTypeDef *huart) {
    if (s_txBusy || s_count == 0) {
        return;
    }
    LogEntry &entry = s_queue[s_head];
    s_txBusy = true;
    if (HAL_UART_Transmit_DMA(huart, (uint8_t *)entry.buf, entry.len) != HAL_OK) {
        s_txBusy = false;
        s_head = (s_head + 1) % kLogQueueSize;
        --s_count;
    }
}

extern "C" void SerialLogger_OnTxComplete(UART_HandleTypeDef* huart) {
    if (!huart || huart->Instance != USART1) {
        return;
    }
    if (s_count > 0) {
        s_head = (s_head + 1) % kLogQueueSize;
        --s_count;
    }
    s_txBusy = false;
    StartNextTx(huart);
}

static uint16_t BuildLogLine(char *dst, size_t dstSize, const char *levelStr,
                             const char *file, int line, const std::string &message) {
    int headerLen = snprintf(dst, dstSize, "[%s] [%s:%d] ", levelStr, file, line);
    if (headerLen < 0) {
        return 0;
    }

    size_t headerSize = static_cast<size_t>(headerLen);
    if (headerSize >= dstSize) {
        headerSize = dstSize - 1;
    }

    const size_t maxPayload = dstSize - 2;
    size_t msgLen = message.length();
    if (headerSize + msgLen > maxPayload) {
        msgLen = (maxPayload > headerSize) ? (maxPayload - headerSize) : 0;
    }

    if (msgLen > 0) {
        memcpy(dst + headerSize, message.c_str(), msgLen);
    }
    dst[headerSize + msgLen] = '\r';
    dst[headerSize + msgLen + 1] = '\n';

    return static_cast<uint16_t>(headerSize + msgLen + 2);
}

void SerialLogger::Log(const ELogLevel level, const char *file, const int line, std::string &message) {
    const char *levelStr = GetLevelString(level);
    char lineBuf[kLogBufferSize];
    const uint16_t totalLen = BuildLogLine(lineBuf, sizeof(lineBuf), levelStr, file, line, message);
    if (totalLen == 0) {
        return;
    }
    if (!EnqueueLog(lineBuf, totalLen)) {
        return;
    }
    StartNextTx(_huart);
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

