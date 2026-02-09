#include "DataModule.h"
#include "IMotor.h"
#include "IocContainer.h"
#include "ILogger.h"
#include "ISystem.h"
#include "MotorTask.h"
#include "Os.h"
#include "RS2205BLDCMotor.h"

void RegisterServices();

int main() {
    InitializeSystem();
    InitializeLogger();

    RegisterServices();

    auto motorData = container->Resolve<IThirdMotorData>();
    auto motorData2 = container->Resolve<IMoreMotorData>();
    auto motorData3 = container->Resolve<IMotorData>();
    auto motorData4 = container->Resolve<IMotorData>();
    auto test = container->Resolve<IDataModule>(motorData, motorData2, motorData3, motorData4);

    motorData->TargetRpm.Modify(1);
    motorData4->TargetRpm.Modify(13);
    motorData2->MaxRpm2.Modify(69);
    motorData3->PwmConfigured = true;

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
    container->RegisterTransientClass<IThirdMotorData>();
    container->RegisterAdditional<IThirdMotorData, IMoreMotorData>();
    container->RegisterAdditional<IThirdMotorData, IMotorData>();
    container->RegisterSingletonWithInterface<IDataModule, DataModule, IMotorData, IMotorData, IMotorData, IMotorData>();
    container->RegisterTransientWithInterface<IMotor, RS2205BLDCMotor, IMoreMotorData>();
}
