#ifndef DATAMODULE_H
#define DATAMODULE_H

#include "IMotorData.h"
#include "TaskInstance.h"
#include <cstddef>
#include <memory>

#include "IDataModule.h"

class DataModule : public IDataModule {
public:
    DataModule(std::shared_ptr<IMotorData> motor1,
               std::shared_ptr<IMotorData> motor2,
               std::shared_ptr<IMotorData> motor3,
               std::shared_ptr<IMotorData> motor4
    ) : Motor1(std::move(motor1)),
        Motor2(std::move(motor2)),
        Motor3(std::move(motor3)),
        Motor4(std::move(motor4)) {

        _dataModels.push_back(Motor1);
        _dataModels.push_back(Motor2);
        _dataModels.push_back(Motor3);
        _dataModels.push_back(Motor4);

        IDataModule::HookDataModels();

        // HalPwmConfig motor1Cfg{};
        // motor1Cfg.timer = HalTimer::Tim3;
        // motor1Cfg.channel = HalTimerChannel::Ch1;
        // motor1Cfg.pin = {HalPort::B, 4};
        // motor1Cfg.frequencyHz = 50;
        //
        // HalPwmConfig motor2Cfg{};
        // motor2Cfg.timer = HalTimer::Tim3;
        // motor2Cfg.channel = HalTimerChannel::Ch2;
        // motor2Cfg.pin = {HalPort::B, 5};
        // motor2Cfg.frequencyHz = 50;
        //
        // HalPwmConfig motor3Cfg{};
        // motor3Cfg.timer = HalTimer::Tim3;
        // motor3Cfg.channel = HalTimerChannel::Ch3;
        // motor3Cfg.pin = {HalPort::B, 0};
        // motor3Cfg.frequencyHz = 50;
        //
        // HalPwmConfig motor4Cfg{};
        // motor4Cfg.timer = HalTimer::Tim3;
        // motor4Cfg.channel = HalTimerChannel::Ch4;
        // motor4Cfg.pin = {HalPort::B, 1};
        // motor4Cfg.frequencyHz = 50;
    }


    std::shared_ptr<IMotorData> Motor1;
    std::shared_ptr<IMotorData> Motor2;
    std::shared_ptr<IMotorData> Motor3;
    std::shared_ptr<IMotorData> Motor4;

    TaskInstance *MotorTask1 = nullptr;
    TaskInstance *MotorTask2 = nullptr;
    TaskInstance *MotorTask3 = nullptr;
    TaskInstance *MotorTask4 = nullptr;
};

#endif // DATAMODULE_H
