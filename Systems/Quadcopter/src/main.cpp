#include "DataModule.h"
#include "IMotor.h"
#include "IocContainer.h"
#include "ILogger.h"
#include "ISystem.h"
#include "MotorTask.h"
#include "Os.h"
#include "PwmProviderAdapter.h"
#include "RS2205BLDCMotor.h"
void RegisterServices();

int main() {
    InitializeSystem();
    InitializeLogger();

    RegisterServices();

    LOG_FATAL("Start");
    auto data = dynamic_pointer_cast<DataModule>(container->Resolve<IDataModule>());
    data->MotorTask1 = StartMotorTask(data->Motor1, "Motor1");
    data->MotorTask2 = StartMotorTask(data->Motor2, "Motor2");
    data->MotorTask3 = StartMotorTask(data->Motor3, "Motor3");
    data->MotorTask4 = StartMotorTask(data->Motor4, "Motor4");

    LOG_FATAL("HELLO");
    vTaskStartScheduler();

    // Should never reach this point since FreeRTOS Tasks should have started here
    LOG_FATAL("Application failed to start");
    while(1);
}

void RegisterServices() {
    container->RegisterTransientClass<IMotorData>();
    container->RegisterSingletonWithInterface<IDataModule, DataModule, IMotorData, IMotorData, IMotorData, IMotorData>();
    container->RegisterSingletonWithInterface<IPwmProvider, HalPwmProviderAdapter>();
    container->RegisterTransientWithInterface<IMotor, RS2205BLDCMotor, IMotorData, IPwmProvider>();
}
