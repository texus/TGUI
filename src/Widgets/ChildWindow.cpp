/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus' Graphical User Interface
// Copyright (C) 2012-2023 Bruno Van de Velde (vdv_b@tgui.eu)
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
#include <TGUI/Vector2.hpp>
#include <TGUI/Backend/Window/BackendGui.hpp>

#if TGUI_HAS_WINDOW_BACKEND_SFML
    #include <SFML/Config.hpp>
#endif

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

#if TGUI_COMPILED_WITH_CPP_VER < 17
    constexpr const char ChildWindow::StaticWidgetType[];
#endif

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ChildWindow::ChildWindow(const char* typeName, bool initRenderer) :
        Container{typeName, false}
    {
        m_isolatedFocus = true;
        m_titleText.setFont(m_fontCached);

        setTitleTextSize(getGlobalTextSize());
        m_titleBarHeightCached = m_titleText.getSize().y * 1.25f;
        if (m_decorationLayoutY && (m_decorationLayoutY == m_size.y.getRightOperand()))
            m_decorationLayoutY->replaceValue(m_bordersCached.getTop() + m_bordersCached.getBottom() + m_titleBarHeightCached + m_borderBelowTitleBarCached);

        if (initRenderer)
        {
            m_renderer = aurora::makeCopied<ChildWindowRenderer>();
            setRenderer(Theme::getDefault()->getRendererNoThrow(m_type));

            setSize({400, 300});
        }

        connectTitleButtonCallbacks();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ChildWindow::ChildWindow(const ChildWindow& other) :
        Container                          {other},
        m_titleText                        {other.m_titleText},
        m_draggingPosition                 {other.m_draggingPosition},
        m_maximumSize                      {other.m_maximumSize},
        m_minimumSize                      {other.m_minimumSize},
        m_titleAlignment                   {other.m_titleAlignment},
        m_titleButtons                     {other.m_titleButtons},
        m_titleTextSize                    {other.m_titleTextSize},
        m_currentChildWindowMouseCursor    {other.m_currentChildWindowMouseCursor},
        m_closeButton                      {other.m_closeButton},
        m_minimizeButton                   {other.m_minimizeButton},
        m_maximizeButton                   {other.m_maximizeButton},
        m_mouseDownOnTitleBar              {other.m_mouseDownOnTitleBar},
        m_keepInParent                     {other.m_keepInParent},
        m_positionLocked                   {other.m_positionLocked},
        m_resizable                        {other.m_resizable},
        m_resizeDirection                  {other.m_resizeDirection},
        m_spriteTitleBar                   {other.m_spriteTitleBar},
        m_spriteBackground                 {other.m_spriteBackground},
        m_bordersCached                    {other.m_bordersCached},
        m_borderColorCached                {other.m_borderColorCached},
        m_borderColorFocusedCached         {other.m_borderColorFocusedCached},
        m_titleColorCached                 {other.m_titleColorCached},
        m_titleBarColorCached              {other.m_titleBarColorCached},
        m_backgroundColorCached            {other.m_backgroundColorCached},
        m_titleBarHeightCached             {other.m_titleBarHeightCached},
        m_borderBelowTitleBarCached        {other.m_borderBelowTitleBarCached},
        m_distanceToSideCached             {other.m_distanceToSideCached},
        m_paddingBetweenButtonsCached      {other.m_paddingBetweenButtonsCached},
        m_minimumResizableBorderWidthCached{other.m_minimumResizableBorderWidthCached},
        m_showTextOnTitleButtonsCached     {other.m_showTextOnTitleButtonsCached}
    {
        // The inner size has changed since the container created the child widgets
        recalculateBoundSizeLayouts();

        connectTitleButtonCallbacks();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ChildWindow::ChildWindow(ChildWindow&& other) noexcept :
        Container                          {std::move(other)},
        m_titleText                        {std::move(other.m_titleText)},
        m_draggingPosition                 {std::move(other.m_draggingPosition)},
        m_maximumSize                      {std::move(other.m_maximumSize)},
        m_minimumSize                      {std::move(other.m_minimumSize)},
        m_titleAlignment                   {std::move(other.m_titleAlignment)},
        m_titleButtons                     {std::move(other.m_titleButtons)},
        m_titleTextSize                    {std::move(other.m_titleTextSize)},
        m_currentChildWindowMouseCursor    {std::move(other.m_currentChildWindowMouseCursor)},
        m_closeButton                      {std::move(other.m_closeButton)},
        m_minimizeButton                   {std::move(other.m_minimizeButton)},
        m_maximizeButton                   {std::move(other.m_maximizeButton)},
        m_mouseDownOnTitleBar              {std::move(other.m_mouseDownOnTitleBar)},
        m_keepInParent                     {std::move(other.m_keepInParent)},
        m_positionLocked                   {std::move(other.m_positionLocked)},
        m_resizable                        {std::move(other.m_resizable)},
        m_resizeDirection                  {std::move(other.m_resizeDirection)},
        m_spriteTitleBar                   {std::move(other.m_spriteTitleBar)},
        m_spriteBackground                 {std::move(other.m_spriteBackground)},
        m_bordersCached                    {std::move(other.m_bordersCached)},
        m_borderColorCached                {std::move(other.m_borderColorCached)},
        m_borderColorFocusedCached         {std::move(other.m_borderColorFocusedCached)},
        m_titleColorCached                 {std::move(other.m_titleColorCached)},
        m_titleBarColorCached              {std::move(other.m_titleBarColorCached)},
        m_backgroundColorCached            {std::move(other.m_backgroundColorCached)},
        m_titleBarHeightCached             {std::move(other.m_titleBarHeightCached)},
        m_borderBelowTitleBarCached        {std::move(other.m_borderBelowTitleBarCached)},
        m_distanceToSideCached             {std::move(other.m_distanceToSideCached)},
        m_paddingBetweenButtonsCached      {std::move(other.m_paddingBetweenButtonsCached)},
        m_minimumResizableBorderWidthCached{std::move(other.m_minimumResizableBorderWidthCached)},
        m_showTextOnTitleButtonsCached     {std::move(other.m_showTextOnTitleButtonsCached)}
    {
        // The inner size has changed since the container created the child widgets
        recalculateBoundSizeLayouts();

        connectTitleButtonCallbacks();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ChildWindow& ChildWindow::operator= (const ChildWindow& other)
    {
        // Make sure it is not the same widget
        if (this != &other)
        {
            Container::operator=(other);

            m_titleText                         = other.m_titleText;
            m_draggingPosition                  = other.m_draggingPosition;
            m_maximumSize                       = other.m_maximumSize;
            m_minimumSize                       = other.m_minimumSize;
            m_titleAlignment                    = other.m_titleAlignment;
            m_titleButtons                      = other.m_titleButtons;
            m_titleTextSize                     = other.m_titleTextSize;
            m_currentChildWindowMouseCursor     = other.m_currentChildWindowMouseCursor;
            m_closeButton                       = other.m_closeButton;
            m_minimizeButton                    = other.m_minimizeButton;
            m_maximizeButton                    = other.m_maximizeButton;
            m_mouseDownOnTitleBar               = other.m_mouseDownOnTitleBar;
            m_keepInParent                      = other.m_keepInParent;
            m_positionLocked                    = other.m_positionLocked;
            m_resizable                         = other.m_resizable;
            m_resizeDirection                   = other.m_resizeDirection;
            m_spriteTitleBar                    = other.m_spriteTitleBar;
            m_spriteBackground                  = other.m_spriteBackground;
            m_bordersCached                     = other.m_bordersCached;
            m_borderColorCached                 = other.m_borderColorCached;
            m_borderColorFocusedCached          = other.m_borderColorFocusedCached;
            m_titleColorCached                  = other.m_titleColorCached;
            m_titleBarColorCached               = other.m_titleBarColorCached;
            m_backgroundColorCached             = other.m_backgroundColorCached;
            m_titleBarHeightCached              = other.m_titleBarHeightCached;
            m_borderBelowTitleBarCached         = other.m_borderBelowTitleBarCached;
            m_distanceToSideCached              = other.m_distanceToSideCached;
            m_paddingBetweenButtonsCached       = other.m_paddingBetweenButtonsCached;
            m_minimumResizableBorderWidthCached = other.m_minimumResizableBorderWidthCached;
            m_showTextOnTitleButtonsCached      = other.m_showTextOnTitleButtonsCached;

            // The inner size has changed since the container created the child widgets
            recalculateBoundSizeLayouts();

            connectTitleButtonCallbacks();
        }

        return *this;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ChildWindow& ChildWindow::operator= (ChildWindow&& other) noexcept
    {
        // Make sure it is not the same widget
        if (this != &other)
        {
            m_titleText                         = std::move(other.m_titleText);
            m_draggingPosition                  = std::move(other.m_draggingPosition);
            m_maximumSize                       = std::move(other.m_maximumSize);
            m_minimumSize                       = std::move(other.m_minimumSize);
            m_titleAlignment                    = std::move(other.m_titleAlignment);
            m_titleButtons                      = std::move(other.m_titleButtons);
            m_titleTextSize                     = std::move(other.m_titleTextSize);
            m_currentChildWindowMouseCursor     = std::move(other.m_currentChildWindowMouseCursor);
            m_closeButton                       = std::move(other.m_closeButton);
            m_minimizeButton                    = std::move(other.m_minimizeButton);
            m_maximizeButton                    = std::move(other.m_maximizeButton);
            m_mouseDownOnTitleBar               = std::move(other.m_mouseDownOnTitleBar);
            m_keepInParent                      = std::move(other.m_keepInParent);
            m_positionLocked                    = std::move(other.m_positionLocked);
            m_resizable                         = std::move(other.m_resizable);
            m_resizeDirection                   = std::move(other.m_resizeDirection);
            m_spriteTitleBar                    = std::move(other.m_spriteTitleBar);
            m_spriteBackground                  = std::move(other.m_spriteBackground);
            m_bordersCached                     = std::move(other.m_bordersCached);
            m_borderColorCached                 = std::move(other.m_borderColorCached);
            m_borderColorFocusedCached          = std::move(other.m_borderColorFocusedCached);
            m_titleColorCached                  = std::move(other.m_titleColorCached);
            m_titleBarColorCached               = std::move(other.m_titleBarColorCached);
            m_backgroundColorCached             = std::move(other.m_backgroundColorCached);
            m_titleBarHeightCached              = std::move(other.m_titleBarHeightCached);
            m_borderBelowTitleBarCached         = std::move(other.m_borderBelowTitleBarCached);
            m_distanceToSideCached              = std::move(other.m_distanceToSideCached);
            m_paddingBetweenButtonsCached       = std::move(other.m_paddingBetweenButtonsCached);
            m_minimumResizableBorderWidthCached = std::move(other.m_minimumResizableBorderWidthCached);
            m_showTextOnTitleButtonsCached      = std::move(other.m_showTextOnTitleButtonsCached);
            Container::operator=(std::move(other));

            // The inner size has changed since the container created the child widgets
            recalculateBoundSizeLayouts();

            connectTitleButtonCallbacks();
        }

        return *this;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ChildWindow::Ptr ChildWindow::create(const String& title, unsigned int titleButtons)
    {
        auto window = std::make_shared<ChildWindow>();
        window->setTitle(title);
        window->setTitleButtons(titleButtons);
        return window;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ChildWindow::Ptr ChildWindow::copy(const ChildWindow::ConstPtr& childWindow)
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

    void ChildWindow::setPosition(const Layout2d& position)
    {
        if (m_keepInParent && m_parent && (m_parent->getSize().x > 0) && (m_parent->getSize().y > 0))
        {
            const Vector2f origin{getOrigin().x * getSize().x, getOrigin().y * getSize().y};
            float x = position.getValue().x - origin.x;
            float y = position.getValue().y - origin.y;

            if ((y < 0) || (y > m_parent->getSize().y - getSize().y) || (x < 0) || (x > m_parent->getSize().x - getSize().x))
            {
                if (y < 0)
                    y = 0;
                else if (y > m_parent->getSize().y - getSize().y)
                    y = std::max(0.f, m_parent->getSize().y - getSize().y);

                if (x < 0)
                    x = 0;
                else if (x > m_parent->getSize().x - getSize().x)
                    x = std::max(0.f, m_parent->getSize().x - getSize().x);

                Container::setPosition({x + origin.x, y + origin.y});
            }
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
            m_titleText.setPosition({m_distanceToSideCached, (m_titleBarHeightCached - m_titleText.getSize().y) / 2.0f});
        }
        else if (m_titleAlignment == TitleAlignment::Center)
        {
            m_titleText.setPosition({m_distanceToSideCached + ((getClientSize().x - (2 * m_distanceToSideCached) - buttonOffsetX - m_titleText.getSize().x) / 2.0f),
                                     (m_titleBarHeightCached - m_titleText.getSize().y) / 2.0f});
        }
        else // if (m_titleAlignment == TitleAlignment::Right)
        {
            m_titleText.setPosition({getClientSize().x - m_distanceToSideCached - buttonOffsetX - m_titleText.getSize().x,
                                     (m_titleBarHeightCached - m_titleText.getSize().y) / 2.0f});
        }

        buttonOffsetX = m_distanceToSideCached;
        for (auto& button : {m_closeButton.get(), m_maximizeButton.get(), m_minimizeButton.get()})
        {
            if (button->isVisible())
            {
                button->setPosition(m_bordersCached.getLeft() + getClientSize().x - buttonOffsetX - button->getSize().x,
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

        m_spriteTitleBar.setSize({getClientSize().x, m_titleBarHeightCached});
        m_spriteBackground.setSize(getClientSize());

        // Reposition the images and text
        setPosition(m_position);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Vector2f ChildWindow::getInnerSize() const
    {
        return {std::max(0.f, getSize().x - m_bordersCached.getLeft() - m_bordersCached.getRight()),
                std::max(0.f, getSize().y - m_bordersCached.getTop() - m_bordersCached.getBottom() - m_titleBarHeightCached - m_borderBelowTitleBarCached)};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChildWindow::setClientSize(const Layout2d& size)
    {
        m_decorationLayoutX = nullptr;
        m_decorationLayoutY = nullptr;

        const Vector2f decorationSize = {m_bordersCached.getLeft() + m_bordersCached.getRight(),
                                         m_bordersCached.getTop() + m_bordersCached.getBottom() + m_titleBarHeightCached + m_borderBelowTitleBarCached};

        setSize(size + decorationSize);

        // Keep a pointer to the layout containing the decoration size. If the decoration changes then we need to update this layout
        m_decorationLayoutX = m_size.x.getRightOperand();
        m_decorationLayoutY = m_size.y.getRightOperand();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Vector2f ChildWindow::getClientSize() const
    {
        return getInnerSize();
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

    void ChildWindow::setTitle(const String& title)
    {
        m_titleText.setString(title);

        // Reposition the images and text
        setPosition(m_position);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const String& ChildWindow::getTitle() const
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

        // Reposition the title text
        updateTitleBarHeight();
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
                m_closeButton->setText(U"\u2715");
            else
                m_closeButton->setText("");
        }
        else
            m_closeButton->setVisible(false);

        if (m_titleButtons & TitleButton::Maximize)
        {
            auto buttonRenderer = getSharedRenderer()->getMaximizeButton();
            if (!buttonRenderer || (buttonRenderer->propertyValuePairs.empty() && !buttonRenderer->connectedTheme))
                buttonRenderer = getSharedRenderer()->getCloseButton();

            m_maximizeButton->setVisible(true);
            m_maximizeButton->setRenderer(buttonRenderer);
            m_maximizeButton->setInheritedOpacity(m_opacityCached);

            if (m_showTextOnTitleButtonsCached)
                m_maximizeButton->setText(U"\u2610");
            else
                m_maximizeButton->setText("");
        }
        else
            m_maximizeButton->setVisible(false);

        if (m_titleButtons & TitleButton::Minimize)
        {
            auto buttonRenderer = getSharedRenderer()->getMinimizeButton();
            if (!buttonRenderer || (buttonRenderer->propertyValuePairs.empty() && !buttonRenderer->connectedTheme))
                buttonRenderer = getSharedRenderer()->getCloseButton();

            m_minimizeButton->setVisible(true);
            m_minimizeButton->setRenderer(buttonRenderer);
            m_minimizeButton->setInheritedOpacity(m_opacityCached);

            if (m_showTextOnTitleButtonsCached)
                m_minimizeButton->setText(U"\u2043");
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

    void ChildWindow::close()
    {
        bool abort = false;
        onClosing.emit(this, &abort);
        if (abort)
            return;

        onClose.emit(this);
        destroy();
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

    void ChildWindow::setPositionLocked(bool positionLocked)
    {
        m_positionLocked = positionLocked;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ChildWindow::isPositionLocked() const
    {
        return m_positionLocked;
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

    bool ChildWindow::isMouseOnWidget(Vector2f pos) const
    {
        FloatRect region{getPosition(), getSize()};

        // Expand the region if the child window is resizable (to make the borders easier to click on)
        if (m_resizable)
        {
            region.left -= std::max(0.f, m_minimumResizableBorderWidthCached - m_bordersCached.getLeft());
            region.top -= std::max(0.f, m_minimumResizableBorderWidthCached - m_bordersCached.getTop());
            region.width += std::max(0.f, m_minimumResizableBorderWidthCached - m_bordersCached.getLeft())
                            + std::max(0.f, m_minimumResizableBorderWidthCached - m_bordersCached.getRight());
            region.height += std::max(0.f, m_minimumResizableBorderWidthCached - m_bordersCached.getTop())
                             + std::max(0.f, m_minimumResizableBorderWidthCached - m_bordersCached.getBottom());
        }

        if (region.contains(pos))
        {
            // If the mouse enters the border or title bar then then none of the widgets can still be under the mouse
            if (m_widgetBelowMouse && !FloatRect{getPosition() + getChildWidgetsOffset(), getClientSize()}.contains(pos))
                m_widgetBelowMouse->mouseNoLongerOnWidget();

            return true;
        }

        return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ChildWindow::leftMousePressed(Vector2f pos)
    {
        pos -= getPosition();

        m_mouseDown = true;

        // Move the child window to the front
        moveToFront();

        onMousePress.emit(this);

        bool isDragging = false;
        if (FloatRect{getChildWidgetsOffset(), getClientSize()}.contains(pos))
        {
            // Propagate the event to the child widgets
            isDragging = Container::leftMousePressed(pos + getPosition());
        }
        else if (!FloatRect{m_bordersCached.getLeft(), m_bordersCached.getTop(), getClientSize().x, getClientSize().y + m_titleBarHeightCached + m_borderBelowTitleBarCached}.contains(pos))
        {
            if (!m_focused)
                setFocused(true);

            // Mouse is on top of the borders
            if (m_resizable)
            {
                // Check on which border the mouse is standing
                m_resizeDirection = ResizeNone;
                if (pos.x < m_bordersCached.getLeft())
                    m_resizeDirection |= ResizeLeft;
                if (pos.y < m_bordersCached.getTop())
                    m_resizeDirection |= ResizeTop;
                if (pos.x >= getSize().x - m_bordersCached.getRight())
                    m_resizeDirection |= ResizeRight;
                if (pos.y >= getSize().y - m_bordersCached.getBottom())
                    m_resizeDirection |= ResizeBottom;

                isDragging = true;
            }

            m_draggingPosition = pos;
        }
        else if (FloatRect{m_bordersCached.getLeft(), m_bordersCached.getTop(), getClientSize().x, m_titleBarHeightCached}.contains(pos))
        {
            if (!m_focused)
                setFocused(true);

            // Send the mouse press event to the title buttons
            for (auto& button : {m_closeButton.get(), m_maximizeButton.get(), m_minimizeButton.get()})
            {
                if (button->isVisible() && button->isMouseOnWidget(pos))
                {
                    button->leftMousePressed(pos);
                    return false;
                }
            }

            // The mouse went down on the title bar
            if (!m_positionLocked)
            {
                m_mouseDownOnTitleBar = true;
                m_draggingPosition = pos;
                isDragging = true;
            }
        }

        return isDragging;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChildWindow::leftMouseReleased(Vector2f pos)
    {
        pos -= getPosition();

        m_mouseDownOnTitleBar = false;
        m_resizeDirection = ResizeNone;

        if (FloatRect{getChildWidgetsOffset(), getClientSize()}.contains(pos))
        {
            // Propagate the event to the child widgets
            Container::leftMouseReleased(pos + getPosition());
        }
        else
        {
            // Tell the widgets that the mouse was released
            for (auto& widget : m_widgets)
                widget->leftMouseButtonNoLongerDown();

            // Check if the mouse is on top of the title bar
            if (FloatRect{m_bordersCached.getLeft(), m_bordersCached.getTop(), getClientSize().x, m_titleBarHeightCached}.contains(pos))
            {
                // Send the mouse release event to the title buttons
                for (auto& button : {m_closeButton.get(), m_maximizeButton.get(), m_minimizeButton.get()})
                {
                    if (button->isVisible() && button->isMouseOnWidget(pos))
                    {
                        button->leftMouseReleased(pos);
                        break;
                    }
                }
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChildWindow::rightMousePressed(Vector2f pos)
    {
        if (FloatRect{getChildWidgetsOffset(), getClientSize()}.contains(pos - getPosition()))
            Container::rightMousePressed(pos);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChildWindow::rightMouseReleased(Vector2f pos)
    {
        pos -= getPosition();

        if (FloatRect{getChildWidgetsOffset(), getClientSize()}.contains(pos))
        {
            // Propagate the event to the child widgets
            Container::rightMouseReleased(pos + getPosition());
        }
        else
        {
            // Tell the widgets that the mouse was released
            for (auto& widget : m_widgets)
                widget->rightMouseButtonNoLongerDown();
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChildWindow::mouseMoved(Vector2f pos)
    {
        pos -= getPosition();

        // Check if you are dragging the child window
        if (m_mouseDown && m_mouseDownOnTitleBar)
        {
            const Vector2f origin{getOrigin().x * getSize().x, getOrigin().y * getSize().y};
            const Vector2f topLeftPos = getPosition() - origin;

            // Move the child window, but don't allow the dragging position to leave the screen
            Vector2f newPosition;
            if (topLeftPos.x + pos.x <= 0)
                newPosition.x = -m_draggingPosition.x + origin.x + 1;
            else if (m_parent && topLeftPos.x + pos.x >= m_parent->getSize().x)
                newPosition.x = m_parent->getSize().x - m_draggingPosition.x + origin.x - 1;
            else
                newPosition.x = getPosition().x + (pos.x - m_draggingPosition.x);

            if (topLeftPos.y + pos.y <= 0)
                newPosition.y = -m_draggingPosition.y + origin.y + 1;
            else if (m_parent && topLeftPos.y + pos.y >= m_parent->getSize().y)
                newPosition.y = m_parent->getSize().y - m_draggingPosition.y + origin.y - 1;
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
            if (m_widgetWithLeftMouseDown || FloatRect{getChildWidgetsOffset(), getClientSize()}.contains(pos))
            {
                if (m_resizable && !m_widgetWithLeftMouseDown)
                    updateResizeMouseCursor(pos);

                // Propagate the event to the child widgets
                Container::mouseMoved(pos + getPosition());
            }
            else
            {
                if (!m_mouseHover)
                    mouseEnteredWidget();

                // Check if the mouse is on top of the title bar
                if (FloatRect{m_bordersCached.getLeft(), m_bordersCached.getTop(), getClientSize().x, m_titleBarHeightCached}.contains(pos))
                {
                    // Send the hover event to the buttons inside the title bar
                    for (auto& button : {m_closeButton.get(), m_maximizeButton.get(), m_minimizeButton.get()})
                    {
                        if (button->isVisible())
                        {
                            if (button->isMouseOnWidget(pos))
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

                if (m_resizable)
                    updateResizeMouseCursor(pos);
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChildWindow::keyPressed(const Event::KeyEvent& event)
    {
        if (event.code == Event::KeyboardKey::Escape)
            onEscapeKeyPress.emit(this);

        Container::keyPressed(event);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ChildWindow::canHandleKeyPress(const Event::KeyEvent& event)
    {
        if (event.code == Event::KeyboardKey::Escape)
            return true;
        else
            return Container::canHandleKeyPress(event);
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

    void ChildWindow::leftMouseButtonNoLongerDown()
    {
        Container::leftMouseButtonNoLongerDown();

        for (const auto& button : {m_closeButton.get(), m_maximizeButton.get(), m_minimizeButton.get()})
        {
            if (button->isVisible())
                button->leftMouseButtonNoLongerDown();
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChildWindow::updateTitleBarHeight()
    {
        m_spriteTitleBar.setSize({getClientSize().x, m_titleBarHeightCached});

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

    void ChildWindow::updateResizeMouseCursor(Vector2f mousePos)
    {
        if (!m_parentGui)
            return;

        Cursor::Type requestedCursor;
        if (mousePos.x >= getChildWidgetsOffset().x + getClientSize().x)
        {
            if (mousePos.y >= getChildWidgetsOffset().y + getClientSize().y)
                requestedCursor = Cursor::Type::SizeBottomRight;
            else if (mousePos.y < m_bordersCached.getTop())
                requestedCursor = Cursor::Type::SizeTopRight;
            else
                requestedCursor = Cursor::Type::SizeRight;
        }
        else if (mousePos.x < m_bordersCached.getLeft())
        {
            if (mousePos.y >= getChildWidgetsOffset().y + getClientSize().y)
                requestedCursor = Cursor::Type::SizeBottomLeft;
            else if (mousePos.y < m_bordersCached.getTop())
                requestedCursor = Cursor::Type::SizeTopLeft;
            else
                requestedCursor = Cursor::Type::SizeLeft;
        }
        else if (mousePos.y < m_bordersCached.getTop())
        {
            requestedCursor = Cursor::Type::SizeTop;
        }
        else if (mousePos.y >= getChildWidgetsOffset().y + getClientSize().y)
        {
            requestedCursor = Cursor::Type::SizeBottom;
        }
        else
            requestedCursor = m_mouseCursor;

        if (m_currentChildWindowMouseCursor == requestedCursor)
            return;

        m_currentChildWindowMouseCursor = requestedCursor;
        m_parentGui->requestMouseCursor(requestedCursor);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Signal& ChildWindow::getSignal(String signalName)
    {
        if (signalName == onMousePress.getName())
            return onMousePress;
        else if (signalName == onClose.getName())
            return onClose;
        else if (signalName == onClosing.getName())
            return onClosing;
        else if (signalName == onMinimize.getName())
            return onMinimize;
        else if (signalName == onMaximize.getName())
            return onMaximize;
        else if (signalName == onEscapeKeyPress.getName())
            return onEscapeKeyPress;
        else
            return Container::getSignal(std::move(signalName));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChildWindow::rendererChanged(const String& property)
    {
        if (property == U"Borders")
        {
            m_bordersCached = getSharedRenderer()->getBorders();

            if (m_decorationLayoutX && (m_decorationLayoutX == m_size.x.getRightOperand()))
                m_decorationLayoutX->replaceValue(m_bordersCached.getLeft() + m_bordersCached.getRight());
            if (m_decorationLayoutY && (m_decorationLayoutY == m_size.y.getRightOperand()))
                m_decorationLayoutY->replaceValue(m_bordersCached.getTop() + m_bordersCached.getBottom() + m_titleBarHeightCached + m_borderBelowTitleBarCached);

            setSize(m_size);
        }
        else if (property == U"TitleColor")
        {
            m_titleText.setColor(getSharedRenderer()->getTitleColor());
        }
        else if ((property == U"TextureTitleBar") || (property == U"TitleBarHeight"))
        {
            const float oldTitleBarHeight = m_titleBarHeightCached;

            if (property == U"TextureTitleBar")
                m_spriteTitleBar.setTexture(getSharedRenderer()->getTextureTitleBar());

            m_titleBarHeightCached = getSharedRenderer()->getTitleBarHeight();
            updateTitleBarHeight();

            if (oldTitleBarHeight != m_titleBarHeightCached)
            {
                if (m_decorationLayoutY && (m_decorationLayoutY == m_size.y.getRightOperand()))
                    m_decorationLayoutY->replaceValue(m_bordersCached.getTop() + m_bordersCached.getBottom() + m_titleBarHeightCached + m_borderBelowTitleBarCached);

                // If the title bar changes in height then the inner size will also change
                recalculateBoundSizeLayouts();
            }
        }
        else if (property == U"TextureBackground")
        {
            m_spriteBackground.setTexture(getSharedRenderer()->getTextureBackground());
        }
        else if (property == U"BorderBelowTitleBar")
        {
            m_borderBelowTitleBarCached = getSharedRenderer()->getBorderBelowTitleBar();
            if (m_decorationLayoutY && (m_decorationLayoutY == m_size.y.getRightOperand()))
                m_decorationLayoutY->replaceValue(m_bordersCached.getTop() + m_bordersCached.getBottom() + m_titleBarHeightCached + m_borderBelowTitleBarCached);
        }
        else if (property == U"DistanceToSide")
        {
            m_distanceToSideCached = getSharedRenderer()->getDistanceToSide();
            setPosition(m_position);
        }
        else if (property == U"PaddingBetweenButtons")
        {
            m_paddingBetweenButtonsCached = getSharedRenderer()->getPaddingBetweenButtons();
            setPosition(m_position);
        }
        else if (property == U"MinimumResizableBorderWidth")
        {
            m_minimumResizableBorderWidthCached = getSharedRenderer()->getMinimumResizableBorderWidth();
        }
        else if (property == U"ShowTextOnTitleButtons")
        {
            m_showTextOnTitleButtonsCached = getSharedRenderer()->getShowTextOnTitleButtons();
            setTitleButtons(m_titleButtons);
        }
        else if (property == U"CloseButton")
        {
            if (m_closeButton->isVisible())
            {
                m_closeButton->setRenderer(getSharedRenderer()->getCloseButton());
                m_closeButton->setInheritedOpacity(m_opacityCached);
            }

            updateTitleBarHeight();
        }
        else if (property == U"MaximizeButton")
        {
            if (m_maximizeButton->isVisible())
            {
                auto buttonRenderer = getSharedRenderer()->getMaximizeButton();
                if (!buttonRenderer || (buttonRenderer->propertyValuePairs.empty() && !buttonRenderer->connectedTheme))
                    buttonRenderer = getSharedRenderer()->getCloseButton();

                m_maximizeButton->setRenderer(buttonRenderer);
                m_maximizeButton->setInheritedOpacity(m_opacityCached);
            }

            updateTitleBarHeight();
        }
        else if (property == U"MinimizeButton")
        {
            if (m_minimizeButton->isVisible())
            {
                auto buttonRenderer = getSharedRenderer()->getMinimizeButton();
                if (!buttonRenderer || (buttonRenderer->propertyValuePairs.empty() && !buttonRenderer->connectedTheme))
                    buttonRenderer = getSharedRenderer()->getCloseButton();

                m_minimizeButton->setRenderer(buttonRenderer);
                m_minimizeButton->setInheritedOpacity(m_opacityCached);
            }

            updateTitleBarHeight();
        }
        else if (property == U"BackgroundColor")
        {
            m_backgroundColorCached = getSharedRenderer()->getBackgroundColor();
        }
        else if (property == U"TitleBarColor")
        {
            m_titleBarColorCached = getSharedRenderer()->getTitleBarColor();
        }
        else if (property == U"BorderColor")
        {
            m_borderColorCached = getSharedRenderer()->getBorderColor();
        }
        else if (property == U"BorderColorFocused")
        {
            m_borderColorFocusedCached = getSharedRenderer()->getBorderColorFocused();
        }
        else if ((property == U"Opacity") || (property == U"OpacityDisabled"))
        {
            Container::rendererChanged(property);

            for (auto& button : {m_closeButton.get(), m_maximizeButton.get(), m_minimizeButton.get()})
            {
                if (button->isVisible())
                    button->setInheritedOpacity(m_opacityCached);
            }

            m_titleText.setOpacity(m_opacityCached);
            m_spriteTitleBar.setOpacity(m_opacityCached);
            m_spriteBackground.setOpacity(m_opacityCached);
        }
        else if (property == U"Font")
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

        if (m_titleAlignment == TitleAlignment::Left)
            node->propertyValuePairs[U"TitleAlignment"] = std::make_unique<DataIO::ValueNode>("Left");
        else if (m_titleAlignment == TitleAlignment::Center)
            node->propertyValuePairs[U"TitleAlignment"] = std::make_unique<DataIO::ValueNode>("Center");
        else if (m_titleAlignment == TitleAlignment::Right)
            node->propertyValuePairs[U"TitleAlignment"] = std::make_unique<DataIO::ValueNode>("Right");

        if (getTitle().length() > 0)
            node->propertyValuePairs[U"Title"] = std::make_unique<DataIO::ValueNode>(Serializer::serialize(getTitle()));

        if (m_titleTextSize != 0)
            node->propertyValuePairs[U"TitleTextSize"] = std::make_unique<DataIO::ValueNode>(String::fromNumber(m_titleTextSize));

        if (m_keepInParent)
            node->propertyValuePairs[U"KeepInParent"] = std::make_unique<DataIO::ValueNode>("true");

        if (m_resizable)
            node->propertyValuePairs[U"Resizable"] = std::make_unique<DataIO::ValueNode>("true");
        if (m_positionLocked)
            node->propertyValuePairs[U"PositionLocked"] = std::make_unique<DataIO::ValueNode>("true");

        if (m_minimumSize != Vector2f{})
            node->propertyValuePairs[U"MinimumSize"] = std::make_unique<DataIO::ValueNode>("(" + String::fromNumber(m_minimumSize.x) + ", " + String::fromNumber(m_minimumSize.y) + ")");
        if (m_maximumSize != Vector2f{std::numeric_limits<float>::infinity(), std::numeric_limits<float>::infinity()})
            node->propertyValuePairs[U"MaximumSize"] = std::make_unique<DataIO::ValueNode>("(" + String::fromNumber(m_maximumSize.x) + ", " + String::fromNumber(m_maximumSize.y) + ")");

        String serializedTitleButtons;
        if (m_titleButtons & TitleButton::Minimize)
            serializedTitleButtons += U" | Minimize";
        if (m_titleButtons & TitleButton::Maximize)
            serializedTitleButtons += U" | Maximize";
        if (m_titleButtons & TitleButton::Close)
            serializedTitleButtons += U" | Close";

        if (!serializedTitleButtons.empty())
            serializedTitleButtons.erase(0, 3);
        else
            serializedTitleButtons = U"None";

        node->propertyValuePairs[U"TitleButtons"] = std::make_unique<DataIO::ValueNode>(serializedTitleButtons);

        return node;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChildWindow::load(const std::unique_ptr<DataIO::Node>& node, const LoadingRenderersMap& renderers)
    {
        Container::load(node, renderers);

        if (node->propertyValuePairs[U"TitleAlignment"])
        {
            if (node->propertyValuePairs[U"TitleAlignment"]->value == U"Left")
                setTitleAlignment(TitleAlignment::Left);
            else if (node->propertyValuePairs[U"TitleAlignment"]->value == U"Center")
                setTitleAlignment(TitleAlignment::Center);
            else if (node->propertyValuePairs[U"TitleAlignment"]->value == U"Right")
                setTitleAlignment(TitleAlignment::Right);
            else
                throw Exception{U"Failed to parse TitleAlignment property. Only the values Left, Center and Right are correct."};
        }

        if (node->propertyValuePairs[U"TitleButtons"])
        {
            unsigned int decodedTitleButtons = TitleButton::None;
            std::vector<String> titleButtons = Deserializer::split(node->propertyValuePairs[U"TitleButtons"]->value, '|');
            for (const auto& elem : titleButtons)
            {
                String requestedTitleButton = elem.trim();
                if (requestedTitleButton == U"Close")
                    decodedTitleButtons |= TitleButton::Close;
                else if (requestedTitleButton == U"Maximize")
                    decodedTitleButtons |= TitleButton::Maximize;
                else if (requestedTitleButton == U"Minimize")
                    decodedTitleButtons |= TitleButton::Minimize;
            }

            setTitleButtons(decodedTitleButtons);
        }

        if (node->propertyValuePairs[U"Title"])
            setTitle(Deserializer::deserialize(ObjectConverter::Type::String, node->propertyValuePairs[U"Title"]->value).getString());

        if (node->propertyValuePairs[U"TitleTextSize"])
            setTitleTextSize(static_cast<unsigned int>(Deserializer::deserialize(ObjectConverter::Type::Number, node->propertyValuePairs[U"TitleTextSize"]->value).getNumber()));

        if (node->propertyValuePairs[U"KeepInParent"])
            setKeepInParent(Deserializer::deserialize(ObjectConverter::Type::Bool, node->propertyValuePairs[U"KeepInParent"]->value).getBool());

        if (node->propertyValuePairs[U"Resizable"])
            setResizable(Deserializer::deserialize(ObjectConverter::Type::Bool, node->propertyValuePairs[U"Resizable"]->value).getBool());
        if (node->propertyValuePairs[U"PositionLocked"])
            setPositionLocked(Deserializer::deserialize(ObjectConverter::Type::Bool, node->propertyValuePairs[U"PositionLocked"]->value).getBool());

        if (node->propertyValuePairs[U"MinimumSize"])
            setMinimumSize(Vector2f{node->propertyValuePairs[U"MinimumSize"]->value});

        if (node->propertyValuePairs[U"MaximumSize"])
            setMaximumSize(Vector2f{node->propertyValuePairs[U"MaximumSize"]->value});
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChildWindow::mouseEnteredWidget()
    {
#if TGUI_HAS_WINDOW_BACKEND_SFML && (SFML_VERSION_MAJOR == 2) && (SFML_VERSION_MINOR < 6)
        if (m_resizable && (m_mouseCursor != Cursor::Type::Arrow))
        {
            // Container::mouseEnteredWidget() can't be called from here because of a bug in SFML < 2.6.
            // Calling the function from the base class would set the mouse cursor that was requested. If the mouse is on top
            // of the borders then we need to replace it with a resize cursor afterwards. These cursor changes would occus out
            // of order though, causing the wrong cursor to show up when the mouse enters a border from the outside.
            m_mouseHover = true;
            onMouseEnter.emit(this);
            m_currentChildWindowMouseCursor = Cursor::Type::Arrow;
            return;
        }
#endif

        Container::mouseEnteredWidget();

        // If the child window has a custom mouse cursor then the Container::mouseEnteredWidget() would have switched to it.
        // We should recheck whether the mouse is on top of the borders to change it back into a resize arrow if needed.
        // The check would be called after mouseEnteredWidget() anyway, so we just make sure that the code realizes that
        // the mouse cursor has been changed by resetting the state.
        m_currentChildWindowMouseCursor = m_mouseCursor;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChildWindow::mouseLeftWidget()
    {
        if (m_currentChildWindowMouseCursor != Cursor::Type::Arrow)
        {
            m_currentChildWindowMouseCursor = Cursor::Type::Arrow;
            if (m_parentGui)
                m_parentGui->requestMouseCursor(Cursor::Type::Arrow);
        }

        Widget::mouseLeftWidget();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChildWindow::draw(BackendRenderTarget& target, RenderStates states) const
    {
        // Draw the borders
        if (m_bordersCached != Borders{0})
        {
            if (m_focused && m_borderColorFocusedCached.isSet())
                target.drawBorders(states, m_bordersCached, getSize(), Color::applyOpacity(m_borderColorFocusedCached, m_opacityCached));
            else
                target.drawBorders(states, m_bordersCached, getSize(), Color::applyOpacity(m_borderColorCached, m_opacityCached));

            states.transform.translate({m_bordersCached.getLeft(), m_bordersCached.getTop()});
        }

        // Draw the title bar
        if (m_spriteTitleBar.isSet())
            target.drawSprite(states, m_spriteTitleBar);
        else
            target.drawFilledRect(states, {getClientSize().x, m_titleBarHeightCached}, Color::applyOpacity(m_titleBarColorCached, m_opacityCached));

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
            const float clippingRight = getClientSize().x - m_distanceToSideCached - buttonOffsetX;
            target.addClippingLayer(states, {{clippingLeft, 0}, {clippingRight - clippingLeft, m_titleBarHeightCached}});
            target.drawText(states, m_titleText);
            target.removeClippingLayer();
        }

        // Draw the buttons
        states.transform.translate({-m_bordersCached.getLeft(), -m_bordersCached.getTop()});
        for (auto& button : {m_closeButton.get(), m_maximizeButton.get(), m_minimizeButton.get()})
        {
            if (!button->isVisible())
                continue;

            RenderStates buttonStates = states;
            buttonStates.transform.translate(button->getPosition());
            button->draw(target, buttonStates);
        }
        states.transform.translate({m_bordersCached.getLeft(), m_bordersCached.getTop() + m_titleBarHeightCached});

        // Draw the border below the title bar
        if (m_borderBelowTitleBarCached > 0)
        {
            if (m_focused && m_borderColorFocusedCached.isSet())
                target.drawFilledRect(states, {getClientSize().x, m_borderBelowTitleBarCached}, Color::applyOpacity(m_borderColorFocusedCached, m_opacityCached));
            else
                target.drawFilledRect(states, {getClientSize().x, m_borderBelowTitleBarCached}, Color::applyOpacity(m_borderColorCached, m_opacityCached));

            states.transform.translate({0, m_borderBelowTitleBarCached});
        }

        // Draw the background
        if (m_spriteBackground.isSet())
            target.drawSprite(states, m_spriteBackground);
        else if (m_backgroundColorCached != Color::Transparent)
            target.drawFilledRect(states, getClientSize(), Color::applyOpacity(m_backgroundColorCached, m_opacityCached));

        // Draw the widgets in the child window
        target.addClippingLayer(states, {{}, {getClientSize()}});
        Container::draw(target, states);
        target.removeClippingLayer();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Widget::Ptr ChildWindow::clone() const
    {
        return std::make_shared<ChildWindow>(*this);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChildWindow::connectTitleButtonCallbacks()
    {
        // Disconnecting is required when calling this function from the move constructor or move assignment operator
        m_maximizeButton->onPress.disconnectAll();
        m_minimizeButton->onPress.disconnectAll();
        m_closeButton->onPress.disconnectAll();

        m_maximizeButton->onPress([this]{ onMaximize.emit(this); });
        m_minimizeButton->onPress([this]{ onMinimize.emit(this); });
        m_closeButton->onPress([this]{ close(); });
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
