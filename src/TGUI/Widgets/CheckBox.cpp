/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus' Graphical User Interface
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


#include <TGUI/Widgets/CheckBox.hpp>
#include <TGUI/Clipping.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <cmath>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    CheckBox::CheckBox()
    {
        m_type = "CheckBox";
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    CheckBox::Ptr CheckBox::create(sf::String text)
    {
        auto checkBox = std::make_shared<CheckBox>();

        if (!text.isEmpty())
            checkBox->setText(text);

        return checkBox;
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
        if (m_spriteUnchecked.isSet() && m_spriteChecked.isSet()
         && (m_textureUncheckedCached.getImageSize() != m_textureCheckedCached.getImageSize()))
        {
            sf::Vector2f sizeDiff = m_spriteChecked.getSize() - m_spriteUnchecked.getSize();
            if (getText().isEmpty())
                return getSize() + sf::Vector2f{std::max(0.f, sizeDiff.x - m_bordersCached.getRight()), std::max(0.f, sizeDiff.y - m_bordersCached.getTop())};
            else
                return getSize() + sf::Vector2f{(getSize().x * m_textDistanceRatioCached) + m_text.getSize().x, std::max(0.f, std::max((m_text.getSize().y - getSize().y) / 2, sizeDiff.y - m_bordersCached.getTop()))};
        }
        else
        {
            if (getText().isEmpty())
                return getSize();
            else
                return {getSize().x + (getSize().x * m_textDistanceRatioCached) + m_text.getSize().x, std::max(getSize().y, m_text.getSize().y)};
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    sf::Vector2f CheckBox::getWidgetOffset() const
    {
        float yOffset = 0;
        if (m_spriteUnchecked.isSet() && m_spriteChecked.isSet()
         && (m_textureUncheckedCached.getImageSize() != m_textureCheckedCached.getImageSize()))
        {
            const float sizeDiff = m_spriteChecked.getSize().y - m_spriteUnchecked.getSize().y;
            if (sizeDiff > m_bordersCached.getTop())
                yOffset = sizeDiff - m_bordersCached.getTop();
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
            if (m_textStyleCheckedCached.isSet())
                m_text.setStyle(m_textStyleCheckedCached);
            else
                m_text.setStyle(m_textStyleCached);

            onCheck->emit(this, true);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void CheckBox::leftMouseReleased(sf::Vector2f pos)
    {
        const bool mouseDown = m_mouseDown;

        ClickableWidget::leftMouseReleased(pos);

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

    void CheckBox::rendererChanged(const std::string& property)
    {
        if (property == "textureunchecked")
            m_textureUncheckedCached = getRenderer()->getTextureUnchecked();
        else if (property == "texturechecked")
            m_textureCheckedCached = getRenderer()->getTextureChecked();

        RadioButton::rendererChanged(property);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void CheckBox::updateTextureSizes()
    {
        if (m_spriteUnchecked.isSet() && m_spriteChecked.isSet())
        {
            m_spriteUnchecked.setSize(getInnerSize());
            m_spriteChecked.setSize(
                {getInnerSize().x + ((m_textureCheckedCached.getImageSize().x - m_textureUncheckedCached.getImageSize().x) * (getInnerSize().x / m_textureUncheckedCached.getImageSize().x)),
                 getInnerSize().y + ((m_textureCheckedCached.getImageSize().y - m_textureUncheckedCached.getImageSize().y) * (getInnerSize().y / m_textureUncheckedCached.getImageSize().y))}
            );

            m_spriteUncheckedHover.setSize(m_spriteUnchecked.getSize());
            m_spriteCheckedHover.setSize(m_spriteChecked.getSize());

            m_spriteUncheckedDisabled.setSize(m_spriteUnchecked.getSize());
            m_spriteCheckedDisabled.setSize(m_spriteChecked.getSize());

            m_spriteFocused.setSize(m_spriteUnchecked.getSize());
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void CheckBox::draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        states.transform.translate(getPosition());

        // Draw the borders
        if (m_bordersCached != Borders{0})
            drawBorders(target, states, m_bordersCached, getSize(), getCurrentBorderColor());

        states.transform.translate({m_bordersCached.getLeft(), m_bordersCached.getTop()});
        if (m_spriteUnchecked.isSet() && m_spriteChecked.isSet())
        {
            if (m_checked)
            {
                const Sprite* checkedSprite;
                if (!m_enabled && m_spriteCheckedDisabled.isSet())
                    checkedSprite = &m_spriteCheckedDisabled;
                else if (m_mouseHover && m_spriteCheckedHover.isSet())
                    checkedSprite = &m_spriteCheckedHover;
                else
                    checkedSprite = &m_spriteChecked;

                // The image may need to be shifted when the check leaves the box
                if (getInnerSize().y != checkedSprite->getSize().y)
                {
                    float diff = getInnerSize().y - checkedSprite->getSize().y;

                    states.transform.translate({0, diff});
                    checkedSprite->draw(target, states);
                    states.transform.translate({0, -diff});
                }
                else // Draw the checked texture normally
                    checkedSprite->draw(target, states);
            }
            else
            {
                if (!m_enabled && m_spriteUncheckedDisabled.isSet())
                    m_spriteUncheckedDisabled.draw(target, states);
                else if (m_mouseHover && m_spriteUncheckedHover.isSet())
                    m_spriteUncheckedHover.draw(target, states);
                else
                    m_spriteUnchecked.draw(target, states);
            }

            // When the radio button is focused then draw an extra image
            if (m_focused && m_spriteFocused.isSet())
                m_spriteFocused.draw(target, states);
        }
        else // There are no images
        {
            drawRectangleShape(target, states, getInnerSize(), getCurrentBackgroundColor());

            if (m_checked)
            {
                const float pi = 3.14159265358979f;
                const sf::Vector2f size = getInnerSize();
                const sf::Vector2f leftPoint = {0, size.y * 5.f/12.f};
                const sf::Vector2f middlePoint = {size.x / 2, size.y};
                const sf::Vector2f rightPoint = {size.x, 0};

                sf::RectangleShape left{{std::min(size.x, size.y) / 6, static_cast<float>(std::sqrt(std::pow(middlePoint.x - leftPoint.x, 2) + std::pow(middlePoint.y - leftPoint.y, 2)))}};
                left.setPosition(leftPoint);
                left.setOrigin({left.getSize().x / 2, 0});
                left.setRotation(-90 + (std::atan2(middlePoint.y - leftPoint.y, middlePoint.x - leftPoint.x) / pi * 180));

                sf::RectangleShape right{{std::min(size.x, size.y) / 5, static_cast<float>(std::sqrt(std::pow(rightPoint.x - middlePoint.x, 2) + std::pow(rightPoint.y - middlePoint.y, 2)))}};
                right.setPosition(middlePoint);
                right.setOrigin({left.getSize().x / 2, 0});
                right.setRotation(-90 + (std::atan2(rightPoint.y - middlePoint.y, rightPoint.x - middlePoint.x) / pi * 180));

                const sf::Color checkColor = getCurrentCheckColor();
                left.setFillColor(checkColor);
                right.setFillColor(checkColor);

                // Set the clipping for all draw calls that happen until this clipping object goes out of scope
                const Clipping clipping{target, states, {}, size};

                target.draw(left, states);
                target.draw(right, states);
            }
        }
        states.transform.translate({-m_bordersCached.getLeft(), -m_bordersCached.getTop()});

        if (!getText().isEmpty())
        {
            states.transform.translate({(1 + m_textDistanceRatioCached) * getSize().x, (getSize().y - m_text.getSize().y) / 2.0f});
            m_text.draw(target, states);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
