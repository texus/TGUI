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


#ifndef TGUI_COMPONENTS_HPP
#define TGUI_COMPONENTS_HPP

#include <TGUI/Text.hpp>
#include <TGUI/Sprite.hpp>
#include <TGUI/Texture.hpp>
#include <TGUI/Outline.hpp>

#if !TGUI_EXPERIMENTAL_USE_STD_MODULE
    #include <unordered_map>
    #include <memory>
    #include <set>
#endif

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    class BackendRenderTarget;

namespace priv
{
namespace dev
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    /// State of a widget or a component inside it
    enum class ComponentState : std::uint8_t
    {
        Normal = 0,             //!< Default state
        Hover = 1,              //!< Mouse hover
        Active = 2,             //!< Pressed/checked/selected
        ActiveHover = 3,        //!< Pressed/checked/selected with mouse hover
        Focused = 4,            //!< Has focused
        FocusedHover = 5,       //!< Has focused with mouse hover
        FocusedActive = 6,      //!< Has focused while pressed/checked/selected
        FocusedActiveHover = 7, //!< Has focused while pressed/checked/selected with mouse hover
        Disabled = 8,           //!< Is disabled
        DisabledActive = 10     //!< Is disabled while pressed/checked/selected
    };

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    /// Layout alignment for automatically setting the position and (part of) the size of a component
    enum class AlignLayout
    {
        None,       //!< Position and size need to be manually set. This is the default.
        Top,        //!< Places the component on on the top and sets its width to the area between Leftmost and Rightmost aligned components. Height needs to be manually set.
        Left,       //!< Places the component on the left side and sets its height to the area between Top and Bottom aligned components. Width needs to be manually set.
        Right,      //!< Places the component on the right side and sets its height to the area between Top and Bottom aligned components. Width needs to be manually set.
        Bottom,     //!< Places the component on on the bottom and sets its width to the area between Leftmost and Rightmost aligned components. Height needs to be manually set.
        Leftmost,   //!< Places the component on the left side and sets height to 100%. Width needs to be manually set. Same as Left alignment if no component uses Top or Bottom alignment.
        Rightmost,  //!< Places the component on the right side and sets height to 100%. Width needs to be manually set. Same as Left alignment if no component uses Top or Bottom alignment.
        Fill        //!< Sets the position to (0,0) and size to (100%,100%).
    };

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    /// Position alignment of a component within its parent
    enum class PositionAlignment
    {
        None,        //!< Place the component at the manually set position. This is the default.
        TopLeft,     //!< Place the component in the upper left corner of its parent
        Top,         //!< Place the component at the upper side of its parent (horizontally centered)
        TopRight,    //!< Place the component in the upper right corner of its parent
        Right,       //!< Place the component at the right side of its parent (vertically centered)
        BottomRight, //!< Place the component in the bottom right corner of its parent
        Bottom,      //!< Place the component at the bottom of its parent (horizontally centered)
        BottomLeft,  //!< Place the component in the bottom left corner of its parent
        Left,        //!< Place the component at the left side of its parent (vertically centered)
        Center       //!< Center the component in its parent
    };

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    class TGUI_API MessageBroker
    {
    public:

        TGUI_NODISCARD static std::uint64_t createTopic();

        static void destroyTopic(std::uint64_t topicId);

        TGUI_NODISCARD static std::uint64_t subscribe(std::uint64_t topicId, std::function<void()> func);

        static void unsubscribe(std::uint64_t callbackId);

        static void sendEvent(std::uint64_t topicId);

    private:
        static std::unordered_map<std::uint64_t, std::set<std::uint64_t>> m_topicIdToCallbackIds;
        static std::unordered_map<std::uint64_t, std::uint64_t> m_callbackIdToTopicId;
        static std::unordered_map<std::uint64_t, std::function<void()>> m_listeners; // CallbackId -> Func

        // All topic and callback ids are unique and non-overlapping
        static std::uint64_t m_lastId;
    };

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    class TGUI_API StylePropertyBase
    {
    public:
        virtual ~StylePropertyBase() = default;
    };

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    template <typename ValueType>
    class TGUI_API StyleProperty : public StylePropertyBase
    {
    public:

        StyleProperty() :
            m_defaultValue  {},
            m_messageTopicId{MessageBroker::createTopic()}
        {
        }

        explicit StyleProperty(ValueType defaultValue) :
            m_defaultValue  {std::move(defaultValue)},
            m_messageTopicId{MessageBroker::createTopic()}
        {
        }

