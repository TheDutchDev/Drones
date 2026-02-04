if (TARGET stm32f411_hal)
    message("stm32f411_hal already exists, skipping...")
    return()
endif ()

add_library(stm32f411_hal STATIC)
set_register_args(stm32f411_hal)

set(BASE_DIR ${CMAKE_CURRENT_LIST_DIR}/../..)
set(CUBE_DIR ${BASE_DIR}/BSP/STM32CubeF4)
set(DRIVER_DIR ${CUBE_DIR}/Drivers/STM32F4xx_HAL_Driver)
set(CMSIS_DIR ${CUBE_DIR}/Drivers/CMSIS)
set(CMSIS_DSP_DIR ${CUBE_DIR}/Drivers/CMSIS/DSP)
set(DEVICE_DIR ${CMSIS_DIR}/Device/ST/STM32F4xx)
set(STARTUP_DIR ${DEVICE_DIR}/Source/Templates/gcc)
set(CMAKE_TOOLCHAIN_FILE ${BASE_DIR}/Toolchains/arm-none-eabi.cmake)

set(STM32_FREERTOS_DIR ${CUBE_DIR}/Middlewares/Third_Party/FreeRTOS/Source)
set(HAL_CONF_FILE ${CMAKE_CURRENT_LIST_DIR}/src/hardware_config/stm32f4xx_hal_conf.h)

target_include_directories(stm32f411_hal SYSTEM
        PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}
        ${STM32_FREERTOS_DIR}/include
        ${STM32_FREERTOS_DIR}/CMSIS_RTOS_V2
        ${STM32_FREERTOS_DIR}/portable/GCC/ARM_CM4F
        ${CMAKE_CURRENT_LIST_DIR}/../include
        ${CMAKE_CURRENT_LIST_DIR}/src
        ${CMAKE_CURRENT_LIST_DIR}/src/hardware_config
        ${CMAKE_CURRENT_LIST_DIR}/include
        ${DRIVER_DIR}/Inc
        ${DRIVER_DIR}/Inc/Legacy
        ${CMSIS_DIR}/Include
        ${CMSIS_DSP_DIR}/Include
        ${DEVICE_DIR}/Include
)

set(STM32_PRIVATE_SOURCES)

file(GLOB_RECURSE STM32_PUBLIC_SOURCES
        ${CMAKE_CURRENT_LIST_DIR}/src/*.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/**/*.cpp
        ${DRIVER_DIR}/Src/stm32f4xx_hal.c
        ${DRIVER_DIR}/Src/stm32f4xx_hal_cortex.c
        ${DRIVER_DIR}/Src/stm32f4xx_hal_uart.c
        ${DRIVER_DIR}/Src/stm32f4xx_hal_rcc.c
        ${DRIVER_DIR}/Src/stm32f4xx_hal_pwr.c
        ${DRIVER_DIR}/Src/stm32f4xx_hal_flash.c
        ${DRIVER_DIR}/Src/stm32f4xx_hal_gpio.c
        ${DRIVER_DIR}/Src/stm32f4xx_hal_tim.c
        ${DRIVER_DIR}/Src/stm32f4xx_hal_dma.c
        ${DRIVER_DIR}/Src/stm32f4xx_hal_iwdg.c
        ${DEVICE_DIR}/Source/Templates/*.c
)

file(GLOB_RECURSE ASM_SOURCES
        "${STARTUP_DIR}/startup_stm32f411xe.s"
)

target_sources(stm32f411_hal
        PRIVATE
        ${ASM_SOURCES}

        PUBLIC
        ${STM32_PUBLIC_SOURCES}
)

target_link_options(stm32f411_hal PUBLIC -T${CMAKE_CURRENT_LIST_DIR}/STM32F411_FLASH.ld)

add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/FreeRTOS)

target_link_libraries(stm32f411_hal
        PUBLIC
        freertos_kernel

        PRIVATE
        freertos_config
)
