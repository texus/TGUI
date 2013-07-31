/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus's Graphical User Interface
// Copyright (C) 2012-2013 Bruno Van de Velde (vdv_b@tgui.eu)
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


#include <TGUI/Widgets.hpp>
#include <TGUI/ClickableWidget.hpp>
#include <TGUI/Label.hpp>

#include <SFML/OpenGL.hpp>

#include <cmath>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Label::Label() :
    m_AutoSize(true)
    {
        m_Callback.widgetType = Type_Label;
        m_Loaded = true;

        m_Background.setFillColor(sf::Color::Transparent);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Label::~Label()
    {
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Label* Label::clone()
    {
        return new Label(*this);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Label::load(const std::string& configFileFilename)
    {
        m_LoadedConfigFile = configFileFilename;

        // Open the config file
        ConfigFile configFile;
        if (!configFile.open(configFileFilename))
        {
            TGUI_OUTPUT("TGUI error: Failed to open " + configFileFilename + ".");
            return false;
        }

        // Read the properties and their values (as strings)
        std::vector<std::string> properties;
        std::vector<std::string> values;
        if (!configFile.read("Label", properties, values))
        {
            TGUI_OUTPUT("TGUI error: Failed to parse " + configFileFilename + ".");
            return false;
        }

        // Close the config file
        configFile.close();

        // Handle the read properties
        for (unsigned int i = 0; i < properties.size(); ++i)
        {
            std::string property = properties[i];
            std::string value = values[i];

            if (property == "textcolor")
            {
                setTextColor(extractColor(value));
            }
            else
                TGUI_OUTPUT("TGUI warning: Unrecognized property '" + property + "' in section Label in " + configFileFilename + ".");
        }

        return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const std::string& Label::getLoadedConfigFile()
    {
        return m_LoadedConfigFile;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Label::setSize(float width, float height)
    {
        // A negative size is not allowed for this object
        if (width  < 0) width  = -width;
        if (height < 0) height = -height;

        // Change the size of the label
        m_Size.x = width;
        m_Size.y = height;

        m_Background.setSize(m_Size);

        // You are no longer auto-sizing
        m_AutoSize = false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Label::setPosition(float x, float y)
    {
        Transformable::setPosition(x, y);

        m_Text.setPosition(std::floor(x + 0.5f), std::floor(y + 0.5f));
        m_Background.setPosition(x + m_Text.getLocalBounds().left, y + m_Text.getLocalBounds().top);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Label::setText(const sf::String& string)
    {
        m_Text.setString(string);

        m_Background.setPosition(getPosition().x + m_Text.getLocalBounds().left, getPosition().y + m_Text.getLocalBounds().top);

        // Change the size of the label if necessary
        if (m_AutoSize)
        {
            m_Size = sf::Vector2f(m_Text.getLocalBounds().left + m_Text.getLocalBounds().width,
                                  m_Text.getLocalBounds().top + m_Text.getLocalBounds().height);

            m_Background.setSize(m_Size);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const sf::String& Label::getText() const
    {
        return m_Text.getString();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Label::setTextFont(const sf::Font& font)
    {
        m_Text.setFont(font);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const sf::Font* Label::getTextFont() const
    {
        return m_Text.getFont();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Label::setTextColor(const sf::Color& color)
    {
        m_Text.setColor(color);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const sf::Color& Label::getTextColor() const
    {
        return m_Text.getColor();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Label::setTextSize(unsigned int size)
    {
        m_Text.setCharacterSize(size);

        m_Background.setPosition(getPosition().x + m_Text.getLocalBounds().left, getPosition().y + m_Text.getLocalBounds().top);

        // Change the size of the label if necessary
        if (m_AutoSize)
        {
            m_Size = sf::Vector2f(m_Text.getLocalBounds().left + m_Text.getLocalBounds().width,
                                  m_Text.getLocalBounds().top + m_Text.getLocalBounds().height);

            m_Background.setSize(m_Size);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int Label::getTextSize() const
    {
        return m_Text.getCharacterSize();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Label::setBackgroundColor(const sf::Color& backgroundColor)
    {
        m_Background.setFillColor(backgroundColor);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const sf::Color& Label::getBackgroundColor() const
    {
        return m_Background.getFillColor();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Label::setAutoSize(bool autoSize)
    {
        m_AutoSize = autoSize;

        // Change the size of the label if necessary
        if (m_AutoSize)
        {
            m_Size = sf::Vector2f(m_Text.getLocalBounds().left + m_Text.getLocalBounds().width,
                                  m_Text.getLocalBounds().top + m_Text.getLocalBounds().height);

            m_Background.setSize(m_Size);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Label::getAutoSize() const
    {
        return m_AutoSize;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Label::setTransparency(unsigned char transparency)
    {
        ClickableWidget::setTransparency(transparency);

        if (m_Background.getFillColor().a != 0)
            m_Background.setFillColor(sf::Color(m_Background.getFillColor().r, m_Background.getFillColor().g, m_Background.getFillColor().b, m_Opacity));

        m_Text.setColor(sf::Color(m_Text.getColor().r, m_Text.getColor().g, m_Text.getColor().b, m_Opacity));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Label::setProperty(const std::string& property, const std::string& value)
    {
        if (!Widget::setProperty(property, value))
        {
            if (property == "ConfigFile")
            {
                load(value);
            }
            else if (property == "Text")
            {
                setText(value);
            }
            else if (property == "TextColor")
            {
                setTextColor(extractColor(value));
            }
            else if (property == "TextSize")
            {
                setTextSize(atoi(value.c_str()));
            }
            else if (property == "BackgroundColor")
            {
                setBackgroundColor(extractColor(value));
            }
            else if (property == "AutoSize")
            {
                if ((value == "true") || (value == "True"))
                    setAutoSize(true);
                else if ((value == "false") || (value == "False"))
                    setAutoSize(false);
                else
                    TGUI_OUTPUT("TGUI error: Failed to parse 'AutoSize' property.");
            }
            else // The property didn't match
                return false;
        }

        // You pass here when one of the properties matched
        return true;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Label::getProperty(const std::string& property, std::string& value)
    {
        if (!Widget::getProperty(property, value))
        {
            if (property == "ConfigFile")
                value = getLoadedConfigFile();
            else if (property == "Text")
                value = getText().toAnsiString();
            else if (property == "TextColor")
                value = "(" + to_string(int(getTextColor().r)) + "," + to_string(int(getTextColor().g)) + "," + to_string(int(getTextColor().b)) + "," + to_string(int(getTextColor().a)) + ")";
            else if (property == "TextSize")
                value = to_string(getTextSize());
            else if (property == "BackgroundColor")
                value = "(" + to_string(int(getBackgroundColor().r)) + "," + to_string(int(getBackgroundColor().g)) + "," + to_string(int(getBackgroundColor().b)) + "," + to_string(int(getBackgroundColor().a)) + ")";
            else if (property == "AutoSize")
                value = m_AutoSize ? "true" : "false";
            else // The property didn't match
                return false;
        }

        // You pass here when one of the properties matched
        return true;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Label::initialize(tgui::Container *const parent)
    {

        m_Parent = parent;
        m_Text.setFont(m_Parent->getGlobalFont());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Label::draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        // When there is no text then there is nothing to draw
        if (m_Text.getString().isEmpty())
            return;

        // Calculate the scale factor of the view
        float scaleViewX = target.getSize().x / target.getView().getSize().x;
        float scaleViewY = target.getSize().y / target.getView().getSize().y;

        // Get the global position
        Vector2f topLeftPosition = states.transform.transformPoint(getPosition() + sf::Vector2f(m_Text.getLocalBounds().left, m_Text.getLocalBounds().top) - target.getView().getCenter() + (target.getView().getSize() / 2.f));
        Vector2f bottomRightPosition = states.transform.transformPoint(getPosition() + sf::Vector2f(m_Text.getLocalBounds().left, m_Text.getLocalBounds().top) + m_Size - target.getView().getCenter() + (target.getView().getSize() / 2.f));

        // Get the old clipping area
        GLint scissor[4];
        glGetIntegerv(GL_SCISSOR_BOX, scissor);

        // Calculate the clipping area
        GLint scissorLeft = TGUI_MAXIMUM(static_cast<GLint>(topLeftPosition.x * scaleViewX), scissor[0]);
        GLint scissorTop = TGUI_MAXIMUM(static_cast<GLint>(topLeftPosition.y * scaleViewY), static_cast<GLint>(target.getSize().y) - scissor[1] - scissor[3]);
        GLint scissorRight = TGUI_MINIMUM(static_cast<GLint>(bottomRightPosition.x * scaleViewX), scissor[0] + scissor[2]);
        GLint scissorBottom = TGUI_MINIMUM(static_cast<GLint>(bottomRightPosition.y * scaleViewY), static_cast<GLint>(target.getSize().y) - scissor[1]);

        // If the object outside the window then don't draw anything
        if (scissorRight < scissorLeft)
            scissorRight = scissorLeft;
        else if (scissorBottom < scissorTop)
            scissorTop = scissorBottom;

        // Set the clipping area
        glScissor(scissorLeft, target.getSize().y - scissorBottom, scissorRight - scissorLeft, scissorBottom - scissorTop);

        // Draw the background
        if (m_Background.getFillColor() != sf::Color::Transparent)
            target.draw(m_Background, states);

        // Draw the text
        target.draw(m_Text, states);

        // Reset the old clipping area
        glScissor(scissor[0], scissor[1], scissor[2], scissor[3]);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
