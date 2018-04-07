/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus' Graphical User Interface
// Copyright (C) 2012-2018 Bruno Van de Velde (vdv_b@tgui.eu)
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

        setRenderer(Theme::getDefault()->getRendererNoThrow(m_type));

        setSize({Text::getLineHeight(m_text) + m_bordersCached.getLeft() + m_bordersCached.getRight(),
                 Text::getLineHeight(m_text) + m_bordersCached.getTop() + m_bordersCached.getBottom()});
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

    Vector2f CheckBox::getFullSize() const
    {
        if (m_spriteUnchecked.isSet() && m_spriteChecked.isSet()
         && (m_textureUncheckedCached.getImageSize() != m_textureCheckedCached.getImageSize()))
        {
            Vector2f sizeDiff = m_spriteChecked.getSize() - m_spriteUnchecked.getSize();
            if (getText().isEmpty())
                return getSize() + Vector2f{std::max(0.f, sizeDiff.x - m_bordersCached.getRight()), std::max(0.f, sizeDiff.y - m_bordersCached.getTop())};
            else
                return getSize() + Vector2f{(getSize().x * m_textDistanceRatioCached) + m_text.getSize().x, std::max(0.f, std::max((m_text.getSize().y - getSize().y) / 2, sizeDiff.y - m_bordersCached.getTop()))};
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

    Vector2f CheckBox::getWidgetOffset() const
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

    void CheckBox::setChecked(bool checked)
    {
        if (checked && !m_checked)
        {
            m_checked = true;

            updateTextColor();
            if (m_textStyleCheckedCached.isSet())
                m_text.setStyle(m_textStyleCheckedCached);
            else
                m_text.setStyle(m_textStyleCached);

            onCheck.emit(this, true);
        }
        else
            RadioButton::setChecked(checked);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void CheckBox::leftMouseReleased(Vector2f pos)
    {
        const bool mouseDown = m_mouseDown;

        ClickableWidget::leftMouseReleased(pos);

        // Check or uncheck when we clicked on the checkbox (not just mouse release)
        if (mouseDown)
            setChecked(!m_checked);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void CheckBox::keyPressed(const sf::Event::KeyEvent& event)
    {
        // Check or uncheck the checkbox if the space key or the return key was pressed
        if ((event.code == sf::Keyboard::Space) || (event.code == sf::Keyboard::Return))
            setChecked(!m_checked);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void CheckBox::rendererChanged(const std::string& property)
    {
        if (property == "textureunchecked")
            m_textureUncheckedCached = getSharedRenderer()->getTextureUnchecked();
        else if (property == "texturechecked")
            m_textureCheckedCached = getSharedRenderer()->getTextureChecked();

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

            m_spriteUncheckedFocused.setSize(m_spriteUnchecked.getSize());
            m_spriteCheckedFocused.setSize(m_spriteChecked.getSize());
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
                else if (m_focused && m_spriteCheckedFocused.isSet())
                    checkedSprite = &m_spriteCheckedFocused;
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
                else if (m_focused && m_spriteUncheckedFocused.isSet())
                    m_spriteUncheckedFocused.draw(target, states);
                else
                    m_spriteUnchecked.draw(target, states);
            }
        }
        else // There are no images
        {
            drawRectangleShape(target, states, getInnerSize(), getCurrentBackgroundColor());

            if (m_checked)
            {
                const float pi = 3.14159265358979f;
                const Color& checkColor = getCurrentCheckColor();
                const Vector2f size = getInnerSize();
                const float lineThickness = std::min(size.x, size.y) / 5;
                const Vector2f leftPoint = {0.14f * size.x, 0.4f * size.y};
                const Vector2f middlePoint = {0.44f * size.x, 0.7f * size.y};
                const Vector2f rightPoint = {0.86f * size.x, 0.28f * size.y};
                const float x = (lineThickness / 2.f) * std::cos(pi / 4.f);
                const float y = (lineThickness / 2.f) * std::sin(pi / 4.f);
                const std::vector<sf::Vertex> vertices = {
                    {{leftPoint.x - x, leftPoint.y + y}, checkColor},
                    {{leftPoint.x + x, leftPoint.y - y}, checkColor},
                    {{middlePoint.x, middlePoint.y + 2*y}, checkColor},
                    {{middlePoint.x, middlePoint.y - 2*y}, checkColor},
                    {{rightPoint.x + x, rightPoint.y + y}, checkColor},
                    {{rightPoint.x - x, rightPoint.y - y}, checkColor}
                };

                target.draw(vertices.data(), vertices.size(), sf::PrimitiveType::TrianglesStrip, states);
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
