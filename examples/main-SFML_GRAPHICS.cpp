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
#include <TGUI/Backend/SFML-Graphics.hpp>

bool runExample(tgui::BackendGui& gui);

int main()
{
#ifdef TGUI_SYSTEM_IOS
    sf::RenderWindow window(sf::VideoMode::getDesktopMode(), "TGUI example (SFML-Graphics)");
#elif SFML_VERSION_MAJOR >= 3
    sf::RenderWindow window(sf::VideoMode{{800, 600}}, "TGUI example (SFML-Graphics)");
#else
    sf::RenderWindow window({800, 600}, "TGUI example (SFML-Graphics)");
#endif

    tgui::Gui gui(window);
    if (!runExample(gui))
        return EXIT_FAILURE;

    gui.mainLoop(); // To use your own main loop, see https://tgui.eu/tutorials/latest-stable/backend-sfml-graphics/
    return EXIT_SUCCESS;
}
