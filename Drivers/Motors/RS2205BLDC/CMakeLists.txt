cmake_minimum_required(VERSION 3.25)

if (TARGET rs2205_bldc)
    message("rs2205_bldc already exists, skipping...")
    return()
endif ()

if (NOT TARGET interface_motors)
    set(INTERFACE_MOTORS_DIR ${CMAKE_CURRENT_LIST_DIR}/../../../Interfaces/Motors)
    add_subdirectory(${INTERFACE_MOTORS_DIR} ${CMAKE_BINARY_DIR}/build_interface_motors)
endif ()

add_library(rs2205_bldc STATIC)

target_sources(rs2205_bldc
        PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}/src/RS2205BLDCMotor.cpp
)

target_include_directories(rs2205_bldc
        PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}/include
)

target_link_libraries(rs2205_bldc
        PUBLIC
        interface_motors
)
