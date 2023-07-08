/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus' Graphical User Interface
// Copyright (C) 2012-2023 Bruno Van de Velde (vdv_b@tgui.eu)
//
// This software is provided 'as-is', without any express or implied warranty.
// In no event will the authors be held liable for any damages arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it freely,
// subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented;
//    you must not claim that you wrote the original software.
//    If you use this software in a product, an acknowledgment
//    in the product documentation would be appreciated but is not required.
//
// 2. Altered source versions must be plainly marked as such,
//    and must not be misrepresented as being the original software.
//
// 3. This notice may not be removed or altered from any source distribution.
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


#ifndef TGUI_OPENGL_HPP
#define TGUI_OPENGL_HPP

#include <TGUI/Config.hpp>

#if TGUI_BUILD_AS_CXX_MODULE && !defined(TGUI_BUILDING_OPENGL_MODULE)
    import tgui.opengl;
#endif

#if defined(__GNUC__)
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wold-style-cast"
#elif defined (_MSC_VER)
#   if defined(__clang__)
#       pragma clang diagnostic push
#       pragma clang diagnostic ignored "-Wold-style-cast"
#       pragma clang diagnostic ignored "-Wlanguage-extension-token"
#   endif
#endif

#if TGUI_USE_SYSTEM_GLAD
    #if TGUI_HAS_RENDERER_BACKEND_OPENGL3
        #include <glad/gl.h>

        #define tgui_gladLoadGL             gladLoadGL
        #define TGUI_GLAD_GL_VERSION_3_3    GLAD_GL_VERSION_3_3
        #define TGUI_GLAD_GL_VERSION_4_0    GLAD_GL_VERSION_4_0
        #define TGUI_GLAD_GL_VERSION_4_1    GLAD_GL_VERSION_4_1
        #define TGUI_GLAD_GL_VERSION_4_2    GLAD_GL_VERSION_4_2
        #define TGUI_GLAD_GL_VERSION_4_3    GLAD_GL_VERSION_4_3
        #define TGUI_GLAD_GL_VERSION_4_4    GLAD_GL_VERSION_4_4
        #define TGUI_GLAD_GL_VERSION_4_5    GLAD_GL_VERSION_4_5
        #define TGUI_GLAD_GL_VERSION_4_6    GLAD_GL_VERSION_4_6
    #endif

    #if TGUI_HAS_RENDERER_BACKEND_GLES2
        #include <glad/gles2.h>

        #define tgui_gladLoadGLES2          gladLoadGLES2
        #define TGUI_GLAD_GL_ES_VERSION_2_0 GLAD_GL_ES_VERSION_2_0
        #define TGUI_GLAD_GL_ES_VERSION_3_0 GLAD_GL_ES_VERSION_3_0
        #define TGUI_GLAD_GL_ES_VERSION_3_1 GLAD_GL_ES_VERSION_3_1
        #define TGUI_GLAD_GL_ES_VERSION_3_2 GLAD_GL_ES_VERSION_3_2
    #endif
#else
    // Include all functions from OpenGL 4.6 and OpenGL ES 3.2
    #include <TGUI/extlibs/glad/gl.h>
#endif

#if defined(__GNUC__)
#   pragma GCC diagnostic pop
#elif defined (_MSC_VER)
#   if defined(__clang__)
#       pragma clang diagnostic pop
#   endif
#endif

#if !TGUI_EXPERIMENTAL_USE_STD_MODULE
    #include <string>
#endif

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#if !defined(NDEBUG)
    #define TGUI_GL_CHECK(expr) do { expr; priv::checkAndLogErrorOpenGL(__FILE__, __LINE__, #expr); } while (false)
#else
    #define TGUI_GL_CHECK(expr) expr
#endif

#if !TGUI_BUILD_AS_CXX_MODULE || defined(TGUI_BUILDING_OPENGL_MODULE)
namespace tgui
{
    namespace priv
    {
#if TGUI_BUILD_AS_CXX_MODULE
        TGUI_MODULE_EXPORT
#else
        inline
#endif
#if !defined(NDEBUG) && !defined(TGUI_NO_RUNTIME_WARNINGS)
        void checkAndLogErrorOpenGL(const char* file, unsigned int line, const char* expression)
        {
            GLenum errorCode = glGetError();
            if (errorCode == GL_NO_ERROR)
                return;

            const char* error;
            switch (errorCode)
            {
            case GL_INVALID_ENUM:       error = "GL_INVALID_ENUM";      break;
            case GL_INVALID_VALUE:      error = "GL_INVALID_VALUE";     break;
            case GL_INVALID_OPERATION:  error = "GL_INVALID_OPERATION"; break;
            case GL_STACK_OVERFLOW:     error = "GL_STACK_OVERFLOW";    break;
            case GL_STACK_UNDERFLOW:    error = "GL_STACK_UNDERFLOW";   break;
            case GL_OUT_OF_MEMORY:      error = "GL_OUT_OF_MEMORY";     break;
            default:                    error = "Unknown error";        break;
            }

            std::string fileStr = file;
            TGUI_PRINT_WARNING("An internal OpenGL call failed in "
                + fileStr.substr(fileStr.find_last_of("\\/") + 1)
                + "(" + std::to_string(line) + ")."
                + "\nExpression:\n   " + expression + "\nError description:\n   " + error + "\n");
        }
#else
        void checkAndLogErrorOpenGL(const char*, unsigned int, const char*)
        {
        }
#endif
    }
}
#endif

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif // TGUI_OPENGL_HPP
