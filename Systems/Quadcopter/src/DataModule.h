#ifndef DATAMODULE_H
#define DATAMODULE_H

#include "IMotorData.h"
#include "TaskInstance.h"
#include <cstddef>
#include <memory>

#include "IDataModule.h"
#include "PwmConfig.h"

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

        PwmConfig motor1Cfg{};
        motor1Cfg.timer = EHalTimer::Tim3;
        motor1Cfg.channel = EHalTimerChannel::Ch1;
        motor1Cfg.pin = {EHalPort::B, 4};
        motor1Cfg.frequencyHz = 50.0f;

        PwmConfig motor2Cfg{};
        motor2Cfg.timer = EHalTimer::Tim3;
        motor2Cfg.channel = EHalTimerChannel::Ch2;
        motor2Cfg.pin = {EHalPort::B, 5};
        motor2Cfg.frequencyHz = 50.0f;

        PwmConfig motor3Cfg{};
        motor3Cfg.timer = EHalTimer::Tim3;
        motor3Cfg.channel = EHalTimerChannel::Ch3;
        motor3Cfg.pin = {EHalPort::B, 0};
        motor3Cfg.frequencyHz = 50.0f;

        PwmConfig motor4Cfg{};
        motor4Cfg.timer = EHalTimer::Tim3;
        motor4Cfg.channel = EHalTimerChannel::Ch4;
        motor4Cfg.pin = {EHalPort::B, 1};
        motor4Cfg.frequencyHz = 50.0f;

        Motor1->Pwm = motor1Cfg;
        Motor1->PwmConfigured = true;
        Motor2->Pwm = motor2Cfg;
        Motor2->PwmConfigured = true;
        Motor3->Pwm = motor3Cfg;
        Motor3->PwmConfigured = true;
        Motor4->Pwm = motor4Cfg;
        Motor4->PwmConfigured = true;
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
