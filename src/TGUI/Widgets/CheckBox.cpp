/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus's Graphical User Interface
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


#include <TGUI/Container.hpp>
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

        m_renderer = std::make_shared<CheckBoxRenderer>(this);
        reload();

        getRenderer()->setPadding({3, 3, 3, 3});
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

    void CheckBox::check()
    {
        if (!m_checked)
        {
            m_checked = true;

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

            m_callback.checked = false;
            sendSignal("Unchecked", m_checked);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void CheckBox::leftMouseReleased(float x, float y)
    {
        bool mouseDown = m_mouseDown;

        ClickableWidget::leftMouseReleased(x, y);

        // Check if we clicked on the checkbox (not just mouse release)
        if (mouseDown)
        {
            // Check or uncheck the checkbox
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
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void CheckBoxRenderer::draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        if (m_textureUnchecked.isLoaded() && m_textureChecked.isLoaded())
        {
            if (m_radioButton->m_checked)
            {
                if (m_radioButton->m_mouseHover && m_textureCheckedHover.isLoaded())
                    target.draw(m_textureCheckedHover, states);
                else
                    target.draw(m_textureChecked, states);
            }
            else
            {
                if (m_radioButton->m_mouseHover && m_textureUncheckedHover.isLoaded())
                    target.draw(m_textureUncheckedHover, states);
                else
                    target.draw(m_textureUnchecked, states);
            }

            // When the radio button is focused then draw an extra image
            if (m_radioButton->m_focused && m_textureFocused.isLoaded())
                target.draw(m_textureFocused, states);
        }
        else // There are no images
        {
            sf::Vector2f foregroundSize = {m_radioButton->getSize().x - m_padding.left - m_padding.right,
                                           m_radioButton->getSize().y - m_padding.top - m_padding.bottom};

            // Draw the background and foreground at once when possible
            if (m_padding.left == m_padding.top && m_padding.top == m_padding.right && m_padding.right == m_padding.bottom && m_padding.bottom > 0)
            {
                sf::RectangleShape rect{m_radioButton->getSize()};
                rect.setPosition(m_radioButton->getPosition());
                rect.setOutlineThickness(-m_padding.left);

                if (m_radioButton->m_mouseHover)
                {
                    rect.setFillColor(calcColorOpacity(m_foregroundColorHover, m_radioButton->getOpacity()));
                    rect.setOutlineColor(calcColorOpacity(m_backgroundColorHover, m_radioButton->getOpacity()));
                }
                else
                {
                    rect.setFillColor(calcColorOpacity(m_foregroundColorNormal, m_radioButton->getOpacity()));
                    rect.setOutlineColor(calcColorOpacity(m_backgroundColorNormal, m_radioButton->getOpacity()));
                }

                target.draw(rect, states);
            }
            else // Draw background and foreground separately
            {
                // Draw the background (borders) if needed
                if (m_padding != Padding{0, 0, 0, 0})
                {
                    sf::RectangleShape border;
                    if (m_radioButton->m_mouseHover)
                        border.setFillColor(calcColorOpacity(m_backgroundColorHover, m_radioButton->getOpacity()));
                    else
                        border.setFillColor(calcColorOpacity(m_backgroundColorNormal, m_radioButton->getOpacity()));

                    sf::Vector2f position = m_radioButton->getPosition();
                    sf::Vector2f size = m_radioButton->getSize();

                    border.setSize({m_padding.left, size.y - m_padding.top});
                    border.setPosition(position.x, position.y + m_padding.top);
                    target.draw(border, states);

                    border.setSize({size.x - m_padding.right, m_padding.top});
                    border.setPosition(position.x, position.y);
                    target.draw(border, states);

                    border.setSize({m_padding.right, size.y - m_padding.bottom});
                    border.setPosition(position.x + size.x - m_padding.right, position.y);
                    target.draw(border, states);

                    border.setSize({size.x - m_padding.left, m_padding.bottom});
                    border.setPosition(position.x + m_padding.left, position.y + size.y - m_padding.top);
                    target.draw(border, states);
                }

                // Draw the foreground
                {
                    sf::RectangleShape foreground{foregroundSize};
                    foreground.setPosition(m_radioButton->getPosition().x + m_padding.left, m_radioButton->getPosition().y + m_padding.top);

                    if (m_radioButton->m_mouseHover)
                        foreground.setFillColor(calcColorOpacity(m_foregroundColorHover, m_radioButton->getOpacity()));
                    else
                        foreground.setFillColor(calcColorOpacity(m_foregroundColorNormal, m_radioButton->getOpacity()));

                    target.draw(foreground, states);
                }
            }

            // Draw the check if the radio button is checked
            if (m_radioButton->m_checked)
            {
                sf::Vector2f position = m_radioButton->getPosition();
                sf::Vector2f size = m_radioButton->getSize();

                // Set the clipping for all draw calls that happen until this clipping object goes out of scope
                Padding padding{m_padding.left + 1, m_padding.top + 1, m_padding.left + 1, m_padding.top + 1};
                Clipping clipping{target, states, {position.x + padding.left, position.y + padding.top}, {size.x - padding.left - padding.right, size.y - padding.top - padding.bottom}};

                sf::Vector2f leftPoint = {position.x + padding.left, position.y + (size.y * 5/12)};
                sf::Vector2f middlePoint = {position.x + (size.x / 2), position.y + size.y - padding.bottom};
                sf::Vector2f rightPoint = {position.x + size.x - padding.right, position.y + padding.top};

                sf::RectangleShape left{{std::min(size.x, size.y) / 6, static_cast<float>(std::sqrt(std::pow(middlePoint.x - leftPoint.x, 2) + std::pow(middlePoint.y - leftPoint.y, 2)))}};
                left.setPosition(leftPoint);
                left.setOrigin({left.getSize().x / 2, 0});
                left.setRotation(-90 + (std::atan2(middlePoint.y - leftPoint.y, middlePoint.x - leftPoint.x) / pi * 180));

                sf::RectangleShape right{{std::min(size.x, size.y) / 5, static_cast<float>(std::sqrt(std::pow(rightPoint.x - middlePoint.x, 2) + std::pow(rightPoint.y - middlePoint.y, 2)))}};
                right.setPosition(middlePoint);
                right.setOrigin({left.getSize().x / 2, 0});
                right.setRotation(-90 + (std::atan2(rightPoint.y - middlePoint.y, rightPoint.x - middlePoint.x) / pi * 180));

                if (m_radioButton->m_mouseHover)
                {
                    left.setFillColor(calcColorOpacity(m_checkColorHover, m_radioButton->getOpacity()));
                    right.setFillColor(calcColorOpacity(m_checkColorHover, m_radioButton->getOpacity()));
                }
                else
                {
                    left.setFillColor(calcColorOpacity(m_checkColorNormal, m_radioButton->getOpacity()));
                    right.setFillColor(calcColorOpacity(m_checkColorNormal, m_radioButton->getOpacity()));
                }

                target.draw(left, states);
                target.draw(right, states);
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::shared_ptr<WidgetRenderer> CheckBoxRenderer::clone(Widget* widget)
    {
        auto renderer = std::make_shared<CheckBoxRenderer>(*this);
        renderer->m_radioButton = static_cast<RadioButton*>(widget);
        return renderer;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
