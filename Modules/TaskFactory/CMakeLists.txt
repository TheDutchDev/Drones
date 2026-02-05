cmake_minimum_required(VERSION 3.25)

if (TARGET module_task_factory)
    message("module_task_factory already exists, skipping...")
    return()
endif ()

set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)
set(CMAKE_CXX_STANDARD 23)

add_library(module_task_factory STATIC)
set_register_args(module_task_factory)

# Enables Logging
target_compile_definitions(module_task_factory PRIVATE LOG_LEVEL=LOG_LEVEL_ERROR)

target_include_directories(module_task_factory PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}/include
)

file(GLOB_RECURSE C_SOURCES
        ${CMAKE_CURRENT_LIST_DIR}/src/*.cpp
)

target_sources(module_task_factory PRIVATE ${C_SOURCES})

add_events(module_task_factory)

target_link_libraries(module_task_factory
        PRIVATE
        interface_hal
)