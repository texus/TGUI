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

#include <TGUI/SharedWidgetPtr.inl>
#include <TGUI/Panel.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Panel::Panel()
    {
        m_callback.widgetType = Type_Panel;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Panel::Panel(const Panel& panelToCopy) :
        Container        (panelToCopy),
        m_size           (panelToCopy.m_size),
        m_backgroundColor(panelToCopy.m_backgroundColor),
        m_texture        (panelToCopy.m_texture)
    {
        if (m_texture)
        {
            m_sprite.setTexture(*m_texture);
            m_sprite.setScale(m_size.x / m_texture->getSize().x, m_size.y / m_texture->getSize().y);
            m_sprite.setColor(sf::Color(255, 255, 255, m_opacity));
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Panel& Panel::operator= (const Panel& right)
    {
        // Make sure it is not the same widget
        if (this != &right)
        {
            Panel temp(right);
            this->Container::operator=(right);

            std::swap(m_size,            temp.m_size);
            std::swap(m_backgroundColor, temp.m_backgroundColor);
            std::swap(m_texture,         temp.m_texture);
            std::swap(m_sprite,          temp.m_sprite);
        }

        return *this;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Panel::setSize(const sf::Vector2f& size)
    {
        m_size.x = std::abs(size.x);
        m_size.y = std::abs(size.y);

        // If there is a background texture then resize it
        if (m_texture)
            m_sprite.setScale(m_size.x / m_texture->getSize().x, m_size.y / m_texture->getSize().y);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Panel::setBackgroundTexture(sf::Texture *const texture)
    {
        // Store the texture
        m_texture = texture;

        // Set the texture for the sprite
        if (m_texture)
        {
            m_sprite.setTexture(*m_texture, true);
            m_sprite.setScale(m_size.x / m_texture->getSize().x, m_size.y / m_texture->getSize().y);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Panel::setTransparency(unsigned char transparency)
    {
        Container::setTransparency(transparency);

        m_sprite.setColor(sf::Color(255, 255, 255, m_opacity));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Panel::mouseOnWidget(float x, float y)
    {
        // Check if the mouse is inside the panel
        if (getTransform().transformRect(sf::FloatRect(0, 0, m_size.x, m_size.y)).contains(x, y))
            return true;

        if (m_mouseHover)
        {
            mouseLeftWidget();

            // Tell the widgets inside the panel that the mouse is no longer on top of them
            for (unsigned int i = 0; i < m_widgets.size(); ++i)
                m_widgets[i]->mouseNotOnWidget();

            m_mouseHover = false;
        }

        return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Panel::leftMousePressed(float x, float y)
    {
        if (mouseOnWidget(x, y))
        {
            m_mouseDown = true;

            if (!m_callbackFunctions[LeftMousePressed].empty())
            {
                m_callback.trigger = LeftMousePressed;
                m_callback.mouse.x = static_cast<int>(x - getPosition().x);
                m_callback.mouse.y = static_cast<int>(y - getPosition().y);
                addCallback();
            }
        }

        Container::leftMousePressed(x, y);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Panel::leftMouseReleased(float x , float y)
    {
        if (mouseOnWidget(x, y))
        {
            if (!m_callbackFunctions[LeftMouseReleased].empty())
            {
                m_callback.trigger = LeftMouseReleased;
                m_callback.mouse.x = static_cast<int>(x - getPosition().x);
                m_callback.mouse.y = static_cast<int>(y - getPosition().y);
                addCallback();
            }

            if (m_mouseDown)
            {
                if (!m_callbackFunctions[LeftMouseClicked].empty())
                {
                    m_callback.trigger = LeftMouseClicked;
                    m_callback.mouse.x = static_cast<int>(x - getPosition().x);
                    m_callback.mouse.y = static_cast<int>(y - getPosition().y);
                    addCallback();
                }
            }
        }

        m_mouseDown = false;

        Container::leftMouseReleased(x, y);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Panel::setProperty(std::string property, const std::string& value)
    {
        property = toLower(property);

        if (property == "backgroundcolor")
        {
            setBackgroundColor(extractColor(value));
        }
        else if (property == "callback")
        {
            Container::setProperty(property, value);

            std::vector<sf::String> callbacks;
            decodeList(value, callbacks);

            for (auto it = callbacks.begin(); it != callbacks.end(); ++it)
            {
                if ((*it == "LeftMousePressed") || (*it == "leftmousepressed"))
                    bindCallback(LeftMousePressed);
                else if ((*it == "LeftMouseReleased") || (*it == "leftmousereleased"))
                    bindCallback(LeftMouseReleased);
                else if ((*it == "LeftMouseClicked") || (*it == "leftmouseclicked"))
                    bindCallback(LeftMouseClicked);
            }
        }
        else // The property didn't match
            Container::setProperty(property, value);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Panel::getProperty(std::string property, std::string& value) const
    {
        property = toLower(property);

        if (property == "backgroundcolor")
            value = "(" + tgui::to_string(int(getBackgroundColor().r)) + "," + tgui::to_string(int(getBackgroundColor().g)) + "," + tgui::to_string(int(getBackgroundColor().b)) + "," + tgui::to_string(int(getBackgroundColor().a)) + ")";
        else if (property == "callback")
        {
            std::string tempValue;
            Container::getProperty(property, tempValue);

            std::vector<sf::String> callbacks;

            if ((m_callbackFunctions.find(LeftMousePressed) != m_callbackFunctions.end()) && (m_callbackFunctions.at(LeftMousePressed).size() == 1) && (m_callbackFunctions.at(LeftMousePressed).front() == nullptr))
                callbacks.push_back("LeftMousePressed");
            if ((m_callbackFunctions.find(LeftMouseReleased) != m_callbackFunctions.end()) && (m_callbackFunctions.at(LeftMouseReleased).size() == 1) && (m_callbackFunctions.at(LeftMouseReleased).front() == nullptr))
                callbacks.push_back("LeftMouseReleased");
            if ((m_callbackFunctions.find(LeftMouseClicked) != m_callbackFunctions.end()) && (m_callbackFunctions.at(LeftMouseClicked).size() == 1) && (m_callbackFunctions.at(LeftMouseClicked).front() == nullptr))
                callbacks.push_back("LeftMouseClicked");

            encodeList(callbacks, value);

            if (value.empty() || tempValue.empty())
                value += tempValue;
            else
                value += "," + tempValue;
        }
        else // The property didn't match
            Container::getProperty(property, value);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::list< std::pair<std::string, std::string> > Panel::getPropertyList() const
    {
        auto list = Container::getPropertyList();
        list.push_back(std::pair<std::string, std::string>("BackgroundColor", "color"));
        return list;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Panel::draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        const sf::View& view = target.getView();

        // Calculate the scale factor of the view
        float scaleViewX = target.getSize().x / view.getSize().x;
        float scaleViewY = target.getSize().y / view.getSize().y;

        // Get the global position
        sf::Vector2f topLeftPosition = sf::Vector2f(((getAbsolutePosition().x - view.getCenter().x + (view.getSize().x / 2.f)) * view.getViewport().width) + (view.getSize().x * view.getViewport().left),
                                                    ((getAbsolutePosition().y - view.getCenter().y + (view.getSize().y / 2.f)) * view.getViewport().height) + (view.getSize().y * view.getViewport().top));
        sf::Vector2f bottomRightPosition = sf::Vector2f((getAbsolutePosition().x + m_size.x - view.getCenter().x + (view.getSize().x / 2.f)) * view.getViewport().width + (view.getSize().x * view.getViewport().left),
                                                        (getAbsolutePosition().y + m_size.y - view.getCenter().y + (view.getSize().y / 2.f)) * view.getViewport().height + (view.getSize().y * view.getViewport().top));

        // Get the old clipping area
        GLint scissor[4];
        glGetIntegerv(GL_SCISSOR_BOX, scissor);

        // Calculate the clipping area
        GLint scissorLeft = TGUI_MAXIMUM(static_cast<GLint>(topLeftPosition.x * scaleViewX), scissor[0]);
        GLint scissorTop = TGUI_MAXIMUM(static_cast<GLint>(topLeftPosition.y * scaleViewY), static_cast<GLint>(target.getSize().y) - scissor[1] - scissor[3]);
        GLint scissorRight = TGUI_MINIMUM(static_cast<GLint>(bottomRightPosition.x * scaleViewX), scissor[0] + scissor[2]);
        GLint scissorBottom = TGUI_MINIMUM(static_cast<GLint>(bottomRightPosition.y * scaleViewY), static_cast<GLint>(target.getSize().y) - scissor[1]);

        // If the widget outside the window then don't draw anything
        if (scissorRight < scissorLeft)
            scissorRight = scissorLeft;
        else if (scissorBottom < scissorTop)
            scissorTop = scissorBottom;

        // Set the clipping area
        glScissor(scissorLeft, target.getSize().y - scissorBottom, scissorRight - scissorLeft, scissorBottom - scissorTop);

        // Set the transform
        states.transform *= getTransform();

        // Draw the background
        if (m_backgroundColor != sf::Color::Transparent)
        {
            sf::RectangleShape background(m_size);
            background.setFillColor(m_backgroundColor);
            target.draw(background, states);
        }

        // Draw the background texture if there is one
        if (m_texture)
            target.draw(m_sprite, states);

        // Draw the widgets
        drawWidgetContainer(&target, states);

        // Reset the old clipping area
        glScissor(scissor[0], scissor[1], scissor[2], scissor[3]);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
