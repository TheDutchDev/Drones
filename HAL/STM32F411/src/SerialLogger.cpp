#include "SerialLogger.h"
#include <cstring>

UART_HandleTypeDef huart1;
ILogger *logger = new SerialLogger(&huart1);

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

namespace {
    static constexpr uint32_t kLogUartTimeoutMs = 1000;
    static constexpr size_t kLogBufferSize = 256;
    static volatile bool s_txBusy = false;
    static char s_txBuffer[kLogBufferSize];

    bool WaitForTxIdle(uint32_t timeoutMs) {
        uint32_t start = HAL_GetTick();
        while (s_txBusy) {
            if ((HAL_GetTick() - start) >= timeoutMs) {
                return false;
            }
        }
        return true;
    }
}

extern "C" void SerialLogger_OnTxComplete(UART_HandleTypeDef* huart) {
    if (huart != nullptr && huart->Instance == USART1) {
        s_txBusy = false;
    }
}

void SerialLogger::Log(const ELogLevel level, const char *file, const int line, std::string &message) {
    const char *levelStr = GetLevelString(level);

    // Format: [LEVEL] [File:Line] Message
    int headerLen = snprintf(s_txBuffer, sizeof(s_txBuffer), "[%s] [%s:%d] ",
                             levelStr, file, line);
    if (headerLen < 0) {
        return;
    }
    if (headerLen >= static_cast<int>(sizeof(s_txBuffer))) {
        headerLen = static_cast<int>(sizeof(s_txBuffer)) - 1;
    }

    const size_t maxPayload = sizeof(s_txBuffer) - 2;
    size_t msgLen = message.length();
    if (static_cast<size_t>(headerLen) + msgLen > maxPayload) {
        msgLen = (maxPayload > static_cast<size_t>(headerLen)) ? (maxPayload - static_cast<size_t>(headerLen)) : 0;
    }

    if (!WaitForTxIdle(kLogUartTimeoutMs)) {
        return;
    }

    if (msgLen > 0) {
        memcpy(s_txBuffer + headerLen, message.c_str(), msgLen);
    }
    s_txBuffer[headerLen + msgLen] = '\r';
    s_txBuffer[headerLen + msgLen + 1] = '\n';

    const uint16_t totalLen = static_cast<uint16_t>(headerLen + msgLen + 2);
    s_txBusy = true;
    if (HAL_UART_Transmit_DMA(_huart, reinterpret_cast<uint8_t*>(s_txBuffer), totalLen) != HAL_OK) {
        s_txBusy = false;
    }
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

