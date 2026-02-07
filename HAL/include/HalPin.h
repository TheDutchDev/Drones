#ifndef HALGPIO_H
#define HALGPIO_H

#include <cstdint>

#include "EHalPort.h"


struct HalPin {
    EHalPort port = EHalPort::A;
    uint8_t pin = 0;
};

#endif // HALGPIO_H
