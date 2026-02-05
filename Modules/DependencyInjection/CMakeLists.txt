cmake_minimum_required(VERSION 3.25)

if (TARGET module_di)
    message("module_di already exists, skipping...")
    return()
endif ()


set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)
set(CMAKE_CXX_STANDARD 23)

add_library(module_di STATIC)
set_register_args(module_di)

# Enables Logging
target_compile_definitions(module_di PRIVATE LOG_LEVEL=LOG_LEVEL_ERROR)

file(GLOB_RECURSE C_SOURCES
        ${CMAKE_CURRENT_LIST_DIR}/src/*.cpp
)

target_sources(module_di PUBLIC ${C_SOURCES})

target_link_libraries(module_di PUBLIC
        interface_di
)
