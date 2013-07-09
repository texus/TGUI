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
#include <TGUI/ContainerWidget.hpp>
#include <TGUI/Panel.hpp>

#include <SFML/OpenGL.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Panel::Panel() :
    m_Size                         (100, 100),
    m_BackgroundColor              (sf::Color::Transparent),
    m_Texture                      (NULL)
    {
        m_Callback.widgetType = Type_Panel;
        m_AllowFocus = true;
        m_Loaded = true;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Panel::Panel(const Panel& panelToCopy) :
    ContainerWidget  (panelToCopy),
    m_Size           (panelToCopy.m_Size),
    m_BackgroundColor(panelToCopy.m_BackgroundColor),
    m_Texture        (panelToCopy.m_Texture)
    {
        if (m_Texture)
        {
            m_Sprite.setTexture(*m_Texture);
            m_Sprite.setScale(m_Size.x / m_Texture->getSize().x, m_Size.y / m_Texture->getSize().y);
            m_Sprite.setColor(sf::Color(255, 255, 255, m_Opacity));
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Panel::~Panel()
    {
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Panel& Panel::operator= (const Panel& right)
    {
        // Make sure it is not the same widget
        if (this != &right)
        {
            Panel temp(right);
            this->ContainerWidget::operator=(right);

            std::swap(m_Size,                          temp.m_Size);
            std::swap(m_BackgroundColor,               temp.m_BackgroundColor);
            std::swap(m_Texture,                       temp.m_Texture);
            std::swap(m_Sprite,                        temp.m_Sprite);
        }

        return *this;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Panel* Panel::clone()
    {
        return new Panel(*this);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Panel::setSize(float width, float height)
    {
        // A negative size is not allowed for this widget
        if (width  < 0) width  = -width;
        if (height < 0) height = -height;

        // Set the size of the panel
        m_Size.x = width;
        m_Size.y = height;

        // If there is a background texture then resize it
        if (m_Texture)
            m_Sprite.setScale(m_Size.x / m_Texture->getSize().x, m_Size.y / m_Texture->getSize().y);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Vector2f Panel::getSize() const
    {
        return Vector2f(m_Size.x, m_Size.y);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Panel::setBackgroundTexture(sf::Texture *const texture)
    {
        // Store the texture
        m_Texture = texture;

        // Set the texture for the sprite
        if (m_Texture)
        {
            m_Sprite.setTexture(*m_Texture, true);
            m_Sprite.setScale(m_Size.x / m_Texture->getSize().x, m_Size.y / m_Texture->getSize().y);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    sf::Texture* Panel::getBackgroundTexture()
    {
        return m_Texture;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Panel::setBackgroundColor(const sf::Color& backgroundColor)
    {
        m_BackgroundColor = backgroundColor;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const sf::Color& Panel::getBackgroundColor() const
    {
        return m_BackgroundColor;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Panel::setTransparency(unsigned char transparency)
    {
        ContainerWidget::setTransparency(transparency);

        m_BackgroundColor.a = m_Opacity;

        m_Sprite.setColor(sf::Color(255, 255, 255, m_Opacity));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Vector2f Panel::getDisplaySize()
    {
        return m_Size;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Panel::mouseOnWidget(float x, float y)
    {
        // Don't continue when the panel has not been loaded yet
        if (m_Loaded == false)
            return false;

        // Check if the mouse is inside the panel
        if (getTransform().transformRect(sf::FloatRect(0, 0, m_Size.x, m_Size.y)).contains(x, y))
            return true;
        else
        {
            if (m_MouseHover)
                mouseLeftWidget();

            // Tell the widgets inside the panel that the mouse is no longer on top of them
            m_EventManager.mouseNotOnWidget();
            m_MouseHover = false;
            return false;
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Panel::widgetFocused()
    {
        m_EventManager.tabKeyPressed();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Panel::widgetUnfocused()
    {
        m_EventManager.unfocusAllWidgets();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Panel::handleEvent(sf::Event& event, float mouseX, float mouseY)
    {
        // Adjust the mouse position of the event when the event is about the mouse
        if (event.type == sf::Event::MouseMoved)
        {
            event.mouseMove.x = static_cast<int>(mouseX - getPosition().x);
            event.mouseMove.y = static_cast<int>(mouseY - getPosition().y);
        }
        else if (event.type == sf::Event::MouseButtonPressed)
        {
            if (mouseOnWidget(event.mouseButton.x, event.mouseButton.y))
            {
                m_MouseDown = true;

                if (!m_CallbackFunctions[LeftMousePressed].empty())
                {
                    m_Callback.trigger = LeftMousePressed;
                    m_Callback.mouse.x = mouseX - getPosition().x;
                    m_Callback.mouse.y = mouseY - getPosition().y;
                    addCallback();
                }
            }

            event.mouseButton.x = static_cast<int>(mouseX - getPosition().x);
            event.mouseButton.y = static_cast<int>(mouseY - getPosition().y);
        }
        else if (event.type == sf::Event::MouseButtonReleased)
        {
            if (mouseOnWidget(event.mouseButton.x, event.mouseButton.y))
            {
                if (!m_CallbackFunctions[LeftMouseReleased].empty())
                {
                    m_Callback.trigger = LeftMouseReleased;
                    m_Callback.mouse.x = mouseX - getPosition().x;
                    m_Callback.mouse.y = mouseY - getPosition().y;
                    addCallback();
                }

                if (m_MouseDown)
                {
                    if (!m_CallbackFunctions[LeftMouseClicked].empty())
                    {
                        m_Callback.trigger = LeftMouseClicked;
                        m_Callback.mouse.x = mouseX - getPosition().x;
                        m_Callback.mouse.y = mouseY - getPosition().y;
                        addCallback();
                    }
                }
            }

            m_MouseDown = false;

            event.mouseButton.x = static_cast<int>(mouseX - getPosition().x);
            event.mouseButton.y = static_cast<int>(mouseY - getPosition().y);
        }
        else if (event.type == sf::Event::MouseWheelMoved)
        {
            event.mouseWheel.x = static_cast<int>(mouseX - getPosition().x);
            event.mouseWheel.y = static_cast<int>(mouseY - getPosition().y);
        }

        // Let the event manager handle the event
        m_EventManager.handleEvent(event);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Panel::draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        // Don't draw when the texture wasn't created
        if (m_Loaded == false)
            return;

        // Calculate the scale factor of the view
        float scaleViewX = target.getSize().x / target.getView().getSize().x;
        float scaleViewY = target.getSize().y / target.getView().getSize().y;

        // Get the global position
        Vector2f topLeftPosition = states.transform.transformPoint(getPosition() - target.getView().getCenter() + (target.getView().getSize() / 2.f));
        Vector2f bottomRightPosition = states.transform.transformPoint(getPosition() + m_Size - target.getView().getCenter() + (target.getView().getSize() / 2.f));

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
        if (m_BackgroundColor != sf::Color::Transparent)
        {
            sf::RectangleShape background(m_Size);
            background.setFillColor(m_BackgroundColor);
            target.draw(background, states);
        }

        // Draw the background texture if there is one
        if (m_Texture)
            target.draw(m_Sprite, states);

        // Draw the widgets
        drawWidgetContainer(&target, states);

        // Reset the old clipping area
        glScissor(scissor[0], scissor[1], scissor[2], scissor[3]);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
