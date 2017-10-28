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

add_executable(gui-builder ${GUI_BUILDER_SOURCES})
target_link_libraries(gui-builder ${PROJECT_NAME} ${TGUI_EXT_LIBS})
target_include_directories(gui-builder PRIVATE include)

# Copy the executable to the gui-builder folder
add_custom_command(TARGET gui-builder POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:gui-builder> ${PROJECT_SOURCE_DIR}/gui-builder/
                   COMMAND ${CMAKE_COMMAND} -E copy_directory ${PROJECT_SOURCE_DIR}/themes ${PROJECT_SOURCE_DIR}/gui-builder/themes)
