cmake_minimum_required(VERSION 2.8)

# Define a macro that helps defining an option
macro(tgui_set_option var default type docstring)
    if(NOT DEFINED ${var})
        set(${var} ${default})
    endif()
    set(${var} ${${var}} CACHE ${type} ${docstring} FORCE)
endmacro()

# Define a macro to add compiler flags
macro(tgui_add_cxx_flag flag)
    string(REGEX REPLACE "\\+" "\\\\+" escapedFlag ${flag})
    if (NOT (${CMAKE_CXX_FLAGS} MATCHES ".*${escapedFlag}.*"))
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${flag}" CACHE STRING "C++ compiler flags" FORCE)
    endif()
endmacro()

# Set a default build type and module path if none was provided
tgui_set_option(CMAKE_BUILD_TYPE Release STRING "Choose the type of build (Debug or Release)")
tgui_set_option(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake/Modules" STRING "The path to the cmake modules. This path must contain the FindSFML.cmake file.")

# Android specific stuff which has to be set before CMake detects/configures the toolchain.
# If not building on android they will be unset later.

    # Define the minimum API level to be used
    tgui_set_option(ANDROID_API_MIN 9 STRING "Choose the Android API level to be used (minimum 9)")

    # Mirror the setting for the toolchain file
    set(ANDROID_NATIVE_API_LEVEL ${ANDROID_API_MIN})

    # Define the path to the Android NDK
    tgui_set_option(ANDROID_NDK "$ENV{ANDROID_NDK}" PATH "Path to the Android NDK")

    # Define the STL implementation to be used
    tgui_set_option(ANDROID_STL c++_shared STRING "Choose the STL implementation to be used (experimental)")

    # Default the ABI to ARM v7a for hardware floating point
    if(NOT ANDROID_ABI)
        set(ANDROID_ABI armeabi-v7a)
    endif()

# Project name
project(tgui)

# project version
SET( MAJOR_VERSION 0 )
SET( MINOR_VERSION 7 )
SET( PATCH_VERSION 3 )

# Disable deprecated warnings from SFML
add_definitions(-DSFML_NO_DEPRECATED_WARNINGS)

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
    # Remove "-fno-exceptions" from the CMAKE_CXX_FLAGS
    string(REPLACE " " ";" TGUI_CXX_FLAGS_LIST ${CMAKE_CXX_FLAGS})
    set(TGUI_TEMP_CXX_FLAGS "")
    foreach (flag ${TGUI_CXX_FLAGS_LIST})
        if (NOT (${flag} STREQUAL "-fno-exceptions"))
            set(TGUI_TEMP_CXX_FLAGS "${TGUI_TEMP_CXX_FLAGS} ${flag}")
        endif()
    endforeach(flag)

    # Remove "-fno-rtti" from the CMAKE_CXX_FLAGS
    string(REPLACE " " ";" TGUI_CXX_FLAGS_LIST ${TGUI_TEMP_CXX_FLAGS})
    set(TGUI_TEMP_CXX_FLAGS "")
    foreach (flag ${TGUI_CXX_FLAGS_LIST})
        if (NOT (${flag} STREQUAL "-fno-rtti"))
            set(TGUI_TEMP_CXX_FLAGS "${TGUI_TEMP_CXX_FLAGS} ${flag}")
        endif()
    endforeach(flag)

    set(CMAKE_CXX_FLAGS "${TGUI_TEMP_CXX_FLAGS}")

    # Add the "-std=c++11", "-fexceptions" and "-frtti" flags if they aren't there already
    tgui_add_cxx_flag(-std=c++11)
    tgui_add_cxx_flag(-fexceptions)
    tgui_add_cxx_flag(-frtti)

    set(TGUI_ACTIVITY_CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS})

