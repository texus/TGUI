####################################################################################################
# TGUI - Texus' Graphical User Interface
# Copyright (C) 2012-2023 Bruno Van de Velde (vdv_b@tgui.eu)
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

set(TGUI_SRC
    Animation.cpp
    Base64.cpp
    Color.cpp
    Components.cpp
    Container.cpp
    Cursor.cpp
    CustomWidgetForBindings.cpp
    FileDialogIconLoader.cpp
    Filesystem.cpp
    Font.cpp
    Global.cpp
    Layout.cpp
    ObjectConverter.cpp
    Sprite.cpp
    Signal.cpp
    String.cpp
    SignalManager.cpp
    SubwidgetContainer.cpp
    SvgImage.cpp
    TextStyle.cpp
    Text.cpp
    Texture.cpp
    TextureManager.cpp
    Timer.cpp
    ToolTip.cpp
    Transform.cpp
    TwoFingerScrollDetect.cpp
    Widget.cpp
    WindowsIMM.cpp
    Backend/Font/BackendFont.cpp
    Backend/Renderer/BackendRenderTarget.cpp
    Backend/Renderer/BackendText.cpp
    Backend/Renderer/BackendTexture.cpp
    Backend/Window/Backend.cpp
    Backend/Window/BackendGui.cpp
    Loading/DataIO.cpp
    Loading/Deserializer.cpp
    Loading/ImageLoader.cpp
    Loading/Serializer.cpp
    Loading/Theme.cpp
    Loading/ThemeLoader.cpp
    Loading/WidgetFactory.cpp
    Renderers/BoxLayoutRenderer.cpp
    Renderers/ButtonRenderer.cpp
    Renderers/ChatBoxRenderer.cpp
    Renderers/ChildWindowRenderer.cpp
    Renderers/ColorPickerRenderer.cpp
    Renderers/ComboBoxRenderer.cpp
    Renderers/EditBoxRenderer.cpp
    Renderers/FileDialogRenderer.cpp
    Renderers/GroupRenderer.cpp
    Renderers/KnobRenderer.cpp
    Renderers/LabelRenderer.cpp
    Renderers/ListBoxRenderer.cpp
    Renderers/ListViewRenderer.cpp
    Renderers/MenuBarRenderer.cpp
    Renderers/MessageBoxRenderer.cpp
    Renderers/PanelRenderer.cpp
    Renderers/PanelListBoxRenderer.cpp
    Renderers/PictureRenderer.cpp
    Renderers/ProgressBarRenderer.cpp
    Renderers/RadioButtonRenderer.cpp
    Renderers/RangeSliderRenderer.cpp
    Renderers/ScrollablePanelRenderer.cpp
    Renderers/ScrollbarRenderer.cpp
    Renderers/SeparatorLineRenderer.cpp
    Renderers/SliderRenderer.cpp
    Renderers/SpinButtonRenderer.cpp
    Renderers/TabsRenderer.cpp
    Renderers/TextAreaRenderer.cpp
    Renderers/TreeViewRenderer.cpp
    Renderers/WidgetRenderer.cpp
    Widgets/BitmapButton.cpp
    Widgets/BoxLayout.cpp
    Widgets/BoxLayoutRatios.cpp
    Widgets/Button.cpp
    Widgets/ButtonBase.cpp
    Widgets/ChatBox.cpp
    Widgets/CheckBox.cpp
    Widgets/ChildWindow.cpp
    Widgets/ClickableWidget.cpp
    Widgets/ColorPicker.cpp
    Widgets/ComboBox.cpp
    Widgets/EditBox.cpp
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
    Widgets/Tabs.cpp
    Widgets/TabContainer.cpp
    Widgets/TextArea.cpp
    Widgets/ToggleButton.cpp
    Widgets/TreeView.cpp
    Widgets/VerticalLayout.cpp
)

if(TGUI_OS_WINDOWS)
    list(APPEND TGUI_SRC FileDialogIconLoaderWindows.cpp)
