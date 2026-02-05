#ifndef TASKFACTORY_H
#define TASKFACTORY_H

#include "ETaskNotifyBits.h"
#include "Os.h"
#include "TaskInstance.h"
#include "TaskDefs.h"

/**
 * @brief TaskFactory
 * Can be used to start temporary tasks with custom handlers, completion sources and cancellation support.
 */
class TaskFactory {
    public:
        /**
         * @brief Init the task factory (automatically handled when Run() is called for the first time)
         */
        static void Init();

        /**
         * @brief Run a new task
         * @param taskFunction The handler method to call within the task context
         * @param context Additional context data to use inside the handler
         * @param stackWords Total stack size for this task in words
         * @param priority Task priority
         * @param name Name of the task
         * @return A new TaskInstance*
         */
        static TaskInstance* Run(TaskFunction taskFunction, void* context,
                                 uint32_t stackWords = MAX_ASYNC_TASK_STACK_SIZE_WORDS,
                                 UBaseType_t priority = tskIDLE_PRIORITY + 1,
                                 const char* name = "AsyncTask");

        /**
         * @brief Cancel a task instance
         * @param task The task to cancel
         * @return true if cancel succeeded
         */
        static bool Cancel(TaskInstance* task);

        /**
         * @brief Notify a task with given bits, can be used with xTaskNotifyWait inside handler to pass notifications along
         * @param task The task to notify
         * @param value The value
         * @param action The notify action
         * @return True if succeeded
         */
        static bool Notify(TaskInstance* task, uint32_t value, eNotifyAction action = eSetBits);

        /**
         * @brief Checks if a task is running
         * @param task The task to check
         * @return true if it's running, false otherwise
         */
        static bool IsRunning(TaskInstance* task);

        /**
         * @brief Wait for the current task to be cancelled or until the given timeout
         * @param delayMs The delay after which timeout is triggered
         * @return True if timedout, false if cancelled
         */
        static bool WaitForCancelOrTimeout(uint32_t delayMs);

        /**
         * @brief checks if a task was cancelled
         * @param bits The bits to check, use xTaskNotifyWait to obtain bits
         * @return True if task bits contain cancelled, false otherwise
         */
        static bool IsCancelled(uint32_t bits);

        /**
         * @brief Checks if a task finished (all the code in handler has run, regardless of status)
         * @param bits the bits to check, use xTaskNotifyWait to obtain bits
         * @return True if finished, false otherwise
         */
        static bool IsFinished(uint32_t bits);

        /**
         * @brief check if a task completed (CompletionSource == true)
         * @param task The task instance to check
         * @return True if the task completed, false otherwise
         */
        static bool IsCompleted(TaskInstance* task);

        /**
         * @brief delay a task by given milliseconds. Triggers pdMS_TO_TICKS() internally
         * @param delayMs The delay to wait for
         */
        static void DelayMs(uint32_t delayMs);

    private:
        static TaskInstance _tasks[MAX_ASYNC_TASKS];
        static StackType_t _taskStacks[MAX_ASYNC_TASKS][MAX_ASYNC_TASK_STACK_SIZE_WORDS];
        static bool _initialized;

        static int GetAvailableIndex();
        static void TaskStarter(void* param);
};

#endif //TASKFACTORY_H