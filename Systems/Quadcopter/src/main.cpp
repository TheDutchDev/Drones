#include <ILogger.h>
#include <ELogLevel.h>
#include <ISystem.h>
#include "Os.h"

void RegisterServices();

int main() {
    InitializeSystem();
    InitializeLogger();

    RegisterServices();

    LOG_FATAL("HELLO");
    vTaskStartScheduler();

    // Should never reach this point since FreeRTOS Tasks should have started here
    LOG_FATAL("Application failed to start");
    while(1);
}

void RegisterServices() {
}