elseif(TGUI_OS_LINUX)
    list(APPEND TGUI_SRC FileDialogIconLoaderLinux.cpp)
endif()

# Bundle multiple files together when performing a unity build (useful for CI to speed up compilation).
# Note that this creates new source files which causes CMake to no longer detect changed to the original source files,
# so this should only be enabled in places where the TGUI is build only once.
# For CMake 3.18 and newer, TGUI supports the CMAKE_UNITY_BUILD option and this code should no longer be used.
if(TGUI_OPTIMIZE_SINGLE_BUILD)
    if (${CMAKE_VERSION} VERSION_GREATER_EQUAL "3.18")
        # The only reason this code still exists is so that the CI can continue to do unity builds with an
        # older CMake version. Once the CMake version is updated, we should fail here to prevent keeping the old code.
        message(FATAL_ERROR "TGUI_OPTIMIZE_SINGLE_BUILD has been replaced with CMAKE_UNITY_BUILD")
    endif()
    list(LENGTH TGUI_SRC fileCount)
    if(TGUI_OPTIMIZE_SINGLE_BUILD_THREADS)
        set(threads ${TGUI_OPTIMIZE_SINGLE_BUILD_THREADS})
    else()
        include(ProcessorCount)
        ProcessorCount(threads)
    endif()

    if(${threads} LESS ${fileCount})
        math(EXPR fileCountMinusOne "${fileCount} - 1")
        math(EXPR threadsMinusOne "${threads} - 1")

        set(NEW_TGUI_SRC "")
        foreach(i RANGE 0 ${threadsMinusOne})
            set(OutputFile "${CMAKE_CURRENT_SOURCE_DIR}/CombinedSources-${i}.cpp")
            file(WRITE "${OutputFile}" "")
            foreach(j RANGE ${i} ${fileCountMinusOne} ${threads})
                list(GET TGUI_SRC ${j} inputFile)
                file(READ ${inputFile} CONTENTS)
                file(APPEND "${OutputFile}" "${CONTENTS}")
                list(APPEND NEW_TGUI_SRC "${OutputFile}")
            endforeach()
        endforeach()

        set(TGUI_SRC ${NEW_TGUI_SRC})
    endif()
endif()

if (TGUI_BUILD_AS_CXX_MODULE)
    file(READ "TGUI-Main-Module.cppm.in" file_contents)
    foreach(source_file ${TGUI_SRC})
        string(APPEND file_contents "#include \"${CMAKE_CURRENT_SOURCE_DIR}/${source_file}\"\n")
    endforeach()
    file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/module_tgui.cppm" "${file_contents}")
    set(TGUI_SRC "${CMAKE_CURRENT_BINARY_DIR}/module_tgui.cppm")
endif()

if (TGUI_BUILD_AS_CXX_MODULE)
    tgui_create_module_from_sources(DefaultBackendWindow.cpp "tgui.default_backend_window")
    list(APPEND TGUI_SRC "${module_source}")
else()
    list(APPEND TGUI_SRC DefaultBackendWindow.cpp)
endif()

# Determine library suffixes depending on static/shared configuration
if(TGUI_SHARED_LIBS)
    add_library(tgui SHARED)
    set_target_properties(tgui PROPERTIES DEFINE_SYMBOL TGUI_EXPORTS)
    set_target_properties(tgui PROPERTIES DEBUG_POSTFIX -d)

    # Set the version and soversion of the target (for compatible systems -- mostly Linuxes)
    # Except for Android which strips soversion suffixes
    if(NOT TGUI_OS_ANDROID)
        set_target_properties(tgui PROPERTIES SOVERSION ${TGUI_VERSION_MAJOR}.${TGUI_VERSION_MINOR}.${TGUI_VERSION_PATCH})
        set_target_properties(tgui PROPERTIES VERSION ${TGUI_VERSION_MAJOR}.${TGUI_VERSION_MINOR}.${TGUI_VERSION_PATCH})
    endif()

    # on Windows/gcc get rid of "lib" prefix for shared libraries,
    # and transform the ".dll.a" suffix into ".a" for import libraries
    if(TGUI_OS_WINDOWS AND TGUI_COMPILER_GCC)
        set_target_properties(tgui PROPERTIES PREFIX "")
        set_target_properties(tgui PROPERTIES IMPORT_SUFFIX ".a")
    endif()

