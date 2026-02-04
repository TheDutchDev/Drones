#include "FreeRTOS.h"
#include <task.h>

__attribute__((section(".ccmram"))) uint8_t ucHeap[ configTOTAL_HEAP_SIZE ];

__attribute__((section(".ccmram"))) static StaticTask_t IdleTaskTCB;
__attribute__((section(".ccmram"))) static StackType_t IdleTaskStack[configMINIMAL_STACK_SIZE];

void vApplicationGetIdleTaskMemory(StaticTask_t **ppxIdleTaskTCBBuffer,
                                   StackType_t **ppxIdleTaskStackBuffer,
                                   uint32_t *pulIdleTaskStackSize)
{
    *ppxIdleTaskTCBBuffer = &IdleTaskTCB;
    *ppxIdleTaskStackBuffer = IdleTaskStack;
    *pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
}

#if (configUSE_TIMERS == 1)

__attribute__((section(".ccmram"))) static StaticTask_t TimerTaskTCB;
__attribute__((section(".ccmram"))) static StackType_t TimerTaskStack[configTIMER_TASK_STACK_DEPTH];

void vApplicationGetTimerTaskMemory(StaticTask_t **ppxTimerTaskTCBBuffer,
                                    StackType_t **ppxTimerTaskStackBuffer,
                                    uint32_t *pulTimerTaskStackSize)
{
    *ppxTimerTaskTCBBuffer = &TimerTaskTCB;
    *ppxTimerTaskStackBuffer = TimerTaskStack;
    *pulTimerTaskStackSize = configTIMER_TASK_STACK_DEPTH;
}
#endif

/******************************************************************************/
/* FreeRTOS Hooks                                                             */
/******************************************************************************/
void vApplicationTickHook(void) {
    //    HAL_IncTick();
}

void vApplicationStackOverflowHook(TaskHandle_t pxTask,
                                   char *pcTaskName) {
    (void)pcTaskName;
    (void)pxTask;

    /* Run time stack overflow checking is performed if
     * configCHECK_FOR_STACK_OVERFLOW is defined to 1 or 2.  This hook
     * function is called if a stack overflow is detected. */

    /* Force an assert. */
    // Handle stack overflow (e.g., log the error, halt the system, etc.)
    //    taskDISABLE_INTERRUPTS();
    for(;;);
}