#include "MotorTask.h"

#include <utility>

#include "IMotor.h"
#include "IMotorData.h"
#include "IocContainer.h"
#include "TaskFactory.h"

static constexpr uint32_t MotorTaskStackWords = 512;
static constexpr UBaseType_t MotorTaskPriority = tskIDLE_PRIORITY + 2;

struct MotorTaskContext {
    std::shared_ptr<IMotorData> MotorData;
};

static bool MotorTask(void *userContext) {
    auto *parameters = static_cast<MotorTaskContext *>(userContext);

    auto motor = container->Resolve<IMotor>(parameters->MotorData);

    motor->Initialize();

    for (;;) {
        motor->Update();
        TaskFactory::DelayMs(5);
    }

    return true;
}

TaskInstance *StartMotorTask(std::shared_ptr<IMotorData> data, const char *name) {
    auto *context = new MotorTaskContext{};
    context->MotorData = std::move(data);

    return TaskFactory::Run(MotorTask, context, MotorTaskStackWords, MotorTaskPriority, name);
}
