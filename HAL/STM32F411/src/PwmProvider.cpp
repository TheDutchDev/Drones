#include "IPwmProvider.h"
#include "stm32f4xx_hal.h"

struct PwmContext {
    TIM_HandleTypeDef *timer = nullptr;
    uint32_t channel = 0;
};

struct TimerState {
    TIM_HandleTypeDef timer{};
    PwmContext contexts[4]{};
    bool initialized = false;
    uint32_t frequency = 0;
};

static TimerState Tim1State{};
static TimerState Tim3State{};

static GPIO_TypeDef *ResolvePort(HalPort port) {
    switch (port) {
        case HalPort::A:
            return GPIOA;
        case HalPort::B:
            return GPIOB;
        case HalPort::C:
            return GPIOC;
        case HalPort::D:
            return GPIOD;
        case HalPort::E:
            return GPIOE;
        case HalPort::H:
            return GPIOH;
        default:
            return nullptr;
    }
}

static uint32_t ResolveChannel(HalTimerChannel channel) {
    switch (channel) {
        case HalTimerChannel::Ch1:
            return TIM_CHANNEL_1;
        case HalTimerChannel::Ch2:
            return TIM_CHANNEL_2;
        case HalTimerChannel::Ch3:
            return TIM_CHANNEL_3;
        case HalTimerChannel::Ch4:
            return TIM_CHANNEL_4;
        default:
            return 0;
    }
}

static uint16_t ResolvePinMask(uint8_t pin) {
    return static_cast<uint16_t>(1u << pin);
}

static bool EnableGpioClock(HalPort port) {
    switch (port) {
        case HalPort::A:
            __HAL_RCC_GPIOA_CLK_ENABLE();
            return true;
        case HalPort::B:
            __HAL_RCC_GPIOB_CLK_ENABLE();
            return true;
        case HalPort::C:
            __HAL_RCC_GPIOC_CLK_ENABLE();
            return true;
        case HalPort::D:
            __HAL_RCC_GPIOD_CLK_ENABLE();
            return true;
        case HalPort::E:
            __HAL_RCC_GPIOE_CLK_ENABLE();
            return true;
        case HalPort::H:
            __HAL_RCC_GPIOH_CLK_ENABLE();
            return true;
        default:
            return false;
    }
}

static uint32_t TimerClockFor(HalTimer timer) {
    if (timer == HalTimer::Tim1) {
        const uint32_t pclk = HAL_RCC_GetPCLK2Freq();
        const bool apb_div1 = ((RCC->CFGR & RCC_CFGR_PPRE2) == RCC_CFGR_PPRE2_DIV1);
        return apb_div1 ? pclk : (pclk * 2u);
    }
    const uint32_t pclk = HAL_RCC_GetPCLK1Freq();
    const bool apb_div1 = ((RCC->CFGR & RCC_CFGR_PPRE1) == RCC_CFGR_PPRE1_DIV1);
    return apb_div1 ? pclk : (pclk * 2u);
}

static bool IsTim3PinValid(HalTimerChannel channel, const HalPin &pin, uint32_t &alternate) {
    alternate = GPIO_AF2_TIM3;
    if (channel == HalTimerChannel::Ch1) {
        return (pin.port == HalPort::A && pin.pin == 6) ||
               (pin.port == HalPort::B && pin.pin == 4);
    }
    if (channel == HalTimerChannel::Ch2) {
        return (pin.port == HalPort::A && pin.pin == 7) ||
               (pin.port == HalPort::B && pin.pin == 5);
    }
    if (channel == HalTimerChannel::Ch3) {
        return (pin.port == HalPort::B && pin.pin == 0);
    }
    if (channel == HalTimerChannel::Ch4) {
        return (pin.port == HalPort::B && pin.pin == 1);
    }
    return false;
}

