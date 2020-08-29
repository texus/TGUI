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
