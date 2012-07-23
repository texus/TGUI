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

    Label::Label() :
    m_AutoSize(true)
    {
        m_ObjectType = label;
        m_Loaded = true;

        m_RenderTexture = new sf::RenderTexture();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Label::initialize(const sf::Font& globalFont)
    {
        m_Text.setFont(globalFont);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Label* Label::clone()
    {
        return new Label(*this);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Label::setSize(float width, float height)
    {
        // A negative size is not allowed for this object
        if (width  < 0) width  = -width;
        if (height < 0) height = -height;

        // Set the size of the render texture
        m_RenderTexture->create(width, height);

        // You are no longer auto-sizing
        m_AutoSize = false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Vector2u Label::getSize() const
    {
        return m_RenderTexture->getSize();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Vector2f Label::getScaledSize() const
    {
        return Vector2f(m_RenderTexture->getSize().x * getScale().x, m_RenderTexture->getSize().y * getScale().y);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Label::setText(const std::string string)
    {
        m_Text.setString(string);

        // Change the size of the label if necessary
        if (m_AutoSize)
            m_RenderTexture->create(m_Text.getLocalBounds().left + m_Text.getLocalBounds().width, m_Text.getLocalBounds().top + m_Text.getLocalBounds().height);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::string Label::getText()
    {
        return m_Text.getString().toAnsiString();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Label::setTextFont(const sf::Font& font)
    {
        m_Text.setFont(font);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const sf::Font& Label::getTextFont()
    {
        return m_Text.getFont();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Label::setTextColor(const sf::Color& color)
    {
        m_Text.setColor(color);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const sf::Color& Label::getTextColor()
    {
        return m_Text.getColor();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Label::setTextSize(const unsigned int size)
    {
        m_Text.setCharacterSize(size);

        // Change the size of the label if necessary
        if (m_AutoSize)
            m_RenderTexture->create(m_Text.getLocalBounds().left + m_Text.getLocalBounds().width, m_Text.getLocalBounds().top + m_Text.getLocalBounds().height);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int Label::getTextSize()
    {
        return m_Text.getCharacterSize();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Label::setAutoSize(bool autoSize)
    {
        m_AutoSize = autoSize;

        // Change the size of the label if necessary
        if (m_AutoSize)
            m_RenderTexture->create(m_Text.getLocalBounds().left + m_Text.getLocalBounds().width, m_Text.getLocalBounds().top + m_Text.getLocalBounds().height);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Label::getAutoSize()
    {
        return m_AutoSize;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Label::mouseOnObject(float x, float y)
    {
        // Check if the mouse is on top of the label
        if (sf::FloatRect(0, 0, static_cast<float>(m_RenderTexture->getSize().x), static_cast<float>(m_RenderTexture->getSize().y)).contains(x - getPosition().x, y - getPosition().y))
            return true;
        else
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
        // When there is no text then there is nothing to draw
        if (m_Text.getString().isEmpty())
            return;

        m_RenderTexture->clear(sf::Color::Transparent);
        m_RenderTexture->draw(m_Text);
        m_RenderTexture->display();

        states.transform *= getTransform();
        target.draw(sf::Sprite(m_RenderTexture->getTexture()), states);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
