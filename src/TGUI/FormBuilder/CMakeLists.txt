# Make a GUI application (so without the command line)
if(SFML_OS_WINDOWS)
    set(GUI_TYPE WIN32)
elseif(SFML_OS_MACOSX)
    set(GUI_TYPE MACOSX_BUNDLE)
endif()

# Include the header files
include_directories( "${PROJECT_SOURCE_DIR}/include/TGUI/FormBuilder" )

# Compile the source files
add_executable(FormBuilder ${GUI_TYPE} "main.cpp" "FormBuilder.cpp" "Form.cpp" "MenuBar.cpp")

# Link with SFML and TGUI
target_link_libraries(FormBuilder ${PROJECT_NAME})
target_link_libraries(FormBuilder ${SFML_LIBRARIES} ${OPENGL_gl_LIBRARY})

# When linking statically, also link the sfml dependencies
if (SFML_STATIC_LIBRARIES)
    target_link_libraries(FormBuilder ${SFML_GRAPHICS_DEPENDENCIES} ${SFML_WINDOW_DEPENDENCIES} ${SFML_SYSTEM_DEPENDENCIES})
endif()

# On mac, also link with the Foundation framework
if (SFML_OS_MACOSX)
    find_library(FOUNDATION_LIB Foundation VARIANT framework)
    target_link_libraries(FormBuilder ${FOUNDATION_LIB})
endif()

if (SFML_OS_LINUX)

    # Define the paths to the data files
    if (NOT TGUI_FORM_BUILDER_USE_LOCAL_FILES)
        add_definitions( -DWIDGETS_FOLDER="${CMAKE_INSTALL_PREFIX}/${INSTALL_MISC_DIR}/widgets" )
        add_definitions( -DIMAGES_FOLDER="${CMAKE_INSTALL_PREFIX}/${INSTALL_MISC_DIR}/form-builder-data/images" )
        add_definitions( -DDATA_FOLDER="${CMAKE_INSTALL_PREFIX}/${INSTALL_MISC_DIR}/form-builder-data" )
        add_definitions( -DFONTS_FOLDER="${CMAKE_INSTALL_PREFIX}/${INSTALL_MISC_DIR}/fonts" )
    endif()

    # The Form Builder must remember the location of the libraries, even when installed
    set_property(TARGET FormBuilder PROPERTY INSTALL_RPATH_USE_LINK_PATH TRUE)

    # Rename the form builder executable
    add_custom_command( TARGET FormBuilder POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy "${PROJECT_BINARY_DIR}/src/TGUI/FormBuilder/FormBuilder" "${CMAKE_SOURCE_DIR}/form-builder" )
    add_custom_command( TARGET FormBuilder POST_BUILD COMMAND mv ARGS -f "${CMAKE_SOURCE_DIR}/form-builder/FormBuilder" "${CMAKE_SOURCE_DIR}/form-builder/tgui-form-builder" )

    # Install the Form Builder
    install(FILES "${CMAKE_SOURCE_DIR}/form-builder/tgui-form-builder"
            DESTINATION "${CMAKE_INSTALL_PREFIX}/bin"
            PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE )

    # Install the data files of the form builder
    install( FILES "${CMAKE_SOURCE_DIR}/form-builder/MenuBar"
                   "${CMAKE_SOURCE_DIR}/form-builder/MenuBar_File"
                   "${CMAKE_SOURCE_DIR}/form-builder/MenuBar_Edit"
             DESTINATION "${CMAKE_INSTALL_PREFIX}/${INSTALL_MISC_DIR}/form-builder-data/"
             PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE )

    install( DIRECTORY "${CMAKE_SOURCE_DIR}/form-builder/images"
             DESTINATION "${CMAKE_INSTALL_PREFIX}/${INSTALL_MISC_DIR}/form-builder-data" )

else()

    # Install the Form Builder
    install( TARGETS FormBuilder
             DESTINATION "${CMAKE_SOURCE_DIR}/form-builder"
             PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE )
endif()

# Check if you were linking dynamically to TGUI on window
if (SFML_OS_WINDOWS AND TGUI_SHARED_LIBS)

    # Put the tgui.dll file in the Form Builder folder
    install( FILES "${PROJECT_BINARY_DIR}/lib/tgui.dll" DESTINATION "${CMAKE_SOURCE_DIR}/form-builder" )

endif()

# Copy the resource files to the bundle on mac
if (SFML_OS_MACOSX)
    install( DIRECTORY "${CMAKE_SOURCE_DIR}/form-builder/images" DESTINATION "${PROJECT_SOURCE_DIR}/form-builder/FormBuilder.app/Contents/Resources" )
    install( DIRECTORY "${CMAKE_SOURCE_DIR}/widgets" DESTINATION "${PROJECT_SOURCE_DIR}/form-builder/FormBuilder.app/Contents/Resources" )
    install( FILES "${CMAKE_SOURCE_DIR}/fonts/DejaVuSans.ttf" DESTINATION "${PROJECT_SOURCE_DIR}/form-builder/FormBuilder.app/Contents/Resources" )
    install( FILES "${CMAKE_SOURCE_DIR}/form-builder/MenuBar" DESTINATION "${PROJECT_SOURCE_DIR}/form-builder/FormBuilder.app/Contents/Resources" )
    install( FILES "${CMAKE_SOURCE_DIR}/form-builder/MenuBar_File" DESTINATION "${PROJECT_SOURCE_DIR}/form-builder/FormBuilder.app/Contents/Resources" )
    install( FILES "${CMAKE_SOURCE_DIR}/form-builder/MenuBar_Edit" DESTINATION "${PROJECT_SOURCE_DIR}/form-builder/FormBuilder.app/Contents/Resources" )
endif()

