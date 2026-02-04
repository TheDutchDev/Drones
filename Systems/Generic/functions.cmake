cmake_minimum_required(VERSION 3.13)

function(set_register_args module_name)
#    message("REGISTER ARGS: ${REGISTER_ARGS}")
    set_target_properties(${module_name} PROPERTIES LINKER_LANGUAGE CXX)

    if(DEFINED REGISTER_ARGS)
        target_link_options(${module_name} PRIVATE ${REGISTER_ARGS})
        target_compile_options(${module_name} PRIVATE ${REGISTER_ARGS})
    endif()
endfunction()

# Custom function to add modules with separate build directories
function(add_module module_directory module_name target_name)

    if(NOT(TARGET ${module_name}))
        message("Adding module: ${module_directory} ${module_name} for target ${target_name}")

        set(module_path "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/../../Modules/${module_directory}")
        set(module_build_dir "${CMAKE_BINARY_DIR}/build_module_${module_name}")

        # Ensure the build directory exists
        file(MAKE_DIRECTORY ${module_build_dir})

        # Add the subdirectory with its own build directory
        add_subdirectory(${module_path} ${module_build_dir})
    endif()

    target_link_libraries(${target_name} PRIVATE ${module_name})
endfunction()

# Custom function to add interfaces with separate build directories
function(add_interface interface_directory interface_name target_name visibility)

    if(NOT(TARGET ${interface_name}))
        message("Adding interface: ${interface_directory} ${interface_name} for target ${target_name}")

        set(interface_path "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/../../Interfaces/${interface_directory}")
        set(interface_build_dir "${CMAKE_BINARY_DIR}/build_interface_${interface_name}")

        # Ensure the build directory exists
        file(MAKE_DIRECTORY ${interface_build_dir})

        # Add the subdirectory with its own build directory
        add_subdirectory(${interface_path} ${interface_build_dir})
    endif()

    target_link_libraries(${target_name} ${visibility} ${interface_name})
endfunction()

# Custom function to add drivers with separate build directories
function(add_driver driver_directory driver_name target_name)

    if(NOT(TARGET ${driver_name}))
        message("Adding driver: ${driver_directory} ${driver_name} for target ${target_name}")

        set(driver_path "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/../../Drivers/${driver_directory}")
        set(driver_build_dir "${CMAKE_BINARY_DIR}/build_driver_${driver_name}")

        # Ensure the build directory exists
        file(MAKE_DIRECTORY ${driver_build_dir})

        # Add the subdirectory with its own build directory
        add_subdirectory(${driver_path} ${driver_build_dir})
    endif()

    target_link_libraries(${target_name} PRIVATE ${driver_name})
endfunction()