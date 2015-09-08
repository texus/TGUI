/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus's Graphical User Interface
// Copyright (C) 2012-2015 Bruno Van de Velde (vdv_b@tgui.eu)
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


#include <TGUI/Animation.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    namespace priv
    {
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        bool Animation::update(sf::Time elapsedTime)
        {
            m_elapsedTime += elapsedTime;
            if (m_elapsedTime > m_totalDuration)
                m_elapsedTime = m_totalDuration;

            if (m_type == Type::Move)
                m_widget->setPosition(m_startPos + ((m_elapsedTime.asSeconds() / m_totalDuration.asSeconds()) * (m_endPos - m_startPos)));
            else if (m_type == Type::Fade)
                m_widget->setTransparency(static_cast<unsigned char>(m_startOpacity + ((m_elapsedTime.asSeconds() / m_totalDuration.asSeconds()) * (static_cast<int>(m_endOpacity) - static_cast<int>(m_startOpacity)))));

            if (m_elapsedTime == m_totalDuration)
            {
                if (m_finishedCallback != nullptr)
                    m_finishedCallback();

                return true;
            }

            return false;
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        MoveAnimation::MoveAnimation(Widget::Ptr widget, sf::Vector2f start, sf::Vector2f end, sf::Time duration, std::function<void()> finishedCallback)
        {
            m_type = Type::Move;
            m_widget = widget;
            m_startPos = start;
            m_endPos = end;
            m_totalDuration = duration;
            m_finishedCallback = finishedCallback;
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        FadeAnimation::FadeAnimation(Widget::Ptr widget, unsigned char start, unsigned char end, sf::Time duration, std::function<void()> finishedCallback)
        {
            m_type = Type::Fade;
            m_widget = widget;
            m_startOpacity = start;
            m_endOpacity = end;
            m_totalDuration = duration;
            m_finishedCallback = finishedCallback;
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
