#include "RS2205BLDCMotor.h"

#include <algorithm>
#include <cmath>

#include "GpOutput.h"

RS2205BLDCMotor::RS2205BLDCMotor(const std::shared_ptr<IMoreMotorData>& motorData)
    : _motorData(motorData) {
    if (_motorData != nullptr) {
        _motorDataEventHandle = _motorData->OnPropertyModified->Subscribe(this, &RS2205BLDCMotor::OnPropertyModified);
    }
}

RS2205BLDCMotor::~RS2205BLDCMotor() {
    if (_motorData != nullptr && _motorDataEventHandle >= 0) {
        _motorData->OnPropertyModified->Unsubscribe(_motorDataEventHandle);
    }
}

void RS2205BLDCMotor::Initialize() {
    _initialized = false;
    _output.reset();

    if (_motorData == nullptr || !_motorData->PwmConfigured) {
        return;
    }

    auto output = std::make_unique<GpOutput>(_motorData->Pwm);
    if (!output->Initialize()) {
        return;
    }

    _output = std::move(output);
    _initialized = true;
    SetDuty(_motorData->DisarmedDuty.Value());
}

void RS2205BLDCMotor::Update() {
    ApplyTargetDuty();
}

void RS2205BLDCMotor::OnPropertyModified(IPropertyBase *property) {
    if (property == nullptr) {
        return;
    }

    switch (property->PropertyType) {
        case EPropertyType::MotorMaxRpm:
        case EPropertyType::MotorTargetRpm:
        case EPropertyType::MotorMinDuty:
        case EPropertyType::MotorMaxDuty:
        case EPropertyType::MotorDisarmedDuty:
            ApplyTargetDuty();
            break;
        default:
            break;
    }
}

float RS2205BLDCMotor::ComputeTargetDuty() const {
    const float maxRpm = _motorData->MaxRpm.Value();
    const float targetRpm = _motorData->TargetRpm.Value();
    const float minDuty = _motorData->MinDuty.Value();
    const float maxDuty = _motorData->MaxDuty.Value();
    const float disarmedDuty = _motorData->DisarmedDuty.Value();

    if (maxRpm <= 0.0f || targetRpm <= 0.0f) {
        return disarmedDuty;
    }

    const float ratio = std::clamp(targetRpm / maxRpm, 0.0f, 1.0f);
    const float low = std::min(minDuty, maxDuty);
    const float high = std::max(minDuty, maxDuty);
    const float duty = minDuty + (maxDuty - minDuty) * ratio;
    return std::clamp(duty, low, high);
}

void RS2205BLDCMotor::ApplyTargetDuty() {
    if (!_initialized || _motorData == nullptr) {
        return;
    }

    const float duty = ComputeTargetDuty();
    if (std::fabs(duty - _lastDuty) < 0.0001f) {
        return;
    }

    SetDuty(duty);
}

void RS2205BLDCMotor::SetDuty(float duty) {
    if (_output == nullptr || !_output->IsValid()) {
        return;
    }

    _output->SetDuty(duty);
    _lastDuty = duty;
}
