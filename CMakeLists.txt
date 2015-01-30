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

# TODO: Add option to build the examples
#if(SFML_OS_IOS OR SFML_OS_ANDROID)
#    set(TGUI_BUILD_EXAMPLES FALSE)
#else()
#    tgui_set_option(TGUI_BUILD_EXAMPLES FALSE BOOL "TRUE to build the TGUI examples, FALSE to ignore them")
#endif()

# Add an option for choosing the OpenGL implementation
tgui_set_option(TGUI_OPENGL_ES ${OPENGL_ES} BOOL "TRUE to use an OpenGL ES implementation, FALSE to use a desktop OpenGL implementation")

# Add an option to build the documentation
tgui_set_option( TGUI_BUILD_DOC FALSE BOOL "TRUE to generate the API documentation, FALSE to ignore it")

# Add options to build the form builder if not on android
if (NOT SFML_OS_ANDROID)
    if (SFML_OS_WINDOWS)
        tgui_set_option( TGUI_BUILD_FORM_BUILDER FALSE BOOL "Build the TGUI Form Builder" )
    else()
        tgui_set_option( TGUI_BUILD_FORM_BUILDER TRUE BOOL "Build the TGUI Form Builder" )
    endif()

    if (SFML_OS_LINUX)
        tgui_set_option( TGUI_FORM_BUILDER_USE_LOCAL_FILES FALSE BOOL "Use resources from current directory instead of from installed files." )
    endif()
endif()

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

elseif(SFML_COMPILER_GCC)

    if (NOT CMAKE_CXX_FLAGS)
        set(CMAKE_CXX_FLAGS "-Wall -Wextra -Wshadow -Wno-long-long -pedantic -std=c++11" CACHE STRING "C++ compiler flags" FORCE)
        set(CMAKE_C_FLAGS "-Wall -Wextra -Wshadow -Wno-long-long -pedantic" CACHE STRING "C compiler flags" FORCE)
    endif()

elseif (SFML_COMPILER_CLANG)

    if (NOT CMAKE_CXX_FLAGS)
        set(CMAKE_CXX_FLAGS "-Wall -Wextra -Wshadow -Wno-long-long -pedantic -std=c++11" CACHE STRING "C++ compiler flags" FORCE)
        set(CMAKE_C_FLAGS "-Wall -Wextra -Wshadow -Wno-long-long -pedantic" CACHE STRING "C compiler flags" FORCE)

        # On mac, clang needs another parameter
        if (SFML_OS_MACOSX)
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

    # Set the correct architecture
    tgui_set_option(CMAKE_OSX_ARCHITECTURES "x86_64" STRING "Build architectures for OSX")

    # TODO: Support frameworks
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
install(DIRECTORY include DESTINATION .)

# Install FindTGUI.cmake file
if(NOT SFML_OS_ANDROID)
    install(FILES cmake/Modules/FindTGUI.cmake DESTINATION "${INSTALL_MISC_DIR}/cmake/Modules")
endif()

# Install Android.mk so the NDK knows how to set up TGUI
if(SFML_OS_ANDROID)
    install(FILES Android.mk DESTINATION .)
endif()
