cmake_minimum_required(VERSION 3.25)

if (TARGET interface_properties)
    message("interface_properties already exists, skipping...")
    return()
endif ()

add_library(interface_properties STATIC ${CMAKE_CURRENT_LIST_DIR}/include)
set_register_args(interface_properties)

target_include_directories(interface_properties
        PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}/include
)

add_events(interface_properties)
