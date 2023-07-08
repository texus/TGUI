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

#include "GuiBuilder.hpp"

// SDL has its own main function on some platforms but it has a define so that we can still call our version "main".
// So if we are using the SDL backend then we must include it in this file.
#if !TGUI_HAS_BACKEND_SFML_GRAPHICS && !TGUI_HAS_BACKEND_SFML_OPENGL3 \
 && (TGUI_HAS_BACKEND_SDL_RENDERER || TGUI_HAS_BACKEND_SDL_OPENGL3 || TGUI_HAS_BACKEND_SDL_GLES2 || TGUI_HAS_BACKEND_SDL_TTF_OPENGL3 || TGUI_HAS_BACKEND_SDL_TTF_GLES2)
    #include <TGUI/extlibs/IncludeSDL.hpp> // To avoid compiler warnings with including SDL_main.h
    #include <SDL_main.h>
#endif

#ifdef TGUI_SYSTEM_WINDOWS
    #include <TGUI/extlibs/IncludeWindows.hpp> // GetCommandLineW
    #include <shellapi.h> // CommandLineToArgvW
#endif

#ifdef TGUI_SYSTEM_WINDOWS
int main(int, char**) // We don't use argv on Windows
#else
// cppcheck-suppress[constParameter,unmatchedSuppression]
int main(int, char* argv[])
#endif
{
    try
    {
        // On Windows we can't rely on argv because it would contain '?' characters if the path contained non-ANSI characters.
        // There are other main function signatures on Windows that do support unicode, but this function might not be the
        // actual entry point of the program (e.g. when including SDL_main.h), so we can't use them.
        // Instead we just use GetCommandLineW and CommandLineToArgvW to access the argv parameter with unicode support.
#ifdef TGUI_SYSTEM_WINDOWS
        LPCWSTR commandLineStr = GetCommandLineW();

        int argcW;
        LPWSTR* argvW = CommandLineToArgvW(commandLineStr, &argcW);
        if (argvW == NULL)
        {
            std::cerr << "Failed to access command line arguments" << std::endl;
            return 1;
        }

        const tgui::String exePath = argvW[0];
        LocalFree(argvW);
#else
        const tgui::String exePath = argv[0];
#endif

        GuiBuilder builder(exePath);
        builder.mainLoop();
    }
    catch (const tgui::Exception& e)
    {
        std::cerr << "TGUI exception thrown: " << e.what() << std::endl;
        return 1;
    }
    catch (const std::exception& e)
    {
        std::cerr << "Exception thrown: " << e.what() << std::endl;
        return 1;
    }
    catch (...)
    {
        std::cerr << "Unknown exception thrown" << std::endl;
        return 1;
    }
    return 0;
}
