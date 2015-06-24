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
#include <TGUI/SharedWidgetPtr.inl>
#include <TGUI/ChatBox.hpp>

#include <cmath>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ChatBox::ChatBox() :
        m_LineSpacing         (0),
        m_TextSize            (16),
        m_TextColor           (sf::Color::Black),
        m_BorderColor         (sf::Color::Black),
        m_MaxLines            (0),
        m_FullTextHeight      (0),
        m_LinesStartFromBottom(false),
        m_Scroll              (nullptr)
    {
        m_Callback.widgetType = Type_ChatBox;
        m_DraggableWidget = true;

        m_Panel = new Panel();
        m_Panel->setSize(360, 200);
        m_Panel->setBackgroundColor(sf::Color::White);

        // Load the chat box with default values
        m_Loaded = true;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ChatBox::ChatBox(const ChatBox& copy) :
        Widget                (copy),
        WidgetBorders         (copy),
        m_LoadedConfigFile    (copy.m_LoadedConfigFile),
        m_LineSpacing         (copy.m_LineSpacing),
        m_TextSize            (copy.m_TextSize),
        m_TextColor           (copy.m_TextColor),
        m_BorderColor         (copy.m_BorderColor),
        m_MaxLines            (copy.m_MaxLines),
        m_FullTextHeight      (copy.m_FullTextHeight),
        m_LinesStartFromBottom(copy.m_LinesStartFromBottom)
    {
        m_Panel = new Panel(*copy.m_Panel);

        // If there is a scrollbar then copy it
        if (copy.m_Scroll != nullptr)
            m_Scroll = new Scrollbar(*copy.m_Scroll);
        else
            m_Scroll = nullptr;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ChatBox::~ChatBox()
    {
        delete m_Panel;

        if (m_Scroll != nullptr)
            delete m_Scroll;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ChatBox& ChatBox::operator= (const ChatBox& right)
    {
        if (this != &right)
        {
            // If there already was a scrollbar then delete it now
            if (m_Scroll != nullptr)
            {
                delete m_Scroll;
                m_Scroll = nullptr;
            }

            ChatBox temp(right);
            this->Widget::operator=(right);
            this->WidgetBorders::operator=(right);

            std::swap(m_LoadedConfigFile,     temp.m_LoadedConfigFile);
            std::swap(m_LineSpacing,          temp.m_LineSpacing);
            std::swap(m_TextSize,             temp.m_TextSize);
            std::swap(m_TextColor,            temp.m_TextColor);
            std::swap(m_BorderColor,          temp.m_BorderColor);
            std::swap(m_MaxLines,             temp.m_MaxLines);
            std::swap(m_FullTextHeight,       temp.m_FullTextHeight);
            std::swap(m_LinesStartFromBottom, temp.m_LinesStartFromBottom);
            std::swap(m_Panel,                temp.m_Panel);
            std::swap(m_Scroll,               temp.m_Scroll);
        }

        return *this;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ChatBox* ChatBox::clone()
    {
        return new ChatBox(*this);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ChatBox::load(const std::string& configFileFilename, const std::string& sectionName)
    {
        m_LoadedConfigFile = getResourcePath() + configFileFilename;

        // When everything is loaded successfully, this will become true.
        m_Loaded = false;

        // If there already was a scrollbar then delete it now
        if (m_Scroll != nullptr)
        {
            delete m_Scroll;
            m_Scroll = nullptr;
        }

        // Open the config file
        ConfigFile configFile;
        if (!configFile.open(m_LoadedConfigFile))
        {
            TGUI_OUTPUT("TGUI error: Failed to open " + m_LoadedConfigFile + ".");
            return false;
        }

        // Read the properties and their values (as strings)
        std::vector<std::string> properties;
        std::vector<std::string> values;
        if (!configFile.read(sectionName, properties, values))
        {
            TGUI_OUTPUT("TGUI error: Failed to parse " + m_LoadedConfigFile + ".");
            return false;
        }

        // Close the config file
        configFile.close();

        // Find the folder that contains the config file
        std::string configFileFolder = "";
        std::string::size_type slashPos = configFileFilename.find_last_of("/\\");
        if (slashPos != std::string::npos)
            configFileFolder = configFileFilename.substr(0, slashPos+1);

        // Handle the read properties
        for (unsigned int i = 0; i < properties.size(); ++i)
        {
            std::string property = properties[i];
            std::string value = values[i];

            if (property == "backgroundcolor")
            {
                setBackgroundColor(configFile.readColor(value));
            }
            else if (property == "bordercolor")
            {
                setBorderColor(configFile.readColor(value));
            }
            else if (property == "borders")
            {
                Borders borders;
                if (extractBorders(value, borders))
                    setBorders(borders.left, borders.top, borders.right, borders.bottom);
            }
            else if (property == "scrollbar")
            {
                if ((value.length() < 3) || (value[0] != '"') || (value[value.length()-1] != '"'))
                {
                    TGUI_OUTPUT("TGUI error: Failed to parse value for Scrollbar in section ChatBox in " + m_LoadedConfigFile + ".");
                    return false;
                }

                // load the scrollbar and check if it failed
                m_Scroll = new Scrollbar();
                if (m_Scroll->load(configFileFolder + value.substr(1, value.length()-2)) == false)
                {
                    // The scrollbar couldn't be loaded so it must be deleted
                    delete m_Scroll;
                    m_Scroll = nullptr;

                    return false;
                }
                else // The scrollbar was loaded successfully
                {
                    // Initialize the scrollbar
                    m_Scroll->setVerticalScroll(true);
                    m_Scroll->setLowValue(static_cast<unsigned int>(m_Panel->getSize().y));
                    m_Scroll->setSize(m_Scroll->getSize().x, m_Panel->getSize().y);
                    m_Scroll->setMaximum(static_cast<unsigned int>(m_FullTextHeight));
                }
            }
            else
                TGUI_OUTPUT("TGUI warning: Unrecognized property '" + property + "' in section ChatBox in " + m_LoadedConfigFile + ".");
        }

        return m_Loaded = true;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const std::string& ChatBox::getLoadedConfigFile() const
    {
        return m_LoadedConfigFile;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChatBox::setSize(float width, float height)
    {
        // Don't do anything when the text box wasn't loaded correctly
        if (m_Loaded == false)
            return;

        // A negative size is not allowed for this widget
        if (width  < 0) width  = -width;
        if (height < 0) height = -height;

        // There is a minimum width
        if (m_Scroll == nullptr)
            width = TGUI_MAXIMUM(50, width);
        else
            width = TGUI_MAXIMUM(50 + m_Scroll->getSize().x, width);

        // Remember the old height
        float oldHeight = m_Panel->getSize().y;

        // Set the new size
        m_Panel->setSize(width, height);

        // If there is a scrollbar then reinitialize it
        if (m_Scroll != nullptr)
        {
            m_Scroll->setLowValue(static_cast<unsigned int>(m_Panel->getSize().y));
            m_Scroll->setSize(m_Scroll->getSize().x, m_Panel->getSize().y);
        }

        // Find out how much the height has changed
        float heightDiff = m_Panel->getSize().y - oldHeight;

        // Reposition all labels in the chatbox
        auto& labels = m_Panel->getWidgets();
        for (auto it = labels.begin(); it != labels.end(); ++it)
            (*it)->setPosition((*it)->getPosition().x, (*it)->getPosition().y + heightDiff);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    sf::Vector2f ChatBox::getSize() const
    {
        return m_Panel->getSize();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    sf::Vector2f ChatBox::getFullSize() const
    {
        return sf::Vector2f(getSize().x + m_LeftBorder + m_RightBorder,
                            getSize().y + m_TopBorder + m_BottomBorder);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChatBox::addLine(const sf::String& text)
    {
        addLine(text, m_TextColor, m_TextSize);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChatBox::addLine(const sf::String& text, const sf::Color& color)
    {
        addLine(text, color, m_TextSize);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChatBox::addLine(const sf::String& text, unsigned int textSize)
    {
        addLine(text, m_TextColor, textSize);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChatBox::addLine(const sf::String& text, const sf::Color& color, unsigned int textSize, const sf::Font* font)
    {
        auto& widgets = m_Panel->getWidgets();

        // Remove the top line if you exceed the maximum
        if ((m_MaxLines > 0) && (m_MaxLines < widgets.size() + 1))
            removeLine(0);

        Label::Ptr label(*m_Panel);
        label->setTextColor(color);
        label->setTextSize(textSize);

        if (font != nullptr)
            label->setTextFont(*font);

        Label::Ptr tempLine;
        tempLine->setTextSize(textSize);
        tempLine->setTextFont(*label->getTextFont());

        float width = m_Panel->getSize().x;
        if (m_Scroll)
            width -= m_Scroll->getSize().x;

        if (width < 0)
            width = 0;

        // Split the label over multiple lines if necessary
        unsigned int pos = 0;
        unsigned int size = 0;
        while (pos + size < text.getSize())
        {
            tempLine->setText(text.substring(pos, ++size));

            if (tempLine->getSize().x + 4.0f > width)
            {
                label->setText(label->getText() + text.substring(pos, size - 1) + "\n");

                pos = pos + size - 1;
                size = 0;
            }
        }
        label->setText(label->getText() + tempLine->getText());

        m_FullTextHeight += getLineSpacing(widgets.size()-1);

        if (m_Scroll != nullptr)
        {
            m_Scroll->setMaximum(static_cast<unsigned int>(m_FullTextHeight));

            if (m_Scroll->getMaximum() > m_Scroll->getLowValue())
                m_Scroll->setValue(m_Scroll->getMaximum() - m_Scroll->getLowValue());
        }

        // Reposition the labels
        updateDisplayedText();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    sf::String ChatBox::getLine(unsigned int lineIndex)
    {
        if (lineIndex < m_Panel->getWidgets().size())
        {
            return tgui::Label::Ptr(m_Panel->getWidgets()[lineIndex])->getText();
        }
        else // Index too high
            return "";
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ChatBox::removeLine(unsigned int lineIndex)
    {
        if (lineIndex < m_Panel->getWidgets().size())
        {
            tgui::Label::Ptr label = m_Panel->getWidgets()[lineIndex];
            m_FullTextHeight -= getLineSpacing(lineIndex);
            m_Panel->remove(label);

            if (m_Scroll != nullptr)
                m_Scroll->setMaximum(static_cast<unsigned int>(m_FullTextHeight));

            updateDisplayedText();
            return true;
        }
        else // Index too high
            return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChatBox::removeAllLines()
    {
        m_Panel->removeAllWidgets();

        m_FullTextHeight = 0;

        if (m_Scroll != nullptr)
            m_Scroll->setMaximum(static_cast<unsigned int>(m_FullTextHeight));

        updateDisplayedText();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int ChatBox::getLineAmount()
    {
        return m_Panel->getWidgets().size();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChatBox::setLineLimit(unsigned int maxLines)
    {
        m_MaxLines = maxLines;

        if ((m_MaxLines > 0) && (m_MaxLines < m_Panel->getWidgets().size()))
        {
            while (m_MaxLines < m_Panel->getWidgets().size())
            {
                m_FullTextHeight -= getLineSpacing(0);
                m_Panel->remove(m_Panel->getWidgets()[0]);
            }

            if (m_Scroll != nullptr)
                m_Scroll->setMaximum(static_cast<unsigned int>(m_FullTextHeight));

            updateDisplayedText();
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChatBox::setTextFont(const sf::Font& font)
    {
        m_Panel->setGlobalFont(font);

        m_FullTextHeight = 0;
        unsigned int labelNr = 0;
        auto& labels = m_Panel->getWidgets();
        for (auto it = labels.begin(); it != labels.end(); ++it, ++labelNr)
        {
            Label::Ptr(*it)->setTextFont(font);
            m_FullTextHeight += getLineSpacing(labelNr);
        }

        if (m_Scroll != nullptr)
        {
            m_Scroll->setMaximum(static_cast<unsigned int>(m_FullTextHeight));
            if (m_Scroll->getMaximum() > m_Scroll->getLowValue())
                m_Scroll->setValue(m_Scroll->getMaximum() - m_Scroll->getLowValue());
        }

        // Reposition the labels
        updateDisplayedText();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const sf::Font* ChatBox::getTextFont() const
    {
        return &m_Panel->getGlobalFont();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChatBox::setTextSize(unsigned int size)
    {
        // Store the new text size
        m_TextSize = size;

        // There is a minimum text size
        if (m_TextSize < 8)
            m_TextSize = 8;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int ChatBox::getTextSize() const
    {
        return m_TextSize;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChatBox::setTextColor(const sf::Color& color)
    {
        m_TextColor = color;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const sf::Color& ChatBox::getTextColor() const
    {
        return m_TextColor;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChatBox::setBorders(unsigned int leftBorder, unsigned int topBorder, unsigned int rightBorder, unsigned int bottomBorder)
    {
        m_LeftBorder   = leftBorder;
        m_TopBorder    = topBorder;
        m_RightBorder  = rightBorder;
        m_BottomBorder = bottomBorder;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChatBox::setBackgroundColor(const sf::Color& backgroundColor)
    {
        m_Panel->setBackgroundColor(backgroundColor);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChatBox::setBorderColor(const sf::Color& borderColor)
    {
        m_BorderColor = borderColor;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const sf::Color& ChatBox::getBackgroundColor() const
    {
        return m_Panel->getBackgroundColor();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const sf::Color& ChatBox::getBorderColor() const
    {
        return m_BorderColor;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChatBox::setLineSpacing(unsigned int lineSpacing)
    {
        m_LineSpacing = lineSpacing;

        updateDisplayedText();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChatBox::setLinesStartFromBottom(bool startFromBottom)
    {
        m_LinesStartFromBottom = startFromBottom;

        updateDisplayedText();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ChatBox::setScrollbar(const std::string& scrollbarConfigFileFilename)
    {
        // Do nothing when the string is empty
        if (scrollbarConfigFileFilename.empty() == true)
            return false;

        // If the scrollbar was already created then delete it first
        if (m_Scroll != nullptr)
            delete m_Scroll;

        // load the scrollbar and check if it failed
        m_Scroll = new Scrollbar();
        if(m_Scroll->load(scrollbarConfigFileFilename) == false)
        {
            // The scrollbar couldn't be loaded so it must be deleted
            delete m_Scroll;
            m_Scroll = nullptr;

            return false;
        }
        else // The scrollbar was loaded successfully
        {
            // Initialize the scrollbar
            m_Scroll->setVerticalScroll(true);
            m_Scroll->setSize(m_Scroll->getSize().x, m_Panel->getSize().y);
            m_Scroll->setLowValue(static_cast<unsigned int>(m_Panel->getSize().y));
            m_Scroll->setMaximum(static_cast<unsigned int>(m_FullTextHeight));

            return true;
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChatBox::removeScrollbar()
    {
        // Delete the scrollbar
        delete m_Scroll;
        m_Scroll = nullptr;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChatBox::setTransparency(unsigned char transparency)
    {
        Widget::setTransparency(transparency);

        m_Panel->setTransparency(transparency);

        if (m_Scroll != nullptr)
            m_Scroll->setTransparency(transparency);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ChatBox::mouseOnWidget(float x, float y)
    {
        // Get the current position
        sf::Vector2f position = getPosition();

        // Pass the event to the scrollbar (if there is one)
        if (m_Scroll != nullptr)
        {
            // Temporarily set the position of the scroll
            m_Scroll->setPosition(position.x + m_Panel->getSize().x - m_Scroll->getSize().x, position.y);

            // Pass the event
            m_Scroll->mouseOnWidget(x, y);

            // Reset the position
            m_Scroll->setPosition(0, 0);
        }

        // Check if the mouse is on top of the list box
        if (getTransform().transformRect(sf::FloatRect(0, 0, m_Panel->getSize().x, m_Panel->getSize().y)).contains(x, y))
            return true;
        else // The mouse is not on top of the list box
        {
            if (m_MouseHover)
                mouseLeftWidget();

            m_MouseHover = false;
            return false;
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChatBox::leftMousePressed(float x, float y)
    {
        // Don't do anything when the text box wasn't loaded correctly
        if (m_Loaded == false)
            return;

        // Set the mouse down flag to true
        m_MouseDown = true;

        // If there is a scrollbar then pass the event
        if (m_Scroll != nullptr)
        {
            // Remember the old scrollbar value
            unsigned int oldValue = m_Scroll->getValue();

            // Temporarily set the position of the scroll
            m_Scroll->setPosition(getPosition().x + m_Panel->getSize().x - m_Scroll->getSize().x, getPosition().y);

            // Pass the event
            if (m_Scroll->mouseOnWidget(x, y))
                m_Scroll->leftMousePressed(x, y);

            // Reset the position
            m_Scroll->setPosition(0, 0);

            // If the value of the scrollbar has changed then update the text
            if (oldValue != m_Scroll->getValue())
                updateDisplayedText();
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChatBox::leftMouseReleased(float x, float y)
    {
        // If there is a scrollbar then pass it the event
        if (m_Scroll != nullptr)
        {
            // Only pass the event when the scrollbar still thinks the mouse is down
            if (m_Scroll->m_MouseDown == true)
            {
                // Remember the old scrollbar value
                unsigned int oldValue = m_Scroll->getValue();

                // Temporarily set the position of the scroll
                m_Scroll->setPosition(getPosition().x + m_Panel->getSize().x - m_Scroll->getSize().x, getPosition().y);

                // Pass the event
                m_Scroll->leftMouseReleased(x, y);

                // Reset the position
                m_Scroll->setPosition(0, 0);

                // If the value of the scrollbar has changed then update the text
                if (oldValue != m_Scroll->getValue())
                {
                    // Check if the scrollbar value was incremented (you have pressed on the down arrow)
                    if (m_Scroll->getValue() == oldValue + 1)
                    {
                        // Decrement the value
                        m_Scroll->setValue(m_Scroll->getValue()-1);

                        // Scroll down with the whole item height instead of with a single pixel
                        m_Scroll->setValue(m_Scroll->getValue() + m_TextSize - (m_Scroll->getValue() % m_TextSize));
                    }
                    else if (m_Scroll->getValue() == oldValue - 1) // Check if the scrollbar value was decremented (you have pressed on the up arrow)
                    {
                        // increment the value
                        m_Scroll->setValue(m_Scroll->getValue()+1);

                        // Scroll up with the whole item height instead of with a single pixel
                        if (m_Scroll->getValue() % m_TextSize > 0)
                            m_Scroll->setValue(m_Scroll->getValue() - (m_Scroll->getValue() % m_TextSize));
                        else
                            m_Scroll->setValue(m_Scroll->getValue() - m_TextSize);
                    }

                    updateDisplayedText();
                }
            }
        }

        m_MouseDown = false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChatBox::mouseMoved(float x, float y)
    {
        if (m_MouseHover == false)
            mouseEnteredWidget();

        // Set the mouse move flag
        m_MouseHover = true;

        // If there is a scrollbar then pass the event
        if (m_Scroll != nullptr)
        {
            // Temporarily set the position of the scroll
            m_Scroll->setPosition(getPosition().x + m_Panel->getSize().x - m_Scroll->getSize().x, getPosition().y);

            // Check if you are dragging the thumb of the scrollbar
            if ((m_Scroll->m_MouseDown) && (m_Scroll->m_MouseDownOnThumb))
            {
                // Remember the old scrollbar value
                unsigned int oldValue = m_Scroll->getValue();

                // Pass the event, even when the mouse is not on top of the scrollbar
                m_Scroll->mouseMoved(x, y);

                // If the value of the scrollbar has changed then update the text
                if (oldValue != m_Scroll->getValue())
                    updateDisplayedText();
            }
            else // You are just moving the mouse
            {
                // When the mouse is on top of the scrollbar then pass the mouse move event
                if (m_Scroll->mouseOnWidget(x, y))
                    m_Scroll->mouseMoved(x, y);
            }

            // Reset the position
            m_Scroll->setPosition(0, 0);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChatBox::mouseNotOnWidget()
    {
        if (m_MouseHover)
            mouseLeftWidget();

        m_MouseHover = false;

        if (m_Scroll != nullptr)
            m_Scroll->m_MouseHover = false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChatBox::mouseNoLongerDown()
    {
        m_MouseDown = false;

        if (m_Scroll != nullptr)
            m_Scroll->m_MouseDown = false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChatBox::mouseWheelMoved(int delta, int, int)
    {
        // Only do something when there is a scrollbar
        if (m_Scroll != nullptr)
        {
            if (m_Scroll->getLowValue() < m_Scroll->getMaximum())
            {
                // Check if you are scrolling down
                if (delta < 0)
                {
                    // Scroll down
                    m_Scroll->setValue(m_Scroll->getValue() + (static_cast<unsigned int>(-delta) * m_TextSize));
                }
                else // You are scrolling up
                {
                    unsigned int change = static_cast<unsigned int>(delta) * m_TextSize;

                    // Scroll up
                    if (change < m_Scroll->getValue())
                        m_Scroll->setValue(m_Scroll->getValue() - change);
                    else
                        m_Scroll->setValue(0);
                }

                updateDisplayedText();
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ChatBox::setProperty(std::string property, const std::string& value)
    {
        property = toLower(property);

        if (property == "configfile")
        {
            load(value);
        }
        else if (property == "textsize")
        {
            setTextSize(atoi(value.c_str()));
        }
        else if (property == "borders")
        {
            Borders borders;
            if (extractBorders(value, borders))
                setBorders(borders.left, borders.top, borders.right, borders.bottom);
            else
                TGUI_OUTPUT("TGUI error: Failed to parse 'Borders' property.");
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
                        return false;

                    sf::Color color = extractColor(line.substr(openBracketPos, closeBracketPos - openBracketPos + 1));

                    std::string::size_type commaPos = line.rfind(',', openBracketPos);
                    if (commaPos == std::string::npos)
                        return false;

                    line.erase(commaPos);

                    addLine(line, color);
                }
                else
                    return false;
            }
        }
        else // The property didn't match
            return Widget::setProperty(property, value);

        // You pass here when one of the properties matched
        return true;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ChatBox::getProperty(std::string property, std::string& value) const
    {
        property = toLower(property);

        if (property == "configfile")
            value = getLoadedConfigFile();
        else if (property == "textsize")
            value = to_string(getTextSize());
        else if (property == "borders")
            value = "(" + to_string(getBorders().left) + "," + to_string(getBorders().top) + "," + to_string(getBorders().right) + "," + to_string(getBorders().bottom) + ")";
        else if (property == "backgroundcolor")
            value = "(" + to_string(int(getBackgroundColor().r)) + "," + to_string(int(getBackgroundColor().g)) + "," + to_string(int(getBackgroundColor().b)) + "," + to_string(int(getBackgroundColor().a)) + ")";
        else if (property == "bordercolor")
            value = "(" + to_string(int(getBorderColor().r)) + "," + to_string(int(getBorderColor().g)) + "," + to_string(int(getBorderColor().b)) + "," + to_string(int(getBorderColor().a)) + ")";
        else if (property == "lines")
        {
            std::vector<sf::String> lines;
            const std::vector<Widget::Ptr>& labels = m_Panel->getWidgets();

            for (auto it = labels.cbegin(); it != labels.cend(); ++it)
                lines.push_back("(" + Label::Ptr(*it)->getText() + "," + convertColorToString(Label::Ptr(*it)->getTextColor()) + ")");

            encodeList(lines, value);
        }
        else // The property didn't match
            return Widget::getProperty(property, value);

        // You pass here when one of the properties matched
        return true;
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
        assert(lineNumber < m_Panel->getWidgets().size());

        auto line = tgui::Label::Ptr(m_Panel->getWidgets()[lineNumber]);

        // Count the amount of lines that the label is taking
        std::string lineText = line->getText().toAnsiString();
        int linesOfText = std::count(lineText.begin(), lineText.end(), '\n') + 1;

        // If a line spacing was manually set then just return that one
        if (m_LineSpacing > 0)
            return m_LineSpacing * linesOfText;

        unsigned int lineSpacing = static_cast<unsigned int>(m_Panel->getGlobalFont().getLineSpacing(line->getTextSize()));
        if (lineSpacing > line->getTextSize())
            return lineSpacing * linesOfText;
        else
            return static_cast<unsigned int>(std::ceil(line->getTextSize() * 13.5 / 10.0) * linesOfText);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChatBox::initialize(Container *const parent)
    {
        m_Parent = parent;
        setTextFont(m_Parent->getGlobalFont());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChatBox::updateDisplayedText()
    {
        float position = 2.0f;
        if (m_Scroll)
            position -= static_cast<float>(m_Scroll->getValue());

        sf::Text tempText("k", m_Panel->getGlobalFont(), 20);

        auto& labels = m_Panel->getWidgets();
        for (unsigned int i = 0; i < labels.size(); ++i)
        {
            tgui::Label::Ptr label = labels[i];

            // Not every line has the same height
            float positionFix = 0;
            tempText.setCharacterSize(label->getTextSize());
            if (tempText.getLocalBounds().height > label->getSize().y)
                positionFix = tempText.getLocalBounds().height - label->getSize().y;

            label->setPosition(2.0f, position + positionFix);
            position += getLineSpacing(i);
        }

        // Correct the position when there is no scrollbar
        if ((m_Scroll == nullptr) && (!labels.empty()))
        {
            tgui::Label::Ptr label = labels.back();
            if (position > m_Panel->getSize().y)
            {
                float diff = position - m_Panel->getSize().y;
                for (auto it = labels.begin(); it != labels.end(); ++it)
                    (*it)->setPosition((*it)->getPosition().x, (*it)->getPosition().y - diff);
            }
        }

        // Put the lines at the bottom of the chat box if needed
        if (m_LinesStartFromBottom && (position < m_Panel->getSize().y))
        {
            float diff = m_Panel->getSize().y - position;
            for (auto it = labels.begin(); it != labels.end(); ++it)
                (*it)->setPosition((*it)->getPosition().x, (*it)->getPosition().y + diff);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChatBox::draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        m_Panel->setPosition(getAbsolutePosition());

        // Draw the panel
        target.draw(*m_Panel);

        // Adjust the transformation
        states.transform *= getTransform();

        // Draw left border
        sf::RectangleShape border(sf::Vector2f(static_cast<float>(m_LeftBorder), m_Panel->getSize().y + m_TopBorder));
        border.setPosition(-static_cast<float>(m_LeftBorder), -static_cast<float>(m_TopBorder));
        border.setFillColor(m_BorderColor);
        target.draw(border, states);

        // Draw top border
        border.setSize(sf::Vector2f(m_Panel->getSize().x + m_RightBorder, static_cast<float>(m_TopBorder)));
        border.setPosition(0, -static_cast<float>(m_TopBorder));
        target.draw(border, states);

        // Draw right border
        border.setSize(sf::Vector2f(static_cast<float>(m_RightBorder), m_Panel->getSize().y + m_BottomBorder));
        border.setPosition(m_Panel->getSize().x, 0);
        target.draw(border, states);

        // Draw bottom border
        border.setSize(sf::Vector2f(m_Panel->getSize().x + m_LeftBorder, static_cast<float>(m_BottomBorder)));
        border.setPosition(-static_cast<float>(m_LeftBorder), m_Panel->getSize().y);
        target.draw(border, states);

        // Check if there is a scrollbar
        if (m_Scroll != nullptr)
        {
            // Draw the scrollbar
            states.transform.translate(m_Panel->getSize().x - m_Scroll->getSize().x, 0);
            target.draw(*m_Scroll, states);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
