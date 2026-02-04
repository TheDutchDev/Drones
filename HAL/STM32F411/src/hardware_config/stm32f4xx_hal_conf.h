#ifndef __STM32F4xx_HAL_CONF_H
#define __STM32F4xx_HAL_CONF_H

#ifdef __cplusplus
extern "C" {
#endif

  /* ########################## Module Selection ############################## */

#define HAL_MODULE_ENABLED
#define HAL_DMA_MODULE_ENABLED
#define HAL_RCC_MODULE_ENABLED
#define HAL_GPIO_MODULE_ENABLED
#define HAL_PWR_MODULE_ENABLED
#define HAL_FLASH_MODULE_ENABLED
#define HAL_UART_MODULE_ENABLED
#define HAL_CORTEX_MODULE_ENABLED

  /* ########################## Oscillator Values ############################## */

#if !defined  (HSE_VALUE)
#define HSE_VALUE    25000000U   /* External crystal frequency */
#endif

#if !defined  (HSI_VALUE)
#define HSI_VALUE    16000000U
#endif

#if !defined  (LSI_VALUE)
#define LSI_VALUE    32000U
#endif

#if !defined  (LSE_VALUE)
#define LSE_VALUE    32768U
#endif

#define HSE_STARTUP_TIMEOUT    100U
#define HSI_STARTUP_TIMEOUT    100U
#define LSI_STARTUP_TIMEOUT    100U
#define LSE_STARTUP_TIMEOUT    5000U

  /* ########################### System Configuration ########################## */

#define  VDD_VALUE                    3300U
#define  TICK_INT_PRIORITY            0x0FU
#define  USE_RTOS                     0
#define  PREFETCH_ENABLE              1
#define  INSTRUCTION_CACHE_ENABLE     1
#define  DATA_CACHE_ENABLE            1

  /* ########################## Assert Selection ############################### */

#define USE_FULL_ASSERT    0
#if (USE_FULL_ASSERT == 0)
#define assert_param(expr) ((void)0)
#endif
  /* ########################## Includes ###################################### */

#include "stm32f4xx_hal_rcc.h"
#include "stm32f4xx_hal_gpio.h"
#include "stm32f4xx_hal_dma.h"
#include "stm32f4xx_hal_pwr.h"
#include "stm32f4xx_hal_flash.h"
#include "stm32f4xx_hal_uart.h"
#include "stm32f4xx_hal_cortex.h"

  /* ########################## UART Configuration ############################ */

#define HAL_UART_MODULE_ENABLED

  /* ########################## Callback Configuration ######################## */

#define USE_HAL_UART_REGISTER_CALLBACKS 0

  /* ########################## Time Base Configuration ######################## */

  /* ########################## MSP Override ################################## */

#define USE_HAL_MspInit      1

#ifdef __cplusplus
}
#endif

#endif /* __STM32F4xx_HAL_CONF_H */
