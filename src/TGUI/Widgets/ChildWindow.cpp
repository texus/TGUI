/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus' Graphical User Interface
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

#include <iostream>
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

    static std::map<std::string, ObjectConverter> defaultRendererValues =
            {
                {"borders", Borders{1}},
                {"bordercolor", sf::Color::Black},
                {"titlecolor", sf::Color::Black},
                {"titlebarcolor", sf::Color::White},
                {"backgroundcolor", Color{230, 230, 230}},
                {"distancetoside", 3.f},
                {"paddingbetweenbuttons", 1.f}
            };

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
        setRenderer(RendererData::create(defaultRendererValues));

        getRenderer()->getCloseButton()->propertyValuePairs["borders"] = {Borders{1}};
        getRenderer()->getMaximizeButton()->propertyValuePairs["borders"] = {Borders{1}};
        getRenderer()->getMinimizeButton()->propertyValuePairs["borders"] = {Borders{1}};

        setTitleButtons(TitleButton::Close);
        setSize(400, 300);
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
            else if (y > m_parent->getSize().y - m_titleBarHeightCached)
                y = m_parent->getSize().y - m_titleBarHeightCached;

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
                buttonOffsetX += (buttonOffsetX > 0 ? m_paddingBetweenButtonsCached : 0) + button->getSize().x;
        }

        if (buttonOffsetX > 0)
            buttonOffsetX += m_distanceToSideCached;

        if (m_titleAlignment == TitleAlignment::Left)
        {
            m_titleText.setPosition(m_distanceToSideCached, (m_titleBarHeightCached - m_titleText.getSize().y) / 2.0f);
        }
        else if (m_titleAlignment == TitleAlignment::Center)
        {
            m_titleText.setPosition(m_distanceToSideCached + (((getSize().x + m_bordersCached.left + m_bordersCached.right) - (2 * m_distanceToSideCached) - buttonOffsetX - m_titleText.getSize().x) / 2.0f),
                                    (m_titleBarHeightCached - m_titleText.getSize().y) / 2.0f);
        }
        else // if (m_titleAlignment == TitleAlignment::Right)
        {
            m_titleText.setPosition((getSize().x + m_bordersCached.left + m_bordersCached.right) - m_distanceToSideCached - buttonOffsetX - m_titleText.getSize().x,
                                    (m_titleBarHeightCached - m_titleText.getSize().y) / 2.0f);
        }

        buttonOffsetX = m_distanceToSideCached;
        for (auto& button : {m_closeButton, m_maximizeButton, m_minimizeButton})
        {
            if (button)
            {
                button->setPosition((getSize().x + m_bordersCached.left + m_bordersCached.right) - buttonOffsetX - button->getSize().x,
                                    (m_titleBarHeightCached - button->getSize().y) / 2.f);

                buttonOffsetX += button->getSize().x + m_paddingBetweenButtonsCached;
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChildWindow::setSize(const Layout2d& size)
    {
        Widget::setSize(size);

        m_spriteTitleBar.setSize({getSize().x + m_bordersCached.left + m_bordersCached.right, m_titleBarHeightCached});

        // Reposition the images and text
        updatePosition();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    sf::Vector2f ChildWindow::getFullSize() const
    {
        return {getSize().x + m_bordersCached.left + m_bordersCached.right,
                getSize().y + m_bordersCached.top + m_bordersCached.bottom + m_titleBarHeightCached};
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

    void ChildWindow::setTitleButtons(unsigned int buttons)
    {
        m_titleButtons = buttons;

        if (m_titleButtons & TitleButton::Close)
        {
            m_closeButton = Button::create();
            m_closeButton->setRenderer(getRenderer()->getCloseButton());
            m_closeButton->getRenderer()->setOpacity(m_opacityCached);
            m_closeButton->connect("pressed", &ChildWindow::titleButtonPressed, this, "closed");
        }
        else
            m_closeButton = nullptr;

        if (m_titleButtons & TitleButton::Maximize)
        {
            m_maximizeButton = Button::create();
            m_maximizeButton->setRenderer(getRenderer()->getMaximizeButton());
            m_maximizeButton->getRenderer()->setOpacity(m_opacityCached);
            m_maximizeButton->connect("pressed", &ChildWindow::titleButtonPressed, this, "maximized");
        }
        else
            m_maximizeButton = nullptr;

        if (m_titleButtons & TitleButton::Minimize)
        {
            m_minimizeButton = Button::create();
            m_minimizeButton->setRenderer(getRenderer()->getMinimizeButton());
            m_minimizeButton->getRenderer()->setOpacity(m_opacityCached);
            m_minimizeButton->connect("pressed", &ChildWindow::titleButtonPressed, this, "minimized");
        }
        else
            m_minimizeButton = nullptr;

        updateTitleBarHeight();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int ChildWindow::getTitleButtons() const
    {
        return m_titleButtons;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChildWindow::destroy()
    {
        if (m_parent)
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
        return {m_bordersCached.left, m_bordersCached.top + m_titleBarHeightCached};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ChildWindow::mouseOnWidget(sf::Vector2f pos) const
    {
        // Check if the mouse is on top of the title bar
        if (sf::FloatRect{0, 0, getSize().x + m_bordersCached.left + m_bordersCached.right, m_titleBarHeightCached}.contains(pos))
        {
            if (m_widgetBelowMouse)
                m_widgetBelowMouse->mouseNoLongerOnWidget();

            return true;
        }

        // Check if the mouse is inside the child window
        return sf::FloatRect{0, 0, getSize().x + m_bordersCached.left + m_bordersCached.right, getSize().y + m_bordersCached.top + m_bordersCached.bottom}.contains(pos.x, pos.y - m_titleBarHeightCached);
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
        if (sf::FloatRect{0, 0, getSize().x + m_bordersCached.left + m_bordersCached.right, m_titleBarHeightCached}.contains(pos))
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
            if ((sf::FloatRect{0, 0, getSize().x + m_bordersCached.left + m_bordersCached.right, getSize().y + m_bordersCached.top + m_bordersCached.bottom + m_titleBarHeightCached}.contains(pos))
             && (!sf::FloatRect{m_bordersCached.left, m_titleBarHeightCached + m_bordersCached.top, getSize().x, getSize().y}.contains(pos)))
            {
                // Check if you start resizing the child window
                if (m_resizable)
                {
                    // Check on which border the mouse is standing
                    if (sf::FloatRect{0, m_titleBarHeightCached + m_bordersCached.top + getSize().y, m_bordersCached.left, m_bordersCached.bottom}.contains(pos))
                        m_resizeDirection = ResizeLeft | ResizeBottom;
                    else if (sf::FloatRect{m_bordersCached.left + getSize().x, m_titleBarHeightCached + m_bordersCached.top + getSize().y, m_bordersCached.right, m_bordersCached.bottom}.contains(pos))
                        m_resizeDirection = ResizeRight | ResizeBottom;
                    else if (sf::FloatRect{0, m_titleBarHeightCached, m_bordersCached.left, m_bordersCached.top + getSize().y}.contains(pos))
                        m_resizeDirection = ResizeLeft;
                    else if (sf::FloatRect{m_bordersCached.left + getSize().x, m_titleBarHeightCached, m_bordersCached.right, m_bordersCached.top + getSize().y}.contains(pos))
                        m_resizeDirection = ResizeRight;
                    else if (sf::FloatRect{m_bordersCached.left, m_titleBarHeightCached + m_bordersCached.top + getSize().y, getSize().x, m_bordersCached.bottom}.contains(pos))
                        m_resizeDirection = ResizeBottom;
                }
            }
            else // Propagate the event to the child widgets
                Container::leftMousePressed(pos);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChildWindow::leftMouseReleased(sf::Vector2f pos)
    {
        m_mouseDownOnTitleBar = false;
        m_resizeDirection = ResizeNone;

        // Check if the mouse is on top of the title bar
        if (sf::FloatRect{0, 0, getSize().x + m_bordersCached.left + m_bordersCached.right, m_titleBarHeightCached}.contains(pos))
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
            if ((sf::FloatRect{0, 0, getSize().x + m_bordersCached.left + m_bordersCached.right, getSize().y + m_bordersCached.top + m_bordersCached.bottom + m_titleBarHeightCached}.contains(pos))
             && (!sf::FloatRect{m_bordersCached.left, m_titleBarHeightCached + m_bordersCached.top, getSize().x, getSize().y}.contains(pos)))
            {
                // Tell the widgets that the mouse was released
                for (auto& widget : m_widgets)
                    widget->mouseNoLongerDown();
            }
            else // Propagate the event to the child widgets
                Container::leftMouseReleased(pos);
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
                    minimumWidth += (minimumWidth > 0 ? m_paddingBetweenButtonsCached : 0) + button->getSize().x;
            }

            minimumWidth += 2 * m_distanceToSideCached - m_bordersCached.left - m_bordersCached.right;

            if ((m_resizeDirection & ResizeLeft) != 0)
            {
                const float diff = clamp(pos.x, getSize().x - m_maximumSize.x, getSize().x - std::max(minimumWidth, m_minimumSize.x));

                setPosition(getPosition().x + diff, getPosition().y);
                setSize(getSize().x - diff, getSize().y);
            }
            else if ((m_resizeDirection & ResizeRight) != 0)
            {
                setSize(clamp(pos.x - m_bordersCached.left, std::max(minimumWidth, m_minimumSize.x), m_maximumSize.x), getSize().y);
            }

            if ((m_resizeDirection & ResizeBottom) != 0)
            {
                const float newY = std::max(0.f, pos.y - (m_titleBarHeightCached + m_bordersCached.top));
                setSize(getSize().x, clamp(newY, m_minimumSize.y, m_maximumSize.y));
            }
        }

        // Check if the mouse is on top of the title bar
        else if (sf::FloatRect{0, 0, getSize().x + m_bordersCached.left + m_bordersCached.right, m_titleBarHeightCached}.contains(pos))
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
            if ((sf::FloatRect{0, 0, getSize().x + m_bordersCached.left + m_bordersCached.right, getSize().y + m_bordersCached.top + m_bordersCached.bottom + m_titleBarHeightCached}.contains(pos))
             && (!sf::FloatRect{m_bordersCached.left, m_titleBarHeightCached + m_bordersCached.top, getSize().x, getSize().y}.contains(pos)))
            {
                if (!m_mouseHover)
                    mouseEnteredWidget();

                // Don't send the event to the widgets
                return;
            }
        }

        Container::mouseMoved(pos);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChildWindow::mouseWheelScrolled(float delta, int x, int y)
    {
        Container::mouseWheelScrolled(delta, x, y);
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
        m_spriteTitleBar.setSize({getSize().x + m_bordersCached.left + m_bordersCached.right, m_titleBarHeightCached});

        // Set the size of the buttons in the title bar
        for (auto& button : {m_closeButton, m_minimizeButton, m_maximizeButton})
        {
            if (button)
            {
                if (m_spriteTitleBar.isSet() && (button->getRenderer()->getTexture().getData() != nullptr))
                {
                    button->setSize(button->getRenderer()->getTexture().getImageSize().x * (m_titleBarHeightCached / m_spriteTitleBar.getTexture().getImageSize().y),
                                    button->getRenderer()->getTexture().getImageSize().y * (m_titleBarHeightCached / m_spriteTitleBar.getTexture().getImageSize().y));
                }
                else
                    button->setSize({m_titleBarHeightCached * 0.8f, m_titleBarHeightCached * 0.8f});
            }
        }

        // Set the size of the text in the title bar
        m_titleText.setCharacterSize(Text::findBestTextSize(m_fontCached, m_titleBarHeightCached * 0.8f));

        // Reposition the images and text
        updatePosition();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChildWindow::rendererChanged(const std::string& property)
    {
        if (property == "borders")
        {
            m_bordersCached = getRenderer()->getBorders();
            updateSize();
        }
        else if (property == "titlecolor")
        {
            m_titleText.setColor(getRenderer()->getTitleColor());
        }
        else if (property == "texturetitlebar")
        {
            m_spriteTitleBar.setTexture(getRenderer()->getTextureTitleBar());

            // If the title bar height is determined by the texture then update it (note that getTitleBarHeight has a non-trivial implementation)
            m_titleBarHeightCached = getRenderer()->getTitleBarHeight();
            if (m_titleBarHeightCached == m_spriteTitleBar.getTexture().getImageSize().y)
                updateTitleBarHeight();
        }
        else if (property == "titlebarheight")
        {
            m_titleBarHeightCached = getRenderer()->getTitleBarHeight();
            updateTitleBarHeight();
        }
        else if (property == "distancetoside")
        {
            m_distanceToSideCached = getRenderer()->getDistanceToSide();
            updatePosition();
        }
        else if (property == "paddingbetweenbuttons")
        {
            m_paddingBetweenButtonsCached = getRenderer()->getPaddingBetweenButtons();
            updatePosition();
        }
        else if (property == "closebutton")
        {
            if (m_closeButton)
            {
                m_closeButton->setRenderer(getRenderer()->getCloseButton());
                m_closeButton->getRenderer()->setOpacity(m_opacityCached);
            }

            updateTitleBarHeight();
        }
        else if (property == "maximizebutton")
        {
            if (m_maximizeButton)
            {
                m_maximizeButton->setRenderer(getRenderer()->getMaximizeButton());
                m_maximizeButton->getRenderer()->setOpacity(m_opacityCached);
            }

            updateTitleBarHeight();
        }
        else if (property == "minimizebutton")
        {
            if (m_minimizeButton)
            {
                m_minimizeButton->setRenderer(getRenderer()->getMinimizeButton());
                m_minimizeButton->getRenderer()->setOpacity(m_opacityCached);
            }

            updateTitleBarHeight();
        }
        else if (property == "backgroundcolor")
        {
            m_backgroundColorCached = getRenderer()->getBackgroundColor();
        }
        else if (property == "titlebarcolor")
        {
            m_titleBarColorCached = getRenderer()->getTitleBarColor();
        }
        else if (property == "bordercolor")
        {
            m_borderColorCached = getRenderer()->getBorderColor();
        }
        else if (property == "opacity")
        {
            Container::rendererChanged(property);

            for (auto& button : {m_closeButton, m_minimizeButton, m_maximizeButton})
            {
                if (button)
                    button->getRenderer()->setOpacity(m_opacityCached);
            }

            m_titleText.setOpacity(m_opacityCached);
            m_spriteTitleBar.setOpacity(m_opacityCached);
        }
        else if (property == "font")
        {
            Container::rendererChanged(property);

            for (auto& button : {m_closeButton, m_minimizeButton, m_maximizeButton})
            {
                if (button)
                    button->getRenderer()->setFont(m_fontCached);
            }

            m_titleText.setFont(m_fontCached);
            m_titleText.setCharacterSize(Text::findBestTextSize(m_fontCached, getRenderer()->getTitleBarHeight() * 0.8f));

            updatePosition();
        }
        else
            Container::rendererChanged(property);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChildWindow::draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        states.transform.translate(getPosition());

        sf::Vector2f sizeIncludingBorders{getSize().x + m_bordersCached.left + m_bordersCached.right, getSize().y + m_bordersCached.top + m_bordersCached.bottom};

        // Draw the title bar
        if (m_spriteTitleBar.isSet())
            m_spriteTitleBar.draw(target, states);
        else
            drawRectangleShape(target, states, {sizeIncludingBorders.x, m_titleBarHeightCached}, m_titleBarColorCached);

        // Draw the text in the title bar (after setting the clipping area)
        {
            float buttonOffsetX = 0;
            for (const auto& button : {m_closeButton, m_maximizeButton, m_minimizeButton})
            {
                if (button)
                    buttonOffsetX += (buttonOffsetX > 0 ? m_paddingBetweenButtonsCached : 0) + button->getSize().x;
            }

            if (buttonOffsetX > 0)
                buttonOffsetX += m_distanceToSideCached;

            float clippingLeft = m_distanceToSideCached;
            float clippingRight = sizeIncludingBorders.x - m_distanceToSideCached - buttonOffsetX;

            Clipping clipping{target, states, {clippingLeft, 0}, {clippingRight - clippingLeft, m_titleBarHeightCached}};

            m_titleText.draw(target, states);
        }

        // Draw the buttons
        for (auto& button : {m_closeButton, m_minimizeButton, m_maximizeButton})
        {
            if (button)
                button->draw(target, states);
        }

        states.transform.translate({0, m_titleBarHeightCached});

        // Draw the borders
        if (m_bordersCached != Borders{0})
        {
            drawBorders(target, states, m_bordersCached, sizeIncludingBorders, m_borderColorCached);
            states.transform.translate({m_bordersCached.left, m_bordersCached.top});
        }

        // Draw the background
        if (m_backgroundColorCached != sf::Color::Transparent)
            drawRectangleShape(target, states, getSize(), m_backgroundColorCached);

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
