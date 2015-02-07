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

# project version
SET( MAJOR_VERSION 0 )
SET( MINOR_VERSION 6 )
SET( PATCH_VERSION 7 )

# Fill in the version in the config file
configure_file ("${CMAKE_CURRENT_SOURCE_DIR}/include/TGUI/Config.hpp.in"
                "${CMAKE_CURRENT_SOURCE_DIR}/include/TGUI/Config.hpp")

# include the configuration file
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/Config.cmake)

# project options
tgui_set_option( TGUI_BUILD_DOC FALSE BOOL "TRUE to generate the API documentation, FALSE to ignore it")
tgui_set_option( TGUI_SHARED_LIBS TRUE BOOL "Build shared libraries (Set to OFF to build static libraries)" )

if (SFML_OS_LINUX)
    tgui_set_option( TGUI_FORM_BUILDER_USE_LOCAL_FILES FALSE BOOL "Use resources from current directory instead of from installed files." )
endif()

if (SFML_OS_WINDOWS)
    tgui_set_option( TGUI_BUILD_FORM_BUILDER FALSE BOOL "Build the TGUI Form Builder" )
else()
    tgui_set_option( TGUI_BUILD_FORM_BUILDER TRUE BOOL "Build the TGUI Form Builder" )
endif()

# Set some useful compile flags.
if( SFML_COMPILER_GCC )

    if( NOT CMAKE_CXX_FLAGS )
        set( CMAKE_CXX_FLAGS "-Wall -Wextra -Wshadow -Wno-long-long -pedantic -std=c++0x" CACHE STRING "C++ compiler flags" FORCE )
        set( CMAKE_C_FLAGS "-Wall -Wextra -Wshadow -Wno-long-long -pedantic" CACHE STRING "C compiler flags" FORCE )
    endif()

elseif ( SFML_COMPILER_CLANG )

    if( NOT CMAKE_CXX_FLAGS )
        set( CMAKE_CXX_FLAGS "-Wall -Wextra -Wshadow -Wno-long-long -pedantic -std=c++11" CACHE STRING "C++ compiler flags" FORCE )
        set( CMAKE_C_FLAGS "-Wall -Wextra -Wshadow -Wno-long-long -pedantic" CACHE STRING "C compiler flags" FORCE )

        # On mac, clang needs another parameter
        if (SFML_OS_MACOSX)
            set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++" CACHE STRING "C++ compiler flags" FORCE )
        endif()
    endif()

endif()

# Define an option for choosing between static and dynamic C runtime (VC++ only)
if(SFML_OS_WINDOWS)
    tgui_set_option(TGUI_USE_STATIC_STD_LIBS FALSE BOOL "TRUE to statically link to the standard libraries, FALSE to use them as DLLs. This option has to match with the one from sfml.")

    # The following combination of flags is not valid
    if (TGUI_SHARED_LIBS AND TGUI_USE_STATIC_STD_LIBS)
        message(FATAL_ERROR "TGUI_SHARED_LIBS and TGUI_USE_STATIC_STD_LIBS cannot be used together")
    endif()

    # Apply it globally by modifying the compiler flags
    if(SFML_COMPILER_MSVC AND TGUI_USE_STATIC_STD_LIBS)
        foreach(flag
                CMAKE_CXX_FLAGS CMAKE_CXX_FLAGS_DEBUG CMAKE_CXX_FLAGS_RELEASE
                CMAKE_CXX_FLAGS_MINSIZEREL CMAKE_CXX_FLAGS_RELWITHDEBINFO)
            if(${flag} MATCHES "/MD")
                string(REGEX REPLACE "/MD" "/MT" ${flag} "${${flag}}")
            endif()
        endforeach()
    endif()
endif()

