/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus's Graphical User Interface
// Copyright (C) 2012 Bruno Van de Velde (VDV_B@hotmail.com)
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


#include <TGUI/TGUI.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Label::Label()
    {
        m_ObjectType = label;

        m_Loaded = true;
        text.setCharacterSize(30);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Label::setSize(float width, float height)
    {
        // Undo the scaling
        setScale(1, 1);

        // Set the new text height
        text.setCharacterSize(static_cast<unsigned int>(height));

        // Get the size of the text
        sf::FloatRect bounds = text.getLocalBounds();

        // The text still has too fit inside the area
        if (bounds.width > width)
        {
            // Adjust the text size
            text.setCharacterSize(static_cast<unsigned int>((height * width) / bounds.width));
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Vector2u Label::getSize() const
    {
        return Vector2u(static_cast<unsigned int>(text.getLocalBounds().left + text.getLocalBounds().width), static_cast<unsigned int>(text.getLocalBounds().top + text.getLocalBounds().height));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Vector2f Label::getScaledSize() const
    {
        return Vector2f((text.getLocalBounds().left + text.getLocalBounds().width) * getScale().x, (text.getLocalBounds().top + text.getLocalBounds().height) * getScale().y);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Label::setText(const std::string string)
    {
        text.setString(string);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::string Label::getText()
    {
        return text.getString().toAnsiString();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Label::setTextFont(const sf::Font& font)
    {
        text.setFont(font);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const sf::Font& Label::getTextFont()
    {
        return text.getFont();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Label::setTextColor(const sf::Color& color)
    {
        text.setColor(color);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const sf::Color& Label::getTextColor()
    {
        return text.getColor();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Label::setTextSize(const unsigned int size)
    {
        text.setCharacterSize(size);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int Label::getTextSize()
    {
        return text.getCharacterSize();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Label::mouseOnObject(float x, float y)
    {
        // Get the current position and the text bounds
        Vector2f position = getPosition();
        sf::FloatRect textBounds = text.getGlobalBounds();

        // Check if the mouse is on top of the label
        if ((x > position.x + textBounds.left) && (x < position.x + textBounds.left + (textBounds.width * getScale().x))
            && (y > position.y + textBounds.top) && (y < position.y + textBounds.top + (textBounds.height * getScale().y)))
        {
            return true;
        }
        else // The mouse is not on top of the text
            return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Label::leftMousePressed(float x, float y)
    {
        TGUI_UNUSED_PARAM(x);
        TGUI_UNUSED_PARAM(y);

        // Set the mouse down flag
        m_MouseDown = true;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Label::leftMouseReleased(float x, float y)
    {
        // Check if we clicked on the label (not just mouse release)
        if (m_MouseDown == true)
        {
            // Add the callback (if the user requested it)
            if (callbackID > 0)
            {
                Callback callback;
                callback.callbackID  = callbackID;
                callback.trigger     = Callback::mouseClick;
                callback.mouseButton = sf::Mouse::Left;
                callback.mouseX      = x - getPosition().x;
                callback.mouseY      = y - getPosition().y;
                m_Parent->addCallback(callback);
            }

            m_MouseDown = false;
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Label::draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        states.transform *= getTransform();
        target.draw(text, states);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
