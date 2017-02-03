/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus's Graphical User Interface
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


#include <TGUI/Container.hpp>
#include <TGUI/Widgets/ProgressBar.hpp>
#include <TGUI/Loading/Theme.hpp>
#include <TGUI/Clipping.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ProgressBar::ProgressBar()
    {
        m_callback.widgetType = "ProgressBar";

        addSignal<int>("ValueChanged");
        addSignal<int>("Full");

        m_renderer = std::make_shared<ProgressBarRenderer>(this);
        reload();

        setSize(160, 20);
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

    void ProgressBar::setPosition(const Layout2d& position)
    {
        Widget::setPosition(position);

        getRenderer()->m_textureBack.setPosition(getPosition());

        getRenderer()->m_textureFront.setPosition(getRenderer()->m_textureBack.getPosition()
                                                  + ((getRenderer()->m_textureBack.getSize() - getRenderer()->m_textureFront.getSize()) / 2.0f));


        m_textBack.setPosition(getPosition().x + (getSize().x - m_textBack.getSize().x) / 2.0f,
                               getPosition().y + (getSize().y - m_textBack.getSize().y) / 2.0f);
        m_textFront.setPosition(m_textBack.getPosition());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ProgressBar::setSize(const Layout2d& size)
    {
        Widget::setSize(size);

        if (getRenderer()->m_textureBack.isLoaded() && getRenderer()->m_textureFront.isLoaded())
        {
            getRenderer()->m_textureBack.setSize(getSize());

            sf::Vector2f frontSize;
            switch (getRenderer()->m_textureBack.getScalingType())
            {
            case Texture::ScalingType::Normal:
                frontSize.x = getRenderer()->m_textureFront.getImageSize().x * getSize().x / getRenderer()->m_textureBack.getImageSize().x;
                frontSize.y = getRenderer()->m_textureFront.getImageSize().y * getSize().y / getRenderer()->m_textureBack.getImageSize().y;
                break;

            case Texture::ScalingType::Horizontal:
                frontSize.x = getSize().x - ((getRenderer()->m_textureBack.getImageSize().x - getRenderer()->m_textureFront.getImageSize().x) * (getSize().y / getRenderer()->m_textureBack.getImageSize().y));
                frontSize.y = getRenderer()->m_textureFront.getImageSize().y * getSize().y / getRenderer()->m_textureBack.getImageSize().y;
                break;

            case Texture::ScalingType::Vertical:
                frontSize.x = getRenderer()->m_textureFront.getImageSize().x * getSize().x / getRenderer()->m_textureBack.getImageSize().x;
                frontSize.y = getSize().y - ((getRenderer()->m_textureBack.getImageSize().y - getRenderer()->m_textureFront.getImageSize().y) * (getSize().x / getRenderer()->m_textureBack.getImageSize().x));
                break;

            case Texture::ScalingType::NineSlice:
                frontSize.x = getSize().x - (getRenderer()->m_textureBack.getImageSize().x - getRenderer()->m_textureFront.getImageSize().x);
                frontSize.y = getSize().y - (getRenderer()->m_textureBack.getImageSize().y - getRenderer()->m_textureFront.getImageSize().y);
                break;
            }

            getRenderer()->m_textureFront.setSize(frontSize);
        }

        // Recalculate the size of the front image
        recalculateSize();

        // Recalculate the text size
        setText(getText());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    sf::Vector2f ProgressBar::getFullSize() const
    {
        return {getSize().x + getRenderer()->getBorders().left + getRenderer()->getBorders().right,
                getSize().y + getRenderer()->getBorders().top + getRenderer()->getBorders().bottom};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ProgressBar::setFont(const Font& font)
    {
        Widget::setFont(font);
        m_textBack.setFont(font);
        m_textFront.setFont(font);

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
        recalculateSize();
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
        recalculateSize();
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

            m_callback.value = static_cast<int>(m_value);
            sendSignal("ValueChanged", m_value);

            if (m_value == m_maximum)
                sendSignal("Full", m_value);

            // Recalculate the size of the front image (the size of the part that will be drawn)
            recalculateSize();
        }
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
        m_textBack.setText(text);
        m_textFront.setText(text);

        // Check if the text is auto sized
        if (m_textSize == 0)
        {
            unsigned int textSize;
            if (getRenderer()->m_textureBack.isLoaded() && getRenderer()->m_textureFront.isLoaded())
               textSize = findBestTextSize(getFont(), getRenderer()->m_textureFront.getSize().y * 0.85f);
            else
                textSize = findBestTextSize(getFont(), getSize().y * 0.85f);

            m_textBack.setTextSize(textSize);

            // Make the text smaller when it's too width
            if (m_textBack.getSize().x > (getSize().x * 0.85f))
                m_textBack.setTextSize(static_cast<unsigned int>(textSize * ((getSize().x * 0.85f) / m_textBack.getSize().x)));
        }
        else // When the text has a fixed size
        {
            // Set the text size
            m_textBack.setTextSize(m_textSize);
        }

        m_textFront.setTextSize(m_textBack.getTextSize());

        // Reposition the text
        updatePosition();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ProgressBar::setTextSize(unsigned int size)
    {
        // Change the text size
        m_textSize = size;

        // Call setText to reposition the text
        setText(getText());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ProgressBar::setFillDirection(FillDirection direction)
    {
        m_fillDirection = direction;

        recalculateSize();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ProgressBar::setOpacity(float opacity)
    {
        ClickableWidget::setOpacity(opacity);

        getRenderer()->m_textureBack.setColor({getRenderer()->m_textureBack.getColor().r, getRenderer()->m_textureBack.getColor().g, getRenderer()->m_textureBack.getColor().b, static_cast<sf::Uint8>(m_opacity * 255)});
        getRenderer()->m_textureFront.setColor({getRenderer()->m_textureFront.getColor().r, getRenderer()->m_textureFront.getColor().g, getRenderer()->m_textureFront.getColor().b, static_cast<sf::Uint8>(m_opacity * 255)});

        m_textBack.setTextColor(calcColorOpacity(getRenderer()->m_textColorBack, getOpacity()));
        m_textFront.setTextColor(calcColorOpacity(getRenderer()->m_textColorFront, getOpacity()));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    sf::Vector2f ProgressBar::getWidgetOffset() const
    {
        return {getRenderer()->getBorders().left, getRenderer()->getBorders().top};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ProgressBar::recalculateSize()
    {
        sf::Vector2f size;
        if (getRenderer()->m_textureBack.isLoaded() && getRenderer()->m_textureFront.isLoaded())
            size = getRenderer()->m_textureFront.getSize();
        else
            size = getSize();

        switch (getFillDirection())
        {
            case FillDirection::LeftToRight:
                m_frontRect =  {0, 0, size.x * ((m_value - m_minimum) / static_cast<float>(m_maximum - m_minimum)), size.y};
                break;

            case FillDirection::RightToLeft:
                m_frontRect =  {0, 0, size.x * ((m_value - m_minimum) / static_cast<float>(m_maximum - m_minimum)), size.y};
                m_frontRect.left = size.x - m_frontRect.width;
                break;

            case FillDirection::TopToBottom:
                m_frontRect =  {0, 0, size.x, size.y * ((m_value - m_minimum) / static_cast<float>(m_maximum - m_minimum))};
                break;

            case FillDirection::BottomToTop:
                m_frontRect =  {0, 0, size.x, size.y * ((m_value - m_minimum) / static_cast<float>(m_maximum - m_minimum))};
                m_frontRect.top = size.y - m_frontRect.height;
                break;
        }

        if (getRenderer()->m_textureBack.isLoaded() && getRenderer()->m_textureFront.isLoaded())
            getRenderer()->m_textureFront.setTextureRect(m_frontRect);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ProgressBar::reload(const std::string& primary, const std::string& secondary, bool force)
    {
        getRenderer()->setBorders({2, 2, 2, 2});
        getRenderer()->setTextColorBack({0, 0, 0});
        getRenderer()->setTextColorFront({255, 255, 255});
        getRenderer()->setBackgroundColor({245, 245, 245});
        getRenderer()->setForegroundColor({0, 110, 255});
        getRenderer()->setBorderColor({0, 0, 0});
        getRenderer()->setBackTexture({});
        getRenderer()->setFrontTexture({});

        if (m_theme && primary != "")
        {
            getRenderer()->setBorders({0, 0, 0, 0});
            Widget::reload(primary, secondary, force);

            if (force)
            {
                // Use the size of the images when images were loaded
                if (getRenderer()->m_textureBack.isLoaded() && getRenderer()->m_textureFront.isLoaded())
                {
                    setSize(getRenderer()->m_textureBack.getImageSize());

                    if (getSize().y >= 2 * getSize().x)
                        setFillDirection(FillDirection::BottomToTop);
                }
            }

            // Calculate the size of the front image (the size of the part that will be drawn)
            recalculateSize();
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ProgressBar::draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        getRenderer()->draw(target, states);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ProgressBarRenderer::setProperty(std::string property, const std::string& value)
    {
        property = toLower(property);

        if (property == "borders")
            setBorders(Deserializer::deserialize(ObjectConverter::Type::Borders, value).getBorders());
        else if (property == "backgroundcolor")
            setBackgroundColor(Deserializer::deserialize(ObjectConverter::Type::Color, value).getColor());
        else if (property == "foregroundcolor")
            setForegroundColor(Deserializer::deserialize(ObjectConverter::Type::Color, value).getColor());
        else if (property == "textcolor")
            setTextColor(Deserializer::deserialize(ObjectConverter::Type::Color, value).getColor());
        else if (property == "textcolorback")
            setTextColorBack(Deserializer::deserialize(ObjectConverter::Type::Color, value).getColor());
        else if (property == "textcolorfront")
            setTextColorFront(Deserializer::deserialize(ObjectConverter::Type::Color, value).getColor());
        else if (property == "bordercolor")
            setBorderColor(Deserializer::deserialize(ObjectConverter::Type::Color, value).getColor());
        else if (property == "backimage")
            setBackTexture(Deserializer::deserialize(ObjectConverter::Type::Texture, value).getTexture());
        else if (property == "frontimage")
            setFrontTexture(Deserializer::deserialize(ObjectConverter::Type::Texture, value).getTexture());
        else
            WidgetRenderer::setProperty(property, value);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ProgressBarRenderer::setProperty(std::string property, ObjectConverter&& value)
    {
        property = toLower(property);

        if (value.getType() == ObjectConverter::Type::Borders)
        {
            if (property == "borders")
                setBorders(value.getBorders());
            else
                return WidgetRenderer::setProperty(property, std::move(value));
        }
        else if (value.getType() == ObjectConverter::Type::Color)
        {
            if (property == "backgroundcolor")
                setBackgroundColor(value.getColor());
            else if (property == "foregroundcolor")
                setForegroundColor(value.getColor());
            else if (property == "textcolor")
                setTextColor(value.getColor());
            else if (property == "textcolorback")
                setTextColorBack(value.getColor());
            else if (property == "textcolorfront")
                setTextColorFront(value.getColor());
            else if (property == "bordercolor")
                setBorderColor(value.getColor());
            else
                WidgetRenderer::setProperty(property, std::move(value));
        }
        else if (value.getType() == ObjectConverter::Type::Texture)
        {
            if (property == "backimage")
                setBackTexture(value.getTexture());
            else if (property == "frontimage")
                setFrontTexture(value.getTexture());
            else
                WidgetRenderer::setProperty(property, std::move(value));
        }
        else
            WidgetRenderer::setProperty(property, std::move(value));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ObjectConverter ProgressBarRenderer::getProperty(std::string property) const
    {
        property = toLower(property);

        if (property == "borders")
            return m_borders;
        else if (property == "backgroundcolor")
            return m_backgroundColor;
        else if (property == "foregroundcolor")
            return m_foregroundColor;
        else if (property == "textcolor")
            return m_textColorBack;
        else if (property == "textcolorback")
            return m_textColorBack;
        else if (property == "textcolorfront")
            return m_textColorFront;
        else if (property == "bordercolor")
            return m_borderColor;
        else if (property == "backimage")
            return m_textureBack;
        else if (property == "frontimage")
            return m_textureFront;
        else
            return WidgetRenderer::getProperty(property);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::map<std::string, ObjectConverter> ProgressBarRenderer::getPropertyValuePairs() const
    {
        auto pairs = WidgetRenderer::getPropertyValuePairs();

        if (m_textureBack.isLoaded() && m_textureFront.isLoaded())
        {
            pairs["BackImage"] = m_textureBack;
            pairs["FrontImage"] = m_textureFront;
        }
        else
        {
            pairs["BackgroundColor"] = m_backgroundColor;
            pairs["ForegroundColor"] = m_foregroundColor;
        }

        if (m_progressBar->m_textBack.getTextColor() == m_progressBar->m_textFront.getTextColor())
            pairs["TextColor"] = m_textColorBack;
        else
        {
            pairs["TextColorBack"] = m_textColorBack;
            pairs["TextColorFront"] = m_textColorFront;
        }

        pairs["BorderColor"] = m_borderColor;
        pairs["Borders"] = m_borders;
        return pairs;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ProgressBarRenderer::setTextColor(const Color& color)
    {
        setTextColorBack(color);
        setTextColorFront(color);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ProgressBarRenderer::setTextColorBack(const Color& color)
    {
        m_textColorBack = color;
        m_progressBar->m_textBack.setTextColor(calcColorOpacity(m_textColorBack, m_progressBar->getOpacity()));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ProgressBarRenderer::setTextColorFront(const Color& color)
    {
        m_textColorFront = color;
        m_progressBar->m_textFront.setTextColor(calcColorOpacity(m_textColorFront, m_progressBar->getOpacity()));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ProgressBarRenderer::setBackgroundColor(const Color& color)
    {
        m_backgroundColor = color;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ProgressBarRenderer::setForegroundColor(const Color& color)
    {
        m_foregroundColor = color;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ProgressBarRenderer::setBorderColor(const Color& color)
    {
        m_borderColor = color;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ProgressBarRenderer::setBackTexture(const Texture& texture)
    {
        m_textureBack = texture;
        if (m_textureBack.isLoaded())
        {
            m_textureBack.setPosition(m_progressBar->getPosition());
            m_textureBack.setSize(m_progressBar->getSize());
            m_textureBack.setColor({m_textureBack.getColor().r, m_textureBack.getColor().g, m_textureBack.getColor().b, static_cast<sf::Uint8>(m_progressBar->getOpacity() * 255)});

            if (m_textureFront.isLoaded())
                m_progressBar->updateSize();
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ProgressBarRenderer::setFrontTexture(const Texture& texture)
    {
        m_textureFront = texture;
        if (m_textureFront.isLoaded())
        {
            if (m_textureBack.isLoaded())
                m_progressBar->updateSize();

            m_textureFront.setColor({m_textureFront.getColor().r, m_textureFront.getColor().g, m_textureFront.getColor().b, static_cast<sf::Uint8>(m_progressBar->getOpacity() * 255)});
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ProgressBarRenderer::draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        // Check if there are textures
        if (m_textureBack.isLoaded() && m_textureFront.isLoaded())
        {
            target.draw(m_textureBack, states);
            target.draw(m_textureFront, states);
        }
        else // There is no background texture
        {
            sf::RectangleShape back(m_progressBar->getSize());
            back.setPosition(m_progressBar->getPosition());
            back.setFillColor(calcColorOpacity(m_backgroundColor, m_progressBar->getOpacity()));
            target.draw(back, states);

            sf::Vector2f frontPosition = m_progressBar->getPosition();
            if (m_progressBar->getFillDirection() == ProgressBar::FillDirection::RightToLeft)
                frontPosition.x += m_progressBar->getSize().x - m_progressBar->m_frontRect.width;
            else if (m_progressBar->getFillDirection() == ProgressBar::FillDirection::BottomToTop)
                frontPosition.y += m_progressBar->getSize().y - m_progressBar->m_frontRect.height;

            sf::RectangleShape front({m_progressBar->m_frontRect.width, m_progressBar->m_frontRect.height});
            front.setPosition(frontPosition);
            front.setFillColor(calcColorOpacity(m_foregroundColor, m_progressBar->getOpacity()));
            target.draw(front, states);
        }

        // Draw the text
        if (m_progressBar->m_textBack.getText() != "")
        {
            if (m_progressBar->m_textBack.getTextColor() == m_progressBar->m_textFront.getTextColor())
                target.draw(m_progressBar->m_textBack, states);
            else
            {
                sf::FloatRect backRect;
                sf::FloatRect frontRect;
                frontRect.width = m_progressBar->m_frontRect.width;
                frontRect.height = m_progressBar->m_frontRect.height;

                switch (m_progressBar->getFillDirection())
                {
                    case ProgressBar::FillDirection::LeftToRight:
                    {
                        frontRect.left = m_progressBar->getPosition().x;
                        frontRect.top = m_progressBar->getPosition().y;

                        if (m_textureBack.isLoaded() && m_textureFront.isLoaded())
                        {
                            frontRect.left += m_textureFront.getPosition().x - m_progressBar->getPosition().x;
                            frontRect.top += m_textureFront.getPosition().y - m_progressBar->getPosition().y;

                            backRect.width = m_textureFront.getSize().x - frontRect.width;
                        }
                        else
                            backRect.width = m_progressBar->getSize().x - frontRect.width;

                        backRect.left = frontRect.left + frontRect.width;
                        backRect.top = frontRect.top;
                        backRect.height = frontRect.height;
                        break;
                    }

                    case ProgressBar::FillDirection::RightToLeft:
                    {
                        backRect.left = m_progressBar->getPosition().x;
                        backRect.top = m_progressBar->getPosition().y;

                        if (m_textureBack.isLoaded() && m_textureFront.isLoaded())
                        {
                            backRect.left += m_textureFront.getPosition().x - m_progressBar->getPosition().x;
                            backRect.top += m_textureFront.getPosition().y - m_progressBar->getPosition().y;

                            backRect.width = m_textureFront.getSize().x - frontRect.width;
                            backRect.height = m_textureFront.getSize().y;
                        }
                        else
                        {
                            backRect.width = m_progressBar->getSize().x - frontRect.width;
                            backRect.height = m_progressBar->getSize().y;
                        }

                        frontRect.left = backRect.left + backRect.width;
                        frontRect.top = backRect.top;
                        frontRect.height = backRect.height;
                        break;
                    }

                    case ProgressBar::FillDirection::TopToBottom:
                    {
                        frontRect.left = m_progressBar->getPosition().x;
                        frontRect.top = m_progressBar->getPosition().y;

                        if (m_textureBack.isLoaded() && m_textureFront.isLoaded())
                        {
                            frontRect.left += m_textureFront.getPosition().x - m_progressBar->getPosition().x;
                            frontRect.top += m_textureFront.getPosition().y - m_progressBar->getPosition().y;

                            backRect.height = m_textureFront.getSize().y - frontRect.height;
                        }
                        else
                            backRect.height = m_progressBar->getSize().y - frontRect.height;

                        backRect.left = frontRect.left;
                        backRect.top = frontRect.top + frontRect.height;
                        backRect.width = frontRect.width;
                        break;
                    }

                    case ProgressBar::FillDirection::BottomToTop:
                    {
                        backRect.left = m_progressBar->getPosition().x;
                        backRect.top = m_progressBar->getPosition().y;

                        if (m_textureBack.isLoaded() && m_textureFront.isLoaded())
                        {
                            backRect.left += m_textureFront.getPosition().x - m_progressBar->getPosition().x;
                            backRect.top += m_textureFront.getPosition().y - m_progressBar->getPosition().y;

                            backRect.width = m_textureFront.getSize().x;
                            backRect.height = m_textureFront.getSize().y - frontRect.height;
                        }
                        else
                        {
                            backRect.width = m_progressBar->getSize().x;
                            backRect.height = m_progressBar->getSize().y - frontRect.height;
                        }

                        frontRect.left = backRect.left;
                        frontRect.top = backRect.top + backRect.height;
                        frontRect.width = backRect.width;
                        break;
                    }
                }

                // Draw the back text
                {
                    Clipping clipping{target, states, {backRect.left, backRect.top}, {backRect.width, backRect.height}};
                    target.draw(m_progressBar->m_textBack, states);
                }

                // Draw the front text
                {
                    Clipping clipping{target, states, {frontRect.left, frontRect.top}, {frontRect.width, frontRect.height}};
                    target.draw(m_progressBar->m_textFront, states);
                }
            }
        }

        // Draw the borders around the progress bar
        if (m_borders != Borders{0, 0, 0, 0})
        {
            sf::Vector2f position = m_progressBar->getPosition();
            sf::Vector2f size = m_progressBar->getSize();

            // Draw left border
            sf::RectangleShape border({m_borders.left, size.y + m_borders.top});
            border.setPosition(position.x - m_borders.left, position.y - m_borders.top);
            border.setFillColor(calcColorOpacity(m_borderColor, m_progressBar->getOpacity()));
            target.draw(border, states);

            // Draw top border
            border.setSize({size.x + m_borders.right, m_borders.top});
            border.setPosition(position.x, position.y - m_borders.top);
            target.draw(border, states);

            // Draw right border
            border.setSize({m_borders.right, size.y + m_borders.bottom});
            border.setPosition(position.x + size.x, position.y);
            target.draw(border, states);

            // Draw bottom border
            border.setSize({size.x + m_borders.left, m_borders.bottom});
            border.setPosition(position.x - m_borders.left, position.y + size.y);
            target.draw(border, states);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::shared_ptr<WidgetRenderer> ProgressBarRenderer::clone(Widget* widget)
    {
        auto renderer = std::make_shared<ProgressBarRenderer>(*this);
        renderer->m_progressBar = static_cast<ProgressBar*>(widget);
        return renderer;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
