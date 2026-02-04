#include "IPwmProvider.h"
#include "stm32f4xx_hal.h"

struct PwmContext {
    TIM_HandleTypeDef *timer = nullptr;
    uint32_t channel = 0;
};

static TIM_HandleTypeDef Tim1{};
static PwmContext Tim1Context[4]{};
static bool Tim1Initialized = false;
static uint32_t Tim1Frequency = 0;

static TIM_HandleTypeDef Tim3{};
static PwmContext Tim3Context[4]{};
static bool Tim3Initialized = false;
static uint32_t Tim3Frequency = 0;

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

static uint32_t TimerClockForTim1() {
    const uint32_t pclk = HAL_RCC_GetPCLK2Freq();
    const bool apb_div1 = ((RCC->CFGR & RCC_CFGR_PPRE2) == RCC_CFGR_PPRE2_DIV1);
    return apb_div1 ? pclk : (pclk * 2u);
}

static uint32_t TimerClockForTim3() {
    const uint32_t pclk = HAL_RCC_GetPCLK1Freq();
    const bool apb_div1 = ((RCC->CFGR & RCC_CFGR_PPRE1) == RCC_CFGR_PPRE1_DIV1);
    return apb_div1 ? pclk : (pclk * 2u);
}

static bool ConfigureTim1(const HalPwmConfig &cfg, PwmContext &ctx) {
    if (cfg.timer != HalTimer::Tim1) {
        return false;
    }

    const uint32_t channel = ResolveChannel(cfg.channel);
    if (channel == 0) {
        return false;
    }

    GPIO_TypeDef *gpio = ResolvePort(cfg.pin.port);
    if (!gpio) {
        return false;
    }

    switch (cfg.pin.port) {
        case HalPort::A:
            __HAL_RCC_GPIOA_CLK_ENABLE();
            break;
        case HalPort::B:
            __HAL_RCC_GPIOB_CLK_ENABLE();
            break;
        case HalPort::C:
            __HAL_RCC_GPIOC_CLK_ENABLE();
            break;
        case HalPort::D:
            __HAL_RCC_GPIOD_CLK_ENABLE();
            break;
        case HalPort::E:
            __HAL_RCC_GPIOE_CLK_ENABLE();
            break;
        case HalPort::H:
            __HAL_RCC_GPIOH_CLK_ENABLE();
            break;
        default:
            return false;
    }
    __HAL_RCC_TIM1_CLK_ENABLE();

    GPIO_InitTypeDef gpioInit{};
    gpioInit.Pin = ResolvePinMask(cfg.pin.pin);
    gpioInit.Mode = GPIO_MODE_AF_PP;
    gpioInit.Pull = GPIO_NOPULL;
    gpioInit.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    gpioInit.Alternate = GPIO_AF1_TIM1;
    HAL_GPIO_Init(gpio, &gpioInit);

    if (Tim1Initialized && Tim1Frequency != cfg.frequencyHz) {
        return false;
    }
    if (!Tim1Initialized) {
        const uint32_t timer_clk = TimerClockForTim1();
        const uint32_t target_clk = 1000000u;
        const uint32_t prescaler = (timer_clk / target_clk) - 1u;
        const uint32_t period = (target_clk / cfg.frequencyHz) - 1u;

        Tim1.Instance = TIM1;
        Tim1.Init.Prescaler = prescaler;
        Tim1.Init.CounterMode = TIM_COUNTERMODE_UP;
        Tim1.Init.Period = period;
        Tim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
        Tim1.Init.RepetitionCounter = 0;
        Tim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
        if (HAL_TIM_PWM_Init(&Tim1) != HAL_OK) {
            return false;
        }
        Tim1Initialized = true;
        Tim1Frequency = cfg.frequencyHz;
    }

    TIM_OC_InitTypeDef ocInit{};
    ocInit.OCMode = TIM_OCMODE_PWM1;
    ocInit.Pulse = 0;
    ocInit.OCPolarity = TIM_OCPOLARITY_HIGH;
    ocInit.OCFastMode = TIM_OCFAST_DISABLE;
    if (HAL_TIM_PWM_ConfigChannel(&Tim1, &ocInit, channel) != HAL_OK) {
        return false;
    }

    if (HAL_TIM_PWM_Start(&Tim1, channel) != HAL_OK) {
        return false;
    }

    ctx.timer = &Tim1;
    ctx.channel = channel;
    return true;
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
    return false;
}

