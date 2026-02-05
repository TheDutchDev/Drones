#ifndef PWM_PROVIDER_ADAPTER_H
#define PWM_PROVIDER_ADAPTER_H

#include "IPwmProvider.h"

class HalPwmProviderAdapter final : public IPwmProvider {
public:
    PwmOutput Create(const HalPwmConfig &config) override {
        auto *provider = GetPwmProvider();
        return provider ? provider->Create(config) : PwmOutput{};
    }
};

#endif // PWM_PROVIDER_ADAPTER_H