else()
    add_library(tgui STATIC)
    set_target_properties(tgui PROPERTIES DEBUG_POSTFIX -s-d)
    set_target_properties(tgui PROPERTIES RELEASE_POSTFIX -s)
    set_target_properties(tgui PROPERTIES MINSIZEREL_POSTFIX -s)
    set_target_properties(tgui PROPERTIES RELWITHDEBINFO_POSTFIX -s)
endif()

if (TGUI_BUILD_AS_CXX_MODULE)
    target_sources(tgui
                   PRIVATE
                   FILE_SET tgui_cxx_module_files
                   TYPE CXX_MODULES
                   BASE_DIRS "${CMAKE_CURRENT_BINARY_DIR}"
                   FILES ${TGUI_SRC})
else()
    target_sources(tgui PRIVATE ${TGUI_SRC})
endif()

if (${CMAKE_VERSION} VERSION_LESS "3.18")
    # We don't support using CMAKE_UNITY_BUILD in CMake < 3.18 (because we need UNITY_BUILD_MODE)
    set_target_properties(tgui PROPERTIES UNITY_BUILD OFF)
else()
    # We manually group files together since we need to keep the different backends separate from each other.
    # Since grouping some files requires manually grouping ALL files, we have to manually create batches here.
    set_target_properties(tgui PROPERTIES UNITY_BUILD_MODE GROUP)

    if(NOT TGUI_BUILD_AS_CXX_MODULE)
        # Enable unity build by default
        if (NOT DEFINED CMAKE_UNITY_BUILD AND NOT DEFINED TGUI_OPTIMIZE_SINGLE_BUILD)
            set_target_properties(tgui PROPERTIES UNITY_BUILD ON)
        endif()
    endif()

    if(UNITY_BUILD_BATCH_SIZE)
        set(batch_size ${UNITY_BUILD_BATCH_SIZE})
    else()
        set(batch_size 8)
    endif()

    # Reduce size of batches if we have sufficient cores
    include(ProcessorCount)
    ProcessorCount(nr_threads)
    if (NOT nr_threads EQUAL 0)
        list(LENGTH TGUI_SRC file_count)
        math(EXPR max_batch_size "${file_count} / ${nr_threads}")
        if (max_batch_size EQUAL 1)
            # Disable unity build if we have more threads than files
            set_target_properties(tgui PROPERTIES UNITY_BUILD OFF)
        endif()
        if (max_batch_size LESS batch_size)
            set(batch_size ${max_batch_size})
        endif()
    endif()

    set(batch_id 1)
    set(files_in_bucket 0)
    math(EXPR file_count_minus_one "${file_count} - 1")
    foreach(i RANGE 0 ${file_count_minus_one})
        list(GET TGUI_SRC ${i} input_file)
        set_source_files_properties(${input_file} PROPERTIES UNITY_GROUP "batch-${batch_id}")
        math(EXPR files_in_bucket "${files_in_bucket} + 1")
        if (files_in_bucket GREATER_EQUAL batch_size)
            set(files_in_bucket 0)
            math(EXPR batch_id "${batch_id} + 1")
        endif()
    endforeach()
endif()

if(TGUI_OS_LINUX)
    # For the FileDialog we need to link to pthreads and dl on Linux and BSD (to load system icons in the background)
    set(THREADS_PREFER_PTHREAD_FLAG ON)
    find_package(Threads REQUIRED)
    target_link_libraries(tgui PRIVATE Threads::Threads)
    target_link_libraries(tgui PRIVATE ${CMAKE_DL_LIBS})
endif()

add_library(TGUI::TGUI ALIAS tgui)
set_target_properties(tgui PROPERTIES EXPORT_NAME TGUI)

