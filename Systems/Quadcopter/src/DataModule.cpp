#include "DataModule.h"

MotorData &DataModule::GetMotor(size_t index) {
    return Motors[index];
}

const MotorData &DataModule::GetMotor(size_t index) const {
    return Motors[index];
}
