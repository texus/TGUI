####################################################################################################
# TGUI - Texus' Graphical User Interface
# Copyright (C) 2012-2024 Bruno Van de Velde (vdv_b@tgui.eu)
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
    Tests.cpp
    CompareFiles.cpp
    AbsoluteOrRelativeValue.cpp
    Animation.cpp
    Clipboard.cpp
    Clipping.cpp
    Color.cpp
    Container.cpp
    Duration.cpp
    Filesystem.cpp
    Focus.cpp
    Font.cpp
    Layouts.cpp
    MouseCursors.cpp
    Outline.cpp
    Sprite.cpp
    Signal.cpp
    SignalManager.cpp
    String.cpp
    SvgImage.cpp
    Text.cpp
    Texture.cpp
    TextureManager.cpp
    Timer.cpp
    ToolTip.cpp
    Vector2.cpp
    Widget.cpp
    Loading/DataIO.cpp
    Loading/Serializer.cpp
    Loading/Deserializer.cpp
    Loading/Theme.cpp
    Loading/ThemeLoader.cpp
    Widgets/BitmapButton.cpp
    Widgets/Button.cpp
    Widgets/ChatBox.cpp
    Widgets/CheckBox.cpp
    Widgets/ChildWindow.cpp
    Widgets/ClickableWidget.cpp
    Widgets/ColorPicker.cpp
    Widgets/ComboBox.cpp
    Widgets/EditBox.cpp
    Widgets/EditBoxSlider.cpp
    Widgets/FileDialog.cpp
    Widgets/Group.cpp
    Widgets/Grid.cpp
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
    Widgets/Tabs.cpp
    Widgets/TabContainer.cpp
    Widgets/TextArea.cpp
    Widgets/ToggleButton.cpp
    Widgets/TreeView.cpp
    Widgets/VerticalLayout.cpp
)

# Bundle multiple files together when performing a unity build (useful for CI to speed up compilation).
# Note that this creates new source files which causes CMake to no longer detect changed to the original source files,
# so this should only be enabled in places where the TGUI is build only once.
# For CMake 3.18 and newer, TGUI supports the CMAKE_UNITY_BUILD option and this code should no longer be used.
if (TGUI_OPTIMIZE_SINGLE_BUILD)
    if (${CMAKE_VERSION} VERSION_GREATER_EQUAL "3.18")
        # The only reason this code still exists is so that the CI can continue to do unity builds with an
        # older CMake version. Once the CMake version is updated, we should fail here to prevent keeping the old code.
        message(FATAL_ERROR "TGUI_OPTIMIZE_SINGLE_BUILD has been replaced with CMAKE_UNITY_BUILD")
    endif()
    list(LENGTH TEST_SOURCES fileCount)
    if (TGUI_OPTIMIZE_SINGLE_BUILD_THREADS)
        set(threads ${TGUI_OPTIMIZE_SINGLE_BUILD_THREADS})
    else()
        include(ProcessorCount)
        ProcessorCount(threads)
    endif()

    if (${threads} LESS ${fileCount})
        math(EXPR threads "(${threads} / 2) + 1")
        math(EXPR fileCountMinusOne "${fileCount} - 1")
        math(EXPR threadsMinusOne "${threads} - 1")

        set(NEW_TEST_SOURCES "")
        foreach(i RANGE 0 ${threadsMinusOne})
            set(OutputFile "${CMAKE_CURRENT_SOURCE_DIR}/CombinedSources-Tests-${i}.cpp")
            file(WRITE "${OutputFile}" "")
            foreach(j RANGE ${i} ${fileCountMinusOne} ${threads})
                list(GET TEST_SOURCES ${j} inputFile)
                file(READ ${inputFile} CONTENTS)
                file(APPEND "${OutputFile}" "${CONTENTS}")
                list(APPEND NEW_TEST_SOURCES "${OutputFile}")
            endforeach()
        endforeach()

        set(TEST_SOURCES ${NEW_TEST_SOURCES})
    endif()
