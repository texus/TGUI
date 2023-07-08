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
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wold-style-cast"
#elif defined (_MSC_VER)
#   if defined(__clang__)
#       pragma clang diagnostic push
#       pragma clang diagnostic ignored "-Wold-style-cast"
#       pragma clang diagnostic ignored "-Wlanguage-extension-token"
#   endif
#endif

#include <TGUI/Config.hpp>
#if TGUI_USE_SDL3
    #include <SDL3/SDL.h>
#else
    #include <SDL.h>

    // To keep code compatible with both SDL2 and SDL3, renamed SDL2 functions are mapped on their SDL3 equivalent
    #define SDL_DestroySurface SDL_FreeSurface
    #define SDL_DestroyCursor SDL_FreeCursor

    #define SDL_RenderClipEnabled SDL_RenderIsClipEnabled
    #define SDL_GetRenderClipRect SDL_RenderGetClipRect
    #define SDL_SetRenderClipRect SDL_RenderSetClipRect
    #define SDL_GetRenderViewport SDL_RenderGetViewport
    #define SDL_SetRenderViewport SDL_RenderSetViewport

    #define SDL_SCALEMODE_BEST SDL_ScaleModeBest
    #define SDL_SCALEMODE_LINEAR SDL_ScaleModeLinear
    #define SDL_SCALEMODE_NEAREST SDL_ScaleModeNearest

    #define SDL_EVENT_QUIT SDL_QUIT
    #define SDL_EVENT_TEXT_INPUT SDL_TEXTINPUT
    #define SDL_EVENT_KEY_DOWN SDL_KEYDOWN
    #define SDL_EVENT_KEY_UP SDL_KEYUP
    #define SDL_EVENT_MOUSE_BUTTON_DOWN SDL_MOUSEBUTTONDOWN
    #define SDL_EVENT_MOUSE_BUTTON_UP SDL_MOUSEBUTTONUP
    #define SDL_EVENT_MOUSE_MOTION SDL_MOUSEMOTION
    #define SDL_EVENT_MOUSE_WHEEL SDL_MOUSEWHEEL
    #define SDL_EVENT_FINGER_DOWN SDL_FINGERDOWN
    #define SDL_EVENT_FINGER_UP SDL_FINGERUP
    #define SDL_EVENT_FINGER_MOTION SDL_FINGERMOTION

    #define SDL_KMOD_NONE KMOD_NONE
    #define SDL_KMOD_ALT KMOD_ALT
    #define SDL_KMOD_GUI KMOD_GUI
    #define SDL_KMOD_CTRL KMOD_CTRL
    #define SDL_KMOD_LCTRL KMOD_LCTRL
    #define SDL_KMOD_RCTRL KMOD_RCTRL
    #define SDL_KMOD_SHIFT KMOD_SHIFT
    #define SDL_KMOD_LSHIFT KMOD_LSHIFT
    #define SDL_KMOD_RSHIFT KMOD_RSHIFT

    #define SDL_ENABLE_SYSWM_X11 SDL_VIDEO_DRIVER_X11
#endif

#if SDL_MAJOR_VERSION < 2
    #error TGUI does not support SDL 1.x
#endif

#if defined(__GNUC__)
    #pragma GCC diagnostic pop
#elif defined (_MSC_VER)
#   if defined(__clang__)
#       pragma clang diagnostic pop
#   endif
#endif
