target_compile_definitions(firmware PRIVATE LOG_LEVEL=LOG_LEVEL_DEBUG)

target_link_libraries(firmware PRIVATE c m)

# Compiler options
target_compile_options(firmware PRIVATE
        -ffunction-sections
        -fdata-sections
        -Wall
        -fstack-usage
)

# Post-build command to create .bin file
add_custom_command(TARGET firmware POST_BUILD
        COMMAND ${CMAKE_OBJCOPY} -O binary $<TARGET_FILE:firmware> ${CMAKE_BINARY_DIR}/${FIRMWARE_FILE_NAME}.bin
        COMMAND ${CMAKE_OBJDUMP} -h -S $<TARGET_FILE:firmware> > ${CMAKE_BINARY_DIR}/${FIRMWARE_FILE_NAME}.lst
        COMMAND ${CMAKE_NM} --size-sort -td $<TARGET_FILE:firmware> > ${CMAKE_BINARY_DIR}/${FIRMWARE_FILE_NAME}.nm
)
