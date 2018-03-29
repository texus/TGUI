# CMake's built-in Android support requires 3.7
if(CMAKE_SYSTEM_NAME MATCHES "Android")
    cmake_minimum_required(VERSION 3.7)
else()
    cmake_minimum_required(VERSION 2.8)
endif()

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

# Use new RPATH behaviors on macOS
if(NOT CMAKE_VERSION VERSION_LESS 3.0)
    cmake_policy(SET CMP0042 NEW)
endif()
if (NOT CMAKE_VERSION VERSION_LESS 3.9)
    cmake_policy(SET CMP0068 NEW)
endif()

# Set a default build type and module path if none was provided
tgui_set_option(CMAKE_BUILD_TYPE Release STRING "Choose the type of build (Debug or Release)")

# Set CMAKE_MODULE_PATH to find SFML < 2.5 without manually having to specify a module path
if ((NOT DEFINED SFML_DIR OR "${SFML_DIR}" STREQUAL "" OR "${SFML_DIR}" STREQUAL "SFML_DIR-NOTFOUND") AND NOT DEFINED CMAKE_MODULE_PATH)
    set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake/Modules")
endif()

# Project name
project(tgui)

# project version
SET(MAJOR_VERSION 0)
SET(MINOR_VERSION 7)
SET(PATCH_VERSION 6)

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

# Add option to build the tests
if(SFML_OS_IOS OR SFML_OS_ANDROID)
    set(TGUI_BUILD_TESTS FALSE)
else()
    tgui_set_option(TGUI_BUILD_TESTS FALSE BOOL "TRUE to build the TGUI tests (requires c++14)")
endif()

# Add an option to build the documentation
tgui_set_option( TGUI_BUILD_DOC FALSE BOOL "TRUE to generate the API documentation, FALSE to ignore it")

# Set compile flags depending on the compiler
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
    tgui_add_cxx_flag(-std=gnu++11)

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
    # Make sure there's the android library available
    if (CMAKE_ANDROID_API LESS 9)
        message(FATAL_ERROR "Android API level (${CMAKE_ANDROID_API}) must be equal or greater than 9.")
    endif()

    # CMake doesn't support defining the STL to be used with Nsight Tegra, so warn the user
    if(CMAKE_VS_PLATFORM_NAME STREQUAL "Tegra-Android")
        message(WARNING "CMake might not properly support setting the STL. Make sure to adjust all generated library projects!")
    endif()

    # Install everything in $NDK/sources/ because this path is appended by the NDK (convenient)
    set(CMAKE_INSTALL_PREFIX ${CMAKE_ANDROID_NDK}/sources/third_party/tgui)

    # We install libs in a subdirectory named after the ABI (e.g. lib/armeabi/libtgui.so)
    set(LIB_SUFFIX "/${CMAKE_ANDROID_ARCH_ABI}")

    # Pass shared STL configuration (if any)
    if (CMAKE_ANDROID_STL_TYPE MATCHES "_shared")
        add_definitions("-DSTL_LIBRARY=${CMAKE_ANDROID_STL_TYPE}")
        if (NOT CMAKE_ANDROID_STL_TYPE MATCHES "c\\+\\+_shared")
            message("Android: Using ${CMAKE_ANDROID_STL_TYPE} as STL. Set CMAKE_ANDROID_STL_TYPE to c++_shared, if there are any issues.")
        endif()
    else()
        message(WARNING "Android: You're using a static STL (${CMAKE_ANDROID_STL_TYPE}). Set CMAKE_ANDROID_STL_TYPE to c++_shared, if there are any issues.")
    endif()
endif()

# Link to the static sfml libraries when building tgui statically
if (NOT TGUI_SHARED_LIBS)
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
            set(CMAKE_LIBRARY_PATH ${CMAKE_LIBRARY_PATH} "${CMAKE_ANDROID_NDK}/sources/third_party/sfml/extlibs/lib/${CMAKE_ANDROID_ARCH_ABI}/")
        elseif(SFML_OS_IOS)
            set(CMAKE_LIBRARY_PATH ${CMAKE_LIBRARY_PATH} "${SFML_ROOT}/extlibs/libs-ios/")
        endif()
    endif()
endif()

# Find sfml
if (SFML_OS_WINDOWS AND SFML_COMPILER_MSVC)  # Also look for the main component when using Visual Studio
    find_package(SFML 2 COMPONENTS main graphics window system)
elseif (SFML_OS_IOS)  # Use the function from the toolchain on iOS
    find_host_package(SFML 2 COMPONENTS graphics window system)
else()
    find_package(SFML 2 COMPONENTS graphics window system)
endif()

# FindSFML couldn't find SFML.
if (NOT SFML_FOUND)
    set(SFML_DIR "" CACHE PATH "Path to SFMLConfig.cmake")
    set(SFML_ROOT "" CACHE PATH "SFML root directory")
    message(FATAL_ERROR "CMake couldn't find SFML.\nEither set SFML_DIR to the directory containing SFMLConfig.cmake or set the SFML_ROOT entry to SFML's root directory (containing \"include\" and \"lib\" directories).")
endif()

# Set the path for the libraries
set( LIBRARY_OUTPUT_PATH "${PROJECT_BINARY_DIR}/lib" )

# Add the sfml and tgui include directories
include_directories( "${PROJECT_SOURCE_DIR}/include" )
include_directories( ${SFML_INCLUDE_DIR} )

# Targets to link are specified manually in case SFMLConfig.cmake is used, SFML_DEPENDENCIES is added in case FindSFML.cmake is used
if (SFML_OS_WINDOWS AND SFML_COMPILER_MSVC)  # Also look for the main component when using Visual Studio
    set(TGUI_EXT_LIBS sfml-main sfml-graphics sfml-window sfml-system ${SFML_DEPENDENCIES})
else()
    set(TGUI_EXT_LIBS sfml-graphics sfml-window sfml-system ${SFML_DEPENDENCIES})
endif()

if(SFML_OS_ANDROID)
    # We need to link to an extra library on android (to use the asset manager)
    set(TGUI_EXT_LIBS ${TGUI_EXT_LIBS} "-landroid")
endif()

# Add SFML_STATIC define when linking statically
if(NOT TGUI_SHARED_LIBS)
    add_definitions(-DSFML_STATIC)
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
