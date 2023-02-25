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

module;

#define TGUI_MODULE_EXPORT export
#include <TGUI/Config.hpp>

#if !TGUI_EXPERIMENTAL_USE_STD_MODULE
    #include <string>
#endif

#ifdef _MSC_VER
    #pragma warning(disable: 5244) // Ignore warnings about '#include' still being used below
#endif

#if !TGUI_USE_SYSTEM_GLAD
    #define TGUI_GLAD_GL_IMPLEMENTATION
#endif

#define TGUI_BUILDING_OPENGL_MODULE

export module tgui.opengl;

#include <TGUI/Backend/Renderer/OpenGL.hpp>
