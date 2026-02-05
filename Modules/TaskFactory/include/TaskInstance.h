#ifndef TASKINSTANCE_H
#define TASKINSTANCE_H

#include <atomic>

#include "Os.h"

using TaskFunction = bool(*)(void* userContext);

struct TaskInstance {
    bool IsInUse;
    TaskFunction Function;
    void* UserContext;
    StaticTask_t Tcb;
    StackType_t* StackPtr;
    uint32_t StackWords;
    TaskHandle_t Handle;
    std::atomic<bool> CompletionSource{false};
};

#endif //TASKINSTANCE_H