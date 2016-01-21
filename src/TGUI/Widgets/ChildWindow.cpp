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


#include <TGUI/Widgets/ChildWindow.hpp>
#include <TGUI/Loading/Theme.hpp>

#include <SFML/OpenGL.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ChildWindow::ChildWindow()
    {
        m_callback.widgetType = "ChildWindow";

        addSignal<sf::Vector2f>("MousePressed");
        addSignal<ChildWindow::Ptr>("Closed");

        m_renderer = std::make_shared<ChildWindowRenderer>(this);
        reload();

        setSize(400, 300);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ChildWindow::Ptr ChildWindow::copy(ChildWindow::ConstPtr childWindow)
    {
        if (childWindow)
            return std::static_pointer_cast<ChildWindow>(childWindow->clone());
        else
            return nullptr;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChildWindow::setPosition(const Layout2d& position)
    {
        float x = position.getValue().x;
        float y = position.getValue().y;

        if (m_parent && m_keepInParent)
        {
            if (y < 0)
                y = 0;
            else if (y > m_parent->getSize().y - getRenderer()->m_titleBarHeight)
                y = m_parent->getSize().y - getRenderer()->m_titleBarHeight;

            if (x < 0)
                x = 0;
            else if (x > m_parent->getSize().x - getSize().x)
                x = m_parent->getSize().x - getSize().x;
        }

        Widget::setPosition({x, y});

        getRenderer()->m_textureTitleBar.setPosition({x, y});
        m_iconTexture.setPosition(x + getRenderer()->m_distanceToSide, y + ((getRenderer()->m_titleBarHeight - m_iconTexture.getSize().y) / 2.0f));

        if (m_titleAlignment == TitleAlignment::Left)
        {
            if (m_iconTexture.isLoaded())
                m_titleText.setPosition(x + 2*getRenderer()->m_distanceToSide + m_iconTexture.getSize().x,
                                        y + ((getRenderer()->m_titleBarHeight - m_titleText.getSize().y) / 2.0f));
            else
                m_titleText.setPosition(x + getRenderer()->m_distanceToSide, y + ((getRenderer()->m_titleBarHeight - m_titleText.getSize().y) / 2.0f));

        }
        else if (m_titleAlignment == TitleAlignment::Center)
        {
            if (m_iconTexture.isLoaded())
                m_titleText.setPosition(x + (2 * getRenderer()->m_distanceToSide) + m_iconTexture.getSize().x
                                        + (((getSize().x + getRenderer()->m_borders.left + getRenderer()->m_borders.right) - (4 * getRenderer()->m_distanceToSide) - m_iconTexture.getSize().x - m_closeButton->getSize().x - m_titleText.getSize().x) / 2.0f),
                                        y + ((getRenderer()->m_titleBarHeight - m_titleText.getSize().y) / 2.0f));
            else
                m_titleText.setPosition(x + getRenderer()->m_distanceToSide + (((getSize().x + getRenderer()->m_borders.left + getRenderer()->m_borders.right) - (3 * getRenderer()->m_distanceToSide) - m_closeButton->getSize().x - m_titleText.getSize().x) / 2.0f),
                                        y + ((getRenderer()->m_titleBarHeight - m_titleText.getSize().y) / 2.0f));
        }
        else // if (m_titleAlignment == TitleAlignmentRight)
        {
            if (m_iconTexture.isLoaded())
                m_titleText.setPosition(x + (getSize().x + getRenderer()->m_borders.left + getRenderer()->m_borders.right) - (2 * getRenderer()->m_distanceToSide) - m_closeButton->getSize().x - m_titleText.getSize().x,
                                        y + ((getRenderer()->m_titleBarHeight - m_titleText.getSize().y) / 2.0f));
            else
                m_titleText.setPosition(x + (getSize().x + getRenderer()->m_borders.left + getRenderer()->m_borders.right) - (2 * getRenderer()->m_distanceToSide) - m_closeButton->getSize().x - m_titleText.getSize().x,
                                        y + ((getRenderer()->m_titleBarHeight - m_titleText.getSize().y) / 2.0f));
        }

        m_closeButton->setPosition({x + (getSize().x + getRenderer()->m_borders.left + getRenderer()->m_borders.right) - getRenderer()->m_distanceToSide - m_closeButton->getSize().x,
                                    y + ((getRenderer()->m_titleBarHeight - m_closeButton->getSize().y) / 2.0f)});
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChildWindow::setSize(const Layout2d& size)
    {
        Widget::setSize(size);

        if (getRenderer()->m_textureTitleBar.isLoaded())
        {
            getRenderer()->m_textureTitleBar.setSize({getSize().x + getRenderer()->m_borders.left + getRenderer()->m_borders.right, getRenderer()->m_titleBarHeight});

            // If there is an icon then give it the correct size
            if (m_iconTexture.isLoaded())
            {
                m_iconTexture.setSize({getRenderer()->m_titleBarHeight / getRenderer()->m_textureTitleBar.getImageSize().y * m_iconTexture.getImageSize().x,
                                       getRenderer()->m_titleBarHeight / getRenderer()->m_textureTitleBar.getImageSize().y * m_iconTexture.getImageSize().y});
            }
        }
        else
        {
            // If there is an icon then give it the correct size
            if (m_iconTexture.isLoaded())
                m_iconTexture.setSize({((getRenderer()->m_titleBarHeight * 0.8f) / m_iconTexture.getImageSize().y) * m_iconTexture.getImageSize().x, getRenderer()->m_titleBarHeight * 0.8f});
        }

        // Reposition the images and text
        updatePosition();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    sf::Vector2f ChildWindow::getFullSize() const
    {
        return {getSize().x + getRenderer()->m_borders.left + getRenderer()->m_borders.right,
                getSize().y + getRenderer()->m_borders.top + getRenderer()->m_borders.bottom + getRenderer()->m_titleBarHeight};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChildWindow::setFont(const Font& font)
    {
        Container::setFont(font);

        m_closeButton->setFont(getFont());
        m_titleText.setFont(getFont());
        m_titleText.setTextSize(findBestTextSize(getFont(), getRenderer()->m_titleBarHeight * 0.85f));

        updatePosition();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChildWindow::setOpacity(float opacity)
    {
        Container::setOpacity(opacity);

        m_closeButton->setOpacity(m_opacity);

        m_iconTexture.setColor({255, 255, 255, static_cast<sf::Uint8>(m_opacity * 255)});
        getRenderer()->m_textureTitleBar.setColor({255, 255, 255, static_cast<sf::Uint8>(m_opacity * 255)});

        m_titleText.setTextColor(calcColorOpacity(getRenderer()->m_titleColor, getOpacity()));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChildWindow::setTitle(const sf::String& title)
    {
        m_titleText.setText(title);

        // Reposition the images and text
        updatePosition();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChildWindow::setTitleAlignment(TitleAlignment alignment)
    {
        m_titleAlignment = alignment;

        // Reposition the images and text
        updatePosition();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChildWindow::setIcon(const Texture& icon)
    {
        m_iconTexture = icon;
        updateSize();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const Texture& ChildWindow::getIcon()
    {
        return m_iconTexture;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChildWindow::destroy()
    {
        m_parent->remove(shared_from_this());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChildWindow::setResizable(bool resizable)
    {
        m_resizable = resizable;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ChildWindow::isResizable() const
    {
        return m_resizable;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChildWindow::keepInParent(bool enabled)
    {
        m_keepInParent = enabled;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ChildWindow::isKeptInParent() const
    {
        return m_keepInParent;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChildWindow::setCloseButton(Button::Ptr closeButton)
    {
        m_closeButton = closeButton;
        updatePosition();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Button::Ptr ChildWindow::getCloseButton() const
    {
        return m_closeButton;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    sf::Vector2f ChildWindow::getChildWidgetsOffset() const
    {
        return {getRenderer()->m_borders.left, getRenderer()->m_borders.top + getRenderer()->m_titleBarHeight};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ChildWindow::mouseOnWidget(float x, float y) const
    {
        // Check if the mouse is on top of the title bar
        if (sf::FloatRect{getPosition().x, getPosition().y, getSize().x + getRenderer()->m_borders.left + getRenderer()->m_borders.right, getRenderer()->m_titleBarHeight}.contains(x, y))
        {
            if (m_widgetBelowMouse)
                m_widgetBelowMouse->mouseNoLongerOnWidget();

            return true;
        }

        // Check if the mouse is inside the child window
        return sf::FloatRect{getPosition().x, getPosition().y, getSize().x + getRenderer()->m_borders.left + getRenderer()->m_borders.right,
                             getSize().y + getRenderer()->m_borders.top + getRenderer()->m_borders.bottom}.contains(x, y - getRenderer()->m_titleBarHeight);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChildWindow::leftMousePressed(float x, float y)
    {
        m_mouseDown = true;

        // Move the child window to the front
        moveToFront();

        m_callback.mouse.x = static_cast<int>(x - getPosition().x);
        m_callback.mouse.y = static_cast<int>(y - getPosition().y);
        sendSignal("MousePressed", sf::Vector2f{x - getPosition().x, y - getPosition().y});

        // Check if the mouse is on top of the title bar
        if (sf::FloatRect{getPosition().x, getPosition().y, getSize().x + getRenderer()->getBorders().left + getRenderer()->getBorders().right, getRenderer()->m_titleBarHeight}.contains(x, y))
        {
            // Send the mouse press event to the close button
            if (m_closeButton->mouseOnWidget(x, y))
                m_closeButton->leftMousePressed(x, y);
            else
            {
                // The mouse went down on the title bar
                m_mouseDownOnTitleBar = true;

                // Remember where we are dragging the title bar
                m_draggingPosition.x = x - getPosition().x;
                m_draggingPosition.y = y - getPosition().y;
            }

            return;
        }
        else // The mouse is not on top of the title bar
        {
            // When the mouse is not on the title bar, the mouse can't be on the close button
            if (m_closeButton->m_mouseHover)
                m_closeButton->mouseNoLongerOnWidget();

            // Check if the mouse is on top of the borders
            if ((sf::FloatRect{getPosition().x, getPosition().y, getSize().x + getRenderer()->getBorders().left + getRenderer()->getBorders().right,
                               getSize().y + getRenderer()->getBorders().top + getRenderer()->getBorders().bottom + getRenderer()->m_titleBarHeight}.contains(x, y))
             && (!sf::FloatRect{getPosition().x + getRenderer()->getBorders().left, getPosition().y + getRenderer()->m_titleBarHeight + getRenderer()->getBorders().top,
                                getSize().x, getSize().y}.contains(x, y)))
            {
                // Check if you start resizing the child window
                if (m_resizable)
                {
                    // Check on which border the mouse is standing
                    if (sf::FloatRect{getPosition().x, getPosition().y + getRenderer()->m_titleBarHeight + getRenderer()->getBorders().top + getSize().y,
                                      getRenderer()->getBorders().left, getRenderer()->getBorders().bottom}.contains(x, y))
                    {
                        m_resizeDirection = ResizeLeft | ResizeBottom;
                    }
                    else if (sf::FloatRect{getPosition().x + getRenderer()->getBorders().left + getSize().x, getPosition().y + getRenderer()->m_titleBarHeight + getRenderer()->getBorders().top + getSize().y,
                                      getRenderer()->getBorders().right, getRenderer()->getBorders().bottom}.contains(x, y))
                    {
                        m_resizeDirection = ResizeRight | ResizeBottom;
                    }
                    else if (sf::FloatRect{getPosition().x, getPosition().y + getRenderer()->m_titleBarHeight,
                                      getRenderer()->getBorders().left, getRenderer()->getBorders().top + getSize().y}.contains(x, y))
                    {
                        m_resizeDirection = ResizeLeft;
                    }
                    else if (sf::FloatRect{getPosition().x + getRenderer()->getBorders().left + getSize().x, getPosition().y + getRenderer()->m_titleBarHeight,
                                      getRenderer()->getBorders().right, getRenderer()->getBorders().top + getSize().y}.contains(x, y))
                    {
                        m_resizeDirection = ResizeRight;
                    }
                    else if (sf::FloatRect{getPosition().x + getRenderer()->getBorders().left, getPosition().y + getRenderer()->m_titleBarHeight + getRenderer()->getBorders().top + getSize().y,
                                           getSize().x, getRenderer()->getBorders().bottom}.contains(x, y))
                    {
                        m_resizeDirection = ResizeBottom;
                    }
                }

                // Don't send the event to the widgets
                return;
            }
        }

        Container::leftMousePressed(x - getRenderer()->getBorders().left, y - (getRenderer()->m_titleBarHeight + getRenderer()->getBorders().top));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChildWindow::leftMouseReleased(float x , float y)
    {
        m_mouseDownOnTitleBar = false;
        m_resizeDirection = ResizeNone;

        // Check if the mouse is on top of the title bar
        if (sf::FloatRect{getPosition().x, getPosition().y, getSize().x + getRenderer()->getBorders().left + getRenderer()->getBorders().right, getRenderer()->m_titleBarHeight}.contains(x, y))
        {
            // Check if the close button was clicked
            if (m_closeButton->m_mouseDown)
            {
                // Check if the mouse is still on the close button
                if (m_closeButton->mouseOnWidget(x, y))
                {
                    if (isSignalBound("closed"))
                        sendSignal("closed", std::static_pointer_cast<ChildWindow>(shared_from_this()));
                    else // The user won't stop the closing, so destroy the window
                    {
                        destroy();
                        return;
                    }
                }
            }

            return;
        }
        else // The mouse is not on top of the title bar
        {
            // When the mouse is not on the title bar, the mouse can't be on the close button
            if (m_closeButton->m_mouseHover)
                m_closeButton->mouseNoLongerOnWidget();

            m_closeButton->mouseNoLongerDown();

            // Check if the mouse is on top of the borders
            if ((sf::FloatRect{getPosition().x, getPosition().y, getSize().x + getRenderer()->m_borders.left + getRenderer()->m_borders.right,
                               getSize().y + getRenderer()->m_borders.top + getRenderer()->m_borders.bottom + getRenderer()->m_titleBarHeight}.contains(x, y))
             && (!sf::FloatRect{getPosition().x + getRenderer()->m_borders.left, getPosition().y + getRenderer()->m_titleBarHeight + getRenderer()->m_borders.top,
                                getSize().x, getSize().y}.contains(x, y)))
            {
                // Tell the widgets about that the mouse was released
                for (unsigned int i = 0; i < m_widgets.size(); ++i)
                    m_widgets[i]->mouseNoLongerDown();

                // Don't send the event to the widgets
                return;
            }
        }

        Container::leftMouseReleased(x - getRenderer()->m_borders.left, y - (getRenderer()->m_titleBarHeight + getRenderer()->m_borders.top));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChildWindow::mouseMoved(float x, float y)
    {
        // Check if you are dragging the child window
        if (m_mouseDown && m_mouseDownOnTitleBar)
        {
            // Move the child window
            setPosition(sf::Vector2f{x, y} - m_draggingPosition);
        }

        // Check if you are resizing the window
        else if (m_mouseDown && m_resizeDirection != ResizeNone)
        {
            if ((m_resizeDirection & ResizeLeft) != 0)
            {
                float diff = x - getPosition().x;
                if (getSize().x - diff >= 25)
                {
                    setPosition(getPosition().x + diff, getPosition().y);
                    setSize(getSize().x - diff, getSize().y);
                }
                else
                {
                    setPosition(getPosition().x + getSize().x - 25, getPosition().y);
                    setSize(25, getSize().y);
                }
            }
            else if ((m_resizeDirection & ResizeRight) != 0)
            {
                setSize(std::max(25.0f, x - (getPosition().x + getRenderer()->m_borders.left)), getSize().y);
            }

            if ((m_resizeDirection & ResizeBottom) != 0)
            {
                setSize(getSize().x, std::max(0.f, y - (getPosition().y + getRenderer()->m_titleBarHeight + getRenderer()->m_borders.top)));
            }
        }

        // Check if the mouse is on top of the title bar
        else if (sf::FloatRect{getPosition().x, getPosition().y, getSize().x + getRenderer()->m_borders.left + getRenderer()->m_borders.right, getRenderer()->m_titleBarHeight}.contains(x, y))
        {
            // Send the hover event to the close button
            if (m_closeButton->mouseOnWidget(x, y))
                m_closeButton->mouseMoved(x, y);
            else
                m_closeButton->mouseNoLongerOnWidget();

            return;
        }
        else // The mouse is not on top of the title bar
        {
            // When the mouse is not on the title bar, the mouse can't be on the close button
            if (m_closeButton->m_mouseHover)
                m_closeButton->mouseNoLongerOnWidget();

            // Check if the mouse is on top of the borders
            if ((sf::FloatRect{getPosition().x, getPosition().y, getSize().x + getRenderer()->m_borders.left + getRenderer()->m_borders.right,
                               getSize().y + getRenderer()->m_borders.top + getRenderer()->m_borders.bottom + getRenderer()->m_titleBarHeight}.contains(x, y))
             && (!sf::FloatRect{getPosition().x + getRenderer()->m_borders.left, getPosition().y + getRenderer()->m_titleBarHeight + getRenderer()->m_borders.top,
                                getSize().x, getSize().y}.contains(x, y)))
            {
                // Don't send the event to the widgets
                return;
            }
        }

        Container::mouseMoved(x - getRenderer()->m_borders.left, y - getRenderer()->m_titleBarHeight - getRenderer()->m_borders.top);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChildWindow::mouseWheelMoved(int delta, int x, int y)
    {
        Container::mouseWheelMoved(delta,
                                   static_cast<int>(x - getRenderer()->m_borders.left),
                                   static_cast<int>(y - (getRenderer()->m_titleBarHeight + getRenderer()->m_borders.top)));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChildWindow::mouseNoLongerOnWidget()
    {
        Container::mouseNoLongerOnWidget();
        m_closeButton->mouseNoLongerOnWidget();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChildWindow::mouseNoLongerDown()
    {
        Container::mouseNoLongerDown();
        m_closeButton->mouseNoLongerDown();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChildWindow::reload(const std::string& primary, const std::string& secondary, bool force)
    {
        m_closeButton->reload();
        m_closeButton->getRenderer()->setBorders({1, 1, 1, 1});

        getRenderer()->setTitleBarHeight(20);
        getRenderer()->setDistanceToSide(3);
        getRenderer()->setBorders({1, 1, 1, 1});
        getRenderer()->setTitleBarColor({255, 255, 255});
        getRenderer()->setTitleColor({0, 0, 0});
        getRenderer()->setBackgroundColor({230, 230, 230});
        getRenderer()->setBorderColor({0, 0, 0});
        getRenderer()->setTitleBarTexture({});

        if (m_theme && primary != "")
        {
            getRenderer()->setBorders({0, 0, 0, 0});
            Widget::reload(primary, secondary, force);

            if (force)
            {
                if (getRenderer()->m_textureTitleBar.isLoaded())
                {
                    getRenderer()->m_titleBarHeight = getRenderer()->m_textureTitleBar.getImageSize().y;
                    getRenderer()->m_textureTitleBar.setSize({getRenderer()->m_textureTitleBar.getSize().x, getRenderer()->m_titleBarHeight});

                    if (m_closeButton->getRenderer()->m_textureNormal.isLoaded())
                        m_closeButton->setSize(m_closeButton->getRenderer()->m_textureNormal.getImageSize());
                }
            }
        }

        if (!m_closeButton->getRenderer()->m_textureNormal.isLoaded())
        {
            m_closeButton->setSize({getRenderer()->m_titleBarHeight * 0.8f, getRenderer()->m_titleBarHeight * 0.8f});
            m_closeButton->setText("X");
        }

        // Set the size of the title text
        m_titleText.setTextSize(findBestTextSize(getFont(), getRenderer()->m_titleBarHeight * 0.85f));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChildWindow::draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        getRenderer()->draw(target, states);

        // Draw a window icon if one was set
        if (m_iconTexture.isLoaded())
            target.draw(m_iconTexture, states);

        // Calculate the scale factor of the view
        const sf::View& view = target.getView();
        float scaleViewX = target.getSize().x / view.getSize().x;
        float scaleViewY = target.getSize().y / view.getSize().y;

        // Get the global position
        sf::Vector2f topLeftPanelPosition = {((getAbsolutePosition().x + getRenderer()->m_borders.left - view.getCenter().x + (view.getSize().x / 2.f)) * view.getViewport().width) + (view.getSize().x * view.getViewport().left),
                                             ((getAbsolutePosition().y + getRenderer()->m_titleBarHeight + getRenderer()->m_borders.top - view.getCenter().y + (view.getSize().y / 2.f)) * view.getViewport().height) + (view.getSize().y * view.getViewport().top)};
        sf::Vector2f bottomRightPanelPosition = {(getAbsolutePosition().x + getSize().x + getRenderer()->m_borders.left - view.getCenter().x + (view.getSize().x / 2.f)) * view.getViewport().width + (view.getSize().x * view.getViewport().left),
                                                 (getAbsolutePosition().y + getRenderer()->m_titleBarHeight + getSize().y + getRenderer()->m_borders.top - view.getCenter().y + (view.getSize().y / 2.f)) * view.getViewport().height + (view.getSize().y * view.getViewport().top)};

        sf::Vector2f topLeftTitleBarPosition;
        sf::Vector2f bottomRightTitleBarPosition;

        if (m_iconTexture.isLoaded())
        {
            topLeftTitleBarPosition = {((getAbsolutePosition().x + 2*getRenderer()->m_distanceToSide + m_iconTexture.getSize().x - view.getCenter().x + (view.getSize().x / 2.f)) * view.getViewport().width) + (view.getSize().x * view.getViewport().left),
                                       ((getAbsolutePosition().y - view.getCenter().y + (view.getSize().y / 2.f)) * view.getViewport().height) + (view.getSize().y * view.getViewport().top)};
        }
        else
        {
            topLeftTitleBarPosition = {((getAbsolutePosition().x + getRenderer()->m_distanceToSide - view.getCenter().x + (view.getSize().x / 2.f)) * view.getViewport().width) + (view.getSize().x * view.getViewport().left),
                                       ((getAbsolutePosition().y - view.getCenter().y + (view.getSize().y / 2.f)) * view.getViewport().height) + (view.getSize().y * view.getViewport().top)};
        }

        bottomRightTitleBarPosition = {(getAbsolutePosition().x + getSize().x + getRenderer()->m_borders.left + getRenderer()->m_borders.right - (2*getRenderer()->m_distanceToSide) - m_closeButton->getSize().x - view.getCenter().x + (view.getSize().x / 2.f))
                                       * view.getViewport().width + (view.getSize().x * view.getViewport().left),
                                       (getAbsolutePosition().y + getRenderer()->m_titleBarHeight - view.getCenter().y + (view.getSize().y / 2.f)) * view.getViewport().height + (view.getSize().y * view.getViewport().top)};

        // Get the old clipping area
        GLint scissor[4];
        glGetIntegerv(GL_SCISSOR_BOX, scissor);

        // Check if there is a title
        if (!m_titleText.getText().isEmpty())
        {
            // Calculate the clipping area
            GLint scissorLeft = std::max(static_cast<GLint>(topLeftTitleBarPosition.x * scaleViewX), scissor[0]);
            GLint scissorTop = std::max(static_cast<GLint>(topLeftTitleBarPosition.y * scaleViewY), static_cast<GLint>(target.getSize().y) - scissor[1] - scissor[3]);
            GLint scissorRight = std::min(static_cast<GLint>(bottomRightTitleBarPosition.x * scaleViewX), scissor[0] + scissor[2]);
            GLint scissorBottom = std::min(static_cast<GLint>(bottomRightTitleBarPosition.y * scaleViewY), static_cast<GLint>(target.getSize().y) - scissor[1]);

            if (scissorRight < scissorLeft)
                scissorRight = scissorLeft;
            else if (scissorBottom < scissorTop)
                scissorTop = scissorBottom;

            // Set the clipping area
            glScissor(scissorLeft, target.getSize().y - scissorBottom, scissorRight - scissorLeft, scissorBottom - scissorTop);

            // Draw the text in the title bar
            target.draw(m_titleText, states);

            // Reset the old clipping area
            glScissor(scissor[0], scissor[1], scissor[2], scissor[3]);
        }


        states.transform.translate(getPosition().x + getRenderer()->m_borders.left, getPosition().y + getRenderer()->m_titleBarHeight + getRenderer()->m_borders.top);

        // Draw the background
        if (getRenderer()->m_backgroundColor != sf::Color::Transparent)
        {
            sf::RectangleShape background(getSize());
            background.setFillColor(calcColorOpacity(getRenderer()->m_backgroundColor, getOpacity()));
            target.draw(background, states);
        }

        // Calculate the clipping area
        GLint scissorLeft = std::max(static_cast<GLint>(topLeftPanelPosition.x * scaleViewX), scissor[0]);
        GLint scissorTop = std::max(static_cast<GLint>(topLeftPanelPosition.y * scaleViewY), static_cast<GLint>(target.getSize().y) - scissor[1] - scissor[3]);
        GLint scissorRight = std::min(static_cast<GLint>(bottomRightPanelPosition.x * scaleViewX), scissor[0] + scissor[2]);
        GLint scissorBottom = std::min(static_cast<GLint>(bottomRightPanelPosition.y * scaleViewY), static_cast<GLint>(target.getSize().y) - scissor[1]);

        if (scissorRight < scissorLeft)
            scissorRight = scissorLeft;
        else if (scissorBottom < scissorTop)
            scissorTop = scissorBottom;

        // Set the clipping area
        glScissor(scissorLeft, target.getSize().y - scissorBottom, scissorRight - scissorLeft, scissorBottom - scissorTop);

        // Draw the widgets in the child window
        drawWidgetContainer(&target, states);

        // Reset the old clipping area
        glScissor(scissor[0], scissor[1], scissor[2], scissor[3]);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChildWindowRenderer::setProperty(std::string property, const std::string& value)
    {
        property = toLower(property);

        if (property == "borders")
            setBorders(Deserializer::deserialize(ObjectConverter::Type::Borders, value).getBorders());
        else if (property == "backgroundcolor")
            setBackgroundColor(Deserializer::deserialize(ObjectConverter::Type::Color, value).getColor());
        else if (property == "titlecolor")
            setTitleColor(Deserializer::deserialize(ObjectConverter::Type::Color, value).getColor());
        else if (property == "titlebarcolor")
            setTitleBarColor(Deserializer::deserialize(ObjectConverter::Type::Color, value).getColor());
        else if (property == "bordercolor")
            setBorderColor(Deserializer::deserialize(ObjectConverter::Type::Color, value).getColor());
        else if (property == "titlebarimage")
            setTitleBarTexture(Deserializer::deserialize(ObjectConverter::Type::Texture, value).getTexture());
        else if (property == "distancetoside")
            setDistanceToSide(Deserializer::deserialize(ObjectConverter::Type::Number, value).getNumber());
        else if (property == "titlebarheight")
            setTitleBarHeight(Deserializer::deserialize(ObjectConverter::Type::Number, value).getNumber());
        else if (property == "closebutton")
        {
            if (value.empty() || toLower(value) == "default")
            {
                m_closeButtonClassName = "";
                m_childWindow->m_closeButton = std::make_shared<Button>();
            }
            else
            {
                m_closeButtonClassName = Deserializer::deserialize(ObjectConverter::Type::String, value).getString();

                /// TODO: Widget files do not contain themes yet. This means that child window cannot be loaded from one.
                ///       Temporarily load default close button in case it is attempted.
                if (m_childWindow->getTheme() == nullptr)
                {
                    m_childWindow->m_closeButton = std::make_shared<Button>();
                    return;
                }

                if (m_childWindow->getTheme() == nullptr)
                    throw Exception{"Failed to load the close button, ChildWindow has no connected theme to load the close button with"};

                m_childWindow->m_closeButton = m_childWindow->getTheme()->internalLoad(
                                                    m_childWindow->getPrimaryLoadingParameter(),
                                                    Deserializer::deserialize(ObjectConverter::Type::String, value).getString()
                                               );
            }
        }
        else
            WidgetRenderer::setProperty(property, value);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChildWindowRenderer::setProperty(std::string property, ObjectConverter&& value)
    {
        property = toLower(property);

        if (value.getType() == ObjectConverter::Type::Borders)
        {
            if (property == "borders")
                setBorders(value.getBorders());
            else
                return WidgetRenderer::setProperty(property, std::move(value));
        }
        else if (value.getType() == ObjectConverter::Type::Color)
        {
            if (property == "backgroundcolor")
                setBackgroundColor(value.getColor());
            else if (property == "titlecolor")
                setTitleColor(value.getColor());
            else if (property == "titlebarcolor")
                setTitleBarColor(value.getColor());
            else if (property == "bordercolor")
                setBorderColor(value.getColor());
            else
                WidgetRenderer::setProperty(property, std::move(value));
        }
        else if (value.getType() == ObjectConverter::Type::Texture)
        {
            if (property == "titlebarimage")
                setTitleBarTexture(value.getTexture());
            else
                WidgetRenderer::setProperty(property, std::move(value));
        }
        else if (value.getType() == ObjectConverter::Type::Number)
        {
            if (property == "distancetoside")
                setDistanceToSide(value.getNumber());
            else if (property == "titlebarheight")
                setTitleBarHeight(value.getNumber());
        }
        else if (value.getType() == ObjectConverter::Type::String)
        {
            if (property == "closebutton")
            {
                m_closeButtonClassName = value.getString();

                if (value.getString().isEmpty())
                    m_childWindow->m_closeButton = std::make_shared<Button>();
                else
                {
                    /// TODO: Widget files do not contain themes yet. This means that child window cannot be loaded from one.
                    ///       Temporarily load default close button in case it is attempted.
                    if (m_childWindow->getTheme() == nullptr)
                    {
                        m_childWindow->m_closeButton = std::make_shared<Button>();
                        return;
                    }

                    if (m_childWindow->getTheme() == nullptr)
                        throw Exception{"Failed to load the close button, ChildWindow has no connected theme to load the close button with"};

                    m_childWindow->m_closeButton = m_childWindow->getTheme()->internalLoad(m_childWindow->getPrimaryLoadingParameter(), value.getString());
                }
            }
        }
        else
            WidgetRenderer::setProperty(property, std::move(value));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ObjectConverter ChildWindowRenderer::getProperty(std::string property) const
    {
        property = toLower(property);

        if (property == "borders")
            return m_borders;
        else if (property == "backgroundcolor")
            return m_backgroundColor;
        else if (property == "titlecolor")
            return m_titleColor;
        else if (property == "titlebarcolor")
            return m_titleBarColor;
        else if (property == "bordercolor")
            return m_borderColor;
        else if (property == "titlebarimage")
            return m_textureTitleBar;
        else if (property == "distancetoside")
            return m_distanceToSide;
        else if (property == "titlebarheight")
            return m_titleBarHeight;
        else
            return WidgetRenderer::getProperty(property);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::map<std::string, ObjectConverter> ChildWindowRenderer::getPropertyValuePairs() const
    {
        auto pairs = WidgetRenderer::getPropertyValuePairs();

        if (m_textureTitleBar.isLoaded())
            pairs["TitleBarImage"] = m_textureTitleBar;
        else
            pairs["TitleBarColor"] = m_titleBarColor;

        pairs["BackgroundColor"] = m_backgroundColor;
        pairs["TitleColor"] = m_titleColor;
        pairs["BorderColor"] = m_borderColor;
        pairs["Borders"] = m_borders;
        pairs["DistanceToSide"] = m_distanceToSide;
        pairs["TitleBarHeight"] = m_titleBarHeight;
        return pairs;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChildWindowRenderer::setTitleBarColor(const Color& color)
    {
        m_titleBarColor = color;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChildWindowRenderer::setTitleBarHeight(float height)
    {
        m_titleBarHeight = height;

        // Set the size of the close button
        if (m_textureTitleBar.isLoaded() && m_childWindow->m_closeButton->getRenderer()->m_textureNormal.isLoaded())
        {
            m_childWindow->m_closeButton->setSize(m_childWindow->m_closeButton->getRenderer()->m_textureNormal.getImageSize().x * (height / m_textureTitleBar.getImageSize().y),
                                                  m_childWindow->m_closeButton->getRenderer()->m_textureNormal.getImageSize().y * (height / m_textureTitleBar.getImageSize().y));
        }
        else
            m_childWindow->m_closeButton->setSize({height * 0.8f, height * 0.8f});

        // Set the size of the text in the title bar
        m_childWindow->m_titleText.setTextSize(findBestTextSize(m_childWindow->getFont(), m_titleBarHeight * 0.85f));

        m_textureTitleBar.setSize({m_childWindow->getSize().x + m_borders.left + m_borders.right, m_titleBarHeight});

        // Reposition the images and text
        m_childWindow->updatePosition();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChildWindowRenderer::setTitleColor(const Color& color)
    {
        m_titleColor = color;
        m_childWindow->m_titleText.setTextColor(calcColorOpacity(m_titleColor, m_childWindow->getOpacity()));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChildWindowRenderer::setBorderColor(const Color& borderColor)
    {
        m_borderColor = borderColor;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChildWindowRenderer::setBorders(const Borders& borders)
    {
        WidgetBorders::setBorders(borders);

        m_textureTitleBar.setSize({m_childWindow->getSize().x + borders.left + borders.right, m_titleBarHeight});

        // Reposition the images and text
        m_childWindow->updatePosition();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChildWindowRenderer::setDistanceToSide(float distanceToSide)
    {
        m_distanceToSide = distanceToSide;

        // Reposition the images and text
        m_childWindow->updatePosition();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChildWindowRenderer::setBackgroundColor(const Color& backgroundColor)
    {
        m_backgroundColor = backgroundColor;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChildWindowRenderer::setTitleBarTexture(const Texture& texture)
    {
        m_textureTitleBar = texture;
        if (m_textureTitleBar.isLoaded())
        {
            m_textureTitleBar.setPosition(m_childWindow->getPosition());
            m_textureTitleBar.setSize({m_childWindow->getSize().x + m_borders.left + m_borders.right, m_titleBarHeight});
            m_textureTitleBar.setColor({255, 255, 255, static_cast<sf::Uint8>(m_childWindow->getOpacity() * 255)});
        }

        setTitleBarHeight(m_titleBarHeight);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::shared_ptr<ButtonRenderer> ChildWindowRenderer::getCloseButton() const
    {
        return m_childWindow->m_closeButton->getRenderer();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChildWindowRenderer::draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        // Draw the title bar
        if (m_textureTitleBar.isLoaded())
            target.draw(m_textureTitleBar, states);
        else
        {
            sf::RectangleShape titleBar{{m_childWindow->getSize().x + m_borders.left + m_borders.right, m_titleBarHeight}};
            titleBar.setPosition({m_childWindow->getPosition().x, m_childWindow->getPosition().y});
            titleBar.setFillColor(calcColorOpacity(m_titleBarColor, m_childWindow->getOpacity()));
            target.draw(titleBar, states);
        }

        // Draw the close button
        target.draw(*m_childWindow->m_closeButton, states);

        // Draw the borders
        if (m_borders != Borders{0, 0, 0, 0})
        {
            sf::Vector2f size = m_childWindow->getSize();
            sf::Vector2f position = {m_childWindow->getPosition().x, m_childWindow->getPosition().y + m_titleBarHeight};

            // Draw left border
            sf::RectangleShape border({m_borders.left, size.y + m_borders.top});
            border.setPosition(position.x, position.y);
            border.setFillColor(calcColorOpacity(m_borderColor, m_childWindow->getOpacity()));
            target.draw(border, states);

            // Draw top border
            border.setSize({size.x + m_borders.right, m_borders.top});
            border.setPosition(position.x + m_borders.left, position.y);
            target.draw(border, states);

            // Draw right border
            border.setSize({m_borders.right, size.y + m_borders.bottom});
            border.setPosition(position.x + size.x + m_borders.left, position.y + m_borders.top);
            target.draw(border, states);

            // Draw bottom border
            border.setSize({size.x + m_borders.left, m_borders.bottom});
            border.setPosition(position.x, position.y + size.y + m_borders.top);
            target.draw(border, states);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::shared_ptr<WidgetRenderer> ChildWindowRenderer::clone(Widget* widget)
    {
        auto renderer = std::make_shared<ChildWindowRenderer>(*this);
        renderer->m_childWindow = static_cast<ChildWindow*>(widget);
        return renderer;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