endif()

# Add some files last that shouldn't be part of the unity build as they may include different backends
list(APPEND TEST_SOURCES Widgets/Canvas.cpp)
list(APPEND TEST_SOURCES BackendEvents.cpp)
list(APPEND TEST_SOURCES main.cpp)

add_executable(tests ${TEST_SOURCES})
target_include_directories(tests PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}")
target_link_libraries(tests PRIVATE tgui tgui-console-app-interface)

if(NOT TGUI_BUILD_AS_CXX_MODULE)
    # Enable unity build by default
    if (NOT DEFINED CMAKE_UNITY_BUILD AND NOT DEFINED TGUI_OPTIMIZE_SINGLE_BUILD)
        set_target_properties(tests PROPERTIES UNITY_BUILD ON)
    endif()
endif()

tgui_set_global_compile_flags(tests)
tgui_set_stdlib(tests)

# Disable warnings about deprecated code, so that we can keep our tests for deprecated functionality
if (TGUI_COMPILER_MSVC)
    target_compile_options(tests PRIVATE /wd4996)
else()
    target_compile_options(tests PRIVATE -Wno-deprecated-declarations)
endif()

if(TGUI_OPTIMIZE_SINGLE_BUILD AND TGUI_COMPILER_MSVC)
    # We don't need this for the new UNITY_BUILD, only for the deprecated TGUI_OPTIMIZE_SINGLE_BUILD option,
    # because the new batching doesn't result in such huge object files.
    target_compile_options(tests PRIVATE /bigobj)
endif()

if(TGUI_ENABLE_DRAW_TESTS)
    target_compile_definitions(tests PRIVATE TGUI_ENABLE_DRAW_TESTS)
endif()

if(NOT TGUI_BUILD_AS_CXX_MODULE)
    # Use a precompiled header to speed up compilation.
    # We must exclude the main file since it contains the implementation of Catch.
    target_precompile_headers(tests PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/Tests.hpp")
    set_source_files_properties(main.cpp PROPERTIES SKIP_PRECOMPILE_HEADERS ON)

    # Mark some files that shouldn't be part of the unity build as they may include different backends
    set_source_files_properties(main.cpp PROPERTIES SKIP_UNITY_BUILD_INCLUSION ON)
    set_source_files_properties(BackendEvents.cpp PROPERTIES SKIP_UNITY_BUILD_INCLUSION ON)
    set_source_files_properties(Widgets/Canvas.cpp PROPERTIES SKIP_UNITY_BUILD_INCLUSION ON)
endif()

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
    if (CMAKE_VERSION VERSION_GREATER_EQUAL 3.20)
        cmake_path(SET lib_dir NORMALIZE "${CMAKE_INSTALL_PREFIX}")
        cmake_path(APPEND lib_dir "${CMAKE_INSTALL_LIBDIR}")

        cmake_path(SET exe_dir NORMALIZE "${CMAKE_INSTALL_PREFIX}")
        cmake_path(APPEND exe_dir "${target_install_dir}")

        set(rel_lib_dir "${lib_dir}")
        cmake_path(RELATIVE_PATH rel_lib_dir BASE_DIRECTORY "${exe_dir}")
    else()
        # This code for CMake < 3.20 only works if CMAKE_INSTALL_PREFIX is an absolute path
        file(RELATIVE_PATH rel_lib_dir
             "${CMAKE_INSTALL_PREFIX}/${target_install_dir}"
             "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}")
    endif()

    set_target_properties(tests PROPERTIES
                          INSTALL_RPATH "$ORIGIN/${rel_lib_dir}"  # RPATH will contain relative path to where we installed our own library
                          INSTALL_RPATH_USE_LINK_PATH TRUE)       # RPATH will contain paths to our dependencies
endif()

if (TGUI_INSTALL)
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
