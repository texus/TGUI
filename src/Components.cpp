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


#include <TGUI/Components.hpp>
#include <TGUI/Backend/Renderer/BackendRenderTarget.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
namespace priv
{
namespace dev
{
/**
    template<typename ValueType>
    std::unordered_map<std::uint64_t, ValueType> StyleProperty<ValueType>::m_globalValues;

    template<typename ValueType>
    std::uint64_t StyleProperty<ValueType>::m_nextGlobalValueIndex = 0;
*/
    template class StyleProperty<Color>;
    template class StyleProperty<Texture>;
    template class StyleProperty<Outline>;
    template class StyleProperty<TextStyles>;

    std::unordered_map<std::uint64_t, std::set<std::uint64_t>> MessageBroker::m_topicIdToCallbackIds;
    std::unordered_map<std::uint64_t, std::uint64_t> MessageBroker::m_callbackIdToTopicId;
    std::unordered_map<std::uint64_t, std::function<void()>> MessageBroker::m_listeners;
    std::uint64_t MessageBroker::m_lastId = 0;

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::uint64_t MessageBroker::createTopic()
    {
        return ++m_lastId;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void MessageBroker::destroyTopic(std::uint64_t topicId)
    {
        auto it = m_topicIdToCallbackIds.find(topicId);

        // If nobody subscribed to the topic then we don't have to destroy anything
        if (it == m_topicIdToCallbackIds.end())
            return;

        // Make a copy of the callback ids to prevent issues when unsubscribe removes the value from the set
        const std::set<std::uint64_t> callbackIds = it->second;
        for (const auto& callbackId : callbackIds)
            unsubscribe(callbackId);

        m_topicIdToCallbackIds.erase(it);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::uint64_t MessageBroker::subscribe(std::uint64_t topicId, std::function<void()> func)
    {
        const std::uint64_t callbackId = ++m_lastId;
        m_topicIdToCallbackIds[topicId].insert(callbackId);
        m_callbackIdToTopicId[callbackId] = topicId;
        m_listeners[callbackId] = std::move(func);
        return callbackId;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void MessageBroker::unsubscribe(std::uint64_t callbackId)
    {
        auto listenersIt = m_listeners.find(callbackId);
        if (listenersIt == m_listeners.end())
        {
            // It is possible that the listener has already been unsubscribed if the topic was destroyed
            // before the listener unsubscribes.
            return;
        }

#if defined(__GNUC__)
#   pragma GCC diagnostic push
#   pragma GCC diagnostic ignored "-Wnull-dereference"  // Suppress warning for not checking the iterator in release mode
#endif
        // Find out to which topic this callback belongs
        auto it = m_callbackIdToTopicId.find(callbackId);
        TGUI_ASSERT(it != m_callbackIdToTopicId.end(), "MessageBroker::unsubscribe requires callback id to have matching topic");
        const auto topicId = it->second;
#if defined(__GNUC__)
#   pragma GCC diagnostic pop
#endif

        // Remove the callback
        m_listeners.erase(listenersIt);
        m_callbackIdToTopicId.erase(it);

        TGUI_ASSERT(m_topicIdToCallbackIds.find(topicId) != m_topicIdToCallbackIds.end(), "Topic id needs to have matching callback in MessageBroker::unsubscribe");
        m_topicIdToCallbackIds[topicId].erase(callbackId);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void MessageBroker::sendEvent(std::uint64_t topicId)
    {
        // If nobody subscribed to the topic then we don't need to send anything
        auto it = m_topicIdToCallbackIds.find(topicId);
        if (it == m_topicIdToCallbackIds.end())
            return;

        // Make a copy of the callback ids to prevent issues when an unsubscribe is called during a callback
        const std::set<std::uint64_t> callbackIds = it->second;
        for (const auto& callbackId : callbackIds)
            m_listeners[callbackId]();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Component::Component(const Component& other) :
        m_state(other.m_state),
        m_positionAlignment(other.m_positionAlignment),
        m_position(other.m_position),
        m_size(other.m_size),
        m_visible(other.m_visible),
        m_opacity(other.m_opacity),
        m_parent(nullptr)
    {
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Component& Component::operator=(const Component& other)
    {
        if (&other != this)
        {
            m_state = other.m_state;
            m_positionAlignment = other.m_positionAlignment;
            m_position = other.m_position;
            m_size = other.m_size;
            m_visible = other.m_visible;
            m_opacity = other.m_opacity;
            m_parent = nullptr;
        }

        return *this;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Component::setPosition(Vector2f position)
    {
        m_position = position;
        m_positionAlignment = PositionAlignment::None;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Vector2f Component::getPosition() const
    {
        return m_position;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Vector2f Component::getSize() const
    {
        return m_size;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Component::setPositionAlignment(PositionAlignment alignment)
    {
        if (m_positionAlignment != alignment)
        {
            m_positionAlignment = alignment;
            updateLayout();
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Component::setVisible(bool visible)
    {
        m_visible = visible;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Component::isVisible() const
    {
        return m_visible;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Component::setParent(GroupComponent* parent)
    {
        TGUI_ASSERT(!m_parent, "Component can't be assigned a parent twice");
        m_parent = parent;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Component::updateLayout()
    {
        if (m_positionAlignment != PositionAlignment::None)
        {
            TGUI_ASSERT(m_positionAlignment == PositionAlignment::Center, "Only None and Center position alignments are currently supported by WIP component system");
            if (m_parent)
                m_position = (m_parent->getClientSize() - getSize()) / 2.f;
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void swap(Component& first, Component& second)
    {
        using std::swap;
        swap(first.m_state, second.m_state);
        swap(first.m_positionAlignment, second.m_positionAlignment);
        swap(first.m_position, second.m_position);
        swap(first.m_size, second.m_size);
        swap(first.m_visible, second.m_visible);
        swap(first.m_opacity, second.m_opacity);
        swap(first.m_parent, second.m_parent);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    GroupComponent::GroupComponent(const GroupComponent& other) :
        Component{other},
        m_children{},
        m_clientSize(other.m_clientSize)
    {
        /// TODO: If this GroupComponent is a BackgroundComponent which contains a TextComponent, and this group is being copied
        ///       because the widget needs to be copied, then the new TextComponent somehow needs a pointer to the style property
        ///       of the new widget instead of the old one. We lack information here to clone the TextComponent in a useful way.

        //m_children.reserve(other.m_children.size());
        //for (const auto& otherChild : other.m_children)
        //{
        //    auto newChild = otherChild->clone();
        //    newChild->setParent(this);
        //    m_children.emplace_back(std::move(newChild));
        //}
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    GroupComponent& GroupComponent::operator=(const GroupComponent& other)
    {
        if (&other != this)
        {
            GroupComponent temp(other);

            std::swap(temp.m_children, m_children);
            std::swap(temp.m_clientSize, m_clientSize);
        }

        return *this;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Vector2f GroupComponent::getClientSize() const
    {
        return m_clientSize;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void GroupComponent::addComponent(const std::shared_ptr<Component>& component)
    {
        component->setParent(this);
        m_children.emplace_back(component);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const std::vector<std::shared_ptr<Component>>& GroupComponent::getComponents() const
    {
        return m_children;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void GroupComponent::draw(BackendRenderTarget& target, RenderStates states) const
    {
        for (const auto& child : m_children)
        {
            if (child->isVisible())
                child->draw(target, states);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void GroupComponent::updateLayout()
    {
        Component::updateLayout();

        for (auto& child : m_children)
            child->updateLayout();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::shared_ptr<Component> GroupComponent::clone() const
    {
        return std::make_shared<GroupComponent>(*this);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void swap(GroupComponent& first, GroupComponent& second)
    {
        using std::swap;
        swap(static_cast<Component&>(first), static_cast<Component&>(second));
        swap(first.m_children, second.m_children);
        swap(first.m_clientSize, second.m_clientSize);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    BackgroundComponent::BackgroundComponent(StylePropertyBackground* backgroundStyle) :
        m_backgroundStyle{backgroundStyle}
    {
        init();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    BackgroundComponent::~BackgroundComponent()
    {
        m_backgroundStyle->borderColor.disconnectCallback(m_borderColorCallbackId);
        m_backgroundStyle->color.disconnectCallback(m_backgroundColorCallbackId);
        m_backgroundStyle->texture.disconnectCallback(m_textureCallbackId);
        m_backgroundStyle->borders.disconnectCallback(m_bordersCallbackId);
        m_backgroundStyle->padding.disconnectCallback(m_paddingCallbackId);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    BackgroundComponent::BackgroundComponent(const BackgroundComponent& other, StylePropertyBackground* backgroundStyle) :
        GroupComponent(other),
        m_backgroundStyle{backgroundStyle ? backgroundStyle : other.m_backgroundStyle},
        m_sprite{other.m_sprite}
    {
        init();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    BackgroundComponent& BackgroundComponent::operator=(const BackgroundComponent& other)
    {
        if (&other != this)
        {
            GroupComponent::operator=(other);
            m_sprite = other.m_sprite;

            m_backgroundStyle->borderColor.disconnectCallback(m_borderColorCallbackId);
            m_backgroundStyle->color.disconnectCallback(m_backgroundColorCallbackId);
            m_backgroundStyle->texture.disconnectCallback(m_textureCallbackId);
            m_backgroundStyle->borders.disconnectCallback(m_bordersCallbackId);
            m_backgroundStyle->padding.disconnectCallback(m_paddingCallbackId);

            m_backgroundStyle = other.m_backgroundStyle;
            init();
        }

        return *this;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void BackgroundComponent::init()
    {
        m_borderColorCallbackId = m_backgroundStyle->borderColor.connectCallback([this]{
            m_borderColor = Color::applyOpacity(m_backgroundStyle->borderColor.getValue(m_state), m_opacity);
        });
        m_backgroundColorCallbackId = m_backgroundStyle->color.connectCallback([this]{
            m_background.color = Color::applyOpacity(m_backgroundStyle->color.getValue(m_state), m_opacity);
        });
        m_textureCallbackId = m_backgroundStyle->texture.connectCallback([this]{
            m_sprite.setTexture(m_backgroundStyle->texture.getValue(m_state));
        });
        m_bordersCallbackId = m_backgroundStyle->borders.connectCallback([this]{
            setBorders(m_backgroundStyle->borders.getValue(m_state));
        });
        m_paddingCallbackId = m_backgroundStyle->padding.connectCallback([this]{
            setPadding(m_backgroundStyle->padding.getValue(m_state));
        });

        m_borderColor = Color::applyOpacity(m_backgroundStyle->borderColor.getValue(m_state), m_opacity);
        m_background.color = Color::applyOpacity(m_backgroundStyle->color.getValue(m_state), m_opacity);
        m_sprite.setTexture(m_backgroundStyle->texture.getValue(m_state));
        setBorders(m_backgroundStyle->borders.getValue(m_state));
        setPadding(m_backgroundStyle->padding.getValue(m_state));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void BackgroundComponent::setSize(Vector2f size)
    {
        m_size = size;
        updateLayout();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void BackgroundComponent::setBorders(const Outline& border)
    {
        m_borders = border;
        updateLayout();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const Outline& BackgroundComponent::getBorders() const
    {
        return m_borders;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void BackgroundComponent::setPadding(const Outline& padding)
    {
        m_padding = padding;
        updateLayout();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const Outline& BackgroundComponent::getPadding() const
    {
        return m_padding;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void BackgroundComponent::setOpacity(float opacity)
    {
        m_opacity = opacity;
        m_borderColor = Color::applyOpacity(m_backgroundStyle->borderColor.getValue(m_state), m_opacity);
        m_background.color = Color::applyOpacity(m_backgroundStyle->color.getValue(m_state), m_opacity);
        m_sprite.setOpacity(opacity);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void BackgroundComponent::setComponentState(ComponentState state)
    {
        if (m_state == state)
            return;

        m_state = state;

        m_borderColor = Color::applyOpacity(m_backgroundStyle->borderColor.getValue(m_state), m_opacity);
        m_background.color = Color::applyOpacity(m_backgroundStyle->color.getValue(m_state), m_opacity);
        m_sprite.setTexture(m_backgroundStyle->texture.getValue(m_state));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool BackgroundComponent::isTransparentPixel(Vector2f pos, bool transparentTexture) const
    {
        if (transparentTexture && m_sprite.isSet()) /// TODO: transparentTexture should be option on Texture (similar to Smooth) instead of a parameter
        {
            if ((pos.x < m_borders.getLeft()) || (pos.y < m_borders.getTop()) || (pos.x >= m_borders.getLeft() + m_clientSize.x) || (pos.y >= m_borders.getTop() + m_clientSize.y))
                return false;
            else
                return m_sprite.isTransparentPixel(pos - m_borders.getOffset());
        }
        else /// TODO: Perform check when using rounded corners
            return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void BackgroundComponent::draw(BackendRenderTarget& target, RenderStates states) const
    {
        states.transform.translate(m_position);

        if ((m_backgroundStyle->roundedBorderRadius > 0) && !m_sprite.isSet())
        {
            target.drawRoundedRectangle(states, m_size, m_background.color, m_backgroundStyle->roundedBorderRadius, m_borders, m_borderColor);
            states.transform.translate(m_background.rect.getPosition());
        }
        else
        {
            // Draw the borders
            if (m_borders != Borders{0})
                target.drawBorders(states, m_borders, m_size, m_borderColor);

            states.transform.translate(m_background.rect.getPosition());

            // Draw either a texture or a color
            if (m_sprite.isSet())
                target.drawSprite(states, m_sprite);
            else
                target.drawFilledRect(states, m_background.rect.getSize(), m_background.color);
        }

        GroupComponent::draw(target, states);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void BackgroundComponent::updateLayout()
    {
        m_padding.updateParentSize(m_size);
        m_borders.updateParentSize(m_size);

        const Vector2f size = getSizeWithoutBorders();
        m_background.rect.setPosition({m_borders.getLeft(), m_borders.getTop()});
        m_background.rect.setSize(size);
        m_sprite.setSize(size);

        m_clientSize = {std::max(0.f, m_size.x - m_borders.getLeft() - m_borders.getRight() - m_padding.getLeft() - m_padding.getRight()),
                        std::max(0.f, m_size.y - m_borders.getTop() - m_borders.getBottom() - m_padding.getTop() - m_padding.getBottom())};
        GroupComponent::updateLayout();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Vector2f BackgroundComponent::getSizeWithoutBorders() const
    {
        return {std::max(0.f, m_size.x - m_borders.getLeft() - m_borders.getRight()),
                std::max(0.f, m_size.y - m_borders.getTop() - m_borders.getBottom())};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::shared_ptr<Component> BackgroundComponent::clone() const
    {
        return std::make_shared<BackgroundComponent>(*this);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    TextComponent::TextComponent(StylePropertyText* textStyle) :
        m_textStyle{textStyle}
    {
        init();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    TextComponent::~TextComponent()
    {
        m_textStyle->color.disconnectCallback(m_colorCallbackId);
        m_textStyle->style.disconnectCallback(m_styleCallbackId);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    TextComponent::TextComponent(const TextComponent& other, StylePropertyText* textStyle) :
        Component(other),
        m_text{other.m_text},
        m_textStyle{textStyle ? textStyle : other.m_textStyle}
    {
        init();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    TextComponent& TextComponent::operator=(const TextComponent& other)
    {
        if (&other != this)
        {
            Component::operator=(other);
            m_text = other.m_text;

            m_textStyle->color.disconnectCallback(m_colorCallbackId);
            m_textStyle->style.disconnectCallback(m_styleCallbackId);

            init();
        }

        return *this;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TextComponent::init()
    {
        m_colorCallbackId = m_textStyle->color.connectCallback([this]{
            m_color = m_textStyle->color.getValue(m_state);
            m_text.setColor(m_color);
        });
        m_styleCallbackId = m_textStyle->style.connectCallback([this]{
            m_style = m_textStyle->style.getValue(m_state);
            m_text.setStyle(m_style);
            updateLayout();
        });

        m_color = m_textStyle->color.getValue(m_state);
        m_style = m_textStyle->style.getValue(m_state);

        m_text.setColor(m_color);
        m_text.setStyle(m_style);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TextComponent::setString(const String& caption)
    {
        m_text.setString(caption);
        updateLayout();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const String& TextComponent::getString() const
    {
        return m_text.getString();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TextComponent::setCharacterSize(unsigned int size)
    {
        m_text.setCharacterSize(size);
        updateLayout();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int TextComponent::getCharacterSize() const
    {
        return m_text.getCharacterSize();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TextComponent::setFont(const Font& font)
    {
        m_text.setFont(font);
        updateLayout();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Font TextComponent::getFont() const
    {
        return m_text.getFont();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TextComponent::setOutlineColor(Color color)
    {
        m_text.setOutlineColor(color);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Color TextComponent::getOutlineColor() const
    {
        return m_text.getOutlineColor();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TextComponent::setOutlineThickness(float thickness)
    {
        m_text.setOutlineThickness(thickness);
        updateLayout();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    float TextComponent::getOutlineThickness() const
    {
        return m_text.getOutlineThickness();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    float TextComponent::getLineHeight() const
    {
        return m_text.getLineHeight();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TextComponent::setOpacity(float opacity)
    {
        m_opacity = opacity;
        m_text.setOpacity(opacity);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TextComponent::updateLayout()
    {
        m_size = m_text.getSize();
        Component::updateLayout();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TextComponent::setComponentState(ComponentState state)
    {
        if (m_state == state)
            return;

        const auto oldTextStyle = m_style;

        m_state = state;

        m_color = m_textStyle->color.getValue(m_state);
        m_style = m_textStyle->style.getValue(m_state);

        m_text.setColor(m_color);
        m_text.setStyle(m_style);

        if (m_style != oldTextStyle)
            updateLayout();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TextComponent::draw(BackendRenderTarget& target, RenderStates states) const
    {
        if (m_text.getString().empty())
            return;

        states.transform.translate(m_position);
        target.drawText(states, m_text);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::shared_ptr<Component> TextComponent::clone() const
    {
        return std::make_shared<TextComponent>(*this);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ImageComponent::ImageComponent(StyleProperty<Texture>* textureStyle) :
        m_textureStyle{textureStyle}
    {
        init();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ImageComponent::~ImageComponent()
    {
        m_textureStyle->disconnectCallback(m_textureCallbackId);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ImageComponent::ImageComponent(const ImageComponent& other, StyleProperty<Texture>* textureStyle) :
        Component(other),
        m_textureStyle{textureStyle ? textureStyle : other.m_textureStyle},
        m_sprite{other.m_sprite}
    {
        init();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ImageComponent& ImageComponent::operator=(const ImageComponent& other)
    {
        if (&other != this)
        {
            Component::operator=(other);
            m_sprite = other.m_sprite;

            m_textureStyle->disconnectCallback(m_textureCallbackId);

            m_textureStyle = other.m_textureStyle;
            init();
        }

        return *this;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ImageComponent::init()
    {
        m_textureCallbackId = m_textureStyle->connectCallback([this]{
            m_sprite.setTexture(m_textureStyle->getValue(m_state));
        });

        m_sprite.setTexture(m_textureStyle->getValue(m_state));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ImageComponent::setSize(Vector2f size)
    {
        m_size = size;
        m_sprite.setSize(size);
        updateLayout();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ImageComponent::setOpacity(float opacity)
    {
        m_sprite.setOpacity(opacity);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ImageComponent::setComponentState(ComponentState state)
    {
        if (m_state == state)
            return;

        m_state = state;
        m_sprite.setTexture(m_textureStyle->getValue(m_state));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ImageComponent::isTransparentPixel(Vector2f pos, bool transparentTexture) const
    {
        if (transparentTexture && m_sprite.isSet()) /// TODO: transparentTexture should be option on Texture (similar to Smooth) instead of a parameter
            return m_sprite.isTransparentPixel(pos);
        else
            return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ImageComponent::draw(BackendRenderTarget& target, RenderStates states) const
    {
        states.transform.translate(m_position);
        target.drawSprite(states, m_sprite);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::shared_ptr<Component> ImageComponent::clone() const
    {
        return std::make_shared<ImageComponent>(*this);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

} // namespace dev
} // namespace priv
} // namespace tgui

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
