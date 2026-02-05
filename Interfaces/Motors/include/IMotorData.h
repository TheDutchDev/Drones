#ifndef IMOTORDATA_H
#define IMOTORDATA_H

#include "PwmConfig.h"
#include "Property.h"

struct IMotorData {
    Property<HalPwmConfig> PwmConfig{};
    Property<float> MaxRpm{};
    Property<float> TargetRpm{};
};

#endif // IMOTORDATA_H
