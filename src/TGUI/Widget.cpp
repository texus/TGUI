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


#include <TGUI/ToolTip.hpp>
#include <TGUI/Container.hpp>
#include <TGUI/Animation.hpp>
#include <SFML/System/Err.hpp>
#include <SFML/Graphics/RectangleShape.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace
{
    void addAnimation(std::vector<std::shared_ptr<tgui::priv::Animation>>& existingAnimations, std::shared_ptr<tgui::priv::Animation> newAnimation)
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
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Widget::Widget()
    {
        m_renderer->subscribe(this, [this](const std::string& property){ rendererChangedCallback(property); });

        // The opacity is 1 by default and thus has to be explicitly initialized
        m_opacityCached = getRenderer()->getOpacity();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Widget::~Widget()
    {
        // The renderer will be null when the widget was moved
        if (m_renderer)
            m_renderer->unsubscribe(this);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Widget::Widget(const Widget& other) :
        Transformable                  {other},
        SignalWidgetBase               {other},
        enable_shared_from_this<Widget>{other},
        m_type                         (other.m_type), // Did not compile in VS2013 when using braces
        m_enabled                      {other.m_enabled},
        m_visible                      {other.m_visible},
        m_parent                       {nullptr},
        m_allowFocus                   {other.m_allowFocus},
        m_draggableWidget              {other.m_draggableWidget},
        m_containerWidget              {other.m_containerWidget},
        m_toolTip                      {other.m_toolTip ? other.m_toolTip->clone() : nullptr},
        m_renderer                     {other.m_renderer},
        m_showAnimations               (other.m_showAnimations), // Did not compile in VS2013 when using braces
        m_fontCached                   {other.m_fontCached},
        m_opacityCached                {other.m_opacityCached}
    {
        m_renderer->subscribe(this, [this](const std::string& property){ rendererChangedCallback(property); });
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Widget::Widget(Widget&& other) :
        Transformable                  {std::move(other)},
        SignalWidgetBase               {std::move(other)},
        enable_shared_from_this<Widget>{std::move(other)},
        onPositionChange               {std::move(other.onPositionChange)},
        onSizeChange                   {std::move(other.onSizeChange)},
        onFocus                        {std::move(other.onFocus)},
        onUnfocus                      {std::move(other.onUnfocus)},
        onMouseEnter                   {std::move(other.onMouseEnter)},
        onMouseLeave                   {std::move(other.onMouseLeave)},
        m_type                         (std::move(other.m_type)), // Did not compile in VS2013 when using braces
        m_enabled                      {std::move(other.m_enabled)},
        m_visible                      {std::move(other.m_visible)},
        m_parent                       {nullptr},
        m_mouseHover                   {std::move(other.m_mouseHover)},
        m_mouseDown                    {std::move(other.m_mouseDown)},
        m_focused                      {std::move(other.m_focused)},
        m_allowFocus                   {std::move(other.m_allowFocus)},
        m_animationTimeElapsed         {std::move(other.m_animationTimeElapsed)},
        m_draggableWidget              {std::move(other.m_draggableWidget)},
        m_containerWidget              {std::move(other.m_containerWidget)},
        m_toolTip                      {std::move(other.m_toolTip)},
        m_renderer                     {other.m_renderer},
        m_showAnimations               (std::move(other.m_showAnimations)), // Did not compile in VS2013 when using braces
        m_fontCached                   {std::move(other.m_fontCached)},
        m_opacityCached                {std::move(other.m_opacityCached)}
    {
        other.m_renderer->unsubscribe(&other);
        m_renderer->subscribe(this, [this](const std::string& property){ rendererChangedCallback(property); });

        other.m_renderer = nullptr;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Widget& Widget::operator=(const Widget& other)
    {
        if (this != &other)
        {
            m_renderer->unsubscribe(this);

            Transformable::operator=(other);
            SignalWidgetBase::operator=(other);
            enable_shared_from_this::operator=(other);

            onPositionChange->disconnectAll();
            onSizeChange->disconnectAll();
            onFocus->disconnectAll();
            onUnfocus->disconnectAll();
            onMouseEnter->disconnectAll();
            onMouseLeave->disconnectAll();

            m_type                 = other.m_type;
            m_enabled              = other.m_enabled;
            m_visible              = other.m_visible;
            m_parent               = nullptr;
            m_mouseHover           = false;
            m_mouseDown            = false;
            m_focused              = false;
            m_allowFocus           = other.m_allowFocus;
            m_animationTimeElapsed = {};
            m_draggableWidget      = other.m_draggableWidget;
            m_containerWidget      = other.m_containerWidget;
            m_toolTip              = other.m_toolTip ? other.m_toolTip->clone() : nullptr;
            m_renderer             = other.m_renderer;
            m_showAnimations       = {};
            m_fontCached           = other.m_fontCached;
            m_opacityCached        = other.m_opacityCached;

            m_renderer->subscribe(this, [this](const std::string& property){ rendererChangedCallback(property); });
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

            Transformable::operator=(std::move(other));
            SignalWidgetBase::operator=(std::move(other));
            enable_shared_from_this::operator=(std::move(other));

            onPositionChange       = std::move(other.onPositionChange);
            onSizeChange           = std::move(other.onSizeChange);
            onFocus                = std::move(other.onFocus);
            onUnfocus              = std::move(other.onUnfocus);
            onMouseEnter           = std::move(other.onMouseEnter);
            onMouseLeave           = std::move(other.onMouseLeave);
            m_type                 = std::move(other.m_type);
            m_enabled              = std::move(other.m_enabled);
            m_visible              = std::move(other.m_visible);
            m_parent               = nullptr;
            m_mouseHover           = std::move(other.m_mouseHover);
            m_mouseDown            = std::move(other.m_mouseDown);
            m_focused              = std::move(other.m_focused);
            m_animationTimeElapsed = std::move(other.m_animationTimeElapsed);
            m_allowFocus           = std::move(other.m_allowFocus);
            m_draggableWidget      = std::move(other.m_draggableWidget);
            m_containerWidget      = std::move(other.m_containerWidget);
            m_toolTip              = std::move(other.m_toolTip);
            m_renderer             = std::move(other.m_renderer);
            m_showAnimations       = std::move(other.m_showAnimations);
            m_fontCached           = std::move(other.m_fontCached);
            m_opacityCached        = std::move(other.m_opacityCached);

            m_renderer->subscribe(this, [this](const std::string& property){ rendererChangedCallback(property); });

            other.m_renderer = nullptr;
        }

        return *this;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Widget::setRenderer(const std::shared_ptr<RendererData>& rendererData)
    {
        std::shared_ptr<RendererData> oldData = m_renderer->getData();

        // If no font is given then try to use the one from the parent
        if (m_parent && m_parent->getRenderer()->getFont() && (rendererData->propertyValuePairs.find("font") == rendererData->propertyValuePairs.end()))
            rendererData->propertyValuePairs["font"] = ObjectConverter(m_parent->getRenderer()->getFont());

        // Update the data
        m_renderer->unsubscribe(this);
        m_renderer->setData(rendererData);
        m_renderer->subscribe(this, [this](const std::string& property){ rendererChangedCallback(property); });

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

    WidgetRenderer* Widget::getRenderer() const
    {
        return m_renderer.get();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Widget::setPosition(const Layout2d& position)
    {
        Transformable::setPosition(position);

        if (m_parent)
        {
            const sf::Vector2f oldPosition = m_position.getValue();
            m_position.updateParentSize(m_parent->getContentSize());
            if (oldPosition != m_position.getValue())
            {
                updatePosition();
                return;
            }
        }

        onPositionChange->emit(this, getPosition());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Widget::setSize(const Layout2d& size)
    {
        Transformable::setSize(size);

        if (m_parent)
        {
            const sf::Vector2f oldSize = m_size.getValue();
            m_size.updateParentSize(m_parent->getContentSize());
            if (oldSize != m_size.getValue())
            {
                updateSize();
                return;
            }
        }

        onSizeChange->emit(this, getSize());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    sf::Vector2f Widget::getAbsolutePosition() const
    {
        if (m_parent)
            return m_parent->getAbsolutePosition() + m_parent->getChildWidgetsOffset() + getPosition();
        else
            return getPosition();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    sf::Vector2f Widget::getWidgetOffset() const
    {
        return sf::Vector2f{0, 0};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Widget::show()
    {
        m_visible = true;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Widget::showWithEffect(ShowAnimationType type, sf::Time duration)
    {
        show();

        switch (type)
        {
            case ShowAnimationType::Fade:
            {
                addAnimation(m_showAnimations, std::make_shared<priv::FadeAnimation>(shared_from_this(), 0.f, m_opacityCached, duration));
                getRenderer()->setOpacity(0);
                break;
            }
            case ShowAnimationType::Scale:
            {
                addAnimation(m_showAnimations, std::make_shared<priv::MoveAnimation>(shared_from_this(), getPosition() + (getSize() / 2.f), getPosition(), duration));
                addAnimation(m_showAnimations, std::make_shared<priv::ResizeAnimation>(shared_from_this(), sf::Vector2f{0, 0}, getSize(), duration));
                setPosition(getPosition() + (getSize() / 2.f));
                setSize(0, 0);
                break;
            }
            case ShowAnimationType::SlideFromLeft:
            {
                addAnimation(m_showAnimations, std::make_shared<priv::MoveAnimation>(shared_from_this(), sf::Vector2f{-getFullSize().x, getPosition().y}, getPosition(), duration));
                setPosition({-getFullSize().x, getPosition().y});
                break;
            }
            case ShowAnimationType::SlideFromRight:
            {
                if (getParent())
                {
                    addAnimation(m_showAnimations, std::make_shared<priv::MoveAnimation>(shared_from_this(), sf::Vector2f{getParent()->getSize().x + getWidgetOffset().x, getPosition().y}, getPosition(), duration));
                    setPosition({getParent()->getSize().x + getWidgetOffset().x, getPosition().y});
                }
                else
                    sf::err() << "TGUI Warning: showWithEffect(SlideFromRight) does not work before widget has a parent." << std::endl;

                break;
            }
            case ShowAnimationType::SlideFromTop:
            {
                addAnimation(m_showAnimations, std::make_shared<priv::MoveAnimation>(shared_from_this(), sf::Vector2f{getPosition().x, -getFullSize().y}, getPosition(), duration));
                setPosition({getPosition().x, -getFullSize().y});
                break;
            }
            case ShowAnimationType::SlideFromBottom:
            {
                if (getParent())
                {
                    addAnimation(m_showAnimations, std::make_shared<priv::MoveAnimation>(shared_from_this(), sf::Vector2f{getPosition().x, getParent()->getSize().y + getWidgetOffset().y}, getPosition(), duration));
                    setPosition({getPosition().x, getParent()->getSize().y + getWidgetOffset().y});
                }
                else
                    sf::err() << "TGUI Warning: showWithEffect(SlideFromBottom) does not work before widget has a parent." << std::endl;

                break;
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Widget::hide()
    {
        m_visible = false;

        // If the widget is focused then it must be unfocused
        unfocus();
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
                float opacity = m_opacityCached;
                addAnimation(m_showAnimations, std::make_shared<priv::FadeAnimation>(shared_from_this(), m_opacityCached, 0.f, duration, [=](){ hide(); getRenderer()->setOpacity(opacity); }));
                break;
            }
            case ShowAnimationType::Scale:
            {
                addAnimation(m_showAnimations, std::make_shared<priv::MoveAnimation>(shared_from_this(), position, position + (size / 2.f), duration, [=](){ hide(); setPosition(position); setSize(size); }));
                addAnimation(m_showAnimations, std::make_shared<priv::ResizeAnimation>(shared_from_this(), size, sf::Vector2f{0, 0}, duration, [=](){ hide(); setPosition(position); setSize(size); }));
                break;
            }
            case ShowAnimationType::SlideToRight:
            {
                if (getParent())
                    addAnimation(m_showAnimations, std::make_shared<priv::MoveAnimation>(shared_from_this(), position, sf::Vector2f{getParent()->getSize().x + getWidgetOffset().x, position.y}, duration, [=](){ hide(); setPosition(position); }));
                else
                    sf::err() << "TGUI Warning: hideWithEffect(SlideToRight) does not work before widget has a parent." << std::endl;

                break;
            }
            case ShowAnimationType::SlideToLeft:
            {
                addAnimation(m_showAnimations, std::make_shared<priv::MoveAnimation>(shared_from_this(), position, sf::Vector2f{-getFullSize().x, position.y}, duration, [=](){ hide(); setPosition(position); }));
                break;
            }
            case ShowAnimationType::SlideToBottom:
            {
                if (getParent())
                    addAnimation(m_showAnimations, std::make_shared<priv::MoveAnimation>(shared_from_this(), position, sf::Vector2f{position.x, getParent()->getSize().y + getWidgetOffset().y}, duration, [=](){ hide(); setPosition(position); }));
                else
                    sf::err() << "TGUI Warning: hideWithEffect(SlideToBottom) does not work before widget has a parent." << std::endl;

                break;
            }
            case ShowAnimationType::SlideToTop:
            {
                addAnimation(m_showAnimations, std::make_shared<priv::MoveAnimation>(shared_from_this(), position, sf::Vector2f{position.x, -getFullSize().y}, duration, [=](){ hide(); setPosition(position); }));
                break;
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Widget::enable()
    {
        m_enabled = true;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Widget::disable()
    {
        m_enabled = false;

        // Change the mouse button state.
        m_mouseHover = false;
        m_mouseDown = false;

        // If the widget is focused then it must be unfocused
        unfocus();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Widget::focus()
    {
        if (m_parent)
            m_parent->focusWidget(shared_from_this());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Widget::unfocus()
    {
        if (m_focused && m_parent)
            m_parent->unfocusWidgets();
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

    void Widget::setToolTip(Widget::Ptr toolTip)
    {
        m_toolTip = toolTip;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Widget::Ptr Widget::getToolTip()
    {
        return m_toolTip;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Widget::setParent(Container* parent)
    {
        m_parent = parent;
        if (m_parent)
            updateParentSize(m_parent->getContentSize());
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

    void Widget::leftMousePressed(sf::Vector2f)
    {
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Widget::leftMouseReleased(sf::Vector2f)
    {
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Widget::mouseMoved(sf::Vector2f)
    {
        if (!m_mouseHover)
            mouseEnteredWidget();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Widget::keyPressed(const sf::Event::KeyEvent&)
    {
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Widget::textEntered(sf::Uint32)
    {
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Widget::mouseWheelScrolled(float, sf::Vector2f)
    {
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Widget::widgetFocused()
    {
        onFocus->emit(this);

        // Make sure the parent is also focused
        if (m_parent)
            m_parent->focus();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Widget::widgetUnfocused()
    {
        onUnfocus->emit(this);
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

    Widget::Ptr Widget::askToolTip(sf::Vector2f mousePos)
    {
        if (m_toolTip && mouseOnWidget(mousePos))
            return getToolTip();
        else
            return nullptr;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Widget::updateParentSize(sf::Vector2f size)
    {
        const sf::Vector2f oldPosition = m_position.getValue();
        const sf::Vector2f oldSize = m_size.getValue();

        m_position.updateParentSize(size);
        m_size.updateParentSize(size);

        if (oldPosition != m_position.getValue())
            updatePosition();
        if (oldSize != m_size.getValue())
            updateSize();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Signal& Widget::getSignal(std::string&& signalName)
    {
        if (signalName == toLower(onPositionChange->getName()))
            return *onPositionChange;
        else if (signalName == toLower(onSizeChange->getName()))
            return *onSizeChange;
        else if (signalName == toLower(onFocus->getName()))
            return *onFocus;
        else if (signalName == toLower(onUnfocus->getName()))
            return *onUnfocus;
        else if (signalName == toLower(onMouseEnter->getName()))
            return *onMouseEnter;
        else if (signalName == toLower(onMouseLeave->getName()))
            return *onMouseLeave;

        throw Exception{"No signal exists with name '" + std::move(signalName) + "'."};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Widget::rendererChanged(const std::string& property)
    {
        if (property == "opacity")
            m_opacityCached = getRenderer()->getOpacity();
        else if (property == "font")
            m_fontCached = getRenderer()->getFont();
        else
            throw Exception{"Could not set property '" + property + "', widget of type '" + getWidgetType() + "' does not has this property."};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Widget::mouseEnteredWidget()
    {
        m_mouseHover = true;
        onMouseEnter->emit(this);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Widget::mouseLeftWidget()
    {
        m_mouseHover = false;
        onMouseLeave->emit(this);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Widget::rendererChangedCallback(const std::string& property)
    {
        rendererChanged(property);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Widget::drawRectangleShape(sf::RenderTarget& target,
                                    const sf::RenderStates& states,
                                    sf::Vector2f size,
                                    sf::Color color) const
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
                             sf::Vector2f size,
                             sf::Color borderColor) const
    {
        const sf::Color color = Color::calcColorOpacity(borderColor, m_opacityCached);

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
