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


#include <TGUI/Widgets/ProgressBar.hpp>
#include <TGUI/Clipping.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ProgressBar::ProgressBar()
    {
        m_type = "ProgressBar";
        m_textBack.setFont(m_fontCached);
        m_textFront.setFont(m_fontCached);

        m_renderer = aurora::makeCopied<ProgressBarRenderer>();
        setRenderer(Theme::getDefault()->getRendererNoThrow(m_type));

        setTextSize(getGlobalTextSize());
        setSize({Text::getLineHeight(m_textBack) * 15,
                 Text::getLineHeight(m_textBack) * 1.25f + m_bordersCached.getTop() + m_bordersCached.getBottom()});

    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ProgressBar::Ptr ProgressBar::create()
    {
        return std::make_shared<ProgressBar>();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ProgressBar::Ptr ProgressBar::copy(ProgressBar::ConstPtr progressBar)
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

    const ProgressBarRenderer* ProgressBar::getRenderer() const
    {
        return aurora::downcast<const ProgressBarRenderer*>(Widget::getRenderer());
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
        setText(getText());
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

    void ProgressBar::setText(const sf::String& text)
    {
        // Set the new text
        m_textBack.setString(text);
        m_textFront.setString(text);

        // Check if the text is auto sized
        if (m_textSize == 0)
        {
            unsigned int textSize;
            if (m_spriteFill.isSet())
                textSize = Text::findBestTextSize(m_fontCached, m_spriteFill.getSize().y * 0.8f);
            else
                textSize = Text::findBestTextSize(m_fontCached, getInnerSize().y * 0.8f);

            m_textBack.setCharacterSize(textSize);

            // Make the text smaller when it's too width
            if (m_textBack.getSize().x > (getInnerSize().x * 0.85f))
                m_textBack.setCharacterSize(static_cast<unsigned int>(textSize * ((getInnerSize().x * 0.85f) / m_textBack.getSize().x)));
        }
        else // When the text has a fixed size
        {
            // Set the text size
            m_textBack.setCharacterSize(m_textSize);
        }

        m_textFront.setCharacterSize(m_textBack.getCharacterSize());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const sf::String& ProgressBar::getText() const
    {
        return m_textBack.getString();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ProgressBar::setTextSize(unsigned int size)
    {
        // Change the text size
        m_textSize = size;
        setText(getText());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int ProgressBar::getTextSize() const
    {
        return m_textBack.getCharacterSize();
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

    bool ProgressBar::mouseOnWidget(Vector2f pos) const
    {
        if (ClickableWidget::mouseOnWidget(pos))
        {
            if (!m_transparentTextureCached || !m_spriteBackground.isTransparentPixel(pos - getPosition() - m_bordersCached.getOffset()))
                return true;
        }

        return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Signal& ProgressBar::getSignal(std::string signalName)
    {
        if (signalName == toLower(onValueChange.getName()))
            return onValueChange;
        else if (signalName == toLower(onFull.getName()))
            return onFull;
        else
            return ClickableWidget::getSignal(std::move(signalName));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ProgressBar::rendererChanged(const std::string& property)
    {
        if (property == "borders")
        {
            m_bordersCached = getSharedRenderer()->getBorders();
            setSize(m_size);
        }
        else if ((property == "textcolor") || (property == "textcolorfilled"))
        {
            m_textBack.setColor(getSharedRenderer()->getTextColor());

            if (getSharedRenderer()->getTextColorFilled().isSet())
                m_textFront.setColor(getSharedRenderer()->getTextColorFilled());
            else
                m_textFront.setColor(getSharedRenderer()->getTextColor());
        }
        else if (property == "texturebackground")
        {
            m_spriteBackground.setTexture(getSharedRenderer()->getTextureBackground());
        }
        else if (property == "texturefill")
        {
            m_spriteFill.setTexture(getSharedRenderer()->getTextureFill());
            recalculateFillSize();
        }
        else if (property == "textstyle")
        {
            m_textBack.setStyle(getSharedRenderer()->getTextStyle());
            m_textFront.setStyle(getSharedRenderer()->getTextStyle());
        }
        else if (property == "bordercolor")
        {
            m_borderColorCached = getSharedRenderer()->getBorderColor();
        }
        else if (property == "backgroundcolor")
        {
            m_backgroundColorCached = getSharedRenderer()->getBackgroundColor();
        }
        else if (property == "fillcolor")
        {
            m_fillColorCached = getSharedRenderer()->getFillColor();
        }
        else if (property == "opacity")
        {
            Widget::rendererChanged(property);

            m_spriteBackground.setOpacity(m_opacityCached);
            m_spriteFill.setOpacity(m_opacityCached);

            m_textBack.setOpacity(m_opacityCached);
            m_textFront.setOpacity(m_opacityCached);
        }
        else if (property == "font")
        {
            Widget::rendererChanged(property);

            m_textBack.setFont(m_fontCached);
            m_textFront.setFont(m_fontCached);
            setText(getText());
        }
        else
            Widget::rendererChanged(property);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::unique_ptr<DataIO::Node> ProgressBar::save(SavingRenderersMap& renderers) const
    {
        auto node = Widget::save(renderers);

        if (!getText().isEmpty())
            node->propertyValuePairs["Text"] = std::make_unique<DataIO::ValueNode>(Serializer::serialize(getText()));

        if (m_fillDirection != ProgressBar::FillDirection::LeftToRight)
        {
            if (m_fillDirection == ProgressBar::FillDirection::RightToLeft)
                node->propertyValuePairs["FillDirection"] = std::make_unique<DataIO::ValueNode>("RightToLeft");
            else if (m_fillDirection == ProgressBar::FillDirection::TopToBottom)
                node->propertyValuePairs["FillDirection"] = std::make_unique<DataIO::ValueNode>("TopToBottom");
            else if (m_fillDirection == ProgressBar::FillDirection::BottomToTop)
                node->propertyValuePairs["FillDirection"] = std::make_unique<DataIO::ValueNode>("BottomToTop");
        }

        node->propertyValuePairs["Minimum"] = std::make_unique<DataIO::ValueNode>(to_string(m_minimum));
        node->propertyValuePairs["Maximum"] = std::make_unique<DataIO::ValueNode>(to_string(m_maximum));
        node->propertyValuePairs["Value"] = std::make_unique<DataIO::ValueNode>(to_string(m_value));
        node->propertyValuePairs["TextSize"] = std::make_unique<DataIO::ValueNode>(to_string(m_textSize));

        return node;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ProgressBar::load(const std::unique_ptr<DataIO::Node>& node, const LoadingRenderersMap& renderers)
    {
        Widget::load(node, renderers);

        if (node->propertyValuePairs["minimum"])
            setMinimum(tgui::stoi(node->propertyValuePairs["minimum"]->value));
        if (node->propertyValuePairs["maximum"])
            setMaximum(tgui::stoi(node->propertyValuePairs["maximum"]->value));
        if (node->propertyValuePairs["value"])
            setValue(tgui::stoi(node->propertyValuePairs["value"]->value));
        if (node->propertyValuePairs["text"])
            setText(Deserializer::deserialize(ObjectConverter::Type::String, node->propertyValuePairs["text"]->value).getString());
        if (node->propertyValuePairs["textsize"])
            setTextSize(tgui::stoi(node->propertyValuePairs["textsize"]->value));

        if (node->propertyValuePairs["filldirection"])
        {
            std::string requestedStyle = toLower(trim(node->propertyValuePairs["filldirection"]->value));
            if (requestedStyle == "lefttoright")
                setFillDirection(ProgressBar::FillDirection::LeftToRight);
            else if (requestedStyle == "righttoleft")
                setFillDirection(ProgressBar::FillDirection::RightToLeft);
            else if (requestedStyle == "toptobottom")
                setFillDirection(ProgressBar::FillDirection::TopToBottom);
            else if (requestedStyle == "bottomtotop")
                setFillDirection(ProgressBar::FillDirection::BottomToTop);
            else
                throw Exception{"Failed to parse FillDirection property, found unknown value."};
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Vector2f ProgressBar::getInnerSize() const
    {
        return {getSize().x - m_bordersCached.getLeft() - m_bordersCached.getRight(),
                getSize().y - m_bordersCached.getTop() - m_bordersCached.getBottom()};
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

    void ProgressBar::draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        states.transform.translate(getPosition());

        // Draw the borders
        if (m_bordersCached != Borders{0})
        {
            drawBorders(target, states, m_bordersCached, getSize(), m_borderColorCached);
            states.transform.translate(m_bordersCached.getOffset());
        }

        // Draw the background
        if (m_spriteBackground.isSet())
            m_spriteBackground.draw(target, states);
        else
        {
            Vector2f positionOffset = {m_backRect.left, m_backRect.top};

            states.transform.translate(positionOffset);
            drawRectangleShape(target, states, {m_backRect.width, m_backRect.height}, m_backgroundColorCached);
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
                m_spriteFill.draw(target, states);
                states.transform.translate(-imageShift);
            }
            else
                m_spriteFill.draw(target, states);
        }
        else // Using colors instead of a texture
        {
            Vector2f positionOffset = {m_frontRect.left, m_frontRect.top};

            states.transform.translate(positionOffset);
            drawRectangleShape(target, states, {m_frontRect.width, m_frontRect.height}, m_fillColorCached);
            states.transform.translate(-positionOffset);
        }

        // Draw the text
        if (m_textBack.getString() != "")
        {
            Vector2f textTranslation = (getInnerSize() - m_textBack.getSize()) / 2.f;

            if (m_textBack.getColor() == m_textFront.getColor())
            {
                states.transform.translate(textTranslation);
                m_textBack.draw(target, states);
                states.transform.translate(-textTranslation);
            }
            else
            {
                // Draw the text on top of the unfilled part
                {
                    Clipping clipping{target, states, imageShift + Vector2f{m_backRect.left, m_backRect.top}, {m_backRect.width, m_backRect.height}};

                    states.transform.translate(textTranslation);
                    m_textBack.draw(target, states);
                    states.transform.translate(-textTranslation);
                }

                // Draw the text on top of the filled part
                {
                    Clipping clipping{target, states, imageShift + Vector2f{m_frontRect.left, m_frontRect.top}, {m_frontRect.width, m_frontRect.height}};

                    states.transform.translate(textTranslation);
                    m_textFront.draw(target, states);
                    states.transform.translate(-textTranslation);
                }
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
