/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus's Graphical User Interface
// Copyright (C) 2012 Bruno Van de Velde (VDV_B@hotmail.com)
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

#ifndef _TGUI_INCLUDED_
#define _TGUI_INCLUDED_

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <iostream>

#include <fstream>
#include <string>
#include <sstream>

#include <queue>
#include <stack>

#include <SFML/Graphics.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    struct OBJECT;
    struct OBJECT_BORDERS;
    struct OBJECT_ANIMATION;
    struct Callback;

    struct Label;
    struct Picture;
    struct Button;
    struct Checkbox;
    struct RadioButton;
    struct EditBox;
    struct Slider;
    struct Scrollbar;
    struct Listbox;
    struct LoadingBar;
    struct ComboBox;
    struct TextBox;
    struct SpriteSheet;

    struct InfoFileParser;
    struct EventManager;
    struct TextureManager;

    struct Group;
    struct Panel;
    struct ChildWindow;
    struct Window;


    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    template <typename T>
    std::string to_string(T value)
    {
        std::ostringstream oss;
        oss << value;
        return oss.str();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    enum ObjectPhase
    {
        ObjectPhase_Hover = 1,
        ObjectPhase_MouseDown = 2,
        ObjectPhase_Focused = 4
    };


    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief A list of all objects in tgui.
    ///
    /// It is used to identify an object.
    /// However I am trying to remove the use of these types as much as possible because it limits to use of custom objects.
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    enum ObjectTypes
    {
        window = 1,
        picture,
        button,
        checkbox,
        radioButton,
        label,
        editBox,
        listbox,
        comboBox,
        slider,
        scrollbar,
        loadingBar,
        panel,
        textBox,
        spriteSheet,
        animatedPicture,
        childWindow,

        ObjectTypesCount
    };

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// The texture manager will not only load the textures, but will also make sure that the textures are only loaded once.
extern tgui::TextureManager     TGUI_TextureManager;

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <TGUI/Config.hpp>
#include <TGUI/Vectors.hpp>

#include <TGUI/Objects.hpp>
#include <TGUI/Label.hpp>
#include <TGUI/Picture.hpp>
#include <TGUI/Button.hpp>
#include <TGUI/Checkbox.hpp>
#include <TGUI/RadioButton.hpp>
#include <TGUI/EditBox.hpp>
#include <TGUI/Slider.hpp>
#include <TGUI/Scrollbar.hpp>
#include <TGUI/Listbox.hpp>
#include <TGUI/LoadingBar.hpp>
#include <TGUI/ComboBox.hpp>
#include <TGUI/TextBox.hpp>
#include <TGUI/SpriteSheet.hpp>
#include <TGUI/AnimatedPicture.hpp>

#include <TGUI/InfoFileParser.hpp>
#include <TGUI/EventManager.hpp>
#include <TGUI/TextureManager.hpp>

#include <TGUI/Group.hpp>
#include <TGUI/Panel.hpp>
#include <TGUI/ChildWindow.hpp>
#include <TGUI/Window.hpp>
#include <TGUI/Form.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define TGUI_OUTPUT(x)       sf::err() << x << std::endl;

#define TGUI_MINIMUM(x, y)   (x < y) ? x : y

#define TGUI_MAXIMUM(x, y)   (x > y) ? x : y

#define TGUI_UNUSED_PARAM(x) (void)x

#define TGUI_MAX_OBJECTS     10000


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*! \mainpage TGUI Documentation
 *
 * These pages contain the description of all usable functions and members in TGUI v0.5.\n
 * The private and protected ones are hidden, like a few other functions that you should not use.
 *
 * Tutorials can be found on the tutorial section of my page (http://tgui.weebly.com/tutorials.html).
 *
 * More information can be found on the TGUI home page (http://tgui.weebly.com).
 */

#endif //_TGUI_INCLUDED_

