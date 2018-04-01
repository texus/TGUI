# Macro to build the examples
# Usage: tgui_add_example(example-folder-name SUOURCES source.cpp)
macro(tgui_add_example target)

    # parse the arguments
    cmake_parse_arguments(THIS "" "" "SOURCES" ${ARGN})

    # Make a GUI application on windows (without having the command line window)
    if(TGUI_OS_WINDOWS)
        set(GUI_TYPE WIN32)
    endif()

    add_executable(${target} ${GUI_TYPE} ${THIS_SOURCES})
    target_link_libraries(${target} PRIVATE tgui)

    # Link to sfml-main (only when SFMLConfig.cmake is used, this is done through the tgui target if FindSFML.cmake is used)
    if(TGUI_OS_WINDOWS AND NOT DEFINED SFML_LIBRARIES)
        target_link_libraries(${target} PRIVATE sfml-main)
    endif()

    tgui_set_global_compile_flags(${target})
    tgui_set_stdlib(${target})

    # Add the install rule for the executable
    install(TARGETS ${target}
            RUNTIME DESTINATION ${TGUI_MISC_INSTALL_PREFIX}/examples/${target} COMPONENT examples
            BUNDLE DESTINATION ${TGUI_MISC_INSTALL_PREFIX}/examples/${target} COMPONENT examples)

endmacro()

# Build the examples
add_subdirectory(many_different_widgets)
add_subdirectory(scalable_login_screen)

# install the examples
install(DIRECTORY "${CMAKE_SOURCE_DIR}/examples/"
        DESTINATION "${TGUI_MISC_INSTALL_PREFIX}/examples/"
        COMPONENT examples)
