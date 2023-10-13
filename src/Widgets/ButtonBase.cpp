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


#include <TGUI/Widgets/ButtonBase.hpp>

#if !TGUI_EXPERIMENTAL_USE_STD_MODULE
    #include <cmath>
#endif

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ButtonBase::ButtonBase(const char* typeName, bool initRenderer) :
        ClickableWidget{typeName, false},
        m_textPosition{RelativeValue(0.5f), RelativeValue(0.5f)},
        m_textOrigin{0.5f, 0.5f},
        m_backgroundComponent{std::make_shared<priv::dev::BackgroundComponent>(&background)},
        m_textComponent{std::make_shared<priv::dev::TextComponent>(&text)}
    {
        ButtonBase::initComponents();

        if (initRenderer)
        {
            m_renderer = aurora::makeCopied<ButtonRenderer>();
            setRenderer(Theme::getDefault()->getRendererNoThrow(m_type));

            m_textComponent->setFont(m_fontCached);
            setTextSize(getGlobalTextSize());
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ButtonBase::ButtonBase(const ButtonBase& other) :
        ClickableWidget               {other},
        m_string                      {other.m_string},
        m_down                        {other.m_down},
        m_state                       {other.m_state},
        m_autoSize                    {other.m_autoSize},
        m_updatingTextSize            {false},
        m_textPosition                {other.m_textPosition},
        m_textOrigin                  {other.m_textOrigin},
        background                    {other.background},
        text                          {other.text},
        m_stylePropertiesNames        {},
        m_stylePropertiesGlobalNames  {},
        m_namedComponents             {},
        m_backgroundComponent         {std::make_shared<priv::dev::BackgroundComponent>(*other.m_backgroundComponent, &background)},
        m_textComponent               {std::make_shared<priv::dev::TextComponent>(*other.m_textComponent, &text)},
        m_components                  {}
    {
        ButtonBase::initComponents();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ButtonBase::ButtonBase(ButtonBase&& other) noexcept :
        ClickableWidget               {std::move(other)},
        m_string                      {std::move(other.m_string)},
        m_down                        {std::move(other.m_down)},
        m_state                       {std::move(other.m_state)},
        m_autoSize                    {std::move(other.m_autoSize)},
        m_updatingTextSize            {false},
        m_textPosition                {std::move(other.m_textPosition)},
        m_textOrigin                  {std::move(other.m_textOrigin)},
        background                    {std::move(other.background)},
        text                          {std::move(other.text)},
        m_stylePropertiesNames        {},
        m_stylePropertiesGlobalNames  {},
        m_namedComponents             {},
        m_backgroundComponent         {std::make_shared<priv::dev::BackgroundComponent>(*other.m_backgroundComponent, &background)},
        m_textComponent               {std::make_shared<priv::dev::TextComponent>(*other.m_textComponent, &text)},
        m_components                  {}
    {
        ButtonBase::initComponents();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ButtonBase& ButtonBase::operator=(const ButtonBase& other)
    {
        if (&other != this)
        {
            text.style.disconnectCallback(m_textStyleChangedCallbackId);

            ClickableWidget::operator=(other);
            m_string                       = other.m_string;
            m_down                         = other.m_down;
            m_state                        = other.m_state;
            m_autoSize                     = other.m_autoSize;
            m_updatingTextSize             = false;
            m_textPosition                 = other.m_textPosition;
            m_textOrigin                   = other.m_textOrigin;
            background                     = other.background;
            text                           = other.text;
            m_stylePropertiesNames         = {};
            m_stylePropertiesGlobalNames   = {};
            m_namedComponents              = {};
            m_backgroundComponent          = std::make_shared<priv::dev::BackgroundComponent>(*other.m_backgroundComponent, &background);
            m_textComponent                = std::make_shared<priv::dev::TextComponent>(*other.m_textComponent, &text);
            m_components                   = {};

            ButtonBase::initComponents();
        }

        return *this;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ButtonBase& ButtonBase::operator=(ButtonBase&& other) noexcept
    {
        if (&other != this)
        {
            text.style.disconnectCallback(m_textStyleChangedCallbackId);

            ClickableWidget::operator=(other);
            m_string                       = std::move(other.m_string);
            m_down                         = std::move(other.m_down);
            m_state                        = std::move(other.m_state);
            m_autoSize                     = std::move(other.m_autoSize);
            m_updatingTextSize             = false;
            m_textPosition                 = std::move(other.m_textPosition);
            m_textOrigin                   = std::move(other.m_textOrigin);
            background                     = std::move(other.background);
            text                           = std::move(other.text);
            m_stylePropertiesNames         = {};
            m_stylePropertiesGlobalNames   = {};
            m_namedComponents              = {};
            m_backgroundComponent          = std::make_shared<priv::dev::BackgroundComponent>(*other.m_backgroundComponent, &background);
            m_textComponent                = std::make_shared<priv::dev::TextComponent>(*other.m_textComponent, &text);
            m_components                   = {};

            ButtonBase::initComponents();
        }

        return *this;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ButtonBase::~ButtonBase()
    {
        text.style.disconnectCallback(m_textStyleChangedCallbackId);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ButtonRenderer* ButtonBase::getSharedRenderer()
    {
        return aurora::downcast<ButtonRenderer*>(Widget::getSharedRenderer());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const ButtonRenderer* ButtonBase::getSharedRenderer() const
    {
        return aurora::downcast<const ButtonRenderer*>(Widget::getSharedRenderer());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ButtonRenderer* ButtonBase::getRenderer()
    {
        return aurora::downcast<ButtonRenderer*>(Widget::getRenderer());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ButtonBase::setSize(const Layout2d& size)
    {
        ClickableWidget::setSize(size);

        m_autoSize = false;
        updateSize();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ButtonBase::setEnabled(bool enabled)
    {
        ClickableWidget::setEnabled(enabled);
        updateState();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ButtonBase::setText(const String& caption)
    {
        m_string = caption;
        m_textComponent->setString(caption);

        updateTextSize();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const String& ButtonBase::getText() const
    {
        return m_string;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ButtonBase::setTextPosition(Vector2<AbsoluteOrRelativeValue> position, Vector2f origin)
    {
        m_textPosition = position;
        m_textOrigin = origin;
        updateTextPosition();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ButtonBase::updateTextSize()
    {
        if ((m_textSize == 0) && !getSharedRenderer()->getTextSize())
        {
            // Auto-size the text
            const Vector2f innerSize = m_backgroundComponent->getClientSize();
            const unsigned int textSize = Text::findBestTextSize(m_fontCached, innerSize.y * 0.8f);

            // Make the text smaller when it's too width
            if (m_textComponent->getSize().x > (innerSize.x * 0.85f))
                m_textSizeCached = static_cast<unsigned int>(textSize * ((innerSize.x * 0.85f) / m_textComponent->getSize().x));
            else
                m_textSizeCached = textSize;
        }

        m_textComponent->setCharacterSize(m_textSizeCached);
        m_updatingTextSize = true;
        updateSize();
        m_updatingTextSize = false;

        updateTextPosition();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ButtonBase::setFocused(bool focused)
    {
        ClickableWidget::setFocused(focused);
        updateState();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ButtonBase::isMouseOnWidget(Vector2f pos) const
    {
        if (ClickableWidget::isMouseOnWidget(pos))
        {
            if (!m_backgroundComponent->isTransparentPixel(pos - getPosition(), m_transparentTextureCached))
                return true;
        }

        return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ButtonBase::leftMousePressed(Vector2f pos)
    {
        ClickableWidget::leftMousePressed(pos);
        updateState();
        return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ButtonBase::leftMouseReleased(Vector2f pos)
    {
        ClickableWidget::leftMouseReleased(pos);
        updateState();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ButtonBase::mouseEnteredWidget()
    {
        ClickableWidget::mouseEnteredWidget();
        updateState();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ButtonBase::mouseLeftWidget()
    {
        ClickableWidget::mouseLeftWidget();
        updateState();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ButtonBase::leftMouseButtonNoLongerDown()
    {
        ClickableWidget::leftMouseButtonNoLongerDown();
        updateState();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ButtonBase::rendererChanged(const String& property)
    {
        if (property == U"Borders")
        {
            background.borders = getSharedRenderer()->getBorders();
            updateSize();
        }
        else if (property == U"RoundedBorderRadius")
        {
            background.roundedBorderRadius = getSharedRenderer()->getRoundedBorderRadius();
        }
        else if (property == U"TextColor")
            priv::dev::setOptionalPropertyValue(text.color, getSharedRenderer()->getTextColor(), priv::dev::ComponentState::Normal);
        else if (property == U"TextColorDown")
            priv::dev::setOptionalPropertyValue(text.color, getSharedRenderer()->getTextColorDown(), priv::dev::ComponentState::Active);
        else if (property == U"TextColorHover")
            priv::dev::setOptionalPropertyValue(text.color, getSharedRenderer()->getTextColorHover(), priv::dev::ComponentState::Hover);
        else if (property == U"TextColorDownHover")
            priv::dev::setOptionalPropertyValue(text.color, getSharedRenderer()->getTextColorDownHover(), priv::dev::ComponentState::ActiveHover);
        else if (property == U"TextColorDisabled")
            priv::dev::setOptionalPropertyValue(text.color, getSharedRenderer()->getTextColorDisabled(), priv::dev::ComponentState::Disabled);
        else if (property == U"TextColorDownDisabled")
            priv::dev::setOptionalPropertyValue(text.color, getSharedRenderer()->getTextColorDownDisabled(), priv::dev::ComponentState::DisabledActive);
        else if (property == U"TextColorFocused")
            priv::dev::setOptionalPropertyValue(text.color, getSharedRenderer()->getTextColorFocused(), priv::dev::ComponentState::Focused);
        else if (property == U"TextColorDownFocused")
            priv::dev::setOptionalPropertyValue(text.color, getSharedRenderer()->getTextColorDownFocused(), priv::dev::ComponentState::FocusedActive);
        else if (property == U"TextStyle")
            priv::dev::setOptionalPropertyValue(text.style, getSharedRenderer()->getTextStyle(), priv::dev::ComponentState::Normal);
        else if (property == U"TextStyleDown")
            priv::dev::setOptionalPropertyValue(text.style, getSharedRenderer()->getTextStyleDown(), priv::dev::ComponentState::Active);
        else if (property == U"TextStyleHover")
            priv::dev::setOptionalPropertyValue(text.style, getSharedRenderer()->getTextStyleHover(), priv::dev::ComponentState::Hover);
        else if (property == U"TextStyleDownHover")
            priv::dev::setOptionalPropertyValue(text.style, getSharedRenderer()->getTextStyleDownHover(), priv::dev::ComponentState::ActiveHover);
        else if (property == U"TextStyleDisabled")
            priv::dev::setOptionalPropertyValue(text.style, getSharedRenderer()->getTextStyleDisabled(), priv::dev::ComponentState::Disabled);
        else if (property == U"TextStyleDownDisabled")
            priv::dev::setOptionalPropertyValue(text.style, getSharedRenderer()->getTextStyleDownDisabled(), priv::dev::ComponentState::DisabledActive);
        else if (property == U"TextStyleFocused")
            priv::dev::setOptionalPropertyValue(text.style, getSharedRenderer()->getTextStyleFocused(), priv::dev::ComponentState::Focused);
        else if (property == U"TextStyleDownFocused")
            priv::dev::setOptionalPropertyValue(text.style, getSharedRenderer()->getTextStyleDownFocused(), priv::dev::ComponentState::FocusedActive);
        else if (property == U"Texture")
            priv::dev::setOptionalPropertyValue(background.texture, getSharedRenderer()->getTexture(), priv::dev::ComponentState::Normal);
        else if (property == U"TextureDown")
            priv::dev::setOptionalPropertyValue(background.texture, getSharedRenderer()->getTextureDown(), priv::dev::ComponentState::Active);
        else if (property == U"TextureHover")
            priv::dev::setOptionalPropertyValue(background.texture, getSharedRenderer()->getTextureHover(), priv::dev::ComponentState::Hover);
        else if (property == U"TextureDownHover")
            priv::dev::setOptionalPropertyValue(background.texture, getSharedRenderer()->getTextureDownHover(), priv::dev::ComponentState::ActiveHover);
        else if (property == U"TextureDisabled")
            priv::dev::setOptionalPropertyValue(background.texture, getSharedRenderer()->getTextureDisabled(), priv::dev::ComponentState::Disabled);
        else if (property == U"TextureDownDisabled")
            priv::dev::setOptionalPropertyValue(background.texture, getSharedRenderer()->getTextureDownDisabled(), priv::dev::ComponentState::Disabled);
        else if (property == U"TextureFocused")
            priv::dev::setOptionalPropertyValue(background.texture, getSharedRenderer()->getTextureFocused(), priv::dev::ComponentState::Focused);
        else if (property == U"TextureDownFocused")
            priv::dev::setOptionalPropertyValue(background.texture, getSharedRenderer()->getTextureDownFocused(), priv::dev::ComponentState::FocusedActive);
        else if (property == U"BorderColor")
            priv::dev::setOptionalPropertyValue(background.borderColor, getSharedRenderer()->getBorderColor(), priv::dev::ComponentState::Normal);
        else if (property == U"BorderColorDown")
            priv::dev::setOptionalPropertyValue(background.borderColor, getSharedRenderer()->getBorderColorDown(), priv::dev::ComponentState::Active);
        else if (property == U"BorderColorHover")
            priv::dev::setOptionalPropertyValue(background.borderColor, getSharedRenderer()->getBorderColorHover(), priv::dev::ComponentState::Hover);
        else if (property == U"BorderColorDownHover")
            priv::dev::setOptionalPropertyValue(background.borderColor, getSharedRenderer()->getBorderColorDownHover(), priv::dev::ComponentState::ActiveHover);
        else if (property == U"BorderColorDisabled")
            priv::dev::setOptionalPropertyValue(background.borderColor, getSharedRenderer()->getBorderColorDisabled(), priv::dev::ComponentState::Disabled);
        else if (property == U"BorderColorDownDisabled")
            priv::dev::setOptionalPropertyValue(background.borderColor, getSharedRenderer()->getBorderColorDownDisabled(), priv::dev::ComponentState::DisabledActive);
        else if (property == U"BorderColorFocused")
            priv::dev::setOptionalPropertyValue(background.borderColor, getSharedRenderer()->getBorderColorFocused(), priv::dev::ComponentState::Focused);
        else if (property == U"BorderColorDownFocused")
            priv::dev::setOptionalPropertyValue(background.borderColor, getSharedRenderer()->getBorderColorDownFocused(), priv::dev::ComponentState::FocusedActive);
        else if (property == U"BackgroundColor")
            priv::dev::setOptionalPropertyValue(background.color, getSharedRenderer()->getBackgroundColor(), priv::dev::ComponentState::Normal);
        else if (property == U"BackgroundColorDown")
            priv::dev::setOptionalPropertyValue(background.color, getSharedRenderer()->getBackgroundColorDown(), priv::dev::ComponentState::Active);
        else if (property == U"BackgroundColorHover")
            priv::dev::setOptionalPropertyValue(background.color, getSharedRenderer()->getBackgroundColorHover(), priv::dev::ComponentState::Hover);
        else if (property == U"BackgroundColorDownHover")
            priv::dev::setOptionalPropertyValue(background.color, getSharedRenderer()->getBackgroundColorDownHover(), priv::dev::ComponentState::ActiveHover);
        else if (property == U"BackgroundColorDisabled")
            priv::dev::setOptionalPropertyValue(background.color, getSharedRenderer()->getBackgroundColorDisabled(), priv::dev::ComponentState::Disabled);
        else if (property == U"BackgroundColorDownDisabled")
            priv::dev::setOptionalPropertyValue(background.color, getSharedRenderer()->getBackgroundColorDownDisabled(), priv::dev::ComponentState::DisabledActive);
        else if (property == U"BackgroundColorFocused")
            priv::dev::setOptionalPropertyValue(background.color, getSharedRenderer()->getBackgroundColorFocused(), priv::dev::ComponentState::Focused);
        else if (property == U"BackgroundColorDownFocused")
            priv::dev::setOptionalPropertyValue(background.color, getSharedRenderer()->getBackgroundColorDownFocused(), priv::dev::ComponentState::FocusedActive);
        else if (property == U"TextOutlineThickness")
        {
            m_textComponent->setOutlineThickness(getSharedRenderer()->getTextOutlineThickness());
            updateTextPosition();
        }
        else if (property == U"TextOutlineColor")
        {
            m_textComponent->setOutlineColor(getSharedRenderer()->getTextOutlineColor());
        }
        else if ((property == U"Opacity") || (property == U"OpacityDisabled"))
        {
            ClickableWidget::rendererChanged(property);
            m_textComponent->setOpacity(m_opacityCached);
            m_backgroundComponent->setOpacity(m_opacityCached);
        }
        else if (property == U"Font")
        {
            ClickableWidget::rendererChanged(property);

            m_textComponent->setFont(m_fontCached);
            updateTextSize();
        }
        else
            ClickableWidget::rendererChanged(property);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::unique_ptr<DataIO::Node> ButtonBase::save(SavingRenderersMap& renderers) const
    {
        auto node = ClickableWidget::save(renderers);

        if (!m_string.empty())
            node->propertyValuePairs[U"Text"] = std::make_unique<DataIO::ValueNode>(Serializer::serialize(m_string));

        // Don't store size when auto-sizing
        if (m_autoSize)
            node->propertyValuePairs.erase("Size");

        return node;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ButtonBase::load(const std::unique_ptr<DataIO::Node>& node, const LoadingRenderersMap& renderers)
    {
        ClickableWidget::load(node, renderers);

        if (node->propertyValuePairs[U"Text"])
            setText(Deserializer::deserialize(ObjectConverter::Type::String, node->propertyValuePairs[U"Text"]->value).getString());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ButtonBase::updateState()
    {
        const auto newState = priv::dev::getStateFromFlags(m_mouseHover, m_down, m_focused, m_enabled);
        if (m_state == newState)
            return;

        m_state = newState;

        m_backgroundComponent->setComponentState(m_state);
        m_textComponent->setComponentState(m_state);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ButtonBase::updateSize()
    {
        if (m_autoSize)
        {
            const float spaceAroundText = m_textComponent->getLineHeight();
            const Outline& borders = m_backgroundComponent->getBorders();
            m_autoLayout = AutoLayout::Manual;
            Widget::setSize({m_textComponent->getSize().x + spaceAroundText + borders.getLeft() + borders.getRight(),
                             std::round(m_textComponent->getLineHeight() * 1.25f) + borders.getTop() + borders.getBottom()});
        }

        m_backgroundComponent->setSize(getSize());

        // Recalculate the text size (needed when auto-sizing)
        if (!m_updatingTextSize)
            updateTextSize();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ButtonBase::updateTextPosition()
    {
        const Outline& borders = m_backgroundComponent->getBorders();
        m_textPosition.x.updateParentSize(getSize().x - borders.getLeft() - borders.getRight());
        m_textPosition.y.updateParentSize(getSize().y - borders.getTop() - borders.getBottom());

        m_textComponent->setPosition({m_textPosition.x.getValue() - m_textOrigin.x * m_textComponent->getSize().x,
                                      m_textPosition.y.getValue() - m_textOrigin.y * m_textComponent->getSize().y});
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ButtonBase::initComponents()
    {
        TGUI_ASSERT(m_stylePropertiesNames.empty() && m_stylePropertiesGlobalNames.empty() && m_namedComponents.empty(),
                    "ButtonBase::initComponents can only be called if maps are empty");

        m_stylePropertiesNames.emplace(m_type + U".Background.BorderColor", &background.borderColor);
        m_stylePropertiesNames.emplace(m_type + U".Background.BackgroundColor", &background.color);
        m_stylePropertiesNames.emplace(m_type + U".Background.BackgroundTexture", &background.texture);
        m_stylePropertiesNames.emplace(m_type + U".Background.Borders", &background.borders);
        m_stylePropertiesNames.emplace(m_type + U".Background.Padding", &background.padding);
        m_stylePropertiesNames.emplace(m_type + U".Text.Color", &text.color);
        m_stylePropertiesNames.emplace(m_type + U".Text.Style", &text.style);

        m_stylePropertiesGlobalNames[U"BorderColor"].emplace_back(&background.borderColor);
        m_stylePropertiesGlobalNames[U"BackgroundColor"].emplace_back(&background.color);
        m_stylePropertiesGlobalNames[U"BackgroundTexture"].emplace_back(&background.texture);
        m_stylePropertiesGlobalNames[U"Borders"].emplace_back(&background.borders);
        m_stylePropertiesGlobalNames[U"Padding"].emplace_back(&background.padding);
        m_stylePropertiesGlobalNames[U"TextColor"].emplace_back(&text.color);
        m_stylePropertiesGlobalNames[U"TextStyle"].emplace_back(&text.style);

        m_namedComponents.emplace("Background", m_backgroundComponent);
        m_namedComponents.emplace("Text", m_textComponent);

        m_backgroundComponent->addComponent(m_textComponent);
        addComponent(m_backgroundComponent);

        m_textStyleChangedCallbackId = text.style.connectCallback([this]{
            updateTextPosition();
        });
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ButtonBase::addComponent(const std::shared_ptr<priv::dev::Component>& component)
    {
        m_components.emplace_back(component);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::shared_ptr<priv::dev::Component> ButtonBase::getComponent(const String& name)
    {
        auto it = m_namedComponents.find(name);
        if (it != m_namedComponents.end())
            return it->second;
        else
            return nullptr;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ButtonBase::draw(BackendRenderTarget& target, RenderStates states) const
    {
        for (auto& component : m_components)
            component->draw(target, states);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
