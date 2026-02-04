cmake_minimum_required(VERSION 3.13)

include(${CMAKE_CURRENT_LIST_DIR}/functions.cmake)

function(add_motors module_name include_module)
    add_interface(Motors interface_motors ${module_name} PUBLIC)

    if (${include_module})
        add_module(Motors module_motors ${module_name})
    endif ()
endfunction()

function(add_events target_name)
    add_interface(Events interface_events ${target_name} PUBLIC)
endfunction()

function(add_properties target_name)
    add_interface(Properties interface_properties ${target_name} PUBLIC)
endfunction()

function(add_rs2205_bldc target_name)
    add_driver(Motors/RS2205BLDC rs2205_bldc ${target_name})
endfunction()
