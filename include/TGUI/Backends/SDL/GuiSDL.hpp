#ifdef TGUI_NEXT
    #error This file was split and moved to TGUI/Backend/SDL-TTF-OpenGL3.hpp and TGUI/Backend/SDL-TTF-GLES2.hpp
#else
    #include <TGUI/Config.hpp>

    #if TGUI_USE_GLES
        #pragma message("Warning: This file was deprecated and moved to TGUI/Backend/SDL-TTF-GLES2.hpp")
        #include <TGUI/Backend/SDL-TTF-GLES2.hpp>
    #else
        #pragma message("Warning: This file was deprecated and moved to TGUI/Backend/SDL-TTF-OpenGL3.hpp")
        #include <TGUI/Backend/SDL-TTF-OpenGL3.hpp>
    #endif
#endif
