/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus' Graphical User Interface
// Copyright (C) 2012-2022 Bruno Van de Velde (vdv_b@tgui.eu)
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
#include <TGUI/Vector2.hpp>
#include <TGUI/GuiBase.hpp>
#include <TGUI/Loading/WidgetFactory.hpp>
#include <TGUI/SignalManager.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    namespace
    {
        void finishExistingConflictingAnimations(std::vector<std::shared_ptr<priv::Animation>>& animations, ShowEffectType type)
        {
            // Only one animation of each type can be played at the same type. If e.g. a fade animation was already in progress
            // when starting a new one, the old animation is finished immediately.
            // Different types of animations (e.g. fading and moving) can occur at the same time.
            auto animIt = animations.begin();
            while (animIt != animations.end())
            {
                auto& animation = *animIt;
                if (((type == ShowEffectType::Fade) && (animation->getType() == priv::Animation::Type::Fade))
                 || ((type == ShowEffectType::Scale) && (animation->getType() == priv::Animation::Type::Resize))
                 || ((type != ShowEffectType::Fade) && (animation->getType() == priv::Animation::Type::Move)))
                {
                    animation->finish();
                    animIt = animations.erase(animIt);
                }
                else
                    ++animIt;
            }
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        static Layout2d parseLayout(String str)
        {
            if (str.empty())
                throw Exception{"Failed to parse layout. String was empty."};

            // Remove the brackets around the value
            if (((str.front() == '(') && (str.back() == ')')) || ((str.front() == '{') && (str.back() == '}')))
                str = str.substr(1, str.length() - 2);

            if (str.empty())
                return {0, 0};

            // Find the comma that splits the x and y layouts, taking into account that these layouts may contain commas too
            unsigned int bracketCount = 0;
            auto commaOrBracketPos = str.find_first_of(",()");
            decltype(commaOrBracketPos) commaPos = 0;
            while (commaOrBracketPos != String::npos)
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
            String x = str.substr(0, commaPos).trim();
            if ((x.size() >= 2) && ((x[0] == '"') && (x[x.length()-1] == '"')))
                x = x.substr(1, x.length()-2);

            String y = str.substr(commaPos + 1).trim();
            if ((y.size() >= 2) && ((y[0] == '"') && (y[y.length()-1] == '"')))
                y = y.substr(1, y.length()-2);

            return {x, y};
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    static Vector2f parseVector2f(String str)
    {
        if (str.empty())
            throw Exception{"Failed to parse Vector2f string. String was empty."};

        // Remove the brackets around the value
        if ((str.front() == '(') && (str.back() == ')'))
            str = str.substr(1, str.length() - 2);

        const auto commaPos = str.find(',');
        if (commaPos == String::npos)
            throw Exception{"Failed to parse Vector2f string '" + str + "'. No comma found."};

        return {str.substr(0, commaPos).trim().toFloat(), str.substr(commaPos + 1).trim().toFloat()};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
TGUI_IGNORE_DEPRECATED_WARNINGS_START
    Widget::Widget(const char* typeName, bool initRenderer) :
        m_type(typeName)
    {
        if (initRenderer)
        {
            m_renderer = aurora::makeCopied<WidgetRenderer>();
            m_renderer->subscribe(this, m_rendererChangedCallback);
        }
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

        SignalManager::getSignalManager()->remove(this);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Widget::Widget(const Widget& other) :
        enable_shared_from_this<Widget>{other},
        m_type                         {other.m_type},
        m_name                         {other.m_name},
        m_position                     {other.m_position},
        m_size                         {other.m_size},
        m_textSize                     {other.m_textSize},
        m_origin                       {other.m_origin},
        m_rotationOrigin               {other.m_rotationOrigin},
        m_scaleOrigin                  {other.m_scaleOrigin},
        m_scaleFactors                 {other.m_scaleFactors},
        m_rotationDeg                  {other.m_rotationDeg},
        m_boundPositionLayouts         {},
        m_boundSizeLayouts             {},
        m_enabled                      {other.m_enabled},
        m_visible                      {other.m_visible},
        m_parent                       {nullptr},
        m_parentGui                    {nullptr},
        m_draggableWidget              {other.m_draggableWidget},
        m_containerWidget              {other.m_containerWidget},
        m_toolTip                      {other.m_toolTip ? other.m_toolTip->clone() : nullptr},
        m_renderer                     {other.m_renderer},
        m_showAnimations               {other.m_showAnimations},
        m_fontCached                   {other.m_fontCached},
        m_opacityCached                {other.m_opacityCached},
        m_mouseCursor                  {other.m_mouseCursor}
    {
        m_position.x.connectWidget(this, true, [this]{ setPosition(getPositionLayout()); });
        m_position.y.connectWidget(this, false, [this]{ setPosition(getPositionLayout()); });
        m_size.x.connectWidget(this, true, [this]{ setSize(getSizeLayout()); });
        m_size.y.connectWidget(this, false, [this]{ setSize(getSizeLayout()); });

        m_renderer->subscribe(this, m_rendererChangedCallback);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Widget::Widget(Widget&& other) :
        enable_shared_from_this<Widget>{std::move(other)},
        onPositionChange               {std::move(other.onPositionChange)},
        onSizeChange                   {std::move(other.onSizeChange)},
        onFocus                        {std::move(other.onFocus)},
        onUnfocus                      {std::move(other.onUnfocus)},
        onMouseEnter                   {std::move(other.onMouseEnter)},
        onMouseLeave                   {std::move(other.onMouseLeave)},
        m_type                         {std::move(other.m_type)},
        m_name                         {std::move(other.m_name)},
        m_position                     {std::move(other.m_position)},
        m_size                         {std::move(other.m_size)},
        m_textSize                     {std::move(other.m_textSize)},
        m_origin                       {std::move(other.m_origin)},
        m_rotationOrigin               {std::move(other.m_rotationOrigin)},
        m_scaleOrigin                  {std::move(other.m_scaleOrigin)},
        m_scaleFactors                 {std::move(other.m_scaleFactors)},
        m_rotationDeg                  {std::move(other.m_rotationDeg)},
        m_boundPositionLayouts         {std::move(other.m_boundPositionLayouts)},
        m_boundSizeLayouts             {std::move(other.m_boundSizeLayouts)},
        m_enabled                      {std::move(other.m_enabled)},
        m_visible                      {std::move(other.m_visible)},
        m_parent                       {nullptr},
        m_parentGui                    {nullptr},
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
        m_opacityCached                {std::move(other.m_opacityCached)},
        m_mouseCursor                  {std::move(other.m_mouseCursor)}
    {
        m_position.x.connectWidget(this, true, [this]{ setPosition(getPositionLayout()); });
        m_position.y.connectWidget(this, false, [this]{ setPosition(getPositionLayout()); });
        m_size.x.connectWidget(this, true, [this]{ setSize(getSizeLayout()); });
        m_size.y.connectWidget(this, false, [this]{ setSize(getSizeLayout()); });

        other.m_renderer->unsubscribe(&other);
        m_renderer->subscribe(this, m_rendererChangedCallback);

        other.m_renderer = nullptr;

        if (other.m_parent)
            other.m_parent->remove(other.shared_from_this());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Widget& Widget::operator=(const Widget& other)
    {
        if (this != &other)
        {
            m_renderer->unsubscribe(this);

            enable_shared_from_this::operator=(other);

            onPositionChange.disconnectAll();
            onSizeChange.disconnectAll();
            onFocus.disconnectAll();
            onUnfocus.disconnectAll();
            onMouseEnter.disconnectAll();
            onMouseLeave.disconnectAll();

            m_type                 = other.m_type;
            m_name                 = other.m_name;
            m_position             = other.m_position;
            m_size                 = other.m_size;
            m_textSize             = other.m_textSize;
            m_origin               = other.m_origin;
            m_rotationOrigin       = other.m_rotationOrigin;
            m_scaleOrigin          = other.m_scaleOrigin;
            m_scaleFactors         = other.m_scaleFactors;
            m_rotationDeg          = other.m_rotationDeg;
            m_boundPositionLayouts = {};
            m_boundSizeLayouts     = {};
            m_enabled              = other.m_enabled;
            m_visible              = other.m_visible;
            m_parent               = nullptr;
            m_parentGui            = nullptr;
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
            m_mouseCursor          = other.m_mouseCursor;

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

            enable_shared_from_this::operator=(std::move(other));

            onPositionChange       = std::move(other.onPositionChange);
            onSizeChange           = std::move(other.onSizeChange);
            onFocus                = std::move(other.onFocus);
            onUnfocus              = std::move(other.onUnfocus);
            onMouseEnter           = std::move(other.onMouseEnter);
            onMouseLeave           = std::move(other.onMouseLeave);
            m_type                 = std::move(other.m_type);
            m_name                 = std::move(other.m_name);
            m_position             = std::move(other.m_position);
            m_size                 = std::move(other.m_size);
            m_textSize             = std::move(other.m_textSize);
            m_origin               = std::move(other.m_origin);
            m_rotationOrigin       = std::move(other.m_rotationOrigin);
            m_scaleOrigin          = std::move(other.m_scaleOrigin);
            m_scaleFactors         = std::move(other.m_scaleFactors);
            m_rotationDeg          = std::move(other.m_rotationDeg);
            m_boundPositionLayouts = std::move(other.m_boundPositionLayouts);
            m_boundSizeLayouts     = std::move(other.m_boundSizeLayouts);
            m_enabled              = std::move(other.m_enabled);
            m_visible              = std::move(other.m_visible);
            m_parent               = nullptr;
            m_parentGui            = nullptr;
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
            m_mouseCursor          = std::move(other.m_mouseCursor);

            m_position.x.connectWidget(this, true, [this]{ setPosition(getPositionLayout()); });
            m_position.y.connectWidget(this, false, [this]{ setPosition(getPositionLayout()); });
            m_size.x.connectWidget(this, true, [this]{ setSize(getSizeLayout()); });
            m_size.y.connectWidget(this, false, [this]{ setSize(getSizeLayout()); });

            m_renderer->subscribe(this, m_rendererChangedCallback);

            other.m_renderer = nullptr;

            if (other.m_parent)
                SignalManager::getSignalManager()->remove(&other);
        }

        return *this;
    }
TGUI_IGNORE_DEPRECATED_WARNINGS_END
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
        Vector2f pos = getPosition();
        if (m_parent)
            pos += m_parent->getAbsolutePosition() + m_parent->getChildWidgetsOffset();

        const bool defaultOrigin = (getOrigin().x == 0) && (getOrigin().y == 0);
        const bool scaledOrRotated = (getScale().x != 1) || (getScale().y != 1) || (getRotation() != 0);
        if (defaultOrigin && !scaledOrRotated)
            return pos;

        const Vector2f origin{getOrigin().x * getSize().x, getOrigin().y * getSize().y};
        if (!scaledOrRotated)
            return pos - origin;

        const Vector2f rotOrigin{getRotationOrigin().x * getSize().x, getRotationOrigin().y * getSize().y};
        const Vector2f scaleOrigin{getScaleOrigin().x * getSize().x, getScaleOrigin().y * getSize().y};

        Transform transform;
        transform.translate(-origin);
        transform.rotate(getRotation(), rotOrigin);
        transform.scale(getScale(), scaleOrigin);
        return Vector2f(transform.transformPoint({0, 0})) + pos;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Vector2f Widget::getWidgetOffset() const
    {
        return Vector2f{0, 0};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Widget::setOrigin(Vector2f origin)
    {
        m_origin = origin;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Widget::setScale(Vector2f scaleFactors)
    {
        m_scaleFactors = scaleFactors;
        m_scaleOrigin.reset();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Widget::setScale(Vector2f scaleFactors, Vector2f origin)
    {
        m_scaleFactors = scaleFactors;
        m_scaleOrigin = origin;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Vector2f Widget::getScaleOrigin() const
    {
        if (m_scaleOrigin)
            return *m_scaleOrigin;
        else
            return m_origin;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Widget::setRotation(float angle)
    {
        m_rotationDeg = angle;
        m_rotationOrigin.reset();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Widget::setRotation(float angle, Vector2f origin)
    {
        m_rotationDeg = angle;
        m_rotationOrigin = origin;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Vector2f Widget::getRotationOrigin() const
    {
        if (m_rotationOrigin)
            return *m_rotationOrigin;
        else
            return m_origin;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
TGUI_IGNORE_DEPRECATED_WARNINGS_START
    void Widget::showWithEffect(ShowEffectType type, Duration duration)
    {
        setVisible(true);

        // We store the state the widget is currently in. In the event another animation was already playing, we should try to
        // use the current state to start our animation at, but this is not the state that the widget should end at. We must
        // get this state BEFORE finishing the previous animation which is done by finishExistingConflictingAnimations.
        const float startOpacity = getInheritedOpacity();
        //const Vector2f startPosition = getPosition();
        //const Vector2f startSize = getSize();

        finishExistingConflictingAnimations(m_showAnimations, type);

        switch (type)
        {
            case ShowEffectType::Fade:
            {
                // Start from fully transparent, unless a fade animation was already playing
                float animStartOpacity = startOpacity;
                const float endOpacity = getInheritedOpacity();
                if (startOpacity == endOpacity)
                {
                    setInheritedOpacity(0);
                    animStartOpacity = 0;
                }
                else // If fading was already in progress then adapt the duration to finish the animation sooner
                    duration *= (startOpacity / endOpacity);

                m_showAnimations.push_back(std::make_shared<priv::FadeAnimation>(shared_from_this(), animStartOpacity, endOpacity, duration,
                                                                                 TGUI_LAMBDA_CAPTURE_EQ_THIS{onAnimationFinish.emit(this, type, true); onShowEffectFinish.emit(this, type, true); }));
                break;
            }
            case ShowEffectType::Scale:
            {
                // TODO: Use setScale instead of setSize
                m_showAnimations.push_back(std::make_shared<priv::MoveAnimation>(shared_from_this(), getPosition() + (getSize() / 2.f), m_position, duration));
                m_showAnimations.push_back(std::make_shared<priv::ResizeAnimation>(shared_from_this(), Vector2f{0, 0}, m_size, duration,
                                                                                   TGUI_LAMBDA_CAPTURE_EQ_THIS{onAnimationFinish.emit(this, type, true); onShowEffectFinish.emit(this, type, true); }));
                setPosition(getPosition() + (getSize() / 2.f));
                setSize(0, 0);
                break;
            }
            case ShowEffectType::SlideFromLeft:
            {
                m_showAnimations.push_back(std::make_shared<priv::MoveAnimation>(shared_from_this(), Vector2f{-getFullSize().x, getPosition().y}, m_position, duration,
                                                                                 TGUI_LAMBDA_CAPTURE_EQ_THIS{onAnimationFinish.emit(this, type, true); onShowEffectFinish.emit(this, type, true); }));
                setPosition({-getFullSize().x, getPosition().y});
                break;
            }
            case ShowEffectType::SlideFromRight:
            {
                if (getParent())
                {
                    m_showAnimations.push_back(std::make_shared<priv::MoveAnimation>(shared_from_this(), Vector2f{getParent()->getSize().x + getWidgetOffset().x, getPosition().y}, m_position, duration,
                                                                                     TGUI_LAMBDA_CAPTURE_EQ_THIS{onAnimationFinish.emit(this, type, true); onShowEffectFinish.emit(this, type, true); }));
                    setPosition({getParent()->getSize().x + getWidgetOffset().x, getPosition().y});
                }
                else
                {
                    TGUI_PRINT_WARNING("showWithEffect(SlideFromRight) does not work before widget has a parent.");
                }

                break;
            }
            case ShowEffectType::SlideFromTop:
            {
                m_showAnimations.push_back(std::make_shared<priv::MoveAnimation>(shared_from_this(), Vector2f{getPosition().x, -getFullSize().y}, m_position, duration,
                                                                                 TGUI_LAMBDA_CAPTURE_EQ_THIS{onAnimationFinish.emit(this, type, true); onShowEffectFinish.emit(this, type, true); }));
                setPosition({getPosition().x, -getFullSize().y});
                break;
            }
            case ShowEffectType::SlideFromBottom:
            {
                if (getParent())
                {
                    m_showAnimations.push_back(std::make_shared<priv::MoveAnimation>(shared_from_this(), Vector2f{getPosition().x, getParent()->getSize().y + getWidgetOffset().y}, m_position, duration,
                                                                                     TGUI_LAMBDA_CAPTURE_EQ_THIS{onAnimationFinish.emit(this, type, true); onShowEffectFinish.emit(this, type, true); }));
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

    void Widget::hideWithEffect(ShowEffectType type, Duration duration)
    {
        // We store the state the widget is currently in. In the event another animation was already playing, we should try to
        // use the current state to start our animation at, but this is not the state that the widget should end at. We must
        // get this state BEFORE finishing the previous animation which is done by finishExistingConflictingAnimations.
        const float startOpacity = getInheritedOpacity();
        //const Vector2f startPosition = getPosition();
        //const Vector2f startSize = getSize();

        finishExistingConflictingAnimations(m_showAnimations, type);

        const Vector2f position = getPosition();
        const Layout2d positionLayout = m_position;

        switch (type)
        {
            case ShowEffectType::Fade:
            {
                const float endOpacity = getInheritedOpacity(); // Value to reset to after widget is hidden

                // If fading was already in progress then adapt the duration to finish the animation sooner
                if (startOpacity != endOpacity)
                    duration *= (startOpacity / endOpacity);

                m_showAnimations.push_back(std::make_shared<priv::FadeAnimation>(shared_from_this(), startOpacity, 0.f, duration,
                    TGUI_LAMBDA_CAPTURE_EQ_THIS{ setVisible(false); setInheritedOpacity(endOpacity); onAnimationFinish.emit(this, type, false); onShowEffectFinish.emit(this, type, false); }));
                break;
            }
            case ShowEffectType::Scale:
            {
                // TODO: Use setScale instead of setSize
                const Vector2f size = getSize();
                const Layout2d sizeLayout = m_size;
                m_showAnimations.push_back(std::make_shared<priv::MoveAnimation>(shared_from_this(), position, position + (size / 2.f), duration));
                m_showAnimations.push_back(std::make_shared<priv::ResizeAnimation>(shared_from_this(), size, Vector2f{0, 0}, duration,
                    TGUI_LAMBDA_CAPTURE_EQ_THIS{ setVisible(false); setPosition(positionLayout); setSize(sizeLayout); onAnimationFinish.emit(this, type, false); onShowEffectFinish.emit(this, type, false); }));
                break;
            }
            case ShowEffectType::SlideToRight:
            {
                if (getParent())
                {
                    m_showAnimations.push_back(std::make_shared<priv::MoveAnimation>(shared_from_this(), position, Vector2f{getParent()->getSize().x + getWidgetOffset().x, position.y}, duration,
                        TGUI_LAMBDA_CAPTURE_EQ_THIS{ setVisible(false); setPosition(positionLayout); onAnimationFinish.emit(this, type, false); onShowEffectFinish.emit(this, type, false); }));
                }
                else
                {
                    TGUI_PRINT_WARNING("hideWithEffect(SlideToRight) does not work before widget has a parent.");
                }

                break;
            }
            case ShowEffectType::SlideToLeft:
            {
                m_showAnimations.push_back(std::make_shared<priv::MoveAnimation>(shared_from_this(), position, Vector2f{-getFullSize().x, position.y}, duration,
                    TGUI_LAMBDA_CAPTURE_EQ_THIS{ setVisible(false); setPosition(positionLayout); onAnimationFinish.emit(this, type, false); onShowEffectFinish.emit(this, type, false); }));
                break;
            }
            case ShowEffectType::SlideToBottom:
            {
                if (getParent())
                {
                    m_showAnimations.push_back(std::make_shared<priv::MoveAnimation>(shared_from_this(), position, Vector2f{position.x, getParent()->getSize().y + getWidgetOffset().y}, duration,
                        TGUI_LAMBDA_CAPTURE_EQ_THIS{ setVisible(false); setPosition(positionLayout); onAnimationFinish.emit(this, type, false); onShowEffectFinish.emit(this, type, false); }));
                }
                else
                {
                    TGUI_PRINT_WARNING("hideWithEffect(SlideToBottom) does not work before widget has a parent.");
                }

                break;
            }
            case ShowEffectType::SlideToTop:
            {
                m_showAnimations.push_back(std::make_shared<priv::MoveAnimation>(shared_from_this(), position, Vector2f{position.x, -getFullSize().y}, duration,
                    TGUI_LAMBDA_CAPTURE_EQ_THIS{ setVisible(false); setPosition(positionLayout); onAnimationFinish.emit(this, type, false); onShowEffectFinish.emit(this, type, false); }));
                break;
            }
        }
    }
TGUI_IGNORE_DEPRECATED_WARNINGS_END
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Widget::setVisible(bool visible)
    {
        m_visible = visible;

        // If the widget is hiden while still focused then it must be unfocused
        if (!visible)
        {
            setFocused(false);

            // If we were still interacting with the widget then stop doing so
            if (m_mouseHover)
                mouseNoLongerOnWidget();

            if (m_mouseDown)
                leftMouseButtonNoLongerDown();

            rightMouseButtonNoLongerDown();
        }
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
            rendererChanged("OpacityDisabled");
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

    const String& Widget::getWidgetType() const
    {
        return m_type;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Widget::isAnimationPlaying() const
    {
        return !m_showAnimations.empty();
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
        rendererChanged("Font");
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
        rendererChanged("Opacity");
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    float Widget::getInheritedOpacity() const
    {
        return m_inheritedOpacity;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Widget::setTextSize(unsigned int size)
    {
        m_textSize = size;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int Widget::getTextSize() const
    {
        return m_textSize;
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

    void Widget::setWidgetName(const String& name)
    {
        if (m_name != name)
        {
            m_name = name;
            if (m_parent)
            {
                SignalManager::getSignalManager()->remove(this);
                SignalManager::getSignalManager()->add(shared_from_this());
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    String Widget::getWidgetName() const
    {
        return m_name;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Widget::setMouseCursor(Cursor::Type cursor)
    {
        m_mouseCursor = cursor;

        if (m_mouseHover)
            m_parentGui->requestMouseCursor(cursor);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Cursor::Type Widget::getMouseCursor() const
    {
        return m_mouseCursor;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Widget::setFocusable(bool focusable)
    {
        m_focusable = focusable;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Widget::isFocusable() const
    {
        return m_focusable;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Widget::canGainFocus() const
    {
        return m_enabled && m_visible && m_focusable;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Widget::isContainer() const
    {
        return m_containerWidget;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Widget::isDraggableWidget() const
    {
        return m_draggableWidget;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Widget::isMouseDown() const
    {
        return m_mouseDown;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Widget::setParent(Container* parent)
    {
        m_parentGui = parent ? parent->getParentGui() : nullptr;
        if (m_parent == parent)
            return;

        if (!parent)
            SignalManager::getSignalManager()->remove(this);
        else if (!m_parent)
            SignalManager::getSignalManager()->add(shared_from_this());

        m_parent = parent;

        // Give the layouts another chance to find widgets to which it refers
        if (parent)
        {
            m_position.x.connectWidget(this, true, [this]{ setPosition(getPositionLayout()); });
            m_position.y.connectWidget(this, false, [this]{ setPosition(getPositionLayout()); });
            m_size.x.connectWidget(this, true, [this]{ setSize(getSizeLayout()); });
            m_size.y.connectWidget(this, false, [this]{ setSize(getSizeLayout()); });
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Widget::updateTime(Duration elapsedTime)
    {
        m_animationTimeElapsed += elapsedTime;

        const bool screenRefreshRequired = !m_showAnimations.empty();
        for (unsigned int i = 0; i < m_showAnimations.size();)
        {
            if (m_showAnimations[i]->update(elapsedTime))
                m_showAnimations.erase(m_showAnimations.begin() + i);
            else
                i++;
        }

        return screenRefreshRequired;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Widget::leftMousePressed(Vector2f)
    {
        m_mouseDown = true;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Widget::leftMouseReleased(Vector2f)
    {
        m_mouseDown = false;
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

    void Widget::mousePressed(Event::MouseButton button, Vector2f pos)
    {
        if (button == Event::MouseButton::Left)
            leftMousePressed(pos);
        else if (button == Event::MouseButton::Right)
            rightMousePressed(pos);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Widget::mouseReleased(Event::MouseButton button, Vector2f pos)
    {
        if (button == Event::MouseButton::Left)
            leftMouseReleased(pos);
        else if (button == Event::MouseButton::Right)
            rightMouseReleased(pos);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Widget::mouseMoved(Vector2f)
    {
        if (!m_mouseHover)
            mouseEnteredWidget();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Widget::keyPressed(const Event::KeyEvent&)
    {
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Widget::textEntered(char32_t)
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

    void Widget::leftMouseButtonNoLongerDown()
    {
        m_mouseDown = false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Widget::rightMouseButtonNoLongerDown()
    {
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Widget::Ptr Widget::askToolTip(Vector2f mousePos)
    {
        if (m_toolTip && isMouseOnWidget(mousePos))
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
TGUI_IGNORE_DEPRECATED_WARNINGS_START
    Signal& Widget::getSignal(String signalName)
    {
        if (signalName == onPositionChange.getName())
            return onPositionChange;
        else if (signalName == onSizeChange.getName())
            return onSizeChange;
        else if (signalName == onFocus.getName())
            return onFocus;
        else if (signalName == onUnfocus.getName())
            return onUnfocus;
        else if (signalName == onMouseEnter.getName())
            return onMouseEnter;
        else if (signalName == onMouseLeave.getName())
            return onMouseLeave;
        else if (signalName == onAnimationFinish.getName())
            return onAnimationFinish;
        else if (signalName == onShowEffectFinish.getName())
            return onShowEffectFinish;

        throw Exception{"No signal exists with name '" + std::move(signalName) + "'."};
    }
TGUI_IGNORE_DEPRECATED_WARNINGS_END
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Widget::rendererChanged(const String& property)
    {
        if ((property == "Opacity") || (property == "OpacityDisabled"))
        {
            if (!m_enabled && (getSharedRenderer()->getOpacityDisabled() != -1))
                m_opacityCached = getSharedRenderer()->getOpacityDisabled() * m_inheritedOpacity;
            else
                m_opacityCached = getSharedRenderer()->getOpacity() * m_inheritedOpacity;
        }
        else if (property == "Font")
        {
            if (getSharedRenderer()->getFont())
                m_fontCached = getSharedRenderer()->getFont();
            else if (m_inheritedFont)
                m_fontCached = m_inheritedFont;
            else
                m_fontCached = Font::getGlobalFont();
        }
        else if (property == "TransparentTexture")
        {
            m_transparentTextureCached = getSharedRenderer()->getTransparentTexture();
        }
        else
            throw Exception{"Could not set property '" + property + "', widget of type '" + getWidgetType() + "' does not has this property."};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::unique_ptr<DataIO::Node> Widget::save(SavingRenderersMap& renderers) const
    {
        auto node = std::make_unique<DataIO::Node>();
        if (m_name.empty())
            node->name = getWidgetType();
        else
            node->name = getWidgetType() + "." + Serializer::serialize(m_name);

        if (!isVisible())
            node->propertyValuePairs["Visible"] = std::make_unique<DataIO::ValueNode>("false");
        if (!isEnabled())
            node->propertyValuePairs["Enabled"] = std::make_unique<DataIO::ValueNode>("false");
        if (getPosition() != Vector2f{})
            node->propertyValuePairs["Position"] = std::make_unique<DataIO::ValueNode>(m_position.toString());
        if (getSize() != Vector2f{})
            node->propertyValuePairs["Size"] = std::make_unique<DataIO::ValueNode>(m_size.toString());
        if (getOrigin() != Vector2f{})
            node->propertyValuePairs["Origin"] = std::make_unique<DataIO::ValueNode>("(" + String::fromNumber(m_origin.x) + "," + String::fromNumber(m_origin.y) + ")");
        if (getScale() != Vector2f{1, 1})
        {
            node->propertyValuePairs["Scale"] = std::make_unique<DataIO::ValueNode>("(" + String::fromNumber(m_scaleFactors.x) + "," + String::fromNumber(m_scaleFactors.y) + ")");
            if (m_scaleOrigin)
                node->propertyValuePairs["ScaleOrigin"] = std::make_unique<DataIO::ValueNode>("(" + String::fromNumber(m_scaleOrigin->x) + "," + String::fromNumber(m_scaleOrigin->y) + ")");
        }
        if (getRotation() != 0)
        {
            node->propertyValuePairs["Rotation"] = std::make_unique<DataIO::ValueNode>(String::fromNumber(m_rotationDeg));
            if (m_rotationOrigin)
                node->propertyValuePairs["RotationOrigin"] = std::make_unique<DataIO::ValueNode>("(" + String::fromNumber(m_rotationOrigin->x) + "," + String::fromNumber(m_rotationOrigin->y) + ")");
        }
#if TGUI_COMPILED_WITH_CPP_VER >= 17
        if (m_userData.has_value())
        {
            if (m_userData.type() == typeid(String))
            {
                const String string = std::any_cast<String>(m_userData);
                node->propertyValuePairs["UserData"] = std::make_unique<DataIO::ValueNode>(Serializer::serialize(string));
            }
            else if (m_userData.type() == typeid(std::string))
            {
                const String string = std::any_cast<std::string>(m_userData);
                node->propertyValuePairs["UserData"] = std::make_unique<DataIO::ValueNode>(Serializer::serialize(string));
            }
            else if (m_userData.type() == typeid(const char*))
            {
                const String string = std::any_cast<const char*>(m_userData);
                node->propertyValuePairs["UserData"] = std::make_unique<DataIO::ValueNode>(Serializer::serialize(string));
            }
        }
#else
        if (m_userData.not_null())
        {
            if (m_userData.is<String>())
            {
                node->propertyValuePairs["UserData"] = std::make_unique<DataIO::ValueNode>(Serializer::serialize(m_userData.as<String>()));
            }
            else if (m_userData.is<std::string>())
            {
                node->propertyValuePairs["UserData"] = std::make_unique<DataIO::ValueNode>(Serializer::serialize(String(m_userData.as<std::string>()))) ;
            }
            else if (m_userData.is<const char*>())
            {
                node->propertyValuePairs["UserData"] = std::make_unique<DataIO::ValueNode>(Serializer::serialize(m_userData.as<const char*>()));
            }
        }
#endif

        String mouseCursorStr;
        switch (m_mouseCursor)
        {
            case Cursor::Type::Text:            mouseCursorStr = "Text"; break;
            case Cursor::Type::Hand:            mouseCursorStr = "Hand"; break;
            case Cursor::Type::SizeLeft:        mouseCursorStr = "SizeLeft"; break;
            case Cursor::Type::SizeRight:       mouseCursorStr = "SizeRight"; break;
            case Cursor::Type::SizeTop:         mouseCursorStr = "SizeTop"; break;
            case Cursor::Type::SizeBottom:      mouseCursorStr = "SizeBottom"; break;
            case Cursor::Type::SizeBottomRight: mouseCursorStr = "SizeBottomRight"; break;
            case Cursor::Type::SizeTopLeft:     mouseCursorStr = "SizeTopLeft"; break;
            case Cursor::Type::SizeBottomLeft:  mouseCursorStr = "SizeBottomLeft"; break;
            case Cursor::Type::SizeTopRight:    mouseCursorStr = "SizeTopRight"; break;
            case Cursor::Type::Crosshair:       mouseCursorStr = "Crosshair"; break;
            case Cursor::Type::Help:            mouseCursorStr = "Help"; break;
            case Cursor::Type::NotAllowed:      mouseCursorStr = "NotAllowed"; break;
            case Cursor::Type::Arrow:           break; // We don't save the cursor if it has the default value
        }
        if (!mouseCursorStr.empty())
            node->propertyValuePairs["MouseCursor"] = std::make_unique<DataIO::ValueNode>(mouseCursorStr);

        if (getToolTip() != nullptr)
        {
            auto toolTipWidgetNode = getToolTip()->save(renderers);

            auto toolTipNode = std::make_unique<DataIO::Node>();
            toolTipNode->name = "ToolTip";
            toolTipNode->children.emplace_back(std::move(toolTipWidgetNode));

            toolTipNode->propertyValuePairs["InitialDelay"] = std::make_unique<DataIO::ValueNode>(String::fromNumber(ToolTip::getInitialDelay().asSeconds()));
            toolTipNode->propertyValuePairs["DistanceToMouse"] = std::make_unique<DataIO::ValueNode>("("
                + String::fromNumber(ToolTip::getDistanceToMouse().x) + "," + String::fromNumber(ToolTip::getDistanceToMouse().y) + ")");

            node->children.emplace_back(std::move(toolTipNode));
        }

        if (renderers.at(this).first)
            node->children.emplace_back(std::move(renderers.at(this).first));
        else
            node->propertyValuePairs["Renderer"] = std::make_unique<DataIO::ValueNode>("&" + renderers.at(this).second);

        return node;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Widget::load(const std::unique_ptr<DataIO::Node>& node, const LoadingRenderersMap& renderers)
    {
        if (node->propertyValuePairs["Visible"])
            setVisible(Deserializer::deserialize(ObjectConverter::Type::Bool, node->propertyValuePairs["Visible"]->value).getBool());
        if (node->propertyValuePairs["Enabled"])
            setEnabled(Deserializer::deserialize(ObjectConverter::Type::Bool, node->propertyValuePairs["Enabled"]->value).getBool());
        if (node->propertyValuePairs["Position"])
            setPosition(parseLayout(node->propertyValuePairs["Position"]->value));
        if (node->propertyValuePairs["Size"])
            setSize(parseLayout(node->propertyValuePairs["Size"]->value));
        if (node->propertyValuePairs["Origin"])
            setOrigin(parseVector2f(node->propertyValuePairs["Origin"]->value));
        if (node->propertyValuePairs["Scale"])
        {
            if (node->propertyValuePairs["ScaleOrigin"])
                setScale(parseVector2f(node->propertyValuePairs["Scale"]->value), parseVector2f(node->propertyValuePairs["ScaleOrigin"]->value));
            else
                setScale(parseVector2f(node->propertyValuePairs["Scale"]->value));
        }
        if (node->propertyValuePairs["Rotation"])
        {
            if (node->propertyValuePairs["RotationOrigin"])
                setRotation(node->propertyValuePairs["Rotation"]->value.toFloat(), parseVector2f(node->propertyValuePairs["RotationOrigin"]->value));
            else
                setRotation(node->propertyValuePairs["Rotation"]->value.toFloat());
        }
        if (node->propertyValuePairs["UserData"])
        {
#if TGUI_COMPILED_WITH_CPP_VER >= 17
            m_userData = std::make_any<String>(Deserializer::deserialize(ObjectConverter::Type::String, node->propertyValuePairs["UserData"]->value).getString());
#else
            m_userData = tgui::Any(Deserializer::deserialize(ObjectConverter::Type::String, node->propertyValuePairs["UserData"]->value).getString());
#endif
        }

        if (node->propertyValuePairs["MouseCursor"])
        {
            String cursorStr = node->propertyValuePairs["MouseCursor"]->value.trim();
            if (cursorStr == "Text")
                m_mouseCursor = Cursor::Type::Text;
            else if (cursorStr == "Hand")
                m_mouseCursor = Cursor::Type::Hand;
            else if (cursorStr == "SizeLeft")
                m_mouseCursor = Cursor::Type::SizeLeft;
            else if (cursorStr == "SizeRight")
                m_mouseCursor = Cursor::Type::SizeRight;
            else if (cursorStr == "SizeTop")
                m_mouseCursor = Cursor::Type::SizeTop;
            else if (cursorStr == "SizeBottom")
                m_mouseCursor = Cursor::Type::SizeBottom;
            else if (cursorStr == "SizeBottomRight")
                m_mouseCursor = Cursor::Type::SizeBottomRight;
            else if (cursorStr == "SizeTopLeft")
                m_mouseCursor = Cursor::Type::SizeTopLeft;
            else if (cursorStr == "SizeBottomLeft")
                m_mouseCursor = Cursor::Type::SizeBottomLeft;
            else if (cursorStr == "SizeTopRight")
                m_mouseCursor = Cursor::Type::SizeTopRight;
            else if (cursorStr == "Crosshair")
                m_mouseCursor = Cursor::Type::Crosshair;
            else if (cursorStr == "Help")
                m_mouseCursor = Cursor::Type::Help;
            else if (cursorStr == "NotAllowed")
                m_mouseCursor = Cursor::Type::NotAllowed;
            else if (cursorStr == "Arrow")
                m_mouseCursor = Cursor::Type::Arrow;
            else
                throw Exception{"Failed to parse 'MouseCursor' property. Invalid cursor '" + cursorStr + "'."};
        }

        if (node->propertyValuePairs["Renderer"])
        {
            const String value = node->propertyValuePairs["Renderer"]->value;
            if (value.empty() || (value[0] != '&'))
                throw Exception{"Expected reference to renderer, did not find '&' character"};

            const auto it = renderers.find(value.substr(1));
            if (it == renderers.end())
                throw Exception{"Widget refers to renderer with name '" + value.substr(1) + "', but no such renderer was found"};

            setRenderer(it->second);
        }

        for (const auto& childNode : node->children)
        {
            if (childNode->name == "ToolTip")
            {
                for (const auto& pair : childNode->propertyValuePairs)
                {
                    if (pair.first == "InitialDelay")
                        ToolTip::setInitialDelay(std::chrono::duration<float>(pair.second->value.toFloat()));
                    else if (pair.first == "DistanceToMouse")
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
            else if (childNode->name == "Renderer")
                setRenderer(RendererData::createFromDataIONode(childNode.get()));

            /// TODO: Signals?
        }
        node->children.erase(std::remove_if(node->children.begin(), node->children.end(), [](const std::unique_ptr<DataIO::Node>& child){
                return (child->name == "ToolTip") || (child->name == "Renderer");
            }), node->children.end());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Widget::mouseEnteredWidget()
    {
        if (m_parentGui && (m_mouseCursor != Cursor::Type::Arrow))
            m_parentGui->requestMouseCursor(m_mouseCursor);

        m_mouseHover = true;
        onMouseEnter.emit(this);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Widget::mouseLeftWidget()
    {
        if (m_parentGui && m_parent && (m_parent->getMouseCursor() != m_mouseCursor))
            m_parentGui->requestMouseCursor(m_parent->getMouseCursor());

        m_mouseHover = false;
        onMouseLeave.emit(this);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Widget::rendererChangedCallback(const String& property)
    {
        rendererChanged(property);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
