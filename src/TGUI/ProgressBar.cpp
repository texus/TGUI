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


#include <TGUI/Container.hpp>
#include <TGUI/ProgressBar.hpp>

#include <SFML/OpenGL.hpp>

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

        getRenderer()->setBorders({2, 2, 2, 2});

        m_textBack.setTextColor({0, 0, 0});
        m_textFront.setTextColor({255, 255, 255});

        setSize(160, 20);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ProgressBar::Ptr ProgressBar::create(const std::string& themeFileFilename, const std::string& section)
    {
        auto progressBar = std::make_shared<ProgressBar>();

        if (themeFileFilename != "")
        {
            progressBar->getRenderer()->setBorders({0, 0, 0, 0});

            std::string loadedThemeFile = getResourcePath() + themeFileFilename;

            // Open the theme file
            ThemeFileParser themeFile{loadedThemeFile, section};

            // Find the folder that contains the theme file
            std::string themeFileFolder = "";
            std::string::size_type slashPos = loadedThemeFile.find_last_of("/\\");
            if (slashPos != std::string::npos)
                themeFileFolder = loadedThemeFile.substr(0, slashPos+1);

            // Handle the read properties
            for (auto it = themeFile.getProperties().cbegin(); it != themeFile.getProperties().cend(); ++it)
            {
                try
                {
                    progressBar->getRenderer()->setProperty(it->first, it->second, themeFileFolder);
                }
                catch (const Exception& e)
                {
                    throw Exception{std::string(e.what()) + " In section '" + section + "' in " + loadedThemeFile + "."};
                }
            }

            // Use the size of the images when images were loaded
            if (progressBar->getRenderer()->m_textureBack.getData() != nullptr)
            {
                progressBar->setSize(progressBar->getRenderer()->m_textureBack.getImageSize());

                if (progressBar->getSize().y >= 2 * progressBar->getSize().x)
                    progressBar->setFillDirection(FillDirection::BottomToTop);
            }

            // Calculate the size of the front image (the size of the part that will be drawn)
            progressBar->recalculateSize();
        }

        return progressBar;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ProgressBar::Ptr ProgressBar::copy(ProgressBar::ConstPtr progressBar)
    {
        if (progressBar)
            return std::make_shared<ProgressBar>(*progressBar);
        else
            return nullptr;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ProgressBar::setPosition(const Layout& position)
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

    void ProgressBar::setSize(const Layout& size)
    {
        Widget::setSize(size);

        if (getRenderer()->m_textureBack.getData() && getRenderer()->m_textureFront.getData())
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

            case Texture::ScalingType::NineSliceScaling:
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
            // Calculate a possible text size
            float size = getSize().y * 0.75f;
            m_textBack.setTextSize(static_cast<unsigned int>(size));

            // Make the text smaller when it is too width
            if (m_textBack.getSize().x > (getSize().x * 0.8f))
                m_textBack.setTextSize(static_cast<unsigned int>(size / (m_textBack.getSize().x / (getSize().x * 0.8f))));
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

    void ProgressBar::setTransparency(unsigned char transparency)
    {
        ClickableWidget::setTransparency(transparency);

        getRenderer()->m_textureBack.setColor(sf::Color(255, 255, 255, m_opacity));
        getRenderer()->m_textureFront.setColor(sf::Color(255, 255, 255, m_opacity));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ProgressBar::recalculateSize()
    {
        sf::Vector2f size;
        if (getRenderer()->m_textureBack.getData() && getRenderer()->m_textureFront.getData())
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

        if (getRenderer()->m_textureBack.getData() && getRenderer()->m_textureFront.getData())
            getRenderer()->m_textureFront.setTextureRect(m_frontRect);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ProgressBar::initialize(Container *const parent)
    {
        Widget::initialize(parent);

        if (!m_font && m_parent->getGlobalFont())
            getRenderer()->setTextFont(m_parent->getGlobalFont());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ProgressBar::draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        getRenderer()->draw(target, states);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ProgressBarRenderer::setProperty(std::string property, const std::string& value, const std::string& rootPath)
    {
        if (property == "backimage")
            extractTextureFromString(property, value, rootPath, m_textureBack);
        else if (property == "frontimage")
            extractTextureFromString(property, value, rootPath, m_textureFront);
        else if (property == "backgroundcolor")
            setBackgroundColor(extractColorFromString(property, value));
        else if (property == "foregroundcolor")
            setForegroundColor(extractColorFromString(property, value));
        else if (property == "textcolor")
            setTextColor(extractColorFromString(property, value));
        else if (property == "textcolorback")
            setTextColorBack(extractColorFromString(property, value));
        else if (property == "textcolorfront")
            setTextColorFront(extractColorFromString(property, value));
        else if (property == "textsize")
            m_progressBar->setTextSize(tgui::stoi(value));
        else if (property == "borders")
            setBorders(extractBordersFromString(property, value));
        else if (property == "bordercolor")
            setBorderColor(extractColorFromString(property, value));
        else
            throw Exception{"Unrecognized property '" + property + "'."};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ProgressBarRenderer::setTextFont(std::shared_ptr<sf::Font> font)
    {
        m_progressBar->m_font = font;
        m_progressBar->m_textBack.setTextFont(font);
        m_progressBar->m_textFront.setTextFont(font);

        m_progressBar->setText(m_progressBar->getText());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ProgressBarRenderer::setTextColor(const sf::Color& color)
    {
        m_progressBar->m_textBack.setTextColor(color);
        m_progressBar->m_textFront.setTextColor(color);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ProgressBarRenderer::setTextColorBack(const sf::Color& color)
    {
        m_progressBar->m_textBack.setTextColor(color);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ProgressBarRenderer::setTextColorFront(const sf::Color& color)
    {
        m_progressBar->m_textFront.setTextColor(color);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ProgressBarRenderer::setBackgroundColor(const sf::Color& color)
    {
        m_backgroundColor = color;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ProgressBarRenderer::setForegroundColor(const sf::Color& color)
    {
        m_foregroundColor = color;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ProgressBarRenderer::setBorderColor(const sf::Color& color)
    {
        m_borderColor = color;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ProgressBarRenderer::setBackImage(const std::string& filename, const sf::IntRect& partRect, const sf::IntRect& middlePart, bool repeated)
    {
        if (filename != "")
            m_textureBack.load(getResourcePath() + filename, partRect, middlePart, repeated);
        else
            m_textureBack = {};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ProgressBarRenderer::setFrontImage(const std::string& filename, const sf::IntRect& partRect, const sf::IntRect& middlePart, bool repeated)
    {
        if (filename != "")
            m_textureFront.load(getResourcePath() + filename, partRect, middlePart, repeated);
        else
            m_textureFront = {};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ProgressBarRenderer::draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        // Check if there are textures
        if ((m_textureBack.getData() != nullptr) && (m_textureFront.getData() != nullptr))
        {
            target.draw(m_textureBack, states);
            target.draw(m_textureFront, states);
        }
        else // There is no background texture
        {
            sf::RectangleShape back(m_progressBar->getSize());
            back.setPosition(m_progressBar->getPosition());
            back.setFillColor(m_backgroundColor);
            target.draw(back, states);

            sf::Vector2f frontPosition = m_progressBar->getPosition();
            if (m_progressBar->getFillDirection() == ProgressBar::FillDirection::RightToLeft)
                frontPosition.x += m_progressBar->getSize().x - m_progressBar->m_frontRect.width;
            else if (m_progressBar->getFillDirection() == ProgressBar::FillDirection::BottomToTop)
                frontPosition.y += m_progressBar->getSize().y - m_progressBar->m_frontRect.height;

            sf::RectangleShape front({m_progressBar->m_frontRect.width, m_progressBar->m_frontRect.height});
            front.setPosition(frontPosition);
            front.setFillColor(m_foregroundColor);
            target.draw(front, states);
        }

        // Draw the text
        if (m_progressBar->m_textBack.getText() != "")
        {
            if (m_progressBar->m_textBack.getTextColor() == m_progressBar->m_textFront.getTextColor())
                target.draw(m_progressBar->m_textBack, states);
            else
            {
                // Get the old clipping area
                GLint scissor[4];
                glGetIntegerv(GL_SCISSOR_BOX, scissor);

                // Calculate the scale factor of the view
                const sf::View& view = target.getView();
                float scaleViewX = target.getSize().x / view.getSize().x;
                float scaleViewY = target.getSize().y / view.getSize().y;

                sf::FloatRect backRect;
                sf::FloatRect frontRect;
                frontRect.width = m_progressBar->m_frontRect.width;
                frontRect.height = m_progressBar->m_frontRect.height;

                switch (m_progressBar->getFillDirection())
                {
                    case ProgressBar::FillDirection::LeftToRight:
                    {
                        frontRect.left = m_progressBar->getAbsolutePosition().x;
                        frontRect.top = m_progressBar->getAbsolutePosition().y;

                        if ((m_textureBack.getData() != nullptr) && (m_textureFront.getData() != nullptr))
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
                        backRect.left = m_progressBar->getAbsolutePosition().x;
                        backRect.top = m_progressBar->getAbsolutePosition().y;

                        if ((m_textureBack.getData() != nullptr) && (m_textureFront.getData() != nullptr))
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
                        frontRect.left = m_progressBar->getAbsolutePosition().x;
                        frontRect.top = m_progressBar->getAbsolutePosition().y;

                        if ((m_textureBack.getData() != nullptr) && (m_textureFront.getData() != nullptr))
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
                        backRect.left = m_progressBar->getAbsolutePosition().x;
                        backRect.top = m_progressBar->getAbsolutePosition().y;

                        if ((m_textureBack.getData() != nullptr) && (m_textureFront.getData() != nullptr))
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

                // Calculate the clipping area for the back text
                GLint scissorLeft = std::max(static_cast<GLint>(backRect.left * scaleViewX), scissor[0]);
                GLint scissorTop = std::max(static_cast<GLint>(backRect.top * scaleViewY), static_cast<GLint>(target.getSize().y) - scissor[1] - scissor[3]);
                GLint scissorRight = std::min(static_cast<GLint>((backRect.left + backRect.width) * scaleViewX), scissor[0] + scissor[2]);
                GLint scissorBottom = std::min(static_cast<GLint>((backRect.top + backRect.height) * scaleViewY), static_cast<GLint>(target.getSize().y) - scissor[1]);

                if (scissorRight < scissorLeft)
                    scissorRight = scissorLeft;
                else if (scissorBottom < scissorTop)
                    scissorTop = scissorBottom;

                // Set the clipping area
                glScissor(scissorLeft, target.getSize().y - scissorBottom, scissorRight - scissorLeft, scissorBottom - scissorTop);

                // Draw the back text
                target.draw(m_progressBar->m_textBack, states);

                // Calculate the clipping area for the front text
                scissorLeft = std::max(static_cast<GLint>(frontRect.left * scaleViewX), scissor[0]);
                scissorTop = std::max(static_cast<GLint>(frontRect.top * scaleViewY), static_cast<GLint>(target.getSize().y) - scissor[1] - scissor[3]);
                scissorRight = std::min(static_cast<GLint>((frontRect.left + frontRect.width) * scaleViewX), scissor[0] + scissor[2]);
                scissorBottom = std::min(static_cast<GLint>((frontRect.top + frontRect.height) * scaleViewY), static_cast<GLint>(target.getSize().y) - scissor[1]);

                if (scissorRight < scissorLeft)
                    scissorRight = scissorLeft;
                else if (scissorBottom < scissorTop)
                    scissorTop = scissorBottom;

                // Set the clipping area
                glScissor(scissorLeft, target.getSize().y - scissorBottom, scissorRight - scissorLeft, scissorBottom - scissorTop);

                // Draw the front text
                target.draw(m_progressBar->m_textFront, states);

                // Reset the old clipping area
                glScissor(scissor[0], scissor[1], scissor[2], scissor[3]);
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
            border.setFillColor(m_borderColor);
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
        auto renderer = std::shared_ptr<ProgressBarRenderer>(new ProgressBarRenderer{*this});
        renderer->m_progressBar = static_cast<ProgressBar*>(widget);
        return renderer;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
