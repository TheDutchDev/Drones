#ifndef HALGPIO_H
#define HALGPIO_H

#include <cstdint>

enum class HalPort : uint8_t {
    A,
    B,
    C,
    D,
    E,
    H,
};

struct HalPin {
    HalPort port = HalPort::A;
    uint8_t pin = 0;
};

#endif // HALGPIO_H
