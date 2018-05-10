set(GUI_BUILDER_SOURCES
    src/main.cpp
    src/Form.cpp
    src/GuiBuilder.cpp
    include/ButtonProperties.hpp
    include/EditBoxProperties.hpp
    include/Form.hpp
    include/GuiBuilder.hpp
    include/PictureProperties.hpp
    include/WidgetProperties.hpp
)

# Make a GUI application on windows (without having the command line window)
if(TGUI_OS_WINDOWS)
    set(GUI_TYPE WIN32)
endif()

add_executable(gui-builder ${GUI_TYPE} ${GUI_BUILDER_SOURCES})
target_link_libraries(gui-builder PRIVATE tgui)
target_include_directories(gui-builder PRIVATE include)

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
