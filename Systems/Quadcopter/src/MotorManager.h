#ifndef MOTORMANAGER_H
#define MOTORMANAGER_H

#include "DataModule.h"
#include "IPwmProvider.h"
#include "RS2205BLDCMotor.h"
#include "Os.h"
#include <cstddef>

class MotorManager {
public:
    void Initialize(DataModule &data, IPwmProvider *pwmProvider);
    void StartAll();
    void StopAll();
    void SetTargetRpm(size_t index, float rpm);

private:
    struct MotorTaskContext {
        RS2205BLDCMotor *motor = nullptr;
        IMotorData *data = nullptr;
        TaskHandle_t handle = nullptr;
    };

    static void MotorTask(void *param);
    void OnTargetRpmChanged(IProperty *property);

    DataModule *Data = nullptr;
    IPwmProvider *Pwm = nullptr;
    RS2205BLDCMotor Motors[DataModule::MotorCount] = {
            RS2205BLDCMotor(PwmOutput{}),
            RS2205BLDCMotor(PwmOutput{}),
            RS2205BLDCMotor(PwmOutput{}),
            RS2205BLDCMotor(PwmOutput{})
    };
    MotorTaskContext Tasks[DataModule::MotorCount] = {};
    IProperty *TargetProperties[DataModule::MotorCount] = {};
    IProperty *MaxRpmProperties[DataModule::MotorCount] = {};
    bool TasksCreated = false;
};

#endif // MOTORMANAGER_H
