#include "TaskFactory.h"
#include "ETaskNotifyBits.h"
#include "ILogger.h"

FREERTOS_STACK TaskInstance TaskFactory::_tasks[MAX_ASYNC_TASKS];
FREERTOS_STACK StackType_t TaskFactory::_taskStacks[MAX_ASYNC_TASKS][MAX_ASYNC_TASK_STACK_SIZE_WORDS];
bool TaskFactory::_initialized = false;

void TaskFactory::Init() {
    if(_initialized)
        return;

    for(int i = 0; i < MAX_ASYNC_TASKS; ++i) {
        _tasks[i].IsInUse = false;
        _tasks[i].Function = nullptr;
        _tasks[i].UserContext = nullptr;
        _tasks[i].StackPtr = _taskStacks[i];
        _tasks[i].StackWords = MAX_ASYNC_TASK_STACK_SIZE_WORDS;
        _tasks[i].Handle = nullptr;
    }
    _initialized = true;
}

int TaskFactory::GetAvailableIndex() {
    for(int i = 0; i < MAX_ASYNC_TASKS; ++i) {
        if(!_tasks[i].IsInUse) {
            return i;
        }
    }
    return INVALID_TASK_ID;
}

TaskInstance *TaskFactory::Run(TaskFunction taskFunction, void *context,
                               uint32_t stackWords,
                               UBaseType_t priority,
                               const char *name) {
    Init();

    const int index = GetAvailableIndex();

    if(index == INVALID_TASK_ID) {
        LOG_ERROR("TaskFactory::Run -> Can't start any more tasks, ran out of available task space!");
        return nullptr;
    }

    if(stackWords > MAX_ASYNC_TASK_STACK_SIZE_WORDS)
        stackWords = MAX_ASYNC_TASK_STACK_SIZE_WORDS;

    _tasks[index].IsInUse = true;
    _tasks[index].Function = taskFunction;
    _tasks[index].UserContext = context;
    _tasks[index].StackWords = stackWords;
    _tasks[index].CompletionSource.store(false);

    TaskHandle_t task = xTaskCreateStatic(
            TaskStarter,
            name,
            stackWords,
            (void *)(intptr_t)index, // param = index index
            priority,
            _tasks[index].StackPtr,
            &_tasks[index].Tcb
    );

    _tasks[index].Handle = task;
    return &_tasks[index];
}

void TaskFactory::TaskStarter(void *param) {
    int index = static_cast<int>((intptr_t)param);

    TaskInstance &task = _tasks[index];
    task.Handle = xTaskGetCurrentTaskHandle();

    xTaskNotifyStateClear(nullptr);

    if(task.Function) {
        auto taskSucceeded = task.Function(task.UserContext);
        task.CompletionSource.store(taskSucceeded);
    }
    else
        task.CompletionSource.store(false);

    xTaskNotify(task.Handle, NOTIFY_FINISHED, eSetBits);

    task.Handle = nullptr;
    task.IsInUse = false;
    task.Function = nullptr;
    task.UserContext = nullptr;
    vTaskDelete(nullptr);
}

bool TaskFactory::Cancel(TaskInstance *task) {
    if(task == nullptr)
        return false;

    if(!task->IsInUse || task->Handle == nullptr)
        return false;

    // Read current notification bits without clearing
    uint32_t currentBits = 0;
    xTaskNotifyWait(0, 0, &currentBits, 0);

    if(IsCancelled(currentBits))
        return true;

    auto result = xTaskNotify(task->Handle, NOTIFY_CANCEL, eSetBits);

    return result == pdPASS;
}

bool TaskFactory::Notify(TaskInstance *task, uint32_t value, eNotifyAction action) {
    if(task == nullptr)
        return false;

    if(!task->IsInUse || task->Handle == nullptr)
        return false;

    return xTaskNotify(task->Handle, value, action) == pdPASS;
}

bool TaskFactory::WaitForCancelOrTimeout(const uint32_t delayMs) {
    TickType_t ticks = pdMS_TO_TICKS(delayMs);

    uint32_t bits = 0;
    BaseType_t notified = xTaskNotifyWait(0, 0, &bits, ticks);

    return notified == pdTRUE && (bits & NOTIFY_CANCEL);
}

bool TaskFactory::IsCancelled(const uint32_t bits) {
    return (bits & NOTIFY_CANCEL) != 0;
}

bool TaskFactory::IsFinished(const uint32_t bits) {
    return (bits & NOTIFY_FINISHED) != 0;
}

bool TaskFactory::IsCompleted(TaskInstance *task) {
    if(task == nullptr)
        return false;

    return task->CompletionSource.load();
}

void TaskFactory::DelayMs(const uint32_t delayMs) {
    vTaskDelay(pdMS_TO_TICKS(delayMs));
}

bool TaskFactory::IsRunning(TaskInstance *task) {
    if(task == nullptr)
        return false;

    return task->IsInUse;
}