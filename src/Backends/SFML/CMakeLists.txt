# Link SFML in the same way as TGUI, unless SFML_STATIC_LIBRARIES is manually specified
if(NOT DEFINED SFML_STATIC_LIBRARIES)
  if(TGUI_SHARED_LIBS)
    set(SFML_STATIC_LIBRARIES FALSE)
  else()
    set(SFML_STATIC_LIBRARIES TRUE)
  endif()
endif()

# Find sfml if not build in same project
if(NOT TARGET sfml-graphics)
    if(TGUI_OS_WINDOWS AND TGUI_COMPILER_MSVC) # Also look for the main component when using Visual Studio
        find_package(SFML 2 COMPONENTS main graphics window system)
    elseif(TGUI_OS_ANDROID)  # Search for SFML in the android NDK (if no other directory is specified)
        find_package(SFML 2 COMPONENTS graphics window system PATHS "${CMAKE_ANDROID_NDK}/sources/third_party/sfml/lib/${CMAKE_ANDROID_ARCH_ABI}/cmake/SFML")
    elseif(TGUI_OS_IOS)  # Use the find_host_package macro from the toolchain on iOS
        find_host_package(SFML 2 COMPONENTS main graphics window system)
    else()
        find_package(SFML 2 COMPONENTS graphics window system)
    endif()

    # find_package couldn't find SFML
    if(NOT SFML_FOUND)
        set(SFML_DIR "" CACHE PATH "Path to SFMLConfig.cmake")
        message(FATAL_ERROR "CMake couldn't find SFML.\nSet SFML_DIR to the directory containing SFMLConfig.cmake (usually something like SFML_ROOT/lib/cmake/SFML), or change TGUI_BACKEND to not use SFML.")
    endif()

    if (${SFML_VERSION} VERSION_LESS "2.5.0")
        message(FATAL_ERROR "TGUI requires SFML 2.5 or higher")
    endif()
endif()

# Link to SFML and set include and library search directories
target_link_libraries(tgui PRIVATE sfml-graphics)

# Add the backend source files to the library
target_sources(tgui PRIVATE
    Backends/SFML/BackendFontSFML.cpp
    Backends/SFML/BackendRenderTargetSFML.cpp
    Backends/SFML/BackendSFML.cpp
    Backends/SFML/BackendTextSFML.cpp
    Backends/SFML/BackendTextureSFML.cpp
    Backends/SFML/GuiSFML.cpp
)

add_library(tgui-sfml-interface INTERFACE)
target_link_libraries(tgui-sfml-interface INTERFACE sfml-graphics)
if(TGUI_OS_WINDOWS OR TGUI_OS_IOS)
    target_link_libraries(tgui-sfml-interface INTERFACE sfml-main)
endif()