static bool ConfigureTim3(const HalPwmConfig &cfg, PwmContext &ctx) {
    if (cfg.timer != HalTimer::Tim3) {
        return false;
    }

    const uint32_t channel = ResolveChannel(cfg.channel);
    if (channel == 0) {
        return false;
    }

    uint32_t alternate = 0;
    if (!IsTim3PinValid(cfg.channel, cfg.pin, alternate)) {
        return false;
    }

    GPIO_TypeDef *gpio = ResolvePort(cfg.pin.port);
    if (!gpio) {
        return false;
    }

    switch (cfg.pin.port) {
        case HalPort::A:
            __HAL_RCC_GPIOA_CLK_ENABLE();
            break;
        case HalPort::B:
            __HAL_RCC_GPIOB_CLK_ENABLE();
            break;
        case HalPort::C:
            __HAL_RCC_GPIOC_CLK_ENABLE();
            break;
        case HalPort::D:
            __HAL_RCC_GPIOD_CLK_ENABLE();
            break;
        case HalPort::E:
            __HAL_RCC_GPIOE_CLK_ENABLE();
            break;
        case HalPort::H:
            __HAL_RCC_GPIOH_CLK_ENABLE();
            break;
        default:
            return false;
    }

    __HAL_RCC_TIM3_CLK_ENABLE();

    GPIO_InitTypeDef gpioInit{};
    gpioInit.Pin = ResolvePinMask(cfg.pin.pin);
    gpioInit.Mode = GPIO_MODE_AF_PP;
    gpioInit.Pull = GPIO_NOPULL;
    gpioInit.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    gpioInit.Alternate = alternate;
    HAL_GPIO_Init(gpio, &gpioInit);

    if (Tim3Initialized && Tim3Frequency != cfg.frequencyHz) {
        return false;
    }
    if (!Tim3Initialized) {
        const uint32_t timer_clk = TimerClockForTim3();
        const uint32_t target_clk = 1000000u;
        const uint32_t prescaler = (timer_clk / target_clk) - 1u;
        const uint32_t period = (target_clk / cfg.frequencyHz) - 1u;

        Tim3.Instance = TIM3;
        Tim3.Init.Prescaler = prescaler;
        Tim3.Init.CounterMode = TIM_COUNTERMODE_UP;
        Tim3.Init.Period = period;
        Tim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
        Tim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
        if (HAL_TIM_PWM_Init(&Tim3) != HAL_OK) {
            return false;
        }
        Tim3Initialized = true;
        Tim3Frequency = cfg.frequencyHz;
    }

    TIM_OC_InitTypeDef ocInit{};
    ocInit.OCMode = TIM_OCMODE_PWM1;
    ocInit.Pulse = 0;
    ocInit.OCPolarity = TIM_OCPOLARITY_HIGH;
    ocInit.OCFastMode = TIM_OCFAST_DISABLE;
    if (HAL_TIM_PWM_ConfigChannel(&Tim3, &ocInit, channel) != HAL_OK) {
        return false;
    }

    if (HAL_TIM_PWM_Start(&Tim3, channel) != HAL_OK) {
        return false;
    }

    ctx.timer = &Tim3;
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

        bool configured = false;
        if (config.timer == HalTimer::Tim1) {
            configured = ConfigureTim1(config, Tim1Context[index]);
        } else if (config.timer == HalTimer::Tim3) {
            configured = ConfigureTim3(config, Tim3Context[index]);
        }
        if (!configured) {
            return output;
        }

        if (config.timer == HalTimer::Tim1) {
            output.context = &Tim1Context[index];
        } else {
            output.context = &Tim3Context[index];
        }
        output.SetDuty = &SetDuty;
        return output;
    }
};

static Stm32PwmProvider Provider{};

IPwmProvider *GetPwmProvider() {
    return &Provider;
}
