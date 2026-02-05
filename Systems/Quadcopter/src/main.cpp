#include "DataModule.h"
#include "IPwmProvider.h"
#include "ILogger.h"
#include "ISystem.h"
#include "MotorManager.h"
#include "Os.h"

void RegisterServices();

static DataModule Data;
static MotorManager MotorControl;

int main() {
    InitializeSystem();
    InitializeLogger();

    RegisterServices();

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

    Data.Motors[0].PwmConfig.Modify(motor1Cfg);
    Data.Motors[0].MaxRpm.Modify(12000.0f);
    Data.Motors[0].TargetRpm.Modify(0.0f);

    Data.Motors[1].PwmConfig.Modify(motor2Cfg);
    Data.Motors[1].MaxRpm.Modify(12000.0f);
    Data.Motors[1].TargetRpm.Modify(0.0f);

    Data.Motors[2].PwmConfig.Modify(motor3Cfg);
    Data.Motors[2].MaxRpm.Modify(12000.0f);
    Data.Motors[2].TargetRpm.Modify(0.0f);

    Data.Motors[3].PwmConfig.Modify(motor4Cfg);
    Data.Motors[3].MaxRpm.Modify(12000.0f);
    Data.Motors[3].TargetRpm.Modify(0.0f);

    MotorControl.Initialize(Data, GetPwmProvider());
    MotorControl.StartAll();

    LOG_FATAL("HELLO");
    vTaskStartScheduler();

    // Should never reach this point since FreeRTOS Tasks should have started here
    LOG_FATAL("Application failed to start");
    while(1);
}

void RegisterServices() {
}
