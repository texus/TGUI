# Make a GUI application (so without the command line)
if(WINDOWS)
    set(GUI_TYPE WIN32)
elseif(MACOSX)
    set(GUI_TYPE MACOSX_BUNDLE)
endif() 

# Include the header files
include_directories( "${PROJECT_SOURCE_DIR}/include/TGUI/FormBuilder" )

# Compile the source files
add_executable(FormBuilder ${GUI_TYPE} "main.cpp" "Properties.cpp" "FormBuilder.cpp")

# Link with SFML and TGUI
target_link_libraries(FormBuilder ${PROJECT_NAME})
target_link_libraries(FormBuilder ${SFML_LIBRARIES} ${OPENGL_gl_LIBRARY})

# On mac, also link with the Foundation framework
if (MACOSX)
    find_library(FOUNDATION_LIB Foundation VARIANT framework)
    target_link_libraries(FormBuilder ${FOUNDATION_LIB})
endif()

# The Form Builder must remember the location of the libraries, even when installed
if (LINUX)
    set_property(TARGET FormBuilder PROPERTY INSTALL_RPATH_USE_LINK_PATH TRUE)
endif()

# Install the Form Builder
install( TARGETS FormBuilder
         DESTINATION "${CMAKE_SOURCE_DIR}/Form Builder"
         PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE )

# Check if you were linking dynamically to TGUI on window
if (WINDOWS AND TGUI_SHARED_LIBS)

    # Put the tgui.dll file in the Form Builder folder
    install( FILES "${PROJECT_BINARY_DIR}/lib/tgui.dll" DESTINATION "${CMAKE_SOURCE_DIR}/Form Builder" )

endif()

# Copy the resource files to the bundle on mac
if (MACOSX)
    install( DIRECTORY "${PROJECT_SOURCE_DIR}/Form Builder/images" DESTINATION "${PROJECT_SOURCE_DIR}/Form Builder/FormBuilder.app/Contents/Resources" )
    install( FILES "${PROJECT_SOURCE_DIR}/Form Builder/DejaVuSans.ttf" DESTINATION "${PROJECT_SOURCE_DIR}/Form Builder/FormBuilder.app/Contents/Resources" )
    install( FILES "${PROJECT_SOURCE_DIR}/Form Builder/FormObjectsWindow.txt" DESTINATION "${PROJECT_SOURCE_DIR}/Form Builder/FormBuilder.app/Contents/Resources" )
endif()