# Mac OS X specific options
if (SFML_OS_MACOSX)

    # Add an option to build framework instead of dylib (release only)
    tgui_set_option(TGUI_BUILD_FRAMEWORK FALSE BOOL "TRUE to build TGUI as a framework library (release only), FALSE to build according to TGUI_SHARED_LIBS")

    # Add an option to let the user specify a custom directory for framework installation
    tgui_set_option(CMAKE_INSTALL_FRAMEWORK_PREFIX "/Library/Frameworks" STRING "Frameworks installation directory")

    # Set the architecture when none was given
    if (NOT CMAKE_OSX_ARCHITECTURES)
        set(CMAKE_OSX_ARCHITECTURES "i386;x86_64" CACHE STRING "Build architectures for OSX" FORCE)
    endif()

    # Enable to use of rpath according to CMake Policy CMP0042
    set(CMAKE_MACOSX_RPATH 1)

    if (TGUI_BUILD_FRAMEWORK)
        # Frameworks are only available for release (because cmake currently doesn't allow specifying a custom framework name so XXX-d is not possible)
        if(NOT CMAKE_BUILD_TYPE STREQUAL "Release")
            message(FATAL_ERROR "CMAKE_BUILD_TYPE should be \"Release\" when TGUI_BUILD_FRAMEWORK is TRUE")
            return()
        endif()

        # Frameworks only work with TGUI_SHARED_LIBS enabled
        if (NOT TGUI_SHARED_LIBS)
            message(FATAL_ERROR "TGUI_SHARED_LIBS should be TRUE when TGUI_BUILD_FRAMEWORK is TRUE")
            return()
        endif()
    endif()
endif()

# Link to the static sfml libraries when building tgui statically
if(NOT TGUI_SHARED_LIBS)
    set(SFML_STATIC_LIBRARIES TRUE)

    # Attempt to find the SFML dependencies when linking statically
    if (SFML_ROOT)
        if (SFML_OS_WINDOWS)
            set(CMAKE_INCLUDE_PATH ${CMAKE_INCLUDE_PATH} "${SFML_ROOT}/extlibs/headers")
            if(SFML_COMPILER_GCC)
                if(ARCH_32BITS)
                    set(CMAKE_LIBRARY_PATH ${CMAKE_LIBRARY_PATH} "${SFML_ROOT}/extlibs/libs-mingw/x86")
                    set(CMAKE_LIBRARY_PATH ${CMAKE_LIBRARY_PATH} "${SFML_ROOT}/extlibs/bin/x86")
                elseif(ARCH_64BITS)
                    set(CMAKE_LIBRARY_PATH ${CMAKE_LIBRARY_PATH} "${SFML_ROOT}/extlibs/libs-mingw/x64")
                    set(CMAKE_LIBRARY_PATH ${CMAKE_LIBRARY_PATH} "${SFML_ROOT}/extlibs/bin/x64")
                endif()
            elseif(SFML_COMPILER_MSVC)
                if(ARCH_32BITS)
                    set(CMAKE_LIBRARY_PATH ${CMAKE_LIBRARY_PATH} "${SFML_ROOT}/extlibs/libs-msvc/x86")
                elseif(ARCH_64BITS)
                    set(CMAKE_LIBRARY_PATH ${CMAKE_LIBRARY_PATH} "${SFML_ROOT}/extlibs/libs-msvc/x64")
                endif()
            endif()
        elseif(SFML_OS_MACOSX)
            set(CMAKE_INCLUDE_PATH ${CMAKE_INCLUDE_PATH} "${SFML_ROOT}/extlibs/headers")
            set(CMAKE_LIBRARY_PATH ${CMAKE_LIBRARY_PATH} "${SFML_ROOT}/extlibs/libs-osx/lib/")
        endif()
    endif()
endif()

# Find sfml (also look for the main component when using Visual Studio)
if (SFML_OS_WINDOWS AND SFML_COMPILER_MSVC)
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

# Install the widgets and fonts on linux
if (SFML_OS_LINUX)
    install( DIRECTORY widgets fonts DESTINATION "${INSTALL_MISC_DIR}" )
endif()

# Build the documentation when requested
if (TGUI_BUILD_DOC)
    add_subdirectory(doc)
endif()

# Install include files
if (NOT TGUI_BUILD_FRAMEWORK)
    install(DIRECTORY include
            DESTINATION .
            COMPONENT devel
            FILES_MATCHING PATTERN "*.hpp" PATTERN "*.inl")
endif()

# Install FindTGUI.cmake file
if(NOT SFML_OS_ANDROID)
    install(FILES cmake/Modules/FindTGUI.cmake DESTINATION "${INSTALL_MISC_DIR}/cmake/Modules")
endif()