static bool ConfigureTimer(const HalPwmConfig &cfg,
                           TimerState &state,
                           TIM_TypeDef *instance,
                           uint32_t alternate,
                           PwmContext &ctx) {
    const uint32_t channel = ResolveChannel(cfg.channel);
    if (channel == 0) {
        return false;
    }

    GPIO_TypeDef *gpio = ResolvePort(cfg.pin.port);
    if (!gpio) {
        return false;
    }
    if (!EnableGpioClock(cfg.pin.port)) {
        return false;
    }

    GPIO_InitTypeDef gpioInit{};
    gpioInit.Pin = ResolvePinMask(cfg.pin.pin);
    gpioInit.Mode = GPIO_MODE_AF_PP;
    gpioInit.Pull = GPIO_NOPULL;
    gpioInit.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    gpioInit.Alternate = alternate;
    HAL_GPIO_Init(gpio, &gpioInit);

    if (state.initialized && state.frequency != cfg.frequencyHz) {
        return false;
    }
    if (!state.initialized) {
        const uint32_t timer_clk = TimerClockFor(cfg.timer);
        const uint32_t target_clk = 1000000u;
        const uint32_t prescaler = (timer_clk / target_clk) - 1u;
        const uint32_t period = (target_clk / cfg.frequencyHz) - 1u;

        state.timer.Instance = instance;
        state.timer.Init.Prescaler = prescaler;
        state.timer.Init.CounterMode = TIM_COUNTERMODE_UP;
        state.timer.Init.Period = period;
        state.timer.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
        state.timer.Init.RepetitionCounter = 0;
        state.timer.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
        if (HAL_TIM_PWM_Init(&state.timer) != HAL_OK) {
            return false;
        }
        state.initialized = true;
        state.frequency = cfg.frequencyHz;
    }

    TIM_OC_InitTypeDef ocInit{};
    ocInit.OCMode = TIM_OCMODE_PWM1;
    ocInit.Pulse = 0;
    ocInit.OCPolarity = TIM_OCPOLARITY_HIGH;
    ocInit.OCFastMode = TIM_OCFAST_DISABLE;
    if (HAL_TIM_PWM_ConfigChannel(&state.timer, &ocInit, channel) != HAL_OK) {
        return false;
    }

    if (HAL_TIM_PWM_Start(&state.timer, channel) != HAL_OK) {
        return false;
    }

    ctx.timer = &state.timer;
    ctx.channel = channel;
    return true;
}

static void SetDuty(void *context, float duty) {
    auto *ctx = static_cast<PwmContext *>(context);
    if (!ctx || !ctx->timer) {
        return;
    }
    if (duty < 0.0f) {
        duty = 0.0f;
    }
    if (duty > 1.0f) {
        duty = 1.0f;
    }
    const uint32_t period = __HAL_TIM_GET_AUTORELOAD(ctx->timer);
    const uint32_t compare = static_cast<uint32_t>(duty * static_cast<float>(period));
    __HAL_TIM_SET_COMPARE(ctx->timer, ctx->channel, compare);
}

class Stm32PwmProvider final : public IPwmProvider {
public:
    PwmOutput Create(const HalPwmConfig &config) override {
        PwmOutput output{};
        const uint8_t index = static_cast<uint8_t>(config.channel);
        if (index >= 4) {
            return output;
        }

        if (config.timer == HalTimer::Tim1) {
            __HAL_RCC_TIM1_CLK_ENABLE();
            if (!ConfigureTimer(config, Tim1State, TIM1, GPIO_AF1_TIM1, Tim1State.contexts[index])) {
                return output;
            }
            output.context = &Tim1State.contexts[index];
        } else if (config.timer == HalTimer::Tim3) {
            uint32_t alternate = 0;
            if (!IsTim3PinValid(config.channel, config.pin, alternate)) {
                return output;
            }
            __HAL_RCC_TIM3_CLK_ENABLE();
            if (!ConfigureTimer(config, Tim3State, TIM3, alternate, Tim3State.contexts[index])) {
                return output;
            }
            output.context = &Tim3State.contexts[index];
        } else {
            return output;
        }

        output.SetDuty = &SetDuty;
        return output;
    }
};

static Stm32PwmProvider Provider{};

IPwmProvider *GetPwmProvider() {
    return &Provider;
}
