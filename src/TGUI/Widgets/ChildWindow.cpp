/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus' Graphical User Interface
// Copyright (C) 2012-2018 Bruno Van de Velde (vdv_b@tgui.eu)
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
#include <TGUI/Vector2f.hpp>
#include <TGUI/Clipping.hpp>
#include <TGUI/SignalImpl.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
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

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ChildWindow::ChildWindow(const sf::String& title, unsigned int titleButtons)
    {
        m_type = "ChildWindow";
        m_isolatedFocus = true;
        m_titleText.setFont(m_fontCached);

        m_renderer = aurora::makeCopied<ChildWindowRenderer>();
        setRenderer(Theme::getDefault()->getRendererNoThrow(m_type));

        setTitle(title);
        setTitleTextSize(getGlobalTextSize());
        m_titleBarHeightCached = m_titleText.getSize().y * 1.25f;
        setTitleButtons(titleButtons);
        setSize({400, 300});

        m_maximizeButton->connect("pressed", [this](){ onMaximize.emit(this); });
        m_minimizeButton->connect("pressed", [this](){ onMinimize.emit(this); });
        m_closeButton->connect("pressed", [this](){
                if (!onClose.emit(this))
                    destroy();
            });
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ChildWindow::Ptr ChildWindow::create(const sf::String& title, unsigned int titleButtons)
    {
        return std::make_shared<ChildWindow>(title, titleButtons);
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

    ChildWindowRenderer* ChildWindow::getSharedRenderer()
    {
        return aurora::downcast<ChildWindowRenderer*>(Widget::getSharedRenderer());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const ChildWindowRenderer* ChildWindow::getSharedRenderer() const
    {
        return aurora::downcast<const ChildWindowRenderer*>(Widget::getSharedRenderer());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ChildWindowRenderer* ChildWindow::getRenderer()
    {
        return aurora::downcast<ChildWindowRenderer*>(Widget::getRenderer());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const ChildWindowRenderer* ChildWindow::getRenderer() const
    {
        return aurora::downcast<const ChildWindowRenderer*>(Widget::getRenderer());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChildWindow::setPosition(const Layout2d& position)
    {
        float x = position.getValue().x;
        float y = position.getValue().y;

        if (m_keepInParent && m_parent && (m_parent->getSize().x > 0) && (m_parent->getSize().y > 0)
         && ((y < 0) || (y > m_parent->getSize().y - getFullSize().y) || (x < 0) || (x > m_parent->getSize().x - getFullSize().x)))
        {
            if (y < 0)
                y = 0;
            else if (y > m_parent->getSize().y - getFullSize().y)
                y = m_parent->getSize().y - getFullSize().y;

            if (x < 0)
                x = 0;
            else if (x > m_parent->getSize().x - getFullSize().x)
                x = m_parent->getSize().x - getFullSize().x;

            Container::setPosition({x, y});
        }
        else
            Container::setPosition(position);

        // Calculate the distance from the right side that the buttons will need
        float buttonOffsetX = 0;
        for (const auto& button : {m_closeButton.get(), m_maximizeButton.get(), m_minimizeButton.get()})
        {
            if (button->isVisible())
                buttonOffsetX += (buttonOffsetX > 0 ? m_paddingBetweenButtonsCached : 0) + button->getSize().x;
        }

        if (buttonOffsetX > 0)
            buttonOffsetX += m_distanceToSideCached;

        if (m_titleAlignment == TitleAlignment::Left)
        {
            m_titleText.setPosition(m_distanceToSideCached,
                                    (m_titleBarHeightCached - m_titleText.getSize().y) / 2.0f);
        }
        else if (m_titleAlignment == TitleAlignment::Center)
        {
            m_titleText.setPosition(m_distanceToSideCached + ((getSize().x - (2 * m_distanceToSideCached) - buttonOffsetX - m_titleText.getSize().x) / 2.0f),
                                    (m_titleBarHeightCached - m_titleText.getSize().y) / 2.0f);
        }
        else // if (m_titleAlignment == TitleAlignment::Right)
        {
            m_titleText.setPosition(getSize().x - m_distanceToSideCached - buttonOffsetX - m_titleText.getSize().x,
                                    (m_titleBarHeightCached - m_titleText.getSize().y) / 2.0f);
        }

        buttonOffsetX = m_distanceToSideCached;
        for (auto& button : {m_closeButton.get(), m_maximizeButton.get(), m_minimizeButton.get()})
        {
            if (button->isVisible())
            {
                button->setPosition(m_bordersCached.getLeft() + getSize().x - buttonOffsetX - button->getSize().x,
                                    m_bordersCached.getTop() + (m_titleBarHeightCached - button->getSize().y) / 2.f);

                buttonOffsetX += button->getSize().x + m_paddingBetweenButtonsCached;
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChildWindow::setSize(const Layout2d& size)
    {
        Container::setSize(size);

        m_bordersCached.updateParentSize(getSize());

        m_spriteTitleBar.setSize({getSize().x, m_titleBarHeightCached});

        // Reposition the images and text
        setPosition(m_position);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Vector2f ChildWindow::getFullSize() const
    {
        return {getSize().x + m_bordersCached.getLeft() + m_bordersCached.getRight(),
                getSize().y + m_bordersCached.getTop() + m_bordersCached.getBottom() + m_titleBarHeightCached + m_borderBelowTitleBarCached};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChildWindow::setMaximumSize(Vector2f size)
    {
        if ((size.x < getSize().x) || (size.y < getSize().y))
        {
            // The window is currently larger than the new maximum size, lets downsize
            setSize(std::min(size.x, getSize().x), std::min(size.y, getSize().y));
        }

        m_maximumSize = size;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Vector2f ChildWindow::getMaximumSize() const
    {
        return m_maximumSize;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChildWindow::setMinimumSize(Vector2f size)
    {
        if ((size.x > getSize().x) || (size.y > getSize().y))
        {
            // The window is currently smaller than the new minimum size, lets upsize
            setSize(std::max(size.x, getSize().x), std::max(size.y, getSize().y));
        }

        m_minimumSize = size;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Vector2f ChildWindow::getMinimumSize() const
    {
        return m_minimumSize;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChildWindow::setTitle(const sf::String& title)
    {
        m_titleText.setString(title);

        // Reposition the images and text
        setPosition(m_position);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const sf::String& ChildWindow::getTitle() const
    {
        return m_titleText.getString();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChildWindow::setTitleTextSize(unsigned int size)
    {
        m_titleTextSize = size;

        if (m_titleTextSize)
            m_titleText.setCharacterSize(m_titleTextSize);
        else
            m_titleText.setCharacterSize(Text::findBestTextSize(m_fontCached, m_titleBarHeightCached * 0.8f));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int ChildWindow::getTitleTextSize() const
    {
        return m_titleTextSize;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChildWindow::setTitleAlignment(TitleAlignment alignment)
    {
        m_titleAlignment = alignment;

        // Reposition the images and text
        setPosition(m_position);
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
            m_closeButton->setVisible(true);
            m_closeButton->setRenderer(getSharedRenderer()->getCloseButton());
            m_closeButton->setInheritedOpacity(m_opacityCached);

            if (m_showTextOnTitleButtonsCached)
                m_closeButton->setText(L"✕");
            else
                m_closeButton->setText("");
        }
        else
            m_closeButton->setVisible(false);

        if (m_titleButtons & TitleButton::Maximize)
        {
            m_maximizeButton->setVisible(true);
            m_maximizeButton->setRenderer(getSharedRenderer()->getMaximizeButton());
            m_maximizeButton->setInheritedOpacity(m_opacityCached);

            if (m_showTextOnTitleButtonsCached)
                m_maximizeButton->setText(L"☐");
            else
                m_maximizeButton->setText("");
        }
        else
            m_maximizeButton->setVisible(false);

        if (m_titleButtons & TitleButton::Minimize)
        {
            m_minimizeButton->setVisible(true);
            m_minimizeButton->setRenderer(getSharedRenderer()->getMinimizeButton());
            m_minimizeButton->setInheritedOpacity(m_opacityCached);

            if (m_showTextOnTitleButtonsCached)
                m_minimizeButton->setText(L"⁃");
            else
                m_minimizeButton->setText("");
        }
        else
            m_minimizeButton->setVisible(false);

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

    void ChildWindow::setKeepInParent(bool enabled)
    {
        m_keepInParent = enabled;

        if (enabled)
            setPosition(m_position);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ChildWindow::isKeptInParent() const
    {
        return m_keepInParent;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Vector2f ChildWindow::getChildWidgetsOffset() const
    {
        return {m_bordersCached.getLeft(), m_bordersCached.getTop() + m_titleBarHeightCached + m_borderBelowTitleBarCached};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChildWindow::setParent(Container* parent)
    {
        Container::setParent(parent);
        if (m_keepInParent)
            setPosition(m_position);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ChildWindow::mouseOnWidget(Vector2f pos) const
    {
        FloatRect region{getPosition(), getFullSize()};

        // Expand the region if the child window is resizable (to make the borders easier to click on)
        if (m_resizable)
        {
            region.left -= std::max(0.f, m_MinimumResizableBorderWidthCached - m_bordersCached.getLeft());
            region.top -= std::max(0.f, m_MinimumResizableBorderWidthCached - m_bordersCached.getTop());
            region.width += std::max(0.f, m_MinimumResizableBorderWidthCached - m_bordersCached.getLeft())
                            + std::max(0.f, m_MinimumResizableBorderWidthCached - m_bordersCached.getRight());
            region.height += std::max(0.f, m_MinimumResizableBorderWidthCached - m_bordersCached.getTop())
                             + std::max(0.f, m_MinimumResizableBorderWidthCached - m_bordersCached.getBottom());
        }

        if (region.contains(pos))
        {
            // If the mouse enters the border or title bar then then none of the widgets can still be under the mouse
            if (m_widgetBelowMouse && !FloatRect{getPosition() + getChildWidgetsOffset(), getSize()}.contains(pos))
                m_widgetBelowMouse->mouseNoLongerOnWidget();

            return true;
        }

        return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChildWindow::leftMousePressed(Vector2f pos)
    {
        pos -= getPosition();

        m_mouseDown = true;

        // Move the child window to the front
        moveToFront();

        onMousePress.emit(this);

        if (FloatRect{getChildWidgetsOffset(), getSize()}.contains(pos))
        {
            // Propagate the event to the child widgets
            Container::leftMousePressed(pos + getPosition());
        }
        else if (!FloatRect{m_bordersCached.getLeft(), m_bordersCached.getTop(), getSize().x, getSize().y + m_titleBarHeightCached + m_borderBelowTitleBarCached}.contains(pos))
        {
            // Mouse is on top of the borders
            if (m_resizable)
            {
                // Check on which border the mouse is standing
                m_resizeDirection = ResizeNone;
                if (pos.x < m_bordersCached.getLeft())
                    m_resizeDirection |= ResizeLeft;
                if (pos.y < m_bordersCached.getTop())
                    m_resizeDirection |= ResizeTop;
                if (pos.x >= getFullSize().x - m_bordersCached.getRight())
                    m_resizeDirection |= ResizeRight;
                if (pos.y >= getFullSize().y - m_bordersCached.getBottom())
                    m_resizeDirection |= ResizeBottom;
            }

            m_draggingPosition = pos;
        }
        else if (FloatRect{m_bordersCached.getLeft(), m_bordersCached.getTop(), getSize().x, m_titleBarHeightCached}.contains(pos))
        {
            // Send the mouse press event to the title buttons
            for (auto& button : {m_closeButton.get(), m_maximizeButton.get(), m_minimizeButton.get()})
            {
                if (button->isVisible() && button->mouseOnWidget(pos))
                {
                    button->leftMousePressed(pos);
                    return;
                }
            }

            // The mouse went down on the title bar
            m_mouseDownOnTitleBar = true;
            m_draggingPosition = pos;
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChildWindow::leftMouseReleased(Vector2f pos)
    {
        pos -= getPosition();

        m_mouseDownOnTitleBar = false;
        m_resizeDirection = ResizeNone;

        if (FloatRect{getChildWidgetsOffset(), getSize()}.contains(pos))
        {
            // Propagate the event to the child widgets
            Container::leftMouseReleased(pos + getPosition());
        }
        else
        {
            // Tell the widgets that the mouse was released
            for (auto& widget : m_widgets)
                widget->mouseNoLongerDown();

            // Check if the mouse is on top of the title bar
            if (FloatRect{m_bordersCached.getLeft(), m_bordersCached.getTop(), getSize().x, m_titleBarHeightCached}.contains(pos))
            {
                // Send the mouse release event to the title buttons
                for (auto& button : {m_closeButton.get(), m_maximizeButton.get(), m_minimizeButton.get()})
                {
                    if (button->isVisible() && button->mouseOnWidget(pos))
                    {
                        button->leftMouseReleased(pos);
                        break;
                    }
                }
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChildWindow::mouseMoved(Vector2f pos)
    {
        pos -= getPosition();

        // Check if you are dragging the child window
        if (m_mouseDown && m_mouseDownOnTitleBar)
        {
            // Move the child window, but don't allow the dragging position to leave the screen
            sf::Vector2f newPosition;
            if (getPosition().x + pos.x <= 0)
                newPosition.x = -m_draggingPosition.x + 1;
            else if (m_parent && getPosition().x + pos.x >= m_parent->getSize().x)
                newPosition.x = m_parent->getSize().x - m_draggingPosition.x - 1;
            else
                newPosition.x = getPosition().x + (pos.x - m_draggingPosition.x);

            if (getPosition().y + pos.y <= 0)
                newPosition.y = -m_draggingPosition.y + 1;
            else if (m_parent && getPosition().y + pos.y >= m_parent->getSize().y)
                newPosition.y = m_parent->getSize().y - m_draggingPosition.y - 1;
            else
                newPosition.y = getPosition().y + (pos.y - m_draggingPosition.y);

            setPosition(newPosition);
        }

        // Check if you are resizing the window
        else if (m_mouseDown && m_resizeDirection != ResizeNone)
        {
            float minimumWidth = 0;
            for (const auto& button : {m_closeButton.get(), m_maximizeButton.get(), m_minimizeButton.get()})
            {
                if (button->isVisible())
                    minimumWidth += (minimumWidth > 0 ? m_paddingBetweenButtonsCached : 0) + button->getSize().x;
            }
            minimumWidth += 2 * m_distanceToSideCached;

            if ((m_resizeDirection & ResizeLeft) != 0)
            {
                const float diff = clamp(m_draggingPosition.x - pos.x, std::max(minimumWidth, m_minimumSize.x) - getSize().x, m_maximumSize.x - getSize().x);
                setPosition(getPosition().x - diff, getPosition().y);
                setSize(getSize().x + diff, getSize().y);
            }
            else if ((m_resizeDirection & ResizeRight) != 0)
            {
                const float diff = clamp(pos.x - m_draggingPosition.x, std::max(minimumWidth, m_minimumSize.x) - getSize().x, m_maximumSize.x - getSize().x);
                setSize(getSize().x + diff, getSize().y);
                m_draggingPosition.x += diff;
            }

            if ((m_resizeDirection & ResizeTop) != 0)
            {
                const float diff = clamp(m_draggingPosition.y - pos.y, m_minimumSize.y - getSize().y, m_maximumSize.y - getSize().y);
                setPosition(getPosition().x, getPosition().y - diff);
                setSize(getSize().x, getSize().y + diff);
            }
            else if ((m_resizeDirection & ResizeBottom) != 0)
            {
                const float diff = clamp(pos.y - m_draggingPosition.y, m_minimumSize.y - getSize().y, m_maximumSize.y - getSize().y);
                setSize(getSize().x, getSize().y + diff);
                m_draggingPosition.y += diff;
            }
        }
        else // Not dragging child window
        {
            if (FloatRect{getChildWidgetsOffset(), getSize()}.contains(pos))
            {
                // Propagate the event to the child widgets
                Container::mouseMoved(pos + getPosition());
            }
            else
            {
                if (!m_mouseHover)
                    mouseEnteredWidget();

                // Check if the mouse is on top of the title bar
                if (FloatRect{m_bordersCached.getLeft(), m_bordersCached.getTop(), getSize().x, m_titleBarHeightCached}.contains(pos))
                {
                    // Send the hover event to the buttons inside the title bar
                    for (auto& button : {m_closeButton.get(), m_maximizeButton.get(), m_minimizeButton.get()})
                    {
                        if (button->isVisible())
                        {
                            if (button->mouseOnWidget(pos))
                                button->mouseMoved(pos);
                            else
                                button->mouseNoLongerOnWidget();
                        }
                    }
                }
                else // When the mouse is not on the title bar, the mouse can't be on the buttons inside it
                {
                    for (auto& button : {m_closeButton.get(), m_maximizeButton.get(), m_minimizeButton.get()})
                    {
                        if (button->isVisible())
                            button->mouseNoLongerOnWidget();
                    }
                }
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChildWindow::mouseNoLongerOnWidget()
    {
        Container::mouseNoLongerOnWidget();

        for (const auto& button : {m_closeButton.get(), m_maximizeButton.get(), m_minimizeButton.get()})
        {
            if (button->isVisible())
                button->mouseNoLongerOnWidget();
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChildWindow::mouseNoLongerDown()
    {
        Container::mouseNoLongerDown();

        for (const auto& button : {m_closeButton.get(), m_maximizeButton.get(), m_minimizeButton.get()})
        {
            if (button->isVisible())
                button->mouseNoLongerDown();
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChildWindow::updateTitleBarHeight()
    {
        m_spriteTitleBar.setSize({getSize().x, m_titleBarHeightCached});

        // Set the size of the buttons in the title bar
        for (auto& button : {m_closeButton.get(), m_maximizeButton.get(), m_minimizeButton.get()})
        {
            if (button->isVisible())
            {
                if (m_spriteTitleBar.isSet() && (button->getSharedRenderer()->getTexture().getData() != nullptr))
                {
                    button->setSize(button->getSharedRenderer()->getTexture().getImageSize().x * (m_titleBarHeightCached / m_spriteTitleBar.getTexture().getImageSize().y),
                                    button->getSharedRenderer()->getTexture().getImageSize().y * (m_titleBarHeightCached / m_spriteTitleBar.getTexture().getImageSize().y));
                }
                else
                    button->setSize({m_titleBarHeightCached * 0.8f, m_titleBarHeightCached * 0.8f});

                const Borders& buttonBorders = button->getSharedRenderer()->getBorders();
                button->setTextSize(Text::findBestTextSize(m_titleText.getFont(), (button->getSize().y - buttonBorders.getTop() - buttonBorders.getBottom()) * 0.8f));
            }
        }

        if (m_titleTextSize == 0)
            m_titleText.setCharacterSize(Text::findBestTextSize(m_fontCached, m_titleBarHeightCached * 0.8f));

        // Reposition the images and text
        setPosition(m_position);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Signal& ChildWindow::getSignal(std::string signalName)
    {
        if (signalName == toLower(onMousePress.getName()))
            return onMousePress;
        else if (signalName == toLower(onClose.getName()))
            return onClose;
        else if (signalName == toLower(onMinimize.getName()))
            return onMinimize;
        else if (signalName == toLower(onMaximize.getName()))
            return onMaximize;
        else
            return Container::getSignal(std::move(signalName));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChildWindow::rendererChanged(const std::string& property)
    {
        if (property == "borders")
        {
            m_bordersCached = getSharedRenderer()->getBorders();
            setSize(m_size);
        }
        else if (property == "titlecolor")
        {
            m_titleText.setColor(getSharedRenderer()->getTitleColor());
        }
        else if (property == "texturetitlebar")
        {
            m_spriteTitleBar.setTexture(getSharedRenderer()->getTextureTitleBar());

            // If the title bar height is determined by the texture then update it (note that getTitleBarHeight has a non-trivial implementation)
            m_titleBarHeightCached = getSharedRenderer()->getTitleBarHeight();
            if (m_titleBarHeightCached == m_spriteTitleBar.getTexture().getImageSize().y)
                updateTitleBarHeight();
        }
        else if (property == "titlebarheight")
        {
            m_titleBarHeightCached = getSharedRenderer()->getTitleBarHeight();
            updateTitleBarHeight();
        }
        else if (property == "borderbelowtitlebar")
        {
            m_borderBelowTitleBarCached = getSharedRenderer()->getBorderBelowTitleBar();
        }
        else if (property == "distancetoside")
        {
            m_distanceToSideCached = getSharedRenderer()->getDistanceToSide();
            setPosition(m_position);
        }
        else if (property == "paddingbetweenbuttons")
        {
            m_paddingBetweenButtonsCached = getSharedRenderer()->getPaddingBetweenButtons();
            setPosition(m_position);
        }
        else if (property == "minimumresizableborderwidth")
        {
            m_MinimumResizableBorderWidthCached = getSharedRenderer()->getMinimumResizableBorderWidth();
        }
        else if (property == "showtextontitlebuttons")
        {
            m_showTextOnTitleButtonsCached = getSharedRenderer()->getShowTextOnTitleButtons();
            setTitleButtons(m_titleButtons);
        }
        else if (property == "closebutton")
        {
            if (m_closeButton->isVisible())
            {
                m_closeButton->setRenderer(getSharedRenderer()->getCloseButton());
                m_closeButton->setInheritedOpacity(m_opacityCached);
            }

            updateTitleBarHeight();
        }
        else if (property == "maximizebutton")
        {
            if (m_maximizeButton->isVisible())
            {
                m_maximizeButton->setRenderer(getSharedRenderer()->getMaximizeButton());
                m_maximizeButton->setInheritedOpacity(m_opacityCached);
            }

            updateTitleBarHeight();
        }
        else if (property == "minimizebutton")
        {
            if (m_minimizeButton->isVisible())
            {
                m_minimizeButton->setRenderer(getSharedRenderer()->getMinimizeButton());
                m_minimizeButton->setInheritedOpacity(m_opacityCached);
            }

            updateTitleBarHeight();
        }
        else if (property == "backgroundcolor")
        {
            m_backgroundColorCached = getSharedRenderer()->getBackgroundColor();
        }
        else if (property == "titlebarcolor")
        {
            m_titleBarColorCached = getSharedRenderer()->getTitleBarColor();
        }
        else if (property == "bordercolor")
        {
            m_borderColorCached = getSharedRenderer()->getBorderColor();
        }
        else if (property == "opacity")
        {
            Container::rendererChanged(property);

            for (auto& button : {m_closeButton.get(), m_maximizeButton.get(), m_minimizeButton.get()})
            {
                if (button->isVisible())
                    button->setInheritedOpacity(m_opacityCached);
            }

            m_titleText.setOpacity(m_opacityCached);
            m_spriteTitleBar.setOpacity(m_opacityCached);
        }
        else if (property == "font")
        {
            Container::rendererChanged(property);

            for (auto& button : {m_closeButton.get(), m_maximizeButton.get(), m_minimizeButton.get()})
            {
                if (button->isVisible())
                    button->setInheritedFont(m_fontCached);
            }

            m_titleText.setFont(m_fontCached);
            if (m_titleTextSize == 0)
                m_titleText.setCharacterSize(Text::findBestTextSize(m_fontCached, getSharedRenderer()->getTitleBarHeight() * 0.8f));

            setPosition(m_position);
        }
        else
            Container::rendererChanged(property);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::unique_ptr<DataIO::Node> ChildWindow::save(SavingRenderersMap& renderers) const
    {
        auto node = Container::save(renderers);

        if (m_titleAlignment == ChildWindow::TitleAlignment::Left)
            node->propertyValuePairs["TitleAlignment"] = std::make_unique<DataIO::ValueNode>("Left");
        else if (m_titleAlignment == ChildWindow::TitleAlignment::Center)
            node->propertyValuePairs["TitleAlignment"] = std::make_unique<DataIO::ValueNode>("Center");
        else if (m_titleAlignment == ChildWindow::TitleAlignment::Right)
            node->propertyValuePairs["TitleAlignment"] = std::make_unique<DataIO::ValueNode>("Right");

        if (getTitle().getSize() > 0)
            node->propertyValuePairs["Title"] = std::make_unique<DataIO::ValueNode>(Serializer::serialize(getTitle()));

        if (m_keepInParent)
            node->propertyValuePairs["KeepInParent"] = std::make_unique<DataIO::ValueNode>("true");

        if (m_resizable)
            node->propertyValuePairs["Resizable"] = std::make_unique<DataIO::ValueNode>("true");

        if (m_minimumSize != Vector2f{})
            node->propertyValuePairs["MinimumSize"] = std::make_unique<DataIO::ValueNode>("(" + to_string(m_minimumSize.x) + ", " + to_string(m_minimumSize.y) + ")");
        if (m_maximumSize != Vector2f{std::numeric_limits<float>::infinity(), std::numeric_limits<float>::infinity()})
            node->propertyValuePairs["MaximumSize"] = std::make_unique<DataIO::ValueNode>("(" + to_string(m_maximumSize.x) + ", " + to_string(m_maximumSize.y) + ")");

        std::string serializedTitleButtons;
        if (m_titleButtons & ChildWindow::TitleButton::Minimize)
            serializedTitleButtons += " | Minimize";
        if (m_titleButtons & ChildWindow::TitleButton::Maximize)
            serializedTitleButtons += " | Maximize";
        if (m_titleButtons & ChildWindow::TitleButton::Close)
            serializedTitleButtons += " | Close";

        if (!serializedTitleButtons.empty())
            serializedTitleButtons.erase(0, 3);
        else
            serializedTitleButtons = "None";

        node->propertyValuePairs["TitleButtons"] = std::make_unique<DataIO::ValueNode>(serializedTitleButtons);

        return node;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChildWindow::load(const std::unique_ptr<DataIO::Node>& node, const LoadingRenderersMap& renderers)
    {
        Container::load(node, renderers);

        if (node->propertyValuePairs["titlealignment"])
        {
            if (toLower(node->propertyValuePairs["titlealignment"]->value) == "left")
                setTitleAlignment(ChildWindow::TitleAlignment::Left);
            else if (toLower(node->propertyValuePairs["titlealignment"]->value) == "center")
                setTitleAlignment(ChildWindow::TitleAlignment::Center);
            else if (toLower(node->propertyValuePairs["titlealignment"]->value) == "right")
                setTitleAlignment(ChildWindow::TitleAlignment::Right);
            else
                throw Exception{"Failed to parse TitleAlignment property. Only the values Left, Center and Right are correct."};
        }

        if (node->propertyValuePairs["titlebuttons"])
        {
            int decodedTitleButtons = ChildWindow::TitleButton::None;
            std::vector<std::string> titleButtons = Deserializer::split(node->propertyValuePairs["titlebuttons"]->value, '|');
            for (const auto& elem : titleButtons)
            {
                std::string requestedTitleButton = toLower(trim(elem));
                if (requestedTitleButton == "close")
                    decodedTitleButtons |= sf::Text::Bold;
                else if (requestedTitleButton == "maximize")
                    decodedTitleButtons |= sf::Text::Italic;
                else if (requestedTitleButton == "minimize")
                    decodedTitleButtons |= sf::Text::Underlined;
            }

            setTitleButtons(decodedTitleButtons);
        }

        if (node->propertyValuePairs["title"])
            setTitle(Deserializer::deserialize(ObjectConverter::Type::String, node->propertyValuePairs["title"]->value).getString());

        if (node->propertyValuePairs["keepinparent"])
            setKeepInParent(Deserializer::deserialize(ObjectConverter::Type::Bool, node->propertyValuePairs["keepinparent"]->value).getBool());

        if (node->propertyValuePairs["resizable"])
            setResizable(Deserializer::deserialize(ObjectConverter::Type::Bool, node->propertyValuePairs["resizable"]->value).getBool());

        if (node->propertyValuePairs["minimumsize"])
            setMinimumSize(Vector2f{node->propertyValuePairs["minimumsize"]->value});

        if (node->propertyValuePairs["maximumsize"])
            setMaximumSize(Vector2f{node->propertyValuePairs["maximumsize"]->value});
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChildWindow::draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        states.transform.translate(getPosition());

        // Draw the borders
        if (m_bordersCached != Borders{0})
        {
            drawBorders(target, states, m_bordersCached, getFullSize(), m_borderColorCached);
            states.transform.translate({m_bordersCached.getLeft(), m_bordersCached.getTop()});
        }

        // Draw the title bar
        if (m_spriteTitleBar.isSet())
            m_spriteTitleBar.draw(target, states);
        else
            drawRectangleShape(target, states, {getSize().x, m_titleBarHeightCached}, m_titleBarColorCached);

        // Draw the text in the title bar (after setting the clipping area)
        {
            float buttonOffsetX = 0;
            for (const auto& button : {m_closeButton.get(), m_maximizeButton.get(), m_minimizeButton.get()})
            {
                if (button->isVisible())
                    buttonOffsetX += (buttonOffsetX > 0 ? m_paddingBetweenButtonsCached : 0) + button->getSize().x;
            }

            if (buttonOffsetX > 0)
                buttonOffsetX += m_distanceToSideCached;

            const float clippingLeft = m_distanceToSideCached;
            const float clippingRight = getSize().x - m_distanceToSideCached - buttonOffsetX;

            const Clipping clipping{target, states, {clippingLeft, 0}, {clippingRight - clippingLeft, m_titleBarHeightCached}};

            m_titleText.draw(target, states);
        }

        // Draw the buttons
        states.transform.translate({-m_bordersCached.getLeft(), -m_bordersCached.getTop()});
        for (auto& button : {m_closeButton.get(), m_maximizeButton.get(), m_minimizeButton.get()})
        {
            if (button->isVisible())
                button->draw(target, states);
        }

        states.transform.translate({m_bordersCached.getLeft(), m_bordersCached.getTop() + m_titleBarHeightCached});

        // Draw the border below the title bar
        if (m_borderBelowTitleBarCached > 0)
        {
            drawRectangleShape(target, states, {getSize().x, m_borderBelowTitleBarCached}, m_borderColorCached);
            states.transform.translate({0, m_borderBelowTitleBarCached});
        }

        // Draw the background
        if (m_backgroundColorCached != Color::Transparent)
            drawRectangleShape(target, states, getSize(), m_backgroundColorCached);

        // Draw the widgets in the child window
        const Clipping clipping{target, states, {}, {getSize()}};
        drawWidgetContainer(&target, states);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
