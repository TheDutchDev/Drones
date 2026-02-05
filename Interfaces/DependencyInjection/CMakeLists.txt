cmake_minimum_required(VERSION 3.25)

if (TARGET interface_di)
    message("interface_di already exists, skipping...")
    return()
endif()

set(CMAKE_CXX_STANDARD 23)

add_library(interface_di STATIC ${CMAKE_CURRENT_LIST_DIR}/include)
set_register_args(interface_di)

target_include_directories(interface_di PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}/include
)
