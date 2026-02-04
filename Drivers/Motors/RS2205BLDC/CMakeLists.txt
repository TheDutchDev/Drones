cmake_minimum_required(VERSION 3.25)

if (TARGET rs2205_bldc)
    message("rs2205_bldc already exists, skipping...")
    return()
endif ()

add_library(rs2205_bldc STATIC)

include(${CMAKE_CURRENT_LIST_DIR}/../../../Systems/Generic/modules.cmake)

target_sources(rs2205_bldc
        PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}/src/RS2205BLDCMotor.cpp
)

target_include_directories(rs2205_bldc
        PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}/include
)

add_motors(rs2205_bldc FALSE)
target_link_libraries(rs2205_bldc
        PUBLIC
        interface_hal
)
