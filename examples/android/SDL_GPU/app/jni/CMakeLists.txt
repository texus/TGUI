cmake_minimum_required(VERSION 3.22)
project(tgui-example CXX)

set(TGUI_BACKEND SDL_GPU)
set(TGUI_CXX_STANDARD 20)

set(SDL3_BUILD_DIR SDL-build)
set(SDL3_TTF_BUILD_DIR SDL_ttf-build)

set(TGUI_ROOT_DIR "../../../../../")

# Download and build SDL and SDL_ttf
include(FetchContent)
FetchContent_Declare(
  SDL
  GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
  GIT_TAG        release-3.4.0
)
FetchContent_Declare(
  SDL_ttf
  GIT_REPOSITORY https://github.com/libsdl-org/SDL_ttf.git
  GIT_TAG        release-3.2.2
)
FetchContent_MakeAvailable(SDL SDL_ttf)

# Build TGUI
add_subdirectory("${TGUI_ROOT_DIR}" TGUI-build)

# Create the libmain.so library that contains the application's c++ code.
# The library needs to be called like this because SDLActivity will search for that name.
add_library(main SHARED example.cpp)
target_link_libraries(main PRIVATE TGUI::TGUI log)
