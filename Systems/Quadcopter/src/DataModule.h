#ifndef DATAMODULE_H
#define DATAMODULE_H

#include "IMotorData.h"
#include <cstddef>
#include <memory>

class DataModule {
public:
    static constexpr size_t MotorCount = 4;

    DataModule(std::shared_ptr<IMotorData> motor1,
               std::shared_ptr<IMotorData> motor2,
               std::shared_ptr<IMotorData> motor3,
               std::shared_ptr<IMotorData> motor4);

    std::shared_ptr<IMotorData> Motor1;
    std::shared_ptr<IMotorData> Motor2;
    std::shared_ptr<IMotorData> Motor3;
    std::shared_ptr<IMotorData> Motor4;
};

#endif // DATAMODULE_H
