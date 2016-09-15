/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus' Graphical User Interface
// Copyright (C) 2012-2016 Bruno Van de Velde (vdv_b@tgui.eu)
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
        m_callback.widgetType = "ProgressBar";
        m_type = "ProgressBar";

        addSignal<int>("ValueChanged");
        addSignal<int>("Full");

        m_renderer = aurora::makeCopied<ProgressBarRenderer>();
        setRenderer(m_renderer->getData());

        getRenderer()->setBorders({2});
        getRenderer()->setTextColor(sf::Color::Black);
        getRenderer()->setTextColorFilled(sf::Color::White);
        getRenderer()->setBackgroundColor({245, 245, 245});
        getRenderer()->setFillColor({0, 110, 255});
        getRenderer()->setBorderColor(sf::Color::Black);

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

    void ProgressBar::setSize(const Layout2d& size)
    {
        Widget::setSize(size);

        if (getRenderer()->getTextureBackground().isLoaded())
            getRenderer()->getTextureBackground().setSize(getInnerSize());

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

            m_callback.value = static_cast<int>(m_value);
            sendSignal("ValueChanged", m_value);

            if (m_value == m_maximum)
                sendSignal("Full", m_value);

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
            if (getRenderer()->getTextureFill().isLoaded())
               textSize = Text::findBestTextSize(getRenderer()->getFont(), getRenderer()->getTextureFill().getSize().y * 0.8f);
            else
                textSize = Text::findBestTextSize(getRenderer()->getFont(), getInnerSize().y * 0.8f);

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

    void ProgressBar::rendererChanged(const std::string& property, ObjectConverter& value)
    {
        if (property == "borders")
        {
            updateSize();
        }
        else if ((property == "textcolor") || (property == "textcolorfilled"))
        {
            m_textBack.setColor(getRenderer()->getTextColor());

            if (getRenderer()->getTextColorFilled().isSet())
                m_textFront.setColor(getRenderer()->getTextColorFilled());
            else
                m_textFront.setColor(getRenderer()->getTextColor());
        }
        else if (property == "texturebackground")
        {
            value.getTexture().setSize(getInnerSize());
            value.getTexture().setOpacity(getRenderer()->getOpacity());
        }
        else if (property == "texturefill")
        {
            recalculateFillSize();
            value.getTexture().setOpacity(getRenderer()->getOpacity());
        }
        else if (property == "textstyle")
        {
            m_textBack.setStyle(value.getTextStyle());
            m_textFront.setStyle(value.getTextStyle());
        }
        else if (property == "opacity")
        {
            getRenderer()->getTextureBackground().setOpacity(value.getNumber());
            getRenderer()->getTextureFill().setOpacity(value.getNumber());

            m_textBack.setOpacity(value.getNumber());
            m_textFront.setOpacity(value.getNumber());
        }
        else if (property == "font")
        {
            m_textBack.setFont(value.getFont());
            m_textFront.setFont(value.getFont());
            setText(getText());
        }
        else if ((property != "bordercolor") && (property != "backgroundcolor") && (property != "fillcolor"))
            Widget::rendererChanged(property, value);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    sf::Vector2f ProgressBar::getInnerSize() const
    {
        Borders borders = getRenderer()->getBorders();
        return {getSize().x - borders.left - borders.right, getSize().y - borders.top - borders.bottom};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ProgressBar::recalculateFillSize()
    {
        sf::Vector2f size = getInnerSize();

        if (getRenderer()->getTextureFill().isLoaded())
        {
            sf::Vector2f frontSize;
            if (getRenderer()->getTextureBackground().isLoaded())
            {
                switch (getRenderer()->getTextureBackground().getScalingType())
                {
                case Texture::ScalingType::Normal:
                    frontSize.x = getRenderer()->getTextureFill().getImageSize().x * getInnerSize().x / getRenderer()->getTextureBackground().getImageSize().x;
                    frontSize.y = getRenderer()->getTextureFill().getImageSize().y * getInnerSize().y / getRenderer()->getTextureBackground().getImageSize().y;
                    break;

                case Texture::ScalingType::Horizontal:
                    frontSize.x = getInnerSize().x - ((getRenderer()->getTextureBackground().getImageSize().x - getRenderer()->getTextureFill().getImageSize().x) * (getInnerSize().y / getRenderer()->getTextureBackground().getImageSize().y));
                    frontSize.y = getRenderer()->getTextureFill().getImageSize().y * getInnerSize().y / getRenderer()->getTextureBackground().getImageSize().y;
                    break;

                case Texture::ScalingType::Vertical:
                    frontSize.x = getRenderer()->getTextureFill().getImageSize().x * getInnerSize().x / getRenderer()->getTextureBackground().getImageSize().x;
                    frontSize.y = getInnerSize().y - ((getRenderer()->getTextureBackground().getImageSize().y - getRenderer()->getTextureFill().getImageSize().y) * (getInnerSize().x / getRenderer()->getTextureBackground().getImageSize().x));
                    break;

                case Texture::ScalingType::NineSlice:
                    frontSize.x = getInnerSize().x - (getRenderer()->getTextureBackground().getImageSize().x - getRenderer()->getTextureFill().getImageSize().x);
                    frontSize.y = getInnerSize().y - (getRenderer()->getTextureBackground().getImageSize().y - getRenderer()->getTextureFill().getImageSize().y);
                    break;
                }
            }
            else // There is a fill texture but not a background one
                frontSize = getInnerSize();

            getRenderer()->getTextureFill().setSize(frontSize);
            size = frontSize;
        }

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

        if (getRenderer()->getTextureFill().isLoaded())
            getRenderer()->getTextureFill().setTextureRect(m_frontRect);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ProgressBar::draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        states.transform.translate(getPosition());

        // Draw the borders
        Borders borders = getRenderer()->getBorders();
        if (borders != Borders{0})
        {
            drawBorders(target, states, borders, getSize(), getRenderer()->getBorderColor());
            states.transform.translate({borders.left, borders.top});
        }

        // Draw the background
        if (getRenderer()->getTextureBackground().isLoaded())
            getRenderer()->getTextureBackground().draw(target, states);
        else
        {
            sf::Vector2f positionOffset = {m_backRect.left, m_backRect.top};

            states.transform.translate(positionOffset);
            drawRectangleShape(target, states, {m_backRect.width, m_backRect.height}, getRenderer()->getBackgroundColor());
            states.transform.translate(-positionOffset);
        }

        // Draw the filled area
        sf::Vector2f imageShift;
        if (getRenderer()->getTextureFill().isLoaded())
        {
            if (getRenderer()->getTextureBackground().isLoaded() && (getRenderer()->getTextureBackground().getSize() != getRenderer()->getTextureFill().getSize()))
            {
                imageShift = (getRenderer()->getTextureBackground().getSize() - getRenderer()->getTextureFill().getSize()) / 2.f;

                states.transform.translate(imageShift);
                getRenderer()->getTextureFill().draw(target, states);
                states.transform.translate(-imageShift);
            }
            else
                getRenderer()->getTextureFill().draw(target, states);
        }
        else // Using colors instead of a texture
        {
            sf::Vector2f positionOffset = {m_frontRect.left, m_frontRect.top};

            states.transform.translate(positionOffset);
            drawRectangleShape(target, states, {m_frontRect.width, m_frontRect.height}, getRenderer()->getFillColor());
            states.transform.translate(-positionOffset);
        }

        // Draw the text
        if (m_textBack.getString() != "")
        {
            sf::Vector2f textTranslation = (getInnerSize() - m_textBack.getSize()) / 2.f;

            if (sf::Color(m_textBack.getColor()) == sf::Color(m_textFront.getColor()))
            {
                states.transform.translate(textTranslation);
                m_textBack.draw(target, states);
                states.transform.translate(-textTranslation);
            }
            else
            {
                // Draw the text on top of the unfilled part
                {
                    Clipping clipping{target, states, imageShift + sf::Vector2f{m_backRect.left, m_backRect.top}, {m_backRect.width, m_backRect.height}};

                    states.transform.translate(textTranslation);
                    m_textBack.draw(target, states);
                    states.transform.translate(-textTranslation);
                }

                // Draw the text on top of the filled part
                {
                    Clipping clipping{target, states, imageShift + sf::Vector2f{m_frontRect.left, m_frontRect.top}, {m_frontRect.width, m_frontRect.height}};

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
