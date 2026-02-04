cmake_minimum_required(VERSION 3.25)

if (TARGET interface_hal)
    message("interface_hal already exists, skipping...")
    return()
endif ()

set(CMAKE_CXX_STANDARD 23)

add_library(interface_hal STATIC ${CMAKE_CURRENT_LIST_DIR}/include)
set_register_args(interface_hal)

target_include_directories(interface_hal PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}/include
)

target_link_libraries(interface_hal INTERFACE
        freertos_kernel
        freertos_config
)
