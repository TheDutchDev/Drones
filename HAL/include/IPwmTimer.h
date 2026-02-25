#ifndef IPWMTIMER_H
#define IPWMTIMER_H

#include "PwmConfig.h"

class IPwmTimer {
public:
    virtual ~IPwmTimer() = default;
    virtual bool Initialize(const PwmConfig &config) = 0;
    virtual void SetDuty(float duty) = 0;
    virtual bool IsValid() const = 0;
};

#endif // IPWMTIMER_H
