#ifndef IPWMPROVIDER_H
#define IPWMPROVIDER_H

#include "PwmConfig.h"
#include "PwmOutput.h"

class IPwmProvider {
public:
    virtual ~IPwmProvider() = default;
    virtual PwmOutput Create(const HalPwmConfig &config) = 0;
};

IPwmProvider *GetPwmProvider();

#endif // IPWMPROVIDER_H
