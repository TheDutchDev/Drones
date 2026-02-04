#ifndef IMOTORDATA_H
#define IMOTORDATA_H

#include "PwmConfig.h"
#include "FloatProperty.h"
#include "ITypedProperty.h"

class IMotorData {
public:
    virtual ~IMotorData() = default;

    virtual ITypedProperty<HalPwmConfig> &PwmConfig() = 0;
    virtual IFloatProperty &MaxRpm() = 0;
    virtual IFloatProperty &TargetRpm() = 0;
};

#endif // IMOTORDATA_H
