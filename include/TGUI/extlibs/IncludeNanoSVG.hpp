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
#   pragma GCC diagnostic ignored "-Wsign-conversion"
#   pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#   pragma GCC diagnostic ignored "-Wzero-as-null-pointer-constant"
#   pragma GCC diagnostic ignored "-Wdouble-promotion"
#   pragma GCC diagnostic ignored "-Wold-style-cast"
#   pragma GCC diagnostic ignored "-Wfloat-conversion"
#   pragma GCC diagnostic ignored "-Wfloat-equal"
#   pragma GCC diagnostic ignored "-Wconversion"
#   pragma GCC diagnostic ignored "-Wcast-align"
#   pragma GCC diagnostic ignored "-Wshadow"
#elif defined (_MSC_VER)
#   if defined(__clang__)
#       pragma clang diagnostic push
#       pragma clang diagnostic ignored "-Wsign-conversion"
#       pragma clang diagnostic ignored "-Wdeprecated-declarations"
#       pragma clang diagnostic ignored "-Wunreachable-code-return"
#       pragma clang diagnostic ignored "-Wzero-as-null-pointer-constant"
#       pragma clang diagnostic ignored "-Wdouble-promotion"
#       pragma clang diagnostic ignored "-Wold-style-cast"
#       pragma clang diagnostic ignored "-Wfloat-conversion"
#       pragma clang diagnostic ignored "-Wfloat-equal"
#       pragma clang diagnostic ignored "-Wconversion"
#       pragma clang diagnostic ignored "-Wcast-align"
#       pragma clang diagnostic ignored "-Wshadow"
#   else
#       pragma warning(push)
#       pragma warning(disable: 4244) // conversion, possible loss of data
#       pragma warning(disable: 4456) // declaration hides previous local declaration
#       pragma warning(disable: 4702) // unreachable code
#   endif
#endif

#if TGUI_USE_SYSTEM_NANOSVG
#   include <nanosvg.h>
#   include <nanosvgrast.h>
#else
    #define NANOSVG_IMPLEMENTATION
    #define NANOSVGRAST_IMPLEMENTATION

    // Include the stdlib headers used by nanosvg to allow wrapping the includes in a namespace
    #include <string.h>
    #include <stdlib.h>
    #include <stdio.h>
    #include <math.h>

    namespace tgui
    {
        namespace priv
        {
            #include <TGUI/extlibs/nanosvg/nanosvg.h>
            #include <TGUI/extlibs/nanosvg/nanosvgrast.h>
        }
    }

    // We place the functions in a namespace to prevent potential issues if the TGUI user also uses nanosvg in their own project.
    // We don't use an anonymous namespace as it leads to a warning from some compilers because SvgImageData (which has to be
    // forward declared in the header file and can't be anonymous) would have public members that have static linkage.
    using tgui::priv::nsvgParseFromFile;
    using tgui::priv::nsvgDelete;
    using tgui::priv::nsvgCreateRasterizer;
    using tgui::priv::nsvgRasterizeXY;
    using tgui::priv::nsvgDeleteRasterizer;
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
