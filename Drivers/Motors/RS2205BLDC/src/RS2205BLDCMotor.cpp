#include "RS2205BLDCMotor.h"

#include <utility>

#include "IPwmProvider.h"

static float Clamp01(float value) {
    if (value < 0.0f) {
        return 0.0f;
    }
    if (value > 1.0f) {
        return 1.0f;
    }
    return value;
}

RS2205BLDCMotor::RS2205BLDCMotor(std::shared_ptr<IMotorData> data,
                                 std::shared_ptr<IPwmProvider> pwmProvider,
                                 float minDuty, float maxDuty, float disarmedDuty)
    : Data(std::move(data)),
      PwmProvider(std::move(pwmProvider)) {

    // Pwm = PwmProvider->Create(Data->PwmConfig.Get());
    Configure(minDuty, maxDuty, disarmedDuty);
    SubscribeToData(Data.get());
}

void RS2205BLDCMotor::Configure(float minDuty, float maxDuty, float disarmedDuty) {
    MinDuty = minDuty;
    MaxDuty = maxDuty;
    DisarmedDuty = disarmedDuty;
}

void RS2205BLDCMotor::Initialize() {
    Disarm();
}

void RS2205BLDCMotor::Arm() {
    Armed = true;
    WriteDuty(MinDuty);
}

void RS2205BLDCMotor::Disarm() {
    Armed = false;
    WriteDuty(DisarmedDuty);
}

void RS2205BLDCMotor::SetThrottle(float normalized) {
    if (!Armed) {
        return;
    }
    const float clamped = Clamp01(normalized);
    const float duty = MinDuty + ((MaxDuty - MinDuty) * clamped);
    WriteDuty(duty);
}

bool RS2205BLDCMotor::IsArmed() const {
    return Armed;
}

void RS2205BLDCMotor::WriteDuty(float duty) {
    if (Pwm.SetDuty) {
        Pwm.SetDuty(Pwm.context, duty);
    }
}

void RS2205BLDCMotor::ApplyTarget() {
    const float throttle = (MaxRpm > 0.0f) ? (TargetRpm / MaxRpm) : 0.0f;
    SetThrottle(throttle);
}

void RS2205BLDCMotor::SubscribeToData(IMotorData *data) {
    // PwmConfigValue = data->PwmConfig.Get();
    TargetRpm = data->TargetRpm.Value();
    MaxRpm = data->MaxRpm.Value();

    // data->PwmConfig.OnModified().Subscribe(this, &RS2205BLDCMotor::OnPropertyModified);
    data->TargetRpm.OnModified->Subscribe(this, &RS2205BLDCMotor::OnPropertyModified);
    data->MaxRpm.OnModified->Subscribe(this, &RS2205BLDCMotor::OnPropertyModified);
}

void RS2205BLDCMotor::UnsubscribeFromData() {

    // Data->PwmConfig.OnModified().Unsubscribe(this, &RS2205BLDCMotor::OnPropertyModified);
    // Data->TargetRpm.OnModified->Unsubscribe(this, &RS2205BLDCMotor::OnPropertyModified);
    // Data->MaxRpm.OnModified().Unsubscribe(this, &RS2205BLDCMotor::OnPropertyModified);
}

void RS2205BLDCMotor::OnPropertyModified(IPropertyBase *property) {

}
