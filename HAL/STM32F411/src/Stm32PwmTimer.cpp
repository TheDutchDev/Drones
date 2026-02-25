#include "Stm32PwmTimer.h"

#include <algorithm>
#include <cmath>

#include "stm32f4xx_hal.h"

struct TimerState {
    TIM_HandleTypeDef handle{};
    bool initialized = false;
    float frequencyHz = 0.0f;
};

static TimerState Tim1State{};
static TimerState Tim3State{};

static uint32_t ResolveTimerChannel(EHalTimerChannel channel) {
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

static bool ResolveTimerTarget(EHalTimer timer, TIM_TypeDef *&timerPtr, TimerState *&state) {
    switch (timer) {
        case EHalTimer::Tim1:
            timerPtr = TIM1;
            state = &Tim1State;
            return true;
        case EHalTimer::Tim3:
            timerPtr = TIM3;
            state = &Tim3State;
            return true;
        default:
            return false;
    }
}

static uint32_t TimerClockFor(EHalTimer timer) {
    const bool isApb2 = (timer == EHalTimer::Tim1);
    const uint32_t pclk = isApb2 ? HAL_RCC_GetPCLK2Freq() : HAL_RCC_GetPCLK1Freq();
    const uint32_t prescaler = isApb2 ? (RCC->CFGR & RCC_CFGR_PPRE2) : (RCC->CFGR & RCC_CFGR_PPRE1);
    const uint32_t div1 = isApb2 ? RCC_CFGR_PPRE2_DIV1 : RCC_CFGR_PPRE1_DIV1;
    const bool apbDiv1 = prescaler == div1;
    return apbDiv1 ? pclk : (pclk * 2U);
}

static bool EnableTimerClock(EHalTimer timer) {
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

static bool ConfigureTimer(const PwmConfig &config, TimerState &state, TIM_TypeDef *timer, uint32_t channel,
                           TIM_HandleTypeDef *&timerHandle, uint32_t &period) {
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

        period = static_cast<uint32_t>(
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

    timerHandle = &state.handle;
    period = state.handle.Init.Period;
    return true;
}

bool Stm32PwmTimer::Initialize(const PwmConfig &config) {
    if (_initialized) {
        return true;
    }

    TIM_TypeDef *timer = nullptr;
    TimerState *state = nullptr;
    if (!ResolveTimerTarget(config.timer, timer, state)) {
        return false;
    }

    if (!EnableTimerClock(config.timer)) {
        return false;
    }

    TIM_HandleTypeDef *timerHandle = nullptr;
    uint32_t period = 0;
    const uint32_t channel = ResolveTimerChannel(config.channel);
    if (!ConfigureTimer(config, *state, timer, channel, timerHandle, period)) {
        return false;
    }

    _timerContext = timerHandle;
    _timerChannel = channel;
    _timerPeriod = period;
    _initialized = true;
    return true;
}

void Stm32PwmTimer::SetDuty(float duty) {
    if (!IsValid()) {
        return;
    }

    if (duty > 1.0f) {
        duty *= 0.01f;
    }

    const float clamped = std::clamp(duty, 0.0f, 1.0f);
    const uint32_t compare = static_cast<uint32_t>(static_cast<float>(_timerPeriod) * clamped);
    auto *timer = static_cast<TIM_HandleTypeDef *>(_timerContext);
    __HAL_TIM_SET_COMPARE(timer, _timerChannel, compare);
}

bool Stm32PwmTimer::IsValid() const {
    return _initialized && (_timerContext != nullptr);
}
