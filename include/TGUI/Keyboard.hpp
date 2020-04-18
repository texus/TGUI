/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus' Graphical User Interface
// Copyright (C) 2012-2020 Bruno Van de Velde (vdv_b@tgui.eu)
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


#ifndef TGUI_KEYBOARD_HPP
#define TGUI_KEYBOARD_HPP

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <TGUI/Config.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Window/Keyboard.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    namespace keyboard
    {
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        inline bool isShiftPressed()
        {
            return sf::Keyboard::isKeyPressed(sf::Keyboard::LShift) || sf::Keyboard::isKeyPressed(sf::Keyboard::RShift);
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        inline bool isMultiselectModifierPressed()
        {
#ifdef TGUI_SYSTEM_MACOS
            return sf::Keyboard::isKeyPressed(sf::Keyboard::LSystem) || sf::Keyboard::isKeyPressed(sf::Keyboard::RSystem);
#else
            return sf::Keyboard::isKeyPressed(sf::Keyboard::LControl) || sf::Keyboard::isKeyPressed(sf::Keyboard::RControl);
#endif
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        inline bool isKeyPressCopy(const sf::Event::KeyEvent& event)
        {
#ifdef TGUI_SYSTEM_MACOS
            return (event.code == sf::Keyboard::Key::C) && !event.control && !event.alt && !event.shift && event.system;
#else
            return (event.code == sf::Keyboard::Key::C) && event.control && !event.alt && !event.shift && !event.system;
#endif
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        inline bool isKeyPressCut(const sf::Event::KeyEvent& event)
        {
#ifdef TGUI_SYSTEM_MACOS
            return (event.code == sf::Keyboard::Key::X) && !event.control && !event.alt && !event.shift && event.system;
#else
            return (event.code == sf::Keyboard::Key::X) && event.control && !event.alt && !event.shift && !event.system;
#endif
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        inline bool isKeyPressPaste(const sf::Event::KeyEvent& event)
        {
#ifdef TGUI_SYSTEM_MACOS
            return (event.code == sf::Keyboard::Key::V) && !event.control && !event.alt && !event.shift && event.system;
#else
            return (event.code == sf::Keyboard::Key::V) && event.control && !event.alt && !event.shift && !event.system;
#endif
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        inline bool isKeyPressSelectAll(const sf::Event::KeyEvent& event)
        {
#ifdef TGUI_SYSTEM_MACOS
            return (event.code == sf::Keyboard::Key::A) && !event.control && !event.alt && !event.shift && event.system;
#else
            return (event.code == sf::Keyboard::Key::A) && event.control && !event.alt && !event.shift && !event.system;
#endif
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        inline bool isKeyPressMoveCaretLeft(const sf::Event::KeyEvent& event)
        {
            return (event.code == sf::Keyboard::Key::Left) && !event.control && !event.alt && !event.system;
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        inline bool isKeyPressMoveCaretRight(const sf::Event::KeyEvent& event)
        {
            return (event.code == sf::Keyboard::Key::Right) && !event.control && !event.alt && !event.system;
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        inline bool isKeyPressMoveCaretWordBegin(const sf::Event::KeyEvent& event)
        {
#ifdef TGUI_SYSTEM_MACOS
            return (event.code == sf::Keyboard::Key::Left) && !event.control && event.alt && !event.system;
#else
            return (event.code == sf::Keyboard::Key::Left) && event.control && !event.alt && !event.system;
#endif
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        inline bool isKeyPressMoveCaretWordEnd(const sf::Event::KeyEvent& event)
        {
#ifdef TGUI_SYSTEM_MACOS
            return (event.code == sf::Keyboard::Key::Right) && !event.control && event.alt && !event.system;
#else
            return (event.code == sf::Keyboard::Key::Right) && event.control && !event.alt && !event.system;
#endif
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        inline bool isKeyPressMoveCaretUp(const sf::Event::KeyEvent& event)
        {
#ifdef TGUI_SYSTEM_MACOS
            // Option+UpArrow should actually move to the beginning of the paragraph (or the previous one), but we don't support this
            return (event.code == sf::Keyboard::Key::Up) && !event.control && !event.system;
#else
            return (event.code == sf::Keyboard::Key::Up) && !event.alt && !event.system;
#endif
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        inline bool isKeyPressMoveCaretDown(const sf::Event::KeyEvent& event)
        {
#ifdef TGUI_SYSTEM_MACOS
            // Option+DownArrow should actually move to the end of the paragraph (or the next one), but we don't support this
            return (event.code == sf::Keyboard::Key::Down) && !event.control && !event.system;
#else
            return (event.code == sf::Keyboard::Key::Down) && !event.alt && !event.system;
#endif
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        inline bool isKeyPressMoveCaretLineStart(const sf::Event::KeyEvent& event)
        {
#ifdef TGUI_SYSTEM_MACOS
            if ((event.code == sf::Keyboard::Key::Left) && !event.control && !event.alt && event.system)
                return true;
#endif
            return (event.code == sf::Keyboard::Key::Home) && !event.control && !event.alt && !event.system;
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        inline bool isKeyPressMoveCaretLineEnd(const sf::Event::KeyEvent& event)
        {
#ifdef TGUI_SYSTEM_MACOS
            if ((event.code == sf::Keyboard::Key::Right) && !event.control && !event.alt && event.system)
                return true;
#endif
            return (event.code == sf::Keyboard::Key::End) && !event.control && !event.alt && !event.system;
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        inline bool isKeyPressMoveCaretDocumentBegin(const sf::Event::KeyEvent& event)
        {
#ifdef TGUI_SYSTEM_MACOS
            return ((event.code == sf::Keyboard::Key::Up) && !event.control && !event.alt && event.system)
                || ((event.code == sf::Keyboard::Key::Home) && !event.control && !event.alt && event.system);
#else
            return (event.code == sf::Keyboard::Key::Home) && event.control && !event.alt && !event.system;
#endif
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        inline bool isKeyPressMoveCaretDocumentEnd(const sf::Event::KeyEvent& event)
        {
#ifdef TGUI_SYSTEM_MACOS
            return ((event.code == sf::Keyboard::Key::Down) && !event.control && !event.alt && event.system)
                || ((event.code == sf::Keyboard::Key::End) && !event.control && !event.alt && event.system);
#else
            return (event.code == sf::Keyboard::Key::End) && event.control && !event.alt && !event.system;
#endif
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif // TGUI_KEYBOARD_HPP
