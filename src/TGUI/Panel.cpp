/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus's Graphical User Interface
// Copyright (C) 2012-2015 Bruno Van de Velde (vdv_b@tgui.eu)
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

#include <TGUI/Panel.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Panel::Panel()
    {
        m_callback.widgetType = WidgetType::Panel;

        addSignal<sf::Vector2f>("MousePressed");
        addSignal<sf::Vector2f>("MouseReleased");
        addSignal<sf::Vector2f>("Clicked");
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Panel::Ptr Panel::create(const sf::Vector2f& size)
    {
        auto panel = std::make_shared<Panel>();

        panel->setSize(size);

        return panel;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Panel::Ptr Panel::copy(Panel::ConstPtr panel)
    {
        if (panel)
            return std::make_shared<Panel>(*panel);
        else
            return nullptr;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Panel::setTransparency(unsigned char transparency)
    {
        Container::setTransparency(transparency);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Panel::mouseOnWidget(float x, float y)
    {
        // Check if the mouse is inside the panel
        if (sf::FloatRect{getPosition().x, getPosition().y, getSize().x, getSize().y}.contains(x, y))
        {
            m_mouseHover = true;
            return true;
        }

        if (m_mouseHover)
        {
            mouseLeftWidget();

            // Tell the widgets inside the panel that the mouse is no longer on top of them
            for (unsigned int i = 0; i < m_widgets.size(); ++i)
                m_widgets[i]->mouseNotOnWidget();
        }

        return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Panel::leftMousePressed(float x, float y)
    {
        if (mouseOnWidget(x, y))
        {
            m_mouseDown = true;

            m_callback.mouse.x = static_cast<int>(x - getPosition().x);
            m_callback.mouse.y = static_cast<int>(y - getPosition().y);
            sendSignal("MousePressed", sf::Vector2f{x - getPosition().x, y - getPosition().y});
        }

        Container::leftMousePressed(x, y);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Panel::leftMouseReleased(float x , float y)
    {
        if (mouseOnWidget(x, y))
        {
            m_callback.mouse.x = static_cast<int>(x - getPosition().x);
            m_callback.mouse.y = static_cast<int>(y - getPosition().y);
            sendSignal("MouseReleased", sf::Vector2f{x - getPosition().x, y - getPosition().y});

            if (m_mouseDown)
                sendSignal("Clicked", sf::Vector2f{x - getPosition().x, y - getPosition().y});
        }

        m_mouseDown = false;

        Container::leftMouseReleased(x, y);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Panel::draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
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

        // Set the position
        states.transform.translate(getPosition());

        // Draw the background
        if (m_backgroundColor != sf::Color::Transparent)
        {
            sf::RectangleShape background(getSize());
            background.setFillColor(m_backgroundColor);
            target.draw(background, states);
        }

        // Draw the widgets
        drawWidgetContainer(&target, states);

        // Reset the old clipping area
        glScissor(scissor[0], scissor[1], scissor[2], scissor[3]);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
