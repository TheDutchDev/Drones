#ifndef IMOTORDATA_H
#define IMOTORDATA_H

#include "IDataModel.h"
#include "IFloatProperty.h"
#include "IProperty.h"
#include "PwmConfig.h"

class IMotorData : public IDataModel {

public:
    IMotorData() {
        RegisterProperty(MaxRpm);
        RegisterProperty(TargetRpm);
        RegisterProperty(MinDuty);
        RegisterProperty(MaxDuty);
        RegisterProperty(DisarmedDuty);

        IDataModel::HookDataModels();
    }

    IFloatProperty MaxRpm = IFloatProperty(100, 0, 100, EPropertyType::MotorMaxRpm);
    IFloatProperty TargetRpm = IFloatProperty(100, 0, 100, EPropertyType::MotorTargetRpm);
    IFloatProperty MinDuty = IFloatProperty(0.05f, 0, 100, EPropertyType::MotorMinDuty);
    IFloatProperty MaxDuty = IFloatProperty(0.10f, 0, 100, EPropertyType::MotorMaxDuty);
    IFloatProperty DisarmedDuty = IFloatProperty(0.0f, 0, 100, EPropertyType::MotorDisarmedDuty);

    PwmConfig Pwm{};
    bool PwmConfigured = false;
};

class IMoreMotorData : public IMotorData {

public:
    IMoreMotorData() {
        RegisterProperty(MaxRpm2);

        IDataModel::HookDataModels();
    }

    IFloatProperty MaxRpm2 = IFloatProperty(100, 0, 100, EPropertyType::MotorMaxRpm);
};

class IThirdMotorData : public IMoreMotorData {

public:
    IThirdMotorData() {
        RegisterProperty(MaxRpm3);

        IDataModel::HookDataModels();
    }

    IFloatProperty MaxRpm3 = IFloatProperty(100, 0, 100, EPropertyType::MotorMaxRpm);
};

#endif // IMOTORDATA_H
