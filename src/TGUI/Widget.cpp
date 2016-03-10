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


#include <TGUI/Loading/Theme.hpp>
#include <TGUI/Widgets/ToolTip.hpp>
#include <TGUI/Container.hpp>
#include <TGUI/Animation.hpp>

#include <cassert>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace
{
    void addAnimation(std::vector<std::shared_ptr<tgui::priv::Animation>>& existingAnimations, std::shared_ptr<tgui::priv::Animation> newAnimation)
    {
        auto type = newAnimation->getType();

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
        m_callback.widget = this;

        addSignal<sf::Vector2f>("PositionChanged");
        addSignal<sf::Vector2f>("SizeChanged");
        addSignal("Focused");
        addSignal("Unfocused");
        addSignal("MouseEntered");
        addSignal("MouseLeft");

        m_renderer->subscribe(this, std::bind(&Widget::rendererChangedCallback, this, std::placeholders::_1, std::placeholders::_2));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Widget::~Widget()
    {
        if (m_renderer)
            m_renderer->unsubscribe(this);

        if (m_position.x.getImpl()->parentWidget == this)
            m_position.x.getImpl()->parentWidget = nullptr;
        if (m_position.y.getImpl()->parentWidget == this)
            m_position.y.getImpl()->parentWidget = nullptr;
        if (m_size.x.getImpl()->parentWidget == this)
            m_size.x.getImpl()->parentWidget = nullptr;
        if (m_size.y.getImpl()->parentWidget == this)
            m_size.y.getImpl()->parentWidget = nullptr;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Widget::Widget(const Widget& other) :
        sf::Drawable     {other},
        Transformable    {other},
        SignalWidgetBase {other},
        enable_shared_from_this<Widget>{other},
        m_enabled        {other.m_enabled},
        m_visible        {other.m_visible},
        m_parent         {other.m_parent},
        m_allowFocus     {other.m_allowFocus},
        m_draggableWidget{other.m_draggableWidget},
        m_containerWidget{other.m_containerWidget},
        m_toolTip        {other.m_toolTip ? other.m_toolTip->clone() : nullptr},
        m_font           {other.m_font},
        m_renderer       {other.m_renderer}
    {
        m_callback.widget = this;

        if (m_renderer)
            m_renderer->subscribe(this, std::bind(&Widget::rendererChangedCallback, this, std::placeholders::_1, std::placeholders::_2));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Widget::Widget(Widget&& other) :
        sf::Drawable                   {std::move(other)},
        Transformable                  {std::move(other)},
        SignalWidgetBase               {std::move(other)},
        enable_shared_from_this<Widget>{std::move(other)},
        m_enabled                      {std::move(other.m_enabled)},
        m_visible                      {std::move(other.m_visible)},
        m_parent                       {std::move(other.m_parent)},
        m_mouseHover                   {std::move(other.m_mouseHover)},
        m_mouseDown                    {std::move(other.m_mouseDown)},
        m_focused                      {std::move(other.m_focused)},
        m_allowFocus                   {std::move(other.m_allowFocus)},
        m_animationTimeElapsed         {std::move(other.m_animationTimeElapsed)},
        m_draggableWidget              {std::move(other.m_draggableWidget)},
        m_containerWidget              {std::move(other.m_containerWidget)},
        m_toolTip                      {std::move(other.m_toolTip)},
        m_font                         {std::move(other.m_font)},
        m_renderer                     {other.m_renderer},
        m_showAnimations               {std::move(other.m_showAnimations)}
    {
        m_callback.widget = this;

        if (m_renderer)
        {
            other.m_renderer->unsubscribe(&other);
            m_renderer->subscribe(this, std::bind(&Widget::rendererChangedCallback, this, std::placeholders::_1, std::placeholders::_2));
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Widget& Widget::operator=(const Widget& other)
    {
        if (this != &other)
        {
            if (m_renderer)
                m_renderer->unsubscribe(this);

            sf::Drawable::operator=(other);
            Transformable::operator=(other);
            SignalWidgetBase::operator=(other);
            enable_shared_from_this::operator=(other);

            m_callback.widget           = this;
            m_enabled                   = other.m_enabled;
            m_visible                   = other.m_visible;
            m_parent                    = other.m_parent;
            m_mouseHover                = false;
            m_mouseDown                 = false;
            m_focused                   = false;
            m_allowFocus                = other.m_allowFocus;
            m_animationTimeElapsed      = {};
            m_draggableWidget           = other.m_draggableWidget;
            m_containerWidget           = other.m_containerWidget;
            m_toolTip                   = other.m_toolTip ? other.m_toolTip->clone() : nullptr;
            m_font                      = other.m_font;
            m_renderer                  = other.m_renderer;
            m_showAnimations            = {};

            if (m_renderer)
                m_renderer->subscribe(this, std::bind(&Widget::rendererChangedCallback, this, std::placeholders::_1, std::placeholders::_2));
        }

        return *this;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Widget& Widget::operator=(Widget&& other)
    {
        if (this != &other)
        {
            if (m_renderer)
                m_renderer->unsubscribe(this);
            if (other.m_renderer)
                other.m_renderer->unsubscribe(&other);

            sf::Drawable::operator=(std::move(other));
            Transformable::operator=(std::move(other));
            SignalWidgetBase::operator=(std::move(other));
            enable_shared_from_this::operator=(std::move(other));

            m_callback.widget           = this;
            m_enabled                   = std::move(other.m_enabled);
            m_visible                   = std::move(other.m_visible);
            m_parent                    = std::move(other.m_parent);
            m_mouseHover                = std::move(other.m_mouseHover);
            m_mouseDown                 = std::move(other.m_mouseDown);
            m_focused                   = std::move(other.m_focused);
            m_animationTimeElapsed      = std::move(other.m_animationTimeElapsed);
            m_allowFocus                = std::move(other.m_allowFocus);
            m_draggableWidget           = std::move(other.m_draggableWidget);
            m_containerWidget           = std::move(other.m_containerWidget);
            m_toolTip                   = std::move(other.m_toolTip);
            m_font                      = std::move(other.m_font);
            m_renderer                  = std::move(other.m_renderer);
            m_showAnimations            = std::move(other.m_showAnimations);

            if (m_renderer)
                m_renderer->subscribe(this, std::bind(&Widget::rendererChangedCallback, this, std::placeholders::_1, std::placeholders::_2));
        }

        return *this;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Widget::setRenderer(std::shared_ptr<RendererData> rendererData)
    {
        if (m_renderer)
            m_renderer->unsubscribe(this);

        m_renderer->m_data = rendererData;

        if (m_renderer)
            m_renderer->subscribe(this, std::bind(&Widget::rendererChangedCallback, this, std::placeholders::_1, std::placeholders::_2));

        for (auto& pair : rendererData->propertyValuePairs)
            rendererChanged(pair.first, ObjectConverter{pair.second});
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    WidgetRenderer* Widget::getRenderer() const
    {
        return m_renderer.get();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Widget::setPosition(const Layout2d& position)
    {
        if (position.x.getImpl()->parentWidget != this)
        {
            position.x.getImpl()->parentWidget = this;
            position.x.getImpl()->recalculate();
        }
        if (position.y.getImpl()->parentWidget != this)
        {
            position.y.getImpl()->parentWidget = this;
            position.y.getImpl()->recalculate();
        }

        Transformable::setPosition(position);

        m_callback.position = getPosition();
        sendSignal("PositionChanged", getPosition());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Widget::setSize(const Layout2d& size)
    {
        if (size.x.getImpl()->parentWidget != this)
        {
            size.x.getImpl()->parentWidget = this;
            size.x.getImpl()->recalculate();
        }
        if (size.y.getImpl()->parentWidget != this)
        {
            size.y.getImpl()->parentWidget = this;
            size.y.getImpl()->recalculate();
        }

        Transformable::setSize(size);

        m_callback.size = getSize();
        sendSignal("SizeChanged", getSize());
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
                addAnimation(m_showAnimations, std::make_shared<priv::FadeAnimation>(shared_from_this(), 0.f, getRenderer()->getOpacity(), duration));
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
        auto position = getPosition();
        auto size = getSize();

        switch (type)
        {
            case ShowAnimationType::Fade:
            {
                float opacity = getRenderer()->getOpacity();
                addAnimation(m_showAnimations, std::make_shared<priv::FadeAnimation>(shared_from_this(), getRenderer()->getOpacity(), 0.f, duration, [=](){ hide(); getRenderer()->setOpacity(opacity); }));
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
        return m_callback.widgetType;
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

    void Widget::setFont(const Font& font)
    {
        m_font = font.getFont();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::shared_ptr<sf::Font> Widget::getFont() const
    {
        return m_font;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    sf::Vector2f Widget::getWidgetOffset() const
    {
        return sf::Vector2f{0, 0};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Widget::setParent(Container* parent)
    {
        m_parent = parent;
        if (m_parent)
        {
            m_position.x.getImpl()->recalculate();
            m_position.y.getImpl()->recalculate();
            m_size.x.getImpl()->recalculate();
            m_size.y.getImpl()->recalculate();
        }
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

    void Widget::leftMousePressed(float, float)
    {
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Widget::leftMouseReleased(float, float)
    {
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Widget::mouseMoved(float, float)
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

    void Widget::mouseWheelMoved(int, int, int)
    {
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Widget::widgetFocused()
    {
        sendSignal("Focused");

        // Make sure the parent is also focused
        if (m_parent)
            m_parent->focus();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Widget::widgetUnfocused()
    {
        sendSignal("Unfocused");
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
        if (m_toolTip && mouseOnWidget(mousePos.x, mousePos.y))
            return getToolTip();
        else
            return nullptr;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Widget::rendererChanged(const std::string& property, ObjectConverter&&)
    {
        if (property != "opacity")
            throw Exception{"Could not set property '" + property + "', widget of type '" + getWidgetType() + "' does not has this property."};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Widget::mouseEnteredWidget()
    {
        m_mouseHover = true;
        sendSignal("MouseEntered");
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Widget::mouseLeftWidget()
    {
        m_mouseHover = false;
        sendSignal("MouseLeft");
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Widget::rendererChangedCallback(const std::string& property, ObjectConverter&& value)
    {
        rendererChanged(property, std::move(value));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Widget::drawBorders(sf::RenderTarget& target, const sf::RenderStates& states, const Borders& borders,
                             const sf::Vector2f& position, const sf::Vector2f& size, const sf::Color& color) const
    {
        sf::RectangleShape border;
        border.setFillColor(color);

        // If size is too small then draw entire size as border
        if ((size.x <= borders.left + borders.right) || (size.y <= borders.top + borders.bottom))
        {
            border.setSize({size.x, size.y});
            border.setPosition(position.x, position.y);
            target.draw(border, states);
        }
        else // Draw borders in the normal way
        {
            // Draw left border
            border.setSize({borders.left, size.y - borders.bottom});
            border.setPosition(position.x, position.y);
            target.draw(border, states);

            // Draw top border
            border.setSize({size.x - borders.left, borders.top});
            border.setPosition(position.x + borders.left, position.y);
            target.draw(border, states);

            // Draw right border
            border.setSize({borders.right, size.y - borders.top});
            border.setPosition(position.x + size.x - borders.right, position.y + borders.top);
            target.draw(border, states);

            // Draw bottom border
            border.setSize({size.x - borders.right, borders.bottom});
            border.setPosition(position.x, position.y + size.y - borders.bottom);
            target.draw(border, states);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void WidgetRenderer::setOpacity(float opacity)
    {
        if (opacity < 0)
            opacity = 0;
        else if (opacity > 1)
            opacity = 1;

        setProperty("opacity", ObjectConverter{opacity});
    }

    TGUI_RENDERER_PROPERTY_GET_NUMBER(WidgetRenderer, Opacity, 1)

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void WidgetRenderer::setProperty(const std::string& property, const std::string& value)
    {
        setProperty(property, ObjectConverter{sf::String{value}});
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void WidgetRenderer::setProperty(const std::string& property, ObjectConverter&& value)
    {
        std::string lowercaseProperty = toLower(property);

        m_data->propertyValuePairs[lowercaseProperty] = value;

        for (auto& observer : m_data->observers)
            observer.second(lowercaseProperty, ObjectConverter{value});
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ObjectConverter WidgetRenderer::getProperty(const std::string& property) const
    {
        auto it = m_data->propertyValuePairs.find(toLower(property));
        if (it != m_data->propertyValuePairs.end())
            return it->second;
        else
            return {};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const std::map<std::string, ObjectConverter>& WidgetRenderer::getPropertyValuePairs() const
    {
        return m_data->propertyValuePairs;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void WidgetRenderer::subscribe(void* id, const std::function<void(const std::string& property, ObjectConverter&& value)>& function)
    {
        m_data->observers[id] = function;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void WidgetRenderer::unsubscribe(void* id)
    {
        m_data->observers.erase(id);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::shared_ptr<RendererData> WidgetRenderer::getData() const
    {
        return m_data;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::shared_ptr<RendererData> WidgetRenderer::clone() const
    {
        return std::make_shared<RendererData>(*m_data);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
