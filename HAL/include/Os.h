#ifndef OS_H
#define OS_H

#include "FreeRTOS.h"
#include <semphr.h>

#define TCB_SIZE_BYTES sizeof(StaticTask_t)

#if defined(__CCMRAM__)
#define FREERTOS_STACK __attribute__((section(".ccmram")))
#else
#define FREERTOS_STACK
#endif

#endif //OS_H
