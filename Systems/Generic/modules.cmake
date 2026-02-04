cmake_minimum_required(VERSION 3.13)

include(${CMAKE_CURRENT_LIST_DIR}/functions.cmake)

function(add_motors module_name include_module)
    add_interface(Motors interface_motors ${module_name} PUBLIC)

    if (${include_module})
        add_module(Motors module_motors ${module_name})
    endif ()
endfunction()