list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/cmake_modules)

find_package(SDL2 REQUIRED)
find_package(SDL2_ttf REQUIRED)

# Link to SDL and set include and library search directories
target_link_libraries(tgui PRIVATE SDL2::Core SDL2::TTF)

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
target_link_libraries(tgui-sdl-interface INTERFACE SDL2::Core SDL2::TTF SDL2::Main)

# Also link to OpenGL or OpenGL ES
if(NOT TGUI_USE_GLES)
    find_package(OpenGL REQUIRED)
    target_link_libraries(tgui PRIVATE OpenGL::GL)
    target_link_libraries(tgui-sdl-interface INTERFACE OpenGL::GL)
else() # OpenGL ES was requested
    # On linux, GL ES 3 code is included in the GLESv2 library
    target_link_libraries(tgui PRIVATE -lEGL -lGLESv2)
    target_link_libraries(tgui-sdl-interface INTERFACE -lEGL -lGLESv2)
endif()
