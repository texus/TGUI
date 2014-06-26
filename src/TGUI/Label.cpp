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


#include <SFML/OpenGL.hpp>

#include <TGUI/Container.hpp>
#include <TGUI/Label.hpp>

#include <cmath>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Label::Label()
    {
        m_callback.widgetType = Type_Label;

        m_background.setFillColor(sf::Color::Transparent);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Label::load(const std::string& configFileFilename)
    {
        m_loadedConfigFile = getResourcePath() + configFileFilename;

        // Open the config file
        ConfigFile configFile{m_loadedConfigFile, "Label"};

        // Handle the read properties
        for (auto it = configFile.getProperties().cbegin(); it != configFile.getProperties().cend(); ++it)
        {
            if (it->first == "textcolor")
                setTextColor(extractColor(it->second));
            else
                throw Exception{"Unrecognized property '" + it->first + "' in section Label in " + m_loadedConfigFile + "."};
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Label::setPosition(const Layout& position)
    {
        Widget::setPosition(position);

        m_background.setPosition(position.getValue());
        m_text.setPosition(std::floor(position.getValue().x - m_text.getLocalBounds().left + 0.5f),
                           std::floor(position.getValue().y - m_text.getLocalBounds().top + 0.5f));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Label::setSize(const Layout& size)
    {
        Widget::setSize(size);

        m_background.setSize(getSize());

        // You are no longer auto-sizing
        m_autoSize = false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Label::setText(const sf::String& string)
    {
        m_text.setString(string);

        updatePosition();

        // Change the size of the label if necessary
        if (m_autoSize)
        {
            setSize({m_text.getLocalBounds().width, m_text.getLocalBounds().height});
            m_autoSize = true;
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Label::setTextFont(const sf::Font& font)
    {
        m_text.setFont(font);
        setText(getText());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Label::setTextSize(unsigned int size)
    {
        m_text.setCharacterSize(size);

        updatePosition();

        // Change the size of the label if necessary
        if (m_autoSize)
        {
            setSize({m_text.getLocalBounds().width, m_text.getLocalBounds().height});
            m_autoSize = true;
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Label::setAutoSize(bool autoSize)
    {
        m_autoSize = autoSize;

        // Change the size of the label if necessary
        if (m_autoSize)
        {
            setSize({m_text.getLocalBounds().width, m_text.getLocalBounds().height});
            m_autoSize = true;
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Label::setProperty(std::string property, const std::string& value)
    {
        property = toLower(property);

        if (property == "configfile")
        {
            load(value);
        }
        else if (property == "text")
        {
            std::string text;
            decodeString(value, text);
            setText(text);
        }
        else if (property == "textcolor")
        {
            setTextColor(extractColor(value));
        }
        else if (property == "textsize")
        {
            setTextSize(tgui::stoi(value));
        }
        else if (property == "backgroundcolor")
        {
            setBackgroundColor(extractColor(value));
        }
        else if (property == "autosize")
        {
            if ((value == "true") || (value == "True"))
                setAutoSize(true);
            else if ((value == "false") || (value == "False"))
                setAutoSize(false);
            else
                throw Exception{"Failed to parse 'AutoSize' property."};
        }
        else // The property didn't match
            ClickableWidget::setProperty(property, value);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Label::getProperty(std::string property, std::string& value) const
    {
        property = toLower(property);

        if (property == "configfile")
            value = getLoadedConfigFile();
        else if (property == "text")
            encodeString(getText(), value);
        else if (property == "textcolor")
            value = "(" + tgui::to_string(int(getTextColor().r)) + "," + tgui::to_string(int(getTextColor().g)) + "," + tgui::to_string(int(getTextColor().b)) + "," + tgui::to_string(int(getTextColor().a)) + ")";
        else if (property == "textsize")
            value = tgui::to_string(getTextSize());
        else if (property == "backgroundcolor")
            value = "(" + tgui::to_string(int(getBackgroundColor().r)) + "," + tgui::to_string(int(getBackgroundColor().g)) + "," + tgui::to_string(int(getBackgroundColor().b)) + "," + tgui::to_string(int(getBackgroundColor().a)) + ")";
        else if (property == "autosize")
            value = m_autoSize ? "true" : "false";
        else // The property didn't match
            ClickableWidget::getProperty(property, value);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::list< std::pair<std::string, std::string> > Label::getPropertyList() const
    {
        auto list = ClickableWidget::getPropertyList();
        list.push_back(std::pair<std::string, std::string>("ConfigFile", "string"));
        list.push_back(std::pair<std::string, std::string>("Text", "string"));
        list.push_back(std::pair<std::string, std::string>("TextColor", "color"));
        list.push_back(std::pair<std::string, std::string>("TextSize", "uint"));
        list.push_back(std::pair<std::string, std::string>("BackgroundColor", "color"));
        list.push_back(std::pair<std::string, std::string>("AutoSize", "bool"));
        return list;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Label::initialize(Container *const parent)
    {
        Widget::initialize(parent);

        if (!getTextFont() && m_parent->getGlobalFont())
            setTextFont(*m_parent->getGlobalFont());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Label::draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        // When there is no text then there is nothing to draw
        if (m_text.getString().isEmpty())
            return;

        const sf::View& view = target.getView();

        // Calculate the scale factor of the view
        float scaleViewX = target.getSize().x / view.getSize().x;
        float scaleViewY = target.getSize().y / view.getSize().y;

        // Get the global position
        sf::Vector2f topLeftPosition = {((getAbsolutePosition().x - view.getCenter().x + (view.getSize().x / 2.f)) * view.getViewport().width) + (view.getSize().x * view.getViewport().left),
                                        ((getAbsolutePosition().y - view.getCenter().y + (view.getSize().y / 2.f)) * view.getViewport().height) + (view.getSize().y * view.getViewport().top)};
        sf::Vector2f bottomRightPosition = {(getAbsolutePosition().x + getSize().x - view.getCenter().x + (view.getSize().x / 2.f)) * view.getViewport().width + (view.getSize().x * view.getViewport().left),
                                            (getAbsolutePosition().y + getSize().y - view.getCenter().y + (view.getSize().y / 2.f)) * view.getViewport().height + (view.getSize().y * view.getViewport().top)};

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
        if (m_background.getFillColor() != sf::Color::Transparent)
            target.draw(m_background, states);

        // Draw the text
        target.draw(m_text, states);

        // Reset the old clipping area
        glScissor(scissor[0], scissor[1], scissor[2], scissor[3]);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
