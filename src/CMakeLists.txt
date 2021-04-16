set(TGUI_SRC
    Animation.cpp
    Backend.cpp
    BackendRenderTarget.cpp
    BackendTexture.cpp
    Color.cpp
    Components.cpp
    Container.cpp
    Cursor.cpp
    CustomWidgetForBindings.cpp
    DefaultBackendWindow.cpp
    FileDialogIconLoader.cpp
    Filesystem.cpp
    Font.cpp
    Global.cpp
    GuiBase.cpp
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
    Widget.cpp
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
    Widgets/Canvas.cpp
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
    Widgets/Picture.cpp
    Widgets/ProgressBar.cpp
    Widgets/RadioButton.cpp
    Widgets/RadioButtonGroup.cpp
    Widgets/RangeSlider.cpp
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

if(TGUI_OPTIMIZE_SINGLE_BUILD)
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

# Determine library suffixes depending on static/shared configuration
if(TGUI_SHARED_LIBS)
    add_library(tgui SHARED ${TGUI_SRC})
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
    add_library(tgui STATIC ${TGUI_SRC})
    set_target_properties(tgui PROPERTIES DEBUG_POSTFIX -s-d)
    set_target_properties(tgui PROPERTIES RELEASE_POSTFIX -s)
    set_target_properties(tgui PROPERTIES MINSIZEREL_POSTFIX -s)
    set_target_properties(tgui PROPERTIES RELWITHDEBINFO_POSTFIX -s)
endif()

# Add the backend files to the library.
# Without the ability to call target_link_libraries in a subdirectory (which was only added in CMake 3.13 in policy CMP0079),
# the code would be more complicated when using add_subdirectory. So we just avoid creating a new scope by using include.
include(Backends/CMakeLists.txt)

# Define either TGUI_STATIC or TGUI_DYNAMIC
if(TGUI_SHARED_LIBS)
    target_compile_definitions(tgui PUBLIC TGUI_DYNAMIC)
else()
    target_compile_definitions(tgui PUBLIC TGUI_STATIC)
endif()

tgui_set_global_compile_flags(tgui)
tgui_set_stdlib(tgui)

if(TGUI_OPTIMIZE_SINGLE_BUILD AND TGUI_COMPILER_MSVC)
    target_compile_options(tgui PRIVATE /bigobj)
endif()

# Add <project>/include as public include directory
target_include_directories(tgui PUBLIC $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>)
if (TGUI_BUILD_FRAMEWORK)
    target_include_directories(tgui INTERFACE $<INSTALL_INTERFACE:TGUI.framework>)
else()
    target_include_directories(tgui INTERFACE $<INSTALL_INTERFACE:include>)
endif()

if(TGUI_OS_LINUX OR TGUI_OS_BSD)
    # On Linux and BSD we also need to link to X11
    find_package(X11 REQUIRED)
    add_library(TGUI_X11 INTERFACE)
    target_include_directories(TGUI_X11 INTERFACE "$<BUILD_INTERFACE:${X11_INCLUDE_DIR}>")
    target_link_libraries(TGUI_X11 INTERFACE "$<BUILD_INTERFACE:${X11_X11_LIB}>")
    install(TARGETS TGUI_X11 EXPORT TGUIConfigExport)
    target_link_libraries(tgui PRIVATE TGUI_X11)

    # For the FileDialog we need to link to pthreads and dl on Linux and BSD (to load system icons in the background)
    set(THREADS_PREFER_PTHREAD_FLAG ON)
    find_package(Threads REQUIRED)
    target_link_libraries(tgui PRIVATE Threads::Threads)
    target_link_libraries(tgui PRIVATE ${CMAKE_DL_LIBS})
endif()

# define TGUI_USE_CPP17 if requested
if(TGUI_USE_CPP17)
    target_compile_definitions(tgui PUBLIC TGUI_USE_CPP17)
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
    target_compile_options(tgui PRIVATE -fvisibility=hidden)
endif()

# For Visual Studio on Windows, export debug symbols (PDB files) to lib directory
if(TGUI_GENERATE_PDB)
    # PDB files are only generated in Debug and RelWithDebInfo configurations, find out which one
    if(${CMAKE_BUILD_TYPE} STREQUAL "Debug")
        set(TGUI_PDB_POSTFIX "-d")
    else()
        set(TGUI_PDB_POSTFIX "")
    endif()

    if(TGUI_SHARED_LIBS)
        # DLLs export debug symbols in the linker PDB (the compiler PDB is an intermediate file)
        set_target_properties(tgui PROPERTIES
                              PDB_NAME "tgui${TGUI_PDB_POSTFIX}"
                              PDB_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/lib")
    else()
        # Static libraries have no linker PDBs, thus the compiler PDBs are relevant
        set_target_properties(tgui PROPERTIES
                              COMPILE_PDB_NAME "tgui-s${TGUI_PDB_POSTFIX}"
                              COMPILE_PDB_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/lib")
    endif()
endif()

# Build frameworks or dylibs
if(TGUI_OS_MACOS AND TGUI_SHARED_LIBS)
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
            if(CMAKE_VERSION VERSION_LESS 3.9)
                set_target_properties(tgui PROPERTIES BUILD_WITH_INSTALL_RPATH TRUE)
            else()
                set_target_properties(tgui PROPERTIES BUILD_WITH_INSTALL_NAME_DIR TRUE)
            endif()
        endif()
    endif()
endif()

# Build tgui-activity on android
if(TGUI_OS_ANDROID)
    add_subdirectory(Android)
endif()

# Install library
install(TARGETS tgui EXPORT TGUIConfigExport
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} COMPONENT bin
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT bin
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT devel
        FRAMEWORK DESTINATION "." COMPONENT bin
)

# Generate the TGUIConfig.cmake file
# This has to happen here instead of in the root CMakeLists.txt because otherwise
# it might try to write to the macOS framework before the framework is installed.
# This is because the order of install rules is undefined between directories.
tgui_export_target(TGUIConfigExport)
