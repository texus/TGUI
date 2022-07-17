if(CMAKE_SYSTEM_NAME MATCHES "Android")
    cmake_minimum_required(VERSION 3.7)  # CMake's built-in Android support requires 3.7
else()
    cmake_minimum_required(VERSION 3.5)
endif()

# Use new RPATH behavior on macOS (CMake 3.9 and newer)
if(POLICY CMP0068)
    cmake_policy(SET CMP0068 NEW)
endif()

# Prefer GLVND OpenGL libraries on linux (CMake 3.11 and newer)
if (POLICY CMP0072)
    cmake_policy(SET CMP0072 NEW)
endif()

# Don't warn about existence of SFML_ROOT variable (CMake 3.12 and newer)
if(POLICY CMP0074)
    cmake_policy(SET CMP0074 NEW)
endif()

# Use target_sources with relative file paths when possible (CMake 3.13 and newer)
if(POLICY CMP0076)
    cmake_policy(SET CMP0076 NEW)
endif()

# Don't add the /W3 flag to MSVC automatically as it causes warnings when we add /W4 (CMake 3.15 and newer)
if(POLICY CMP0092)
    cmake_policy(SET CMP0092 NEW)
endif()

# Initialize CMAKE_OSX_ARCHITECTURES for macOS if not provided by the user.
# This variable has to be set before the call to project() and is ignored on non-Apple platforms.
if(NOT CMAKE_OSX_ARCHITECTURES)
    set(CMAKE_OSX_ARCHITECTURES "x86_64" CACHE STRING "macOS architecture to build. Possible values are 'x86_64', 'arm64' or 'x86_64;arm64' (to build for both)." FORCE)
endif()

include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/Macros.cmake)

# Set a default build type
tgui_set_option(CMAKE_BUILD_TYPE Release STRING "Choose the type of build (Debug or Release)")

# Project name and version
project(TGUI VERSION 0.9.5)

# TGUI uses the paths from the cmake GNUInstallDirs module as defaults (https://cmake.org/cmake/help/v3.0/module/GNUInstallDirs.html)
include(GNUInstallDirs)

# Include the configuration file
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/Config.cmake)

# Add an option for choosing the build type (shared or static)
if(NOT TGUI_OS_IOS AND NOT TGUI_OS_ANDROID)
    if(DEFINED BUILD_SHARED_LIBS)
        tgui_set_option(TGUI_SHARED_LIBS ${BUILD_SHARED_LIBS} BOOL "TRUE to build TGUI as a shared library, FALSE to build it as a static library")
    else()
        tgui_set_option(TGUI_SHARED_LIBS TRUE BOOL "TRUE to build TGUI as a shared library, FALSE to build it as a static library")
    endif()
else()
    if(TGUI_OS_IOS)
        set(TGUI_SHARED_LIBS FALSE)
    elseif(TGUI_OS_ANDROID)
        set(TGUI_SHARED_LIBS TRUE)
    endif()
endif()

# Add option to build the examples
if(NOT TGUI_OS_ANDROID)
    tgui_set_option(TGUI_BUILD_EXAMPLES FALSE BOOL "TRUE to build the TGUI examples, FALSE to ignore them")
else()
    set(TGUI_BUILD_EXAMPLES FALSE)
endif()

# Add options to build the Gui Builder and tests
if(NOT TGUI_OS_IOS AND NOT TGUI_OS_ANDROID)
    tgui_set_option(TGUI_BUILD_GUI_BUILDER TRUE BOOL "TRUE to compile the GUI Builder")
    tgui_set_option(TGUI_BUILD_TESTS FALSE BOOL "TRUE to build the TGUI tests")
endif()

# Set the default c++ standard version to 17 on GCC >= 11
# Note that we can't use VERSION_GREATER_EQUAL which was only added in CMake 3.7
if (TGUI_COMPILER_GCC AND (NOT ${CMAKE_CXX_COMPILER_VERSION} VERSION_LESS "11"))
    set(TGUI_DEFAULT_CXX_STANDARD "17")
else() # Set the default c++ standard version to 14 on all other compilers
    set(TGUI_DEFAULT_CXX_STANDARD "14")
endif()

tgui_set_option(TGUI_BUILD_DOC FALSE BOOL "TRUE to generate the API documentation, FALSE to ignore it")
tgui_set_option(TGUI_CXX_STANDARD ${TGUI_DEFAULT_CXX_STANDARD} STRING "C++ standard version to build TGUI with. Possible values: 14, 17 or 20. Projects using TGUI must use a version equal or higher to this")

