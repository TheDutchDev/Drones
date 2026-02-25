#ifndef GPIOHELPERS_H
#define GPIOHELPERS_H

#include "EHalPort.h"
#include "stm32f4xx_hal.h"

inline GPIO_TypeDef *ResolveGpioPort(EHalPort port) {
    switch (port) {
        case EHalPort::A:
            return GPIOA;
        case EHalPort::B:
            return GPIOB;
        case EHalPort::C:
            return GPIOC;
        case EHalPort::D:
            return GPIOD;
        case EHalPort::E:
            return GPIOE;
        case EHalPort::H:
            return GPIOH;
        default:
            return nullptr;
    }
}

inline bool EnableGpioClock(EHalPort port) {
    switch (port) {
        case EHalPort::A:
            __HAL_RCC_GPIOA_CLK_ENABLE();
            return true;
        case EHalPort::B:
            __HAL_RCC_GPIOB_CLK_ENABLE();
            return true;
        case EHalPort::C:
            __HAL_RCC_GPIOC_CLK_ENABLE();
            return true;
        case EHalPort::D:
            __HAL_RCC_GPIOD_CLK_ENABLE();
            return true;
        case EHalPort::E:
            __HAL_RCC_GPIOE_CLK_ENABLE();
            return true;
        case EHalPort::H:
            __HAL_RCC_GPIOH_CLK_ENABLE();
            return true;
        default:
            return false;
    }
}

inline uint16_t ResolvePinMask(uint8_t pin) {
    if (pin >= 16) {
        return 0;
    }
    return static_cast<uint16_t>(1U << pin);
}

#endif // GPIOHELPERS_H
