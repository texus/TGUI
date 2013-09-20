cmake_minimum_required(VERSION 2.8)

# define a macro that helps defining an option
macro(tgui_set_option var default type docstring)
    if(NOT DEFINED ${var})
        set(${var} ${default})
    endif()
    set(${var} ${${var}} CACHE ${type} ${docstring} FORCE)
endmacro()

# Specify default build type if none provided
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build (Debug or Release)" FORCE)
endif()

# Specify default cmake module path if none provided
if(NOT CMAKE_MODULE_PATH)
    set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/Modules CACHE STRING "The path to the cmake modules. This path must contain the FindSFML.cmake file." FORCE)
endif()

project(tgui)

# include the configuration file
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/Config.cmake)

# project version
SET( MAJOR_VERSION 0 )
SET( MINOR_VERSION 6 )

# project options
tgui_set_option( TGUI_BUILD_DOC FALSE BOOL "TRUE to generate the API documentation, FALSE to ignore it")
tgui_set_option( TGUI_SHARED_LIBS TRUE BOOL "Build shared libraries (Set to OFF to build static libraries)" )

if(WINDOWS)
    tgui_set_option( TGUI_BUILD_FORM_BUILDER FALSE BOOL "Build the TGUI Form Builder" )
else()
    tgui_set_option( TGUI_BUILD_FORM_BUILDER TRUE BOOL "Build the TGUI Form Builder" )
endif()

# Set some useful compile flags.
if( COMPILER_GCC )
    if( NOT CMAKE_CXX_FLAGS )
        set( CMAKE_CXX_FLAGS "-Wall -Wextra -Wshadow -Wno-long-long -pedantic -std=c++0x" CACHE STRING "C++ compiler flags" FORCE )
        set( CMAKE_C_FLAGS "-Wall -Wextra -Wshadow -Wno-long-long -pedantic" CACHE STRING "C compiler flags" FORCE )
    endif()

    if( NOT CMAKE_CXX_FLAGS_DEBUG )
        set( CMAKE_CXX_FLAGS_DEBUG "-g -O0 -std=c++0x" CACHE STRING "C++ compiler flags (debug)" FORCE )
        set( CMAKE_C_FLAGS_DEBUG "-g -O0" CACHE STRING "C++ compiler flags (debug)" FORCE )
    endif()
endif()

# Define an option for choosing between static and dynamic C runtime (VC++ only)
if(WINDOWS AND COMPILER_MSVC)
    tgui_set_option(TGUI_USE_STATIC_STD_LIBS FALSE BOOL "TRUE to statically link to the standard libraries, FALSE to use them as DLLs. This option has to match with the one from sfml.")

    # Apply it globally by modifying the compiler flags
    if(TGUI_USE_STATIC_STD_LIBS)
        foreach(flag
                CMAKE_CXX_FLAGS CMAKE_CXX_FLAGS_DEBUG CMAKE_CXX_FLAGS_RELEASE
                CMAKE_CXX_FLAGS_MINSIZEREL CMAKE_CXX_FLAGS_RELWITHDEBINFO)
            if(${flag} MATCHES "/MD")
                string(REGEX REPLACE "/MD" "/MT" ${flag} "${${flag}}")
            endif()
        endforeach()
    endif()
endif()

# Link to the static sfml libraries when building tgui statically
if(NOT TGUI_SHARED_LIBS)
    set(SFML_STATIC_LIBRARIES TRUE)
endif()

# Find sfml (also look for the main component when using Visual Studio)
if (WINDOWS AND COMPILER_MSVC)
    find_package( SFML 2 COMPONENTS main graphics window system )
else()
    find_package( SFML 2 COMPONENTS graphics window system )
endif()

# FindSFML couldn't find SFML.
if( NOT SFML_FOUND )
    set( SFML_ROOT "" CACHE PATH "SFML root directory" )
    message( FATAL_ERROR "CMake couldn't find SFML. Set the SFML_ROOT entry to SFML's root directory (containing \"include\" and \"lib\" directories)." )
endif()

# Set the path for the libraries
set( LIBRARY_OUTPUT_PATH "${PROJECT_BINARY_DIR}/lib" )

# Jump to the CMakeLists.txt file in the source folder
add_subdirectory(src/TGUI)

# Build the documentation when requested
if (TGUI_BUILD_DOC)
    add_subdirectory(doc)
endif()

# Build examples
# TODO

# install include files
install(DIRECTORY include DESTINATION .)

# Create uninstall
# TODO

