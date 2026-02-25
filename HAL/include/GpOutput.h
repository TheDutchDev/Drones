#ifndef GPOUTPUT_H
#define GPOUTPUT_H

#include <memory>

#include "IGpOutput.h"
#include "IPwmPin.h"
#include "IPwmTimer.h"
#include "PwmConfig.h"

class GpOutput final : public IGpOutput {
public:
    explicit GpOutput(const PwmConfig &config);
    bool Initialize() override;
    void SetDuty(float duty) override;
    bool IsValid() const override;

private:
    PwmConfig _config{};
    std::unique_ptr<IPwmPin> _pwmPin{};
    std::unique_ptr<IPwmTimer> _pwmTimer{};
    bool _initialized = false;
};

#endif // GPOUTPUT_H
