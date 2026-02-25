#ifndef STM32PWMPIN_H
#define STM32PWMPIN_H

#include "IPwmPin.h"

class Stm32PwmPin final : public IPwmPin {
public:
    bool Initialize(const PwmConfig &config) override;
    bool IsValid() const override;

private:
    bool _initialized = false;
};

#endif // STM32PWMPIN_H