elseif(SFML_COMPILER_GCC OR SFML_COMPILER_CLANG)
    tgui_add_cxx_flag(-Wall)
    tgui_add_cxx_flag(-Wextra)
    tgui_add_cxx_flag(-Wshadow)
    tgui_add_cxx_flag(-Wno-long-long)
    tgui_add_cxx_flag(-pedantic)
    tgui_add_cxx_flag(-std=c++11)

    # On mac, clang needs another parameter
    if (SFML_COMPILER_CLANG AND SFML_OS_MACOSX)
        tgui_add_cxx_flag(-stdlib=libc++)
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

    # Only the default architecture (i.e. 64-bit) is supported
    if(CMAKE_OSX_ARCHITECTURES AND NOT "${CMAKE_OSX_ARCHITECTURES}" STREQUAL "x86_64")
        message(FATAL_ERROR "Only 64-bit architecture is supported")
        return()
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

    if (${ANDROID_API_MIN} LESS 9)
        message(FATAL_ERROR "Android API level must be equal or greater than 9. Please adjust the CMake variable 'ANDROID_API_MIN'.")
    endif()

    if(NOT ANDROID_NDK)
        message(FATAL_ERROR "The Android NDK couldn't be found. Please adjust the CMake variable 'ANDROID_NDK' to point to the NDK directory.")
    endif()

    # CMake doesn't support defining the STL to be used with Nsight Tegra, so warn the user
    if(CMAKE_VS_PLATFORM_NAME STREQUAL "Tegra-Android")
        message(WARNING "CMake might not properly support setting the STL. Make sure to adjust all generated library projects!")
    endif()

    # Install everything in $NDK/sources/ because this path is appended by the NDK (convenient)
    set(CMAKE_INSTALL_PREFIX ${ANDROID_NDK}/sources/tgui)

    # We install libs in a subdirectory named after the ABI (e.g. lib/armeabi/libtgui.so)
    set(LIB_SUFFIX "/${ANDROID_ABI}")

    # Pass shared STL configuration (if any)
    if (ANDROID_STL MATCHES "_shared")
        add_definitions("-DSTL_LIBRARY=${ANDROID_STL}")
    endif()

    # Macro to find packages on the host OS
    # This is the same as in the toolchain file, which is here for Nsight Tegra VS since it won't use the Android toolchain file
    if(CMAKE_VS_PLATFORM_NAME STREQUAL "Tegra-Android")
        macro(find_host_package)
            set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
            set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY NEVER)
            set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE NEVER)
            if(CMAKE_HOST_WIN32)
                set(WIN32 1)
                set(UNIX)
            elseif(CMAKE_HOST_APPLE)
                set(APPLE 1)
                set(UNIX)
            endif()
            find_package(${ARGN})
            set(WIN32)
            set(APPLE)
            set(UNIX 1)
            set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM ONLY)
            set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
            set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
        endmacro()
    endif()
else()
    unset(ANDROID_ABI CACHE)
    unset(ANDROID_API_MIN CACHE)
    unset(ANDROID_STL CACHE)
    unset(ANDROID_NATIVE_API_LEVEL CACHE)
    unset(ANDROID_NDK CACHE)
endif()

# Link to the static sfml libraries when building tgui statically
if(NOT TGUI_SHARED_LIBS)
    set(SFML_STATIC_LIBRARIES TRUE)

    # Attempt to find the SFML dependencies when linking statically
    if (SFML_ROOT)
        if (SFML_OS_WINDOWS)
            set(CMAKE_INCLUDE_PATH ${CMAKE_INCLUDE_PATH} "${SFML_ROOT}/extlibs/headers")
            if(ARCH_32BITS)
                if(SFML_COMPILER_MSVC AND MSVC_VERSION LESS 1900) # older than VC++14
                    set(CMAKE_LIBRARY_PATH ${CMAKE_LIBRARY_PATH} "${SFML_ROOT}/extlibs/libs-msvc/x86")
                elseif(SFML_COMPILER_MSVC) # VC++14 or newer
                    set(CMAKE_LIBRARY_PATH ${CMAKE_LIBRARY_PATH} "${SFML_ROOT}/extlibs/libs-msvc-universal/x86")
                else() # gcc
                    set(CMAKE_LIBRARY_PATH ${CMAKE_LIBRARY_PATH} "${SFML_ROOT}/extlibs/libs-mingw/x86")
                endif()
            elseif(ARCH_64BITS)
                if(SFML_COMPILER_MSVC AND MSVC_VERSION LESS 1900) # older than VC++14
                    set(CMAKE_LIBRARY_PATH ${CMAKE_LIBRARY_PATH} "${SFML_ROOT}/extlibs/libs-msvc/x64")
                elseif(SFML_COMPILER_MSVC) # VC++14 or newer
                    set(CMAKE_LIBRARY_PATH ${CMAKE_LIBRARY_PATH} "${SFML_ROOT}/extlibs/libs-msvc-universal/x64")
                else() # gcc
                    set(CMAKE_LIBRARY_PATH ${CMAKE_LIBRARY_PATH} "${SFML_ROOT}/extlibs/libs-mingw/x64")
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

# Add SFML_STATIC define when linking statically and link to SFML dependencies
if(NOT TGUI_SHARED_LIBS)
    add_definitions(-DSFML_STATIC)
    set(TGUI_EXT_LIBS ${TGUI_EXT_LIBS} ${SFML_DEPENDENCIES})
endif()

# Generate .gcno files when requested
if (TGUI_BUILD_TESTS AND TGUI_USE_GCOV)
    tgui_add_cxx_flag(-fprofile-arcs)
    tgui_add_cxx_flag(-ftest-coverage)
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
