cmake_minimum_required(VERSION 3.25)

include(${CMAKE_CURRENT_LIST_DIR}/functions.cmake)

set(MAJOR_VERSION 0)
set(MINOR_VERSION 0)
set(PATCH_VERSION 1)
set(STAGE "alpha")

set(VERSION_DEFS
        MAJOR_VERSION=${MAJOR_VERSION}
        MINOR_VERSION=${MINOR_VERSION}
        PATCH_VERSION=${PATCH_VERSION}
        STAGE="${STAGE}"
)

# Default firmware name for any build
set(FIRMWARE_FILE_NAME firmware)

# When building release, append versions to filename
if (CMAKE_BUILD_TYPE STREQUAL "Release")
    set(FIRMWARE_FILE_NAME ${FIRMWARE_FILE_NAME}-v${MAJOR_VERSION}.${MINOR_VERSION}.${PATCH_VERSION}-${STAGE})
endif ()

# Build version is added through build pipelines (revision count + partial commit hash)
if (DEFINED BUILD_VERSION)
    set(FIRMWARE_FILE_NAME "${FIRMWARE_FILE_NAME}-${BUILD_VERSION}")
else () # default value
    list(APPEND VERSION_DEFS BUILD_VERSION="")
endif ()

# Always add version to compile definitions
add_compile_definitions(${VERSION_DEFS})

message(STATUS "Firmware file name: ${FIRMWARE_FILE_NAME}.bin")