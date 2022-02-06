/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus' Graphical User Interface
// Copyright (C) 2012-2022 Bruno Van de Velde (vdv_b@tgui.eu)
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


#include <TGUI/Widgets/ToggleButton.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ToggleButton::ToggleButton(const char* typeName, bool initRenderer) :
        ButtonBase{typeName, initRenderer}
    {
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ToggleButton::Ptr ToggleButton::create(const String& text, bool down)
    {
        auto button = std::make_shared<ToggleButton>();

        if (!text.empty())
            button->setText(text);
        if (down)
            button->setDown(down);

        return button;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ToggleButton::Ptr ToggleButton::copy(ToggleButton::ConstPtr button)
    {
        if (button)
            return std::static_pointer_cast<ToggleButton>(button->clone());
        else
            return nullptr;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ToggleButton::setDown(bool down)
    {
        if (m_down == down)
            return;

        m_down = down;
        updateState();

        onToggle.emit(this, m_down);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ToggleButton::isDown() const
    {
        return m_down;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ToggleButton::leftMouseReleased(Vector2f pos)
    {
        if (m_mouseDown)
        {
            m_down = !m_down;
            onToggle.emit(this, m_down);
        }

        ButtonBase::leftMouseReleased(pos);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ToggleButton::keyPressed(const Event::KeyEvent& event)
    {
        if ((event.code == Event::KeyboardKey::Space) || (event.code == Event::KeyboardKey::Enter))
        {
            m_down = !m_down;
            updateState();

            onToggle.emit(this, m_down);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Signal& ToggleButton::getSignal(String signalName)
    {
        if (signalName == onToggle.getName())
            return onToggle;
        else
            return ButtonBase::getSignal(std::move(signalName));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
