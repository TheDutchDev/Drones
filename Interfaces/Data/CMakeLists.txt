cmake_minimum_required(VERSION 3.25)

if (TARGET interface_data)
    message("interface_data already exists, skipping...")
    return()
endif ()

set(CMAKE_CXX_STANDARD 23)

add_library(interface_data STATIC ${CMAKE_CURRENT_LIST_DIR}/include)
set_register_args(interface_data)

target_include_directories(interface_data PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}/include
        ${CMAKE_CURRENT_LIST_DIR}/include/Properties
        ${CMAKE_CURRENT_LIST_DIR}/include/Combinations
        ${CMAKE_CURRENT_LIST_DIR}/include/Enums
)

add_events(interface_data)