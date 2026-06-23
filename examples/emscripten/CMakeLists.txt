cmake_minimum_required(VERSION 3.30)
project(tgui_emscripten)
include(FetchContent)
set(CMAKE_CXX_STANDARD 26)
set(FETCHCONTENT_QUIET OFF)
set(CMAKE_EXECUTABLE_SUFFIX ".html")

# Set debug mode for testing so that we can see the exception messages if something fails
set(CMAKE_BUILD_TYPE Debug)

# Select which backend(s) to use.
# We set the value to "Custom" here so that this example can build multiple backends at once.
set(TGUI_BACKEND Custom)
set(TGUI_HAS_BACKEND_SDL_RENDERER ON)
set(TGUI_HAS_BACKEND_SDL_TTF_GLES2 ON)
set(TGUI_HAS_BACKEND_SDL_GLES2 ON)
set(TGUI_HAS_BACKEND_GLFW_GLES2 ON)
set(TGUI_HAS_BACKEND_RAYLIB ON)

# Download and build SDL3 when it is used as backend
if(TGUI_HAS_BACKEND_SDL_RENDERER OR TGUI_HAS_BACKEND_SDL_TTF_GLES2 OR TGUI_HAS_BACKEND_SDL_GLES2)
    FetchContent_Declare(SDL3 GIT_REPOSITORY https://github.com/libsdl-org/SDL.git GIT_TAG "release-3.4.2"  GIT_PROGRESS ON)
    FetchContent_MakeAvailable(SDL3)
endif()

# Download and build SDL3_ttf when it is used as backend
if(TGUI_HAS_BACKEND_SDL_RENDERER OR TGUI_HAS_BACKEND_SDL_TTF_GLES2)
    set(SDLTTF_VENDORED ON)
    FetchContent_Declare(SDL3_ttf GIT_REPOSITORY https://github.com/libsdl-org/SDL_ttf.git GIT_TAG "release-3.2.2"  GIT_PROGRESS ON)
    FetchContent_MakeAvailable(SDL3_ttf)
endif()

# Download and build raylib when it is used as backend
if(TGUI_HAS_BACKEND_RAYLIB)
    set(PLATFORM Web CACHE STRING "Platform" FORCE)
    FetchContent_Declare(raylib GIT_REPOSITORY https://github.com/raysan5/raylib.git GIT_TAG "5.5"  GIT_PROGRESS ON)
    FetchContent_MakeAvailable(raylib)
endif()

# Download and build TGUI
FetchContent_Declare(TGUI GIT_REPOSITORY https://github.com/texus/TGUI.git GIT_TAG "origin/1.x"  GIT_PROGRESS ON)
FetchContent_MakeAvailable(TGUI)

# Copy the "data" directory into the build folder, so that you can run the .html there.
# With the --preload-file option in target_link_options later on, we will make the data folder
# in the build directory available for the executable.
if(NOT EXISTS "${CMAKE_CURRENT_BINARY_DIR}/data")
    add_custom_target(copy_data ALL
        COMMAND ${CMAKE_COMMAND} -E copy_directory
                "${CMAKE_CURRENT_SOURCE_DIR}/data"
                "${CMAKE_CURRENT_BINARY_DIR}/data"
    )
endif()

# Build our example program (depending on the selected backends)
if(TGUI_HAS_BACKEND_SDL_RENDERER)
    add_executable(main-SDL_RENDERER main-SDL_RENDERER.cpp)
    target_link_libraries(main-SDL_RENDERER PRIVATE TGUI::TGUI)
    target_link_options(main-SDL_RENDERER PRIVATE -fexceptions --preload-file "data/")
    target_compile_options(main-SDL_RENDERER PRIVATE -fexceptions)
endif()
if(TGUI_HAS_BACKEND_SDL_TTF_GLES2)
    add_executable(main-SDL_TTF_GLES2 main-SDL_TTF_GLES2.cpp)
    target_link_libraries(main-SDL_TTF_GLES2 PRIVATE TGUI::TGUI)
    target_link_options(main-SDL_TTF_GLES2 PRIVATE -fexceptions --preload-file "data/")
    target_compile_options(main-SDL_TTF_GLES2 PRIVATE -fexceptions)
endif()
if(TGUI_HAS_BACKEND_SDL_GLES2)
    add_executable(main-SDL_GLES2 main-SDL_GLES2.cpp)
    target_link_libraries(main-SDL_GLES2 PRIVATE TGUI::TGUI)
    target_link_options(main-SDL_GLES2 PRIVATE -fexceptions --preload-file "data/")
    target_compile_options(main-SDL_GLES2 PRIVATE -fexceptions)
endif()
if(TGUI_HAS_BACKEND_GLFW_GLES2)
    add_executable(main-GLFW_GLES2 main-GLFW_GLES2.cpp)
    target_link_libraries(main-GLFW_GLES2 PRIVATE TGUI::TGUI)
    target_link_options(main-GLFW_GLES2 PRIVATE -fexceptions --preload-file "data/")
    target_compile_options(main-GLFW_GLES2 PRIVATE -fexceptions)
    target_link_options(main-GLFW_GLES2 PRIVATE "-sUSE_GLFW=3")
endif()
if(TGUI_HAS_BACKEND_RAYLIB)
    add_executable(main-RAYLIB main-RAYLIB.cpp)
    target_link_libraries(main-RAYLIB PRIVATE TGUI::TGUI)
    target_link_options(main-RAYLIB PRIVATE -fexceptions --preload-file "data/")
    target_compile_options(main-RAYLIB PRIVATE -fexceptions)
endif()