        StyleProperty(const StyleProperty& other) :
            m_defaultValue  {other.m_defaultValue},
            m_messageTopicId{MessageBroker::createTopic()},
            m_globalValues  {other.m_globalValues}
        {
            unsetValue();

            const std::uint64_t baseIndex = m_propertyData & 0xFFFFFFFFFFFF0000;
            const std::uint64_t oldBaseIndex = other.m_propertyData & 0xFFFFFFFFFFFF0000;
            const std::uint16_t oldStoredStates = static_cast<std::uint16_t>(other.m_propertyData & 0xFFFF);

            std::uint16_t total = 0;
            std::uint8_t bitIndex = 0;
            while (total < oldStoredStates)
            {
                if (oldStoredStates & (1 << bitIndex))
                {
                    m_globalValues[baseIndex + bitIndex] = m_globalValues[oldBaseIndex + bitIndex];
                    total += static_cast<std::uint16_t>(1 << bitIndex);
                }
                ++bitIndex;
            }

            m_propertyData = baseIndex | oldStoredStates;
        }

        StyleProperty(StyleProperty&& other) noexcept :
            m_defaultValue  {std::move(other.m_defaultValue)},
            m_propertyData  {std::move(other.m_propertyData)},
            m_messageTopicId{std::move(other.m_messageTopicId)},
            m_globalValues  {std::move(other.m_globalValues)}
        {
            other.m_messageTopicId = 0;
        }

        ~StyleProperty() override
        {
            if (m_messageTopicId) // Can be 0 on moved object
                MessageBroker::destroyTopic(m_messageTopicId);
            unsetValueImpl();
        }

        StyleProperty& operator=(const StyleProperty& other)
        {
            if (&other != this)
            {
                StyleProperty temp(other);
                std::swap(m_defaultValue,   temp.m_defaultValue);
                std::swap(m_propertyData,   temp.m_propertyData);
                std::swap(m_messageTopicId, temp.m_messageTopicId);
                std::swap(m_globalValues,   temp.m_globalValues);
            }

            return *this;
        }

        StyleProperty& operator=(StyleProperty&& other) noexcept
        {
            if (&other != this)
            {
                m_defaultValue = std::move(other.m_defaultValue);
                m_propertyData = std::move(other.m_propertyData);
                m_messageTopicId = std::move(other.m_messageTopicId);
                m_globalValues = std::move(other.m_globalValues);

                other.m_messageTopicId = 0;
            }

            return *this;
        }

        StyleProperty& operator=(const ValueType& value)
        {
            unsetValueImpl();
            setValue(value, ComponentState::Normal);
            return *this;
        }

        void setValue(const ValueType& value, ComponentState state = ComponentState::Normal)
        {
            const std::uint64_t baseIndex = m_propertyData & 0xFFFFFFFFFFFF0000;
            m_propertyData |= static_cast<std::uint64_t>(1) << static_cast<std::uint8_t>(state);
            m_globalValues[baseIndex + static_cast<std::uint8_t>(state)] = value;

            MessageBroker::sendEvent(m_messageTopicId);
        }

        void unsetValue(ComponentState state)
        {
            const std::uint64_t baseIndex = m_propertyData & 0xFFFFFFFFFFFF0000;
            m_propertyData &= ~(static_cast<std::uint64_t>(1) << static_cast<std::uint8_t>(state));
            m_globalValues.erase(baseIndex + static_cast<std::uint8_t>(state));

            MessageBroker::sendEvent(m_messageTopicId);
        }

        void unsetValue()
        {
            unsetValueImpl();
            MessageBroker::sendEvent(m_messageTopicId);
        }

