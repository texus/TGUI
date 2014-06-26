/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus's Graphical User Interface
// Copyright (C) 2012-2014 Bruno Van de Velde (vdv_b@tgui.eu)
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
#include <TGUI/SharedWidgetPtr.inl>
#include <TGUI/ChatBox.hpp>

#include <cmath>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ChatBox::ChatBox()
    {
        m_callback.widgetType = Type_ChatBox;
        m_draggableWidget = true;

        m_panel = new Panel();
        m_panel->setSize({360, 200});
        m_panel->setBackgroundColor(sf::Color::White);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ChatBox::ChatBox(const ChatBox& copy) :
        Widget            {copy},
        WidgetBorders     {copy},
        m_loadedConfigFile{copy.m_loadedConfigFile},
        m_lineSpacing     {copy.m_lineSpacing},
        m_textSize        {copy.m_textSize},
        m_textColor       {copy.m_textColor},
        m_borderColor     {copy.m_borderColor},
        m_maxLines        {copy.m_maxLines},
        m_fullTextHeight  {copy.m_fullTextHeight}
    {
        m_panel = new Panel(*copy.m_panel);

        // If there is a scrollbar then copy it
        if (copy.m_scroll != nullptr)
            m_scroll = new Scrollbar(*copy.m_scroll);
        else
            m_scroll = nullptr;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// TODO: Get rid of manual memory managment
/**
    ChatBox::~ChatBox()
    {
        delete m_panel;

        if (m_scroll != nullptr)
            delete m_scroll;
    }
*/
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ChatBox& ChatBox::operator= (const ChatBox& right)
    {
        if (this != &right)
        {
            // If there already was a scrollbar then delete it now
            if (m_scroll != nullptr)
            {
                delete m_scroll;
                m_scroll = nullptr;
            }

            ChatBox temp{right};
            Widget::operator=(right);
            WidgetBorders::operator=(right);

            std::swap(m_loadedConfigFile, temp.m_loadedConfigFile);
            std::swap(m_lineSpacing,      temp.m_lineSpacing);
            std::swap(m_textSize,         temp.m_textSize);
            std::swap(m_textColor,        temp.m_textColor);
            std::swap(m_borderColor,      temp.m_borderColor);
            std::swap(m_maxLines,         temp.m_maxLines);
            std::swap(m_fullTextHeight,   temp.m_fullTextHeight);
            std::swap(m_panel,            temp.m_panel);
            std::swap(m_scroll,           temp.m_scroll);
        }

        return *this;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChatBox::load(const std::string& configFileFilename)
    {
        m_loadedConfigFile = getResourcePath() + configFileFilename;

        // If there already was a scrollbar then delete it now
        if (m_scroll != nullptr)
        {
            delete m_scroll;
            m_scroll = nullptr;
        }

        // Open the config file
        ConfigFile configFile{m_loadedConfigFile, "ChatBox"};

        // Find the folder that contains the config file
        std::string configFileFolder = "";
        std::string::size_type slashPos = configFileFilename.find_last_of("/\\");
        if (slashPos != std::string::npos)
            configFileFolder = configFileFilename.substr(0, slashPos+1);

        // Handle the read properties
        for (auto it = configFile.getProperties().cbegin(); it != configFile.getProperties().cend(); ++it)
        {
            if (it->first == "backgroundcolor")
            {
                setBackgroundColor(configFile.readColor(it));
            }
            else if (it->first == "bordercolor")
            {
                setBorderColor(configFile.readColor(it));
            }
            else if (it->first == "borders")
            {
                Borders borders;
                if (extractBorders(it->second, borders))
                    setBorders(borders);
                else
                    throw Exception("Failed to parse the 'Borders' property in section ChatBox in " + m_loadedConfigFile);
            }
            else if (it->first == "scrollbar")
            {
                if ((it->second.length() < 3) || (it->second[0] != '"') || (it->second[it->second.length()-1] != '"'))
                    throw Exception("Failed to parse value for Scrollbar property in section ChatBox in " + m_loadedConfigFile + ".");

                // load the scrollbar and check if it failed
                m_scroll = new Scrollbar();
                try {
                    m_scroll->load(configFileFolder + it->second.substr(1, it->second.length()-2));
                }
                catch (Exception& e)
                {
                    delete m_scroll;
                    m_scroll = nullptr;
                    throw e;
                }

                // Initialize the scrollbar
                m_scroll->setVerticalScroll(true);
                m_scroll->setLowValue(static_cast<unsigned int>(getSize().y));
                m_scroll->setSize(m_scroll->getSize());
                m_scroll->setMaximum(static_cast<unsigned int>(m_fullTextHeight));
            }
            else
                throw Exception("Unrecognized property '" + it->first + "' in section ChatBox in " + m_loadedConfigFile + ".");
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChatBox::setSize(const Layout& size)
    {
        // Remember the old height
        float oldHeight = getSize().y;

        Widget::setSize(size);

        m_panel->setSize(getSize());

        // If there is a scrollbar then reinitialize it
        if (m_scroll != nullptr)
        {
            m_scroll->setLowValue(static_cast<unsigned int>(getSize().y));
            m_scroll->setSize({m_scroll->getSize().x, getSize().y});
        }

        // Find out how much the height has changed
        float heightDiff = getSize().y - oldHeight;

        // Reposition all labels in the chatbox
        auto& labels = m_panel->getWidgets();
        for (auto it = labels.begin(); it != labels.end(); ++it)
            (*it)->setPosition({(*it)->getPosition().x, (*it)->getPosition().y + heightDiff});
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

    void ChatBox::addLine(const sf::String& text, const sf::Color& color, unsigned int textSize, const sf::Font* font)
    {
        if (m_panel->getGlobalFont() == nullptr && font == nullptr)
            throw Exception("ChatBox::addLine called while no valid font was set.");

        auto& widgets = m_panel->getWidgets();

        // Remove the top line if you exceed the maximum
        if ((m_maxLines > 0) && (m_maxLines < widgets.size() + 1))
            removeLine(0);

        Label::Ptr label(*m_panel);
        label->setTextColor(color);
        label->setTextSize(textSize);

        if (font != nullptr)
            label->setTextFont(*font);

        Label::Ptr tempLine;
        tempLine->setTextSize(textSize);
        tempLine->setTextFont(*label->getTextFont());

        float width = getSize().x;
        if (m_scroll)
            width -= m_scroll->getSize().x;

        if (width < 0)
            width = 0;

        // Split the label over multiple lines if necessary
        unsigned int pos = 0;
        unsigned int size = 0;
        while (pos + size < text.getSize())
        {
            tempLine->setText(text.toWideString().substr(pos, ++size));

            if (tempLine->getSize().x + 4.0f > width)
            {
                label->setText(label->getText() + text.toWideString().substr(pos, size - 1) + "\n");

                pos = pos + size - 1;
                size = 0;
            }
        }
        label->setText(label->getText() + tempLine->getText());

        m_fullTextHeight += getLineSpacing(widgets.size()-1);

        if (m_scroll != nullptr)
        {
            m_scroll->setMaximum(static_cast<unsigned int>(m_fullTextHeight));

            if (m_scroll->getMaximum() > m_scroll->getLowValue())
                m_scroll->setValue(m_scroll->getMaximum() - m_scroll->getLowValue());
        }

        // Reposition the labels
        updateDisplayedText();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    sf::String ChatBox::getLine(unsigned int lineIndex)
    {
        if (lineIndex < m_panel->getWidgets().size())
        {
            return tgui::Label::Ptr(m_panel->getWidgets()[lineIndex])->getText();
        }
        else // Index too high
            return "";
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ChatBox::removeLine(unsigned int lineIndex)
    {
        if (lineIndex < m_panel->getWidgets().size())
        {
            tgui::Label::Ptr label = m_panel->getWidgets()[lineIndex];
            m_fullTextHeight -= getLineSpacing(lineIndex);
            m_panel->remove(label);

            if (m_scroll != nullptr)
                m_scroll->setMaximum(static_cast<unsigned int>(m_fullTextHeight));

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

        m_fullTextHeight = 0;

        if (m_scroll != nullptr)
            m_scroll->setMaximum(static_cast<unsigned int>(m_fullTextHeight));

        updateDisplayedText();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChatBox::setLineLimit(unsigned int maxLines)
    {
        m_maxLines = maxLines;

        if ((m_maxLines > 0) && (m_maxLines < m_panel->getWidgets().size()))
        {
            while (m_maxLines < m_panel->getWidgets().size())
            {
                m_fullTextHeight -= getLineSpacing(0);
                m_panel->remove(m_panel->getWidgets()[0]);
            }

            if (m_scroll != nullptr)
                m_scroll->setMaximum(static_cast<unsigned int>(m_fullTextHeight));

            updateDisplayedText();
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChatBox::setTextFont(const sf::Font& font)
    {
        m_panel->setGlobalFont(font);

        m_fullTextHeight = 0;
        auto& labels = m_panel->getWidgets();
        for (unsigned int i = 0; i < labels.size(); ++i)
        {
            Label::Ptr(labels[i])->setTextFont(font);
            m_fullTextHeight += getLineSpacing(i);
        }

        if (m_scroll != nullptr)
        {
            m_scroll->setMaximum(static_cast<unsigned int>(m_fullTextHeight));

            if (m_scroll->getMaximum() > m_scroll->getLowValue())
                m_scroll->setValue(m_scroll->getMaximum() - m_scroll->getLowValue());
        }

        updateDisplayedText();
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
        m_lineSpacing = lineSpacing;

        updateDisplayedText();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChatBox::setScrollbar(const std::string& scrollbarConfigFileFilename)
    {
        // If the scrollbar was already created then delete it first
        if (m_scroll != nullptr)
            delete m_scroll;

        // load the scrollbar and check if it failed
        m_scroll = new Scrollbar();
        try {
            m_scroll->load(scrollbarConfigFileFilename);
        }
        catch (Exception& e)
        {
            delete m_scroll;
            m_scroll = nullptr;
            throw e;
        }

        // Initialize the scrollbar
        m_scroll->setVerticalScroll(true);
        m_scroll->setSize({m_scroll->getSize().x, getSize().y});
        m_scroll->setLowValue(static_cast<unsigned int>(getSize().y));
        m_scroll->setMaximum(static_cast<unsigned int>(m_fullTextHeight));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChatBox::removeScrollbar()
    {
        // Delete the scrollbar
        delete m_scroll;
        m_scroll = nullptr;
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
        {
            // Temporarily set the position of the scroll
            m_scroll->setPosition({getPosition().x + getSize().x - m_scroll->getSize().x, getPosition().y});

            // Pass the event
            m_scroll->mouseOnWidget(x, y);

            // Reset the position
            m_scroll->setPosition({0, 0});
        }

        // Check if the mouse is on top of the list box
        if (getTransform().transformRect(sf::FloatRect(0, 0, getSize().x, getSize().y)).contains(x, y))
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

            // Temporarily set the position of the scroll
            m_scroll->setPosition({getPosition().x + getSize().x - m_scroll->getSize().x, getPosition().y});

            // Pass the event
            if (m_scroll->mouseOnWidget(x, y))
                m_scroll->leftMousePressed(x, y);

            // Reset the position
            m_scroll->setPosition({0, 0});

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

                // Temporarily set the position of the scroll
                m_scroll->setPosition({getPosition().x + getSize().x - m_scroll->getSize().x, getPosition().y});

                // Pass the event
                m_scroll->leftMouseReleased(x, y);

                // Reset the position
                m_scroll->setPosition({0, 0});

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
        if (m_mouseHover == false)
            mouseEnteredWidget();

        // Set the mouse move flag
        m_mouseHover = true;

        // If there is a scrollbar then pass the event
        if (m_scroll != nullptr)
        {
            // Temporarily set the position of the scroll
            m_scroll->setPosition({getPosition().x + getSize().x - m_scroll->getSize().x, getPosition().y});

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

            // Reset the position
            m_scroll->setPosition({0, 0});
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChatBox::mouseNotOnWidget()
    {
        if (m_mouseHover)
            mouseLeftWidget();

        m_mouseHover = false;

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

    void ChatBox::setProperty(std::string property, const std::string& value)
    {
        property = toLower(property);

        if (property == "configfile")
        {
            load(value);
        }
        else if (property == "textsize")
        {
            setTextSize(tgui::stoi(value));
        }
        else if (property == "borders")
        {
            Borders borders;
            if (extractBorders(value, borders))
                setBorders(borders);
            else
                throw Exception("Failed to parse 'Borders' property.");
        }
        else if (property == "backgroundcolor")
        {
            setBackgroundColor(extractColor(value));
        }
        else if (property == "bordercolor")
        {
            setBorderColor(extractColor(value));
        }
        else if (property == "lines")
        {
            removeAllLines();

            std::vector<sf::String> lines;
            decodeList(value, lines);

            for (auto it = lines.cbegin(); it != lines.cend(); ++it)
            {
                std::string line = *it;

                if ((line.length() >= 2) && (line[0] == '(' && line[line.length()-1] == ')'))
                {
                    line.erase(0, 1);
                    line.erase(line.length()-1, 1);

                    std::string::size_type openBracketPos = line.rfind('(');
                    std::string::size_type closeBracketPos = line.rfind(')');

                    if ((openBracketPos == std::string::npos) || (closeBracketPos == std::string::npos) || (openBracketPos >= closeBracketPos))
                        throw Exception("Could not decode Lines property: failed to match brackets.");

                    sf::Color color = extractColor(line.substr(openBracketPos, closeBracketPos - openBracketPos + 1));

                    std::string::size_type commaPos = line.rfind(',', openBracketPos);
                    if (commaPos == std::string::npos)
                        throw Exception("Could not decode Lines property: failed to find comma.");

                    line.erase(commaPos);

                    addLine(line, color);
                }
                else
                    throw Exception("Could not decode Lines property: failed to match brackets.");
            }
        }
        else // The property didn't match
            Widget::setProperty(property, value);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChatBox::getProperty(std::string property, std::string& value) const
    {
        property = toLower(property);

        if (property == "configfile")
            value = getLoadedConfigFile();
        else if (property == "textsize")
            value = tgui::to_string(getTextSize());
        else if (property == "borders")
            value = "(" + tgui::to_string(getBorders().left) + "," + tgui::to_string(getBorders().top) + "," + tgui::to_string(getBorders().right) + "," + tgui::to_string(getBorders().bottom) + ")";
        else if (property == "backgroundcolor")
            value = "(" + tgui::to_string(int(getBackgroundColor().r)) + "," + tgui::to_string(int(getBackgroundColor().g)) + "," + tgui::to_string(int(getBackgroundColor().b)) + "," + tgui::to_string(int(getBackgroundColor().a)) + ")";
        else if (property == "bordercolor")
            value = "(" + tgui::to_string(int(getBorderColor().r)) + "," + tgui::to_string(int(getBorderColor().g)) + "," + tgui::to_string(int(getBorderColor().b)) + "," + tgui::to_string(int(getBorderColor().a)) + ")";
        else if (property == "lines")
        {
            std::vector<sf::String> lines;
            std::vector<Widget::Ptr>& labels = m_panel->getWidgets();

            for (auto it = labels.cbegin(); it != labels.cend(); ++it)
                lines.push_back("(" + Label::Ptr(*it)->getText() + "," + convertColorToString(Label::Ptr(*it)->getTextColor()) + ")");

            encodeList(lines, value);
        }
        else // The property didn't match
            Widget::getProperty(property, value);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::list< std::pair<std::string, std::string> > ChatBox::getPropertyList() const
    {
        auto list = Widget::getPropertyList();
        list.push_back(std::pair<std::string, std::string>("ConfigFile", "string"));
        list.push_back(std::pair<std::string, std::string>("TextSize", "uint"));
        list.push_back(std::pair<std::string, std::string>("Borders", "borders"));
        list.push_back(std::pair<std::string, std::string>("BackgroundColor", "color"));
        list.push_back(std::pair<std::string, std::string>("BorderColor", "color"));
        list.push_back(std::pair<std::string, std::string>("Lines", "string"));
        return list;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int ChatBox::getLineSpacing(unsigned int lineNumber)
    {
        assert(lineNumber < m_panel->getWidgets().size());

        auto line = tgui::Label::Ptr(m_panel->getWidgets()[lineNumber]);

        // Count the amount of lines that the label is taking
        std::string lineText = line->getText().toAnsiString();
        int linesOfText = std::count(lineText.begin(), lineText.end(), '\n') + 1;

        // If a line spacing was manually set then just return that one
        if (m_lineSpacing > 0)
            return m_lineSpacing * linesOfText;

        unsigned int lineSpacing;
        if (m_panel->getGlobalFont())
            lineSpacing = m_panel->getGlobalFont()->getLineSpacing(line->getTextSize());
        else
            lineSpacing = 0;

        if (lineSpacing > line->getTextSize())
            return lineSpacing * linesOfText;
        else
            return static_cast<unsigned int>(std::ceil(line->getTextSize() * 13.5 / 10.0) * linesOfText);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChatBox::initialize(Container *const parent)
    {
        Widget::initialize(parent);

        if (!getTextFont() && m_parent->getGlobalFont())
            setTextFont(*m_parent->getGlobalFont());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChatBox::updateDisplayedText()
    {
        float position = 2.0f;
        if (m_scroll)
            position -= static_cast<float>(m_scroll->getValue());

        auto& labels = m_panel->getWidgets();
        for (unsigned int i = 0; i < labels.size(); ++i)
        {
            tgui::Label::Ptr label = labels[i];
            label->setPosition({2.0f, position});

            position += getLineSpacing(i);
        }

        // Correct the position when there is no scrollbar
        if ((m_scroll == nullptr) && (!labels.empty()))
        {
            tgui::Label::Ptr label = labels.back();
            if (position > getSize().y)
            {
                float diff = position - getSize().y;
                for (auto it = labels.begin(); it != labels.end(); ++it)
                    (*it)->setPosition({(*it)->getPosition().x, (*it)->getPosition().y - diff});
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChatBox::draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        m_panel->setPosition(getAbsolutePosition());

        // Draw the panel
        target.draw(*m_panel);

        // Adjust the transformation
        states.transform *= getTransform();

        // Draw left border
        sf::RectangleShape border({m_borders.left, getSize().y + m_borders.top});
        border.setPosition(-m_borders.left, -m_borders.top);
        border.setFillColor(m_borderColor);
        target.draw(border, states);

        // Draw top border
        border.setSize({getSize().x + m_borders.right, m_borders.top});
        border.setPosition(0, -m_borders.top);
        target.draw(border, states);

        // Draw right border
        border.setSize({m_borders.right, getSize().y + m_borders.bottom});
        border.setPosition(getSize().x, 0);
        target.draw(border, states);

        // Draw bottom border
        border.setSize({getSize().x + m_borders.left, m_borders.bottom});
        border.setPosition(-m_borders.left, getSize().y);
        target.draw(border, states);

        // Check if there is a scrollbar
        if (m_scroll != nullptr)
        {
            // Draw the scrollbar
            states.transform.translate(getSize().x - m_scroll->getSize().x, 0);
            target.draw(*m_scroll, states);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
