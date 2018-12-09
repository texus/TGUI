# Macro to build the examples
# Usage: tgui_add_example(example-folder-name SUOURCES source.cpp)
macro(tgui_add_example target)

    # parse the arguments
    cmake_parse_arguments(THIS "" "" "SOURCES" ${ARGN})

    if(TGUI_OS_WINDOWS)
        set(GUI_APP WIN32)
    elseif(TGUI_OS_IOS)
        set(GUI_APP MACOSX_BUNDLE)
    else()
        set(GUI_APP "")
    endif()

    add_executable(${target} ${GUI_APP} ${THIS_SOURCES})
    if(DEFINED SFML_LIBRARIES)
        # SFML found via FindSFML.cmake
        target_include_directories(${target} PRIVATE ${SFML_INCLUDE_DIR})
        target_link_libraries(${target} PRIVATE tgui ${SFML_LIBRARIES} ${SFML_DEPENDENCIES})
    else()
        # SFML found via SFMLConfig.cmake
        target_link_libraries(${target} PRIVATE tgui sfml-graphics)
    endif()

    # Link to sfml-main (only when SFMLConfig.cmake is used, this is done through the tgui target if FindSFML.cmake is used)
    if((TGUI_OS_WINDOWS OR TGUI_OS_IOS) AND NOT DEFINED SFML_LIBRARIES)
        target_link_libraries(${target} PRIVATE sfml-main)
    endif()

    tgui_set_global_compile_flags(${target})
    tgui_set_stdlib(${target})

    set(target_install_dir ${TGUI_MISC_INSTALL_PREFIX}/examples/${target})

    # Set the RPATH of the executable on Linux and BSD
    if (TGUI_SHARED_LIBS AND (TGUI_OS_LINUX OR TGUI_OS_BSD))
        file(RELATIVE_PATH rel_lib_dir
             ${CMAKE_INSTALL_PREFIX}/${target_install_dir}
             ${CMAKE_INSTALL_PREFIX}/lib${LIB_SUFFIX})

        set_target_properties(${target} PROPERTIES
                              INSTALL_RPATH "$ORIGIN/${rel_lib_dir}")
    endif()

    # Add the install rule for the executable
    install(TARGETS ${target}
            RUNTIME DESTINATION ${target_install_dir} COMPONENT examples
            BUNDLE DESTINATION ${target_install_dir} COMPONENT examples)

endmacro()

# Build the examples
if(TGUI_OS_IOS)
    add_subdirectory(iOS)
else()
    add_subdirectory(many_different_widgets)
    add_subdirectory(scalable_login_screen)
endif()

# install the examples
install(DIRECTORY "${CMAKE_SOURCE_DIR}/examples/"
        DESTINATION "${TGUI_MISC_INSTALL_PREFIX}/examples/"
        COMPONENT examples)
