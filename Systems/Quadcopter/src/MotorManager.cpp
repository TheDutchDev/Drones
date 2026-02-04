#include "MotorManager.h"

static constexpr uint32_t MotorTaskStack = 512;
static constexpr UBaseType_t MotorTaskPriority = 2;

void MotorManager::Initialize(DataModule &data, IPwmProvider *pwmProvider) {
    Data = &data;
    Pwm = pwmProvider;

    for (size_t i = 0; i < DataModule::MotorCount; ++i) {
        PwmOutput output{};
        if (Pwm) {
            output = Pwm->Create(Data->GetMotor(i).PwmConfig().Get());
        }

        Motors[i].Configure(output, &Data->GetMotor(i), 0.05f, 0.10f);
        Motors[i].Initialize();
        Motors[i].Arm();

        Tasks[i].motor = &Motors[i];
        Tasks[i].data = &Data->GetMotor(i);
        TargetProperties[i] = &Tasks[i].data->TargetRpm();
        MaxRpmProperties[i] = &Tasks[i].data->MaxRpm();
        Tasks[i].data->TargetRpm().OnModified().Subscribe(this, &MotorManager::OnTargetRpmChanged);
        Tasks[i].data->MaxRpm().OnModified().Subscribe(this, &MotorManager::OnTargetRpmChanged);

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
    Data->GetMotor(index).TargetRpm().Modify(rpm);
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
