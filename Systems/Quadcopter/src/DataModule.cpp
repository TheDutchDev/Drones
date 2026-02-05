#include "DataModule.h"

DataModule::DataModule(std::shared_ptr<IMotorData> motor1,
                       std::shared_ptr<IMotorData> motor2,
                       std::shared_ptr<IMotorData> motor3,
                       std::shared_ptr<IMotorData> motor4)
    : Motor1(std::move(motor1)),
      Motor2(std::move(motor2)),
      Motor3(std::move(motor3)),
      Motor4(std::move(motor4)) {
    HalPwmConfig motor1Cfg{};
    motor1Cfg.timer = HalTimer::Tim3;
    motor1Cfg.channel = HalTimerChannel::Ch1;
    motor1Cfg.pin = {HalPort::B, 4};
    motor1Cfg.frequencyHz = 50;

    HalPwmConfig motor2Cfg{};
    motor2Cfg.timer = HalTimer::Tim3;
    motor2Cfg.channel = HalTimerChannel::Ch2;
    motor2Cfg.pin = {HalPort::B, 5};
    motor2Cfg.frequencyHz = 50;

    HalPwmConfig motor3Cfg{};
    motor3Cfg.timer = HalTimer::Tim3;
    motor3Cfg.channel = HalTimerChannel::Ch3;
    motor3Cfg.pin = {HalPort::B, 0};
    motor3Cfg.frequencyHz = 50;

    HalPwmConfig motor4Cfg{};
    motor4Cfg.timer = HalTimer::Tim3;
    motor4Cfg.channel = HalTimerChannel::Ch4;
    motor4Cfg.pin = {HalPort::B, 1};
    motor4Cfg.frequencyHz = 50;

    Motor1->PwmConfig.Modify(motor1Cfg);
    Motor1->MaxRpm.Modify(12000.0f);
    Motor1->TargetRpm.Modify(0.0f);

    Motor2->PwmConfig.Modify(motor2Cfg);
    Motor2->MaxRpm.Modify(12000.0f);
    Motor2->TargetRpm.Modify(0.0f);

    Motor3->PwmConfig.Modify(motor3Cfg);
    Motor3->MaxRpm.Modify(12000.0f);
    Motor3->TargetRpm.Modify(0.0f);

    Motor4->PwmConfig.Modify(motor4Cfg);
    Motor4->MaxRpm.Modify(12000.0f);
    Motor4->TargetRpm.Modify(0.0f);
}
