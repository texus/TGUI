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


#include <TGUI/Widgets/ProgressBar.hpp>

#if !TGUI_EXPERIMENTAL_USE_STD_MODULE
    #include <cmath>
#endif

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
#if TGUI_COMPILED_WITH_CPP_VER < 17
    constexpr const char ProgressBar::StaticWidgetType[];
#endif

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ProgressBar::ProgressBar(const char* typeName, bool initRenderer) :
        ClickableWidget{typeName, false}
    {
        m_textBack.setFont(m_fontCached);
        m_textFront.setFont(m_fontCached);

        if (initRenderer)
        {
            m_renderer = aurora::makeCopied<ProgressBarRenderer>();
            setRenderer(Theme::getDefault()->getRendererNoThrow(m_type));

            setTextSize(getGlobalTextSize());
            setSize({m_textBack.getLineHeight() * 15,
                     std::round(m_textBack.getLineHeight() * 1.25f) + m_bordersCached.getTop() + m_bordersCached.getBottom()});
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ProgressBar::Ptr ProgressBar::create()
    {
        return std::make_shared<ProgressBar>();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ProgressBar::Ptr ProgressBar::copy(const ProgressBar::ConstPtr& progressBar)
    {
        if (progressBar)
            return std::static_pointer_cast<ProgressBar>(progressBar->clone());
        else
            return nullptr;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ProgressBarRenderer* ProgressBar::getSharedRenderer()
    {
        return aurora::downcast<ProgressBarRenderer*>(Widget::getSharedRenderer());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const ProgressBarRenderer* ProgressBar::getSharedRenderer() const
    {
        return aurora::downcast<const ProgressBarRenderer*>(Widget::getSharedRenderer());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ProgressBarRenderer* ProgressBar::getRenderer()
    {
        return aurora::downcast<ProgressBarRenderer*>(Widget::getRenderer());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ProgressBar::setSize(const Layout2d& size)
    {
        Widget::setSize(size);

        m_bordersCached.updateParentSize(getSize());

        m_spriteBackground.setSize(getInnerSize());

        // Recalculate the size of the front image
        recalculateFillSize();

        // Recalculate the text size
        updateTextSize();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ProgressBar::setMinimum(unsigned int minimum)
    {
        // Set the new minimum
        m_minimum = minimum;

        // The minimum can never be greater than the maximum
        if (m_minimum > m_maximum)
            m_maximum = m_minimum;

        // When the value is below the minimum then adjust it
        if (m_value < m_minimum)
            m_value = m_minimum;

        // Recalculate the size of the front image (the size of the part that will be drawn)
        recalculateFillSize();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int ProgressBar::getMinimum() const
    {
        return m_minimum;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ProgressBar::setMaximum(unsigned int maximum)
    {
        // Set the new maximum
        m_maximum = maximum;

        // The maximum can never be below the minimum
        if (m_maximum < m_minimum)
            m_minimum = m_maximum;

        // When the value is above the maximum then adjust it
        if (m_value > m_maximum)
            m_value = m_maximum;

        // Recalculate the size of the front image (the size of the part that will be drawn)
        recalculateFillSize();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int ProgressBar::getMaximum() const
    {
        return m_maximum;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ProgressBar::setValue(unsigned int value)
    {
        // When the value is below the minimum or above the maximum then adjust it
        if (value < m_minimum)
            value = m_minimum;
        else if (value > m_maximum)
            value = m_maximum;

        if (m_value != value)
        {
            m_value = value;

            onValueChange.emit(this, m_value);

            if (m_value == m_maximum)
                onFull.emit(this);

            // Recalculate the size of the front image (the size of the part that will be drawn)
            recalculateFillSize();
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int ProgressBar::getValue() const
    {
        return m_value;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int ProgressBar::incrementValue()
    {
        // When the value is still below the maximum then adjust it
        if (m_value < m_maximum)
            setValue(m_value + 1);

        // return the new value
        return m_value;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ProgressBar::setText(const String& text)
    {
        m_textBack.setString(text);
        m_textFront.setString(text);

        updateTextSize();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const String& ProgressBar::getText() const
    {
        return m_textBack.getString();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ProgressBar::setFillDirection(FillDirection direction)
    {
        m_fillDirection = direction;
        recalculateFillSize();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ProgressBar::FillDirection ProgressBar::getFillDirection() const
    {
        return m_fillDirection;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ProgressBar::isMouseOnWidget(Vector2f pos) const
    {
        if (ClickableWidget::isMouseOnWidget(pos))
        {
            if (!m_transparentTextureCached || !m_spriteBackground.isSet() || !m_spriteBackground.isTransparentPixel(pos - getPosition() - m_bordersCached.getOffset()))
                return true;
        }

        return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Signal& ProgressBar::getSignal(String signalName)
    {
        if (signalName == onValueChange.getName())
            return onValueChange;
        else if (signalName == onFull.getName())
            return onFull;
        else
            return ClickableWidget::getSignal(std::move(signalName));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ProgressBar::rendererChanged(const String& property)
    {
        if (property == U"Borders")
        {
            m_bordersCached = getSharedRenderer()->getBorders();
            setSize(m_size);
        }
        else if ((property == U"TextColor") || (property == U"TextColorFilled"))
        {
            m_textBack.setColor(getSharedRenderer()->getTextColor());

            if (getSharedRenderer()->getTextColorFilled().isSet())
                m_textFront.setColor(getSharedRenderer()->getTextColorFilled());
            else
                m_textFront.setColor(getSharedRenderer()->getTextColor());
        }
        else if (property == U"TextureBackground")
        {
            m_spriteBackground.setTexture(getSharedRenderer()->getTextureBackground());
        }
        else if (property == U"TextureFill")
        {
            m_spriteFill.setTexture(getSharedRenderer()->getTextureFill());
            recalculateFillSize();
        }
        else if (property == U"TextStyle")
        {
            m_textBack.setStyle(getSharedRenderer()->getTextStyle());
            m_textFront.setStyle(getSharedRenderer()->getTextStyle());
        }
        else if (property == U"TextOutlineThickness")
        {
            const float outlineThickness = getSharedRenderer()->getTextOutlineThickness();
            m_textBack.setOutlineThickness(outlineThickness);
            m_textFront.setOutlineThickness(outlineThickness);
        }
        else if (property == U"TextOutlineColor")
        {
            const Color& outlineColor = getSharedRenderer()->getTextOutlineColor();
            m_textBack.setOutlineColor(outlineColor);
            m_textFront.setOutlineColor(outlineColor);
        }
        else if (property == U"BorderColor")
        {
            m_borderColorCached = getSharedRenderer()->getBorderColor();
        }
        else if (property == U"BackgroundColor")
        {
            m_backgroundColorCached = getSharedRenderer()->getBackgroundColor();
        }
        else if (property == U"FillColor")
        {
            m_fillColorCached = getSharedRenderer()->getFillColor();
        }
        else if ((property == U"Opacity") || (property == U"OpacityDisabled"))
        {
            Widget::rendererChanged(property);

            m_spriteBackground.setOpacity(m_opacityCached);
            m_spriteFill.setOpacity(m_opacityCached);

            m_textBack.setOpacity(m_opacityCached);
            m_textFront.setOpacity(m_opacityCached);
        }
        else if (property == U"Font")
        {
            Widget::rendererChanged(property);

            m_textBack.setFont(m_fontCached);
            m_textFront.setFont(m_fontCached);
            updateTextSize();
        }
        else
            Widget::rendererChanged(property);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::unique_ptr<DataIO::Node> ProgressBar::save(SavingRenderersMap& renderers) const
    {
        auto node = Widget::save(renderers);

        if (!getText().empty())
            node->propertyValuePairs[U"Text"] = std::make_unique<DataIO::ValueNode>(Serializer::serialize(getText()));

        if (m_fillDirection != ProgressBar::FillDirection::LeftToRight)
        {
            if (m_fillDirection == ProgressBar::FillDirection::RightToLeft)
                node->propertyValuePairs[U"FillDirection"] = std::make_unique<DataIO::ValueNode>("RightToLeft");
            else if (m_fillDirection == ProgressBar::FillDirection::TopToBottom)
                node->propertyValuePairs[U"FillDirection"] = std::make_unique<DataIO::ValueNode>("TopToBottom");
            else if (m_fillDirection == ProgressBar::FillDirection::BottomToTop)
                node->propertyValuePairs[U"FillDirection"] = std::make_unique<DataIO::ValueNode>("BottomToTop");
        }

        node->propertyValuePairs[U"Minimum"] = std::make_unique<DataIO::ValueNode>(String::fromNumber(m_minimum));
        node->propertyValuePairs[U"Maximum"] = std::make_unique<DataIO::ValueNode>(String::fromNumber(m_maximum));
        node->propertyValuePairs[U"Value"] = std::make_unique<DataIO::ValueNode>(String::fromNumber(m_value));

        return node;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ProgressBar::load(const std::unique_ptr<DataIO::Node>& node, const LoadingRenderersMap& renderers)
    {
        Widget::load(node, renderers);

        if (node->propertyValuePairs[U"Minimum"])
            setMinimum(node->propertyValuePairs[U"Minimum"]->value.toUInt());
        if (node->propertyValuePairs[U"Maximum"])
            setMaximum(node->propertyValuePairs[U"Maximum"]->value.toUInt());
        if (node->propertyValuePairs[U"Value"])
            setValue(node->propertyValuePairs[U"Value"]->value.toUInt());
        if (node->propertyValuePairs[U"Text"])
            setText(Deserializer::deserialize(ObjectConverter::Type::String, node->propertyValuePairs[U"Text"]->value).getString());

        if (node->propertyValuePairs[U"FillDirection"])
        {
            String direction = node->propertyValuePairs[U"FillDirection"]->value.trim();
            if (direction == U"LeftToRight")
                setFillDirection(ProgressBar::FillDirection::LeftToRight);
            else if (direction == U"RightToLeft")
                setFillDirection(ProgressBar::FillDirection::RightToLeft);
            else if (direction == U"TopToBottom")
                setFillDirection(ProgressBar::FillDirection::TopToBottom);
            else if (direction == U"BottomToTop")
                setFillDirection(ProgressBar::FillDirection::BottomToTop);
            else
                throw Exception{U"Failed to parse FillDirection property, found unknown value '" + direction + U"'."};
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ProgressBar::updateTextSize()
    {
        // Check if the text is auto sized
        if ((m_textSize == 0) && !getSharedRenderer()->getTextSize())
        {
            if (m_spriteFill.isSet())
                m_textSizeCached = Text::findBestTextSize(m_fontCached, m_spriteFill.getSize().y * 0.8f);
            else
                m_textSizeCached = Text::findBestTextSize(m_fontCached, getInnerSize().y * 0.8f);

            m_textBack.setCharacterSize(m_textSizeCached);

            // Make the text smaller when it's too width
            if (m_textBack.getSize().x > (getInnerSize().x * 0.85f))
                m_textSizeCached = static_cast<unsigned int>(m_textSizeCached * ((getInnerSize().x * 0.85f) / m_textBack.getSize().x));
        }

        m_textBack.setCharacterSize(m_textSizeCached);
        m_textFront.setCharacterSize(m_textSizeCached);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Vector2f ProgressBar::getInnerSize() const
    {
        return {std::max(0.f, getSize().x - m_bordersCached.getLeft() - m_bordersCached.getRight()),
                std::max(0.f, getSize().y - m_bordersCached.getTop() - m_bordersCached.getBottom())};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ProgressBar::recalculateFillSize()
    {
        Vector2f size;
        if (m_spriteFill.isSet())
        {
            const Vector2f frontSize = getFrontImageSize();
            m_spriteFill.setSize(frontSize);
            size = frontSize;
        }
        else
            size = getInnerSize();

        switch (getFillDirection())
        {
            case FillDirection::LeftToRight:
                m_frontRect =  {0, 0, size.x * ((m_value - m_minimum) / static_cast<float>(m_maximum - m_minimum)), size.y};
                m_backRect = {m_frontRect.width, 0, size.x - m_frontRect.width, size.y};
                break;

            case FillDirection::RightToLeft:
                m_frontRect =  {0, 0, size.x * ((m_value - m_minimum) / static_cast<float>(m_maximum - m_minimum)), size.y};
                m_frontRect.left = size.x - m_frontRect.width;
                m_backRect = {0, 0, size.x - m_frontRect.width, size.y};
                break;

            case FillDirection::TopToBottom:
                m_frontRect =  {0, 0, size.x, size.y * ((m_value - m_minimum) / static_cast<float>(m_maximum - m_minimum))};
                m_backRect = {0, m_frontRect.height, size.x, size.y - m_frontRect.height};
                break;

            case FillDirection::BottomToTop:
                m_frontRect =  {0, 0, size.x, size.y * ((m_value - m_minimum) / static_cast<float>(m_maximum - m_minimum))};
                m_frontRect.top = size.y - m_frontRect.height;
                m_backRect = {0, 0, size.x, size.y - m_frontRect.height};
                break;
        }

        if (m_spriteFill.isSet())
            m_spriteFill.setVisibleRect(m_frontRect);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Vector2f ProgressBar::getFrontImageSize() const
    {
        if (m_spriteBackground.isSet())
        {
            switch (m_spriteBackground.getScalingType())
            {
            case Sprite::ScalingType::Normal:
                return {m_spriteFill.getTexture().getImageSize().x * getInnerSize().x / m_spriteBackground.getTexture().getImageSize().x,
                        m_spriteFill.getTexture().getImageSize().y * getInnerSize().y / m_spriteBackground.getTexture().getImageSize().y};

            case Sprite::ScalingType::Horizontal:
                return {getInnerSize().x - ((m_spriteBackground.getTexture().getImageSize().x - m_spriteFill.getTexture().getImageSize().x) * (getInnerSize().y / m_spriteBackground.getTexture().getImageSize().y)),
                        m_spriteFill.getTexture().getImageSize().y * getInnerSize().y / m_spriteBackground.getTexture().getImageSize().y};

            case Sprite::ScalingType::Vertical:
                return {m_spriteFill.getTexture().getImageSize().x * getInnerSize().x / m_spriteBackground.getTexture().getImageSize().x,
                        getInnerSize().y - ((m_spriteBackground.getTexture().getImageSize().y - m_spriteFill.getTexture().getImageSize().y) * (getInnerSize().x / m_spriteBackground.getTexture().getImageSize().x))};

            case Sprite::ScalingType::NineSlice:
                return {getInnerSize().x - (m_spriteBackground.getTexture().getImageSize().x - m_spriteFill.getTexture().getImageSize().x),
                        getInnerSize().y - (m_spriteBackground.getTexture().getImageSize().y - m_spriteFill.getTexture().getImageSize().y)};
            }
        }

        return getInnerSize();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ProgressBar::draw(BackendRenderTarget& target, RenderStates states) const
    {
        // Draw the borders
        if (m_bordersCached != Borders{0})
        {
            target.drawBorders(states, m_bordersCached, getSize(), Color::applyOpacity(m_borderColorCached, m_opacityCached));
            states.transform.translate(m_bordersCached.getOffset());
        }

        // Draw the background
        if (m_spriteBackground.isSet())
            target.drawSprite(states, m_spriteBackground);
        else
        {
            Vector2f positionOffset = {m_backRect.left, m_backRect.top};

            states.transform.translate(positionOffset);
            target.drawFilledRect(states, {m_backRect.width, m_backRect.height}, Color::applyOpacity(m_backgroundColorCached, m_opacityCached));
            states.transform.translate(-positionOffset);
        }

        // Draw the filled area
        Vector2f imageShift;
        if (m_spriteFill.isSet())
        {
            if (m_spriteBackground.isSet() && (m_spriteBackground.getSize() != m_spriteFill.getSize()))
            {
                imageShift = (m_spriteBackground.getSize() - m_spriteFill.getSize()) / 2.f;

                states.transform.translate(imageShift);
                target.drawSprite(states, m_spriteFill);
                states.transform.translate(-imageShift);
            }
            else
                target.drawSprite(states, m_spriteFill);
        }
        else // Using colors instead of a texture
        {
            Vector2f positionOffset = {m_frontRect.left, m_frontRect.top};

            states.transform.translate(positionOffset);
            target.drawFilledRect(states, {m_frontRect.width, m_frontRect.height}, Color::applyOpacity(m_fillColorCached, m_opacityCached));
            states.transform.translate(-positionOffset);
        }

        // Draw the text
        if (m_textBack.getString() != U"")
        {
            const Vector2f textTranslation = (getInnerSize() - m_textBack.getSize()) / 2.f;

            if (m_textBack.getColor() == m_textFront.getColor())
            {
                states.transform.translate(textTranslation);
                target.drawText(states, m_textBack);
                states.transform.translate(-textTranslation);
            }
            else
            {
                // Draw the text on top of the unfilled part
                {
                    target.addClippingLayer(states, {imageShift + Vector2f{m_backRect.left, m_backRect.top}, {m_backRect.width, m_backRect.height}});

                    states.transform.translate(textTranslation);
                    target.drawText(states, m_textBack);
                    states.transform.translate(-textTranslation);

                    target.removeClippingLayer();
                }

                // Draw the text on top of the filled part
                {
                    target.addClippingLayer(states, {imageShift + Vector2f{m_frontRect.left, m_frontRect.top}, {m_frontRect.width, m_frontRect.height}});

                    states.transform.translate(textTranslation);
                    target.drawText(states, m_textFront);
                    states.transform.translate(-textTranslation);

                    target.removeClippingLayer();
                }
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Widget::Ptr ProgressBar::clone() const
    {
        return std::make_shared<ProgressBar>(*this);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
