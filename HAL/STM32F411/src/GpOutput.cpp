#include "GpOutput.h"

#include <memory>

#include "Stm32PwmPin.h"
#include "Stm32PwmTimer.h"

GpOutput::GpOutput(const PwmConfig &config)
    : _config(config),
      _pwmPin(std::make_unique<Stm32PwmPin>()),
      _pwmTimer(std::make_unique<Stm32PwmTimer>()) {
}

bool GpOutput::Initialize() {
    if (_initialized) {
        return true;
    }

    if (_pwmPin == nullptr || _pwmTimer == nullptr) {
        return false;
    }
    if (!_pwmPin->Initialize(_config) || !_pwmPin->IsValid()) {
        return false;
    }
    if (!_pwmTimer->Initialize(_config) || !_pwmTimer->IsValid()) {
        return false;
    }

    _initialized = true;
    return true;
}

void GpOutput::SetDuty(float duty) {
    if (!IsValid()) {
        return;
    }

    _pwmTimer->SetDuty(duty);
}

bool GpOutput::IsValid() const {
    if (!_initialized) {
        return false;
    }
    if (_pwmPin == nullptr || _pwmTimer == nullptr) {
        return false;
    }
    return _pwmPin->IsValid() && _pwmTimer->IsValid();
}
