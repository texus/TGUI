/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus' Graphical User Interface
// Copyright (C) 2012-2016 Bruno Van de Velde (vdv_b@tgui.eu)
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


#include <TGUI/Widgets/CheckBox.hpp>
#include <TGUI/Clipping.hpp>

#include <cmath>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    CheckBox::CheckBox()
    {
        m_callback.widgetType = "CheckBox";
        m_type = "CheckBox";
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    CheckBox::Ptr CheckBox::copy(CheckBox::ConstPtr checkbox)
    {
        if (checkbox)
            return std::static_pointer_cast<CheckBox>(checkbox->clone());
        else
            return nullptr;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    sf::Vector2f CheckBox::getFullSize() const
    {
        if (getRenderer()->getTextureUnchecked().isLoaded() && getRenderer()->getTextureChecked().isLoaded()
         && (getRenderer()->getTextureUnchecked().getImageSize() != getRenderer()->getTextureChecked().getImageSize()))
        {
            sf::Vector2f sizeDiff = getRenderer()->getTextureChecked().getSize() - getRenderer()->getTextureUnchecked().getSize();
            if (getText().isEmpty())
                return getSize() + sf::Vector2f{std::max(0.f, sizeDiff.x - getRenderer()->getBorders().right), std::max(0.f, sizeDiff.y - getRenderer()->getBorders().top)};
            else
                return getSize() + sf::Vector2f{(getSize().x * getRenderer()->getTextDistanceRatio()) + m_text.getSize().x, std::max(0.f, std::max((m_text.getSize().y - getSize().y) / 2, sizeDiff.y - getRenderer()->getBorders().top))};
        }
        else
        {
            if (getText().isEmpty())
                return getSize();
            else
                return {getSize().x + (getSize().x * getRenderer()->getTextDistanceRatio()) + m_text.getSize().x, std::max(getSize().y, m_text.getSize().y)};
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    sf::Vector2f CheckBox::getWidgetOffset() const
    {
        float yOffset = 0;
        if (getRenderer()->getTextureUnchecked().isLoaded() && getRenderer()->getTextureChecked().isLoaded()
         && (getRenderer()->getTextureUnchecked().getImageSize() != getRenderer()->getTextureChecked().getImageSize()))
        {
            float sizeDiff = getRenderer()->getTextureChecked().getSize().y - getRenderer()->getTextureUnchecked().getSize().y;
            if (sizeDiff > getRenderer()->getBorders().top)
                yOffset = sizeDiff - getRenderer()->getBorders().top;
        }

        if (getText().isEmpty() || (getSize().y >= m_text.getSize().y))
            return {0, -yOffset};
        else
            return {0, -std::max(yOffset, (m_text.getSize().y - getSize().y) / 2)};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void CheckBox::check()
    {
        if (!m_checked)
        {
            m_checked = true;

            updateTextColor();
            if (getRenderer()->getTextStyleChecked().isSet())
                m_text.getRenderer()->setTextStyle(getRenderer()->getTextStyleChecked());
            else
                m_text.getRenderer()->setTextStyle(getRenderer()->getTextStyle());

            m_callback.checked = true;
            sendSignal("Checked", m_checked);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void CheckBox::uncheck()
    {
        if (m_checked)
        {
            m_checked = false;

            updateTextColor();
            m_text.getRenderer()->setTextStyle(getRenderer()->getTextStyle());

            m_callback.checked = false;
            sendSignal("Unchecked", m_checked);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void CheckBox::leftMouseReleased(float x, float y)
    {
        bool mouseDown = m_mouseDown;

        ClickableWidget::leftMouseReleased(x, y);

        // Check or uncheck when we clicked on the checkbox (not just mouse release)
        if (mouseDown)
        {
            if (m_checked)
                uncheck();
            else
                check();
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void CheckBox::keyPressed(const sf::Event::KeyEvent& event)
    {
        // Check or uncheck the checkbox if the space key or the return key was pressed
        if ((event.code == sf::Keyboard::Space) || (event.code == sf::Keyboard::Return))
        {
            if (m_checked)
                uncheck();
            else
                check();
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void CheckBox::draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        states.transform.translate(getPosition());

        // Draw the borders
        Borders borders = getRenderer()->getBorders();
        if (borders != Borders{0})
            drawBorders(target, states, borders, getSize(), getCurrentBorderColor());

        states.transform.translate({borders.left, borders.top});
        if (getRenderer()->getTextureUnchecked().isLoaded() && getRenderer()->getTextureChecked().isLoaded())
        {
            if (m_checked)
            {
                if (!m_enabled && getRenderer()->getTextureCheckedDisabled().isLoaded())
                    getRenderer()->getTextureCheckedDisabled().draw(target, states);
                else if (m_mouseHover && getRenderer()->getTextureCheckedHover().isLoaded())
                    getRenderer()->getTextureCheckedHover().draw(target, states);
                else
                    getRenderer()->getTextureChecked().draw(target, states);
            }
            else
            {
                if (!m_enabled && getRenderer()->getTextureUncheckedDisabled().isLoaded())
                    getRenderer()->getTextureUncheckedDisabled().draw(target, states);
                else if (m_mouseHover && getRenderer()->getTextureUncheckedHover().isLoaded())
                    getRenderer()->getTextureUncheckedHover().draw(target, states);
                else
                    getRenderer()->getTextureUnchecked().draw(target, states);
            }

            // When the radio button is focused then draw an extra image
            if (m_focused && getRenderer()->getTextureFocused().isLoaded())
                getRenderer()->getTextureFocused().draw(target, states);
        }
        else // There are no images
        {
            drawRectangleShape(target, states, getInnerSize(), getCurrentBackgroundColor());

            if (m_checked)
            {
                // Set the clipping for all draw calls that happen until this clipping object goes out of scope
                Clipping clipping{target, states, {borders.left, borders.top}, getInnerSize()};

                sf::Vector2f size = getInnerSize();

                sf::Vector2f leftPoint = {0, size.y * 5.f/12.f};
                sf::Vector2f middlePoint = {size.x / 2, size.y};
                sf::Vector2f rightPoint = {size.x, 0};

                sf::RectangleShape left{{std::min(size.x, size.y) / 6, static_cast<float>(std::sqrt(std::pow(middlePoint.x - leftPoint.x, 2) + std::pow(middlePoint.y - leftPoint.y, 2)))}};
                left.setPosition(leftPoint);
                left.setOrigin({left.getSize().x / 2, 0});
                left.setRotation(-90 + (std::atan2(middlePoint.y - leftPoint.y, middlePoint.x - leftPoint.x) / pi * 180));

                sf::RectangleShape right{{std::min(size.x, size.y) / 5, static_cast<float>(std::sqrt(std::pow(rightPoint.x - middlePoint.x, 2) + std::pow(rightPoint.y - middlePoint.y, 2)))}};
                right.setPosition(middlePoint);
                right.setOrigin({left.getSize().x / 2, 0});
                right.setRotation(-90 + (std::atan2(rightPoint.y - middlePoint.y, rightPoint.x - middlePoint.x) / pi * 180));

                sf::Color checkColor = getCurrentCheckColor();
                left.setFillColor(checkColor);
                right.setFillColor(checkColor);

                target.draw(left, states);
                target.draw(right, states);
            }
        }
        states.transform.translate({-borders.left, -borders.top});

        if (!getText().isEmpty())
        {
            states.transform.translate({(1 + getRenderer()->getTextDistanceRatio()) * getSize().x, (getSize().y - m_text.getSize().y) / 2.0f});
            target.draw(m_text, states);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void CheckBox::updateTextureSizes()
    {
        if (getRenderer()->getTextureUnchecked().isLoaded() && getRenderer()->getTextureChecked().isLoaded())
        {
            getRenderer()->getTextureUnchecked().setSize(getInnerSize());
            getRenderer()->getTextureChecked().setSize(
                {getInnerSize().x + ((getRenderer()->getTextureChecked().getImageSize().x - getRenderer()->getTextureUnchecked().getImageSize().x) * (getInnerSize().x / getRenderer()->getTextureUnchecked().getImageSize().x)),
                 getInnerSize().y + ((getRenderer()->getTextureChecked().getImageSize().y - getRenderer()->getTextureUnchecked().getImageSize().y) * (getInnerSize().y / getRenderer()->getTextureUnchecked().getImageSize().y))}
            );

            getRenderer()->getTextureUncheckedHover().setSize(getRenderer()->getTextureUnchecked().getSize());
            getRenderer()->getTextureCheckedHover().setSize(getRenderer()->getTextureChecked().getSize());

            getRenderer()->getTextureUncheckedDisabled().setSize(getRenderer()->getTextureUnchecked().getSize());
            getRenderer()->getTextureCheckedDisabled().setSize(getRenderer()->getTextureChecked().getSize());

            getRenderer()->getTextureFocused().setSize(getRenderer()->getTextureUnchecked().getSize());
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
