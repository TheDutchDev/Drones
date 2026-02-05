#include "DataModule.h"

DataModule::DataModule(std::shared_ptr<IMotorData> motor1,
                       std::shared_ptr<IMotorData> motor2,
                       std::shared_ptr<IMotorData> motor3,
                       std::shared_ptr<IMotorData> motor4)
    : Motor1(std::move(motor1)),
      Motor2(std::move(motor2)),
      Motor3(std::move(motor3)),
      Motor4(std::move(motor4)) {
}
