
set(TGUI_SRC
    Global.cpp
    Canvas.cpp
    Clipboard.cpp
    Callback.cpp
    Transformable.cpp
    Widget.cpp
    Label.cpp
    Picture.cpp
    Button.cpp
    Checkbox.cpp
    RadioButton.cpp
    EditBox.cpp
    Slider.cpp
    Scrollbar.cpp
    ListBox.cpp
    LoadingBar.cpp
    ComboBox.cpp
    TextBox.cpp
    SpriteSheet.cpp
    AnimatedPicture.cpp
    SpinButton.cpp
    Slider2d.cpp
    Tab.cpp
    ConfigFile.cpp
    TextureManager.cpp
    Container.cpp
    Panel.cpp
    ChildWindow.cpp
    Grid.cpp
    Gui.cpp
    ClickableWidget.cpp
    MenuBar.cpp
    ChatBox.cpp
    MessageBox.cpp
    Knob.cpp
)

# add the sfml and tgui include directories
include_directories( "${PROJECT_SOURCE_DIR}/include" )
include_directories( ${SFML_INCLUDE_DIR} )

# OpenGL is required (due to a temporary fix)
find_package(OpenGL REQUIRED)
include_directories(${OPENGL_INCLUDE_DIR})

# Determine library suffixes depending on static/shared configuration
if(TGUI_SHARED_LIBS)
    add_library(${PROJECT_NAME} SHARED ${TGUI_SRC})
    set_target_properties( ${PROJECT_NAME} PROPERTIES DEBUG_POSTFIX -d )
    set_target_properties( ${PROJECT_NAME} PROPERTIES VERSION ${MAJOR_VERSION}.${MINOR_VERSION}.${PATCH_VERSION} )

    # the library should be linked to sfml, unless you are on linux
    if (NOT SFML_OS_LINUX)
        set(TGUI_EXT_LIBS ${OPENGL_gl_LIBRARY} ${SFML_LIBRARIES})
        target_link_libraries( ${PROJECT_NAME} ${TGUI_EXT_LIBS} )

        # on Windows/gcc get rid of "lib" prefix for shared libraries,
        # and transform the ".dll.a" suffix into ".a" for import libraries
        if (SFML_OS_WINDOWS AND SFML_COMPILER_GCC)
            set_target_properties(${PROJECT_NAME} PROPERTIES PREFIX "")
            set_target_properties(${PROJECT_NAME} PROPERTIES IMPORT_SUFFIX ".a")
        endif()
    endif()

else()
    add_definitions(-DSFML_STATIC)
    add_library(${PROJECT_NAME} STATIC ${TGUI_SRC})
    set_target_properties(${PROJECT_NAME} PROPERTIES DEBUG_POSTFIX -s-d)
    set_target_properties(${PROJECT_NAME} PROPERTIES RELEASE_POSTFIX -s)
endif()

# for gcc >= 4.0 on Windows, apply the TGUI_USE_STATIC_STD_LIBS option if it is enabled
if(SFML_OS_WINDOWS AND SFML_COMPILER_GCC AND NOT SFML_GCC_VERSION VERSION_LESS "4")
    if(TGUI_USE_STATIC_STD_LIBS AND NOT SFML_COMPILER_GCC_TDM)
        set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS "-static-libgcc -static-libstdc++")
    elseif(NOT TGUI_USE_STATIC_STD_LIBS AND SFML_COMPILER_GCC_TDM)
        set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS "-shared-libgcc -shared-libstdc++")
    endif()
endif()

# if using gcc >= 4.0 or clang >= 3.0 on a non-Windows platform, we must hide public symbols by default
# (exported ones are explicitely marked)
if(NOT SFML_OS_WINDOWS AND ((SFML_COMPILER_GCC AND NOT SFML_GCC_VERSION VERSION_LESS "4") OR (SFML_COMPILER_CLANG AND NOT SFML_CLANG_VERSION VERSION_LESS "3")))
    set_target_properties(${PROJECT_NAME} PROPERTIES COMPILE_FLAGS -fvisibility=hidden)
endif()

# Build frameworks or dylibs
if(SFML_OS_MACOSX AND TGUI_SHARED_LIBS)
    if(TGUI_BUILD_FRAMEWORK)
        # Adapt target to build frameworks instead of dylibs
        set_target_properties(${PROJECT_NAME} PROPERTIES
                              FRAMEWORK TRUE
                              FRAMEWORK_VERSION ${MAJOR_VERSION}.${MINOR_VERSION}.${PATCH_VERSION}
                              MACOSX_FRAMEWORK_IDENTIFIER org.tgui.${PROJECT_NAME}
                              MACOSX_FRAMEWORK_SHORT_VERSION_STRING ${MAJOR_VERSION}.${MINOR_VERSION}.${PATCH_VERSION}
                              MACOSX_FRAMEWORK_BUNDLE_VERSION ${MAJOR_VERSION}.${MINOR_VERSION}.${PATCH_VERSION})

        # Install the header files to the framework
        add_custom_command(TARGET ${PROJECT_NAME}
                           POST_BUILD
                           COMMAND ${CMAKE_COMMAND} -E make_directory $<TARGET_FILE_DIR:${PROJECT_NAME}>/Headers
                           COMMAND ${CMAKE_COMMAND} -E copy_directory ${PROJECT_SOURCE_DIR}/include/TGUI $<TARGET_FILE_DIR:${PROJECT_NAME}>/Headers
                           COMMAND ${CMAKE_COMMAND} -E create_symlink $<TARGET_FILE_DIR:${PROJECT_NAME}>/Headers $<TARGET_FILE_DIR:${PROJECT_NAME}>/../../Headers)

        # The framework has to be with a capital letter (because it includes the header files which must be found in a "TGUI" directory)
        set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME TGUI)
    endif()

    # Adapt install directory to allow distributing dylibs/frameworks in user’s frameworks/application bundle
    set_target_properties(${PROJECT_NAME} PROPERTIES
                          BUILD_WITH_INSTALL_RPATH 1
                          INSTALL_NAME_DIR "@rpath")
endif()

# Install library
install(TARGETS ${PROJECT_NAME}
        RUNTIME DESTINATION bin COMPONENT bin
        LIBRARY DESTINATION lib${LIB_SUFFIX} COMPONENT bin
        ARCHIVE DESTINATION lib${LIB_SUFFIX} COMPONENT devel
        FRAMEWORK DESTINATION ${CMAKE_INSTALL_FRAMEWORK_PREFIX} COMPONENT bin
)


# Build the Form Builder if requested
if (TGUI_BUILD_FORM_BUILDER)
    add_subdirectory(FormBuilder)
endif()