        TGUI_NODISCARD const ValueType& getValue(ComponentState state = ComponentState::Normal) const
        {
            const std::uint64_t baseIndex = m_propertyData & 0xFFFFFFFFFFFF0000;
            const std::uint16_t storedStates = static_cast<std::uint16_t>(m_propertyData & 0xFFFF);

            // If we don't have a value for any state then we can just return the default value
            if (storedStates == 0)
                return m_defaultValue;

            // If we only have a value for the Normal state then always use this value
            if (storedStates == 1)
                return m_globalValues.at(baseIndex);

            if (static_cast<std::uint8_t>(state) & static_cast<std::uint8_t>(ComponentState::Disabled))
            {
                if ((static_cast<std::uint8_t>(state) & static_cast<std::uint8_t>(ComponentState::Active)) && (storedStates & (1 << static_cast<std::uint8_t>(ComponentState::DisabledActive))))
                    return m_globalValues.at(baseIndex + static_cast<std::uint8_t>(ComponentState::DisabledActive));
                if (storedStates & (1 << static_cast<std::uint8_t>(ComponentState::Disabled)))
                    return m_globalValues.at(baseIndex + static_cast<std::uint8_t>(ComponentState::Disabled));
            }

            if (static_cast<std::uint8_t>(state) & static_cast<std::uint8_t>(ComponentState::Active))
            {
                if (static_cast<std::uint8_t>(state) & static_cast<std::uint8_t>(ComponentState::Hover))
                {
                    if ((static_cast<std::uint8_t>(state) & static_cast<std::uint8_t>(ComponentState::Focused)) && (storedStates & (1 << static_cast<std::uint8_t>(ComponentState::FocusedActiveHover))))
                        return m_globalValues.at(baseIndex + static_cast<std::uint8_t>(ComponentState::FocusedActiveHover));
                    if (storedStates & (1 << static_cast<std::uint8_t>(ComponentState::ActiveHover)))
                        return m_globalValues.at(baseIndex + static_cast<std::uint8_t>(ComponentState::ActiveHover));
                }

                if ((static_cast<std::uint8_t>(state) & static_cast<std::uint8_t>(ComponentState::Focused)) && (storedStates & (1 << static_cast<std::uint8_t>(ComponentState::FocusedActive))))
                    return m_globalValues.at(baseIndex + static_cast<std::uint8_t>(ComponentState::FocusedActive));
                if (storedStates & (1 << static_cast<std::uint8_t>(ComponentState::Active)))
                    return m_globalValues.at(baseIndex + static_cast<std::uint8_t>(ComponentState::Active));
            }

            if (static_cast<std::uint8_t>(state) & static_cast<std::uint8_t>(ComponentState::Hover))
            {
                if ((static_cast<std::uint8_t>(state) & static_cast<std::uint8_t>(ComponentState::Focused)) && (storedStates & (1 << static_cast<std::uint8_t>(ComponentState::FocusedHover))))
                    return m_globalValues.at(baseIndex + static_cast<std::uint8_t>(ComponentState::FocusedHover));
                if (storedStates & (1 << static_cast<std::uint8_t>(ComponentState::Hover)))
                    return m_globalValues.at(baseIndex + static_cast<std::uint8_t>(ComponentState::Hover));
            }

            if (static_cast<std::uint8_t>(state) & static_cast<std::uint8_t>(ComponentState::Focused))
            {
                if (storedStates & (1 << static_cast<std::uint8_t>(ComponentState::Focused)))
                    return m_globalValues.at(baseIndex + static_cast<std::uint8_t>(ComponentState::Focused));
            }

            if (storedStates & 1)
            {
                // We have a value for the Normal state, so return it. It is possible to pass here while storedStates != 1 when there
                // is e.g. a value for both Normal and Disabled state and the widget is enabled.
                return m_globalValues.at(baseIndex + static_cast<std::uint8_t>(ComponentState::Normal));
            }
            else
            {
                // We don't have any relevant values, so return the default value. It is possible to pass here while storedStates > 0 when
                // there is e.g. only a value for the Disabled state and the widget is enabled.
                return m_defaultValue;
            }
        }

        TGUI_NODISCARD std::uint64_t connectCallback(std::function<void()> func)
        {
            return MessageBroker::subscribe(m_messageTopicId, std::move(func));
        }

        void disconnectCallback(std::uint64_t id)
        {
            return MessageBroker::unsubscribe(id);
        }

    private:

        void unsetValueImpl()
        {
            const std::uint64_t baseIndex = m_propertyData & 0xFFFFFFFFFFFF0000;
            const std::uint16_t storedStates = static_cast<std::uint16_t>(m_propertyData & 0xFFFF);

            std::uint16_t total = 0;
            std::uint8_t bitIndex = 0;
            while (total < storedStates)
            {
                if (storedStates & (1 << bitIndex))
                {
                    m_globalValues.erase(baseIndex + bitIndex);
                    total += static_cast<std::uint16_t>(1 << bitIndex);
                }
                ++bitIndex;
            }

            m_propertyData = baseIndex; // Forget about the states that were stored
        }

    private:

        ValueType m_defaultValue;

