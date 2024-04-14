/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus' Graphical User Interface
// Copyright (C) 2012-2024 Bruno Van de Velde (vdv_b@tgui.eu)
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

#include <TGUI/TGUI.hpp>
#include <TGUI/Backend/raylib.hpp>

bool runExample(tgui::BackendGui& gui);

// We don't put this code in main() to make sure that all TGUI resources are destroyed before destroying raylib
void run_application()
{
    tgui::Gui gui;
    runExample(gui);
    gui.mainLoop(); // To use your own main loop, see https://tgui.eu/tutorials/latest-stable/backend-raylib/#main-loop
}

int main()
{
    SetTraceLogLevel(LOG_WARNING);

    InitWindow(800, 600, "TGUI example (RAYLIB)");
    SetTargetFPS(30);

    run_application();

    CloseWindow();
}
