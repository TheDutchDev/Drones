#ifndef IMOTORDATA_H
#define IMOTORDATA_H

#include "IDataModel.h"
#include "IFloatProperty.h"
#include "IProperty.h"

class IMotorData : public IDataModel {

public:
    IMotorData() {
        RegisterProperty(MaxRpm);
        RegisterProperty(TargetRpm);

        IDataModel::HookDataModels();
    }

    IFloatProperty MaxRpm = IFloatProperty(100, 0, 100, EPropertyType::MaxRpm);
    IFloatProperty TargetRpm = IFloatProperty(100, 0, 100, EPropertyType::TargetRpm);
};

#endif // IMOTORDATA_H
