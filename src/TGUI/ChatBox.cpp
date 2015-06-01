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


#include <TGUI/Label.hpp>
#include <TGUI/Panel.hpp>
#include <TGUI/Scrollbar.hpp>
#include <TGUI/ChatBox.hpp>

#include <cassert>
#include <cmath>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ChatBox::ChatBox()
    {
        m_callback.widgetType = "ChatBox";
        m_draggableWidget = true;

        m_renderer = std::make_shared<ChatBoxRenderer>(this);

        getRenderer()->setBorders({2, 2, 2, 2});
        getRenderer()->setPadding({2, 2, 2, 2});

        m_panel->setBackgroundColor(sf::Color::Transparent);

        setSize({200, 120});
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ChatBox::ChatBox(const ChatBox& chatBoxToCopy) :
        Widget             {chatBoxToCopy},
        m_lineSpacing      {chatBoxToCopy.m_lineSpacing},
        m_textSize         {chatBoxToCopy.m_textSize},
        m_textColor        {chatBoxToCopy.m_textColor},
        m_maxLines         {chatBoxToCopy.m_maxLines},
        m_fullTextHeight   {chatBoxToCopy.m_fullTextHeight},
        m_linesStartFromTop{chatBoxToCopy.m_linesStartFromTop},
        m_panel            {Panel::copy(chatBoxToCopy.m_panel)},
        m_scroll           {Scrollbar::copy(chatBoxToCopy.m_scroll)}
    {
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ChatBox& ChatBox::operator= (const ChatBox& right)
    {
        if (this != &right)
        {
            ChatBox temp{right};
            Widget::operator=(right);

            std::swap(m_lineSpacing,       temp.m_lineSpacing);
            std::swap(m_textSize,          temp.m_textSize);
            std::swap(m_textColor,         temp.m_textColor);
            std::swap(m_maxLines,          temp.m_maxLines);
            std::swap(m_fullTextHeight,    temp.m_fullTextHeight);
            std::swap(m_linesStartFromTop, temp.m_linesStartFromTop);
            std::swap(m_panel,             temp.m_panel);
            std::swap(m_scroll,            temp.m_scroll);
        }

        return *this;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ChatBox::Ptr ChatBox::create(const std::string& themeFileFilename, const std::string& section)
    {
        auto chatBox = std::make_shared<ChatBox>();

        if (themeFileFilename != "")
        {
            chatBox->getRenderer()->setBorders({0, 0, 0, 0});

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
                    if (it->first == "scrollbar")
                    {
                        if (toLower(it->second) != "none")
                        {
                            if ((it->second.length() < 3) || (it->second[0] != '"') || (it->second[it->second.length()-1] != '"'))
                                throw Exception{"Failed to parse value for 'Scrollbar' property."};

                            chatBox->getRenderer()->setScrollbar(themeFileFilename, it->second.substr(1, it->second.length()-2));
                        }
                        else // There should be no scrollbar
                            chatBox->removeScrollbar();
                    }
                    else
                        chatBox->getRenderer()->setProperty(it->first, it->second, themeFileFolder);
                }
                catch (const Exception& e)
                {
                    throw Exception{std::string(e.what()) + " In section '" + section + "' in " + loadedThemeFile + "."};
                }
            }

            chatBox->updateSize();
        }

        return chatBox;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ChatBox::Ptr ChatBox::copy(ChatBox::ConstPtr chatBox)
    {
        if (chatBox)
            return std::make_shared<ChatBox>(*chatBox);
        else
            return nullptr;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChatBox::setPosition(const Layout& position)
    {
        sf::Vector2f oldPosition = getPosition();

        Widget::setPosition(position);

        getRenderer()->m_backgroundTexture.setPosition(getPosition());

        m_panel->move(getPosition() - oldPosition);
        if (m_scroll)
            m_scroll->move(getPosition() - oldPosition);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChatBox::setSize(const Layout& size)
    {
        Widget::setSize(size);

        getRenderer()->m_backgroundTexture.setSize(getSize());

        updateRendering();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    sf::Vector2f ChatBox::getFullSize() const
    {
        return {getSize().x + getRenderer()->m_borders.left + getRenderer()->m_borders.right,
                getSize().y + getRenderer()->m_borders.top + getRenderer()->m_borders.bottom};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChatBox::addLine(const sf::String& text)
    {
        addLine(text, m_textColor, m_textSize);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChatBox::addLine(const sf::String& text, const sf::Color& color)
    {
        addLine(text, color, m_textSize);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChatBox::addLine(const sf::String& text, unsigned int textSize)
    {
        addLine(text, m_textColor, textSize);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChatBox::addLine(const sf::String& text, const sf::Color& color, unsigned int textSize, std::shared_ptr<sf::Font> font)
    {
        // Remove the top line if you exceed the maximum
        if ((m_maxLines > 0) && (m_maxLines < m_panel->getWidgets().size() + 1))
            removeLine(0);

        auto label = Label::create();
        label->setTextColor(color);
        label->setTextSize(textSize);
        label->setText(text);
        m_panel->add(label);

        if (font != nullptr)
            label->setTextFont(font);

        recalculateFullTextHeight();

        if (m_scroll && (m_scroll->getMaximum() > m_scroll->getLowValue()))
            m_scroll->setValue(m_scroll->getMaximum() - m_scroll->getLowValue());

        updateDisplayedText();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    sf::String ChatBox::getLine(std::size_t lineIndex)
    {
        if (lineIndex < m_panel->getWidgets().size())
        {
            return std::static_pointer_cast<Label>(m_panel->getWidgets()[lineIndex])->getText();
        }
        else // Index too high
            return "";
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ChatBox::removeLine(std::size_t lineIndex)
    {
        if (lineIndex < m_panel->getWidgets().size())
        {
            m_panel->remove(std::static_pointer_cast<Label>(m_panel->getWidgets()[lineIndex]));

            recalculateFullTextHeight();
            updateDisplayedText();
            return true;
        }
        else // Index too high
            return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChatBox::removeAllLines()
    {
        m_panel->removeAllWidgets();

        recalculateFullTextHeight();
        updateDisplayedText();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChatBox::setLineLimit(std::size_t maxLines)
    {
        m_maxLines = maxLines;

        if ((m_maxLines > 0) && (m_maxLines < m_panel->getWidgets().size()))
        {
            while (m_maxLines < m_panel->getWidgets().size())
                m_panel->remove(m_panel->getWidgets()[0]);

            recalculateFullTextHeight();
            updateDisplayedText();
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChatBox::setTextFont(std::shared_ptr<sf::Font> font)
    {
        m_font = font;
        m_panel->setGlobalFont(font);

        bool lineChanged = false;
        for (auto& label : m_panel->getWidgets())
        {
            auto line = std::static_pointer_cast<Label>(label);
            if (line->getTextFont() == nullptr)
            {
                line->setTextFont(font);
                lineChanged = true;
            }
        }

        if (lineChanged)
        {
            recalculateFullTextHeight();

            if (m_scroll && (m_scroll->getMaximum() > m_scroll->getLowValue()))
                m_scroll->setValue(m_scroll->getMaximum() - m_scroll->getLowValue());

            updateDisplayedText();
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChatBox::setTextSize(unsigned int size)
    {
        // Store the new text size
        m_textSize = size;

        // There is a minimum text size
        if (m_textSize < 8)
            m_textSize = 8;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChatBox::setLineSpacing(unsigned int lineSpacing)
    {
        if (m_lineSpacing != lineSpacing)
        {
            m_lineSpacing = lineSpacing;

            recalculateFullTextHeight();
            updateDisplayedText();
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChatBox::removeScrollbar()
    {
        if (m_scroll != nullptr)
        {
            m_scroll = nullptr;

            recalculateFullTextHeight();
            updateDisplayedText();
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChatBox::setLinesStartFromTop(bool startFromTop)
    {
        if (m_linesStartFromTop != startFromTop)
        {
            m_linesStartFromTop = startFromTop;

            updateDisplayedText();
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChatBox::setTransparency(unsigned char transparency)
    {
        Widget::setTransparency(transparency);

        m_panel->setTransparency(transparency);

        if (m_scroll != nullptr)
            m_scroll->setTransparency(transparency);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ChatBox::mouseOnWidget(float x, float y)
    {
        // Pass the event to the scrollbar (if there is one)
        if (m_scroll != nullptr)
            m_scroll->mouseOnWidget(x, y);

        // Check if the mouse is on top of the list box
        if (sf::FloatRect{getPosition().x, getPosition().y, getSize().x, getSize().y}.contains(x, y))
            return true;
        else // The mouse is not on top of the list box
        {
            if (m_mouseHover)
                mouseLeftWidget();

            m_mouseHover = false;
            return false;
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChatBox::leftMousePressed(float x, float y)
    {
        // Set the mouse down flag to true
        m_mouseDown = true;

        // If there is a scrollbar then pass the event
        if (m_scroll != nullptr)
        {
            // Remember the old scrollbar value
            unsigned int oldValue = m_scroll->getValue();

            // Pass the event
            if (m_scroll->mouseOnWidget(x, y))
                m_scroll->leftMousePressed(x, y);

            // If the value of the scrollbar has changed then update the text
            if (oldValue != m_scroll->getValue())
                updateDisplayedText();
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChatBox::leftMouseReleased(float x, float y)
    {
        // If there is a scrollbar then pass it the event
        if (m_scroll != nullptr)
        {
            // Only pass the event when the scrollbar still thinks the mouse is down
            if (m_scroll->m_mouseDown == true)
            {
                // Remember the old scrollbar value
                unsigned int oldValue = m_scroll->getValue();

                // Pass the event
                m_scroll->leftMouseReleased(x, y);

                // If the value of the scrollbar has changed then update the text
                if (oldValue != m_scroll->getValue())
                {
                    // Check if the scrollbar value was incremented (you have pressed on the down arrow)
                    if (m_scroll->getValue() == oldValue + 1)
                    {
                        // Decrement the value
                        m_scroll->setValue(m_scroll->getValue()-1);

                        // Scroll down with the whole item height instead of with a single pixel
                        m_scroll->setValue(m_scroll->getValue() + m_textSize - (m_scroll->getValue() % m_textSize));
                    }
                    else if (m_scroll->getValue() == oldValue - 1) // Check if the scrollbar value was decremented (you have pressed on the up arrow)
                    {
                        // increment the value
                        m_scroll->setValue(m_scroll->getValue()+1);

                        // Scroll up with the whole item height instead of with a single pixel
                        if (m_scroll->getValue() % m_textSize > 0)
                            m_scroll->setValue(m_scroll->getValue() - (m_scroll->getValue() % m_textSize));
                        else
                            m_scroll->setValue(m_scroll->getValue() - m_textSize);
                    }

                    updateDisplayedText();
                }
            }
        }

        m_mouseDown = false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChatBox::mouseMoved(float x, float y)
    {
        if (!m_mouseHover)
            mouseEnteredWidget();

        // If there is a scrollbar then pass the event
        if (m_scroll != nullptr)
        {
            // Check if you are dragging the thumb of the scrollbar
            if ((m_scroll->m_mouseDown) && (m_scroll->m_mouseDownOnThumb))
            {
                // Remember the old scrollbar value
                unsigned int oldValue = m_scroll->getValue();

                // Pass the event, even when the mouse is not on top of the scrollbar
                m_scroll->mouseMoved(x, y);

                // If the value of the scrollbar has changed then update the text
                if (oldValue != m_scroll->getValue())
                    updateDisplayedText();
            }
            else // You are just moving the mouse
            {
                // When the mouse is on top of the scrollbar then pass the mouse move event
                if (m_scroll->mouseOnWidget(x, y))
                    m_scroll->mouseMoved(x, y);
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChatBox::mouseNotOnWidget()
    {
        if (m_mouseHover)
            mouseLeftWidget();

        if (m_scroll != nullptr)
            m_scroll->m_mouseHover = false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChatBox::mouseNoLongerDown()
    {
        m_mouseDown = false;

        if (m_scroll != nullptr)
            m_scroll->m_mouseDown = false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChatBox::mouseWheelMoved(int delta, int, int)
    {
        // Only do something when there is a scrollbar
        if (m_scroll != nullptr)
        {
            if (m_scroll->getLowValue() < m_scroll->getMaximum())
            {
                // Check if you are scrolling down
                if (delta < 0)
                {
                    // Scroll down
                    m_scroll->setValue(m_scroll->getValue() + (static_cast<unsigned int>(-delta) * m_textSize));
                }
                else // You are scrolling up
                {
                    unsigned int change = static_cast<unsigned int>(delta) * m_textSize;

                    // Scroll up
                    if (change < m_scroll->getValue())
                        m_scroll->setValue(m_scroll->getValue() - change);
                    else
                        m_scroll->setValue(0);
                }

                updateDisplayedText();
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChatBox::initialize(Container *const parent)
    {
        Widget::initialize(parent);

        if (!m_font && m_parent->getGlobalFont())
            setTextFont(m_parent->getGlobalFont());

        m_panel->initialize(m_parent);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    float ChatBox::getLineSpacing(const Label::Ptr& line)
    {
        // If a line spacing was manually set then just return that one
        if (m_lineSpacing > 0)
            return line->getSize().y + m_lineSpacing - line->getTextSize();

        float lineSpacing = line->getSize().y;
        if (line->getTextFont())
        {
            lineSpacing += line->getTextFont()->getLineSpacing(line->getTextSize()) - line->getTextSize();

            lineSpacing = std::max(lineSpacing, sf::Text{"kg", *line->getTextFont(), line->getTextSize()}.getLocalBounds().height);
        }

        if (lineSpacing > line->getSize().y)
            return lineSpacing;
        else
            return line->getSize().y + std::ceil(line->getTextSize() * 3.5f / 10.0f);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChatBox::recalculateFullTextHeight()
    {
        m_fullTextHeight = 0;
        if (!m_panel->getWidgets().empty())
        {
            for (std::size_t i = 0; i < m_panel->getWidgets().size(); ++i)
            {
                auto label = std::static_pointer_cast<Label>(m_panel->getWidgets()[i]);

                // Limit the width of the label
                if (m_scroll)
                    label->setMaximumTextWidth(m_panel->getSize().x - m_scroll->getSize().x);
                else
                    label->setMaximumTextWidth(m_panel->getSize().x);

                m_fullTextHeight += getLineSpacing(label);
            }

            // There should be no space below the bottom line
            auto lastLine = std::static_pointer_cast<Label>(m_panel->getWidgets().back());
            m_fullTextHeight -= getLineSpacing(lastLine) - lastLine->getSize().y;

            // Set the maximum of the scrollbar when there is one
            if (m_scroll != nullptr)
                m_scroll->setMaximum(static_cast<unsigned int>(m_fullTextHeight));
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChatBox::updateDisplayedText()
    {
        m_fullTextHeight = 0;
        if (!m_panel->getWidgets().empty())
        {
            float positionY = 0;
            if (m_scroll)
                positionY -= static_cast<float>(m_scroll->getValue());

            for (std::size_t i = 0; i < m_panel->getWidgets().size(); ++i)
            {
                auto label = std::static_pointer_cast<Label>(m_panel->getWidgets()[i]);

                // Not every line has the same height
                float positionFix = 0;
                if ((label->getTextFont()) && (label->getTextFont()->getGlyph('k', label->getTextSize(), false).bounds.height > label->getSize().y))
                    positionFix = label->getTextFont()->getGlyph('k', label->getTextSize(), false).bounds.height - label->getSize().y;

                label->setPosition({0, positionY + positionFix});
                positionY += getLineSpacing(label);
            }

            // Display the last lines when there is no scrollbar
            if (m_scroll == nullptr)
            {
                if (positionY > getSize().y)
                {
                    float diff = positionY - getSize().y;
                    for (auto it = m_panel->getWidgets().begin(); it != m_panel->getWidgets().end(); ++it)
                        (*it)->setPosition({(*it)->getPosition().x, (*it)->getPosition().y - diff});
                }
            }

            // Put the lines at the bottom of the chat box if needed
            if (!m_linesStartFromTop && (positionY < m_panel->getSize().y))
            {
                float diff = m_panel->getSize().y - positionY;
                for (auto it = m_panel->getWidgets().begin(); it != m_panel->getWidgets().end(); ++it)
                    (*it)->setPosition((*it)->getPosition().x, (*it)->getPosition().y + diff);
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChatBox::updateRendering()
    {
        Padding padding = getRenderer()->getPadding();
        Padding scaledPadding = padding;

        auto& texture = getRenderer()->m_backgroundTexture;
        if (texture.getData() != nullptr)
        {
            switch (texture.getScalingType())
            {
            case Texture::ScalingType::Normal:
                scaledPadding.left = padding.left * (texture.getSize().x / texture.getImageSize().x);
                scaledPadding.right = padding.right * (texture.getSize().x / texture.getImageSize().x);
                scaledPadding.top = padding.top * (texture.getSize().y / texture.getImageSize().y);
                scaledPadding.bottom = padding.bottom * (texture.getSize().y / texture.getImageSize().y);
                break;

            case Texture::ScalingType::Horizontal:
                scaledPadding.left = padding.left * (texture.getSize().y / texture.getImageSize().y);
                scaledPadding.right = padding.right * (texture.getSize().y / texture.getImageSize().y);
                scaledPadding.top = padding.top * (texture.getSize().y / texture.getImageSize().y);
                scaledPadding.bottom = padding.bottom * (texture.getSize().y / texture.getImageSize().y);
                break;

            case Texture::ScalingType::Vertical:
                scaledPadding.left = padding.left * (texture.getSize().x / texture.getImageSize().x);
                scaledPadding.right = padding.right * (texture.getSize().x / texture.getImageSize().x);
                scaledPadding.top = padding.top * (texture.getSize().x / texture.getImageSize().x);
                scaledPadding.bottom = padding.bottom * (texture.getSize().x / texture.getImageSize().x);
                break;

            case Texture::ScalingType::NineSliceScaling:
                break;
            }
        }

        m_panel->setPosition({getPosition().x + scaledPadding.left, getPosition().y + scaledPadding.top});
        m_panel->setSize({getSize().x - scaledPadding.left - scaledPadding.right, getSize().y - scaledPadding.top - scaledPadding.bottom});

        if (m_scroll)
        {
            m_scroll->setSize({m_scroll->getSize().x, m_panel->getSize().y});
            m_scroll->setLowValue(static_cast<unsigned int>(m_panel->getSize().y));
            m_scroll->setPosition({getPosition().x + getSize().x - m_scroll->getSize().x - scaledPadding.right, getPosition().y + scaledPadding.top});
        }

        updateDisplayedText();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChatBox::draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        // Draw the background
        getRenderer()->draw(target, states);

        // Draw the panel
        target.draw(*m_panel, states);

        // Draw the scrollbar if there is one
        if (m_scroll != nullptr)
            target.draw(*m_scroll, states);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChatBoxRenderer::setProperty(std::string property, const std::string& value, const std::string& rootPath)
    {
        if (property == "backgroundimage")
            extractTextureFromString(property, value, rootPath, m_backgroundTexture);
        else if (property == "backgroundcolor")
            setBackgroundColor(extractColorFromString(property, value));
        else if (property == "bordercolor")
            setBorderColor(extractColorFromString(property, value));
        else if (property == "borders")
            setBorders(extractBordersFromString(property, value));
        else if (property == "padding")
            setPadding(extractBordersFromString(property, value));
        else
            throw Exception{"Unrecognized property '" + property + "'."};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChatBoxRenderer::setBackgroundImage(const std::string& filename, const sf::IntRect& partRect, const sf::IntRect& middlePart, bool repeated)
    {
        if (filename != "")
            m_backgroundTexture.load(filename, partRect, middlePart, repeated);
        else
            m_backgroundTexture = {};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChatBoxRenderer::setScrollbar(const std::string& scrollbarThemeFileFilename, const std::string& section)
    {
        m_chatBox->m_scroll = Scrollbar::create(scrollbarThemeFileFilename, section);

        m_chatBox->m_scroll->setSize({m_chatBox->m_scroll->getSize().x, m_chatBox->getSize().y});
        m_chatBox->m_scroll->setLowValue(static_cast<unsigned int>(m_chatBox->getSize().y));

        m_chatBox->recalculateFullTextHeight();
        m_chatBox->updateDisplayedText();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChatBoxRenderer::setPadding(const Padding& padding)
    {
        WidgetPadding::setPadding(padding);

        m_chatBox->updateRendering();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChatBoxRenderer::draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        if (m_backgroundTexture.getData() == nullptr)
        {
            sf::RectangleShape background(m_chatBox->getSize());
            background.setPosition(m_chatBox->getPosition());
            background.setFillColor(m_backgroundColor);
            target.draw(background, states);
        }
        else
            target.draw(m_backgroundTexture, states);

        if (m_borders != Borders{0, 0, 0, 0})
        {
            sf::Vector2f position = m_chatBox->getPosition();
            sf::Vector2f size = m_chatBox->getSize();

            // Draw left border
            sf::RectangleShape border({m_borders.left, size.y + m_borders.top});
            border.setPosition({position.x - m_borders.left, position.y - m_borders.top});
            border.setFillColor(m_borderColor);
            target.draw(border, states);

            // Draw top border
            border.setSize({size.x + m_borders.right, m_borders.top});
            border.setPosition({position.x, position.y - m_borders.top});
            target.draw(border, states);

            // Draw right border
            border.setSize({m_borders.right, size.y + m_borders.bottom});
            border.setPosition({position.x + size.x, position.y});
            target.draw(border, states);

            // Draw bottom border
            border.setSize({size.x + m_borders.left, m_borders.bottom});
            border.setPosition({position.x - m_borders.left, position.y + size.y});
            target.draw(border, states);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::shared_ptr<WidgetRenderer> ChatBoxRenderer::clone(Widget* widget)
    {
        auto renderer = std::shared_ptr<ChatBoxRenderer>(new ChatBoxRenderer{*this});
        renderer->m_chatBox = static_cast<ChatBox*>(widget);
        return renderer;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
