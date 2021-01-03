list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/cmake_modules)

find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)
find_package(SDL2_ttf REQUIRED)

# Link to SDL and set include and library search directories
target_link_libraries(tgui PRIVATE SDL2::Core SDL2::Image SDL2::TTF)

# Add the backend source files to the library
target_sources(tgui PRIVATE
    Backends/SDL/BackendFontSDL.cpp
    Backends/SDL/BackendRenderTargetSDL.cpp
    Backends/SDL/BackendSDL.cpp
    Backends/SDL/BackendTextSDL.cpp
    Backends/SDL/BackendTextureSDL.cpp
    Backends/SDL/FontCacheSDL.cpp
    Backends/SDL/GuiSDL.cpp
    OpenGL.cpp
)

add_library(tgui-sdl-interface INTERFACE)
target_link_libraries(tgui-sdl-interface INTERFACE SDL2::Core SDL2::Image SDL2::TTF)

if(TGUI_BUILD_GUI_BUILDER OR TGUI_BUILD_TESTS OR TGUI_BUILD_EXAMPLES)
    find_package(OpenGL REQUIRED)
    add_library(tgui-sdl-main-interface INTERFACE)
    target_link_libraries(tgui-sdl-main-interface INTERFACE SDL2::Main OpenGL::OpenGL)
endif()
