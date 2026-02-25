#ifndef STM32GPIOINPUTPIN_H
#define STM32GPIOINPUTPIN_H

#include <cstdint>

#include "IGpioInputPin.h"

class Stm32GpioInputPin final : public IGpioInputPin {
public:
    bool Initialize(const GpInputConfig &config) override;
    bool Read() const override;
    bool IsValid() const override;

private:
    void *_portContext = nullptr;
    uint16_t _pinMask = 0;
    bool _initialized = false;
};

#endif // STM32GPIOINPUTPIN_H
