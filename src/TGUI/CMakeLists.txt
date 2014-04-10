
set(TGUI_SRC
    AnimatedPicture.cpp
    Button.cpp
    Callback.cpp
    Canvas.cpp
    ChatBox.cpp
    Checkbox.cpp
    ChildWindow.cpp
    ClickableWidget.cpp
    Clipboard.cpp
    ConfigFile.cpp
    Container.cpp
    ComboBox.cpp
    EditBox.cpp
    Global.cpp
    Grid.cpp
    Gui.cpp
    Knob.cpp
    Label.cpp
    LoadingBar.cpp
    ListBox.cpp
    MenuBar.cpp
    MessageBox.cpp
    Panel.cpp
    Picture.cpp
    RadioButton.cpp
    Scrollbar.cpp
    Slider.cpp
    Slider2d.cpp
    SpinButton.cpp
    SpriteSheet.cpp
    Tab.cpp
    TextBox.cpp
    Texture.cpp
    TextureManager.cpp
    Transformable.cpp
    Widget.cpp
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

# Install library (use "bin" folder for DLL, "lib" for SO files)
if(SFML_OS_WINDOWS)
    install( TARGETS ${PROJECT_NAME}
             RUNTIME DESTINATION bin
             LIBRARY DESTINATION lib
             ARCHIVE DESTINATION lib 
             PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE )
else()
    install( TARGETS ${PROJECT_NAME}
             DESTINATION lib
             PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE )
endif()

