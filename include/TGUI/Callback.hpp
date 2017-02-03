/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus's Graphical User Interface
// Copyright (C) 2012-2017 Bruno Van de Velde (vdv_b@tgui.eu)
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


#ifndef TGUI_CALLBACK_HPP
#define TGUI_CALLBACK_HPP

#include <map>
#include <list>
#include <functional>

#include <TGUI/Global.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    class Widget;

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Struct that tells more about the callback that happened.
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    struct TGUI_API Callback
    {
        // The callback id that was passed to the widget. It is used to identify from what widget the callback came from.
        unsigned int id = 0;

        // How did the callback occur?
        std::string trigger;

        // Pointer to the widget
        Widget* widget = nullptr;

        // The type of the widget
        std::string widgetType = "Unknown";

        // When the mouse has something to do with the callback then this data will be filled
        sf::Vector2i mouse;

        // This text is only used by some widgets, but it can be set together with some other member
        sf::String text;

        // Used to identify an item
        sf::String itemId;

        // Only one of these things can be filled at a given time
        bool         checked = false;
        int          value = 0;
        unsigned int index = 0;
        sf::Vector2f value2d;
        sf::Vector2f position;
        sf::Vector2f size;
    };

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif // TGUI_CALLBACK_HPP

