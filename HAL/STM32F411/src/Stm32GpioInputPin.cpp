#include "Stm32GpioInputPin.h"

#include "GpioHelpers.h"
#include "stm32f4xx_hal.h"

static uint32_t ResolvePull(EGpPull pull) {
    switch (pull) {
        case EGpPull::None:
            return GPIO_NOPULL;
        case EGpPull::Up:
            return GPIO_PULLUP;
        case EGpPull::Down:
            return GPIO_PULLDOWN;
        default:
            return GPIO_NOPULL;
    }
}

bool Stm32GpioInputPin::Initialize(const GpInputConfig &config) {
    if (_initialized) {
        return true;
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
    gpioInit.Mode = GPIO_MODE_INPUT;
    gpioInit.Pull = ResolvePull(config.pull);
    gpioInit.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(port, &gpioInit);

    _portContext = port;
    _pinMask = pinMask;
    _initialized = true;
    return true;
}

bool Stm32GpioInputPin::Read() const {
    if (!IsValid()) {
        return false;
    }

    auto *port = static_cast<GPIO_TypeDef *>(_portContext);
    return HAL_GPIO_ReadPin(port, _pinMask) == GPIO_PIN_SET;
}

bool Stm32GpioInputPin::IsValid() const {
    return _initialized && (_portContext != nullptr) && (_pinMask != 0);
}
