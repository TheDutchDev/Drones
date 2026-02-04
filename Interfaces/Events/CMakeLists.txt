cmake_minimum_required(VERSION 3.25)

if (TARGET interface_events)
    message("interface_events already exists, skipping...")
    return()
endif ()

add_library(interface_events STATIC ${CMAKE_CURRENT_LIST_DIR}/include)
set_register_args(interface_events)

target_include_directories(interface_events
        PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}/include
)