        // The highest 48 bits store the index in a static map of values (see below).
        // The next 16 bits are used to indicate the set of states that are present for this property.
        std::uint64_t m_propertyData = 0;

        // Index of the event that we publish to when the property changes.
        std::uint64_t m_messageTopicId = 0;

        // All values are stored in a static map, which can be seen as a very large sparse list.
        // Each instance holds a unique 48-bit id that can be seen as the high bits of the index in the list.
        // The lowest 4 bits of the index contain the widget state. The remaining 12 bits inbetween are always 0.
        ///static std::unordered_map<std::uint64_t, ValueType> m_globalValues;
        ///static std::uint64_t m_nextGlobalValueIndex;

        /// TODO: The system with global values caused way too many implementation and platform-specific problems.
        ///       For now we just store the values in the StyleProperty itself.
        std::unordered_map<std::uint64_t, ValueType> m_globalValues;
    };

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    struct TGUI_API StylePropertyBackground
    {
        StyleProperty<Color> borderColor{Color::Black};
        StyleProperty<Color> color{Color::White};
        StyleProperty<Texture> texture;
        StyleProperty<Outline> borders;
        StyleProperty<Outline> padding;

        float roundedBorderRadius = 0;
    };

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    struct TGUI_API StylePropertyText
    {
        StyleProperty<Color> color{Color::Black};
        StyleProperty<TextStyles> style;
    };

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    class GroupComponent;

    class TGUI_API Component
    {
    public:

        Component() = default;
        virtual ~Component() = default;

        Component(const Component&);
        Component& operator=(const Component&);

        Component(Component&&) = default;
        Component& operator=(Component&&) = default;

        void setPosition(Vector2f position);

        TGUI_NODISCARD Vector2f getPosition() const;

        TGUI_NODISCARD Vector2f getSize() const;

        void setPositionAlignment(PositionAlignment alignment);

        void setVisible(bool visible);

        TGUI_NODISCARD bool isVisible() const;

        void setParent(GroupComponent* parent);

        virtual void draw(BackendRenderTarget& target, RenderStates states) const = 0;

        virtual void updateLayout();

        TGUI_NODISCARD virtual std::shared_ptr<Component> clone() const = 0;

    protected:

        friend void swap(Component& first, Component& second);

    protected:

        ComponentState m_state = ComponentState::Normal;
        PositionAlignment m_positionAlignment = PositionAlignment::None;
        Vector2f m_position;
        Vector2f m_size;
        bool m_visible = true;
        float m_opacity = 1;
        GroupComponent* m_parent = nullptr;
    };

    class TGUI_API GroupComponent : public Component
    {
    public:

        GroupComponent(const GroupComponent&);
        GroupComponent& operator=(const GroupComponent&);

        GroupComponent(GroupComponent&&) = default;
        GroupComponent& operator=(GroupComponent&&) = default;

        TGUI_NODISCARD Vector2f getClientSize() const;

        void addComponent(const std::shared_ptr<Component>& component);

        TGUI_NODISCARD const std::vector<std::shared_ptr<Component>>& getComponents() const;

        void draw(BackendRenderTarget& target, RenderStates states) const override;

        void updateLayout() override;

        TGUI_NODISCARD std::shared_ptr<Component> clone() const override;

        friend void swap(GroupComponent& first, GroupComponent& second);

    protected:

        GroupComponent() = default;

