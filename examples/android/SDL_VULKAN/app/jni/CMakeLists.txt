cmake_minimum_required(VERSION 3.22)
project(tgui-example CXX)

set(TGUI_BACKEND SDL_VULKAN)
set(TGUI_CXX_STANDARD 20)

set(SDL3_BUILD_DIR SDL-build)

set(TGUI_ROOT_DIR "../../../../../")

# Download and build SDL and FreeType.
# FreeType is built without any of its optional dependencies, which TGUI doesn't need.
set(FT_DISABLE_ZLIB ON)
set(FT_DISABLE_BZIP2 ON)
set(FT_DISABLE_PNG ON)
set(FT_DISABLE_HARFBUZZ ON)
set(FT_DISABLE_BROTLI ON)

include(FetchContent)
FetchContent_Declare(
  SDL
  GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
  GIT_TAG        release-3.4.0
)
FetchContent_Declare(
  freetype
  GIT_REPOSITORY https://github.com/freetype/freetype.git
  GIT_TAG        VER-2-14-1
)
FetchContent_MakeAvailable(SDL freetype)

# TGUI searches for the Freetype::Freetype target, which the FreeType project only
# provides when it is installed, so we create it ourselves.
if(NOT TARGET Freetype::Freetype)
    add_library(Freetype::Freetype ALIAS freetype)
endif()

# Build TGUI
add_subdirectory("${TGUI_ROOT_DIR}" TGUI-build)

# Create the libmain.so library that contains the application's c++ code.
# The library needs to be called like this because SDLActivity will search for that name.
add_library(main SHARED example.cpp)
target_link_libraries(main PRIVATE TGUI::TGUI vulkan log)
