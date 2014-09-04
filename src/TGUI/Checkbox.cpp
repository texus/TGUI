/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus's Graphical User Interface
// Copyright (C) 2012-2014 Bruno Van de Velde (vdv_b@tgui.eu)
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
#include <TGUI/Checkbox.hpp>

#include <SFML/OpenGL.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Checkbox::Checkbox()
    {
        m_callback.widgetType = Type_Checkbox;

        m_renderer = std::make_shared<CheckboxRenderer>(this);

        getRenderer()->setPadding({3, 3, 3, 3});
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Checkbox::Ptr Checkbox::create(const std::string& themeFileFilename, const std::string& section)
    {
        auto checkbox = std::make_shared<Checkbox>();

        if (themeFileFilename != "")
        {
            std::string loadedThemeFile = getResourcePath() + themeFileFilename;

            // Open the theme file
            ThemeFileParser themeFile{loadedThemeFile, section};

            // Find the folder that contains the theme file
            std::string themeFileFolder = "";
            std::string::size_type slashPos = loadedThemeFile.find_last_of("/\\");
            if (slashPos != std::string::npos)
                themeFileFolder = loadedThemeFile.substr(0, slashPos+1);

            // Handle the read properties
            for (auto it = themeFile.getProperties().cbegin(); it != themeFile.getProperties().cend(); ++it)
            {
                try
                {
                    checkbox->getRenderer()->setProperty(it->first, it->second, themeFileFolder);
                }
                catch (const Exception& e)
                {
                    throw Exception{std::string(e.what()) + " In section '" + section + "' in " + loadedThemeFile + "."};
                }
            }

            if (checkbox->getRenderer()->m_textureUnchecked.getData())
                checkbox->setSize(checkbox->getRenderer()->m_textureUnchecked.getImageSize());

            // The widget can only be focused when there is an image available for this phase
            if (checkbox->getRenderer()->m_textureFocused.getData() != nullptr)
                checkbox->m_allowFocus = true;
        }

        return checkbox;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Checkbox::check()
    {
        if (m_checked == false)
        {
            m_checked = true;

            // Add the callback (if the user requested it)
            if (m_callbackFunctions[Checked].empty() == false)
            {
                m_callback.trigger = Checked;
                m_callback.checked = true;
                addCallback();
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Checkbox::uncheck()
    {
        if (m_checked)
        {
            m_checked = false;

            // Add the callback (if the user requested it)
            if (m_callbackFunctions[Unchecked].empty() == false)
            {
                m_callback.trigger = Unchecked;
                m_callback.checked = false;
                addCallback();
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Checkbox::leftMouseReleased(float x, float y)
    {
        // Add the callback (if the user requested it)
        if (m_callbackFunctions[LeftMouseReleased].empty() == false)
        {
            m_callback.trigger = LeftMouseReleased;
            m_callback.checked = m_checked;
            m_callback.mouse.x = static_cast<int>(x - getPosition().x);
            m_callback.mouse.y = static_cast<int>(y - getPosition().y);
            addCallback();
        }

        // Check if we clicked on the checkbox (not just mouse release)
        if (m_mouseDown == true)
        {
            // Check or uncheck the checkbox
            if (m_checked)
                uncheck();
            else
                check();

            // Add the callback (if the user requested it)
            if (m_callbackFunctions[LeftMouseClicked].empty() == false)
            {
                m_callback.trigger = LeftMouseClicked;
                m_callback.checked = m_checked;
                m_callback.mouse.x = static_cast<int>(x - getPosition().x);
                m_callback.mouse.y = static_cast<int>(y - getPosition().y);
                addCallback();
            }

            m_mouseDown = false;
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Checkbox::keyPressed(const sf::Event::KeyEvent& event)
    {
        // Check if the space key or the return key was pressed
        if (event.code == sf::Keyboard::Space)
        {
            // Check or uncheck the checkbox
            if (m_checked)
                uncheck();
            else
                check();

            // Add the callback (if the user requested it)
            if (m_callbackFunctions[SpaceKeyPressed].empty() == false)
            {
                m_callback.trigger = SpaceKeyPressed;
                m_callback.checked = m_checked;
                addCallback();
            }
        }
        else if (event.code == sf::Keyboard::Return)
        {
            // Check or uncheck the checkbox
            if (m_checked)
                uncheck();
            else
                check();

            // Add the callback (if the user requested it)
            if (m_callbackFunctions[ReturnKeyPressed].empty() == false)
            {
                m_callback.trigger = ReturnKeyPressed;
                m_callback.checked = m_checked;
                addCallback();
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void CheckboxRenderer::draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        if ((m_textureUnchecked.getData() != nullptr) && (m_textureChecked.getData() != nullptr))
        {
            if (m_radioButton->m_checked)
            {
                if (m_radioButton->m_mouseHover && m_textureCheckedHover.getData())
                    target.draw(m_textureCheckedHover, states);
                else
                    target.draw(m_textureChecked, states);
            }
            else
            {
                if (m_radioButton->m_mouseHover && m_textureUncheckedHover.getData())
                    target.draw(m_textureUncheckedHover, states);
                else
                    target.draw(m_textureUnchecked, states);
            }

            // When the radio button is focused then draw an extra image
            if (m_radioButton->m_focused && m_textureFocused.getData())
                target.draw(m_textureFocused, states);
        }
        else // There are no images
        {
            // Draw the background (borders) if needed
            if (m_padding != Padding{0, 0, 0, 0})
            {
                sf::RectangleShape background(m_radioButton->getSize());
                background.setPosition(m_radioButton->getPosition());

                if (m_radioButton->m_mouseHover)
                    background.setFillColor(m_backgroundColorHover);
                else
                    background.setFillColor(m_backgroundColorNormal);

                target.draw(background, states);
            }

            sf::Vector2f foregroundSize = {m_radioButton->getSize().x - m_padding.left - m_padding.right,
                                           m_radioButton->getSize().y - m_padding.top - m_padding.bottom};

            // Draw the foreground
            {
                sf::RectangleShape foreground{foregroundSize};
                foreground.setPosition(m_radioButton->getPosition().x + m_padding.left, m_radioButton->getPosition().y + m_padding.top);

                if (m_radioButton->m_mouseHover)
                    foreground.setFillColor(m_foregroundColorHover);
                else
                    foreground.setFillColor(m_foregroundColorNormal);

                target.draw(foreground, states);
            }

            // Draw the check if the radio button is checked
            if (m_radioButton->m_checked)
            {
                sf::Vector2f position = m_radioButton->getPosition();
                sf::Vector2f size = m_radioButton->getSize();

                // Calculate the scale factor of the view
                const sf::View& view = target.getView();
                float scaleViewX = target.getSize().x / view.getSize().x;
                float scaleViewY = target.getSize().y / view.getSize().y;

                Padding padding{m_padding.left + 1, m_padding.top + 1, m_padding.left + 1, m_padding.top + 1};

                // Get the global position
                sf::Vector2f topLeftPosition = {((m_radioButton->getAbsolutePosition().x + padding.left - view.getCenter().x + (view.getSize().x / 2.f)) * view.getViewport().width) + (view.getSize().x * view.getViewport().left),
                                                ((m_radioButton->getAbsolutePosition().y + padding.top - view.getCenter().y + (view.getSize().y / 2.f)) * view.getViewport().height) + (view.getSize().y * view.getViewport().top)};
                sf::Vector2f bottomRightPosition = {(m_radioButton->getAbsolutePosition().x + size.x - padding.right - view.getCenter().x + (view.getSize().x / 2.f)) * view.getViewport().width + (view.getSize().x * view.getViewport().left),
                                                    (m_radioButton->getAbsolutePosition().y + size.y - padding.bottom - view.getCenter().y + (view.getSize().y / 2.f)) * view.getViewport().height + (view.getSize().y * view.getViewport().top)};

                // Get the old clipping area
                GLint scissor[4];
                glGetIntegerv(GL_SCISSOR_BOX, scissor);

                // Calculate the clipping area
                GLint scissorLeft = std::max(static_cast<GLint>(topLeftPosition.x * scaleViewX), scissor[0]);
                GLint scissorTop = std::max(static_cast<GLint>(topLeftPosition.y * scaleViewY), static_cast<GLint>(target.getSize().y) - scissor[1] - scissor[3]);
                GLint scissorRight = std::min(static_cast<GLint>(bottomRightPosition.x * scaleViewX), scissor[0] + scissor[2]);
                GLint scissorBottom = std::min(static_cast<GLint>(bottomRightPosition.y * scaleViewY), static_cast<GLint>(target.getSize().y) - scissor[1]);

                if (scissorRight < scissorLeft)
                    scissorRight = scissorLeft;
                else if (scissorBottom < scissorTop)
                    scissorTop = scissorBottom;

                // Set the clipping area
                glScissor(scissorLeft, target.getSize().y - scissorBottom, scissorRight - scissorLeft, scissorBottom - scissorTop);

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
                    left.setFillColor(m_checkColorHover);
                    right.setFillColor(m_checkColorHover);
                }
                else
                {
                    left.setFillColor(m_checkColorNormal);
                    right.setFillColor(m_checkColorNormal);
                }

                target.draw(left, states);
                target.draw(right, states);

                // Reset the old clipping area
                glScissor(scissor[0], scissor[1], scissor[2], scissor[3]);
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::shared_ptr<WidgetRenderer> CheckboxRenderer::clone(Widget* widget)
    {
        auto renderer = std::shared_ptr<CheckboxRenderer>(new CheckboxRenderer{*this});
        renderer->m_radioButton = static_cast<RadioButton*>(widget);
        return renderer;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
