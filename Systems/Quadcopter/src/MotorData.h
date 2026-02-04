#ifndef MOTORDATA_H
#define MOTORDATA_H

#include "IMotorData.h"
#include "Property.h"
#include "Os.h"

class MotorData final : public IMotorData {
public:
    MotorData();
    ~MotorData() override;

    ITypedProperty<HalPwmConfig> &PwmConfig() override;
    IFloatProperty &MaxRpm() override;
    IFloatProperty &TargetRpm() override;

private:
    mutable SemaphoreHandle_t Mutex = nullptr;
    Property<HalPwmConfig> Config{};
    Property<float> MaxRpmValue{};
    Property<float> TargetRpmValue{};
};

#endif // MOTORDATA_H
