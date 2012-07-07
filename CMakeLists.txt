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
SET( ${PROJECT_NAME}_MAJOR_VERSION 0 )
SET( ${PROJECT_NAME}_MINOR_VERSION 4 )
SET( ${PROJECT_NAME}_PATCH_VERSION 0 )

# project options
tgui_set_option( TGUI_SHARED_LIBS TRUE BOOL "Build shared libraries (Set to OFF to build static libraries)" )

# Set some useful compile flags.
if( CMAKE_COMPILER_IS_GNUCXX )
	if( NOT CMAKE_CXX_FLAGS )
		set( CMAKE_CXX_FLAGS "-Wall -Wextra -Wshadow -Wno-long-long -pedantic" CACHE STRING "C++ compiler flags" FORCE )
		set( CMAKE_C_FLAGS "-Wall -Wextra -Wshadow -Wno-long-long -pedantic" CACHE STRING "C compiler flags" FORCE )
	endif()

	if( NOT CMAKE_CXX_FLAGS_DEBUG )
		set( CMAKE_CXX_FLAGS_DEBUG "-g -O0" CACHE STRING "C++ compiler flags (debug)" FORCE )
		set( CMAKE_C_FLAGS_DEBUG "-g -O0" CACHE STRING "C++ compiler flags (debug)" FORCE )
	endif()
endif()

# Link to the static sfml libraries when building tgui statically
if(NOT TGUI_SHARED_LIBS)
    set(SFML_STATIC_LIBRARIES TRUE)
endif()

# Find sfml
find_package( SFML 2.0 COMPONENTS graphics window system )

# FindSFML couldn't find SFML.
if( NOT SFML_FOUND )
	set( SFMLDIR "" CACHE PATH "SFML root directory" )
	message( FATAL_ERROR "CMake couldn't find SFML. Set the SFMLDIR entry to SFML's root directory (containing \"include\" and \"lib\" directories)." )
endif()

# Set the path for the libraries
set( LIBRARY_OUTPUT_PATH "${PROJECT_BINARY_DIR}/lib" )

# Jump to the CMakeLists.txt file in the source folder
add_subdirectory(src/TGUI)

# install include files
# TODO

# Create uninstall
# TODO

