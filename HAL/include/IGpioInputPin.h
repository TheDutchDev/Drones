#ifndef IGPIOINPUTPIN_H
#define IGPIOINPUTPIN_H

#include "GpInputConfig.h"

class IGpioInputPin {
public:
    virtual ~IGpioInputPin() = default;
    virtual bool Initialize(const GpInputConfig &config) = 0;
    virtual bool Read() const = 0;
    virtual bool IsValid() const = 0;
};

#endif // IGPIOINPUTPIN_H
