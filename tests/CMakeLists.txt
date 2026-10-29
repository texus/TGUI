####################################################################################################
# TGUI - Texus' Graphical User Interface
# Copyright (C) 2012-2026 Bruno Van de Velde (vdv_b@tgui.eu)
#
# This software is provided 'as-is', without any express or implied warranty.
# In no event will the authors be held liable for any damages arising from the use of this software.
#
# Permission is granted to anyone to use this software for any purpose,
# including commercial applications, and to alter it and redistribute it freely,
# subject to the following restrictions:
#
# 1. The origin of this software must not be misrepresented;
#    you must not claim that you wrote the original software.
#    If you use this software in a product, an acknowledgment
#    in the product documentation would be appreciated but is not required.
#
# 2. Altered source versions must be plainly marked as such,
#    and must not be misrepresented as being the original software.
#
# 3. This notice may not be removed or altered from any source distribution.
####################################################################################################

if(NOT TGUI_DEFAULT_BACKEND)
    message(WARNING "Warning: Skipping building tests. The TGUI_BUILD_TESTS option was TRUE but no backend was selected")
    return()
endif()

set(TEST_SOURCES
    AbsoluteOrRelativeValue.cpp
    Animation.cpp
    Clipboard.cpp
    Clipping.cpp
    Color.cpp
    CompareFiles.cpp
    Container.cpp
    Duration.cpp
    Filesystem.cpp
    Focus.cpp
    Font.cpp
    Global.cpp
    Layouts.cpp
    Loading/DataIO.cpp
    Loading/FormThemeLoad.cpp
    Loading/Deserializer.cpp
    Loading/Serializer.cpp
    Loading/Theme.cpp
    Loading/ThemeLoader.cpp
    MouseCursors.cpp
    Outline.cpp
    Rect.cpp
    Signal.cpp
    SignalManager.cpp
    Sprite.cpp
    String.cpp
    SvgImage.cpp
    Tests.cpp
    Text.cpp
    Texture.cpp
    TextureManager.cpp
    Timer.cpp
    ToolTip.cpp
    Vector2.cpp
    Widget.cpp
    Widgets/BitmapButton.cpp
    Widgets/Button.cpp
    Widgets/ChatBox.cpp
    Widgets/CheckBox.cpp
    Widgets/ChildWindow.cpp
    Widgets/ClickableWidget.cpp
    Widgets/ColorPicker.cpp
    Widgets/ComboBox.cpp
    Widgets/ContextMenu.cpp
    Widgets/EditBox.cpp
    Widgets/EditBoxSlider.cpp
    Widgets/FileDialog.cpp
    Widgets/Grid.cpp
    Widgets/Group.cpp
    Widgets/GrowHorizontalLayout.cpp
    Widgets/GrowVerticalLayout.cpp
    Widgets/HorizontalLayout.cpp
    Widgets/HorizontalWrap.cpp
    Widgets/Knob.cpp
    Widgets/Label.cpp
    Widgets/ListBox.cpp
    Widgets/ListView.cpp
    Widgets/MenuBar.cpp
    Widgets/MessageBox.cpp
    Widgets/Panel.cpp
    Widgets/PanelListBox.cpp
    Widgets/Picture.cpp
    Widgets/ProgressBar.cpp
    Widgets/RadioButton.cpp
    Widgets/RadioButtonGroup.cpp
    Widgets/RangeSlider.cpp
    Widgets/RichTextLabel.cpp
    Widgets/ScrollablePanel.cpp
    Widgets/Scrollbar.cpp
    Widgets/SeparatorLine.cpp
    Widgets/Slider.cpp
    Widgets/SpinButton.cpp
    Widgets/SpinControl.cpp
    Widgets/SplitContainer.cpp
    Widgets/TabContainer.cpp
    Widgets/Tabs.cpp
    Widgets/TextArea.cpp
    Widgets/ToggleButton.cpp
    Widgets/TreeView.cpp
    Widgets/VerticalLayout.cpp
    Widgets/VerticalTabs.cpp
)

# Add some files last that shouldn't be part of the unity build as they may include different backends
list(APPEND TEST_SOURCES Widgets/Canvas.cpp)
list(APPEND TEST_SOURCES BackendEvents.cpp)
list(APPEND TEST_SOURCES main.cpp)

add_executable(tests ${TEST_SOURCES})
target_include_directories(tests PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}")
target_link_libraries(tests PRIVATE tgui tgui-console-app-interface)

