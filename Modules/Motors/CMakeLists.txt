cmake_minimum_required(VERSION 3.25)

if (TARGET module_motor_tasks)
    message("module_motor_tasks already exists, skipping...")
    return()
endif ()

set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)
set(CMAKE_CXX_STANDARD 23)

add_library(module_motor_tasks STATIC)
set_register_args(module_motor_tasks)

target_compile_definitions(module_motor_tasks PRIVATE LOG_LEVEL=LOG_LEVEL_ERROR)

file(GLOB_RECURSE C_SOURCES
        ${CMAKE_CURRENT_LIST_DIR}/src/*.cpp
)

target_sources(module_motor_tasks PUBLIC ${C_SOURCES})

target_include_directories(module_motor_tasks PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}/include
)

add_motors(module_motor_tasks FALSE)
add_task_factory(module_motor_tasks)

target_link_libraries(module_motor_tasks
        PUBLIC
        interface_di
        interface_hal
)
