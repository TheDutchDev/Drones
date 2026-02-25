#include "GpInput.h"

#include <memory>

#include "Stm32GpioInputPin.h"

GpInput::GpInput(const GpInputConfig &config)
    : _config(config),
      _inputPin(std::make_unique<Stm32GpioInputPin>()) {
}

bool GpInput::Initialize() {
    if (_initialized) {
        return true;
    }

    if (_inputPin == nullptr) {
        return false;
    }
    if (!_inputPin->Initialize(_config) || !_inputPin->IsValid()) {
        return false;
    }

    _initialized = true;
    return true;
}

bool GpInput::Read() const {
    if (!IsValid()) {
        return false;
    }

    return _inputPin->Read();
}

bool GpInput::IsValid() const {
    if (!_initialized || _inputPin == nullptr) {
        return false;
    }
    return _inputPin->IsValid();
}
