#ifndef GPINPUTCONFIG_H
#define GPINPUTCONFIG_H

#include "HalPin.h"

enum class EGpPull {
    None,
    Up,
    Down
};

struct GpInputConfig {
    HalPin pin{};
    EGpPull pull = EGpPull::None;
};

#endif // GPINPUTCONFIG_H
