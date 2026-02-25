#ifndef STM32PWMTIMER_H
#define STM32PWMTIMER_H

#include <cstdint>

#include "IPwmTimer.h"

class Stm32PwmTimer final : public IPwmTimer {
public:
    bool Initialize(const PwmConfig &config) override;
    void SetDuty(float duty) override;
    bool IsValid() const override;

private:
    void *_timerContext = nullptr;
    uint32_t _timerChannel = 0;
    uint32_t _timerPeriod = 0;
    bool _initialized = false;
};

#endif // STM32PWMTIMER_H
