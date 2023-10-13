/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus' Graphical User Interface
// Copyright (C) 2012-2019 Bruno Van de Velde (vdv_b@tgui.eu)
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


#ifndef TGUI_EVENT_HPP
#define TGUI_EVENT_HPP

#include <TGUI/Config.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

TGUI_MODULE_EXPORT namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    // Based on sf::Event from SFML
    struct Event
    {
        // Based on sf::KeyBoard::Key from SFML
        /// Keyboard key codes
        enum class KeyboardKey
        {
            Unknown = -1, //!< Unhandled key
            A = 0,        //!< The A key
            B,            //!< The B key
            C,            //!< The C key
            D,            //!< The D key
            E,            //!< The E key
            F,            //!< The F key
            G,            //!< The G key
            H,            //!< The H key
            I,            //!< The I key
            J,            //!< The J key
            K,            //!< The K key
            L,            //!< The L key
            M,            //!< The M key
            N,            //!< The N key
            O,            //!< The O key
            P,            //!< The P key
            Q,            //!< The Q key
            R,            //!< The R key
            S,            //!< The S key
            T,            //!< The T key
            U,            //!< The U key
            V,            //!< The V key
            W,            //!< The W key
            X,            //!< The X key
            Y,            //!< The Y key
            Z,            //!< The Z key
            Num0,         //!< The 0 key
            Num1,         //!< The 1 key
            Num2,         //!< The 2 key
            Num3,         //!< The 3 key
            Num4,         //!< The 4 key
            Num5,         //!< The 5 key
            Num6,         //!< The 6 key
            Num7,         //!< The 7 key
            Num8,         //!< The 8 key
            Num9,         //!< The 9 key
            Escape,       //!< The Escape key
            LControl,     //!< The left Control key
            LShift,       //!< The left Shift key
            LAlt,         //!< The left Alt key
            LSystem,      //!< The left OS specific key (windows key / super key / command key)
            RControl,     //!< The right Control key
            RShift,       //!< The right Shift key
            RAlt,         //!< The right Alt key
            RSystem,      //!< The right OS specific key (windows key / super key / command key)
            Menu,         //!< The Menu key
            LBracket,     //!< The [ key
            RBracket,     //!< The ] key
            Semicolon,    //!< The ; key
            Comma,        //!< The , key
            Period,       //!< The . key
            Quote,        //!< The ' key
            Slash,        //!< The / key
            Backslash,    //!< The \ key
            Tilde,        //!< The ~ key
            Equal,        //!< The = key
            Minus,        //!< The - key (hyphen)
            Space,        //!< The Space key
            Enter,        //!< The Enter/Return keys
            Backspace,    //!< The Backspace key
            Tab,          //!< The Tabulation key
            PageUp,       //!< The Page up key
            PageDown,     //!< The Page down key
            End,          //!< The End key
            Home,         //!< The Home key
            Insert,       //!< The Insert key
            Delete,       //!< The Delete key
            Add,          //!< The + key
            Subtract,     //!< The - key (minus from numpad)
            Multiply,     //!< The * key
            Divide,       //!< The / key
            Left,         //!< Left arrow
            Right,        //!< Right arrow
            Up,           //!< Up arrow
            Down,         //!< Down arrow
            Numpad0,      //!< The numpad 0 key
            Numpad1,      //!< The numpad 1 key
            Numpad2,      //!< The numpad 2 key
            Numpad3,      //!< The numpad 3 key
            Numpad4,      //!< The numpad 4 key
            Numpad5,      //!< The numpad 5 key
            Numpad6,      //!< The numpad 6 key
            Numpad7,      //!< The numpad 7 key
            Numpad8,      //!< The numpad 8 key
            Numpad9,      //!< The numpad 9 key
            F1,           //!< The F1 key
            F2,           //!< The F2 key
            F3,           //!< The F3 key
            F4,           //!< The F4 key
            F5,           //!< The F5 key
            F6,           //!< The F6 key
            F7,           //!< The F7 key
            F8,           //!< The F8 key
            F9,           //!< The F9 key
            F10,          //!< The F10 key
            F11,          //!< The F11 key
            F12,          //!< The F12 key
            F13,          //!< The F13 key
            F14,          //!< The F14 key
            F15,          //!< The F15 key
            Pause         //!< The Pause key
        };

        /// Mouse buttons
        enum class MouseButton
        {
            Left,   //!< The left mouse button
            Right,  //!< The right mouse button
            Middle  //!< The middle (wheel) mouse button
        };

        /// Modifiers keys
        enum class KeyModifier
        {
            System,
            Control,
            Shift,
            Alt
        };

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief KeyPressed event parameters
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        struct KeyEvent
        {
            KeyboardKey code;    //!< Code of the key that has been pressed
            bool        alt;     //!< Is the Alt key pressed?
            bool        control; //!< Is the Control key pressed?
            bool        shift;   //!< Is the Shift key pressed?
            bool        system;  //!< Is the System key pressed?
        };

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief TextEntered event parameters
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        struct TextEvent
        {
            char32_t unicode; //!< UTF-32 Unicode value of the character
        };

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief MouseMoved event parameters
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        struct MouseMoveEvent
        {
            int x; //!< X position of the mouse pointer, relative to the left of the owner window
            int y; //!< Y position of the mouse pointer, relative to the top of the owner window
        };

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief MouseButtonPressed/MouseButtonReleased events parameters
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        struct MouseButtonEvent
        {
            MouseButton button; //!< Code of the button that has been pressed
            int         x;      //!< X position of the mouse pointer, relative to the left of the owner window
            int         y;      //!< Y position of the mouse pointer, relative to the top of the owner window
        };

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief MouseWheelScrolled events parameters
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        struct MouseWheelEvent
        {
            float delta; //!< Wheel offset (positive is up, negative is down). High-precision mice may use non-integral offsets.
            int   x;     //!< X position of the mouse pointer, relative to the left of the owner window
            int   y;     //!< Y position of the mouse pointer, relative to the top of the owner window
        };

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Resized events parameters
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        struct SizeEvent
        {
            unsigned int width;  //!< New width, in pixels
            unsigned int height; //!< New height, in pixels
        };

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Enumeration of the different types of events
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        enum class Type
        {
            LostFocus,              //!< The window lost the focus (no data)
            GainedFocus,            //!< The window gained the focus (no data)
            TextEntered,            //!< A character was entered (data in event.text)
            KeyPressed,             //!< A key was pressed (data in event.key)
            MouseWheelScrolled,     //!< The mouse wheel was scrolled (data in event.mouseWheel)
            MouseButtonPressed,     //!< A mouse button was pressed (data in event.mouseButton)
            MouseButtonReleased,    //!< A mouse button was released (data in event.mouseButton)
            MouseMoved,             //!< The mouse cursor moved (data in event.mouseMove)
            MouseEntered,           //!< The mouse cursor entered the content area of the window (no data)
            MouseLeft,              //!< The mouse cursor left the content area of the window (no data)
            Resized,                //!< The window was resized (data in event.size)
            Closed                  //!< The window was closed (no data)
        };

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        Type type; //!< Type of the event

        union
        {
            KeyEvent              key;               //!< Key event parameters (Event::KeyPressed)
            TextEvent             text;              //!< Text event parameters (Event::TextEntered)
            MouseMoveEvent        mouseMove;         //!< Mouse move event parameters (Event::MouseMoved)
            MouseButtonEvent      mouseButton;       //!< Mouse button event parameters (Event::MouseButtonPressed, Event::MouseButtonReleased)
            MouseWheelEvent       mouseWheel;        //!< Mouse wheel event parameters (Event::MouseWheelScrolled)
            SizeEvent             size;              //!< Size event parameters (Event::Resized)
        };
    };

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif // TGUI_EVENT_HPP
