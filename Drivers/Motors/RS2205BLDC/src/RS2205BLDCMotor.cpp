#include "RS2205BLDCMotor.h"

static float Clamp01(float value) {
    if (value < 0.0f) {
        return 0.0f;
    }
    if (value > 1.0f) {
        return 1.0f;
    }
    return value;
}

RS2205BLDCMotor::RS2205BLDCMotor(PwmOutput pwm, IMotorData *data,
                                 float minDuty, float maxDuty, float disarmedDuty) {
    Configure(pwm, data, minDuty, maxDuty, disarmedDuty);
}

void RS2205BLDCMotor::Configure(PwmOutput pwm, IMotorData *data,
                                float minDuty, float maxDuty, float disarmedDuty) {
    UnsubscribeFromData();
    Pwm = pwm;
    MinDuty = minDuty;
    MaxDuty = maxDuty;
    DisarmedDuty = disarmedDuty;
    SubscribeToData(data);
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
    Data = data;
    if (!Data) {
        return;
    }

    PwmConfigProperty = &Data->PwmConfig;
    TargetProperty = &Data->TargetRpm;
    MaxRpmProperty = &Data->MaxRpm;

    PwmConfigValue = Data->PwmConfig.Get();
    TargetRpm = Data->TargetRpm.Get();
    MaxRpm = Data->MaxRpm.Get();

    Data->PwmConfig.OnModified().Subscribe(this, &RS2205BLDCMotor::OnPropertyModified);
    Data->TargetRpm.OnModified().Subscribe(this, &RS2205BLDCMotor::OnPropertyModified);
    Data->MaxRpm.OnModified().Subscribe(this, &RS2205BLDCMotor::OnPropertyModified);
}

void RS2205BLDCMotor::UnsubscribeFromData() {
    if (!Data) {
        return;
    }
    Data->PwmConfig.OnModified().Unsubscribe(this, &RS2205BLDCMotor::OnPropertyModified);
    Data->TargetRpm.OnModified().Unsubscribe(this, &RS2205BLDCMotor::OnPropertyModified);
    Data->MaxRpm.OnModified().Unsubscribe(this, &RS2205BLDCMotor::OnPropertyModified);
    Data = nullptr;
    PwmConfigProperty = nullptr;
    TargetProperty = nullptr;
    MaxRpmProperty = nullptr;
}

void RS2205BLDCMotor::OnPropertyModified(IProperty *property) {
    if (!Data) {
        return;
    }
    if (property == PwmConfigProperty) {
        PwmConfigValue = Data->PwmConfig.Get();
    } else if (property == TargetProperty) {
        TargetRpm = Data->TargetRpm.Get();
    } else if (property == MaxRpmProperty) {
        MaxRpm = Data->MaxRpm.Get();
    }
}