    protected:
        std::vector<std::shared_ptr<Component>> m_children;
        Vector2f m_clientSize;
    };

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    class TGUI_API BackgroundComponent : public GroupComponent
    {
    public:

        BackgroundComponent(StylePropertyBackground* backgroundStyle);

        ~BackgroundComponent() override;

        BackgroundComponent(const BackgroundComponent& other, StylePropertyBackground* backgroundStyle = nullptr);
        BackgroundComponent& operator=(const BackgroundComponent& other);

        void init();

        void setSize(Vector2f size);

        void setBorders(const Outline& border);

        TGUI_NODISCARD const Outline& getBorders() const;

        void setPadding(const Outline& padding);

        TGUI_NODISCARD const Outline& getPadding() const;

        void setOpacity(float opacity);

        void setComponentState(ComponentState state);

        TGUI_NODISCARD bool isTransparentPixel(Vector2f pos, bool transparentTexture) const;

        void draw(BackendRenderTarget& target, RenderStates states) const override;

        TGUI_NODISCARD Vector2f getSizeWithoutBorders() const;

        void updateLayout() override;

        TGUI_NODISCARD std::shared_ptr<Component> clone() const override;

    private:

        struct ColorRect
        {
            Color color;
            FloatRect rect;
        };

        StylePropertyBackground* m_backgroundStyle;

        ColorRect m_background{Color::White, {}};
        Color m_borderColor = Color::Black;
        Sprite m_sprite;
        Outline m_borders;
        Outline m_padding;

        std::uint64_t m_borderColorCallbackId = 0;
        std::uint64_t m_backgroundColorCallbackId = 0;
        std::uint64_t m_textureCallbackId = 0;
        std::uint64_t m_bordersCallbackId = 0;
        std::uint64_t m_paddingCallbackId = 0;
    };

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    class TGUI_API TextComponent : public Component
    {
    public:

        TextComponent(StylePropertyText* textStyle);

        ~TextComponent() override;

        TextComponent(const TextComponent& other, StylePropertyText* textStyle = nullptr);
        TextComponent& operator=(const TextComponent& other);

        void init();

        void setString(const String& caption);

        TGUI_NODISCARD const String& getString() const;

        void setCharacterSize(unsigned int size);

        TGUI_NODISCARD unsigned int getCharacterSize() const;

        void setFont(const Font& font);

        TGUI_NODISCARD Font getFont() const;

        void setOutlineColor(Color color);

        TGUI_NODISCARD Color getOutlineColor() const;

        void setOutlineThickness(float thickness);

        TGUI_NODISCARD float getOutlineThickness() const;

        TGUI_NODISCARD float getLineHeight() const;

        void setOpacity(float opacity);

        void updateLayout() override;

        void setComponentState(ComponentState state);

        void draw(BackendRenderTarget& target, RenderStates states) const override;

        TGUI_NODISCARD std::shared_ptr<Component> clone() const override;

    private:
        Text m_text;
        StylePropertyText* m_textStyle;

        Color m_color = Color::Black;
        TextStyles m_style = TextStyle::Regular;

        std::uint64_t m_colorCallbackId = 0;
        std::uint64_t m_styleCallbackId = 0;
    };

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    class TGUI_API ImageComponent : public Component
    {
    public:

        ImageComponent(StyleProperty<Texture>* textureStyle);

        ~ImageComponent() override;

        ImageComponent(const ImageComponent& other, StyleProperty<Texture>* textureStyle = nullptr);
        ImageComponent& operator=(const ImageComponent& other);

        void init();

        void setSize(Vector2f size);

        void setOpacity(float opacity);

        void setComponentState(ComponentState state);

        TGUI_NODISCARD bool isTransparentPixel(Vector2f pos, bool transparentTexture) const;

        void draw(BackendRenderTarget& target, RenderStates states) const override;

        TGUI_NODISCARD std::shared_ptr<Component> clone() const override;

    private:

        StyleProperty<Texture>* m_textureStyle;
        Sprite m_sprite;

        std::uint64_t m_textureCallbackId = 0;
    };

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    TGUI_NODISCARD inline ComponentState getStateFromFlags(bool hover, bool active, bool focused = false, bool enabled = true)
    {
        if (!enabled)
        {
            if (active)
                return ComponentState::DisabledActive;
            else
                return ComponentState::Disabled;
        }
        else if (focused)
        {
            if (active)
            {
                if (hover)
                    return ComponentState::FocusedActiveHover;
                else
                    return ComponentState::FocusedActive;
            }
            else if (hover)
                return ComponentState::FocusedHover;
            else
                return ComponentState::Focused;
        }
        else if (active)
        {
            if (hover)
                return ComponentState::ActiveHover;
            else
                return ComponentState::Active;
        }
        else if (hover)
            return ComponentState::Hover;
        else
            return ComponentState::Normal;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    inline void setOptionalPropertyValue(StyleProperty<Color>& property, const Color& color, ComponentState state)
    {
        if (color.isSet())
            property.setValue(color, state);
        else
            property.unsetValue(state);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    inline void setOptionalPropertyValue(StyleProperty<TextStyles>& property, const TextStyles& style, ComponentState state)
    {
        if (style.isSet())
            property.setValue(style, state);
        else
            property.unsetValue(state);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    inline void setOptionalPropertyValue(StyleProperty<Texture>& property, const Texture& texture, ComponentState state)
    {
        if (texture.getData())
            property.setValue(texture, state);
        else
            property.unsetValue(state);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

} // namespace dev
} // namespace priv
} // namespace tgui

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif // TGUI_COMPONENTS_HPP