# At least c++14 has to be used
if(TGUI_CXX_STANDARD LESS "14")
    message(FATAL_ERROR "TGUI_CXX_STANDARD has to be at least 14")
endif()

# Define the install directory for miscellaneous files
if(TGUI_OS_WINDOWS OR TGUI_OS_IOS)
    set(DEFAULT_INSTALL_MISC_DIR .)
elseif(TGUI_OS_ANDROID)
    set(DEFAULT_INSTALL_MISC_DIR ${CMAKE_ANDROID_NDK}/sources/third_party/tgui)
else()
    set(DEFAULT_INSTALL_MISC_DIR ${CMAKE_INSTALL_DATAROOTDIR}/tgui-${TGUI_VERSION_MAJOR}.${TGUI_VERSION_MINOR})
endif()
tgui_set_option(TGUI_MISC_INSTALL_PREFIX "${DEFAULT_INSTALL_MISC_DIR}" PATH "Prefix installation path for miscellaneous files")
mark_as_advanced(TGUI_MISC_INSTALL_PREFIX)

# Install pkg-config files by default on Linux and BSD
if (TGUI_OS_LINUX OR TGUI_OS_BSD)
    set(TGUI_INSTALL_PKGCONFIG_DEFAULT TRUE)
else()
    set(TGUI_INSTALL_PKGCONFIG_DEFAULT FALSE)
endif()

tgui_set_option(TGUI_INSTALL_PKGCONFIG_FILES ${TGUI_INSTALL_PKGCONFIG_DEFAULT} BOOL "TRUE to automatically install pkg-config files so other projects can find TGUI")

if (TGUI_INSTALL_PKGCONFIG_FILES)
    tgui_set_option(TGUI_PKGCONFIG_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}/${TGUI_PKGCONFIG_DIR}" PATH "Install directory for TGUI's pkg-config .pc files")

    configure_file("pkgconfig/tgui.pc.in" "pkgconfig/tgui.pc" @ONLY)
    install(FILES "${CMAKE_CURRENT_BINARY_DIR}/pkgconfig/tgui.pc" DESTINATION "${TGUI_PKGCONFIG_INSTALL_PREFIX}")
endif()

# Define an option for choosing between static and dynamic C runtime
if(TGUI_OS_WINDOWS)
    tgui_set_option(TGUI_USE_STATIC_STD_LIBS FALSE BOOL "TRUE to statically link to the standard libraries, FALSE to use them as DLLs. This option has to match with the one from SFML.")

    # The following combination of flags is not valid
    if(TGUI_SHARED_LIBS AND TGUI_USE_STATIC_STD_LIBS)
        message(FATAL_ERROR "TGUI_SHARED_LIBS and TGUI_USE_STATIC_STD_LIBS cannot be used together")
    endif()
endif()

# macOS specific options
if(TGUI_OS_MACOS)

    # Add an option to build framework instead of dylib (release only)
    tgui_set_option(TGUI_BUILD_FRAMEWORK ${TGUI_SHARED_LIBS} BOOL "TRUE to build TGUI as a framework library (release only), FALSE to build according to TGUI_SHARED_LIBS")

    # Only the default x86_64 or arm64 architectures are supported
    foreach (arch IN LISTS CMAKE_OSX_ARCHITECTURES)
        if (NOT (arch STREQUAL "x86_64" OR arch STREQUAL "arm64"))
            message(FATAL_ERROR "Invalid arch ${arch}, only x86_64 and arm64 are supported")
        endif()
    endforeach()

    # Enable to use of rpath according to CMake Policy CMP0042
    set(CMAKE_MACOSX_RPATH 1)

    if(TGUI_BUILD_FRAMEWORK)
        # Frameworks are only available for release (because cmake currently doesn't allow specifying a custom framework name so TGUI-d is not possible)
        if(NOT CMAKE_BUILD_TYPE STREQUAL "Release")
            message(FATAL_ERROR "CMAKE_BUILD_TYPE should be \"Release\" when TGUI_BUILD_FRAMEWORK is TRUE")
            return()
        endif()

        # Frameworks only work with TGUI_SHARED_LIBS enabled
        if(NOT TGUI_SHARED_LIBS)
            message(FATAL_ERROR "TGUI_SHARED_LIBS should be TRUE when TGUI_BUILD_FRAMEWORK is TRUE")
            return()
        endif()
    endif()
endif()

