set(GUI_BUILDER_SOURCES
    src/main.cpp
    src/Form.cpp
    src/GuiBuilder.cpp
)

# Make a GUI application on windows (without having the command line window)
if(TGUI_OS_WINDOWS)
    set(GUI_TYPE WIN32)
endif()

add_executable(gui-builder ${GUI_TYPE} ${GUI_BUILDER_SOURCES})
target_include_directories(gui-builder PRIVATE include)
if(DEFINED SFML_LIBRARIES)
    # SFML found via FindSFML.cmake
    target_include_directories(gui-builder PRIVATE ${SFML_INCLUDE_DIR})
    target_link_libraries(gui-builder PRIVATE tgui ${SFML_LIBRARIES} ${SFML_DEPENDENCIES})
else()
    # SFML found via SFMLConfig.cmake
    target_link_libraries(gui-builder PRIVATE tgui sfml-graphics)
endif()

# Link to sfml-main (only when SFMLConfig.cmake is used, this is done through the tgui target if FindSFML.cmake is used)
if(TGUI_OS_WINDOWS AND NOT DEFINED SFML_LIBRARIES)
    target_link_libraries(gui-builder PRIVATE sfml-main)
endif()

tgui_set_global_compile_flags(gui-builder)
tgui_set_stdlib(gui-builder)

# Copy the executable to the gui-builder folder
add_custom_command(TARGET gui-builder POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:gui-builder> ${PROJECT_SOURCE_DIR}/gui-builder/
                   COMMAND ${CMAKE_COMMAND} -E copy_directory ${PROJECT_SOURCE_DIR}/themes ${PROJECT_SOURCE_DIR}/gui-builder/themes)

set(target_install_dir "${TGUI_MISC_INSTALL_PREFIX}/gui-builder")

# Set the RPATH of the executable on Linux and BSD
if (TGUI_SHARED_LIBS AND (TGUI_OS_LINUX OR TGUI_OS_BSD))
    file(RELATIVE_PATH rel_lib_dir
         ${CMAKE_INSTALL_PREFIX}/${target_install_dir}
         ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR})

    set_target_properties(gui-builder PROPERTIES
                          INSTALL_RPATH "$ORIGIN/${rel_lib_dir}")
endif()

# Add the install rule for the executable
install(TARGETS gui-builder
        RUNTIME DESTINATION ${target_install_dir} COMPONENT gui-builder
        BUNDLE DESTINATION ${target_install_dir} COMPONENT gui-builder)

# Install the resources next to the test executable
install(DIRECTORY "${PROJECT_SOURCE_DIR}/gui-builder/resources"
        DESTINATION "${target_install_dir}"
        COMPONENT gui-builder)
install(DIRECTORY "${PROJECT_SOURCE_DIR}/themes"
        DESTINATION "${target_install_dir}"
        COMPONENT gui-builder)
