if(NOT TGUI_HAS_BACKEND_SFML AND NOT TGUI_HAS_BACKEND_SDL)
    message(FATAL_ERROR "Gui Builder can't be build without a backend. Uncheck TGUI_BUILD_GUI_BUILDER or change the backend.")
endif()

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
target_link_libraries(gui-builder PRIVATE tgui tgui-default-backend-interface)

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
