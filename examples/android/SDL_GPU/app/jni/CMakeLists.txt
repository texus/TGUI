cmake_minimum_required(VERSION 3.22)
project(tgui-example CXX)

set(TGUI_BACKEND SDL_GPU)
set(TGUI_CXX_STANDARD 20)

set(SDL3_BUILD_DIR SDL-build)
set(SDL3_TTF_BUILD_DIR SDL_ttf-build)

set(TGUI_ROOT_DIR "../../../../../")

# Build SDL and SDL_ttf
add_subdirectory("${TGUI_ROOT_DIR}/../SDL3" ${SDL3_BUILD_DIR})
add_subdirectory("${TGUI_ROOT_DIR}/../SDL3_ttf" ${SDL3_TTF_BUILD_DIR})

# Build TGUI
add_subdirectory("${TGUI_ROOT_DIR}" TGUI-build)

# Create the libmain.so library that contains the application's c++ code.
# The library needs to be called like this because SDLActivity will search for that name.
add_library(main SHARED example.cpp)
target_link_libraries(main PRIVATE TGUI::TGUI log)