if(TGUI_HAS_RENDERER_BACKEND_VULKAN)
    # The tests for Vulkan create the Vulkan instance and thus needs to link the Vulkan loader
    find_package(Vulkan REQUIRED)
    target_link_libraries(tests PRIVATE Vulkan::Vulkan)
endif()

# Enable unity build by default
if(NOT DEFINED CMAKE_UNITY_BUILD)
    set_target_properties(tests PROPERTIES UNITY_BUILD ON)
endif()

tgui_set_global_compile_flags(tests)
tgui_set_stdlib(tests)

# Disable warnings about deprecated code, so that we can keep our tests for deprecated functionality
if(TGUI_COMPILER_MSVC)
    target_compile_options(tests PRIVATE /wd4996)
else()
    target_compile_options(tests PRIVATE -Wno-deprecated-declarations)
endif()

if(TGUI_ENABLE_DRAW_TESTS)
    target_compile_definitions(tests PRIVATE TGUI_ENABLE_DRAW_TESTS)
endif()

if(TGUI_USE_SYSTEM_STB)
    target_include_directories(tests SYSTEM PRIVATE "${TGUI_SYSTEM_STB_IMAGE_WRITE_INCLUDE_DIR}")
endif()

# Use a precompiled header to speed up compilation.
# We must exclude the main file since it contains the implementation of Catch.
target_precompile_headers(tests PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/Tests.hpp")
set_source_files_properties(main.cpp PROPERTIES SKIP_PRECOMPILE_HEADERS ON)

# Mark some files that shouldn't be part of the unity build as they may include different backends
set_source_files_properties(main.cpp PROPERTIES SKIP_UNITY_BUILD_INCLUSION ON)
set_source_files_properties(BackendEvents.cpp PROPERTIES SKIP_UNITY_BUILD_INCLUSION ON)
set_source_files_properties(Widgets/Canvas.cpp PROPERTIES SKIP_UNITY_BUILD_INCLUSION ON)

# Copy the resources folder to the build directory to execute the tests without installing them
add_custom_command(TARGET tests
                   POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy_directory "${PROJECT_SOURCE_DIR}/tests/expected" "$<TARGET_FILE_DIR:tests>/expected"
                   COMMAND ${CMAKE_COMMAND} -E copy_directory "${PROJECT_SOURCE_DIR}/tests/resources" "$<TARGET_FILE_DIR:tests>/resources"
                   COMMAND ${CMAKE_COMMAND} -E copy_directory "${PROJECT_SOURCE_DIR}/themes" "$<TARGET_FILE_DIR:tests>/resources"
                   VERBATIM)

set(target_install_dir "${TGUI_MISC_INSTALL_PREFIX}/tests")

copy_dlls_to_exe("$<TARGET_FILE_DIR:tests>" "${target_install_dir}" tests)

# Set the RPATH of the executable on Linux (and BSD)
if(TGUI_SHARED_LIBS AND TGUI_OS_LINUX)
    cmake_path(SET lib_dir NORMALIZE "${CMAKE_INSTALL_PREFIX}")
    cmake_path(APPEND lib_dir "${CMAKE_INSTALL_LIBDIR}")

    cmake_path(SET exe_dir NORMALIZE "${CMAKE_INSTALL_PREFIX}")
    cmake_path(APPEND exe_dir "${target_install_dir}")

    set(rel_lib_dir "${lib_dir}")
    cmake_path(RELATIVE_PATH rel_lib_dir BASE_DIRECTORY "${exe_dir}")

    set_target_properties(tests PROPERTIES
                          INSTALL_RPATH "$ORIGIN/${rel_lib_dir}"  # RPATH will contain relative path to where we installed our own library
                          INSTALL_RPATH_USE_LINK_PATH TRUE)       # RPATH will contain paths to our dependencies
endif()

if(TGUI_INSTALL)
    # Add the install rule for the executable
    install(TARGETS tests
            RUNTIME DESTINATION ${target_install_dir} COMPONENT tests
            BUNDLE DESTINATION ${target_install_dir} COMPONENT tests)

    # Install the resources next to the test executable
    install(DIRECTORY "${PROJECT_SOURCE_DIR}/tests/resources"
            DESTINATION "${target_install_dir}"
            COMPONENT tests)
    install(DIRECTORY "${PROJECT_SOURCE_DIR}/tests/expected"
            DESTINATION "${target_install_dir}"
            COMPONENT tests)
    install(DIRECTORY "${PROJECT_SOURCE_DIR}/themes/"
            DESTINATION "${target_install_dir}/resources"
            COMPONENT tests)
endif()
