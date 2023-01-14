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


#include <TGUI/Animation.hpp>
#include <TGUI/Widget.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    namespace priv
    {
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        AnimationType Animation::getType() const
        {
            return m_type;
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        void Animation::finish()
        {
            if (m_finishedCallback != nullptr)
                m_finishedCallback();
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        Animation::Animation(AnimationType type, Widget::Ptr widget, Duration duration, std::function<void()> finishedCallback) :
            m_type            {type},
            m_widget          {std::move(widget)},
            m_totalDuration   {duration},
            m_finishedCallback{std::move(finishedCallback)}
        {
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        MoveAnimation::MoveAnimation(Widget::Ptr widget, Vector2f start, Layout2d end, Duration duration, std::function<void()> finishedCallback) :
            Animation {AnimationType::Move, std::move(widget), duration, std::move(finishedCallback)},
            m_startPos{start},
            m_endPos  {std::move(end)}
        {
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        bool MoveAnimation::update(Duration elapsedTime)
        {
            m_elapsedTime += elapsedTime;
            if (m_elapsedTime >= m_totalDuration)
            {
                finish();
                return true;
            }

            m_widget->setPosition(m_startPos + ((m_elapsedTime / m_totalDuration) * (m_endPos.getValue() - m_startPos)));
            return false;
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        void MoveAnimation::finish()
        {
            m_widget->setPosition(m_endPos);
            Animation::finish();
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        ResizeAnimation::ResizeAnimation(Widget::Ptr widget, Vector2f start, Layout2d end, Duration duration, std::function<void()> finishedCallback) :
            Animation  {AnimationType::Resize, std::move(widget), duration, std::move(finishedCallback)},
            m_startSize{start},
            m_endSize  {std::move(end)}
        {
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        bool ResizeAnimation::update(Duration elapsedTime)
        {
            m_elapsedTime += elapsedTime;
            if (m_elapsedTime >= m_totalDuration)
            {
                finish();
                return true;
            }

            m_widget->setSize(m_startSize + ((m_elapsedTime / m_totalDuration) * (m_endSize.getValue() - m_startSize)));
            return false;
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        void ResizeAnimation::finish()
        {
            m_widget->setSize(m_endSize);
            Animation::finish();
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        FadeAnimation::FadeAnimation(Widget::Ptr widget, float start, float end, Duration duration, std::function<void()> finishedCallback) :
            Animation     {AnimationType::Opacity, std::move(widget), duration, std::move(finishedCallback)},
            m_startOpacity{std::max(0.f, std::min(1.f, start))},
            m_endOpacity  {std::max(0.f, std::min(1.f, end))}
        {
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        bool FadeAnimation::update(Duration elapsedTime)
        {
            m_elapsedTime += elapsedTime;
            if (m_elapsedTime >= m_totalDuration)
            {
                finish();
                return true;
            }

            m_widget->setInheritedOpacity(m_startOpacity + ((m_elapsedTime / m_totalDuration) * (m_endOpacity - m_startOpacity)));
            return false;
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        void FadeAnimation::finish()
        {
            m_widget->setInheritedOpacity(m_endOpacity);
            Animation::finish();
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
