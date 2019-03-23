/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus' Graphical User Interface
// Copyright (C) 2012-2019 Bruno Van de Velde (vdv_b@tgui.eu)
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


#include <TGUI/ToolTip.hpp>
#include <TGUI/Container.hpp>
#include <TGUI/Animation.hpp>
#include <TGUI/Vector2f.hpp>
#include <TGUI/Loading/WidgetFactory.hpp>
#include <SFML/System/Err.hpp>
#include <SFML/Graphics/RectangleShape.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    namespace
    {
        void addAnimation(std::vector<std::shared_ptr<priv::Animation>>& existingAnimations, std::shared_ptr<priv::Animation> newAnimation)
        {
            const auto type = newAnimation->getType();

            // If another animation is already running with the same type then instantly finish it
            unsigned int i = 0;
            while (i < existingAnimations.size())
            {
                if (existingAnimations[i]->getType() == type)
                {
                    existingAnimations[i]->finish();
                    existingAnimations.erase(existingAnimations.begin() + i);
                }
                else
                    ++i;
            }

            existingAnimations.push_back(newAnimation);
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        static Layout2d parseLayout(std::string str)
        {
            if (str.empty())
                throw Exception{"Failed to parse layout '" + str + "'. String was empty."};

            // Remove the brackets around the value
            if (((str.front() == '(') && (str.back() == ')')) || ((str.front() == '{') && (str.back() == '}')))
                str = str.substr(1, str.length() - 2);

            if (str.empty())
                return {0, 0};

            // Find the comma that splits the x and y layouts, taking into account that these layouts may contain commas too
            unsigned int bracketCount = 0;
            auto commaOrBracketPos = str.find_first_of(",()");
            decltype(commaOrBracketPos) commaPos = 0;
            while (commaOrBracketPos != std::string::npos)
            {
                if (str[commaOrBracketPos] == '(')
                    bracketCount++;
                else if (str[commaOrBracketPos] == ')')
                {
                    if (bracketCount == 0)
                        throw Exception{"Failed to parse layout '" + str + "'. Brackets didn't match."};

                    bracketCount--;
                }
                else // if (str[commaOrBracketPos] == ',')
                {
                    if (bracketCount == 0)
                        commaPos = commaOrBracketPos;
                }

                commaOrBracketPos = str.find_first_of(",()", commaOrBracketPos + 1);
            }

            // Remove quotes around the values
            std::string x = trim(str.substr(0, commaPos));
            if ((x.size() >= 2) && ((x[0] == '"') && (x[x.length()-1] == '"')))
                x = x.substr(1, x.length()-2);

            std::string y = trim(str.substr(commaPos + 1));
            if ((y.size() >= 2) && ((y[0] == '"') && (y[y.length()-1] == '"')))
                y = y.substr(1, y.length()-2);

            return {x, y};
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Widget::Widget()
    {
        m_renderer->subscribe(this, m_rendererChangedCallback);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Widget::~Widget()
    {
        // The renderer will be null when the widget was moved
        if (m_renderer)
            m_renderer->unsubscribe(this);

        for (auto& layout : m_boundPositionLayouts)
            layout->unbindWidget();

        for (auto& layout : m_boundSizeLayouts)
            layout->unbindWidget();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Widget::Widget(const Widget& other) :
        SignalWidgetBase               {other},
        enable_shared_from_this<Widget>{other},
        m_type                         {other.m_type},
        m_position                     {other.m_position},
        m_size                         {other.m_size},
        m_boundPositionLayouts         {},
        m_boundSizeLayouts             {},
        m_enabled                      {other.m_enabled},
        m_visible                      {other.m_visible},
        m_parent                       {nullptr},
        m_draggableWidget              {other.m_draggableWidget},
        m_containerWidget              {other.m_containerWidget},
        m_toolTip                      {other.m_toolTip ? other.m_toolTip->clone() : nullptr},
        m_renderer                     {other.m_renderer},
        m_showAnimations               {other.m_showAnimations},
        m_fontCached                   {other.m_fontCached},
        m_opacityCached                {other.m_opacityCached}
    {
        m_position.x.connectWidget(this, true, [this]{ setPosition(getPositionLayout()); });
        m_position.y.connectWidget(this, false, [this]{ setPosition(getPositionLayout()); });
        m_size.x.connectWidget(this, true, [this]{ setSize(getSizeLayout()); });
        m_size.y.connectWidget(this, false, [this]{ setSize(getSizeLayout()); });

        m_renderer->subscribe(this, m_rendererChangedCallback);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Widget::Widget(Widget&& other) :
        SignalWidgetBase               {std::move(other)},
        enable_shared_from_this<Widget>{std::move(other)},
        onPositionChange               {std::move(other.onPositionChange)},
        onSizeChange                   {std::move(other.onSizeChange)},
        onFocus                        {std::move(other.onFocus)},
        onUnfocus                      {std::move(other.onUnfocus)},
        onMouseEnter                   {std::move(other.onMouseEnter)},
        onMouseLeave                   {std::move(other.onMouseLeave)},
        m_type                         {std::move(other.m_type)},
        m_position                     {std::move(other.m_position)},
        m_size                         {std::move(other.m_size)},
        m_boundPositionLayouts         {std::move(other.m_boundPositionLayouts)},
        m_boundSizeLayouts             {std::move(other.m_boundSizeLayouts)},
        m_enabled                      {std::move(other.m_enabled)},
        m_visible                      {std::move(other.m_visible)},
        m_parent                       {nullptr},
        m_mouseHover                   {std::move(other.m_mouseHover)},
        m_mouseDown                    {std::move(other.m_mouseDown)},
        m_focused                      {std::move(other.m_focused)},
        m_animationTimeElapsed         {std::move(other.m_animationTimeElapsed)},
        m_draggableWidget              {std::move(other.m_draggableWidget)},
        m_containerWidget              {std::move(other.m_containerWidget)},
        m_toolTip                      {std::move(other.m_toolTip)},
        m_renderer                     {other.m_renderer},
        m_showAnimations               {std::move(other.m_showAnimations)},
        m_fontCached                   {std::move(other.m_fontCached)},
        m_opacityCached                {std::move(other.m_opacityCached)}
    {
        m_position.x.connectWidget(this, true, [this]{ setPosition(getPositionLayout()); });
        m_position.y.connectWidget(this, false, [this]{ setPosition(getPositionLayout()); });
        m_size.x.connectWidget(this, true, [this]{ setSize(getSizeLayout()); });
        m_size.y.connectWidget(this, false, [this]{ setSize(getSizeLayout()); });

        other.m_renderer->unsubscribe(&other);
        m_renderer->subscribe(this, m_rendererChangedCallback);

        other.m_renderer = nullptr;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Widget& Widget::operator=(const Widget& other)
    {
        if (this != &other)
        {
            m_renderer->unsubscribe(this);

            SignalWidgetBase::operator=(other);
            enable_shared_from_this::operator=(other);

            onPositionChange.disconnectAll();
            onSizeChange.disconnectAll();
            onFocus.disconnectAll();
            onUnfocus.disconnectAll();
            onMouseEnter.disconnectAll();
            onMouseLeave.disconnectAll();

            m_type                 = other.m_type;
            m_position             = other.m_position;
            m_size                 = other.m_size;
            m_boundPositionLayouts = {};
            m_boundSizeLayouts     = {};
            m_enabled              = other.m_enabled;
            m_visible              = other.m_visible;
            m_parent               = nullptr;
            m_mouseHover           = false;
            m_mouseDown            = false;
            m_focused              = false;
            m_animationTimeElapsed = {};
            m_draggableWidget      = other.m_draggableWidget;
            m_containerWidget      = other.m_containerWidget;
            m_toolTip              = other.m_toolTip ? other.m_toolTip->clone() : nullptr;
            m_renderer             = other.m_renderer;
            m_showAnimations       = {};
            m_fontCached           = other.m_fontCached;
            m_opacityCached        = other.m_opacityCached;

            m_position.x.connectWidget(this, true, [this]{ setPosition(getPositionLayout()); });
            m_position.y.connectWidget(this, false, [this]{ setPosition(getPositionLayout()); });
            m_size.x.connectWidget(this, true, [this]{ setSize(getSizeLayout()); });
            m_size.y.connectWidget(this, false, [this]{ setSize(getSizeLayout()); });

            m_renderer->subscribe(this, m_rendererChangedCallback);
        }

        return *this;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Widget& Widget::operator=(Widget&& other)
    {
        if (this != &other)
        {
            m_renderer->unsubscribe(this);
            other.m_renderer->unsubscribe(&other);

            SignalWidgetBase::operator=(std::move(other));
            enable_shared_from_this::operator=(std::move(other));

            onPositionChange       = std::move(other.onPositionChange);
            onSizeChange           = std::move(other.onSizeChange);
            onFocus                = std::move(other.onFocus);
            onUnfocus              = std::move(other.onUnfocus);
            onMouseEnter           = std::move(other.onMouseEnter);
            onMouseLeave           = std::move(other.onMouseLeave);
            m_type                 = std::move(other.m_type);
            m_position             = std::move(other.m_position);
            m_size                 = std::move(other.m_size);
            m_boundPositionLayouts = std::move(other.m_boundPositionLayouts);
            m_boundSizeLayouts     = std::move(other.m_boundSizeLayouts);
            m_enabled              = std::move(other.m_enabled);
            m_visible              = std::move(other.m_visible);
            m_parent               = nullptr;
            m_mouseHover           = std::move(other.m_mouseHover);
            m_mouseDown            = std::move(other.m_mouseDown);
            m_focused              = std::move(other.m_focused);
            m_animationTimeElapsed = std::move(other.m_animationTimeElapsed);
            m_draggableWidget      = std::move(other.m_draggableWidget);
            m_containerWidget      = std::move(other.m_containerWidget);
            m_toolTip              = std::move(other.m_toolTip);
            m_renderer             = std::move(other.m_renderer);
            m_showAnimations       = std::move(other.m_showAnimations);
            m_fontCached           = std::move(other.m_fontCached);
            m_opacityCached        = std::move(other.m_opacityCached);

            m_position.x.connectWidget(this, true, [this]{ setPosition(getPositionLayout()); });
            m_position.y.connectWidget(this, false, [this]{ setPosition(getPositionLayout()); });
            m_size.x.connectWidget(this, true, [this]{ setSize(getSizeLayout()); });
            m_size.y.connectWidget(this, false, [this]{ setSize(getSizeLayout()); });

            m_renderer->subscribe(this, m_rendererChangedCallback);

            other.m_renderer = nullptr;
        }

        return *this;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Widget::setRenderer(std::shared_ptr<RendererData> rendererData)
    {
        if (rendererData == nullptr)
            rendererData = RendererData::create();

        std::shared_ptr<RendererData> oldData = m_renderer->getData();

        // Update the data
        m_renderer->unsubscribe(this);
        m_renderer->setData(rendererData);
        m_renderer->subscribe(this, m_rendererChangedCallback);
        rendererData->shared = true;

        // Tell the widget about all the updated properties, both new ones and old ones that were now reset to their default value
        auto oldIt = oldData->propertyValuePairs.begin();
        auto newIt = rendererData->propertyValuePairs.begin();
        while (oldIt != oldData->propertyValuePairs.end() && newIt != rendererData->propertyValuePairs.end())
        {
            if (oldIt->first < newIt->first)
            {
                // Update values that no longer exist in the new renderer and are now reset to the default value
                rendererChanged(oldIt->first);
                ++oldIt;
            }
            else
            {
                // Update changed and new properties
                rendererChanged(newIt->first);

                if (newIt->first < oldIt->first)
                    ++newIt;
                else
                {
                    ++oldIt;
                    ++newIt;
                }
            }
        }
        while (oldIt != oldData->propertyValuePairs.end())
        {
            rendererChanged(oldIt->first);
            ++oldIt;
        }
        while (newIt != rendererData->propertyValuePairs.end())
        {
            rendererChanged(newIt->first);
            ++newIt;
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const WidgetRenderer* Widget::getSharedRenderer() const
    {
        // You should not be allowed to call setters on the renderer when the widget is const
        return m_renderer.get();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    WidgetRenderer* Widget::getSharedRenderer()
    {
        return m_renderer.get();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const WidgetRenderer* Widget::getRenderer() const
    {
        if (m_renderer->getData()->shared)
        {
            m_renderer->unsubscribe(this);
            m_renderer->setData(m_renderer->clone());
            m_renderer->subscribe(this, m_rendererChangedCallback);
            m_renderer->getData()->shared = false;
        }

        // You should not be allowed to call setters on the renderer when the widget is const
        return m_renderer.get();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    WidgetRenderer* Widget::getRenderer()
    {
        if (m_renderer->getData()->shared)
        {
            m_renderer->unsubscribe(this);
            m_renderer->setData(m_renderer->clone());
            m_renderer->subscribe(this, m_rendererChangedCallback);
            m_renderer->getData()->shared = false;
        }

        return m_renderer.get();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Widget::setPosition(const Layout2d& position)
    {
        m_position = position;
        m_position.x.connectWidget(this, true, [this]{ setPosition(getPositionLayout()); });
        m_position.y.connectWidget(this, false, [this]{ setPosition(getPositionLayout()); });

        if (getPosition() != m_prevPosition)
        {
            m_prevPosition = getPosition();
            onPositionChange.emit(this, getPosition());

            for (auto& layout : m_boundPositionLayouts)
                layout->recalculateValue();
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Widget::setSize(const Layout2d& size)
    {
        m_size = size;
        m_size.x.connectWidget(this, true, [this]{ setSize(getSizeLayout()); });
        m_size.y.connectWidget(this, false, [this]{ setSize(getSizeLayout()); });

        if (getSize() != m_prevSize)
        {
            m_prevSize = getSize();
            onSizeChange.emit(this, getSize());

            for (auto& layout : m_boundSizeLayouts)
                layout->recalculateValue();
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Vector2f Widget::getFullSize() const
    {
        return getSize();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Vector2f Widget::getAbsolutePosition() const
    {
        if (m_parent)
            return m_parent->getAbsolutePosition() + m_parent->getChildWidgetsOffset() + getPosition();
        else
            return getPosition();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Vector2f Widget::getWidgetOffset() const
    {
        return Vector2f{0, 0};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Widget::showWithEffect(ShowAnimationType type, sf::Time duration)
    {
        setVisible(true);

        switch (type)
        {
            case ShowAnimationType::Fade:
            {
                addAnimation(m_showAnimations, std::make_shared<priv::FadeAnimation>(shared_from_this(), 0.f, getInheritedOpacity(), duration, [=]{onAnimationFinished.emit(this, type, true); }));
                setInheritedOpacity(0);
                break;
            }
            case ShowAnimationType::Scale:
            {
                addAnimation(m_showAnimations, std::make_shared<priv::MoveAnimation>(shared_from_this(), getPosition() + (getSize() / 2.f), getPosition(), duration));
                addAnimation(m_showAnimations, std::make_shared<priv::ResizeAnimation>(shared_from_this(), Vector2f{0, 0}, getSize(), duration, [=]{onAnimationFinished.emit(this, type, true); }));
                setPosition(getPosition() + (getSize() / 2.f));
                setSize(0, 0);
                break;
            }
            case ShowAnimationType::SlideFromLeft:
            {
                addAnimation(m_showAnimations, std::make_shared<priv::MoveAnimation>(shared_from_this(), Vector2f{-getFullSize().x, getPosition().y}, getPosition(), duration, [=]{onAnimationFinished.emit(this, type, true); }));
                setPosition({-getFullSize().x, getPosition().y});
                break;
            }
            case ShowAnimationType::SlideFromRight:
            {
                if (getParent())
                {
                    addAnimation(m_showAnimations, std::make_shared<priv::MoveAnimation>(shared_from_this(), Vector2f{getParent()->getSize().x + getWidgetOffset().x, getPosition().y}, getPosition(), duration, [=]{onAnimationFinished.emit(this, type, true); }));
                    setPosition({getParent()->getSize().x + getWidgetOffset().x, getPosition().y});
                }
                else
                {
                    TGUI_PRINT_WARNING("showWithEffect(SlideFromRight) does not work before widget has a parent.");
                }

                break;
            }
            case ShowAnimationType::SlideFromTop:
            {
                addAnimation(m_showAnimations, std::make_shared<priv::MoveAnimation>(shared_from_this(), Vector2f{getPosition().x, -getFullSize().y}, getPosition(), duration, [=]{onAnimationFinished.emit(this, type, true); }));
                setPosition({getPosition().x, -getFullSize().y});
                break;
            }
            case ShowAnimationType::SlideFromBottom:
            {
                if (getParent())
                {
                    addAnimation(m_showAnimations, std::make_shared<priv::MoveAnimation>(shared_from_this(), Vector2f{getPosition().x, getParent()->getSize().y + getWidgetOffset().y}, getPosition(), duration, [=]{onAnimationFinished.emit(this, type, true); }));
                    setPosition({getPosition().x, getParent()->getSize().y + getWidgetOffset().y});
                }
                else
                {
                    TGUI_PRINT_WARNING("showWithEffect(SlideFromBottom) does not work before widget has a parent.");
                }

                break;
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Widget::hideWithEffect(ShowAnimationType type, sf::Time duration)
    {
        const auto position = getPosition();
        const auto size = getSize();

        switch (type)
        {
            case ShowAnimationType::Fade:
            {
                float opacity = getInheritedOpacity();
                addAnimation(m_showAnimations, std::make_shared<priv::FadeAnimation>(shared_from_this(), getInheritedOpacity(), 0.f, duration,
                    [=](){ setVisible(false); setInheritedOpacity(opacity); onAnimationFinished.emit(this, type, false); }));
                break;
            }
            case ShowAnimationType::Scale:
            {
                addAnimation(m_showAnimations, std::make_shared<priv::MoveAnimation>(shared_from_this(), position, position + (size / 2.f), duration, [=](){ setVisible(false); setPosition(position); setSize(size); }));
                addAnimation(m_showAnimations, std::make_shared<priv::ResizeAnimation>(shared_from_this(), size, Vector2f{0, 0}, duration,
                    [=](){ setVisible(false); setPosition(position); setSize(size); onAnimationFinished.emit(this, type, false); }));
                break;
            }
            case ShowAnimationType::SlideToRight:
            {
                if (getParent())
                {
                    addAnimation(m_showAnimations, std::make_shared<priv::MoveAnimation>(shared_from_this(), position, Vector2f{getParent()->getSize().x + getWidgetOffset().x, position.y}, duration,
                        [=](){ setVisible(false); setPosition(position); onAnimationFinished.emit(this, type, false); }));
                }
                else
                {
                    TGUI_PRINT_WARNING("hideWithEffect(SlideToRight) does not work before widget has a parent.");
                }

                break;
            }
            case ShowAnimationType::SlideToLeft:
            {
                addAnimation(m_showAnimations, std::make_shared<priv::MoveAnimation>(shared_from_this(), position, Vector2f{-getFullSize().x, position.y}, duration,
                    [=](){ setVisible(false); setPosition(position); onAnimationFinished.emit(this, type, false); }));
                break;
            }
            case ShowAnimationType::SlideToBottom:
            {
                if (getParent())
                {
                    addAnimation(m_showAnimations, std::make_shared<priv::MoveAnimation>(shared_from_this(), position, Vector2f{position.x, getParent()->getSize().y + getWidgetOffset().y}, duration,
                        [=](){ setVisible(false); setPosition(position); onAnimationFinished.emit(this, type, false); }));
                }
                else
                {
                    TGUI_PRINT_WARNING("hideWithEffect(SlideToBottom) does not work before widget has a parent.");
                }

                break;
            }
            case ShowAnimationType::SlideToTop:
            {
                addAnimation(m_showAnimations, std::make_shared<priv::MoveAnimation>(shared_from_this(), position, Vector2f{position.x, -getFullSize().y}, duration,
                    [=](){ setVisible(false); setPosition(position); onAnimationFinished.emit(this, type, false); }));
                break;
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Widget::setVisible(bool visible)
    {
        m_visible = visible;

        // If the widget is hiden while still focused then it must be unfocused
        if (!visible)
            setFocused(false);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Widget::setEnabled(bool enabled)
    {
        m_enabled = enabled;

        if (!enabled)
        {
            m_mouseHover = false;
            m_mouseDown = false;
            setFocused(false);
        }

        // Refresh widget opacity if there is a different value set for enabled and disabled widgets
        if (getSharedRenderer()->getOpacityDisabled() != -1)
            rendererChanged("opacitydisabled");
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Widget::setFocused(bool focused)
    {
        if (m_focused == focused)
            return;

        if (focused)
        {
            if (canGainFocus())
            {
                m_focused = true;

                if (m_parent)
                    m_parent->childWidgetFocused(shared_from_this());

                onFocus.emit(this);
            }
        }
        else // Unfocusing widget
        {
            m_focused = false;
            onUnfocus.emit(this);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const std::string& Widget::getWidgetType() const
    {
        return m_type;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Widget::moveToFront()
    {
        if (m_parent)
            m_parent->moveWidgetToFront(shared_from_this());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Widget::moveToBack()
    {
        if (m_parent)
            m_parent->moveWidgetToBack(shared_from_this());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Widget::setInheritedFont(const Font& font)
    {
        m_inheritedFont = font;
        rendererChanged("font");
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const Font& Widget::getInheritedFont() const
    {
        return m_inheritedFont;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Widget::setInheritedOpacity(float opacity)
    {
        m_inheritedOpacity = opacity;
        rendererChanged("opacity");
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    float Widget::getInheritedOpacity() const
    {
        return m_inheritedOpacity;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Widget::setToolTip(Widget::Ptr toolTip)
    {
        m_toolTip = toolTip;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Widget::Ptr Widget::getToolTip() const
    {
        return m_toolTip;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Widget::canGainFocus() const
    {
        return m_enabled && m_visible;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Widget::isContainer() const
    {
        return m_containerWidget;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Widget::setParent(Container* parent)
    {
        m_parent = parent;

        // Give the layouts another chance to find widgets to which it refers
        m_position.x.connectWidget(this, true, [this]{ setPosition(getPositionLayout()); });
        m_position.y.connectWidget(this, false, [this]{ setPosition(getPositionLayout()); });
        m_size.x.connectWidget(this, true, [this]{ setSize(getSizeLayout()); });
        m_size.y.connectWidget(this, false, [this]{ setSize(getSizeLayout()); });
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Widget::update(sf::Time elapsedTime)
    {
        m_animationTimeElapsed += elapsedTime;

        for (unsigned int i = 0; i < m_showAnimations.size();)
        {
            if (m_showAnimations[i]->update(elapsedTime))
                m_showAnimations.erase(m_showAnimations.begin() + i);
            else
                i++;
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Widget::leftMousePressed(Vector2f)
    {
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Widget::leftMouseReleased(Vector2f)
    {
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Widget::rightMousePressed(Vector2f)
    {
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Widget::rightMouseReleased(Vector2f)
    {
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Widget::mouseMoved(Vector2f)
    {
        if (!m_mouseHover)
            mouseEnteredWidget();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Widget::keyPressed(const sf::Event::KeyEvent&)
    {
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Widget::textEntered(std::uint32_t)
    {
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Widget::mouseWheelScrolled(float, Vector2f)
    {
        return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Widget::mouseNoLongerOnWidget()
    {
        if (m_mouseHover)
            mouseLeftWidget();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Widget::mouseNoLongerDown()
    {
        m_mouseDown = false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Widget::Ptr Widget::askToolTip(Vector2f mousePos)
    {
        if (m_toolTip && mouseOnWidget(mousePos))
            return getToolTip();
        else
            return nullptr;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Widget::bindPositionLayout(Layout* layout)
    {
        m_boundPositionLayouts.insert(layout);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Widget::unbindPositionLayout(Layout* layout)
    {
        m_boundPositionLayouts.erase(layout);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Widget::bindSizeLayout(Layout* layout)
    {
        m_boundSizeLayouts.insert(layout);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Widget::unbindSizeLayout(Layout* layout)
    {
        m_boundSizeLayouts.erase(layout);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Signal& Widget::getSignal(std::string signalName)
    {
        if (signalName == toLower(onPositionChange.getName()))
            return onPositionChange;
        else if (signalName == toLower(onSizeChange.getName()))
            return onSizeChange;
        else if (signalName == toLower(onFocus.getName()))
            return onFocus;
        else if (signalName == toLower(onUnfocus.getName()))
            return onUnfocus;
        else if (signalName == toLower(onMouseEnter.getName()))
            return onMouseEnter;
        else if (signalName == toLower(onMouseLeave.getName()))
            return onMouseLeave;
        else if (signalName == toLower(onAnimationFinished.getName()))
            return onAnimationFinished;

        throw Exception{"No signal exists with name '" + std::move(signalName) + "'."};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Widget::rendererChanged(const std::string& property)
    {
        if ((property == "opacity") || (property == "opacitydisabled"))
        {
            if (!m_enabled && (getSharedRenderer()->getOpacityDisabled() != -1))
                m_opacityCached = getSharedRenderer()->getOpacityDisabled() * m_inheritedOpacity;
            else
                m_opacityCached = getSharedRenderer()->getOpacity() * m_inheritedOpacity;
        }
        else if (property == "font")
        {
            if (getSharedRenderer()->getFont())
                m_fontCached = getSharedRenderer()->getFont();
            else if (m_inheritedFont)
                m_fontCached = m_inheritedFont;
            else
                m_fontCached = getGlobalFont();
        }
        else if (property == "transparenttexture")
        {
            m_transparentTextureCached = getSharedRenderer()->getTransparentTexture();
        }
        else
            throw Exception{"Could not set property '" + property + "', widget of type '" + getWidgetType() + "' does not has this property."};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::unique_ptr<DataIO::Node> Widget::save(SavingRenderersMap& renderers) const
    {
        sf::String widgetName;
        if (getParent())
            widgetName = getParent()->getWidgetName(shared_from_this());

        auto node = std::make_unique<DataIO::Node>();
        if (widgetName.isEmpty())
            node->name = getWidgetType();
        else
            node->name = getWidgetType() + "." + Serializer::serialize(widgetName);

        if (!isVisible())
            node->propertyValuePairs["Visible"] = std::make_unique<DataIO::ValueNode>("false");
        if (!isEnabled())
            node->propertyValuePairs["Enabled"] = std::make_unique<DataIO::ValueNode>("false");
        if (getPosition() != Vector2f{})
            node->propertyValuePairs["Position"] = std::make_unique<DataIO::ValueNode>(m_position.toString());
        if (getSize() != Vector2f{})
            node->propertyValuePairs["Size"] = std::make_unique<DataIO::ValueNode>(m_size.toString());

        if (getToolTip() != nullptr)
        {
            auto toolTipWidgetNode = getToolTip()->save(renderers);

            auto toolTipNode = std::make_unique<DataIO::Node>();
            toolTipNode->name = "ToolTip";
            toolTipNode->children.emplace_back(std::move(toolTipWidgetNode));

            toolTipNode->propertyValuePairs["TimeToDisplay"] = std::make_unique<DataIO::ValueNode>(to_string(ToolTip::getTimeToDisplay().asSeconds()));
            toolTipNode->propertyValuePairs["DistanceToMouse"] = std::make_unique<DataIO::ValueNode>("(" + to_string(ToolTip::getDistanceToMouse().x) + "," + to_string(ToolTip::getDistanceToMouse().y) + ")");

            node->children.emplace_back(std::move(toolTipNode));
        }

        if (renderers.at(this).first)
            node->children.emplace_back(std::move(renderers.at(this).first));
        else
            node->propertyValuePairs["renderer"] = std::make_unique<DataIO::ValueNode>("&" + renderers.at(this).second);

        return node;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Widget::load(const std::unique_ptr<DataIO::Node>& node, const LoadingRenderersMap& renderers)
    {
        if (node->propertyValuePairs["visible"])
            setVisible(Deserializer::deserialize(ObjectConverter::Type::Bool, node->propertyValuePairs["visible"]->value).getBool());
        if (node->propertyValuePairs["enabled"])
            setEnabled(Deserializer::deserialize(ObjectConverter::Type::Bool, node->propertyValuePairs["enabled"]->value).getBool());
        if (node->propertyValuePairs["position"])
            setPosition(parseLayout(node->propertyValuePairs["position"]->value));
        if (node->propertyValuePairs["size"])
            setSize(parseLayout(node->propertyValuePairs["size"]->value));

        if (node->propertyValuePairs["renderer"])
        {
            const sf::String value = node->propertyValuePairs["renderer"]->value;
            if (value.isEmpty() || (value[0] != '&'))
                throw Exception{"Expected reference to renderer, did not find '&' character"};

            const auto it = renderers.find(toLower(value.substring(1)));
            if (it == renderers.end())
                throw Exception{"Widget refers to renderer with name '" + value.substring(1) + "', but no such renderer was found"};

            setRenderer(it->second);
        }

        for (const auto& childNode : node->children)
        {
            if (toLower(childNode->name) == "tooltip")
            {
                for (const auto& pair : childNode->propertyValuePairs)
                {
                    if (pair.first == "timetodisplay")
                        ToolTip::setTimeToDisplay(sf::seconds(tgui::stof(pair.second->value)));
                    else if (pair.first == "distancetomouse")
                        ToolTip::setDistanceToMouse(Vector2f{pair.second->value});
                }

                if (!childNode->children.empty())
                {
                    // There can only be one child in the tool tip section
                    if (childNode->children.size() > 1)
                        throw Exception{"ToolTip section contained multiple children."};

                    const auto& toolTipWidgetNode = childNode->children[0];
                    const auto& constructor = WidgetFactory::getConstructFunction(toolTipWidgetNode->name);
                    if (constructor)
                    {
                        Widget::Ptr toolTip = constructor();
                        toolTip->load(toolTipWidgetNode, renderers);
                        setToolTip(toolTip);
                    }
                    else
                        throw Exception{"No construct function exists for widget type '" + toolTipWidgetNode->name + "'."};
                }
            }
            else if (toLower(childNode->name) == "renderer")
                setRenderer(RendererData::createFromDataIONode(childNode.get()));

            /// TODO: Signals?
        }
        node->children.erase(std::remove_if(node->children.begin(), node->children.end(), [](const std::unique_ptr<DataIO::Node>& child){
                return (toLower(child->name) == "tooltip") || (toLower(child->name) == "renderer");
            }), node->children.end());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Widget::mouseEnteredWidget()
    {
        m_mouseHover = true;
        onMouseEnter.emit(this);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Widget::mouseLeftWidget()
    {
        m_mouseHover = false;
        onMouseLeave.emit(this);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Widget::rendererChangedCallback(const std::string& property)
    {
        rendererChanged(property);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Widget::drawRectangleShape(sf::RenderTarget& target,
                                    const sf::RenderStates& states,
                                    Vector2f size,
                                    Color color) const
    {
        sf::RectangleShape shape{size};

        if (m_opacityCached < 1)
            shape.setFillColor(Color::calcColorOpacity(color, m_opacityCached));
        else
            shape.setFillColor(color);

        target.draw(shape, states);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Widget::drawBorders(sf::RenderTarget& target,
                             const sf::RenderStates& states,
                             const Borders& borders,
                             Vector2f size,
                             Color borderColor) const
    {
        const Color color = Color::calcColorOpacity(borderColor, m_opacityCached);

        // If size is too small then draw entire size as border
        if ((size.x <= borders.getLeft() + borders.getRight()) || (size.y <= borders.getTop() + borders.getBottom()))
        {
            sf::RectangleShape border;
            border.setFillColor(color);
            border.setSize({size.x, size.y});
            target.draw(border, states);
        }
        else // Draw borders in the normal way
        {
            //////////////////////
            // 0--1/8---------6 //
            // |              | //
            // |   9------7   | //
            // |   |      |   | //
            // |   |      |   | //
            // |   3------5   | //
            // |              | //
            // 2--------------4 //
            //////////////////////
            const std::vector<sf::Vertex> vertices = {
                {{0, 0}, color},
                {{borders.getLeft(), 0}, color},
                {{0, size.y}, color},
                {{borders.getLeft(), size.y - borders.getBottom()}, color},
                {{size.x, size.y}, color},
                {{size.x - borders.getRight(), size.y - borders.getBottom()}, color},
                {{size.x, 0}, color},
                {{size.x - borders.getRight(), borders.getTop()}, color},
                {{borders.getLeft(), 0}, color},
                {{borders.getLeft(), borders.getTop()}, color}
            };

            target.draw(vertices.data(), vertices.size(), sf::PrimitiveType::TrianglesStrip, states);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
