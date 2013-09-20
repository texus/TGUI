# Make a GUI application (so without the command line)
if(WINDOWS)
    set(GUI_TYPE WIN32)
elseif(MACOSX)
    set(GUI_TYPE MACOSX_BUNDLE)
endif()

# Include the header files
include_directories( "${PROJECT_SOURCE_DIR}/include/TGUI/FormBuilder" )

# Compile the source files
add_executable(FormBuilder ${GUI_TYPE} "main.cpp" "FormBuilder.cpp" "Form.cpp" "MenuBar.cpp")

# Link with SFML and TGUI
target_link_libraries(FormBuilder ${PROJECT_NAME})
target_link_libraries(FormBuilder ${SFML_LIBRARIES} ${OPENGL_gl_LIBRARY})

# On mac, also link with the Foundation framework
if (MACOSX)
    find_library(FOUNDATION_LIB Foundation VARIANT framework)
    target_link_libraries(FormBuilder ${FOUNDATION_LIB})
endif()

# Install the form builder on linux
if (LINUX)

    install( DIRECTORY ${CMAKE_SOURCE_DIR}/form-builder/images
             DESTINATION "/usr/local/share/tgui-${MAJOR_VERSION}.${MINOR_VERSION}/form-builder-data" )

    install( FILES "${CMAKE_SOURCE_DIR}/form-builder/MenuBar"
                   "${CMAKE_SOURCE_DIR}/form-builder/MenuBar_Edit"
                   "${CMAKE_SOURCE_DIR}/form-builder/MenuBar_File"
             DESTINATION "/usr/local/share/tgui-${MAJOR_VERSION}.${MINOR_VERSION}/form-builder-data" )

    ADD_CUSTOM_COMMAND(
		TARGET FormBuilder
		POST_BUILD
		COMMAND ${CMAKE_COMMAND}
		ARGS -E copy ${PROJECT_BINARY_DIR}/src/TGUI/FormBuilder/FormBuilder ${CMAKE_SOURCE_DIR}/form-builder)

    add_custom_command( TARGET FormBuilder POST_BUILD COMMAND mv ARGS -f "${CMAKE_SOURCE_DIR}/form-builder/FormBuilder" "/usr/bin/tgui-form-builder" )
    add_custom_command( TARGET FormBuilder POST_BUILD COMMAND ln ARGS -sf "/usr/bin/tgui-form-builder" "${CMAKE_SOURCE_DIR}/form-builder/tgui-form-builder" )

endif()

# Check if you were linking dynamically to TGUI on window
if (WINDOWS AND TGUI_SHARED_LIBS)

    # Put the tgui.dll file in the Form Builder folder
    install( FILES "${PROJECT_BINARY_DIR}/lib/tgui.dll" DESTINATION "${CMAKE_SOURCE_DIR}/form-builder" )

endif()

# Copy the resource files to the bundle on mac
if (MACOSX)
    install( DIRECTORY "${PROJECT_SOURCE_DIR}/form-builder/images" DESTINATION "${PROJECT_SOURCE_DIR}/form-builder/FormBuilder.app/Contents/Resources" )
    install( FILES "${PROJECT_SOURCE_DIR}/form-builder/DejaVuSans.ttf" DESTINATION "${PROJECT_SOURCE_DIR}/form-builder/FormBuilder.app/Contents/Resources" )
    install( FILES "${PROJECT_SOURCE_DIR}/form-builder/FormObjectsWindow.txt" DESTINATION "${PROJECT_SOURCE_DIR}/form-builder/FormBuilder.app/Contents/Resources" )
endif()

