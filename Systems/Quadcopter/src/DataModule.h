#ifndef DATAMODULE_H
#define DATAMODULE_H

#include "IMotorData.h"
#include <cstddef>

class DataModule {
public:
    static constexpr size_t MotorCount = 4;

    IMotorData Motors[MotorCount]{};
};

#endif // DATAMODULE_H
