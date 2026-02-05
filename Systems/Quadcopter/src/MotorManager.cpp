#include "MotorManager.h"

static constexpr uint32_t MotorTaskStack = 512;
static constexpr UBaseType_t MotorTaskPriority = 2;

IMotorData *MotorManager::ResolveMotor(DataModule *data, size_t index) {
    if (!data) {
        return nullptr;
    }
    switch (index) {
        case 0:
            return data->Motor1.get();
        case 1:
            return data->Motor2.get();
        case 2:
            return data->Motor3.get();
        case 3:
            return data->Motor4.get();
        default:
            return nullptr;
    }
}

void MotorManager::Initialize(DataModule &data, IPwmProvider *pwmProvider) {
    Data = &data;
    Pwm = pwmProvider;

    for (size_t i = 0; i < DataModule::MotorCount; ++i) {
        IMotorData *motorData = ResolveMotor(Data, i);
        if (!motorData) {
            continue;
        }

        PwmOutput output{};
        if (Pwm) {
            output = Pwm->Create(motorData->PwmConfig.Get());
        }

        Motors[i].Configure(output, motorData, 0.05f, 0.10f);
        Motors[i].Initialize();
        Motors[i].Arm();

        Tasks[i].motor = &Motors[i];
        Tasks[i].data = motorData;
        TargetProperties[i] = &Tasks[i].data->TargetRpm;
        MaxRpmProperties[i] = &Tasks[i].data->MaxRpm;
        Tasks[i].data->TargetRpm.OnModified().Subscribe(this, &MotorManager::OnTargetRpmChanged);
        Tasks[i].data->MaxRpm.OnModified().Subscribe(this, &MotorManager::OnTargetRpmChanged);

        if (!TasksCreated) {
            xTaskCreate(MotorTask, "Motor", MotorTaskStack, &Tasks[i],
                        MotorTaskPriority, &Tasks[i].handle);
        }
    }

    TasksCreated = true;
}

void MotorManager::StartAll() {
    for (size_t i = 0; i < DataModule::MotorCount; ++i) {
        if (Tasks[i].handle) {
            vTaskResume(Tasks[i].handle);
            xTaskNotifyGive(Tasks[i].handle);
        }
    }
}

void MotorManager::StopAll() {
    for (size_t i = 0; i < DataModule::MotorCount; ++i) {
        if (Tasks[i].handle) {
            vTaskSuspend(Tasks[i].handle);
        }
    }
}

void MotorManager::SetTargetRpm(size_t index, float rpm) {
    if (!Data || index >= DataModule::MotorCount) {
        return;
    }
    IMotorData *motorData = ResolveMotor(Data, index);
    if (motorData) {
        motorData->TargetRpm.Modify(rpm);
    }
}

void MotorManager::MotorTask(void *param) {
    auto *ctx = static_cast<MotorTaskContext *>(param);
    for (;;) {
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
        ctx->motor->ApplyTarget();
    }
}

void MotorManager::OnTargetRpmChanged(IProperty *property) {
    for (size_t i = 0; i < DataModule::MotorCount; ++i) {
        if ((TargetProperties[i] == property || MaxRpmProperties[i] == property) && Tasks[i].handle) {
            xTaskNotifyGive(Tasks[i].handle);
        }
    }
}
