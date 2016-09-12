/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus' Graphical User Interface
// Copyright (C) 2012-2016 Bruno Van de Velde (vdv_b@tgui.eu)
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
#include <TGUI/Clipping.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    namespace
    {
        const std::string closeButtonText = "x";
        const std::string maximizeButtonText = "+";
        const std::string minimizeButtonText = "-";

        float clamp(float value, float lower, float upper)
        {
            if (value < lower)
                return lower;

            if (value > upper)
                return upper;

            return value;
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ChildWindow::ChildWindow()
    {
        m_type = "ChildWindow";
        m_callback.widgetType = "ChildWindow";

        addSignal<sf::Vector2f>("MousePressed");
        addSignal<ChildWindow::Ptr>("Closed");
        addSignal<ChildWindow::Ptr>("Minimized");
        addSignal<ChildWindow::Ptr>("Maximized");

        m_renderer = aurora::makeCopied<ChildWindowRenderer>();
        setRenderer(m_renderer->getData());

        getRenderer()->setDistanceToSide(3);
        getRenderer()->setBorders({1});
        getRenderer()->setBackgroundColor({230, 230, 230});
        getRenderer()->setTitleColor(sf::Color::Black);
        getRenderer()->getCloseButton()->propertyValuePairs["borders"] = {Borders{1}};
        getRenderer()->getMaximizeButton()->propertyValuePairs["borders"] = {Borders{1}};
        getRenderer()->getMinimizeButton()->propertyValuePairs["borders"] = {Borders{1}};

        setTitleButtons(TitleButtons::Close);
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
        float distanceToSide = getRenderer()->getDistanceToSide();
        float paddingBetweenButtons = getRenderer()->getPaddingBetweenButtons();
        float titleBarHeight = getRenderer()->getTitleBarHeight();
        Borders borders = getRenderer()->getBorders();

        if (m_parent && m_keepInParent)
        {
            if (y < 0)
                y = 0;
            else if (y > m_parent->getSize().y - titleBarHeight)
                y = m_parent->getSize().y - titleBarHeight;

            if (x < 0)
                x = 0;
            else if (x > m_parent->getSize().x - getSize().x)
                x = m_parent->getSize().x - getSize().x;
        }

        Widget::setPosition({x, y});

        // Calculate the distance from the right side that the buttons will need
        float buttonOffsetX = 0;
        for (const auto& button : {m_closeButton, m_maximizeButton, m_minimizeButton})
        {
            if (button)
                buttonOffsetX += (buttonOffsetX > 0 ? paddingBetweenButtons : 0) + button->getSize().x;
        }

        if (buttonOffsetX > 0)
            buttonOffsetX += distanceToSide;

        if (m_titleAlignment == TitleAlignment::Left)
        {
            m_titleText.setPosition(distanceToSide, (titleBarHeight - m_titleText.getSize().y) / 2.0f);
        }
        else if (m_titleAlignment == TitleAlignment::Center)
        {
            m_titleText.setPosition(distanceToSide + (((getSize().x + borders.left + borders.right) - (2 * distanceToSide) - buttonOffsetX - m_titleText.getSize().x) / 2.0f),
                                    (titleBarHeight - m_titleText.getSize().y) / 2.0f);
        }
        else // if (m_titleAlignment == TitleAlignment::Right)
        {
            m_titleText.setPosition((getSize().x + borders.left + borders.right) - distanceToSide - buttonOffsetX - m_titleText.getSize().x,
                                    (titleBarHeight - m_titleText.getSize().y) / 2.0f);
        }

        buttonOffsetX = distanceToSide;
        for (auto& button : {m_closeButton, m_maximizeButton, m_minimizeButton})
        {
            if (button)
            {
                button->setPosition((getSize().x + borders.left + borders.right) - buttonOffsetX - button->getSize().x,
                                    (titleBarHeight - button->getSize().y) / 2.f);

                buttonOffsetX += button->getSize().x + paddingBetweenButtons;
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChildWindow::setSize(const Layout2d& size)
    {
        Widget::setSize(size);

        if (getRenderer()->getTextureTitleBar().isLoaded())
            getRenderer()->getTextureTitleBar().setSize({getSize().x + getRenderer()->getBorders().left + getRenderer()->getBorders().right, getRenderer()->getTitleBarHeight()});

        // Reposition the images and text
        updatePosition();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    sf::Vector2f ChildWindow::getFullSize() const
    {
        Borders borders = getRenderer()->getBorders();
        return {getSize().x + borders.left + borders.right,
                getSize().y + borders.top + borders.bottom + getRenderer()->getTitleBarHeight()};
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

    void ChildWindow::setTitle(const sf::String& title)
    {
        m_titleText.setString(title);

        // Reposition the images and text
        updatePosition();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const sf::String& ChildWindow::getTitle() const
    {
        return m_titleText.getString();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChildWindow::setTitleAlignment(TitleAlignment alignment)
    {
        m_titleAlignment = alignment;

        // Reposition the images and text
        updatePosition();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ChildWindow::TitleAlignment ChildWindow::getTitleAlignment() const
    {
        return m_titleAlignment;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChildWindow::setTitleButtons(int buttons)
    {
        m_titleButtons = buttons;

        if (m_titleButtons & TitleButtons::Close)
        {
            m_closeButton = std::make_shared<tgui::Button>();
            m_closeButton->setRenderer(getRenderer()->getCloseButton());
            m_closeButton->getRenderer()->setOpacity(getRenderer()->getOpacity());
            m_closeButton->connect("pressed", &ChildWindow::titleButtonPressed, this, "closed");
        }
        else
            m_closeButton = nullptr;

        if (m_titleButtons & TitleButtons::Maximize)
        {
            m_maximizeButton = std::make_shared<tgui::Button>();
            m_maximizeButton->setRenderer(getRenderer()->getMaximizeButton());
            m_maximizeButton->getRenderer()->setOpacity(getRenderer()->getOpacity());
            m_maximizeButton->connect("pressed", &ChildWindow::titleButtonPressed, this, "maximized");
        }
        else
            m_maximizeButton = nullptr;

        if (m_titleButtons & TitleButtons::Minimize)
        {
            m_minimizeButton = std::make_shared<tgui::Button>();
            m_minimizeButton->setRenderer(getRenderer()->getMinimizeButton());
            m_minimizeButton->getRenderer()->setOpacity(getRenderer()->getOpacity());
            m_minimizeButton->connect("pressed", &ChildWindow::titleButtonPressed, this, "minimized");
        }
        else
            m_minimizeButton = nullptr;

        updateTitleBarHeight();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    int ChildWindow::getTitleButtons() const
    {
        return m_titleButtons;
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

    sf::Vector2f ChildWindow::getChildWidgetsOffset() const
    {
        return {getRenderer()->getBorders().left, getRenderer()->getBorders().top + getRenderer()->getTitleBarHeight()};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ChildWindow::mouseOnWidget(sf::Vector2f pos) const
    {
        float titleBarHeight = getRenderer()->getTitleBarHeight();
        Borders borders = getRenderer()->getBorders();

        // Check if the mouse is on top of the title bar
        if (sf::FloatRect{0, 0, getSize().x + borders.left + borders.right, titleBarHeight}.contains(pos))
        {
            if (m_widgetBelowMouse)
                m_widgetBelowMouse->mouseNoLongerOnWidget();

            return true;
        }

        // Check if the mouse is inside the child window
        return sf::FloatRect{0, 0, getSize().x + borders.left + borders.right, getSize().y + borders.top + borders.bottom}.contains(pos.x, pos.y - titleBarHeight);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChildWindow::leftMousePressed(sf::Vector2f pos)
    {
        m_mouseDown = true;

        // Move the child window to the front
        moveToFront();

        m_callback.mouse.x = static_cast<int>(pos.x);
        m_callback.mouse.y = static_cast<int>(pos.y);
        sendSignal("MousePressed", pos);

        // Check if the mouse is on top of the title bar
        Borders borders = getRenderer()->getBorders();
        float titleBarHeight = getRenderer()->getTitleBarHeight();
        if (sf::FloatRect{0, 0, getSize().x + borders.left + borders.right, titleBarHeight}.contains(pos))
        {
            // Send the mouse press event to the title buttons
            for (auto& button : {m_closeButton, m_maximizeButton, m_minimizeButton})
            {
                if (button && button->mouseOnWidget(pos - button->getPosition()))
                {
                    button->leftMousePressed(pos - button->getPosition());
                    return;
                }
            }

            // The mouse went down on the title bar
            m_mouseDownOnTitleBar = true;

            // Remember where we are dragging the title bar
            m_draggingPosition = pos;
        }
        else // The mouse is not on top of the title bar
        {
            // Check if the mouse is on top of the borders
            if ((sf::FloatRect{0, 0, getSize().x + borders.left + borders.right, getSize().y + borders.top + borders.bottom + titleBarHeight}.contains(pos))
             && (!sf::FloatRect{borders.left, titleBarHeight + borders.top, getSize().x, getSize().y}.contains(pos)))
            {
                // Check if you start resizing the child window
                if (m_resizable)
                {
                    // Check on which border the mouse is standing
                    if (sf::FloatRect{0, titleBarHeight + borders.top + getSize().y, borders.left, borders.bottom}.contains(pos))
                        m_resizeDirection = ResizeLeft | ResizeBottom;
                    else if (sf::FloatRect{borders.left + getSize().x, titleBarHeight + borders.top + getSize().y, borders.right, borders.bottom}.contains(pos))
                        m_resizeDirection = ResizeRight | ResizeBottom;
                    else if (sf::FloatRect{0, titleBarHeight, borders.left, borders.top + getSize().y}.contains(pos))
                        m_resizeDirection = ResizeLeft;
                    else if (sf::FloatRect{borders.left + getSize().x, titleBarHeight, borders.right, borders.top + getSize().y}.contains(pos))
                        m_resizeDirection = ResizeRight;
                    else if (sf::FloatRect{borders.left, titleBarHeight + borders.top + getSize().y, getSize().x, borders.bottom}.contains(pos))
                        m_resizeDirection = ResizeBottom;
                }
            }
            else // Propagate the event to the child widgets
                Container::leftMousePressed({pos.x - borders.left, pos.y - (titleBarHeight + borders.top)});
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChildWindow::leftMouseReleased(sf::Vector2f pos)
    {
        m_mouseDownOnTitleBar = false;
        m_resizeDirection = ResizeNone;

        // Check if the mouse is on top of the title bar
        Borders borders = getRenderer()->getBorders();
        float titleBarHeight = getRenderer()->getTitleBarHeight();
        if (sf::FloatRect{0, 0, getSize().x + borders.left + borders.right, titleBarHeight}.contains(pos))
        {
            // Send the mouse release event to the title buttons
            for (auto& button : {m_closeButton, m_maximizeButton, m_minimizeButton})
            {
                if (button && button->mouseOnWidget(pos - button->getPosition()))
                {
                    button->leftMouseReleased(pos - button->getPosition());
                    break;
                }
            }
        }
        else // The mouse is not on top of the title bar
        {
            // Check if the mouse is on top of the borders
            if ((sf::FloatRect{0, 0, getSize().x + borders.left + borders.right, getSize().y + borders.top + borders.bottom + titleBarHeight}.contains(pos))
             && (!sf::FloatRect{borders.left, titleBarHeight + borders.top, getSize().x, getSize().y}.contains(pos)))
            {
                // Tell the widgets that the mouse was released
                for (auto& widget : m_widgets)
                    widget->mouseNoLongerDown();
            }
            else // Propagate the event to the child widgets
                Container::leftMouseReleased({pos.x - borders.left, pos.y - (titleBarHeight + borders.top)});
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChildWindow::mouseMoved(sf::Vector2f pos)
    {
        // Check if you are dragging the child window
        if (m_mouseDown && m_mouseDownOnTitleBar)
        {
            // Move the child window
            setPosition(getPosition() + (pos - m_draggingPosition));
        }

        // Check if you are resizing the window
        else if (m_mouseDown && m_resizeDirection != ResizeNone)
        {
            float minimumWidth = 0;

            for (const auto& button : {m_closeButton, m_maximizeButton, m_minimizeButton})
            {
                if (button)
                    minimumWidth += (minimumWidth > 0 ? getRenderer()->getPaddingBetweenButtons() : 0) + button->getSize().x;
            }

            minimumWidth += 2 * getRenderer()->getDistanceToSide() - getRenderer()->getBorders().left - getRenderer()->getBorders().right;

            if ((m_resizeDirection & ResizeLeft) != 0)
            {
                const float diff = clamp(pos.x, getSize().x - m_maximumSize.x, getSize().x - std::max(minimumWidth, m_minimumSize.x));

                setPosition(getPosition().x + diff, getPosition().y);
                setSize(getSize().x - diff, getSize().y);
            }
            else if ((m_resizeDirection & ResizeRight) != 0)
            {
                setSize(clamp(pos.x - getRenderer()->getBorders().left, std::max(minimumWidth, m_minimumSize.x), m_maximumSize.x), getSize().y);
            }

            if ((m_resizeDirection & ResizeBottom) != 0)
            {
                const float newY = std::max(0.f, pos.y - (getRenderer()->getTitleBarHeight() + getRenderer()->getBorders().top));
                setSize(getSize().x, clamp(newY, m_minimumSize.y, m_maximumSize.y));
            }
        }

        // Check if the mouse is on top of the title bar
        else if (sf::FloatRect{0, 0, getSize().x + getRenderer()->getBorders().left + getRenderer()->getBorders().right, getRenderer()->getTitleBarHeight()}.contains(pos))
        {
            // Send the hover event to the button inside the title bar
            for (auto& button : {m_closeButton, m_maximizeButton, m_minimizeButton})
            {
                if (button)
                {
                    if (button->mouseOnWidget(pos - button->getPosition()))
                        button->mouseMoved(pos - button->getPosition());
                    else
                        button->mouseNoLongerOnWidget();
                }
            }

            if (!m_mouseHover)
                mouseEnteredWidget();

            return;
        }
        else // The mouse is not on top of the title bar
        {
            // When the mouse is not on the title bar, the mouse can't be on the buttons inside it
            for (auto& button : {m_closeButton, m_maximizeButton, m_minimizeButton})
            {
                if (button)
                    button->mouseNoLongerOnWidget();
            }

            // Check if the mouse is on top of the borders
            Borders borders = getRenderer()->getBorders();
            if ((sf::FloatRect{0, 0, getSize().x + borders.left + borders.right, getSize().y + borders.top + borders.bottom + getRenderer()->getTitleBarHeight()}.contains(pos))
             && (!sf::FloatRect{borders.left, getRenderer()->getTitleBarHeight() + borders.top, getSize().x, getSize().y}.contains(pos)))
            {
                if (!m_mouseHover)
                    mouseEnteredWidget();

                // Don't send the event to the widgets
                return;
            }
        }

        Container::mouseMoved({pos.x - getRenderer()->getBorders().left, pos.y - getRenderer()->getTitleBarHeight() - getRenderer()->getBorders().top});
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChildWindow::mouseWheelMoved(int delta, int x, int y)
    {
        sf::Vector2f offset = getChildWidgetsOffset();
        Container::mouseWheelMoved(delta, static_cast<int>(x - offset.x), static_cast<int>(y - offset.y));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChildWindow::mouseNoLongerOnWidget()
    {
        Container::mouseNoLongerOnWidget();

        for (const auto& button : {m_closeButton, m_maximizeButton, m_minimizeButton})
        {
            if (button)
                button->mouseNoLongerOnWidget();
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChildWindow::mouseNoLongerDown()
    {
        Container::mouseNoLongerDown();

        for (const auto& button : {m_closeButton, m_maximizeButton, m_minimizeButton})
        {
            if (button)
                button->mouseNoLongerDown();
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChildWindow::updateTitleBarHeight()
    {
        float height = getRenderer()->getTitleBarHeight();

        Borders borders = getRenderer()->getBorders();
        Texture& textureTitleBar = getRenderer()->getTextureTitleBar();
        textureTitleBar.setSize({getSize().x + borders.left + borders.right, height});

        // Set the size of the buttons in the title bar
        for (auto& button : {m_closeButton, m_minimizeButton, m_maximizeButton})
        {
            if (button)
            {
                if (textureTitleBar.isLoaded() && button->getRenderer()->getTexture().isLoaded())
                {
                    button->setSize(button->getRenderer()->getTexture().getImageSize().x * (height / textureTitleBar.getImageSize().y),
                                    button->getRenderer()->getTexture().getImageSize().y * (height / textureTitleBar.getImageSize().y));
                }
                else
                    button->setSize({height * 0.8f, height * 0.8f});
            }
        }

        // Set the size of the text in the title bar
        m_titleText.setCharacterSize(findBestTextSize(getRenderer()->getFont(), height * 0.8f));

        // Reposition the images and text
        updatePosition();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChildWindow::rendererChanged(const std::string& property, ObjectConverter& value)
    {
        if (property == "borders")
        {
            updateSize();
        }
        else if (property == "titlecolor")
        {
            m_titleText.setColor(value.getColor());
        }
        else if (property == "texturetitlebar")
        {
            value.getTexture().setOpacity(getRenderer()->getOpacity());
            value.getTexture().setSize({getSize().x + getRenderer()->getBorders().left + getRenderer()->getBorders().right,
                                        getRenderer()->getTitleBarHeight()});

            // If the title bar height is determined by the texture then update it
            if (getRenderer()->getTitleBarHeight() == value.getTexture().getImageSize().y)
                updateTitleBarHeight();
        }
        else if (property == "titlebarheight")
        {
            updateTitleBarHeight();
        }
        else if ((property == "distancetoside") || (property == "paddingbetweenbuttons"))
        {
            updatePosition();
        }
        else if (property == "closebutton")
        {
            if (m_closeButton)
            {
                m_closeButton->setRenderer(value.getRenderer());
                m_closeButton->getRenderer()->setOpacity(getRenderer()->getOpacity());
            }

            updatePosition();
        }
        else if (property == "maximizebutton")
        {
            if (m_maximizeButton)
            {
                m_maximizeButton->setRenderer(value.getRenderer());
                m_maximizeButton->getRenderer()->setOpacity(getRenderer()->getOpacity());
            }

            updatePosition();
        }
        else if (property == "minimizebutton")
        {
            if (m_minimizeButton)
            {
                m_minimizeButton->setRenderer(value.getRenderer());
                m_minimizeButton->getRenderer()->setOpacity(getRenderer()->getOpacity());
            }

            updatePosition();
        }
        else if (property == "opacity")
        {
            float opacity = value.getNumber();

            for (auto& button : {m_closeButton, m_minimizeButton, m_maximizeButton})
            {
                if (button)
                    button->getRenderer()->setOpacity(opacity);
            }

            m_titleText.setOpacity(opacity);
            getRenderer()->getTextureTitleBar().setOpacity(opacity);
        }
        else if (property == "font")
        {
            Font font = value.getFont();

            for (auto& button : {m_closeButton, m_minimizeButton, m_maximizeButton})
            {
                if (button)
                    button->getRenderer()->setFont(font);
            }

            m_titleText.setFont(font);
            m_titleText.setCharacterSize(findBestTextSize(font, getRenderer()->getTitleBarHeight() * 0.8f));

            updatePosition();

            Container::rendererChanged(property, value);
        }
        else if ((property != "backgroundcolor")
              && (property != "titlebarcolor")
              && (property != "bordercolor"))
        {
            Container::rendererChanged(property, value);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChildWindow::draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        states.transform.translate(getPosition());

        Borders borders = getRenderer()->getBorders();
        sf::Vector2f sizeIncludingBorders{getSize().x + borders.left + borders.right, getSize().y + borders.top + borders.bottom};

        // Draw the title bar
        if (getRenderer()->getTextureTitleBar().isLoaded())
            getRenderer()->getTextureTitleBar().draw(target, states);
        else
            drawRectangleShape(target, states, {sizeIncludingBorders.x, getRenderer()->getTitleBarHeight()}, getRenderer()->getTitleBarColor());

        // Draw the text in the title bar (after setting the clipping area)
        {
            float buttonOffsetX = 0;
            for (const auto& button : {m_closeButton, m_maximizeButton, m_minimizeButton})
            {
                if (button)
                    buttonOffsetX += (buttonOffsetX > 0 ? getRenderer()->getPaddingBetweenButtons() : 0) + button->getSize().x;
            }

            if (buttonOffsetX > 0)
                buttonOffsetX += getRenderer()->getDistanceToSide();

            float clippingLeft = getRenderer()->getDistanceToSide();
            float clippingRight = sizeIncludingBorders.x - getRenderer()->getDistanceToSide() - buttonOffsetX;

            Clipping clipping{target, states, {clippingLeft, 0}, {clippingRight - clippingLeft, getRenderer()->getTitleBarHeight()}};

            m_titleText.draw(target, states);
        }

        // Draw the buttons
        for (auto& button : {m_closeButton, m_minimizeButton, m_maximizeButton})
        {
            if (button)
                target.draw(*button, states);
        }

        states.transform.translate({0, getRenderer()->getTitleBarHeight()});

        // Draw the borders
        if (borders != Borders{0})
        {
            drawBorders(target, states, borders, sizeIncludingBorders, getRenderer()->getBorderColor());
            states.transform.translate({borders.left, borders.top});
        }

        // Draw the background
        if (getRenderer()->getBackgroundColor() != sf::Color::Transparent)
            drawRectangleShape(target, states, getSize(), getRenderer()->getBackgroundColor());

        // Draw the widgets in the child window
        Clipping clipping{target, states, {}, {getSize()}};
        drawWidgetContainer(&target, states);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChildWindow::titleButtonPressed(const sf::String& signal)
    {
        sendSignal(signal, std::static_pointer_cast<ChildWindow>(shared_from_this()));

        // When the closed signal is send while there is no signal handler for it then destroy the child window
        if (signal == "closed")
        {
            if (!isSignalBound("closed"))
                destroy();
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
