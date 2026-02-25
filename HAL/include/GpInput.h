#ifndef GPINPUT_H
#define GPINPUT_H

#include <memory>

#include "GpInputConfig.h"
#include "IGpioInputPin.h"
#include "IGpInput.h"

class GpInput final : public IGpInput {
public:
    explicit GpInput(const GpInputConfig &config);
    bool Initialize() override;
    bool Read() const override;
    bool IsValid() const override;

private:
    GpInputConfig _config{};
    std::unique_ptr<IGpioInputPin> _inputPin{};
    bool _initialized = false;
};

#endif // GPINPUT_H
