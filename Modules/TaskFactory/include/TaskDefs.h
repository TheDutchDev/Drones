#ifndef TASKDEFS_H
#define TASKDEFS_H

#include <cstdint>

#define INVALID_TASK_ID (-1)
constexpr int MAX_ASYNC_TASKS = 8;
constexpr uint32_t MAX_ASYNC_TASK_STACK_SIZE_WORDS = 256;

#endif //TASKDEFS_H