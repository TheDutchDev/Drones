cmake_minimum_required(VERSION 3.25)

# Options, can be toggled through cmake with -D (eg -DOPT_REBOOT_ON_FATAL=ON) OR by manually setting to ON/OFF here.
option(OPT_ENABLE_LOGGING "Enable logging (has no effect in debug builds)" OFF) # -DOPT_ENABLE_LOGGING=ON

set(OPT_DEFINITIONS_LIST)

if (CMAKE_BUILD_TYPE STREQUAL "Debug" OR OPT_ENABLE_LOGGING)
    list(APPEND OPT_DEFINITIONS_LIST ENABLE_LOGGING)
    message("Logging Enabled!")
endif ()

add_compile_definitions(${OPT_DEFINITIONS_LIST})