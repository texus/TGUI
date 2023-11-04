####################################################################################################
# TGUI - Texus' Graphical User Interface
# Copyright (C) 2012-2023 Bruno Van de Velde (vdv_b@tgui.eu)
#
# This software is provided 'as-is', without any express or implied warranty.
# In no event will the authors be held liable for any damages arising from the use of this software.
#
# Permission is granted to anyone to use this software for any purpose,
# including commercial applications, and to alter it and redistribute it freely,
# subject to the following restrictions:
#
# 1. The origin of this software must not be misrepresented;
#    you must not claim that you wrote the original software.
#    If you use this software in a product, an acknowledgment
#    in the product documentation would be appreciated but is not required.
#
# 2. Altered source versions must be plainly marked as such,
#    and must not be misrepresented as being the original software.
#
# 3. This notice may not be removed or altered from any source distribution.
####################################################################################################

# If the cmake version is higher than the minimum version then enable all new policies, up to the maximum version specified.
# When cmake is newer than the highest version then its newest policies will still be set to the old behavior for compatibility.
cmake_minimum_required(VERSION 3.16...3.27)

# Include macros such as tgui_set_option
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/Macros.cmake)

# Set a default build type
tgui_set_option(CMAKE_BUILD_TYPE Release STRING "Choose the type of build (Debug or Release)")

# Project name and version
project(TGUI VERSION 1.1.0 LANGUAGES CXX)

# Use the paths from the cmake GNUInstallDirs module as defaults (https://cmake.org/cmake/help/latest/module/GNUInstallDirs.html)
include(GNUInstallDirs)

# Include the configuration file which identifies the OS and compiler
include(${PROJECT_SOURCE_DIR}/cmake/Config.cmake)

if(TGUI_OS_IOS)
    # SFML 2 doesn't detect iOS in SFMLConfigDependencies.cmake when CMAKE_SYSTEM_NAME isn't "Darwin",
    # but luckily they don't have an error case so we just manually set the option about which OS was detected.
    # This hack is no longer needed for SFML 3.
    set(FIND_SFML_OS_IOS 1)
endif()

# Add the modules directory to find some of the dependencies
list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake/Modules")

