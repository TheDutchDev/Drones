#include "MotorData.h"

MotorData::MotorData() {
    Mutex = xSemaphoreCreateMutex();
    PropertyLock lock{};
    lock.context = Mutex;
    lock.Lock = [](void *ctx) { xSemaphoreTake(static_cast<SemaphoreHandle_t>(ctx), portMAX_DELAY); };
    lock.Unlock = [](void *ctx) { xSemaphoreGive(static_cast<SemaphoreHandle_t>(ctx)); };

    Config.SetLock(lock);
    MaxRpmValue.SetLock(lock);
    TargetRpmValue.SetLock(lock);
}

MotorData::~MotorData() {
    if (Mutex) {
        vSemaphoreDelete(Mutex);
    }
}

ITypedProperty<HalPwmConfig> &MotorData::PwmConfig() {
    return Config;
}

IFloatProperty &MotorData::MaxRpm() {
    return MaxRpmValue;
}

IFloatProperty &MotorData::TargetRpm() {
    return TargetRpmValue;
}
