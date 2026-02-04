cmake_minimum_required(VERSION 3.25)

if (TARGET interface_motors)
    message("interface_motors already exists, skipping...")
    return()
endif ()

add_library(interface_motors STATIC)

target_sources(interface_motors
        PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}/src/IMotor.cpp
)

target_include_directories(interface_motors
        PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}/include
)

add_properties(interface_motors)
target_link_libraries(interface_motors
        PUBLIC
        interface_hal
)
