/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus's Graphical User Interface
// Copyright (C) 2012-2017 Bruno Van de Velde (vdv_b@tgui.eu)
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
#include <TGUI/Clipping.hpp>

#include <limits>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace
{
    float clamp(float value, float lower, float upper)
    {
        if (value < lower)
            return lower;

        if (value > upper)
            return upper;

        return value;
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ChildWindow::ChildWindow()
    {
        m_callback.widgetType = "ChildWindow";

        addSignal<sf::Vector2f>("MousePressed");
        addSignal<ChildWindow::Ptr>("Closed");
        addSignal<ChildWindow::Ptr>("Minimized");
        addSignal<ChildWindow::Ptr>("Maximized");

        m_minimizeButton->hide();
        m_maximizeButton->hide();

        m_renderer = std::make_shared<ChildWindowRenderer>(this);
        reload();

        setSize(400, 300);
        m_minimumSize = {0, 0};
        m_maximumSize = {std::numeric_limits<float>::infinity(), std::numeric_limits<float>::infinity()};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ChildWindow::Ptr ChildWindow::create()
    {
        return std::make_shared<ChildWindow>();
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

        // Calculate the distance from the right side that the buttons will need
        float buttonOffsetX = 0;
        for (const auto& button : {m_closeButton, m_maximizeButton, m_minimizeButton})
        {
            if (button->isVisible())
                buttonOffsetX += (buttonOffsetX > 0 ? getRenderer()->m_paddingBetweenButtons : 0) + button->getSize().x;
        }

        if (buttonOffsetX > 0)
            buttonOffsetX += getRenderer()->m_distanceToSide;

        if (m_titleAlignment == TitleAlignment::Left)
        {
            m_titleText.setPosition(x + getRenderer()->m_distanceToSide, y + ((getRenderer()->m_titleBarHeight - m_titleText.getSize().y) / 2.0f));
            if (m_iconTexture.isLoaded())
                m_titleText.move(m_iconTexture.getSize().x + getRenderer()->m_distanceToSide, 0);
        }
        else if (m_titleAlignment == TitleAlignment::Center)
        {
            if (m_iconTexture.isLoaded())
                m_titleText.setPosition(x + (2 * getRenderer()->m_distanceToSide) + m_iconTexture.getSize().x
                                        + (((getSize().x + getRenderer()->m_borders.left + getRenderer()->m_borders.right) - (3 * getRenderer()->m_distanceToSide) - m_iconTexture.getSize().x - buttonOffsetX - m_titleText.getSize().x) / 2.0f),
                                        y + ((getRenderer()->m_titleBarHeight - m_titleText.getSize().y) / 2.0f));
            else
                m_titleText.setPosition(x + getRenderer()->m_distanceToSide + (((getSize().x + getRenderer()->m_borders.left + getRenderer()->m_borders.right) - (2 * getRenderer()->m_distanceToSide) - buttonOffsetX - m_titleText.getSize().x) / 2.0f),
                                        y + ((getRenderer()->m_titleBarHeight - m_titleText.getSize().y) / 2.0f));
        }
        else // if (m_titleAlignment == TitleAlignment::Right)
        {
            m_titleText.setPosition(x + (getSize().x + getRenderer()->m_borders.left + getRenderer()->m_borders.right) - getRenderer()->m_distanceToSide - buttonOffsetX - m_titleText.getSize().x,
                                    y + ((getRenderer()->m_titleBarHeight - m_titleText.getSize().y) / 2.0f));
        }

        buttonOffsetX = getRenderer()->m_distanceToSide;
        for (auto& button : {m_closeButton, m_maximizeButton, m_minimizeButton})
        {
            if (button->isVisible())
            {
                button->setPosition({x + (getSize().x + getRenderer()->m_borders.left + getRenderer()->m_borders.right) - buttonOffsetX - button->getSize().x,
                                     y + ((getRenderer()->m_titleBarHeight - button->getSize().y) / 2.0f)});

                buttonOffsetX += button->getSize().x + getRenderer()->m_paddingBetweenButtons;
            }
        }
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

    void ChildWindow::setMaximumSize(sf::Vector2f size)
    {
        if ((size.x < getSize().x) || (size.y < getSize().y))
        {
            // The window is currently larger than the new maximum size, lets downsize
            setSize(std::min(size.x, getSize().x), std::min(size.y, getSize().y));
        }

        m_maximumSize = size;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    sf::Vector2f ChildWindow::getMaximumSize() const
    {
        return m_maximumSize;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChildWindow::setMinimumSize(sf::Vector2f size)
    {
        if ((size.x > getSize().x) || (size.y > getSize().y))
        {
            // The window is currently smaller than the new minimum size, lets upsize
            setSize(std::max(size.x, getSize().x), std::max(size.y, getSize().y));
        }

        m_minimumSize = size;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    sf::Vector2f ChildWindow::getMinimumSize() const
    {
        return m_minimumSize;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChildWindow::setFont(const Font& font)
    {
        Container::setFont(font);

        m_closeButton->setFont(getFont());
        m_maximizeButton->setFont(getFont());
        m_minimizeButton->setFont(getFont());
        m_titleText.setFont(getFont());
        m_titleText.setTextSize(findBestTextSize(getFont(), getRenderer()->m_titleBarHeight * 0.85f));

        updatePosition();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChildWindow::setOpacity(float opacity)
    {
        Container::setOpacity(opacity);

        m_closeButton->setOpacity(m_opacity);
        m_maximizeButton->setOpacity(m_opacity);
        m_minimizeButton->setOpacity(m_opacity);

        m_iconTexture.setColor({m_iconTexture.getColor().r, m_iconTexture.getColor().g, m_iconTexture.getColor().b, static_cast<sf::Uint8>(m_opacity * 255)});
        getRenderer()->m_textureTitleBar.setColor({getRenderer()->m_textureTitleBar.getColor().r, getRenderer()->m_textureTitleBar.getColor().g, getRenderer()->m_textureTitleBar.getColor().b, static_cast<sf::Uint8>(m_opacity * 255)});

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

    void ChildWindow::setTitleButtonsText(const sf::String& closeText, const sf::String& minimizeText, const sf::String& maximizeText)
    {
        m_closeButtonText = closeText;
        m_minimizeButtonText = minimizeText;
        m_maximizeButtonText = maximizeText;

        m_closeButton->setText(m_closeButtonText);
        m_minimizeButton->setText(m_minimizeButtonText);
        m_maximizeButton->setText(m_maximizeButtonText);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChildWindow::setTitleAlignment(TitleAlignment alignment)
    {
        m_titleAlignment = alignment;

        // Reposition the images and text
        updatePosition();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChildWindow::setTitleButtons(TitleButtons buttons)
    {
        m_titleButtons = buttons;

        //Reset the visibility of the buttons given the new input
        if (m_titleButtons & TitleButtons::Close)
            m_closeButton->show();
        else
            m_closeButton->hide();

        if (m_titleButtons & TitleButtons::Minimize)
            m_minimizeButton->show();
        else
            m_minimizeButton->hide();

        if (m_titleButtons & TitleButtons::Maximize)
            m_maximizeButton->show();
        else
            m_maximizeButton->hide();
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

    void ChildWindow::setMinimizeButton(Button::Ptr minimizeButton)
    {
        m_minimizeButton = minimizeButton;
        updatePosition();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Button::Ptr ChildWindow::getMinimizeButton() const
    {
        return m_minimizeButton;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChildWindow::setMaximizeButton(Button::Ptr maximizeButton)
    {
        m_maximizeButton = maximizeButton;
        updatePosition();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Button::Ptr ChildWindow::getMaximizeButton() const
    {
        return m_maximizeButton;
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

            // Send the mouse press event to the minimize button
            else if (m_minimizeButton->mouseOnWidget(x, y))
                m_minimizeButton->leftMousePressed(x, y);

            // Send the mouse press event to the maximize button
            else if (m_maximizeButton->mouseOnWidget(x, y))
                m_maximizeButton->leftMousePressed(x, y);

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
            // When the mouse is not on the title bar then the mouse is definitely not on the buttons inside the title bar
            for (auto& button : {m_closeButton, m_maximizeButton, m_minimizeButton})
            {
                if (button->m_mouseHover)
                    button->mouseNoLongerOnWidget();
            }

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
                    }
                }
            }

            // Check if the minimize button was clicked
            else if (m_minimizeButton->m_mouseDown)
            {
                // Check if the mouse is still on the minimize button
                if (m_minimizeButton->mouseOnWidget(x, y))
                {
                    if (isSignalBound("minimized"))
                        sendSignal("minimized", std::static_pointer_cast<ChildWindow>(shared_from_this()));
                }
            }

            // Check if the maximize button was clicked
            else if (m_maximizeButton->m_mouseDown)
            {
                // Check if the mouse is still on the minimize button
                if (m_maximizeButton->mouseOnWidget(x, y))
                {
                    if (isSignalBound("maximized"))
                        sendSignal("maximized", std::static_pointer_cast<ChildWindow>(shared_from_this()));
                }
            }

            return;
        }
        else // The mouse is not on top of the title bar
        {
            // When the mouse is not on the title bar, the mouse can't be on the button inside it
            for (auto& button : {m_closeButton, m_maximizeButton, m_minimizeButton})
            {
                if (button->m_mouseHover)
                    button->mouseNoLongerOnWidget();

                button->mouseNoLongerDown();
            }

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
            float minimumWidth = 0;

            for (const auto& button : {m_closeButton, m_maximizeButton, m_minimizeButton})
            {
                if (button->isVisible())
                    minimumWidth += (minimumWidth > 0 ? getRenderer()->m_paddingBetweenButtons : 0) + button->getSize().x;
            }

            minimumWidth += 2 * getRenderer()->m_distanceToSide - getRenderer()->getBorders().left - getRenderer()->getBorders().right;

            if ((m_resizeDirection & ResizeLeft) != 0)
            {
                const float diff = clamp(x - getPosition().x, getSize().x - m_maximumSize.x, getSize().x - std::max(minimumWidth, m_minimumSize.x));

                setPosition(getPosition().x + diff, getPosition().y);
                setSize(getSize().x - diff, getSize().y);
            }
            else if ((m_resizeDirection & ResizeRight) != 0)
            {
                const float newX = std::max(0.f, x - (getPosition().x + getRenderer()->m_borders.left));
                setSize(clamp(newX, std::max(minimumWidth, m_minimumSize.x), m_maximumSize.x), getSize().y);
            }

            if ((m_resizeDirection & ResizeBottom) != 0)
            {
                const float newY = std::max(0.f, y - (getPosition().y + getRenderer()->m_titleBarHeight + getRenderer()->m_borders.top));
                setSize(getSize().x, clamp(newY, m_minimumSize.y, m_maximumSize.y));
            }
        }

        // Check if the mouse is on top of the title bar
        else if (sf::FloatRect{getPosition().x, getPosition().y, getSize().x + getRenderer()->m_borders.left + getRenderer()->m_borders.right, getRenderer()->m_titleBarHeight}.contains(x, y))
        {
            // Send the hover event to the button inside the title bar
            for (auto& button : {m_closeButton, m_maximizeButton, m_minimizeButton})
            {
                if (button->mouseOnWidget(x, y))
                    button->mouseMoved(x, y);
                else
                    button->mouseNoLongerOnWidget();
            }

            return;
        }
        else // The mouse is not on top of the title bar
        {
            // When the mouse is not on the title bar, the mouse can't be on the buttons inside it
            for (auto& button : {m_closeButton, m_maximizeButton, m_minimizeButton})
            {
                if (button->m_mouseHover)
                    button->mouseNoLongerOnWidget();
            }

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
        m_minimizeButton->mouseNoLongerOnWidget();
        m_maximizeButton->mouseNoLongerOnWidget();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChildWindow::mouseNoLongerDown()
    {
        Container::mouseNoLongerDown();
        m_closeButton->mouseNoLongerDown();
        m_minimizeButton->mouseNoLongerDown();
        m_maximizeButton->mouseNoLongerDown();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChildWindow::reload(const std::string& primary, const std::string& secondary, bool force)
    {
        m_closeButton->reload();
        m_closeButton->getRenderer()->setBorders({1, 1, 1, 1});

        m_minimizeButton->reload();
        m_minimizeButton->getRenderer()->setBorders({1, 1, 1, 1});

        m_maximizeButton->reload();
        m_maximizeButton->getRenderer()->setBorders({1, 1, 1, 1});

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

                    if (m_minimizeButton->getRenderer()->m_textureNormal.isLoaded())
                        m_minimizeButton->setSize(m_minimizeButton->getRenderer()->m_textureNormal.getImageSize());

                    if (m_maximizeButton->getRenderer()->m_textureNormal.isLoaded())
                        m_maximizeButton->setSize(m_maximizeButton->getRenderer()->m_textureNormal.getImageSize());
                }
            }
        }

        if (!m_closeButton->getRenderer()->m_textureNormal.isLoaded())
        {
            m_closeButton->setSize({getRenderer()->m_titleBarHeight * 0.8f, getRenderer()->m_titleBarHeight * 0.8f});
            m_closeButton->setText(m_closeButtonText);
        }

        if (!m_minimizeButton->getRenderer()->m_textureNormal.isLoaded())
        {
            m_minimizeButton->setSize({getRenderer()->m_titleBarHeight * 0.8f, getRenderer()->m_titleBarHeight * 0.8f});
            m_minimizeButton->setText(m_minimizeButtonText);
        }

        if (!m_maximizeButton->getRenderer()->m_textureNormal.isLoaded())
        {
            m_maximizeButton->setSize({getRenderer()->m_titleBarHeight * 0.8f, getRenderer()->m_titleBarHeight * 0.8f});
            m_maximizeButton->setText(m_maximizeButtonText);
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

        float buttonOffsetX = 0;
        for (const auto& button : {m_closeButton, m_maximizeButton, m_minimizeButton})
        {
            if (button->isVisible())
                buttonOffsetX += (buttonOffsetX > 0 ? getRenderer()->m_paddingBetweenButtons : 0) + button->getSize().x;
        }

        if (buttonOffsetX > 0)
            buttonOffsetX += getRenderer()->m_distanceToSide;

        // Check if there is a title
        if (!m_titleText.getText().isEmpty())
        {
            sf::Vector2f clippingTopLeft = {getPosition().x + getRenderer()->m_distanceToSide, getPosition().y};
            sf::Vector2f clippingBottomRight = {getPosition().x + getRenderer()->m_borders.left + getSize().x - getRenderer()->m_distanceToSide - buttonOffsetX,
                                                getPosition().y + getRenderer()->m_titleBarHeight};

            if (m_iconTexture.isLoaded())
                clippingTopLeft.x += getRenderer()->m_distanceToSide + m_iconTexture.getSize().x;

            // Set the clipping for all draw calls that happen until this clipping object goes out of scope
            Clipping clipping{target, states, clippingTopLeft, clippingBottomRight - clippingTopLeft};

            // Draw the text in the title bar
            target.draw(m_titleText, states);
        }


        states.transform.translate(getPosition().x + getRenderer()->m_borders.left, getPosition().y + getRenderer()->m_titleBarHeight + getRenderer()->m_borders.top);

        // Draw the background
        if (getRenderer()->m_backgroundColor != sf::Color::Transparent)
        {
            sf::RectangleShape background(getSize());
            background.setFillColor(calcColorOpacity(getRenderer()->m_backgroundColor, getOpacity()));
            target.draw(background, states);
        }

        // Set the clipping for all draw calls that happen until this clipping object goes out of scope
        Clipping clipping{target, states, {}, getSize()};

        // Draw the widgets in the child window
        drawWidgetContainer(&target, states);
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

                m_childWindow->m_closeButton = m_childWindow->getTheme()->internalLoad(
                                                    m_childWindow->getPrimaryLoadingParameter(),
                                                    Deserializer::deserialize(ObjectConverter::Type::String, value).getString()
                                               );
            }
        }
        else if (property == "minimizebutton")
        {
            if (value.empty() || toLower(value) == "default")
            {
                m_minimizeButtonClassName = "";
                m_childWindow->m_minimizeButton = std::make_shared<Button>();
            }
            else
            {
                m_minimizeButtonClassName = Deserializer::deserialize(ObjectConverter::Type::String, value).getString();

                /// TODO: Widget files do not contain themes yet. This means that child window cannot be loaded from one.
                ///       Temporarily load default minimize button in case it is attempted.
                if (m_childWindow->getTheme() == nullptr)
                {
                    m_childWindow->m_minimizeButton = std::make_shared<Button>();
                    return;
                }

                m_childWindow->m_minimizeButton = m_childWindow->getTheme()->internalLoad(
                                                    m_childWindow->getPrimaryLoadingParameter(),
                                                    Deserializer::deserialize(ObjectConverter::Type::String, value).getString()
                                               );
            }
        }
        else if (property == "maximizebutton")
        {
            if (value.empty() || toLower(value) == "default")
            {
                m_maximizeButtonClassName = "";
                m_childWindow->m_maximizeButton = std::make_shared<Button>();
            }
            else
            {
                m_maximizeButtonClassName = Deserializer::deserialize(ObjectConverter::Type::String, value).getString();

                /// TODO: Widget files do not contain themes yet. This means that child window cannot be loaded from one.
                ///       Temporarily load default minimize button in case it is attempted.
                if (m_childWindow->getTheme() == nullptr)
                {
                    m_childWindow->m_maximizeButton = std::make_shared<Button>();
                    return;
                }

                m_childWindow->m_maximizeButton = m_childWindow->getTheme()->internalLoad(
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

                    m_childWindow->m_closeButton = m_childWindow->getTheme()->internalLoad(m_childWindow->getPrimaryLoadingParameter(), value.getString());
                }
            }

            else if (property == "minimizebutton")
            {
                m_minimizeButtonClassName = value.getString();

                if (value.getString().isEmpty())
                    m_childWindow->m_minimizeButton = std::make_shared<Button>();
                else
                {
                    /// TODO: Widget files do not contain themes yet. This means that child window cannot be loaded from one.
                    ///       Temporarily load default minimize button in case it is attempted.
                    if (m_childWindow->getTheme() == nullptr)
                    {
                        m_childWindow->m_minimizeButton = std::make_shared<Button>();
                        return;
                    }

                    m_childWindow->m_minimizeButton = m_childWindow->getTheme()->internalLoad(m_childWindow->getPrimaryLoadingParameter(), value.getString());
                }
            }

            else if (property == "maximizebutton")
            {
                m_maximizeButtonClassName = value.getString();

                if (value.getString().isEmpty())
                    m_childWindow->m_maximizeButton = std::make_shared<Button>();
                else
                {
                    /// TODO: Widget files do not contain themes yet. This means that child window cannot be loaded from one.
                    ///       Temporarily load default maximize button in case it is attempted.
                    if (m_childWindow->getTheme() == nullptr)
                    {
                        m_childWindow->m_maximizeButton = std::make_shared<Button>();
                        return;
                    }

                    m_childWindow->m_maximizeButton = m_childWindow->getTheme()->internalLoad(m_childWindow->getPrimaryLoadingParameter(), value.getString());
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

        // Set the size of the buttons in the title bar
        for (auto& button : {m_childWindow->m_closeButton, m_childWindow->m_minimizeButton, m_childWindow->m_maximizeButton})
        {
            if (m_textureTitleBar.isLoaded() && button->getRenderer()->m_textureNormal.isLoaded())
            {
                button->setSize(button->getRenderer()->m_textureNormal.getImageSize().x * (height / m_textureTitleBar.getImageSize().y),
                                button->getRenderer()->m_textureNormal.getImageSize().y * (height / m_textureTitleBar.getImageSize().y));
            }
            else
                button->setSize({height * 0.8f, height * 0.8f});
        }

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

    void ChildWindowRenderer::setPaddingBetweenButtons(float paddingBetweenButtons)
    {
        m_paddingBetweenButtons = paddingBetweenButtons;

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
            m_textureTitleBar.setColor({m_textureTitleBar.getColor().r, m_textureTitleBar.getColor().g, m_textureTitleBar.getColor().b, static_cast<sf::Uint8>(m_childWindow->getOpacity() * 255)});
        }

        setTitleBarHeight(m_titleBarHeight);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::shared_ptr<ButtonRenderer> ChildWindowRenderer::getCloseButton() const
    {
        return m_childWindow->m_closeButton->getRenderer();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::shared_ptr<ButtonRenderer> ChildWindowRenderer::getMinimizeButton() const
    {
        return m_childWindow->m_minimizeButton->getRenderer();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::shared_ptr<ButtonRenderer> ChildWindowRenderer::getMaximizeButton() const
    {
        return m_childWindow->m_maximizeButton->getRenderer();
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

        // Draw the buttons
        if (m_childWindow->m_closeButton->isVisible())
            target.draw(*m_childWindow->m_closeButton, states);

        if (m_childWindow->m_minimizeButton->isVisible())
            target.draw(*m_childWindow->m_minimizeButton, states);

        if (m_childWindow->m_maximizeButton->isVisible())
            target.draw(*m_childWindow->m_maximizeButton, states);

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
