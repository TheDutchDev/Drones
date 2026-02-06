
set(BASE_DIR ${CMAKE_CURRENT_LIST_DIR}/../../..)
set(CUBE_DIR ${BASE_DIR}/BSP/STM32CubeF4)

set(FREERTOS_KERNEL_PATH ${BASE_DIR}/FreeRTOS/FreeRTOS/Source/)

add_library(freertos_config STATIC ${CMAKE_CURRENT_LIST_DIR}/include)
set_register_args(freertos_config)

target_include_directories(freertos_config SYSTEM
        PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}/include  # Directory containing FreeRTOSConfig.h
)


# Select the heap port.  values between 1-4 will pick a heap.
set(FREERTOS_HEAP "4" CACHE STRING "" FORCE)

# Select the native compile PORT
set(FREERTOS_PORT "GCC_ARM_CM4F" CACHE STRING "" FORCE)

add_subdirectory(${FREERTOS_KERNEL_PATH} FreeRTOS-Kernel)

target_sources(freertos_kernel
        PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}/src/FreeRTOSHooks.c
        ${CMAKE_CURRENT_LIST_DIR}/src/FreeRTOSOverloads.cpp
)

set_property(TARGET freertos_kernel PROPERTY C_STANDARD 90)

target_compile_options(freertos_kernel PUBLIC
        -mcpu=${MCU}
        -mthumb
        -mfpu=fpv4-sp-d16
        -mfloat-abi=hard
)

# Linker options
target_link_options(freertos_kernel PUBLIC
        -mcpu=${MCU}
        -mthumb
        -mfpu=fpv4-sp-d16
        -mfloat-abi=hard
)

target_compile_options(freertos_config PUBLIC
        -mcpu=${MCU}
        -mthumb
        -mfpu=fpv4-sp-d16
        -mfloat-abi=hard
)

# Linker options
target_link_options(freertos_config PUBLIC
        -mcpu=${MCU}
        -mthumb
        -mfpu=fpv4-sp-d16
        -mfloat-abi=hard
)

########################################################################
# Overall Compile Options
# Note the compile option strategy is to error on everything and then
# Per library opt-out of things that are warnings/errors.
# This ensures that no matter what strategy for compilation you take, the
# builds will still occur.
#
# Only tested with GNU and Clang.
# Other options are https://cmake.org/cmake/help/latest/variable/CMAKE_LANG_COMPILER_ID.html#variable:CMAKE_%3CLANG%3E_COMPILER_ID
# Naming of compilers translation map:
#
#   FreeRTOS    | CMake
#   -------------------
#   CCS         | ?TBD?
#   GCC         | GNU, Clang, *Clang Others?
#   IAR         | IAR
#   Keil        | ARMCC
#   MSVC        | MSVC # Note only for MinGW?
#   Renesas     | ?TBD?

target_compile_options(freertos_kernel PRIVATE
        # Suppressions required to build clean with clang.
        $<$<COMPILE_LANG_AND_ID:C,Clang>:-Wno-unused-macros>
        $<$<COMPILE_LANG_AND_ID:C,Clang>:-Wno-padded>
        $<$<COMPILE_LANG_AND_ID:C,Clang>:-Wno-missing-variable-declarations>
        $<$<COMPILE_LANG_AND_ID:C,Clang>:-Wno-covered-switch-default>
        $<$<COMPILE_LANG_AND_ID:C,Clang>:-Wno-cast-align>
)