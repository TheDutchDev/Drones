#include "IPwmProvider.h"

#include <algorithm>
#include <cmath>

#include "stm32f4xx_hal.h"

namespace {
struct PwmContext {
    TIM_HandleTypeDef *timer = nullptr;
    uint32_t channel = 0;
    uint32_t period = 0;
};

struct TimerState {
    TIM_HandleTypeDef handle{};
    bool initialized = false;
    float frequencyHz = 0.0f;
};

TimerState Tim1State{};
TimerState Tim3State{};

GPIO_TypeDef *ResolvePort(EHalPort port) {
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

uint32_t ResolveChannel(EHalTimerChannel channel) {
    switch (channel) {
        case EHalTimerChannel::Ch1:
            return TIM_CHANNEL_1;
        case EHalTimerChannel::Ch2:
            return TIM_CHANNEL_2;
        case EHalTimerChannel::Ch3:
            return TIM_CHANNEL_3;
        case EHalTimerChannel::Ch4:
            return TIM_CHANNEL_4;
        default:
            return TIM_CHANNEL_1;
    }
}

uint16_t ResolvePinMask(uint8_t pin) {
    if (pin >= 16) {
        return 0;
    }
    return static_cast<uint16_t>(1U << pin);
}

bool EnableGpioClock(EHalPort port) {
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

uint32_t TimerClockFor(EHalTimer timer) {
    const bool isApb2 = (timer == EHalTimer::Tim1);
    const uint32_t pclk = isApb2 ? HAL_RCC_GetPCLK2Freq() : HAL_RCC_GetPCLK1Freq();
    const uint32_t prescaler = isApb2 ? (RCC->CFGR & RCC_CFGR_PPRE2) : (RCC->CFGR & RCC_CFGR_PPRE1);
    const uint32_t div1 = isApb2 ? RCC_CFGR_PPRE2_DIV1 : RCC_CFGR_PPRE1_DIV1;
    const bool apbDiv1 = prescaler == div1;
    return apbDiv1 ? pclk : (pclk * 2U);
}

bool EnableTimerClock(EHalTimer timer) {
    switch (timer) {
        case EHalTimer::Tim1:
            __HAL_RCC_TIM1_CLK_ENABLE();
            return true;
        case EHalTimer::Tim2:
            __HAL_RCC_TIM2_CLK_ENABLE();
            return true;
        case EHalTimer::Tim3:
            __HAL_RCC_TIM3_CLK_ENABLE();
            return true;
        case EHalTimer::Tim4:
            __HAL_RCC_TIM4_CLK_ENABLE();
            return true;
        case EHalTimer::Tim5:
            __HAL_RCC_TIM5_CLK_ENABLE();
            return true;
        default:
            return false;
    }
}

bool IsTim3PinValid(EHalTimerChannel channel, const HalPin &pin, uint32_t &alternate) {
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

bool IsTim1PinValid(EHalTimerChannel channel, const HalPin &pin, uint32_t &alternate) {
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

bool ConfigureTimer(const PwmConfig &config, TimerState &state, TIM_TypeDef *timer,
                    uint32_t channel, PwmContext &context) {
    if (config.frequencyHz <= 0.0f) {
        return false;
    }

    if (!state.initialized || std::fabs(state.frequencyHz - config.frequencyHz) > 0.0001f) {
        const uint32_t timerClk = TimerClockFor(config.timer);
        const float ticksPerPeriod = static_cast<float>(timerClk) / config.frequencyHz;
        uint32_t prescaler = 0;
        if (ticksPerPeriod > 65535.0f) {
            prescaler = static_cast<uint32_t>(ticksPerPeriod / 65535.0f);
        }
        const uint32_t period = static_cast<uint32_t>(
            (static_cast<float>(timerClk) / (config.frequencyHz * static_cast<float>(prescaler + 1U))) - 1.0f);

        state.handle.Instance = timer;
        state.handle.Init.Prescaler = prescaler;
        state.handle.Init.CounterMode = TIM_COUNTERMODE_UP;
        state.handle.Init.Period = period;
        state.handle.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
        state.handle.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
        if (config.timer == EHalTimer::Tim1) {
            state.handle.Init.RepetitionCounter = 0;
        }

        if (HAL_TIM_PWM_Init(&state.handle) != HAL_OK) {
            return false;
        }

        state.initialized = true;
        state.frequencyHz = config.frequencyHz;
    }

    TIM_OC_InitTypeDef ocInit{};
    ocInit.OCMode = TIM_OCMODE_PWM1;
    ocInit.Pulse = 0;
    ocInit.OCPolarity = TIM_OCPOLARITY_HIGH;
    ocInit.OCFastMode = TIM_OCFAST_DISABLE;
    if (config.timer == EHalTimer::Tim1) {
        ocInit.OCNPolarity = TIM_OCNPOLARITY_HIGH;
        ocInit.OCIdleState = TIM_OCIDLESTATE_RESET;
        ocInit.OCNIdleState = TIM_OCNIDLESTATE_RESET;
    }

    if (HAL_TIM_PWM_ConfigChannel(&state.handle, &ocInit, channel) != HAL_OK) {
        return false;
    }

    if (HAL_TIM_PWM_Start(&state.handle, channel) != HAL_OK) {
        return false;
    }

    context.timer = &state.handle;
    context.channel = channel;
    context.period = state.handle.Init.Period;
    return true;
}

void SetDuty(void *context, float duty) {
    if (context == nullptr) {
        return;
    }

    auto *pwm = static_cast<PwmContext *>(context);
    if (pwm->timer == nullptr) {
        return;
    }

    if (duty > 1.0f) {
        duty *= 0.01f;
    }
    const float clamped = std::clamp(duty, 0.0f, 1.0f);
    const uint32_t compare = static_cast<uint32_t>(static_cast<float>(pwm->period) * clamped);
    __HAL_TIM_SET_COMPARE(pwm->timer, pwm->channel, compare);
}
} // namespace

class Stm32PwmProvider : public IPwmProvider {
public:
    ~Stm32PwmProvider() override = default;

    PwmOutput Create(const PwmConfig &config) override {
        uint32_t alternate = 0;
        TIM_TypeDef *timer = nullptr;
        TimerState *state = nullptr;

        switch (config.timer) {
            case EHalTimer::Tim1:
                timer = TIM1;
                state = &Tim1State;
                if (!IsTim1PinValid(config.channel, config.pin, alternate)) {
                    return {};
                }
                break;
            case EHalTimer::Tim3:
                timer = TIM3;
                state = &Tim3State;
                if (!IsTim3PinValid(config.channel, config.pin, alternate)) {
                    return {};
                }
                break;
            default:
                return {};
        }

        if (!EnableGpioClock(config.pin.port)) {
            return {};
        }
        if (!EnableTimerClock(config.timer)) {
            return {};
        }

        const uint16_t pinMask = ResolvePinMask(config.pin.pin);
        if (pinMask == 0) {
            return {};
        }

        GPIO_InitTypeDef gpioInit{};
        gpioInit.Pin = pinMask;
        gpioInit.Mode = GPIO_MODE_AF_PP;
        gpioInit.Pull = GPIO_NOPULL;
        gpioInit.Speed = GPIO_SPEED_FREQ_HIGH;
        gpioInit.Alternate = alternate;
        HAL_GPIO_Init(ResolvePort(config.pin.port), &gpioInit);

        auto *context = new PwmContext{};
        const uint32_t channel = ResolveChannel(config.channel);
        if (!ConfigureTimer(config, *state, timer, channel, *context)) {
            delete context;
            return {};
        }

        return {context, SetDuty};
    }
};

IPwmProvider *GetPwmProvider() {
    static Stm32PwmProvider provider;
    return &provider;
}
