cmake_minimum_required(VERSION 3.21)
project(tgui-example)

set(TGUI_BACKEND SDL_TTF_GLES2)

set(SDL2_BUILD_DIR SDL-build)
set(SDL2_TTF_BUILD_DIR SDL_ttf-build)

set(TGUI_ROOT_DIR "../../../../../")

# Build SDL and SDL_ttf
add_subdirectory("${TGUI_ROOT_DIR}/../SDL" ${SDL2_BUILD_DIR})
add_subdirectory("${TGUI_ROOT_DIR}/../SDL_ttf" ${SDL2_TTF_BUILD_DIR})

# Make sure TGUI can find SDL_ttf
add_library(SDL2::SDL2_ttf ALIAS SDL2_ttf)

# Workaround issue of building SDL and TGUI together
set(TGUI_SKIP_EXPORT ON)

# Build TGUI
add_subdirectory("${TGUI_ROOT_DIR}" TGUI-build)

# Create the libmain.so library that contains the application's c++ code
add_library(main SHARED)
target_sources(main PRIVATE src/example.cpp)
target_link_libraries(main TGUI::TGUI EGL GLESv2)
