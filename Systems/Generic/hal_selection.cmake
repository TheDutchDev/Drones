cmake_minimum_required(VERSION 3.25)

set(CMAKE_EXE_LINKER_FLAGS_INIT "")

if (NOT DEFINED HAL_TARGET)
    set(HAL_TARGET STM32F411)
endif ()

add_compile_definitions(HAL_TARGET=${HAL_TARGET})
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/../../HAL ${CMAKE_BINARY_DIR}/build_interface_hal)

if (HAL_TARGET MATCHES "STM32F411")
    message("Building for Default: STM32F411")

    set(MCU cortex-m4)
    set(STM32F4 TRUE)
    add_definitions(-DUSE_HAL_DRIVER)
    add_definitions(-DCORE_CM4)
    add_definitions(-D__CCMRAM__)

    add_definitions(-DSTM32F411xE)
    add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/../../HAL/STM32F411 ${CMAKE_BINARY_DIR}/build_stm32f411)
    target_link_libraries(firmware PRIVATE stm32f411_hal)

    set_target_properties(firmware PROPERTIES LINK_FLAGS "-Wl,--gc-sections")
    set_target_properties(firmware PROPERTIES SUFFIX ".elf")

    set(REGISTER_ARGS
            -mfpu=fpv4-sp-d16
            -mfloat-abi=hard
    )

    set(SHARED_OPTIONS
            -mcpu=${MCU}
            ${REGISTER_ARGS}
            -mthumb
            $<$<CONFIG:Release>:-Os>
            -g
            --specs=nano.specs
            --specs=nosys.specs
            -Wl,-e,Reset_Handler
            -Wl,--gc-sections
            -Wl,-Map=${CMAKE_BINARY_DIR}/firmware.map
            -Wl,--print-memory-usage
            -u _printf_float
    )

    target_link_options(firmware PRIVATE ${SHARED_OPTIONS})
    target_compile_options(firmware PRIVATE ${SHARED_OPTIONS})

    target_link_libraries(firmware PRIVATE nosys)
endif ()
