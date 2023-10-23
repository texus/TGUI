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

#if defined(__GNUC__)
#   pragma GCC diagnostic push
#   pragma GCC diagnostic ignored "-Wold-style-cast"
#   pragma GCC diagnostic ignored "-Wsign-compare"
#   pragma GCC diagnostic ignored "-Wsign-conversion"
#   pragma GCC diagnostic ignored "-Wdouble-promotion"
#   pragma GCC diagnostic ignored "-Wunused-function"
#   pragma GCC diagnostic ignored "-Wcast-align"
#   pragma GCC diagnostic ignored "-Wimplicit-fallthrough"
#   if defined(__clang__)
#       if __has_warning("-Wunused-but-set-variable")
#           pragma GCC diagnostic ignored "-Wunused-but-set-variable"
#       endif
#   else
#       pragma GCC diagnostic ignored "-Wunused-but-set-variable"
#       pragma GCC diagnostic ignored "-Wmaybe-uninitialized"
#   endif
#elif defined (_MSC_VER)
#   if defined(__clang__)
#       pragma clang diagnostic push
#       pragma clang diagnostic ignored "-Wold-style-cast"
#       pragma clang diagnostic ignored "-Wsign-compare"
#       pragma clang diagnostic ignored "-Wsign-conversion"
#       pragma clang diagnostic ignored "-Wdouble-promotion"
#       pragma clang diagnostic ignored "-Wunused-function"
#       pragma clang diagnostic ignored "-Wcast-align"
#       pragma clang diagnostic ignored "-Wimplicit-fallthrough"
#   else
#       pragma warning(push)
#       pragma warning(disable: 4244) // conversion, possible loss of data
#       pragma warning(disable: 4505) // Unreferenced local function
#   endif
#endif

#define STBI_NO_STDIO
#define STBI_WINDOWS_UTF8

#if TGUI_USE_SYSTEM_STB
#   include <stb_image.h>
#else
#   define STB_IMAGE_STATIC
#   define STB_IMAGE_IMPLEMENTATION
#   include <TGUI/extlibs/stb/stb_image.h>
#endif

#if defined(__GNUC__)
#   pragma GCC diagnostic pop
#elif defined (_MSC_VER)
#   if defined(__clang__)
#       pragma clang diagnostic pop
#   else
#       pragma warning(pop)
#   endif
#endif