# Add the backends to the library.
# Without the ability to call target_link_libraries in a subdirectory (which was only added in CMake 3.13 in policy CMP0079),
# the code would be more complicated when using add_subdirectory. So we just avoid creating a new scope by using include.
# TODO: We now require a newer CMake version, so we should be able to find a solution that uses add_subdirectory instead of include
include(Backend/CMakeLists.txt)

# Define either TGUI_STATIC or TGUI_DYNAMIC
if(TGUI_SHARED_LIBS)
    # The TGUI_DYNAMIC define is practically never needed. It would only be required in a project that dynamically
    # links to TGUI without an SFML backend, while linking to a static SFML library in the project itself.
    target_compile_definitions(tgui PUBLIC TGUI_DYNAMIC)
else()
    target_compile_definitions(tgui PUBLIC TGUI_STATIC)
endif()

tgui_set_global_compile_flags(tgui)
tgui_set_stdlib(tgui)

if((TGUI_OPTIMIZE_SINGLE_BUILD OR TGUI_BUILD_AS_CXX_MODULE) AND TGUI_COMPILER_MSVC)
    # We don't need this for the new UNITY_BUILD, only for the deprecated TGUI_OPTIMIZE_SINGLE_BUILD option,
    # because the new batching doesn't result in such huge object files.
    # Building TGUI as a module currently concatenates ALL source files, which is why /bigobj is required again.
    target_compile_options(tgui PRIVATE /bigobj)
endif()

# Add <project>/include as public include directory
target_include_directories(tgui PUBLIC $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>)
if (TGUI_BUILD_FRAMEWORK)
    target_include_directories(tgui INTERFACE $<INSTALL_INTERFACE:TGUI.framework>)
else()
    target_include_directories(tgui INTERFACE $<INSTALL_INTERFACE:include>)
endif()

# We need to link to an extra library on android (to use the asset manager)
if(TGUI_OS_ANDROID)
    target_link_libraries(tgui PRIVATE android)
endif()

# Enable automatic reference counting on iOS
if(TGUI_OS_IOS)
    set_target_properties(tgui PROPERTIES XCODE_ATTRIBUTE_CLANG_ENABLE_OBJC_ARC YES)
endif()

# Hide public symbols by default (exported ones are explicitly marked)
if(NOT TGUI_OS_WINDOWS AND TGUI_SHARED_LIBS)
    set_target_properties(tgui PROPERTIES
                          CXX_VISIBILITY_PRESET hidden
                          VISIBILITY_INLINES_HIDDEN YES)
endif()

