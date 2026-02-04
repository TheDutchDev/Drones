#ifndef RS2205BLDCMOTOR_H
#define RS2205BLDCMOTOR_H

#include <cstdint>

#include "IMotor.h"
#include "IMotorData.h"
#include "IProperty.h"
#include "PwmOutput.h"

class RS2205BLDCMotor final : public IMotor {
public:
    RS2205BLDCMotor() = default;
    explicit RS2205BLDCMotor(PwmOutput pwm, IMotorData *data = nullptr,
                             float minDuty = 0.05f, float maxDuty = 0.10f, float disarmedDuty = 0.0f);

    void Configure(PwmOutput pwm, IMotorData *data,
                   float minDuty = 0.05f, float maxDuty = 0.10f, float disarmedDuty = 0.0f);

    void Initialize() override;
    void Arm() override;
    void Disarm() override;
    void SetThrottle(float normalized) override;
    bool IsArmed() const override;

    void ApplyTarget();

private:
    void WriteDuty(float duty);
    void SubscribeToData(IMotorData *data);
    void UnsubscribeFromData();
    void OnPropertyModified(IProperty *property);

    PwmOutput Pwm{};
    IMotorData *Data = nullptr;
    IProperty *PwmConfigProperty = nullptr;
    IProperty *TargetProperty = nullptr;
    IProperty *MaxRpmProperty = nullptr;
    HalPwmConfig PwmConfigValue{};
    float TargetRpm = 0.0f;
    float MaxRpm = 0.0f;
    float MinDuty = 0.05f;
    float MaxDuty = 0.10f;
    float DisarmedDuty = 0.0f;
    bool Armed = false;
};

#endif // RS2205BLDCMOTOR_H
