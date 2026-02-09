#ifndef PWMCONFIG_H
#define PWMCONFIG_H

#include "EHalTimer.h"
#include "EHalTimerChannel.h"
#include "HalPin.h"

struct PwmConfig {
    EHalTimer timer = EHalTimer::Tim3;
    EHalTimerChannel channel = EHalTimerChannel::Ch1;
    HalPin pin{};
    float frequencyHz = 50.0f;
};

#endif // PWMCONFIG_H