# When building for Android, we install everything in $NDK/sources/third_party by default.
# The CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT variable is only TRUE on the first run,
# so this code needs to be before the backend selection (or any other code that can fail).
if(TGUI_OS_ANDROID AND CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    set(CMAKE_INSTALL_PREFIX ${CMAKE_ANDROID_NDK}/sources/third_party/tgui CACHE PATH "Installation path (should be inside your NDK's 'sources' directory)" FORCE)
endif()

# Add an option for choosing the build type (shared or static)
if(TGUI_OS_ANDROID)
    set(TGUI_SHARED_LIBS TRUE)
else()
    # If TGUI_SHARED_LIBS is explicitly set then it overrules BUILD_SHARED_LIBS, otherwise honor the global setting
    if(NOT DEFINED TGUI_SHARED_LIBS)
        if(NOT DEFINED BUILD_SHARED_LIBS)
            option(BUILD_SHARED_LIBS "ON to build a shared (dynamic) library, OFF to build a static library" ON)
        endif()

        set(TGUI_SHARED_LIBS ${BUILD_SHARED_LIBS})
    endif()
endif()

# Use c++17 by default on GCC >= 11, Clang >= 16 and IntelLLVM
if ((TGUI_COMPILER_GCC AND ${CMAKE_CXX_COMPILER_VERSION} VERSION_GREATER_EQUAL "11")
 OR (TGUI_COMPILER_LLVM_CLANG AND ${CMAKE_CXX_COMPILER_VERSION} VERSION_GREATER_EQUAL "16")
 OR TGUI_COMPILER_INTEL_ONEAPI)
    set(TGUI_DEFAULT_CXX_STANDARD "17")
else() # Use c++14 by default on all other compilers
    set(TGUI_DEFAULT_CXX_STANDARD "14")
endif()

tgui_set_option(TGUI_CXX_STANDARD ${TGUI_DEFAULT_CXX_STANDARD} STRING "C++ standard version to build TGUI with. Possible values: 14, 17, 20 or 23. Projects using TGUI must use a c++ version equal or higher to this")

# At least c++14 has to be used
if(TGUI_CXX_STANDARD LESS "14")
    message(FATAL_ERROR "TGUI_CXX_STANDARD has to be at least 14")
endif()

if(TGUI_BACKEND)
    set(backend_provided TRUE)
endif()

# Construct a list of supported backends based on the platform
set(TGUI_BACKEND_OPTIONS "")
set(TGUI_BACKEND_OPTIONS_DESC "")
if(TGUI_OS_WINDOWS OR TGUI_OS_LINUX OR TGUI_OS_MACOS OR TGUI_OS_ANDROID OR TGUI_OS_IOS)
    list(APPEND TGUI_BACKEND_OPTIONS SFML_GRAPHICS)
    string(APPEND TGUI_BACKEND_OPTIONS_DESC " - SFML_GRAPHICS: sfml-graphics\n")
endif()
if(TGUI_OS_WINDOWS OR TGUI_OS_LINUX OR TGUI_OS_MACOS)
    list(APPEND TGUI_BACKEND_OPTIONS SFML_OPENGL3)
    string(APPEND TGUI_BACKEND_OPTIONS_DESC " - SFML_OPENGL3: sfml-window + OpenGL + FreeType\n")
endif()
if(TGUI_OS_WINDOWS OR TGUI_OS_LINUX OR TGUI_OS_MACOS OR TGUI_OS_ANDROID OR TGUI_OS_IOS)
    list(APPEND TGUI_BACKEND_OPTIONS SDL_RENDERER)
    string(APPEND TGUI_BACKEND_OPTIONS_DESC " - SDL_RENDERER: SDL2 + SDL_ttf\n")
endif()
if(TGUI_OS_WINDOWS OR TGUI_OS_LINUX OR TGUI_OS_MACOS)
    list(APPEND TGUI_BACKEND_OPTIONS SDL_OPENGL3)
    string(APPEND TGUI_BACKEND_OPTIONS_DESC " - SDL_OPENGL3: SDL2 + OpenGL + FreeType\n")
endif()
if(TGUI_OS_LINUX)
    list(APPEND TGUI_BACKEND_OPTIONS SDL_GLES2)
    string(APPEND TGUI_BACKEND_OPTIONS_DESC " - SDL_GLES2: SDL2 + OpenGL ES + FreeType\n")
endif()
if(TGUI_OS_WINDOWS OR TGUI_OS_LINUX OR TGUI_OS_MACOS)
    list(APPEND TGUI_BACKEND_OPTIONS SDL_TTF_OPENGL3)
    string(APPEND TGUI_BACKEND_OPTIONS_DESC " - SDL_TTF_OPENGL3: SDL2 + OpenGL + SDL2_ttf\n")
endif()
if(TGUI_OS_LINUX OR TGUI_OS_ANDROID OR TGUI_OS_IOS)
    list(APPEND TGUI_BACKEND_OPTIONS SDL_TTF_GLES2)
    string(APPEND TGUI_BACKEND_OPTIONS_DESC " - SDL_TTF_GLES2: SDL2 + OpenGL ES + SDL2_ttf\n")
endif()
if(TGUI_OS_WINDOWS OR TGUI_OS_LINUX OR TGUI_OS_MACOS)
    list(APPEND TGUI_BACKEND_OPTIONS GLFW_OPENGL3)
    string(APPEND TGUI_BACKEND_OPTIONS_DESC " - GLFW_OPENGL3: GLFW + OpenGL + FreeType\n")
endif()
if(TGUI_OS_LINUX)
    list(APPEND TGUI_BACKEND_OPTIONS GLFW_GLES2)
    string(APPEND TGUI_BACKEND_OPTIONS_DESC " - GLFW_GLES2: GLFW + OpenGL ES + FreeType\n")
endif()

tgui_set_option(TGUI_BACKEND SFML_GRAPHICS STRING "Select a backend for rendering or select Custom to use multiple or no backends")
set_property(CACHE TGUI_BACKEND PROPERTY STRINGS Custom;${TGUI_BACKEND_OPTIONS})

if(NOT backend_provided)
    message(FATAL_ERROR
      " Please select a backend by setting the value of TGUI_BACKEND.\n"
      " \n"
      " The following backends are available:\n"
      ${TGUI_BACKEND_OPTIONS_DESC}
      " \n"
      " SFML_GRAPHICS is the default and has been preselected, press configure again to continue with this backend.\n"
      " Select 'Custom' if you wish to build TGUI with multiple or no backends.\n")
endif()

string(TOUPPER "${TGUI_BACKEND}" TGUI_BACKEND_UPPERCASE)

# Add an option to build TGUI as a c++20 module (when using CMake >= 3.25)
if(CMAKE_VERSION VERSION_GREATER_EQUAL 3.25)
    # We currently don't create the option as it is still too experimental,
    # it can still be used if the user manually specifies the option though.
    #option(TGUI_BUILD_AS_CXX_MODULE "ON to build TGUI as a c++20 module instead of using header files" OFF)
    #mark_as_advanced(TGUI_BUILD_AS_CXX_MODULE)
    if(TGUI_BUILD_AS_CXX_MODULE)
        if(TGUI_CXX_STANDARD LESS 20)
            message(FATAL_ERROR "TGUI_BUILD_AS_CXX_MODULE can only be turned on if TGUI_CXX_STANDARD is set to at least 20")
        endif()

        if (NOT CMAKE_EXPERIMENTAL_CXX_MODULE_CMAKE_API)
            if(CMAKE_VERSION VERSION_GREATER_EQUAL 3.27)
                set(CMAKE_EXPERIMENTAL_CXX_MODULE_CMAKE_API "aa1f7df0-828a-4fcd-9afc-2dc80491aca7") # CMake 3.27
            else()
                set(CMAKE_EXPERIMENTAL_CXX_MODULE_CMAKE_API "2182bf5c-ef0d-489a-91da-49dbc3090d2a") # CMake 3.26
                set(CMAKE_EXPERIMENTAL_CXX_MODULE_DYNDEP 1)
            endif()
        endif()
    endif()
endif()

# Define an option for choosing between static and dynamic C runtime
if(TGUI_OS_WINDOWS)
    option(TGUI_USE_STATIC_STD_LIBS "TRUE to statically link to the standard libraries, FALSE to use the standard libraries as DLLs. This is NOT the option that you need to build TGUI statically, for that you just need to set TGUI_SHARED_LIBS to FALSE." FALSE)
    mark_as_advanced(TGUI_USE_STATIC_STD_LIBS)

    # Building TGUI as a dynamic library but linking statically to the standard library is not valid
    if(TGUI_SHARED_LIBS AND TGUI_USE_STATIC_STD_LIBS)
        message(FATAL_ERROR "TGUI_SHARED_LIBS and TGUI_USE_STATIC_STD_LIBS cannot be used together, you can only link statically to the standard library when TGUI is also build statically")
    endif()
endif()

if(TGUI_OS_WINDOWS AND TGUI_COMPILER_MSVC AND (CMAKE_GENERATOR MATCHES "Visual Studio"))
    option(TGUI_USE_MULTI_PROCESSOR_COMPILATION "Use /MP to speed up compilation of the TGUI library by using all cores" ON)
endif()

# Apple-specific options
if(TGUI_OS_MACOS OR TGUI_OS_IOS)

    # Add an option to build framework instead of dylib (release only)
    option(TGUI_BUILD_FRAMEWORK "TRUE to build TGUI as a framework library (release only), FALSE to build according to TGUI_SHARED_LIBS" ${TGUI_SHARED_LIBS})

    if (TGUI_OS_IOS AND TGUI_SHARED_LIBS AND NOT TGUI_BUILD_FRAMEWORK)
        message(FATAL_ERROR "Naked dynamic libs not supported for iOS, build static or frameworks instead")
    endif()

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

# Android-specific options
if(TGUI_OS_ANDROID)

    if(CMAKE_ANDROID_API LESS 19)
        message(FATAL_ERROR "Android API level (${CMAKE_ANDROID_API}) must be equal or greater than 19.")
    endif()

    # We install libs in a subdirectory named after the ABI (e.g. lib/armeabi/libtgui.so)
    set(CMAKE_INSTALL_LIBDIR "${CMAKE_INSTALL_LIBDIR}/${CMAKE_ANDROID_ARCH_ABI}")
endif()

# Add an option to choose whether PDB debug symbols should be generated for Visual Studio
if(TGUI_COMPILER_MSVC)
    option(TGUI_GENERATE_PDB "True to generate PDB debug symbols, FALSE otherwise." TRUE)
endif()

# Set the path for the libraries
set(LIBRARY_OUTPUT_PATH "${PROJECT_BINARY_DIR}/lib")

# Define the macros to link TGUI to its dependencies
include(${PROJECT_SOURCE_DIR}/cmake/Dependencies.cmake)

# Set up the TGUI library
add_subdirectory(src)

# If system dependencies are used instead of the headers from the extlibs directory then locate the header files
if(TGUI_USE_SYSTEM_STB)
    find_path(TGUI_SYSTEM_STB_IMAGE_INCLUDE_DIR stb_image.h REQUIRED)
    target_include_directories(tgui SYSTEM PRIVATE "${TGUI_SYSTEM_STB_IMAGE_INCLUDE_DIR}")

    find_path(TGUI_SYSTEM_STB_IMAGE_WRITE_INCLUDE_DIR stb_image_write.h REQUIRED)
    target_include_directories(tgui SYSTEM PRIVATE "${TGUI_SYSTEM_STB_IMAGE_WRITE_INCLUDE_DIR}")
endif()
if(TGUI_USE_SYSTEM_NANOSVG)
    find_path(TGUI_SYSTEM_NANOSVG_INCLUDE_DIR nanosvg.h REQUIRED)
    target_include_directories(tgui SYSTEM PRIVATE "${TGUI_SYSTEM_NANOSVG_INCLUDE_DIR}")

    find_path(TGUI_SYSTEM_NANOSVG_RAST_INCLUDE_DIR nanosvgrast.h REQUIRED)
    target_include_directories(tgui SYSTEM PRIVATE "${TGUI_SYSTEM_NANOSVG_RAST_INCLUDE_DIR}")
endif()
if(TGUI_USE_SYSTEM_GLAD)
    find_package(glad CONFIG REQUIRED)
    target_link_libraries(tgui PRIVATE glad::glad)
endif()

# Create an interface to create a console app that uses TGUI (used to build the tests)
add_library(tgui-console-app-interface INTERFACE)
target_link_libraries(tgui-console-app-interface INTERFACE tgui)
if(TGUI_DEFAULT_BACKEND STREQUAL "SFML_GRAPHICS" OR TGUI_DEFAULT_BACKEND STREQUAL "SFML_OPENGL3")
    if(TGUI_OS_IOS)
        tgui_find_dependency_sfml(Main QUIET)
        if (${SFML_VERSION} VERSION_GREATER_EQUAL 3)
            target_link_libraries(tgui-console-app-interface INTERFACE SFML::Main)
        else()
            target_link_libraries(tgui-console-app-interface INTERFACE sfml-main)
        endif()
    endif()
elseif(TGUI_DEFAULT_BACKEND STREQUAL "SDL_RENDERER"
       OR TGUI_DEFAULT_BACKEND STREQUAL "SDL_OPENGL3" OR TGUI_DEFAULT_BACKEND STREQUAL "SDL_GLES2"
       OR TGUI_DEFAULT_BACKEND STREQUAL "SDL_TTF_OPENGL3" OR TGUI_DEFAULT_BACKEND STREQUAL "SDL_TTF_GLES2")
    tgui_find_dependency_sdl()
    if (NOT TGUI_USE_SDL3 AND TARGET SDL2::SDL2main) # This target is only required on Windows or iOS
        target_link_libraries(tgui-console-app-interface INTERFACE SDL2::SDL2main)
    endif()
elseif(TGUI_DEFAULT_BACKEND STREQUAL "GLFW_OPENGL3" OR TGUI_DEFAULT_BACKEND STREQUAL "GLFW_GLES2")
    # We don't have to do anything for console apps when using GLFW as it assumes a normal main function
endif()

# Create an interface to create a graphical app that uses TGUI (used to build the examples and Gui Builder)
add_library(tgui-gui-app-interface INTERFACE)
target_link_libraries(tgui-gui-app-interface INTERFACE tgui)
if(TGUI_DEFAULT_BACKEND STREQUAL "SFML_GRAPHICS" OR TGUI_DEFAULT_BACKEND STREQUAL "SFML_OPENGL3")
    if(TGUI_OS_WINDOWS OR TGUI_OS_IOS)
        tgui_find_dependency_sfml(Main QUIET)
        if (SFML_VERSION VERSION_GREATER_EQUAL 3)
            target_link_libraries(tgui-gui-app-interface INTERFACE SFML::Main)
        else()
            target_link_libraries(tgui-gui-app-interface INTERFACE sfml-main)
        endif()
    endif()
elseif(TGUI_DEFAULT_BACKEND STREQUAL "SDL_RENDERER"
       OR TGUI_DEFAULT_BACKEND STREQUAL "SDL_OPENGL3" OR TGUI_DEFAULT_BACKEND STREQUAL "SDL_GLES2"
       OR TGUI_DEFAULT_BACKEND STREQUAL "SDL_TTF_OPENGL3" OR TGUI_DEFAULT_BACKEND STREQUAL "SDL_TTF_GLES2")
    tgui_find_dependency_sdl()
    if (NOT TGUI_USE_SDL3 AND TARGET SDL2::SDL2main) # This target is only required on Windows or iOS
        target_link_libraries(tgui-gui-app-interface INTERFACE SDL2::SDL2main)
    endif()
elseif(TGUI_DEFAULT_BACKEND STREQUAL "GLFW_OPENGL3" OR TGUI_DEFAULT_BACKEND STREQUAL "GLFW_GLES2")
    if(TGUI_OS_WINDOWS)
        if(TGUI_COMPILER_MSVC OR TGUI_COMPILER_CLANG_CL)
            target_link_options(tgui-gui-app-interface INTERFACE "/ENTRY:mainCRTStartup" "/SUBSYSTEM:WINDOWS")
        elseif(TGUI_COMPILER_LLVM_CLANG OR TGUI_COMPILER_INTEL_ONEAPI)
            target_link_options(tgui-gui-app-interface INTERFACE "LINKER:/ENTRY:mainCRTStartup,/SUBSYSTEM:WINDOWS")
        else()
            target_link_options(tgui-gui-app-interface INTERFACE "-mwindows")
        endif()
    endif()
endif()

# Define the install directory for miscellaneous files
if(TGUI_OS_WINDOWS OR TGUI_OS_IOS)
    set(DEFAULT_INSTALL_MISC_DIR .)
elseif(TGUI_OS_ANDROID)
    set(DEFAULT_INSTALL_MISC_DIR ${CMAKE_ANDROID_NDK}/sources/third_party/tgui)
else()
    set(DEFAULT_INSTALL_MISC_DIR ${CMAKE_INSTALL_DATAROOTDIR}/tgui-${TGUI_VERSION_MAJOR})
endif()
tgui_set_option(TGUI_MISC_INSTALL_PREFIX "${DEFAULT_INSTALL_MISC_DIR}" PATH "Prefix installation path for miscellaneous files")
mark_as_advanced(TGUI_MISC_INSTALL_PREFIX)

# Optionally build the GUI Builder
if(NOT TGUI_OS_IOS AND NOT TGUI_OS_ANDROID)
    tgui_assign_bool(TGUI_DEFAULT_BUILD_GUI_BUILDER TGUI_DEFAULT_BACKEND)

    option(TGUI_BUILD_GUI_BUILDER "TRUE to build the GUI Builder" ${TGUI_DEFAULT_BUILD_GUI_BUILDER})
    if(TGUI_BUILD_GUI_BUILDER)
        add_subdirectory("${PROJECT_SOURCE_DIR}/gui-builder")
    endif()
endif()

# Optionally build the examples
if(NOT TGUI_OS_ANDROID)
    option(TGUI_BUILD_EXAMPLES "TRUE to build the TGUI examples, FALSE to ignore them" FALSE)
    if(TGUI_BUILD_EXAMPLES)
        if(TGUI_BUILD_AS_CXX_MODULE)
            message(FATAL_ERROR "Examples don't support c++20 modules. Turn off either TGUI_BUILD_AS_CXX_MODULE or TGUI_BUILD_EXAMPLES.")
        endif()

        add_subdirectory(examples)
    endif()
endif()

# Optionally build the tests
if(NOT TGUI_OS_IOS AND NOT TGUI_OS_ANDROID)
    option(TGUI_BUILD_TESTS "TRUE to build the TGUI tests" FALSE)
    if(TGUI_BUILD_TESTS)
        add_subdirectory(tests)
    endif()
endif()

# Optionally build the documentation
option(TGUI_BUILD_DOC "TRUE to generate the API documentation, FALSE to ignore it" FALSE)
if(TGUI_BUILD_DOC)
    add_subdirectory(doc)
endif()

# Install pkg-config files by default on Linux (and BSD)
tgui_assign_bool(TGUI_INSTALL_PKGCONFIG_DEFAULT TGUI_OS_LINUX)
option(TGUI_INSTALL_PKGCONFIG_FILES "TRUE to automatically install pkg-config files so other projects can find TGUI" ${TGUI_INSTALL_PKGCONFIG_DEFAULT})

if (TGUI_INSTALL_PKGCONFIG_FILES)
    tgui_set_option(TGUI_PKGCONFIG_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}/${TGUI_PKGCONFIG_DIR}" PATH "Install directory for TGUI's pkg-config .pc files")

    configure_file("cmake/pkgconfig/tgui.pc.in" "pkgconfig/tgui.pc" @ONLY)
    install(FILES "${CMAKE_CURRENT_BINARY_DIR}/pkgconfig/tgui.pc" DESTINATION "${TGUI_PKGCONFIG_INSTALL_PREFIX}")
endif()

# Install include files
if(NOT TGUI_BUILD_FRAMEWORK)
    install(DIRECTORY include/
            DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
            COMPONENT devel
            PATTERN "*.in" EXCLUDE)
endif()

# Install PDB file
if(TGUI_GENERATE_PDB)
    # When using a Visual Studio generator, the pdb files are located in Debug and RelWithDebInfo subfolders, but not when
    # using single-configuration generators such as NMake Makefiles.
    # When using Linker PDB files (i.e. TGUI_SHARED_LIBS=FALSE), CMake 3.13 introduced a generator expression that helps us.
    # Linker PDB files are installed next to the dll file (where VS searches them) while compiler PDB files are placed next to the lib file.
    if(TGUI_SHARED_LIBS)
        install (FILES $<TARGET_PDB_FILE:tgui> DESTINATION ${CMAKE_INSTALL_BINDIR} OPTIONAL COMPONENT devel)
    else()
        # TODO: Find a reliable way to know whether the file has a "-d" postfix instead of trying to install both. OPTIONAL is also required when config is Release.
        if (MSVC_IDE)
            install(FILES "${PROJECT_BINARY_DIR}/lib/\${CMAKE_INSTALL_CONFIG_NAME}/tgui-s.pdb" DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT devel OPTIONAL)
            install(FILES "${PROJECT_BINARY_DIR}/lib/\${CMAKE_INSTALL_CONFIG_NAME}/tgui-s-d.pdb" DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT devel OPTIONAL)
        else()
            install(FILES "${PROJECT_BINARY_DIR}/lib/tgui-s.pdb" DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT devel OPTIONAL)
            install(FILES "${PROJECT_BINARY_DIR}/lib/tgui-s-d.pdb" DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT devel OPTIONAL)
        endif()
    endif()
endif()

# Install miscellaneous files
install(FILES license.txt DESTINATION ${CMAKE_INSTALL_DOCDIR})
install(FILES README.md DESTINATION ${CMAKE_INSTALL_DOCDIR})
install(DIRECTORY themes DESTINATION "${TGUI_MISC_INSTALL_PREFIX}")
