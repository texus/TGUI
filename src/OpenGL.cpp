/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus' Graphical User Interface
// Copyright (C) 2012-2022 Bruno Van de Velde (vdv_b@tgui.eu)
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

#include <TGUI/Config.hpp>

#if TGUI_USE_GLES
    #define TGUI_GLAD_GLES2_IMPLEMENTATION
#else
    #define TGUI_GLAD_GL_IMPLEMENTATION
#endif

#include <TGUI/OpenGL.hpp>
#include <TGUI/String.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    namespace priv
    {
#if !defined(NDEBUG) && !defined(TGUI_NO_RUNTIME_WARNINGS)
        void checkAndLogOpenGlError(const char* file, unsigned int line, const char* expression)
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
                case GL_OUT_OF_MEMORY:      error = "GL_OUT_OF_MEMORY";     break;
                default:                    error = "Unknown error";        break;
            }

            std::string fileStr = file;
            TGUI_PRINT_WARNING("An internal OpenGL call failed in "
                               + fileStr.substr(fileStr.find_last_of("\\/") + 1)
                               + "(" + String(line).toStdString() + ")."
                               + "\nExpression:\n   " + expression + "\nError description:\n   " + error + "\n");
        }
#else
        void checkAndLogOpenGlError(const char*, unsigned int, const char*)
        {
        }
#endif
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
