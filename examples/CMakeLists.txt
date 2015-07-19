# Macro to build the examples
# Usage: tgui_add_example(example-folder-name SUOURCES source.cpp)
macro(tgui_add_example target)

    # parse the arguments
    cmake_parse_arguments(THIS "" "" "SOURCES" ${ARGN})

    # Make a GUI application on windows (so without the command line)
    if(SFML_OS_WINDOWS)
        set(GUI_TYPE WIN32)
    endif()

    add_executable(${target} ${THIS_SOURCES})
    target_link_libraries(${target} ${PROJECT_NAME} ${TGUI_EXT_LIBS})

    # for gcc >= 4.0 on Windows, apply the SFML_USE_STATIC_STD_LIBS option if it is enabled
    if(SFML_OS_WINDOWS AND SFML_COMPILER_GCC AND NOT SFML_GCC_VERSION VERSION_LESS "4")
        if(SFML_USE_STATIC_STD_LIBS AND NOT SFML_COMPILER_GCC_TDM)
            set_target_properties(${target} PROPERTIES LINK_FLAGS "-static-libgcc -static-libstdc++")
        elseif(NOT SFML_USE_STATIC_STD_LIBS AND SFML_COMPILER_GCC_TDM)
            set_target_properties(${target} PROPERTIES LINK_FLAGS "-shared-libgcc -shared-libstdc++")
        endif()
    endif()

    # Add the install rule for the executable
    install(TARGETS ${target}
            RUNTIME DESTINATION ${INSTALL_MISC_DIR}/examples/${target} COMPONENT examples
            BUNDLE DESTINATION ${INSTALL_MISC_DIR}/examples/${target} COMPONENT examples)

endmacro()

# Build the examples
add_subdirectory(full_example)
add_subdirectory(login_screen)
add_subdirectory(scalable)

# install the examples
install(DIRECTORY "${CMAKE_SOURCE_DIR}/examples/"
        DESTINATION "${INSTALL_MISC_DIR}/examples/"
        COMPONENT examples)
