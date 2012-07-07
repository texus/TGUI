
set(TGUI_SRC
    TGUI.cpp
    Objects.cpp
    Label.cpp
    Picture.cpp
    Button.cpp
    Checkbox.cpp
    RadioButton.cpp
    EditBox.cpp
    Slider.cpp
    Scrollbar.cpp
    Listbox.cpp
    LoadingBar.cpp
    ComboBox.cpp
    TextBox.cpp
    SpriteSheet.cpp
    InfoFileParser.cpp
    EventManager.cpp
    TextureManager.cpp
    Group.cpp
    Panel.cpp
    Window.cpp
)

# add the sfml and tgui include directories
include_directories( "${PROJECT_SOURCE_DIR}/include" )
include_directories( ${SFML_INCLUDE_DIR} )

# Determine library suffixes depending on static/shared configuration
if(TGUI_SHARED_LIBS)
    add_library(${PROJECT_NAME} SHARED ${TGUI_SRC})
    set_target_properties( ${PROJECT_NAME} PROPERTIES DEBUG_POSTFIX -d )

    # the library should be linked to sfml, unless you are on linux
    if (NOT LINUX)
        target_link_libraries( ${PROJECT_NAME} ${SFML_LIBRARIES} )

        # on Windows/gcc get rid of "lib" prefix for shared libraries,
        # and transform the ".dll.a" suffix into ".a" for import libraries
        if (WINDOWS AND COMPILER_GCC)
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

# Install library (use "bin" folder for DLL, "lib" for SO files)
if(WINDOWS)
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