# For Visual Studio on Windows, export debug symbols (PDB files) to lib directory (in Debug and RelWithDebInfo configurations)
if(TGUI_GENERATE_PDB)
    # Linker PDB files seem to be generated on the correct location and with the correct name in VS2017,
    # so we only have deal with the compiler PDBs for static libraries.
    # In VS2019 compiler PDBs are also correctly generated by default, but in VS2017 the file is located at the wrong
    # location and lacks the "-d" postfix. The behavior depends on the toolset version, not the IDE version itself.
    # When using NMake Makefiles (even with VS2019), the same behavior as the v141 toolset (VS2017) is observed.
    # MSVC_IDE is 1 when using a generator with "Visual Studio" in its name.
    if(NOT TGUI_SHARED_LIBS AND (NOT MSVC_IDE OR MSVC_VERSION LESS 1920))  # 1920 = VS2019 (v142 toolset)
        # In multi-configuration builds the CMAKE_BUILD_TYPE shouldn't be used and the postfix can be wrong.
        # A generator expression for COMPILE_PDB_NAME to solve this didn't seem to work though.
        # RelWithDebInfo is less likely to be used, so we will default to "-d" at the end.
        if(CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
            set(TGUI_PDB_POSTFIX "")
        else()
            set(TGUI_PDB_POSTFIX "-d")
        endif()

        set_target_properties(tgui PROPERTIES
                              COMPILE_PDB_NAME "tgui-s${TGUI_PDB_POSTFIX}"
                              COMPILE_PDB_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/lib")
    endif()
endif()

# Build frameworks or dylibs on Apple platforms
if((TGUI_OS_MACOS OR TGUI_OS_IOS) AND TGUI_SHARED_LIBS)
    if(TGUI_BUILD_FRAMEWORK)
        # Adapt target to build frameworks instead of dylibs
        set_target_properties(tgui PROPERTIES
                              FRAMEWORK TRUE
                              FRAMEWORK_VERSION ${TGUI_VERSION_MAJOR}.${TGUI_VERSION_MINOR}.${TGUI_VERSION_PATCH}
                              MACOSX_FRAMEWORK_IDENTIFIER org.tgui.tgui
                              MACOSX_FRAMEWORK_SHORT_VERSION_STRING ${TGUI_VERSION_MAJOR}.${TGUI_VERSION_MINOR}.${TGUI_VERSION_PATCH}
                              MACOSX_FRAMEWORK_BUNDLE_VERSION ${TGUI_VERSION_MAJOR}.${TGUI_VERSION_MINOR}.${TGUI_VERSION_PATCH})

        # Install the header files to the framework
        add_custom_command(TARGET tgui
                           POST_BUILD
                           COMMAND ${CMAKE_COMMAND} -E make_directory $<TARGET_FILE_DIR:tgui>/Headers
                           COMMAND ${CMAKE_COMMAND} -E copy_directory ${PROJECT_SOURCE_DIR}/include/TGUI $<TARGET_FILE_DIR:tgui>/Headers
                           COMMAND ${CMAKE_COMMAND} -E create_symlink Versions/${TGUI_VERSION_MAJOR}.${TGUI_VERSION_MINOR}.${TGUI_VERSION_PATCH}/Headers $<TARGET_FILE_DIR:tgui>/../../Headers)

        # The framework has to be with a capital letter (because it includes the header files which must be found in a "TGUI" directory)
        set_target_properties(tgui PROPERTIES OUTPUT_NAME TGUI)
    endif()

    # Adapt install directory to allow distributing dylibs/frameworks in user’s frameworks/application bundle but only if cmake rpath options aren't set
    if(NOT CMAKE_SKIP_RPATH AND NOT CMAKE_SKIP_INSTALL_RPATH AND NOT CMAKE_INSTALL_RPATH AND NOT CMAKE_INSTALL_RPATH_USE_LINK_PATH AND NOT CMAKE_INSTALL_NAME_DIR)
        set_target_properties(tgui PROPERTIES INSTALL_NAME_DIR "@rpath")
        if(NOT CMAKE_SKIP_BUILD_RPATH)
            set_target_properties(tgui PROPERTIES BUILD_WITH_INSTALL_NAME_DIR TRUE)
        endif()
    endif()
endif()

# Generate the Config.hpp header
configure_file(${PROJECT_SOURCE_DIR}/include/TGUI/Config.hpp.in ${PROJECT_SOURCE_DIR}/include/TGUI/Config.hpp @ONLY)

if(NOT TGUI_BUILD_AS_CXX_MODULE AND NOT TGUI_DISABLE_PRECOMPILED_HEADER)
    # Use a precompiled header to speed up compilation
    target_precompile_headers(tgui PRIVATE "${PROJECT_SOURCE_DIR}/include/TGUI/Widget.hpp")
endif()

# Install library
install(TARGETS tgui EXPORT TGUIConfigExport
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} COMPONENT bin
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT bin
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT devel
        FRAMEWORK DESTINATION "." COMPONENT bin
)

# Generate the TGUIConfig.cmake file
# This had to happen here instead of in the root CMakeLists.txt because otherwise it might try to write to the macOS framework before the framework is installed.
# TODO: CMake 3.13 fixed order of installation, so check if the issue still exists.
# Since the config files relies on variables that are only defined in this scope (via the backend include), caution is required when moving this.
if (NOT TGUI_SKIP_EXPORT)
  tgui_export_target(TGUIConfigExport)
endif()
