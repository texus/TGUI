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


#include <TGUI/Widgets/Label.hpp>
#include <TGUI/Widgets/Panel.hpp>
#include <TGUI/Widgets/Scrollbar.hpp>
#include <TGUI/Widgets/ChatBox.hpp>
#include <TGUI/Loading/Theme.hpp>

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

        m_panel->setBackgroundColor(sf::Color::Transparent);

        m_renderer = std::make_shared<ChatBoxRenderer>(this);
        reload();

        setSize({200, 126});
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ChatBox::ChatBox(const ChatBox& chatBoxToCopy) :
        Widget               {chatBoxToCopy},
        m_textSize           {chatBoxToCopy.m_textSize},
        m_textColor          {chatBoxToCopy.m_textColor},
        m_maxLines           {chatBoxToCopy.m_maxLines},
        m_fullTextHeight     {chatBoxToCopy.m_fullTextHeight},
        m_linesStartFromTop  {chatBoxToCopy.m_linesStartFromTop},
        m_newLinesBelowOthers{chatBoxToCopy.m_newLinesBelowOthers},
        m_panel              {Panel::copy(chatBoxToCopy.m_panel)},
        m_scroll             {Scrollbar::copy(chatBoxToCopy.m_scroll)}
    {
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ChatBox& ChatBox::operator= (const ChatBox& right)
    {
        if (this != &right)
        {
            ChatBox temp{right};
            Widget::operator=(right);

            std::swap(m_textSize,            temp.m_textSize);
            std::swap(m_textColor,           temp.m_textColor);
            std::swap(m_maxLines,            temp.m_maxLines);
            std::swap(m_fullTextHeight,      temp.m_fullTextHeight);
            std::swap(m_linesStartFromTop,   temp.m_linesStartFromTop);
            std::swap(m_newLinesBelowOthers, temp.m_newLinesBelowOthers);
            std::swap(m_panel,               temp.m_panel);
            std::swap(m_scroll,              temp.m_scroll);
        }

        return *this;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ChatBox::Ptr ChatBox::copy(ChatBox::ConstPtr chatBox)
    {
        if (chatBox)
            return std::static_pointer_cast<ChatBox>(chatBox->clone());
        else
            return nullptr;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChatBox::setPosition(const Layout2d& position)
    {
        Widget::setPosition(position);

        getRenderer()->m_backgroundTexture.setPosition(getPosition());

        updateRendering();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChatBox::setSize(const Layout2d& size)
    {
        Widget::setSize(size);

        getRenderer()->m_backgroundTexture.setSize(getSize());

        updatePosition();
        recalculateFullTextHeight();
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

    void ChatBox::addLine(const sf::String& text, const sf::Color& color, unsigned int textSize, const Font& font)
    {
        // Remove the oldest line if you exceed the maximum
        if ((m_maxLines > 0) && (m_maxLines == m_panel->getWidgets().size()))
        {
            if (m_newLinesBelowOthers)
                removeLine(0);
            else
                removeLine(m_maxLines-1);
        }

        auto label = std::make_shared<Label>();
        label->getRenderer()->setTextColor(color);
        label->setTextSize(textSize);
        label->setText(text);
        m_panel->add(label);

        if (!m_newLinesBelowOthers)
            label->moveToBack();

        if (font.getFont())
            label->setFont(font.getFont());
        else
            label->setFont(getFont());

        recalculateFullTextHeight();

        if (m_scroll && (m_scroll->getMaximum() > m_scroll->getLowValue()))
            m_scroll->setValue(m_scroll->getMaximum() - m_scroll->getLowValue());

        updateDisplayedText();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    sf::String ChatBox::getLine(std::size_t lineIndex) const
    {
        if (lineIndex < m_panel->getWidgets().size())
        {
            return std::static_pointer_cast<Label>(m_panel->getWidgets()[lineIndex])->getText();
        }
        else // Index too high
            return "";
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    sf::Color ChatBox::getLineColor(std::size_t lineIndex) const
    {
        if (lineIndex < m_panel->getWidgets().size())
        {
            return std::static_pointer_cast<Label>(m_panel->getWidgets()[lineIndex])->getTextColor();
        }
        else // Index too high
            return m_textColor;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int ChatBox::getLineTextSize(std::size_t lineIndex) const
    {
        if (lineIndex < m_panel->getWidgets().size())
        {
            return std::static_pointer_cast<Label>(m_panel->getWidgets()[lineIndex])->getTextSize();
        }
        else // Index too high
            return m_textSize;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::shared_ptr<sf::Font> ChatBox::getLineFont(std::size_t lineIndex) const
    {
        if (lineIndex < m_panel->getWidgets().size())
        {
            return std::static_pointer_cast<Label>(m_panel->getWidgets()[lineIndex])->getFont();
        }
        else // Index too high
            return getFont();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ChatBox::removeLine(std::size_t lineIndex)
    {
        if (lineIndex < m_panel->getWidgets().size())
        {
            m_panel->remove(m_panel->getWidgets()[lineIndex]);

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

        // Remove the oldest lines if there are too many lines
        if ((m_maxLines > 0) && (m_maxLines < m_panel->getWidgets().size()))
        {
            while (m_maxLines < m_panel->getWidgets().size())
            {
                if (m_newLinesBelowOthers)
                    m_panel->remove(m_panel->getWidgets()[0]);
                else
                    m_panel->remove(m_panel->getWidgets().back());
            }

            recalculateFullTextHeight();
            updateDisplayedText();
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::size_t ChatBox::getLineLimit()
    {
        return m_maxLines;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChatBox::setFont(const Font& font)
    {
        Widget::setFont(font);

        bool lineChanged = false;
        for (auto& label : m_panel->getWidgets())
        {
            auto line = std::static_pointer_cast<Label>(label);
            if (line->getFont() == nullptr)
            {
                line->setFont(font);
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

    void ChatBox::setTextColor(const Color& color)
    {
        m_textColor = color;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChatBox::setScrollbar(Scrollbar::Ptr scrollbar)
    {
        m_scroll = scrollbar;

        if (m_scroll)
        {
            m_scroll->setSize({m_scroll->getSize().x, m_panel->getSize().y});
            m_scroll->setLowValue(static_cast<unsigned int>(m_panel->getSize().y));
        }

        recalculateFullTextHeight();
        updateDisplayedText();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Scrollbar::Ptr ChatBox::getScrollbar() const
    {
        return m_scroll;
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

    bool ChatBox::getLinesStartFromTop() const
    {
        return m_linesStartFromTop;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChatBox::setNewLinesBelowOthers(bool newLinesBelowOthers)
    {
        m_newLinesBelowOthers = newLinesBelowOthers;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ChatBox::getNewLinesBelowOthers() const
    {
        return m_newLinesBelowOthers;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChatBox::setOpacity(float opacity)
    {
        Widget::setOpacity(opacity);

        m_panel->setOpacity(m_opacity);

        if (m_scroll != nullptr)
            m_scroll->setOpacity(m_opacity);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    sf::Vector2f ChatBox::getWidgetOffset() const
    {
        return {getRenderer()->getBorders().left, getRenderer()->getBorders().top};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChatBox::setParent(Container* parent)
    {
        Widget::setParent(parent);
        m_panel->setParent(parent);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ChatBox::mouseOnWidget(float x, float y) const
    {
        return sf::FloatRect{getPosition().x, getPosition().y, getSize().x, getSize().y}.contains(x, y);
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
        if (m_scroll && getFont())
        {
            // Only pass the event when the scrollbar still thinks the mouse is down
            if (m_scroll->m_mouseDown)
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
                        m_scroll->setValue(static_cast<unsigned int>(m_scroll->getValue() + getFont()->getLineSpacing(m_textSize) - (std::fmod(m_scroll->getValue(), getFont()->getLineSpacing(m_textSize)))));
                    }
                    else if (m_scroll->getValue() == oldValue - 1) // Check if the scrollbar value was decremented (you have pressed on the up arrow)
                    {
                        // increment the value
                        m_scroll->setValue(m_scroll->getValue()+1);

                        // Scroll up with the whole item height instead of with a single pixel
                        if (std::fmod(m_scroll->getValue(), getFont()->getLineSpacing(m_textSize)) > 0)
                            m_scroll->setValue(static_cast<unsigned int>(m_scroll->getValue() - std::fmod(m_scroll->getValue(), getFont()->getLineSpacing(m_textSize))));
                        else
                            m_scroll->setValue(static_cast<unsigned int>(m_scroll->getValue() - getFont()->getLineSpacing(m_textSize)));
                    }

                    updateDisplayedText();
                }
            }
        }
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

    void ChatBox::mouseNoLongerOnWidget()
    {
        if (m_mouseHover)
            mouseLeftWidget();

        if (m_scroll != nullptr)
            m_scroll->m_mouseHover = false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChatBox::mouseNoLongerDown()
    {
        Widget::mouseNoLongerDown();

        if (m_scroll != nullptr)
            m_scroll->m_mouseDown = false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChatBox::mouseWheelMoved(int delta, int x, int y)
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
                    m_scroll->setValue(static_cast<unsigned int>(m_scroll->getValue() + ((-delta) * getFont()->getLineSpacing(m_textSize))));
                }
                else // You are scrolling up
                {
                    unsigned int change = static_cast<unsigned int>(delta * getFont()->getLineSpacing(m_textSize));

                    // Scroll up
                    if (change < m_scroll->getValue())
                        m_scroll->setValue(m_scroll->getValue() - change);
                    else
                        m_scroll->setValue(0);
                }

                updateDisplayedText();
                mouseMoved(static_cast<float>(x), static_cast<float>(y));
            }
        }
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

                m_fullTextHeight += label->getSize().y;
            }

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

                label->setPosition({0, positionY});
                positionY += label->getSize().y;
            }

            // Display the last lines when there is no scrollbar
            if (!m_scroll)
            {
                if (positionY > m_panel->getSize().y)
                {
                    float diff = positionY - m_panel->getSize().y;
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
        if (texture.isLoaded())
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

            case Texture::ScalingType::NineSlice:
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

    void ChatBox::reload(const std::string& primary, const std::string& secondary, bool force)
    {
        getRenderer()->setBorders({2, 2, 2, 2});
        getRenderer()->setPadding({2, 2, 2, 2});
        getRenderer()->setBackgroundColor({245, 245, 245});
        getRenderer()->setBorderColor({0, 0, 0});
        getRenderer()->setBackgroundTexture({});

        if (m_theme && primary != "")
        {
            getRenderer()->setBorders({0, 0, 0, 0});
            getRenderer()->setPadding({0, 0, 0, 0});

            Widget::reload(primary, secondary, force);

            if (force)
            {
                if (getRenderer()->m_backgroundTexture.isLoaded())
                    setSize(getRenderer()->m_backgroundTexture.getImageSize());
            }

            updateSize();
        }
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

    void ChatBoxRenderer::setProperty(std::string property, const std::string& value)
    {
        property = toLower(property);

        if (property == "borders")
            setBorders(Deserializer::deserialize(ObjectConverter::Type::Borders, value).getBorders());
        else if (property == "padding")
            setPadding(Deserializer::deserialize(ObjectConverter::Type::Borders, value).getBorders());
        else if (property == "backgroundcolor")
            setBackgroundColor(Deserializer::deserialize(ObjectConverter::Type::Color, value).getColor());
        else if (property == "bordercolor")
            setBorderColor(Deserializer::deserialize(ObjectConverter::Type::Color, value).getColor());
        else if (property == "backgroundimage")
            setBackgroundTexture(Deserializer::deserialize(ObjectConverter::Type::Texture, value).getTexture());
        else if (property == "scrollbar")
        {
            if (toLower(value) == "none")
                m_chatBox->setScrollbar(nullptr);
            else
            {
                if (m_chatBox->getTheme() == nullptr)
                    throw Exception{"Failed to load scrollbar, ChatBox has no connected theme to load the scrollbar with"};

                m_chatBox->setScrollbar(m_chatBox->getTheme()->internalLoad(m_chatBox->m_primaryLoadingParameter,
                                                                            Deserializer::deserialize(ObjectConverter::Type::String, value).getString()));
            }
        }
        else
            WidgetRenderer::setProperty(property, value);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChatBoxRenderer::setProperty(std::string property, ObjectConverter&& value)
    {
        property = toLower(property);

        if (value.getType() == ObjectConverter::Type::Borders)
        {
            if (property == "borders")
                setBorders(value.getBorders());
            else if (property == "padding")
                setPadding(value.getBorders());
            else
                return WidgetRenderer::setProperty(property, std::move(value));
        }
        else if (value.getType() == ObjectConverter::Type::Color)
        {
            if (property == "backgroundcolor")
                setBackgroundColor(value.getColor());
            else if (property == "bordercolor")
                setBorderColor(value.getColor());
            else
                WidgetRenderer::setProperty(property, std::move(value));
        }
        else if (value.getType() == ObjectConverter::Type::Texture)
        {
            if (property == "backgroundimage")
                setBackgroundTexture(value.getTexture());
            else
                WidgetRenderer::setProperty(property, std::move(value));
        }
        else if (value.getType() == ObjectConverter::Type::String)
        {
            if (property == "scrollbar")
            {
                if (toLower(value.getString()) == "none")
                    m_chatBox->setScrollbar(nullptr);
                else
                {
                    if (m_chatBox->getTheme() == nullptr)
                        throw Exception{"Failed to load scrollbar, ChatBox has no connected theme to load the scrollbar with"};

                    m_chatBox->setScrollbar(m_chatBox->getTheme()->internalLoad(m_chatBox->getPrimaryLoadingParameter(), value.getString()));
                }
            }
        }
        else
            WidgetRenderer::setProperty(property, std::move(value));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ObjectConverter ChatBoxRenderer::getProperty(std::string property) const
    {
        property = toLower(property);

        if (property == "borders")
            return m_borders;
        else if (property == "padding")
            return m_padding;
        else if (property == "backgroundcolor")
            return m_backgroundColor;
        else if (property == "bordercolor")
            return m_borderColor;
        else if (property == "backgroundimage")
            return m_backgroundTexture;
        else
            return WidgetRenderer::getProperty(property);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::map<std::string, ObjectConverter> ChatBoxRenderer::getPropertyValuePairs() const
    {
        auto pairs = WidgetRenderer::getPropertyValuePairs();

        if (m_backgroundTexture.isLoaded())
            pairs["BackgroundImage"] = m_backgroundTexture;
        else
            pairs["BackgroundColor"] = m_backgroundColor;

        pairs["BorderColor"] = m_borderColor;
        pairs["Borders"] = m_borders;
        pairs["Padding"] = m_padding;
        return pairs;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChatBoxRenderer::setBorderColor(const Color& borderColor)
    {
        m_borderColor = borderColor;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChatBoxRenderer::setBackgroundColor(const Color& backgroundColor)
    {
        m_backgroundColor = backgroundColor;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChatBoxRenderer::setBackgroundTexture(const Texture& texture)
    {
        m_backgroundTexture = texture;
        if (m_backgroundTexture.isLoaded())
        {
            m_backgroundTexture.setPosition(m_chatBox->getPosition());
            m_backgroundTexture.setSize(m_chatBox->getSize());
            m_backgroundTexture.setColor({255, 255, 255, static_cast<sf::Uint8>(m_chatBox->getOpacity() * 255)});
        }
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
        if (m_backgroundTexture.isLoaded())
            target.draw(m_backgroundTexture, states);
        else
        {
            sf::RectangleShape background(m_chatBox->getSize());
            background.setPosition(m_chatBox->getPosition());
            background.setFillColor(calcColorOpacity(m_backgroundColor, m_chatBox->getOpacity()));
            target.draw(background, states);
        }

        if (m_borders != Borders{0, 0, 0, 0})
        {
            sf::Vector2f position = m_chatBox->getPosition();
            sf::Vector2f size = m_chatBox->getSize();

            // Draw left border
            sf::RectangleShape border({m_borders.left, size.y + m_borders.top});
            border.setPosition({position.x - m_borders.left, position.y - m_borders.top});
            border.setFillColor(calcColorOpacity(m_borderColor, m_chatBox->getOpacity()));
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
        auto renderer = std::make_shared<ChatBoxRenderer>(*this);
        renderer->m_chatBox = static_cast<ChatBox*>(widget);
        return renderer;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
