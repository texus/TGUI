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


#ifndef TGUI_KEYBOARD_HPP
#define TGUI_KEYBOARD_HPP

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <TGUI/Config.hpp>
#include <TGUI/Event.hpp>
#include <TGUI/Container.hpp>
#include <TGUI/Widgets/ScrollablePanel.hpp>
#include <TGUI/Backend/Window/Backend.hpp>
#include <TGUI/Backend/Window/BackendGui.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    namespace keyboard
    {
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef TGUI_REMOVE_DEPRECATED_CODE
        TGUI_DEPRECATED("Use BackendGui::startTextInput instead") inline void openVirtualKeyboard(const Widget* requestingWidget, FloatRect inputRect)
        {
            const Widget* widget = requestingWidget;
            while (widget)
            {
                const bool defaultOrigin = (widget->getOrigin().x == 0) && (widget->getOrigin().y == 0);
                const bool scaledOrRotated = (widget->getScale().x != 1) || (widget->getScale().y != 1) || (widget->getRotation() != 0);
                if (defaultOrigin && !scaledOrRotated)
                    inputRect.setPosition(inputRect.getPosition() + widget->getPosition());
                else
                {
                    const Vector2f origin{widget->getOrigin().x * widget->getSize().x, widget->getOrigin().y * widget->getSize().y};
                    if (!scaledOrRotated)
                        inputRect.setPosition(inputRect.getPosition() + widget->getPosition() - origin);
                    else
                    {
                        const Vector2f rotOrigin{widget->getRotationOrigin().x * widget->getSize().x, widget->getRotationOrigin().y * widget->getSize().y};
                        const Vector2f scaleOrigin{widget->getScaleOrigin().x * widget->getSize().x, widget->getScaleOrigin().y * widget->getSize().y};

                        Transform transform;
                        transform.translate(widget->getPosition() - origin);
                        transform.rotate(widget->getRotation(), rotOrigin);
                        transform.scale(widget->getScale(), scaleOrigin);
                        inputRect = transform.transformRect(inputRect);
                    }
                }

                const Container* parent = widget->getParent();
                if (parent)
                {
                    inputRect.setPosition(inputRect.getPosition() + parent->getChildWidgetsOffset());

                    const ScrollablePanel* panel = dynamic_cast<const ScrollablePanel*>(parent);
                    if (panel)
                        inputRect.setPosition(inputRect.getPosition() - panel->getContentOffset());
                }

                widget = parent;
            }

            const auto gui = requestingWidget->getParentGui();
            if (gui)
            {
                const Vector2f topLeftPos = gui->mapCoordsToPixel(inputRect.getPosition());
                const Vector2f bottomRightPos = gui->mapCoordsToPixel(inputRect.getPosition() + inputRect.getSize());
                inputRect = {topLeftPos, bottomRightPos - topLeftPos};
            }

            TGUI_IGNORE_DEPRECATED_WARNINGS_START
            getBackend()->openVirtualKeyboard(inputRect);
            TGUI_IGNORE_DEPRECATED_WARNINGS_END
        }
#endif
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        TGUI_NODISCARD inline bool isShiftPressed(const Event::KeyEvent& event)
        {
            return event.shift;
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        TGUI_NODISCARD inline bool isShiftPressed()
        {
            return getBackend()->isKeyboardModifierPressed(Event::KeyModifier::Shift);
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef TGUI_REMOVE_DEPRECATED_CODE
        TGUI_DEPRECATED("Use BackendGui::stopTextInput instead") inline void closeVirtualKeyboard()
        {
            TGUI_IGNORE_DEPRECATED_WARNINGS_START
            getBackend()->closeVirtualKeyboard();
            TGUI_IGNORE_DEPRECATED_WARNINGS_END
        }
#endif
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        TGUI_NODISCARD inline bool isMultiselectModifierPressed(const Event::KeyEvent& event)
        {
#ifdef TGUI_SYSTEM_MACOS
            return event.system;
#else
            return event.control;
#endif
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        TGUI_NODISCARD inline bool isMultiselectModifierPressed()
        {
#ifdef TGUI_SYSTEM_MACOS
            return getBackend()->isKeyboardModifierPressed(Event::KeyModifier::System);
#else
            return getBackend()->isKeyboardModifierPressed(Event::KeyModifier::Control);
#endif
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        TGUI_NODISCARD inline bool isKeyPressCopy(const Event::KeyEvent& event)
        {
#ifdef TGUI_SYSTEM_MACOS
            return (event.code == Event::KeyboardKey::C) && !event.control && !event.alt && !event.shift && event.system;
#else
            return (event.code == Event::KeyboardKey::C) && event.control && !event.alt && !event.shift && !event.system;
#endif
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        TGUI_NODISCARD inline bool isKeyPressCut(const Event::KeyEvent& event)
        {
#ifdef TGUI_SYSTEM_MACOS
            return (event.code == Event::KeyboardKey::X) && !event.control && !event.alt && !event.shift && event.system;
#else
            return (event.code == Event::KeyboardKey::X) && event.control && !event.alt && !event.shift && !event.system;
#endif
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        TGUI_NODISCARD inline bool isKeyPressPaste(const Event::KeyEvent& event)
        {
#ifdef TGUI_SYSTEM_MACOS
            return (event.code == Event::KeyboardKey::V) && !event.control && !event.alt && !event.shift && event.system;
#else
            return (event.code == Event::KeyboardKey::V) && event.control && !event.alt && !event.shift && !event.system;
#endif
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        TGUI_NODISCARD inline bool isKeyPressSelectAll(const Event::KeyEvent& event)
        {
#ifdef TGUI_SYSTEM_MACOS
            return (event.code == Event::KeyboardKey::A) && !event.control && !event.alt && !event.shift && event.system;
#else
            return (event.code == Event::KeyboardKey::A) && event.control && !event.alt && !event.shift && !event.system;
#endif
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        TGUI_NODISCARD inline bool isKeyPressMoveCaretLeft(const Event::KeyEvent& event)
        {
            return (event.code == Event::KeyboardKey::Left) && !event.control && !event.alt && !event.system;
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        TGUI_NODISCARD inline bool isKeyPressMoveCaretRight(const Event::KeyEvent& event)
        {
            return (event.code == Event::KeyboardKey::Right) && !event.control && !event.alt && !event.system;
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        TGUI_NODISCARD inline bool isKeyPressMoveCaretWordBegin(const Event::KeyEvent& event)
        {
#ifdef TGUI_SYSTEM_MACOS
            return (event.code == Event::KeyboardKey::Left) && !event.control && event.alt && !event.system;
#else
            return (event.code == Event::KeyboardKey::Left) && event.control && !event.alt && !event.system;
#endif
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        TGUI_NODISCARD inline bool isKeyPressMoveCaretWordEnd(const Event::KeyEvent& event)
        {
#ifdef TGUI_SYSTEM_MACOS
            return (event.code == Event::KeyboardKey::Right) && !event.control && event.alt && !event.system;
#else
            return (event.code == Event::KeyboardKey::Right) && event.control && !event.alt && !event.system;
#endif
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        TGUI_NODISCARD inline bool isKeyPressMoveCaretUp(const Event::KeyEvent& event)
        {
#ifdef TGUI_SYSTEM_MACOS
            // Option+UpArrow should actually move to the beginning of the paragraph (or the previous one), but we don't support this
            return (event.code == Event::KeyboardKey::Up) && !event.control && !event.system;
#else
            return (event.code == Event::KeyboardKey::Up) && !event.alt && !event.system;
#endif
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        TGUI_NODISCARD inline bool isKeyPressMoveCaretDown(const Event::KeyEvent& event)
        {
#ifdef TGUI_SYSTEM_MACOS
            // Option+DownArrow should actually move to the end of the paragraph (or the next one), but we don't support this
            return (event.code == Event::KeyboardKey::Down) && !event.control && !event.system;
#else
            return (event.code == Event::KeyboardKey::Down) && !event.alt && !event.system;
#endif
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        TGUI_NODISCARD inline bool isKeyPressMoveCaretLineStart(const Event::KeyEvent& event)
        {
#ifdef TGUI_SYSTEM_MACOS
            if ((event.code == Event::KeyboardKey::Left) && !event.control && !event.alt && event.system)
                return true;
#endif
            return (event.code == Event::KeyboardKey::Home) && !event.control && !event.alt && !event.system;
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        TGUI_NODISCARD inline bool isKeyPressMoveCaretLineEnd(const Event::KeyEvent& event)
        {
#ifdef TGUI_SYSTEM_MACOS
            if ((event.code == Event::KeyboardKey::Right) && !event.control && !event.alt && event.system)
                return true;
#endif
            return (event.code == Event::KeyboardKey::End) && !event.control && !event.alt && !event.system;
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        TGUI_NODISCARD inline bool isKeyPressMoveCaretDocumentBegin(const Event::KeyEvent& event)
        {
#ifdef TGUI_SYSTEM_MACOS
            return ((event.code == Event::KeyboardKey::Up) && !event.control && !event.alt && event.system)
                || ((event.code == Event::KeyboardKey::Home) && !event.control && !event.alt && event.system);
#else
            return (event.code == Event::KeyboardKey::Home) && event.control && !event.alt && !event.system;
#endif
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        TGUI_NODISCARD inline bool isKeyPressMoveCaretDocumentEnd(const Event::KeyEvent& event)
        {
#ifdef TGUI_SYSTEM_MACOS
            return ((event.code == Event::KeyboardKey::Down) && !event.control && !event.alt && event.system)
                || ((event.code == Event::KeyboardKey::End) && !event.control && !event.alt && event.system);
#else
            return (event.code == Event::KeyboardKey::End) && event.control && !event.alt && !event.system;
#endif
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif // TGUI_KEYBOARD_HPP

