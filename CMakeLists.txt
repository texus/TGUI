cmake_minimum_required(VERSION 2.8)

# Define a macro that helps defining an option
macro(tgui_set_option var default type docstring)
    if(NOT DEFINED ${var})
        set(${var} ${default})
    endif()
    set(${var} ${${var}} CACHE ${type} ${docstring} FORCE)
endmacro()

# Set a default build type and module path if none was provided
tgui_set_option(CMAKE_BUILD_TYPE Release STRING "Choose the type of build (Debug or Release)")
tgui_set_option(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake/Modules" STRING "The path to the cmake modules. This path must contain the FindSFML.cmake file.")

# Project name
project(tgui)

# project version
SET( MAJOR_VERSION 0 )
SET( MINOR_VERSION 7 )
SET( PATCH_VERSION 0 )

# Fill in the version in the config file
configure_file ("${CMAKE_CURRENT_SOURCE_DIR}/include/TGUI/Config.hpp.in"
                "${CMAKE_CURRENT_SOURCE_DIR}/include/TGUI/Config.hpp")

# Include the configuration file
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/Config.cmake)

# Add an option for choosing the build type (shared or static)
if(NOT (SFML_OS_IOS OR SFML_OS_ANDROID))
    tgui_set_option(TGUI_SHARED_LIBS TRUE BOOL "TRUE to build TGUI as a shared library, FALSE to build it as a static library")
else()
    if(SFML_OS_IOS)
        set(TGUI_SHARED_LIBS FALSE)
    elseif(SFML_OS_ANDROID)
        set(TGUI_SHARED_LIBS TRUE)
    endif()
endif()

# Add option to build the examples
if(SFML_OS_IOS OR SFML_OS_ANDROID)
    set(TGUI_BUILD_EXAMPLES FALSE)
else()
    tgui_set_option(TGUI_BUILD_EXAMPLES FALSE BOOL "TRUE to build the TGUI examples, FALSE to ignore them")
endif()

# Add option to build the tests on linux
if(SFML_OS_IOS OR SFML_OS_ANDROID)
    set(TGUI_BUILD_TESTS FALSE)
else()
    tgui_set_option(TGUI_BUILD_TESTS FALSE BOOL "TRUE to build the TGUI tests (requires c++14)")
endif()

# Add an option for choosing the OpenGL implementation
tgui_set_option(TGUI_OPENGL_ES ${OPENGL_ES} BOOL "TRUE to use an OpenGL ES implementation, FALSE to use a desktop OpenGL implementation")

# Add an option to build the documentation
tgui_set_option( TGUI_BUILD_DOC FALSE BOOL "TRUE to generate the API documentation, FALSE to ignore it")

# Set compile flags for gcc and clang
if (SFML_OS_ANDROID)
    set(TGUI_ACTIVITY_CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS})

    string(REPLACE " " ";" TGUI_CXX_FLAGS_LIST ${CMAKE_CXX_FLAGS})

    # Remove "-fno-exceptions" from the CMAKE_CXX_FLAGS
    set(TGUI_TEMP_CXX_FLAGS "")
    foreach (flag ${TGUI_CXX_FLAGS_LIST})
        if (NOT (${flag} STREQUAL "-fno-exceptions"))
            set(TGUI_TEMP_CXX_FLAGS "${TGUI_TEMP_CXX_FLAGS} ${flag}")
        endif()
    endforeach(flag)

    # Detect if we have "-std=c++11" and "-fexceptions" flags already
    set(TGUI_CPP11_SET FALSE)
    set(TGUI_EXCEPTIONS_SET FALSE)
    foreach (flag ${TGUI_CXX_FLAGS_LIST})
        if (${flag} STREQUAL "-std=c++11")
            set(TGUI_CPP11_SET TRUE)
        endif()
        if (${flag} STREQUAL "-fexceptions")
            set(TGUI_EXCEPTIONS_SET TRUE)
        endif()
    endforeach(flag)

    # Add the "-std=c++11" flag if it wasn't there already
    if (NOT TGUI_CPP11_SET)
        set(TGUI_TEMP_CXX_FLAGS "${TGUI_TEMP_CXX_FLAGS} -std=c++11")
    endif()

    # Add the "-fexceptions" flag if it wasn't there already
    if (NOT TGUI_EXCEPTIONS_SET)
        set(TGUI_TEMP_CXX_FLAGS "${TGUI_TEMP_CXX_FLAGS} -fexceptions")
    endif()

    set(CMAKE_CXX_FLAGS "${TGUI_TEMP_CXX_FLAGS}")

elseif(SFML_COMPILER_GCC OR SFML_COMPILER_CLANG)

    if (NOT CMAKE_CXX_FLAGS)
        set(CMAKE_CXX_FLAGS "-Wall -Wextra -Wshadow -Wno-long-long -pedantic -std=c++11" CACHE STRING "C compiler flags" FORCE)
        set(CMAKE_C_FLAGS "-Wall -Wextra -Wshadow -Wno-long-long -pedantic" CACHE STRING "C compiler flags" FORCE)

        # On mac, clang needs another parameter
        if (SFML_COMPILER_CLANG AND SFML_OS_MACOSX)
            set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++" CACHE STRING "C++ compiler flags" FORCE)
        endif()
    endif()