# Android options
if(TGUI_OS_ANDROID)

    # Make sure there's the android library available
    if(CMAKE_ANDROID_API LESS 19)
        message(FATAL_ERROR "Android API level (${CMAKE_ANDROID_API}) must be equal or greater than 19.")
    endif()

    # CMake doesn't support defining the STL to be used with Nsight Tegra, so warn the user
    if(CMAKE_VS_PLATFORM_NAME STREQUAL "Tegra-Android")
        message(WARNING "CMake might not properly support setting the STL. Make sure to adjust all generated library projects!")
    endif()

    # Install everything in $NDK/sources/third_party by default
    if (CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
        set(CMAKE_INSTALL_PREFIX ${CMAKE_ANDROID_NDK}/sources/third_party/tgui CACHE PATH "Installation path (should be inside your NDK's 'sources' directory)" FORCE)
    endif()

    # We install libs in a subdirectory named after the ABI (e.g. lib/armeabi/libtgui.so)
    set(CMAKE_INSTALL_LIBDIR "${CMAKE_INSTALL_LIBDIR}/${CMAKE_ANDROID_ARCH_ABI}")
endif()

# Enable OpenGL ES on mobile platforms or when explicitly requested (option is ignored by backends that don't use OpenGL)
if(TGUI_OS_IOS OR TGUI_OS_ANDROID)
    set(TGUI_USE_GLES 1)
elseif (NOT DEFINED TGUI_USE_GLES)
    set(TGUI_USE_GLES 0)
endif()

# Add an option to choose whether PDB debug symbols should be generated for Visual Studio
if(TGUI_COMPILER_MSVC)
    tgui_set_option(TGUI_GENERATE_PDB TRUE BOOL "True to generate PDB debug symbols, FALSE otherwise.")
endif()

# Apply the TGUI_USE_STATIC_STD_LIBS option on windows when using VC++
# This can't be done in the tgui_set_stdlib macro because it is too late to change these variables by then
if(TGUI_USE_STATIC_STD_LIBS AND (TGUI_COMPILER_MSVC OR (TGUI_OS_WINDOWS AND TGUI_COMPILER_CLANG AND NOT MINGW)))
    foreach(flag
            CMAKE_CXX_FLAGS CMAKE_CXX_FLAGS_DEBUG CMAKE_CXX_FLAGS_RELEASE
            CMAKE_CXX_FLAGS_MINSIZEREL CMAKE_CXX_FLAGS_RELWITHDEBINFO)
        if(${flag} MATCHES "/MD")
            string(REGEX REPLACE "/MD" "/MT" ${flag} "${${flag}}")
        endif()
    endforeach()
endif()

# Set the path for the libraries
set(LIBRARY_OUTPUT_PATH "${PROJECT_BINARY_DIR}/lib")

# Jump to the CMakeLists.txt file in the source folder
add_subdirectory(src)

# Generate the Config.hpp header
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/include/TGUI/Config.hpp.in ${CMAKE_CURRENT_SOURCE_DIR}/include/TGUI/Config.hpp @ONLY)

# Build the documentation when requested
if(TGUI_BUILD_DOC)
    add_subdirectory(doc)
endif()

# Build the examples if requested
if(TGUI_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

# Build the tests if requested
if(TGUI_BUILD_TESTS)
    if(NOT ${CMAKE_BUILD_TYPE} STREQUAL "Debug")
        message(WARNING "TGUI_BUILD_TESTS is ON but CMAKE_BUILD_TYPE isn't Debug")
    endif()

    add_subdirectory(tests)
endif()

# Build the GUI Builder if requested
if(TGUI_BUILD_GUI_BUILDER)
    add_subdirectory("${PROJECT_SOURCE_DIR}/gui-builder")
endif()

# Install include files
if(NOT TGUI_BUILD_FRAMEWORK)
    install(DIRECTORY include
            DESTINATION .
            COMPONENT devel
            FILES_MATCHING PATTERN "*.hpp" PATTERN "*.inl")

    if(TGUI_GENERATE_PDB)
        install(DIRECTORY "${PROJECT_BINARY_DIR}/lib"
                DESTINATION .
                COMPONENT devel
                FILES_MATCHING PATTERN "*.pdb")
    endif()
endif()

# Install miscellaneous files
install(FILES license.txt DESTINATION ${CMAKE_INSTALL_DOCDIR})
install(FILES README.md DESTINATION ${CMAKE_INSTALL_DOCDIR})
install(DIRECTORY themes DESTINATION "${TGUI_MISC_INSTALL_PREFIX}")
