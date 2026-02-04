#ifndef PWMCONFIG_H
#define PWMCONFIG_H

#include "HalGpio.h"
#include <cstdint>

enum class HalTimer : uint8_t {
    Tim1,
    Tim2,
    Tim3,
    Tim4,
    Tim5,
};

enum class HalTimerChannel : uint8_t {
    Ch1,
    Ch2,
    Ch3,
    Ch4,
};

struct HalPwmConfig {
    HalTimer timer = HalTimer::Tim1;
    HalTimerChannel channel = HalTimerChannel::Ch1;
    HalPin pin{};
    uint32_t frequencyHz = 50;
};

inline bool operator==(const HalPwmConfig &lhs, const HalPwmConfig &rhs) {
    return lhs.timer == rhs.timer &&
           lhs.channel == rhs.channel &&
           lhs.pin.port == rhs.pin.port &&
           lhs.pin.pin == rhs.pin.pin &&
           lhs.frequencyHz == rhs.frequencyHz;
}

#endif // PWMCONFIG_H
