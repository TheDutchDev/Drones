set(CMAKE_SYSTEM_NAME Generic)
cmake_minimum_required(VERSION 3.25)

set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)
set(CMAKE_CXX_STANDARD 23)

project(firmware C CXX ASM)
add_executable(firmware)

include(${CMAKE_SOURCE_DIR}/../Generic/CMakeLists.txt)

# Include directories (Needs to be separated to
include_directories(
        ${CMAKE_SOURCE_DIR}/include
)

target_include_directories(firmware
        PRIVATE
        ${CMAKE_SOURCE_DIR}/src

        PUBLIC
        ${CMAKE_SOURCE_DIR}/include
)

include(${CMAKE_SOURCE_DIR}/../../Modules/CMakeLists.txt)

file(GLOB_RECURSE C_SOURCES
        ${CMAKE_SOURCE_DIR}/src/*.cpp
)

target_sources(firmware PUBLIC ${C_SOURCES})

add_events(firmware)
add_properties(firmware)
add_motors(firmware FALSE)
add_rs2205_bldc(firmware)
add_di(firmware TRUE)
