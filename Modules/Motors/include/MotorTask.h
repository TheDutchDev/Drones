#ifndef MOTOR_TASK_H
#define MOTOR_TASK_H

#include <memory>

struct IMotorData;
struct TaskInstance;

TaskInstance *StartMotorTask(std::shared_ptr<IMotorData> data,
                             const char *name = "Motor");

#endif // MOTOR_TASK_H
