#ifndef PWMCONFIG_H
#define PWMCONFIG_H

#include "HalPin.h"
#include <cstdint>
#include "EHalTimer.h"
#include "EHalTimerChannel.h"


struct PwmConfig {
    EHalTimer timer = EHalTimer::Tim1;
    EHalTimerChannel channel = EHalTimerChannel::Ch1;
    HalPin pin{};
    uint32_t frequencyHz = 50;
};

inline bool operator==(const PwmConfig &lhs, const PwmConfig &rhs) {
    return lhs.timer == rhs.timer &&
           lhs.channel == rhs.channel &&
           lhs.pin.port == rhs.pin.port &&
           lhs.pin.pin == rhs.pin.pin &&
           lhs.frequencyHz == rhs.frequencyHz;
}

#endif // PWMCONFIG_H
