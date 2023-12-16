cmake_minimum_required(VERSION 3.22)
project(tgui-example CXX)

set(TGUI_BACKEND SDL_RENDERER)
set(TGUI_CXX_STANDARD 17)

set(SDL2_BUILD_DIR SDL-build)
set(SDL2_TTF_BUILD_DIR SDL_ttf-build)

set(TGUI_ROOT_DIR "../../../../../")

# Build SDL and SDL_ttf
set(SDL2TTF_SAMPLES OFF)  # Fix for SDL_ttf < 2.21
set(SDL2TTF_INSTALL OFF)  # Fix for SDL_ttf < 2.21
set(SDL2TTF_VENDORED ON)  # Fix for SDL_ttf < 2.21
add_subdirectory("${TGUI_ROOT_DIR}/../SDL2" ${SDL2_BUILD_DIR})
add_subdirectory("${TGUI_ROOT_DIR}/../SDL2_ttf" ${SDL2_TTF_BUILD_DIR})

# Build TGUI
add_subdirectory("${TGUI_ROOT_DIR}" TGUI-build)

# Create the libmain.so library that contains the application's c++ code.
# The library needs to be called like this because SDLActivity will search for that name.
add_library(main SHARED src/example.cpp)
target_link_libraries(main PRIVATE TGUI::TGUI)