endif()

# Define an option for choosing between static and dynamic C runtime (VC++ only)
if (SFML_OS_WINDOWS)
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

# Android options
if(SFML_OS_ANDROID)

    # Force usage of the STL port
    set(ANDROID_USE_STLPORT TRUE)

    # Make sure there's the android library available
    if (${ANDROID_NATIVE_API_LEVEL} LESS 9)
        message(FATAL_ERROR "API level must be equal or greater than 9")
    endif()

    # Install everything in $NDK/sources/ because this path is appended by the NDK (convenient)
    set(CMAKE_INSTALL_PREFIX ${ANDROID_NDK}/sources/tgui)

    # We install libs in a subdirectory named after the ABI (e.g. lib/armeabi/libtgui.so)
    set(LIB_SUFFIX "/${ANDROID_ABI}")
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
        elseif(SFML_OS_ANDROID)
            set(CMAKE_LIBRARY_PATH ${CMAKE_LIBRARY_PATH} "${ANDROID_NDK}/sources/sfml/extlibs/lib/armeabi/")
        elseif(SFML_OS_IOS)
            set(CMAKE_LIBRARY_PATH ${CMAKE_LIBRARY_PATH} "${SFML_ROOT}/extlibs/libs-ios/")
        endif()
    endif()
endif()

# Find sfml (also look for the main component when using Visual Studio)
if (SFML_OS_WINDOWS AND SFML_COMPILER_MSVC)
    find_package(SFML 2 COMPONENTS main graphics window system)
elseif (SFML_OS_ANDROID)
    find_host_package(SFML 2 COMPONENTS graphics window system)
else()
    find_package(SFML 2 COMPONENTS graphics window system)
endif()

# FindSFML couldn't find SFML.
if( NOT SFML_FOUND )
    set( SFML_ROOT "" CACHE PATH "SFML root directory" )
    message( FATAL_ERROR "CMake couldn't find SFML. Set the SFML_ROOT entry to SFML's root directory (containing \"include\" and \"lib\" directories)." )
endif()

# Set the path for the libraries
set( LIBRARY_OUTPUT_PATH "${PROJECT_BINARY_DIR}/lib" )

# Add the sfml and tgui include directories
include_directories( "${PROJECT_SOURCE_DIR}/include" )
include_directories( ${SFML_INCLUDE_DIR} )

# OpenGL is required (due to a temporary fix)
if (NOT TGUI_OPENGL_ES)
    find_package(OpenGL REQUIRED)
    set(TGUI_EXT_LIBS ${OPENGL_gl_LIBRARY} ${SFML_LIBRARIES})
else()
    if (SFML_OS_LINUX)

        # Try to manually find the packages for when we are on Raspberry Pi
        find_path(EGL_INCLUDE_DIR EGL/egl.h PATHS /opt/vc/include)
        find_path(GLES_INCLUDE_DIR GLES/gl.h PATHS /opt/vc/include)
        find_library(EGL_LIBRARY NAMES EGL PATHS /opt/vc/lib)
        find_library(GLES_LIBRARY NAMES GLESv1_CM PATHS /opt/vc/lib)

        # Search on other places in case we did not find them yet
        find_package(EGL REQUIRED)
        find_package(GLES REQUIRED)

        add_definitions(-DSFML_OPENGL_ES)
        include_directories(${EGL_INCLUDE_DIR} ${GLES_INCLUDE_DIR})
    elseif(SFML_OS_ANDROID)
        set(TGUI_EXT_LIBS ${SFML_LIBRARIES} "-lEGL -lGLESv1_CM")
    endif()
endif()

# We need to link to an extra library on android (to use the asset manager)
if(SFML_OS_ANDROID)
    set(TGUI_EXT_LIBS ${TGUI_EXT_LIBS} "-landroid")
endif()

# Add SFML_STATIC define when linking statically
if(NOT TGUI_SHARED_LIBS)
    add_definitions(-DSFML_STATIC)
endif()

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

# Build the examples if requested
if (TGUI_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

# Build the tests if requested
if (TGUI_BUILD_TESTS)
    if (NOT ${CMAKE_BUILD_TYPE} STREQUAL "Debug")
        message(FATAL_ERROR "TGUI_BUILD_TESTS should only be enabled when CMAKE_BUILD_TYPE is Debug")
    endif()

    add_subdirectory(tests)
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

# Install Android.mk so the NDK knows how to set up TGUI
if(SFML_OS_ANDROID)
    install(FILES Android.mk DESTINATION .)
endif()

# Fix CMake install rules broken for iOS (see http://public.kitware.com/Bug/view.php?id=12506)
if(SFML_OS_IOS)
    install(DIRECTORY "${CMAKE_BINARY_DIR}/lib/\$ENV{CONFIGURATION}/" DESTINATION lib${LIB_SUFFIX})
endif()
