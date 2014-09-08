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
#include <TGUI/RadioButton.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    RadioButton::RadioButton()
    {
        m_callback.widgetType = Type_RadioButton;

        m_renderer = std::make_shared<RadioButtonRenderer>(this);

        getRenderer()->setPadding({3, 3, 3, 3});

        m_text.setTextColor(getRenderer()->m_textColorNormal);

        setSize({24, 24});
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    RadioButton::Ptr RadioButton::create(const std::string& themeFileFilename, const std::string& section)
    {
        auto radioButton = std::make_shared<RadioButton>();

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
                    radioButton->getRenderer()->setProperty(it->first, it->second, themeFileFolder);
                }
                catch (const Exception& e)
                {
                    throw Exception{std::string(e.what()) + " In section '" + section + "' in " + loadedThemeFile + "."};
                }
            }

            if (radioButton->getRenderer()->m_textureUnchecked.getData())
                radioButton->setSize(radioButton->getRenderer()->m_textureUnchecked.getImageSize());

            // The widget can only be focused when there is an image available for this phase
            if (radioButton->getRenderer()->m_textureFocused.getData() != nullptr)
                radioButton->m_allowFocus = true;
        }

        return radioButton;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void RadioButton::setPosition(const Layout& position)
    {
        ClickableWidget::setPosition(position);

        getRenderer()->m_textureUnchecked.setPosition(getPosition());
        getRenderer()->m_textureChecked.setPosition(getPosition().x, getPosition().y + getSize().y - getRenderer()->m_textureChecked.getSize().y);
        getRenderer()->m_textureUncheckedHover.setPosition(getPosition());
        getRenderer()->m_textureCheckedHover.setPosition(getPosition());
        getRenderer()->m_textureFocused.setPosition(getPosition());

        m_text.setPosition(getPosition().x + getSize().x * 11.0f / 10.0f,
                           getPosition().y + ((getSize().y - m_text.getSize().y) / 2.0f));
    }


    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void RadioButton::setSize(const Layout& size)
    {
        Widget::setSize(size);

        // If the text is auto sized then recalculate the size
        if (m_textSize == 0)
            setText(m_text.getText());

        if (getRenderer()->m_textureUnchecked.getData() && getRenderer()->m_textureChecked.getData())
        {
            getRenderer()->m_textureFocused.setSize(getSize());
            getRenderer()->m_textureUnchecked.setSize(getSize());
            getRenderer()->m_textureChecked.setSize(
                {getSize().x + ((getRenderer()->m_textureChecked.getImageSize().x - getRenderer()->m_textureUnchecked.getImageSize().x) * (getSize().x / getRenderer()->m_textureUnchecked.getImageSize().x)),
                 getSize().y + ((getRenderer()->m_textureChecked.getImageSize().y - getRenderer()->m_textureUnchecked.getImageSize().y) * (getSize().y / getRenderer()->m_textureUnchecked.getImageSize().y))}
            );

            getRenderer()->m_textureUncheckedHover.setSize(getSize());
            getRenderer()->m_textureCheckedHover.setSize(getRenderer()->m_textureChecked.getSize());
        }

        // Reposition the text
        updatePosition();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    sf::Vector2f RadioButton::getFullSize() const
    {
        if (m_text.getText().isEmpty())
            return getSize();
        else
            return {(getSize().x * 11.0f / 10.0f) + m_text.getSize().x, getSize().y};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void RadioButton::check()
    {
        if (m_checked == false)
        {
            // Tell our parent that all the radio buttons should be unchecked
            m_parent->uncheckRadioButtons();

            // Check this radio button
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

    void RadioButton::uncheck()
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

    void RadioButton::setText(const sf::String& text)
    {
        // Set the new text
        m_text.setText(text);

        // Set the text size
        if (m_textSize == 0)
            m_text.setTextSize(static_cast<unsigned int>(getSize().y * 0.75f));
        else
            m_text.setTextSize(m_textSize);

        // Reposition the text
        updatePosition();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void RadioButton::setTextSize(unsigned int size)
    {
        // Change the text size
        m_textSize = size;

        // Call setText to reposition the text
        setText(m_text.getText());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void RadioButton::allowTextClick(bool acceptTextClick)
    {
        m_allowTextClick = acceptTextClick;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void RadioButton::setTransparency(unsigned char transparency)
    {
        ClickableWidget::setTransparency(transparency);

        getRenderer()->m_textureUnchecked.setColor(sf::Color(255, 255, 255, m_opacity));
        getRenderer()->m_textureChecked.setColor(sf::Color(255, 255, 255, m_opacity));
        getRenderer()->m_textureUncheckedHover.setColor(sf::Color(255, 255, 255, m_opacity));
        getRenderer()->m_textureCheckedHover.setColor(sf::Color(255, 255, 255, m_opacity));
        getRenderer()->m_textureFocused.setColor(sf::Color(255, 255, 255, m_opacity));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool RadioButton::mouseOnWidget(float x, float y)
    {
        if (m_allowTextClick)
        {
            // Check if the mouse is on top of the image or the small gap between image and text
            if (sf::FloatRect{getPosition().x, getPosition().y, getSize().x * 11.0f / 10.0f, getSize().y}.contains(x, y))
                return true;

            // Check if the mouse is on top of the text
            if (sf::FloatRect{getPosition().x, getPosition().y, m_text.getSize().x, m_text.getSize().y}.contains(x - (getSize().x * 11.0f / 10.0f), y - ((getSize().y - m_text.getSize().y) / 2.0f)))
                return true;
        }
        else // You are not allowed to click on the text
        {
            // Check if the mouse is on top of the image
            if (sf::FloatRect{getPosition().x, getPosition().y, getSize().x, getSize().y}.contains(x, y))
                return true;
        }

        if (m_mouseHover == true)
            mouseLeftWidget();

        return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void RadioButton::leftMouseReleased(float x, float y)
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

        // Check if we clicked on the radio button (not just mouse release)
        if (m_mouseDown == true)
        {
            // Check the radio button
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

    void RadioButton::keyPressed(const sf::Event::KeyEvent& event)
    {
        // Check if the space key or the return key was pressed
        if (event.code == sf::Keyboard::Space)
        {
            // Check the radio button
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
            // Check the radio button
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

    void RadioButton::widgetFocused()
    {
        // We can't be focused when we don't have a focus image
        if (getRenderer()->m_textureFocused.getData())
            Widget::widgetFocused();
        else
            unfocus();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void RadioButton::initialize(Container *const parent)
    {
        Widget::initialize(parent);

        if (!m_font && m_parent->getGlobalFont())
            getRenderer()->setTextFont(m_parent->getGlobalFont());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void RadioButton::mouseEnteredWidget()
    {
        Widget::mouseEnteredWidget();

        m_text.setTextColor(getRenderer()->m_textColorHover);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void RadioButton::mouseLeftWidget()
    {
        Widget::mouseLeftWidget();

        m_text.setTextColor(getRenderer()->m_textColorNormal);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void RadioButton::draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        getRenderer()->draw(target, states);

        // Draw the text
        target.draw(m_text, states);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void RadioButtonRenderer::setProperty(std::string property, const std::string& value, const std::string& rootPath)
    {
        if (property == "uncheckedimage")
            extractTextureFromString(property, value, rootPath, m_textureUnchecked);
        else if (property == "checkedimage")
            extractTextureFromString(property, value, rootPath, m_textureChecked);
        else if (property == "uncheckedhoverimage")
            extractTextureFromString(property, value, rootPath, m_textureUncheckedHover);
        else if (property == "checkedhoverimage")
            extractTextureFromString(property, value, rootPath, m_textureCheckedHover);
        else if (property == "focusedimage")
            extractTextureFromString(property, value, rootPath, m_textureFocused);
        else if (property == "textcolor")
            setTextColor(extractColorFromString(property, value));
        else if (property == "textcolornormal")
            setTextColorNormal(extractColorFromString(property, value));
        else if (property == "textcolorhover")
            setTextColorHover(extractColorFromString(property, value));
        else if (property == "backgroundcolor")
            setBackgroundColor(extractColorFromString(property, value));
        else if (property == "backgroundcolornormal")
            setBackgroundColorNormal(extractColorFromString(property, value));
        else if (property == "backgroundcolorhover")
            setBackgroundColorHover(extractColorFromString(property, value));
        else if (property == "foregroundcolor")
            setForegroundColor(extractColorFromString(property, value));
        else if (property == "foregroundcolornormal")
            setForegroundColorNormal(extractColorFromString(property, value));
        else if (property == "foregroundcolorhover")
            setForegroundColorHover(extractColorFromString(property, value));
        else if (property == "checkcolor")
            setCheckColor(extractColorFromString(property, value));
        else if (property == "checkcolornormal")
            setCheckColorNormal(extractColorFromString(property, value));
        else if (property == "checkcolorhover")
            setCheckColorHover(extractColorFromString(property, value));
        else if (property == "padding")
            setPadding(extractBordersFromString(property, value));
        else
            throw Exception{"Unrecognized property '" + property + "'."};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void RadioButtonRenderer::setTextFont(std::shared_ptr<sf::Font> font)
    {
        m_radioButton->m_font = font;
        m_radioButton->m_text.setTextFont(font);

        // Recalculate the text position and size
        m_radioButton->setText(m_radioButton->getText());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void RadioButtonRenderer::setTextColor(const sf::Color& color)
    {
        m_textColorNormal = color;
        m_textColorHover = color;

        m_radioButton->m_text.setTextColor(color);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void RadioButtonRenderer::setTextColorNormal(const sf::Color& color)
    {
        m_textColorNormal = color;

        if (!m_radioButton->m_mouseHover)
            m_radioButton->m_text.setTextColor(color);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void RadioButtonRenderer::setTextColorHover(const sf::Color& color)
    {
        m_textColorHover = color;

        if (m_radioButton->m_mouseHover)
            m_radioButton->m_text.setTextColor(color);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void RadioButtonRenderer::setUncheckedImage(const std::string& filename, const sf::IntRect& partRect, const sf::IntRect& middlePart, bool repeated)
    {
        if (filename != "")
            m_textureUnchecked.load(getResourcePath() + filename, partRect, middlePart, repeated);
        else
            m_textureUnchecked = {};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void RadioButtonRenderer::setCheckedImage(const std::string& filename, const sf::IntRect& partRect, const sf::IntRect& middlePart, bool repeated)
    {
        if (filename != "")
            m_textureChecked.load(getResourcePath() + filename, partRect, middlePart, repeated);
        else
            m_textureChecked = {};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void RadioButtonRenderer::setUncheckedHoverImage(const std::string& filename, const sf::IntRect& partRect, const sf::IntRect& middlePart, bool repeated)
    {
        if (filename != "")
            m_textureUncheckedHover.load(getResourcePath() + filename, partRect, middlePart, repeated);
        else
            m_textureUncheckedHover = {};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void RadioButtonRenderer::setCheckedHoverImage(const std::string& filename, const sf::IntRect& partRect, const sf::IntRect& middlePart, bool repeated)
    {
        if (filename != "")
            m_textureCheckedHover.load(getResourcePath() + filename, partRect, middlePart, repeated);
        else
            m_textureCheckedHover = {};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void RadioButtonRenderer::setBackgroundColor(const sf::Color& color)
    {
        m_backgroundColorNormal = color;
        m_backgroundColorHover = color;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void RadioButtonRenderer::setBackgroundColorNormal(const sf::Color& color)
    {
        m_backgroundColorNormal = color;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void RadioButtonRenderer::setBackgroundColorHover(const sf::Color& color)
    {
        m_backgroundColorHover = color;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void RadioButtonRenderer::setForegroundColor(const sf::Color& color)
    {
        m_foregroundColorNormal = color;
        m_foregroundColorHover = color;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void RadioButtonRenderer::setForegroundColorNormal(const sf::Color& color)
    {
        m_foregroundColorNormal = color;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void RadioButtonRenderer::setForegroundColorHover(const sf::Color& color)
    {
        m_foregroundColorHover = color;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void RadioButtonRenderer::setCheckColor(const sf::Color& color)
    {
        m_checkColorNormal = color;
        m_checkColorHover = color;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void RadioButtonRenderer::setCheckColorNormal(const sf::Color& color)
    {
        m_checkColorNormal = color;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void RadioButtonRenderer::setCheckColorHover(const sf::Color& color)
    {
        m_checkColorHover = color;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void RadioButtonRenderer::draw(sf::RenderTarget& target, sf::RenderStates states) const
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
                sf::CircleShape background{std::min(m_radioButton->getSize().x / 2.0f, m_radioButton->getSize().y / 2.0f)};
                background.setPosition(m_radioButton->getPosition());

                if (m_radioButton->m_mouseHover)
                    background.setFillColor(m_backgroundColorHover);
                else
                    background.setFillColor(m_backgroundColorNormal);

                target.draw(background, states);
            }

            float foregroundSize = std::min(m_radioButton->getSize().x - m_padding.left - m_padding.right,
                                            m_radioButton->getSize().y - m_padding.top - m_padding.bottom);

            // Draw the foreground
            {
                sf::CircleShape foreground{foregroundSize / 2.0f};
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
                sf::CircleShape check{foregroundSize * 7/24};
                check.setPosition(m_radioButton->getPosition().x + m_padding.left + ((foregroundSize / 2.0f) - check.getRadius()),
                                  m_radioButton->getPosition().y + m_padding.top + ((foregroundSize / 2.0f) - check.getRadius()));

                if (m_radioButton->m_mouseHover)
                    check.setFillColor(m_checkColorHover);
                else
                    check.setFillColor(m_checkColorNormal);

                target.draw(check, states);
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::shared_ptr<WidgetRenderer> RadioButtonRenderer::clone(Widget* widget)
    {
        auto renderer = std::shared_ptr<RadioButtonRenderer>(new RadioButtonRenderer{*this});
        renderer->m_radioButton = static_cast<RadioButton*>(widget);
        return renderer;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
