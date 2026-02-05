#include "MotorTask.h"

#include <utility>

#include "IMotor.h"
#include "IMotorData.h"
#include "IocContainer.h"
#include "TaskFactory.h"

static constexpr uint32_t MotorTaskStackWords = 512;
static constexpr UBaseType_t MotorTaskPriority = tskIDLE_PRIORITY + 2;

struct MotorTaskContext {
    std::shared_ptr<IMotorData> Data{};
};

static bool MotorTask(void *userContext) {
    auto *ctx = static_cast<MotorTaskContext *>(userContext);
    if (!ctx || !ctx->Data || !container) {
        return false;
    }

    auto motor = container->Resolve<IMotor>(ctx->Data);
    if (!motor) {
        return false;
    }

    motor->Configure(0.05f, 0.10f, 0.0f);
    motor->Initialize();
    motor->Arm();

    for (;;) {
        const float maxRpm = ctx->Data->MaxRpm.Get();
        const float targetRpm = ctx->Data->TargetRpm.Get();
        const float throttle = (maxRpm > 0.0f) ? (targetRpm / maxRpm) : 0.0f;
        motor->SetThrottle(throttle);
        TaskFactory::DelayMs(5);
    }

    return true;
}

TaskInstance *StartMotorTask(std::shared_ptr<IMotorData> data,
                             const char *name) {
    if (!data) {
        return nullptr;
    }

    auto *context = new MotorTaskContext{};
    context->Data = std::move(data);

    return TaskFactory::Run(MotorTask, context, MotorTaskStackWords, MotorTaskPriority, name);
}
