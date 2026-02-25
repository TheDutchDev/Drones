#include "Stm32PwmPin.h"

#include "GpioHelpers.h"
#include "stm32f4xx_hal.h"

static bool ResolveTim3PwmPin(EHalTimerChannel channel, const HalPin &pin, uint32_t &alternate) {
    alternate = GPIO_AF2_TIM3;
    if (pin.port != EHalPort::B) {
        return false;
    }

    switch (channel) {
        case EHalTimerChannel::Ch1:
            return pin.pin == 4;
        case EHalTimerChannel::Ch2:
            return pin.pin == 5;
        case EHalTimerChannel::Ch3:
            return pin.pin == 0;
        case EHalTimerChannel::Ch4:
            return pin.pin == 1;
        default:
            return false;
    }
}

static bool ResolveTim1PwmPin(EHalTimerChannel channel, const HalPin &pin, uint32_t &alternate) {
    alternate = GPIO_AF1_TIM1;
    if (pin.port != EHalPort::A) {
        return false;
    }

    switch (channel) {
        case EHalTimerChannel::Ch1:
            return pin.pin == 8;
        case EHalTimerChannel::Ch2:
            return pin.pin == 9;
        case EHalTimerChannel::Ch3:
            return pin.pin == 10;
        case EHalTimerChannel::Ch4:
            return pin.pin == 11;
        default:
            return false;
    }
}

static bool ResolveAlternateFunction(const PwmConfig &config, uint32_t &alternate) {
    switch (config.timer) {
        case EHalTimer::Tim1:
            return ResolveTim1PwmPin(config.channel, config.pin, alternate);
        case EHalTimer::Tim3:
            return ResolveTim3PwmPin(config.channel, config.pin, alternate);
        default:
            return false;
    }
}

bool Stm32PwmPin::Initialize(const PwmConfig &config) {
    if (_initialized) {
        return true;
    }

    uint32_t alternate = 0;
    if (!ResolveAlternateFunction(config, alternate)) {
        return false;
    }

    if (!EnableGpioClock(config.pin.port)) {
        return false;
    }

    GPIO_TypeDef *port = ResolveGpioPort(config.pin.port);
    if (port == nullptr) {
        return false;
    }

    const uint16_t pinMask = ResolvePinMask(config.pin.pin);
    if (pinMask == 0) {
        return false;
    }

    GPIO_InitTypeDef gpioInit{};
    gpioInit.Pin = pinMask;
    gpioInit.Mode = GPIO_MODE_AF_PP;
    gpioInit.Pull = GPIO_NOPULL;
    gpioInit.Speed = GPIO_SPEED_FREQ_HIGH;
    gpioInit.Alternate = alternate;
    HAL_GPIO_Init(port, &gpioInit);

    _initialized = true;
    return true;
}

bool Stm32PwmPin::IsValid() const {
    return _initialized;
}
