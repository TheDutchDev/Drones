#ifndef IPWMPIN_H
#define IPWMPIN_H

#include "PwmConfig.h"

class IPwmPin {
public:
    virtual ~IPwmPin() = default;
    virtual bool Initialize(const PwmConfig &config) = 0;
    virtual bool IsValid() const = 0;
};

#endif // IPWMPIN_H
