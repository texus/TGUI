# find doxygen
if(MACOSX)
    # Add some path to search doxygen in more directories.
    set(ADDITIONAL_PATHS 
        /Developer/Applications/Doxygen.app/Contents/Resources 
        /Developer/Applications/Doxygen.app/Contents/MacOS 
        $ENV{HOME}/Applications/Doxygen.app/Contents/Resources 
        $ENV{HOME}/Applications/Doxygen.app/Contents/MacOS 
        $ENV{HOME}/Applications/Developer/Doxygen.app/Contents/Resources 
        $ENV{HOME}/Applications/Developer/Doxygen.app/Contents/MacOS)
 
    set(CMAKE_PROGRAM_PATH ${CMAKE_PROGRAM_PATH} ${ADDITIONAL_PATHS})
endif()

find_package(Doxygen REQUIRED)

# configure the source Doxyfile by copying it and replacing all @variables@
configure_file(doxyfile.in doxyfile @ONLY)

# target setup
add_custom_target(doc ALL
                  COMMAND ${CMAKE_COMMAND} -E echo_append "Building API Documentation..."
                  COMMAND Doxygen::doxygen ${CMAKE_CURRENT_BINARY_DIR}/doxyfile
                  COMMAND ${CMAKE_COMMAND} -E echo "Done."
                  WORKING_DIRECTORY ${PROJECT_SOURCE_DIR})

# setup install rules
install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/html
        DESTINATION ${CMAKE_INSTALL_DOCDIR}
        COMPONENT doc)
