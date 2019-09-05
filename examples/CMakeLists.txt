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
endmacro()

# Build the examples
if(TGUI_OS_IOS)
    add_subdirectory(iOS)
else()
    add_subdirectory(many_different_widgets)
    add_subdirectory(scalable_login_screen)
endif()
