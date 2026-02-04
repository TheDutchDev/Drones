#ifndef SERIALLOGGER_H
#define SERIALLOGGER_H
#include "ILogger.h"
#include "stm32f4xx_hal.h"

class SerialLogger : public ILogger {
public:
    SerialLogger(UART_HandleTypeDef* huart);

protected:
    void Log(const ELogLevel level, const char* file, const int line, std::string& message) override;

private:
    UART_HandleTypeDef* _huart;

    const char* GetLevelString(const ELogLevel level);
};

#endif //SERIALLOGGER_H