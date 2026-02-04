#ifndef DATAMODULE_H
#define DATAMODULE_H

#include "MotorData.h"
#include <cstddef>

class DataModule {
public:
    static constexpr size_t MotorCount = 4;

    MotorData &GetMotor(size_t index);
    const MotorData &GetMotor(size_t index) const;

private:
    MotorData Motors[MotorCount]{};
};

#endif // DATAMODULE_H
