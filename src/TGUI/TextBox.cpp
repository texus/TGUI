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


#include <SFML/OpenGL.hpp>

#include <TGUI/Scrollbar.hpp>
#include <TGUI/Container.hpp>
#include <TGUI/TextBox.hpp>
#include <TGUI/Clipboard.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    TextBox::TextBox() :
    m_Size                    (360, 200),
    m_Text                    (""),
    m_DisplayedText           (""),
    m_TextSize                (30),
    m_LineHeight              (40),
    m_Lines                   (1),
    m_MaxChars                (0),
    m_TopLine                 (1),
    m_VisibleLines            (1),
    m_SelChars                (0),
    m_SelStart                (0),
    m_SelEnd                  (0),
    m_SelectionPointPosition  (0, 0),
    m_SelectionPointVisible   (true),
    m_SelectionPointColor     (110, 110, 255),
    m_SelectionPointWidth     (2),
    m_SelectionTextsNeedUpdate(true),
    m_Scroll                  (nullptr),
    m_PossibleDoubleClick     (false),
    m_readOnly                (false)
    {
        m_Callback.widgetType = Type_TextBox;
        m_AnimatedWidget = true;
        m_DraggableWidget = true;

        changeColors();

        // Load the text box with default values
        m_Loaded = true;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    TextBox::TextBox(const TextBox& copy) :
    Widget                       (copy),
    WidgetBorders                (copy),
    m_LoadedConfigFile           (copy.m_LoadedConfigFile),
    m_Size                       (copy.m_Size),
    m_Text                       (copy.m_Text),
    m_DisplayedText              (copy.m_DisplayedText),
    m_TextSize                   (copy.m_TextSize),
    m_LineHeight                 (copy.m_LineHeight),
    m_Lines                      (copy.m_Lines),
    m_MaxChars                   (copy.m_MaxChars),
    m_TopLine                    (copy.m_TopLine),
    m_VisibleLines               (copy.m_VisibleLines),
    m_SelChars                   (copy.m_SelChars),
    m_SelStart                   (copy.m_SelStart),
    m_SelEnd                     (copy.m_SelEnd),
    m_SelectionPointPosition     (copy.m_SelectionPointPosition),
    m_SelectionPointVisible      (copy.m_SelectionPointVisible),
    m_SelectionPointColor        (copy.m_SelectionPointColor),
    m_SelectionPointWidth        (copy.m_SelectionPointWidth),
    m_SelectionTextsNeedUpdate   (copy.m_SelectionTextsNeedUpdate),
    m_BackgroundColor            (copy.m_BackgroundColor),
    m_SelectedTextBgrColor       (copy.m_SelectedTextBgrColor),
    m_BorderColor                (copy.m_BorderColor),
    m_TextBeforeSelection        (copy.m_TextBeforeSelection),
    m_TextSelection1             (copy.m_TextSelection1),
    m_TextSelection2             (copy.m_TextSelection2),
    m_TextAfterSelection1        (copy.m_TextAfterSelection1),
    m_TextAfterSelection2        (copy.m_TextAfterSelection2),
    m_MultilineSelectionRectWidth(copy.m_MultilineSelectionRectWidth),
    m_PossibleDoubleClick        (copy.m_PossibleDoubleClick),
    m_readOnly                   (copy.m_readOnly)
    {
        // If there is a scrollbar then copy it
        if (copy.m_Scroll != nullptr)
            m_Scroll = new Scrollbar(*copy.m_Scroll);
        else
            m_Scroll = nullptr;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    TextBox::~TextBox()
    {
        if (m_Scroll != nullptr)
            delete m_Scroll;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    TextBox& TextBox::operator= (const TextBox& right)
    {
        if (this != &right)
        {
            // If there already was a scrollbar then delete it now
            if (m_Scroll != nullptr)
            {
                delete m_Scroll;
                m_Scroll = nullptr;
            }

            TextBox temp(right);
            this->Widget::operator=(right);
            this->WidgetBorders::operator=(right);

            std::swap(m_LoadedConfigFile,            temp.m_LoadedConfigFile);
            std::swap(m_Size,                        temp.m_Size);
            std::swap(m_Text,                        temp.m_Text);
            std::swap(m_DisplayedText,               temp.m_DisplayedText);
            std::swap(m_TextSize,                    temp.m_TextSize);
            std::swap(m_LineHeight,                  temp.m_LineHeight);
            std::swap(m_Lines,                       temp.m_Lines);
            std::swap(m_MaxChars,                    temp.m_MaxChars);
            std::swap(m_TopLine,                     temp.m_TopLine);
            std::swap(m_VisibleLines,                temp.m_VisibleLines);
            std::swap(m_SelChars,                    temp.m_SelChars);
            std::swap(m_SelStart,                    temp.m_SelStart);
            std::swap(m_SelEnd,                      temp.m_SelEnd);
            std::swap(m_SelectionPointPosition,      temp.m_SelectionPointPosition);
            std::swap(m_SelectionPointVisible,       temp.m_SelectionPointVisible);
            std::swap(m_SelectionPointColor,         temp.m_SelectionPointColor);
            std::swap(m_SelectionPointWidth,         temp.m_SelectionPointWidth);
            std::swap(m_SelectionTextsNeedUpdate,    temp.m_SelectionTextsNeedUpdate);
            std::swap(m_BackgroundColor,             temp.m_BackgroundColor);
            std::swap(m_SelectedTextBgrColor,        temp.m_SelectedTextBgrColor);
            std::swap(m_BorderColor,                 temp.m_BorderColor);
            std::swap(m_TextBeforeSelection,         temp.m_TextBeforeSelection);
            std::swap(m_TextSelection1,              temp.m_TextSelection1);
            std::swap(m_TextSelection2,              temp.m_TextSelection2);
            std::swap(m_TextAfterSelection1,         temp.m_TextAfterSelection1);
            std::swap(m_TextAfterSelection2,         temp.m_TextAfterSelection2);
            std::swap(m_MultilineSelectionRectWidth, temp.m_MultilineSelectionRectWidth);
            std::swap(m_Scroll,                      temp.m_Scroll);
            std::swap(m_PossibleDoubleClick,         temp.m_PossibleDoubleClick);
            std::swap(m_readOnly,                    temp.m_readOnly);
        }

        return *this;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    TextBox* TextBox::clone()
    {
        return new TextBox(*this);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool TextBox::load(const std::string& configFileFilename)
    {
        m_LoadedConfigFile = getResourcePath() + configFileFilename;

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
        if (!configFile.read("TextBox", properties, values))
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
            else if (property == "textcolor")
            {
                setTextColor(configFile.readColor(value));
            }
            else if (property == "selectedtextbackgroundcolor")
            {
                setSelectedTextBackgroundColor(configFile.readColor(value));
            }
            else if (property == "selectedtextcolor")
            {
                setSelectedTextColor(configFile.readColor(value));
            }
            else if (property == "selectionpointcolor")
            {
                setSelectionPointColor(configFile.readColor(value));
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
                    m_Scroll->setLowValue(m_Size.y);
                    m_Scroll->setSize(m_Scroll->getSize().x, static_cast<float>(m_Size.y));
                }
            }
            else
                TGUI_OUTPUT("TGUI warning: Unrecognized property '" + property + "' in section ChatBox in " + m_LoadedConfigFile + ".");
        }

        return true;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const std::string& TextBox::getLoadedConfigFile() const
    {
        return m_LoadedConfigFile;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TextBox::setSize(float width, float height)
    {
        // Don't do anything when the text box wasn't loaded correctly
        if (m_Loaded == false)
            return;

        // Don't continue when line height is 0
        if (m_LineHeight == 0)
            return;

        // A negative size is not allowed for this widget
        if (width  < 0) width  = -width;
        if (height < 0) height = -height;

        // There is a minimum width
        if (m_Scroll == nullptr)
            width = TGUI_MAXIMUM(50, width);
        else
            width = TGUI_MAXIMUM(50 + m_Scroll->getSize().x, width);

        // There is also a minimum height
        if (m_Scroll == nullptr)
        {
            // If there is a text then it should still fit inside the text box
            if (m_Text.getSize() > 0)
            {
                if (m_Size.y < (m_Lines * m_LineHeight))
                    m_Size.y = (m_Lines * m_LineHeight);
            }
            else // There are no items
            {
                // At least one item should fit inside the text box
                if (m_Size.y < m_LineHeight)
                    m_Size.y = m_LineHeight;
            }
        }
        else // There is a scrollbar
        {
            // At least one item should fit inside the text box
            if (m_Size.y < m_LineHeight)
                m_Size.y = m_LineHeight;
        }

        // There is also a minimum height
        if (height < m_LineHeight)
        {
            height = static_cast<float>(m_LineHeight);
        }

        // Store the values
        m_Size.x = static_cast<unsigned int>(width);
        m_Size.y = static_cast<unsigned int>(height);

        // If there is a scrollbar then reinitialize it
        if (m_Scroll != nullptr)
        {
            m_Scroll->setLowValue(m_Size.y);
            m_Scroll->setSize(m_Scroll->getSize().x, static_cast<float>(m_Size.y));
        }

        // The size of the textbox has changed, update the text
        m_SelectionTextsNeedUpdate = true;
        updateDisplayedText();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    sf::Vector2f TextBox::getSize() const
    {
        return sf::Vector2f(static_cast<float>(m_Size.x), static_cast<float>(m_Size.y));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    sf::Vector2f TextBox::getFullSize() const
    {
        return sf::Vector2f(static_cast<float>(m_Size.x + m_LeftBorder + m_RightBorder),
                            static_cast<float>(m_Size.y + m_TopBorder + m_BottomBorder));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TextBox::setText(const sf::String& text)
    {
        // Don't do anything when the text box wasn't loaded correctly
        if (m_Loaded == false)
            return;

        // Store the text
        m_Text = text;

        // Set the selection point behind the last character
        setSelectionPointPosition(m_Text.getSize());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TextBox::addText(const sf::String& text)
    {
        // Don't do anything when the text box wasn't loaded correctly
        if (m_Loaded == false)
            return;

        // Add the text
        m_Text += text;

        // Set the selection point behind the last character
        setSelectionPointPosition(m_Text.getSize());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    sf::String TextBox::getText() const
    {
        return m_Text;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TextBox::setTextFont(const sf::Font& font)
    {
        m_TextBeforeSelection.setFont(font);
        m_TextSelection1.setFont(font);
        m_TextSelection2.setFont(font);
        m_TextAfterSelection1.setFont(font);
        m_TextAfterSelection2.setFont(font);

        setTextSize(m_TextSize);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const sf::Font* TextBox::getTextFont() const
    {
        return m_TextBeforeSelection.getFont();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TextBox::setTextSize(unsigned int size)
    {
        // Store the new text size
        m_TextSize = size;

        // There is a minimum text size
        if (m_TextSize < 8)
            m_TextSize = 8;

        // Change the text size
        m_TextBeforeSelection.setCharacterSize(m_TextSize);
        m_TextSelection1.setCharacterSize(m_TextSize);
        m_TextSelection2.setCharacterSize(m_TextSize);
        m_TextAfterSelection1.setCharacterSize(m_TextSize);
        m_TextAfterSelection2.setCharacterSize(m_TextSize);

        if (!m_TextBeforeSelection.getFont())
            return;

        // Calculate the height of one line
        m_LineHeight = static_cast<unsigned int>(m_TextBeforeSelection.getFont()->getLineSpacing(m_TextSize));

        // There is also a minimum height
        if (m_Size.y < m_LineHeight)
            m_Size.y = m_LineHeight;

        // If there is a scrollbar then reinitialize it
        if (m_Scroll != nullptr)
        {
            m_Scroll->setLowValue(m_Size.y);
            m_Scroll->setSize(m_Scroll->getSize().x, static_cast<float>(m_Size.y));
        }

        // The size has changed, update the text
        m_SelectionTextsNeedUpdate = true;
        updateDisplayedText();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int TextBox::getTextSize() const
    {
        return m_TextSize;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TextBox::setMaximumCharacters(unsigned int maxChars)
    {
        // Set the new character limit ( 0 to disable the limit )
        m_MaxChars = maxChars;

        // If there is a character limit then check if it is exeeded
        if ((m_MaxChars > 0) && (m_Text.getSize() > m_MaxChars))
        {
            // Remove all the excess characters
            m_Text.erase(m_MaxChars, sf::String::InvalidPos);

            // Set the selection point behind the last character
            setSelectionPointPosition(m_Text.getSize());
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int TextBox::getMaximumCharacters() const
    {
        return m_MaxChars;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TextBox::setBorders(unsigned int leftBorder, unsigned int topBorder, unsigned int rightBorder, unsigned int bottomBorder)
    {
        m_LeftBorder   = leftBorder;
        m_TopBorder    = topBorder;
        m_RightBorder  = rightBorder;
        m_BottomBorder = bottomBorder;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TextBox::changeColors(const sf::Color& backgroundColor,
                               const sf::Color& color,
                               const sf::Color& selectedColor,
                               const sf::Color& selectedBgrColor,
                               const sf::Color& borderColor,
                               const sf::Color& selectionPointColor)
    {
        m_TextBeforeSelection.setColor(color);
        m_TextSelection1.setColor(selectedColor);
        m_TextSelection2.setColor(selectedColor);
        m_TextAfterSelection1.setColor(color);
        m_TextAfterSelection2.setColor(color);

        m_SelectionPointColor           = selectionPointColor;
        m_BackgroundColor               = backgroundColor;
        m_SelectedTextBgrColor          = selectedBgrColor;
        m_BorderColor                   = borderColor;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TextBox::setBackgroundColor(const sf::Color& backgroundColor)
    {
        m_BackgroundColor = backgroundColor;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TextBox::setTextColor(const sf::Color& textColor)
    {
        m_TextBeforeSelection.setColor(textColor);
        m_TextAfterSelection1.setColor(textColor);
        m_TextAfterSelection2.setColor(textColor);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TextBox::setSelectedTextColor(const sf::Color& selectedTextColor)
    {
        m_TextSelection1.setColor(selectedTextColor);
        m_TextSelection2.setColor(selectedTextColor);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TextBox::setSelectedTextBackgroundColor(const sf::Color& selectedTextBackgroundColor)
    {
        m_SelectedTextBgrColor = selectedTextBackgroundColor;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TextBox::setBorderColor(const sf::Color& borderColor)
    {
        m_BorderColor = borderColor;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TextBox::setSelectionPointColor(const sf::Color& selectionPointColor)
    {
        m_SelectionPointColor = selectionPointColor;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const sf::Color& TextBox::getBackgroundColor() const
    {
        return m_BackgroundColor;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const sf::Color& TextBox::getTextColor() const
    {
        return m_TextBeforeSelection.getColor();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const sf::Color& TextBox::getSelectedTextColor() const
    {
        return m_TextSelection1.getColor();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const sf::Color& TextBox::getSelectedTextBackgroundColor() const
    {
        return m_SelectedTextBgrColor;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const sf::Color& TextBox::getBorderColor() const
    {
        return m_BorderColor;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const sf::Color& TextBox::getSelectionPointColor() const
    {
        return m_SelectionPointColor;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TextBox::setSelectionPointPosition(unsigned int charactersBeforeSelectionPoint)
    {
        // The selection point position has to stay inside the string
        if (charactersBeforeSelectionPoint > m_Text.getSize())
            charactersBeforeSelectionPoint = m_Text.getSize();

        // Don't continue when line height is 0
        if (m_LineHeight == 0)
            return;

        // Set the selection point to the correct position
        m_SelChars = 0;
        m_SelStart = charactersBeforeSelectionPoint;
        m_SelEnd = charactersBeforeSelectionPoint;

        // Change our three texts
        m_TextBeforeSelection.setString(m_DisplayedText);
        m_TextSelection1.setString("");
        m_TextSelection2.setString("");
        m_TextAfterSelection1.setString("");
        m_TextAfterSelection2.setString("");

        // Update the text
        updateDisplayedText();

        // Check if there is a scrollbar
        if (m_Scroll != nullptr)
        {
            unsigned int newlines = 0;
            unsigned int newlinesAdded = 0;
            unsigned int totalLines = 0;

            // Loop through all characters
            for (unsigned int i=0; i<m_SelEnd; ++i)
            {
                // Make sure there is no newline in the text
                if (m_Text[i] != '\n')
                {
                    // Check if there is a newline in the displayed text
                    if (m_DisplayedText[i + newlinesAdded] == '\n')
                    {
                        // A newline was added here
                        ++newlinesAdded;
                        ++totalLines;

                        if (i < m_SelEnd)
                            ++newlines;
                    }
                }
                else // The text already contains a newline
                {
                    ++totalLines;

                    if (i < m_SelEnd)
                        ++newlines;
                }
            }

            // Check if the selection point is located above the view
            if ((newlines < m_TopLine - 1) || ((newlines < m_TopLine) && (m_Scroll->getValue() % m_LineHeight > 0)))
            {
                m_Scroll->setValue(newlines * m_LineHeight);
                updateDisplayedText();
            }

            // Check if the selection point is below the view
            else if (newlines > m_TopLine + m_VisibleLines - 2)
            {
                m_Scroll->setValue((newlines - m_VisibleLines + 1) * m_LineHeight);
                updateDisplayedText();
            }
            else if ((newlines > m_TopLine + m_VisibleLines - 3) && (m_Scroll->getValue() % m_LineHeight > 0))
            {
                m_Scroll->setValue((newlines - m_VisibleLines + 2) * m_LineHeight);
                updateDisplayedText();
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool TextBox::setScrollbar(const std::string& scrollbarConfigFileFilename)
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
            m_Scroll->setSize(m_Scroll->getSize().x, static_cast<float>(m_Size.y));
            m_Scroll->setLowValue(m_Size.y);
            m_Scroll->setMaximum(m_Lines * m_LineHeight);

            return true;
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TextBox::removeScrollbar()
    {
        // Delete the scrollbar
        delete m_Scroll;
        m_Scroll = nullptr;

        m_TopLine = 1;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TextBox::setSelectionPointWidth(unsigned int width)
    {
        m_SelectionPointWidth = width;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int TextBox::getSelectionPointWidth() const
    {
        return m_SelectionPointWidth;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TextBox::setReadOnly(bool readOnly)
    {
        m_readOnly = readOnly;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TextBox::setTransparency(unsigned char transparency)
    {
        Widget::setTransparency(transparency);

        if (m_Scroll != nullptr)
            m_Scroll->setTransparency(m_Opacity);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool TextBox::mouseOnWidget(float x, float y)
    {
        // Don't do anything when the text box wasn't loaded correctly
        if (m_Loaded == false)
            return false;

        // Get the current position
        sf::Vector2f position = getPosition();

        // Pass the event to the scrollbar (if there is one)
        if (m_Scroll != nullptr)
        {
            // Temporarily set the position of the scroll
            m_Scroll->setPosition(position.x + m_Size.x - m_Scroll->getSize().x, position.y);

            // Pass the event
            m_Scroll->mouseOnWidget(x, y);

            // Reset the position
            m_Scroll->setPosition(0, 0);
        }

        // Check if the mouse is on top of the text box
        if (getTransform().transformRect(sf::FloatRect(0, 0, static_cast<float>(m_Size.x), static_cast<float>(m_Size.y))).contains(x, y))
            return true;
        else // The mouse is not on top of the text box
        {
            if (m_MouseHover)
                mouseLeftWidget();

            m_MouseHover = false;
            return false;
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TextBox::leftMousePressed(float x, float y)
    {
        // Don't do anything when the text box wasn't loaded correctly
        if (m_Loaded == false)
            return;

        // Set the mouse down flag
        m_MouseDown = true;

        // This will be true when the click didn't occur on the scrollbar
        bool clickedOnTextBox = true;

        // If there is a scrollbar then pass the event
        if (m_Scroll != nullptr)
        {
            // Remember the old scrollbar value
            unsigned int oldValue = m_Scroll->getValue();

            // Temporarily set the position of the scroll
            m_Scroll->setPosition(getPosition().x + m_Size.x - m_Scroll->getSize().x, getPosition().y);

            // Pass the event
            if (m_Scroll->mouseOnWidget(x, y))
            {
                m_Scroll->leftMousePressed(x, y);
                clickedOnTextBox = false;
            }

            // Reset the position
            m_Scroll->setPosition(0, 0);

            // If the value of the scrollbar has changed then update the text
            if (oldValue != m_Scroll->getValue())
                updateDisplayedText();
        }

        // If the click occured on the text box
        if (clickedOnTextBox)
        {
            // Don't continue when line height is 0
            if (m_LineHeight == 0)
                return;

            unsigned int selectionPointPosition = findSelectionPointPosition(x - getPosition().x - 4, y - getPosition().y);

            // Check if this is a double click
            if ((m_PossibleDoubleClick) && (m_SelChars == 0) && (selectionPointPosition == m_SelEnd))
            {
                // The next click is going to be a normal one again
                m_PossibleDoubleClick = false;

                // Select the whole text
                m_SelStart = 0;
                m_SelEnd = m_Text.getSize();
                m_SelChars = m_Text.getSize();

                // Update the text
                m_SelectionTextsNeedUpdate = true;
                updateDisplayedText();

                // Check if there is a scrollbar
                if (m_Scroll != nullptr)
                {
                    unsigned int newlines = 0;
                    unsigned int newlinesAdded = 0;
                    unsigned int totalLines = 0;

                    // Loop through all characters
                    for (unsigned int i=0; i<m_SelEnd; ++i)
                    {
                        // Make sure there is no newline in the text
                        if (m_Text[i] != '\n')
                        {
                            // Check if there is a newline in the displayed text
                            if (m_DisplayedText[i + newlinesAdded] == '\n')
                            {
                                // A newline was added here
                                ++newlinesAdded;
                                ++totalLines;

                                if (i < m_SelEnd)
                                    ++newlines;
                            }
                        }
                        else // The text already contains a newline
                        {
                            ++totalLines;

                            if (i < m_SelEnd)
                                ++newlines;
                        }
                    }

                    // Check if the selection point is located above the view
                    if ((newlines < m_TopLine - 1) || ((newlines < m_TopLine) && (m_Scroll->getValue() % m_LineHeight > 0)))
                    {
                        m_Scroll->setValue(newlines * m_LineHeight);
                        updateDisplayedText();
                    }

                    // Check if the selection point is below the view
                    else if (newlines > m_TopLine + m_VisibleLines - 2)
                    {
                        m_Scroll->setValue((newlines - m_VisibleLines + 1) * m_LineHeight);
                        updateDisplayedText();
                    }
                    else if ((newlines > m_TopLine + m_VisibleLines - 3) && (m_Scroll->getValue() % m_LineHeight > 0))
                    {
                        m_Scroll->setValue((newlines - m_VisibleLines + 2) * m_LineHeight);
                        updateDisplayedText();
                    }
                }
            }
            else // No double clicking
            {
                // Set the new selection point
                setSelectionPointPosition(selectionPointPosition);

                // If the next click comes soon enough then it will be a double click
                m_PossibleDoubleClick = true;
            }

            // The selection point should be visible
            m_SelectionPointVisible = true;
            m_AnimationTimeElapsed = sf::Time();
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TextBox::leftMouseReleased(float x, float y)
    {
        // Don't do anything when the text box wasn't loaded correctly
        if (m_Loaded == false)
            return;

        // If there is a scrollbar then pass it the event
        if (m_Scroll != nullptr)
        {
            // Only pass the event when the scrollbar still thinks the mouse is down
            if (m_Scroll->m_MouseDown == true)
            {
                // Don't continue when line height is 0
                if (m_LineHeight == 0)
                    return;

                // Remember the old scrollbar value
                unsigned int oldValue = m_Scroll->getValue();

                // Temporarily set the position of the scroll
                m_Scroll->setPosition(getPosition().x + m_Size.x - m_Scroll->getSize().x, getPosition().y);

                // Pass the event
                m_Scroll->leftMouseReleased(x, y);

                // Reset the position
                m_Scroll->setPosition(0, 0);

                // If the value of the scrollbar has changed then update the text
                if (oldValue != m_Scroll->getValue())
                {
                    updateDisplayedText();

                    // Check if the scrollbar value was incremented (you have pressed on the down arrow)
                    if (m_Scroll->getValue() == oldValue + 1)
                    {
                        // Decrement the value
                        m_Scroll->setValue(m_Scroll->getValue()-1);

                        // Scroll down with the whole item height instead of with a single pixel
                        m_Scroll->setValue(m_Scroll->getValue() + m_LineHeight - (m_Scroll->getValue() % m_LineHeight));
                    }
                    else if (m_Scroll->getValue() == oldValue - 1) // Check if the scrollbar value was decremented (you have pressed on the up arrow)
                    {
                        // increment the value
                        m_Scroll->setValue(m_Scroll->getValue()+1);

                        // Scroll up with the whole item height instead of with a single pixel
                        if (m_Scroll->getValue() % m_LineHeight > 0)
                            m_Scroll->setValue(m_Scroll->getValue() - (m_Scroll->getValue() % m_LineHeight));
                        else
                            m_Scroll->setValue(m_Scroll->getValue() - m_LineHeight);
                    }
                }
            }
        }

        m_MouseDown = false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TextBox::mouseMoved(float x, float y)
    {
        // Don't do anything when the text box wasn't loaded correctly
        if (m_Loaded == false)
            return;

        if (m_MouseHover == false)
            mouseEnteredWidget();

        // Set the mouse move flag
        m_MouseHover = true;

        // The mouse has moved so a double click is no longer possible
        m_PossibleDoubleClick = false;

        // If there is a scrollbar then pass the event
        if (m_Scroll != nullptr)
        {
            // Temporarily set the position of the scroll
            m_Scroll->setPosition(getPosition().x + m_Size.x - m_Scroll->getSize().x, getPosition().y);

            // Check if you are dragging the thumb of the scrollbar
            if (m_Scroll->m_MouseDown)
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

                // If the mouse is down then you are selecting text
                if (m_MouseDown)
                    selectText(x, y);
            }

            // Reset the position
            m_Scroll->setPosition(0, 0);
        }
        else // There is no scrollbar
        {
            // If the mouse is down then you are selecting text
            if (m_MouseDown)
                selectText(x, y);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TextBox::mouseNotOnWidget()
    {
        if (m_MouseHover)
            mouseLeftWidget();

        m_MouseHover = false;

        if (m_Scroll != nullptr)
            m_Scroll->m_MouseHover = false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TextBox::mouseNoLongerDown()
    {
        m_MouseDown = false;

        if (m_Scroll != nullptr)
            m_Scroll->m_MouseDown = false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TextBox::keyPressed(const sf::Event::KeyEvent& event)
    {
        // Don't do anything when the edit box wasn't loaded correctly
        if (m_Loaded == false)
            return;

        // Check if one of the correct keys was pressed
        if (event.code == sf::Keyboard::Left)
        {
            // Check if we have selected some text
            if (m_SelChars > 0)
            {
                // We will not move the selection point, but just undo the selection
                if (m_SelStart < m_SelEnd)
                    setSelectionPointPosition(m_SelStart);
                else
                    setSelectionPointPosition(m_SelEnd);
            }
            else // When we didn't select any text
            {
                // You don't have to do anything when the selection point is at the beginning of the text
                if (m_SelEnd > 0)
                {
                    // Move the selection point to the left
                    setSelectionPointPosition(m_SelEnd - 1);
                }
            }

            // Our selection point has moved, it should be visible
            m_SelectionPointVisible = true;
            m_AnimationTimeElapsed = sf::Time();
        }
        else if (event.code == sf::Keyboard::Right)
        {
            // Check if we have selected some text
            if (m_SelChars > 0)
            {
                // We will not move the selection point, but just undo the selection
                if (m_SelStart < m_SelEnd)
                    setSelectionPointPosition(m_SelEnd);
                else
                    setSelectionPointPosition(m_SelStart);
            }
            else // When we didn't select any text
            {
                // You don't have to do anything when the selection point is at the end of the text
                if (m_SelEnd < m_Text.getSize())
                {
                    // Move the selection point to the left
                    setSelectionPointPosition(m_SelEnd + 1);
                }
            }

            // Our selection point has moved, it should be visible
            m_SelectionPointVisible = true;
            m_AnimationTimeElapsed = sf::Time();
        }
        else if (event.code == sf::Keyboard::Up)
        {
            sf::Text tempText(m_TextBeforeSelection);
            tempText.setString(m_DisplayedText);

            bool newlineAdded = false;
            unsigned int newlinesAdded = 0;

            unsigned int character;
            unsigned int newTopPosition = 0;
            sf::Vector2u newPosition(0, 0);

            int distanceX;
            unsigned int previousdistanceX = m_Size.x;

            // Loop through all characters
            for (unsigned int i=0; i<m_SelEnd; ++i)
            {
                // Make sure there is no newline in the text
                if (m_Text[i] != '\n')
                {
                    // Check if there is a newline in the displayed text
                    if (m_DisplayedText[i + newlinesAdded] == '\n')
                    {
                        // A newline was added here
                        ++newlinesAdded;
                        newlineAdded = true;
                    }
                }
                else // The newline occurs in the text itself
                    newlineAdded = false;
            }

            // Get the position of the character behind the selection point
            sf::Vector2u originalPosition = sf::Vector2u(tempText.findCharacterPos(m_SelEnd + newlinesAdded));

            // Try to find the line above the selection point
            for (character=m_SelEnd; character > 0; --character)
            {
                // Get the top position of the character before it
                newTopPosition = static_cast<unsigned int>(tempText.findCharacterPos(character + newlinesAdded - 1).y);

                // Check if the the character is on the line above the original one
                if (newTopPosition < originalPosition.y)
                    break;
            }

            // Our selection point has moved, it should be visible
            m_SelectionPointVisible = true;
            m_AnimationTimeElapsed = sf::Time();

            // Don't do anything when the selection point is on the first line
            if (character > 0)
            {
                // Try to find the closest character
                for ( ; character > 0; --character)
                {
                    // Get the position of the character before it
                    newPosition = sf::Vector2u(tempText.findCharacterPos(character + newlinesAdded - 1));

                    // The character must remain on the same line
                    if (newPosition.y < newTopPosition)
                    {
                        // We have found the character that we were looking for
                        setSelectionPointPosition(character + newlineAdded);
                        return;
                    }

                    // Calculate the distance to the original position
                    distanceX = newPosition.x - originalPosition.x;

                    // Check if the distance is going away again
                    if (static_cast<unsigned int>(abs(distanceX)) > previousdistanceX)
                    {
                        // We have found the character that we were looking for
                        setSelectionPointPosition(character + newlineAdded);
                        return;
                    }

                    // Remember the previous distance
                    previousdistanceX = abs(distanceX);
                }

                // Check if the selection point should be behind the first character
                if (originalPosition.x > previousdistanceX)
                {
                    // We have found the character that we were looking for
                    setSelectionPointPosition(character + newlineAdded);
                    return;
                }

                // If you pass here then the selection point should be at the beginning of the text
                setSelectionPointPosition(0);
            }
        }
        else if (event.code == sf::Keyboard::Down)
        {
            sf::Text tempText(m_TextBeforeSelection);
            tempText.setString(m_DisplayedText);

            bool newlineAdded = false;
            unsigned int newlinesAdded = 0;

            unsigned int character;
            unsigned int newTopPosition = 0;
            sf::Vector2u newPosition(0, 0);

            int distanceX;
            int previousdistanceX = m_Size.x;

            // Loop through all characters
            for (unsigned int i=0; i<m_Text.getSize(); ++i)
            {
                // Make sure there is no newline in the text
                if (m_Text[i] != '\n')
                {
                    // Check if there is a newline in the displayed text
                    if (m_DisplayedText[i + newlinesAdded] == '\n')
                    {
                        // A newline was added here
                        ++newlinesAdded;
                        newlineAdded = true;

                        // Stop when the newline behind the selection point was found
                        if (i > m_SelEnd)
                            break;
                    }
                }
                else // The newline occurs in the text itself
                {
                    newlineAdded = false;

                    // Stop when the newline behind the selection point was found
                    if (i > m_SelEnd)
                        break;
                }
            }

            // Get the position of the character behind the selection point
            sf::Vector2u originalPosition = sf::Vector2u(tempText.findCharacterPos(m_SelEnd + newlinesAdded - newlineAdded));

            // Try to find the line below the selection point
            for (character=m_SelEnd; character < m_Text.getSize(); ++character)
            {
                // Get the top position of the character after it
                newTopPosition = static_cast<unsigned int>(tempText.findCharacterPos(character + newlinesAdded - newlineAdded + 1).y);

                // Check if the the character is on the line below the original one
                if (newTopPosition > originalPosition.y)
                    break;
            }

            // Our selection point has moved, it should be visible
            m_SelectionPointVisible = true;
            m_AnimationTimeElapsed = sf::Time();

            // Don't do anything when the selection point is on the last line
            if (character < m_Text.getSize())
            {
                // Try to find the closest character
                for ( ; character < m_Text.getSize() + 1; ++character)
                {
                    // Get the position of the character after it
                    newPosition = sf::Vector2u(tempText.findCharacterPos(character + newlinesAdded - newlineAdded + 1));

                    // The character must remain on the same line
                    if (newPosition.y > newTopPosition)
                    {
                        // We have found the character that we were looking for
                        setSelectionPointPosition(character - newlineAdded);
                        return;
                    }

                    // Calculate the distance to the original position
                    distanceX = newPosition.x - originalPosition.x;

                    // Check if the distance is going away again
                    if (abs(distanceX) > previousdistanceX)
                    {
                        // We have found the character that we were looking for
                        setSelectionPointPosition(character - newlineAdded);
                        return;
                    }

                    // Remember the previous distance
                    previousdistanceX = abs(distanceX);
                }

                // If you pass here then the selection point should be at the end of the text
                setSelectionPointPosition(m_Text.getSize());
            }
        }
        else if (event.code == sf::Keyboard::Home)
        {
            // Set the selection point to the beginning of the text
            setSelectionPointPosition(0);

            // Our selection point has moved, it should be visible
            m_SelectionPointVisible = true;
            m_AnimationTimeElapsed = sf::Time();
        }
        else if (event.code == sf::Keyboard::End)
        {
            // Set the selection point behind the text
            setSelectionPointPosition(m_Text.getSize());

            // Our selection point has moved, it should be visible
            m_SelectionPointVisible = true;
            m_AnimationTimeElapsed = sf::Time();
        }
        else if (event.code == sf::Keyboard::Return)
        {
            // Add a newline
            textEntered('\n');
        }
        else if (event.code == sf::Keyboard::BackSpace)
        {
            if (m_readOnly)
                return;

            // Make sure that we didn't select any characters
            if (m_SelChars == 0)
            {
                // We can't delete any characters when you are at the beginning of the string
                if (m_SelEnd == 0)
                    return;

                // Erase the character
                m_Text.erase(m_SelEnd-1, 1);

                // Set the selection point back on the correct position
                setSelectionPointPosition(m_SelEnd - 1);

                if (m_Scroll != nullptr)
                {
                    // Check if the scrollbar is behind the text
                    if (m_Scroll->getValue() > m_Scroll->getMaximum() - m_Scroll->getLowValue())
                    {
                        // Adjust the value of the scrollbar
                        m_Scroll->setValue(m_Scroll->getValue());

                        // The text has to be updated again
                        m_SelectionTextsNeedUpdate = true;
                        updateDisplayedText();
                    }
                }
            }
            else // When you did select some characters then delete them
                deleteSelectedCharacters();

            // The selection point should be visible again
            m_SelectionPointVisible = true;
            m_AnimationTimeElapsed = sf::Time();

            // Add the callback (if the user requested it)
            if (m_CallbackFunctions[TextChanged].empty() == false)
            {
                m_Callback.trigger = TextChanged;
                m_Callback.text    = m_Text;
                addCallback();
            }
        }
        else if (event.code == sf::Keyboard::Delete)
        {
            if (m_readOnly)
                return;

            // Make sure that no text is selected
            if (m_SelChars == 0)
            {
                // When the selection point is at the end of the line then you can't delete anything
                if (m_SelEnd == m_Text.getSize())
                    return;

                // Erase the character
                m_Text.erase(m_SelEnd, 1);

                // Set the selection point back on the correct position
                setSelectionPointPosition(m_SelEnd);

                if (m_Scroll != nullptr)
                {
                    // Check if there is a risk that the scrollbar is going to be behind the text
                    if ((m_Scroll->getValue() == m_Scroll->getMaximum() - m_Scroll->getLowValue()) || (m_Scroll->getValue() > m_Scroll->getMaximum() - m_Scroll->getLowValue() - m_LineHeight))
                    {
                        // Reset the value of the scroll. If it is too high then it will be automatically be adjusted.
                        m_Scroll->setValue(m_Scroll->getValue());

                        // The text has to be updated again
                        m_SelectionTextsNeedUpdate = true;
                        updateDisplayedText();
                    }
                }
            }
            else // You did select some characters, so remove them
                deleteSelectedCharacters();

            // The selection point should be visible again
            m_SelectionPointVisible = true;
            m_AnimationTimeElapsed = sf::Time();

            // Add the callback (if the user requested it)
            if (m_CallbackFunctions[TextChanged].empty() == false)
            {
                m_Callback.trigger = TextChanged;
                m_Callback.text    = m_Text;
                addCallback();
            }
        }
        else
        {
            // Check if you are copying, pasting or cutting text
            if (event.control)
            {
                if (event.code == sf::Keyboard::C)
                {
                    if ((m_TextSelection1.getString() != "") && (m_TextSelection2.getString() != ""))
                        TGUI_Clipboard.set(m_TextSelection1.getString() + "\n" + m_TextSelection2.getString());
                    else
                        TGUI_Clipboard.set(m_TextSelection1.getString() + m_TextSelection2.getString());
                }
                else if (event.code == sf::Keyboard::V)
                {
                    if (m_readOnly)
                        return;

                    auto clipboardContents = TGUI_Clipboard.get();

                    // Only continue pasting if you actually have to do something
                    if ((m_SelChars > 0) || (clipboardContents.getSize() > 0))
                    {
                        deleteSelectedCharacters();

                        unsigned int oldCaretPos = m_SelEnd;

                        if (m_Text.getSize() > m_SelEnd)
                            setText(m_Text.substring(0, m_SelEnd) + TGUI_Clipboard.get() + m_Text.substring(m_SelEnd, m_Text.getSize() - m_SelEnd));
                        else
                            setText(m_Text + clipboardContents);

                        setSelectionPointPosition(oldCaretPos + clipboardContents.getSize());

                        // Add the callback (if the user requested it)
                        if (m_CallbackFunctions[TextChanged].empty() == false)
                        {
                            m_Callback.trigger = TextChanged;
                            m_Callback.text    = m_Text;
                            addCallback();
                        }
                    }
                }
                else if (event.code == sf::Keyboard::X)
                {
                    if ((m_TextSelection1.getString() != "") && (m_TextSelection2.getString() != ""))
                        TGUI_Clipboard.set(m_TextSelection1.getString() + "\n" + m_TextSelection2.getString());
                    else
                        TGUI_Clipboard.set(m_TextSelection1.getString() + m_TextSelection2.getString());

                    if (m_readOnly)
                        return;

                    deleteSelectedCharacters();
                }
                else if (event.code == sf::Keyboard::A)
                {
                    m_SelStart = 0;
                    m_SelEnd = m_Text.getSize();
                    m_SelChars = m_Text.getSize();

                    m_TextBeforeSelection.setString("");
                    m_TextSelection1.setString(m_DisplayedText);
                    m_TextSelection2.setString("");
                    m_TextAfterSelection1.setString("");
                    m_TextAfterSelection2.setString("");

                    updateDisplayedText();
                }
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TextBox::textEntered(sf::Uint32 key)
    {
        // Don't do anything when the text box wasn't loaded correctly
        if (m_Loaded == false)
            return;

        if (m_readOnly)
            return;

        // If there were selected characters then delete them first
        deleteSelectedCharacters();

        // Make sure we don't exceed our maximum characters limit
        if ((m_MaxChars > 0) && (m_Text.getSize() + 1 > m_MaxChars))
                return;

        // If there is a limit in the amount of lines then make a simulation
        if (m_Scroll == nullptr)
        {
            // Don't continue when line height is 0
            if (m_LineHeight == 0)
                return;

            float maxLineWidth = TGUI_MAXIMUM(m_Size.x - 4.0f, 0);

            // Make some preparations
            sf::Text tempText(m_TextBeforeSelection);
            unsigned int beginChar = 0;
            unsigned int newlinesAdded = 0;

            sf::String text = m_Text;
            text.insert(m_SelEnd, key);

            sf::String displayedText = text;
            unsigned int lines = 1;

            // Loop through every character
            for (unsigned i=1; i < text.getSize() + 1; ++i)
            {
                // Make sure the character is not a newline
                if (text[i-1] != '\n')
                {
                    // Add the next character to the text widget
                    tempText.setString(text.substring(beginChar, i - beginChar));

                    // Check if the string still fits on the line
                    if (tempText.findCharacterPos(i).x > maxLineWidth)
                    {
                        // Insert the newline character
                        displayedText.insert(i + newlinesAdded - 1, '\n');

                        // Prepare to find the next line end
                        beginChar = i - 1;
                        ++newlinesAdded;
                        ++lines;
                    }
                }
                else // The character was a newline
                {
                    beginChar = i;
                    ++lines;
                }

                // Check if you passed this limit
                if (lines > m_Size.y / m_LineHeight)
                {
                    // The character can't be added
                    return;
                }
            }
        }

        // Insert our character
        m_Text.insert(m_SelEnd, key);

        // Move our selection point forward
        setSelectionPointPosition(m_SelEnd + 1);

        // The selection point should be visible again
        m_SelectionPointVisible = true;
        m_AnimationTimeElapsed = sf::Time();

        // Add the callback (if the user requested it)
        if (m_CallbackFunctions[TextChanged].empty() == false)
        {
            m_Callback.trigger = TextChanged;
            m_Callback.text    = m_Text;
            addCallback();
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TextBox::mouseWheelMoved(int delta, int, int)
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
                    m_Scroll->setValue(m_Scroll->getValue() + (static_cast<unsigned int>(-delta) * (m_LineHeight / 2)));
                }
                else // You are scrolling up
                {
                    unsigned int change = static_cast<unsigned int>(delta) * (m_LineHeight / 2);

                    // Scroll up
                    if (change < m_Scroll->getValue())
                        m_Scroll->setValue(m_Scroll->getValue() - change);
                    else
                        m_Scroll->setValue(0);
                }
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TextBox::widgetUnfocused()
    {
        // If there is a selection then undo it now
        if (m_SelChars)
            setSelectionPointPosition(m_SelEnd);

        Widget::widgetUnfocused();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool TextBox::setProperty(std::string property, const std::string& value)
    {
        property = toLower(property);

        if (property == "configfile")
        {
            load(value);
        }
        else if (property == "text")
        {
            std::string text;
            decodeString(value, text);
            setText(text);
        }
        else if (property == "textsize")
        {
            setTextSize(atoi(value.c_str()));
        }
        else if (property == "maximumcharacters")
        {
            setMaximumCharacters(atoi(value.c_str()));
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
        else if (property == "textcolor")
        {
            setTextColor(extractColor(value));
        }
        else if (property == "selectedtextcolor")
        {
            setSelectedTextColor(extractColor(value));
        }
        else if (property == "selectedtextbackgroundcolor")
        {
            setSelectedTextBackgroundColor(extractColor(value));
        }
        else if (property == "bordercolor")
        {
            setBorderColor(extractColor(value));
        }
        else if (property == "selectionpointcolor")
        {
            setSelectionPointColor(extractColor(value));
        }
        else if (property == "selectionpointwidth")
        {
            setSelectionPointWidth(atoi(value.c_str()));
        }
        else if (property == "callback")
        {
            Widget::setProperty(property, value);

            std::vector<sf::String> callbacks;
            decodeList(value, callbacks);

            for (auto it = callbacks.begin(); it != callbacks.end(); ++it)
            {
                if ((*it == "TextChanged") || (*it == "textchanged"))
                    bindCallback(TextChanged);
            }
        }
        else // The property didn't match
            return Widget::setProperty(property, value);

        // You pass here when one of the properties matched
        return true;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool TextBox::getProperty(std::string property, std::string& value) const
    {
        property = toLower(property);

        if (property == "configfile")
            value = getLoadedConfigFile();
        else if (property == "text")
            encodeString(getText(), value);
        else if (property == "textsize")
            value = to_string(getTextSize());
        else if (property == "maximumcharacters")
            value = to_string(getMaximumCharacters());
        else if (property == "borders")
            value = "(" + to_string(getBorders().left) + "," + to_string(getBorders().top) + "," + to_string(getBorders().right) + "," + to_string(getBorders().bottom) + ")";
        else if (property == "backgroundcolor")
            value = "(" + to_string(int(getBackgroundColor().r)) + "," + to_string(int(getBackgroundColor().g)) + "," + to_string(int(getBackgroundColor().b)) + "," + to_string(int(getBackgroundColor().a)) + ")";
        else if (property == "textcolor")
            value = "(" + to_string(int(getTextColor().r)) + "," + to_string(int(getTextColor().g)) + "," + to_string(int(getTextColor().b)) + "," + to_string(int(getTextColor().a)) + ")";
        else if (property == "selectedtextcolor")
            value = "(" + to_string(int(getSelectedTextColor().r)) + "," + to_string(int(getSelectedTextColor().g))
                    + "," + to_string(int(getSelectedTextColor().b)) + "," + to_string(int(getSelectedTextColor().a)) + ")";
        else if (property == "selectedtextbackgroundcolor")
            value = "(" + to_string(int(getSelectedTextBackgroundColor().r)) + "," + to_string(int(getSelectedTextBackgroundColor().g))
                    + "," + to_string(int(getSelectedTextBackgroundColor().b)) + "," + to_string(int(getSelectedTextBackgroundColor().a)) + ")";
        else if (property == "bordercolor")
            value = "(" + to_string(int(getBorderColor().r)) + "," + to_string(int(getBorderColor().g)) + "," + to_string(int(getBorderColor().b)) + "," + to_string(int(getBorderColor().a)) + ")";
        else if (property == "selectionpointcolor")
            value = "(" + to_string(int(getSelectionPointColor().r)) + "," + to_string(int(getSelectionPointColor().g)) + "," + to_string(int(getSelectionPointColor().b)) + "," + to_string(int(getSelectionPointColor().a)) + ")";
        else if (property == "selectionpointwidth")
            value = to_string(getSelectionPointWidth());
        else if (property == "callback")
        {
            std::string tempValue;
            Widget::getProperty(property, tempValue);

            std::vector<sf::String> callbacks;

            if ((m_CallbackFunctions.find(TextChanged) != m_CallbackFunctions.end()) && (m_CallbackFunctions.at(TextChanged).size() == 1) && (m_CallbackFunctions.at(TextChanged).front() == nullptr))
                callbacks.push_back("TextChanged");

            encodeList(callbacks, value);

            if (value.empty())
                value = tempValue;
            else if (!tempValue.empty())
                value += "," + tempValue;
        }
        else // The property didn't match
            return Widget::getProperty(property, value);

        // You pass here when one of the properties matched
        return true;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::list< std::pair<std::string, std::string> > TextBox::getPropertyList() const
    {
        auto list = Widget::getPropertyList();
        list.push_back(std::pair<std::string, std::string>("ConfigFile", "string"));
        list.push_back(std::pair<std::string, std::string>("Text", "string"));
        list.push_back(std::pair<std::string, std::string>("TextSize", "uint"));
        list.push_back(std::pair<std::string, std::string>("MaximumCharacters", "uint"));
        list.push_back(std::pair<std::string, std::string>("Borders", "borders"));
        list.push_back(std::pair<std::string, std::string>("BackgroundColor", "color"));
        list.push_back(std::pair<std::string, std::string>("TextColor", "color"));
        list.push_back(std::pair<std::string, std::string>("SelectedTextColor", "color"));
        list.push_back(std::pair<std::string, std::string>("SelectedTextBackgroundColor", "color"));
        list.push_back(std::pair<std::string, std::string>("BorderColor", "color"));
        list.push_back(std::pair<std::string, std::string>("SelectionPointColor", "color"));
        list.push_back(std::pair<std::string, std::string>("SelectionPointWidth", "uint"));
        return list;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int TextBox::findSelectionPointPosition(float posX, float posY)
    {
        // This code will crash when the text box is empty. We need to avoid this.
        if (m_Text.isEmpty())
            return 0;

        // Don't continue when line height is 0
        if (m_LineHeight == 0)
            return 0;

        // Find out on which line you clicked
        unsigned int line;

        // Check if there is a scrollbar
        if (m_Scroll == nullptr)
        {
            // If the position is negative then set the selection point before the first character
            if (posY < 0)
                return 0;
            else
                line = static_cast<unsigned int>(posY / m_LineHeight + 1);
        }
        else // There is no scrollbar
        {
            // If the position is negative then set the selection point before the first character
            if (posY + m_Scroll->getValue() < 0)
                return 0;
            else
                line = static_cast<unsigned int>((posY + m_Scroll->getValue()) / m_LineHeight + 1);
        }

        // Create a temporary text widget that contains the full text
        sf::Text fullText(m_TextBeforeSelection);
        fullText.setString(m_DisplayedText);

        // Check if you clicked behind all characters
        if ((line > m_Lines) || ((line == m_Lines) && (posX > fullText.findCharacterPos(m_DisplayedText.getSize()).x)))
        {
            // The selection point should be behind the last character
            return m_Text.getSize();
        }
        else // You clicked inside the text
        {
            // Prepare to skip part of the text
            sf::String tempString = m_DisplayedText;
            std::size_t newlinePos = 0;

            // Only remove lines when there are lines to remove
            if (line > 1)
            {
                unsigned int i=1;

                // If the first character is a newline then remove it if needed
                if (m_Text[0] == '\n')
                    ++i;

                // Skip the lines above the line where you clicked
                for ( ; i<line; ++i)
                    newlinePos = tempString.find('\n', newlinePos + 1);

                // Remove the first lines
                tempString.erase(0, newlinePos + 1);
            }

            // Only keep one line
            std::size_t newlinePos2 = tempString.find('\n');
            if (newlinePos2 != sf::String::InvalidPos)
                tempString.erase(newlinePos2, sf::String::InvalidPos);

            // Create a temporary text widget
            sf::Text tempText(m_TextBeforeSelection);

            // We are going to calculate the number of newlines we have added
            unsigned int newlinesAdded = 0;
            unsigned int totalNewlinesAdded = 0;
            unsigned int beginChar = 0;
            sf::String   tempString2 = m_Text;

            // Calculate the maximum line width
            float maxLineWidth = m_Size.x - 4.0f;
            if (m_Scroll != nullptr)
                maxLineWidth -= m_Scroll->getSize().x;

            // If the width is negative then the text box is too small to be displayed
            if (maxLineWidth < 0)
                maxLineWidth = 0;

            // Loop through every character
            for (unsigned i = 1; (i < m_Text.getSize() + 1) && (totalNewlinesAdded != line - 1); ++i)
            {
                // Make sure the character is not a newline
                if (m_Text[i-1] != '\n')
                {
                    // Add the next character to the text widget
                    tempText.setString(m_Text.substring(beginChar, i - beginChar));

                    // Check if the string still fits on the line
                    if (tempText.findCharacterPos(i).x > maxLineWidth)
                    {
                        // Insert the newline character
                        tempString2.insert(i + newlinesAdded - 1, '\n');

                        // Prepare to find the next line end
                        beginChar = i - 1;
                        ++newlinesAdded;
                        ++totalNewlinesAdded;
                    }
                }
                else // The character was a newline
                {
                    beginChar = i;
                    ++totalNewlinesAdded;
                }
            }

            // Store the single line that we found a while ago in the temporary text widget
            tempText.setString(tempString);

            // If the line contains nothing but a newline character then put the selction point on that line
            if (tempString.getSize() == 0)
            {
                if (line > 1)
                    return newlinePos - newlinesAdded + 1;
                else
                    return newlinePos;
            }

            // Check if the click occured before the first character
            if ((tempString.getSize() == 1) && (posX < (tempText.findCharacterPos(1).x / 2.f)))
            {
                if (line > 1)
                    return newlinePos - newlinesAdded;
                else
                {
                    if (newlinePos > 0)
                        return newlinePos - 1;
                    else
                        return 0;
                }
            }

            // Try to find between which characters the mouse is standing
            for (unsigned int i = 1; i <= tempString.getSize(); ++i)
            {
                if (posX < (tempText.findCharacterPos(i-1).x + tempText.findCharacterPos(i).x) / 2.f)
                {
                    if (line > 1)
                        return newlinePos + i - newlinesAdded;
                    else
                        return newlinePos + i - 1;
                }

            }

            // The mouse is on the second half of the last character on the line
            if (line > 1)
                return newlinePos + tempString.getSize() + 1 - newlinesAdded;
            else
                return newlinePos + tempString.getSize();
        }

        // Something went wrong, don't move the selection point position
        return m_SelEnd;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TextBox::selectText(float posX, float posY)
    {
        // Don't continue when line height is 0
        if (m_LineHeight == 0)
            return;

        // Find out where the selection point should be
        m_SelEnd = findSelectionPointPosition(posX - getPosition().x - 4, posY - getPosition().y);

        // Calculate how many character are being selected
        if (m_SelEnd < m_SelStart)
            m_SelChars = m_SelStart - m_SelEnd;
        else
            m_SelChars = m_SelEnd - m_SelStart;

        // Update the text
        m_SelectionTextsNeedUpdate = true;
        updateDisplayedText();

        // Check if there is a scrollbar
        if (m_Scroll != nullptr)
        {
            unsigned int newlines = 0;
            unsigned int newlinesAdded = 0;
            unsigned int totalLines = 0;

            // Loop through all characters
            for (unsigned int i=0; i<m_SelEnd; ++i)
            {
                // Make sure there is no newline in the text
                if (m_Text[i] != '\n')
                {
                    // Check if there is a newline in the displayed text
                    if (m_DisplayedText[i + newlinesAdded] == '\n')
                    {
                        // A newline was added here
                        ++newlinesAdded;
                        ++totalLines;

                        if (i < m_SelEnd)
                            ++newlines;
                    }
                }
                else // The text already contains a newline
                {
                    ++totalLines;

                    if (i < m_SelEnd)
                        ++newlines;
                }
            }

            // Check if the selection point is located above the view
            if ((newlines < m_TopLine - 1) || ((newlines < m_TopLine) && (m_Scroll->getValue() % m_LineHeight > 0)))
            {
                m_Scroll->setValue(newlines * m_LineHeight);
                updateDisplayedText();
            }

            // Check if the selection point is below the view
            else if (newlines > m_TopLine + m_VisibleLines - 2)
            {
                m_Scroll->setValue((newlines - m_VisibleLines + 1) * m_LineHeight);
                updateDisplayedText();
            }
            else if ((newlines > m_TopLine + m_VisibleLines - 3) && (m_Scroll->getValue() % m_LineHeight > 0))
            {
                m_Scroll->setValue((newlines - m_VisibleLines + 2) * m_LineHeight);
                updateDisplayedText();
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TextBox::deleteSelectedCharacters()
    {
        // Nothing to delete when no text was selected
        if (m_SelChars == 0)
            return;

        // Erase the characters
        m_Text.erase(TGUI_MINIMUM(m_SelStart, m_SelEnd), m_SelChars);

        // Set the selection point back on the correct position
        setSelectionPointPosition(TGUI_MINIMUM(m_SelStart, m_SelEnd));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TextBox::updateDisplayedText()
    {
        // Don't continue when the text box wasn't loaded correctly
        if (m_Loaded == false)
            return;

        // Don't continue when line height is 0
        if (m_LineHeight == 0)
            return;

        float maxLineWidth = m_Size.x - 4.0f;
        if (m_Scroll != nullptr)
            maxLineWidth -= m_Scroll->getSize().x;

        // If the width is negative then the text box is too small to be displayed
        if (maxLineWidth < 0)
            maxLineWidth = 0;

        // Make some preparations
        sf::Text tempText(m_TextBeforeSelection);
        unsigned int beginChar = 0;
        unsigned int newlinesAdded = 0;
        unsigned int newlinesAddedBeforeSelection = 0;

        m_DisplayedText = m_Text;
        m_Lines = 1;

        // Loop through every character
        for (unsigned i=1; i < m_Text.getSize() + 1; ++i)
        {
            // Make sure the character is not a newline
            if (m_Text[i-1] != '\n')
            {
                // Add the next character to the text widget
                tempText.setString(m_Text.substring(beginChar, i - beginChar));

                // Check if the string still fits on the line
                if (tempText.findCharacterPos(i).x > maxLineWidth)
                {
                    // Insert the newline character
                    m_DisplayedText.insert(i + newlinesAdded - 1, '\n');

                    // Prepare to find the next line end
                    beginChar = i - 1;
                    ++newlinesAdded;
                    ++m_Lines;
                }
            }
            else // The character was a newline
            {
                beginChar = i;
                ++m_Lines;
            }

            // Find the position of the selection point
            if (m_SelEnd == i - 1)
                newlinesAddedBeforeSelection = newlinesAdded;

            // Check if there is a limit in the amount of lines
            if (m_Scroll == nullptr)
            {
                // Check if you passed this limit
                if (m_Lines > m_Size.y / m_LineHeight)
                {
                    // Remove all exceeding lines
                    m_DisplayedText.erase(i + newlinesAdded - 1, sf::String::InvalidPos);
                    m_Text.erase(i-1, sf::String::InvalidPos);

                    // We counted one line too much
                    --m_Lines;

                    break;
                }
            }
        }

        // Find the position of the selection point
        if (m_SelEnd == m_Text.getSize())
            newlinesAddedBeforeSelection = newlinesAdded;

        // Check if there is a scrollbar
        if (m_Scroll != nullptr)
        {
            // Tell the scrollbar how many pixels the text contains
            m_Scroll->setMaximum(m_Lines * m_LineHeight);

            // Calculate the top line
            m_TopLine = m_Scroll->getValue() / m_LineHeight + 1;

            // Calculate the number of visible lines
            if ((m_Scroll->getValue() % m_LineHeight) == 0)
                m_VisibleLines = TGUI_MINIMUM(m_Size.y / m_LineHeight, m_Lines);
            else
                m_VisibleLines = TGUI_MINIMUM((m_Size.y / m_LineHeight) + 1, m_Lines);
        }
        else // There is no scrollbar
        {
            // You are always at the top line
            m_TopLine = 1;

            // Calculate the number of visible lines
            m_VisibleLines = TGUI_MINIMUM(m_Size.y / m_LineHeight, m_Lines);
        }

        // Fill the temporary text widget with the whole text
        tempText.setString(m_DisplayedText);

        // Set the position of the selection point
        m_SelectionPointPosition = sf::Vector2u(tempText.findCharacterPos(m_SelEnd + newlinesAddedBeforeSelection));

        // Only do the check when the selection point is not standing at the first character
        if ((m_SelEnd > 0) && (m_SelEnd + newlinesAddedBeforeSelection > 0))
        {
            // If you are at the end of the line then also set the selection point there, instead of at the beginning of the next line
            if ((m_Text[m_SelEnd - 1] != '\n') && (m_DisplayedText[m_SelEnd + newlinesAddedBeforeSelection - 1] == '\n'))
                m_SelectionPointPosition = sf::Vector2u(tempText.findCharacterPos(m_SelEnd + newlinesAddedBeforeSelection - 1));
        }

        // Check if the text has to be redivided in five pieces
        if (m_SelectionTextsNeedUpdate)
            updateSelectionTexts(maxLineWidth);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TextBox::updateSelectionTexts(float maxLineWidth)
    {
        // If there is no selection then just put the whole text in m_TextBeforeSelection
        if (m_SelChars == 0)
        {
            m_TextBeforeSelection.setString(m_DisplayedText);
            m_TextSelection1.setString("");
            m_TextSelection2.setString("");
            m_TextAfterSelection1.setString("");
            m_TextAfterSelection2.setString("");

            // Clear the list of selection rectangle sizes
            m_MultilineSelectionRectWidth.clear();
        }
        else // Some text is selected
        {
            // Make some preparations
            unsigned i;
            unsigned int selectionStart = TGUI_MINIMUM(m_SelEnd, m_SelStart);
            unsigned int selectionEnd = TGUI_MAXIMUM(m_SelEnd, m_SelStart);

            sf::Text tempText(m_TextBeforeSelection);
            unsigned int beginChar = 0;
            unsigned int newlinesAddedBeforeSelection = 0;
            unsigned int newlinesAddedInsideSelection = 0;
            unsigned int lastNewlineBeforeSelection = 0;
            bool newlineFoundInsideSelection = false;

            // Clear the list of selection rectangle sizes
            m_MultilineSelectionRectWidth.clear();

            // Loop through every character before the selection
            for (i=0; i < selectionStart; ++i)
            {
                // Make sure the character is not a newline
                if (m_Text[i] != '\n')
                {
                    // Add the next character to the text widget
                    tempText.setString(m_Text.substring(beginChar, i - beginChar + 1));

                    // Check if the string still fits on the line
                    if (tempText.findCharacterPos(i+1).x > maxLineWidth)
                    {
                        beginChar = i;
                        lastNewlineBeforeSelection = i;
                        ++newlinesAddedBeforeSelection;
                    }
                }
                else // The character was a newline
                {
                    beginChar = i+1;
                    lastNewlineBeforeSelection = i;
                }
            }

            // Reset the position from where we should start looking for newlines
            beginChar = lastNewlineBeforeSelection;

            // Loop through every selected character
            for (i=selectionStart; i < selectionEnd; ++i)
            {
                // Make sure the character is not a newline
                if (m_Text[i] != '\n')
                {
                    // Add the next character to the text widget
                    tempText.setString(m_Text.substring(beginChar, i - beginChar + 1));

                    // Check if the string still fits on the line
                    if (tempText.findCharacterPos(i+1).x > maxLineWidth)
                    {
                        beginChar = i;
                        ++newlinesAddedInsideSelection;
                    }
                }
                else // The character was a newline
                    beginChar = i+1;
            }

            // Find out where the selection starts and where it ends
            if (m_SelEnd < m_SelStart)
            {
                selectionStart = m_SelEnd + newlinesAddedBeforeSelection;
                selectionEnd = m_SelStart + newlinesAddedBeforeSelection + newlinesAddedInsideSelection;
            }
            else
            {
                selectionStart = m_SelStart + newlinesAddedBeforeSelection;
                selectionEnd = m_SelEnd + newlinesAddedBeforeSelection + newlinesAddedInsideSelection;
            }

            // Create another temprary text widget
            sf::Text tempText2(m_TextBeforeSelection);
            tempText2.setString(m_DisplayedText);

            // Loop through the selected characters again
            for (i=selectionStart; i<selectionEnd; ++i)
            {
                if (m_DisplayedText[i] == '\n')
                {
                    // Check if this is not the first newline
                    if (newlineFoundInsideSelection == true)
                    {
                        // Add a new rectangle to the selection
                        if (tempText2.findCharacterPos(i).x > 0)
                            m_MultilineSelectionRectWidth.push_back(tempText2.findCharacterPos(i).x);
                        else
                            m_MultilineSelectionRectWidth.push_back(2);
                    }
                    else // This is the first newline, skip it
                        newlineFoundInsideSelection = true;
                }
            }

            // Add the last selection rectangle
            m_MultilineSelectionRectWidth.push_back(tempText2.findCharacterPos(i).x);

            // Set the text before selection
            m_TextBeforeSelection.setString(m_DisplayedText.substring(0, selectionStart));

            // Set the text that is selected. If it consists of multiple lines then it will be changed below.
            m_TextSelection1.setString(m_DisplayedText.substring(selectionStart, m_SelChars));
            m_TextSelection2.setString("");

            // Loop through every character inside the selection
            for (i=selectionStart; i < selectionEnd; ++i)
            {
                // Check if the character is a newline
                if (m_DisplayedText[i] == '\n')
                {
                    // Set the text that is selected
                    m_TextSelection1.setString(m_DisplayedText.substring(selectionStart, i - selectionStart));
                    m_TextSelection2.setString(m_DisplayedText.substring(i + 1, m_SelChars + newlinesAddedInsideSelection + selectionStart - i - 1));
                    break;
                }
            }

            // Set the text after the selection. If it consists of multiple lines then it will be changed below.
            m_TextAfterSelection1.setString(m_DisplayedText.substring(selectionEnd, m_DisplayedText.getSize() - selectionEnd));
            m_TextAfterSelection2.setString("");

            // Loop through every character after the selection
            for (i=selectionEnd; i < m_DisplayedText.getSize(); ++i)
            {
                // Check if the character is a newline
                if (m_DisplayedText[i] == '\n')
                {
                    // Set the text that is selected
                    m_TextAfterSelection1.setString(m_DisplayedText.substring(selectionEnd, i - selectionEnd));
                    m_TextAfterSelection2.setString(m_DisplayedText.substring(i + 1, m_DisplayedText.getSize() - i - 1));
                    break;
                }
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TextBox::initialize(Container *const parent)
    {
        m_Parent = parent;
        setTextFont(m_Parent->getGlobalFont());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TextBox::update()
    {
        // Only show/hide the selection point every half second
        if (m_AnimationTimeElapsed < sf::milliseconds(500))
            return;

        // Reset the elapsed time
        m_AnimationTimeElapsed = sf::Time();

        // Only update when the editbox is visible
        if (m_Visible == false)
            return;

        // Switch the value of the visible flag
        m_SelectionPointVisible = !m_SelectionPointVisible;

        // Too slow for double clicking
        m_PossibleDoubleClick = false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TextBox::draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        // Don't draw anything when the text box wasn't loaded correctly
        if (m_Loaded == false)
            return;

        const sf::View& view = target.getView();

        // Calculate the scale factor of the view
        float scaleViewX = target.getSize().x / view.getSize().x;
        float scaleViewY = target.getSize().y / view.getSize().y;

        // Get the global position
        sf::Vector2f topLeftPosition = sf::Vector2f(((getAbsolutePosition().x - view.getCenter().x + (view.getSize().x / 2.f)) * view.getViewport().width) + (view.getSize().x * view.getViewport().left),
                                                    ((getAbsolutePosition().y - view.getCenter().y + (view.getSize().y / 2.f)) * view.getViewport().height) + (view.getSize().y * view.getViewport().top));
        sf::Vector2f bottomRightPosition = sf::Vector2f((getAbsolutePosition().x + m_Size.x - view.getCenter().x + (view.getSize().x / 2.f)) * view.getViewport().width + (view.getSize().x * view.getViewport().left),
                                                        (getAbsolutePosition().y + m_Size.y - view.getCenter().y + (view.getSize().y / 2.f)) * view.getViewport().height + (view.getSize().y * view.getViewport().top));

        // Adjust the transformation
        states.transform *= getTransform();

        // Store the current transform
        sf::Transform origTransform = states.transform;

        // Draw the borders
        {
            // Draw left border
            sf::RectangleShape border(sf::Vector2f(static_cast<float>(m_LeftBorder), static_cast<float>(m_Size.y + m_TopBorder)));
            border.setPosition(-static_cast<float>(m_LeftBorder), -static_cast<float>(m_TopBorder));
            border.setFillColor(m_BorderColor);
            target.draw(border, states);

            // Draw top border
            border.setSize(sf::Vector2f(static_cast<float>(m_Size.x + m_RightBorder), static_cast<float>(m_TopBorder)));
            border.setPosition(0, -static_cast<float>(m_TopBorder));
            target.draw(border, states);

            // Draw right border
            border.setSize(sf::Vector2f(static_cast<float>(m_RightBorder), static_cast<float>(m_Size.y + m_BottomBorder)));
            border.setPosition(static_cast<float>(m_Size.x), 0);
            target.draw(border, states);

            // Draw bottom border
            border.setSize(sf::Vector2f(static_cast<float>(m_Size.x + m_LeftBorder), static_cast<float>(m_BottomBorder)));
            border.setPosition(-static_cast<float>(m_LeftBorder), static_cast<float>(m_Size.y));
            target.draw(border, states);
        }

        // Draw the background
        sf::RectangleShape front(sf::Vector2f(static_cast<float>(m_Size.x), static_cast<float>(m_Size.y)));
        front.setFillColor(m_BackgroundColor);
        target.draw(front, states);

        // Set the text on the correct position
        if (m_Scroll != nullptr)
            states.transform.translate(2, -static_cast<float>(m_Scroll->getValue()));
        else
            states.transform.translate(2, 0);

        // Remeber this tranformation
        sf::Transform oldTransform = states.transform;

        // Get the old clipping area
        GLint scissor[4];
        glGetIntegerv(GL_SCISSOR_BOX, scissor);

        // Calculate the clipping area
        GLint scissorLeft = TGUI_MAXIMUM(static_cast<GLint>(topLeftPosition.x * scaleViewX), scissor[0]);
        GLint scissorTop = TGUI_MAXIMUM(static_cast<GLint>(topLeftPosition.y * scaleViewY), static_cast<GLint>(target.getSize().y) - scissor[1] - scissor[3]);
        GLint scissorRight = TGUI_MINIMUM(static_cast<GLint>(bottomRightPosition.x * scaleViewX), scissor[0] + scissor[2]);
        GLint scissorBottom = TGUI_MINIMUM(static_cast<GLint>(bottomRightPosition.y * scaleViewY), static_cast<GLint>(target.getSize().y) - scissor[1]);

        // If the widget outside the window then don't draw anything
        if (scissorRight < scissorLeft)
            scissorRight = scissorLeft;
        else if (scissorBottom < scissorTop)
            scissorTop = scissorBottom;

        // Set the clipping area
        glScissor(scissorLeft, target.getSize().y - scissorBottom, scissorRight - scissorLeft, scissorBottom - scissorTop);

        sf::Text tempText(m_TextBeforeSelection);
        tempText.setString("kg");
        sf::Vector2f textShift(tempText.getLocalBounds().left, tempText.getLocalBounds().top);

        // Draw the text
        states.transform.translate(-textShift.x, -textShift.y);
        target.draw(m_TextBeforeSelection, states);
        states.transform.translate(0, textShift.y);

        // Check if there is a selection
        if (m_SelChars > 0)
        {
            // Store the lenghts of the texts
            unsigned int textBeforeSelectionLength = m_TextBeforeSelection.getString().getSize() + 1;
            unsigned int textSelection1Length = m_TextSelection1.getString().getSize() + 1;
            unsigned int textSelection2Length = m_TextSelection2.getString().getSize() + 1;

            // Set the text on the correct position
            states.transform.translate(m_TextBeforeSelection.findCharacterPos(textBeforeSelectionLength).x, m_TextBeforeSelection.findCharacterPos(textBeforeSelectionLength).y);

            // Watch out for kerning
            if (textBeforeSelectionLength > 1)
                states.transform.translate(static_cast<float>(m_TextBeforeSelection.getFont()->getKerning(m_DisplayedText[textBeforeSelectionLength-2], m_DisplayedText[textBeforeSelectionLength-1], m_TextSize)), 0);

            // Create the selection background
            sf::RectangleShape selectionBackground1(sf::Vector2f(m_TextSelection1.findCharacterPos(textSelection1Length).x, static_cast<float>(m_LineHeight)));
            selectionBackground1.setFillColor(m_SelectedTextBgrColor);

            // Draw the selection background
            target.draw(selectionBackground1, states);

            // Draw the first part of the selected text
            states.transform.translate(0, -textShift.y);
            target.draw(m_TextSelection1, states);
            states.transform.translate(0, textShift.y);

            // Check if there is a second part in the selection
            if (m_TextSelection2.getString().getSize() > 0)
            {
                // Translate to the beginning of the next line
                states.transform.translate(-m_TextBeforeSelection.findCharacterPos(textBeforeSelectionLength).x, static_cast<float>(m_LineHeight));

                // If there was a kerning correction then undo it now
                if (textBeforeSelectionLength > 1)
                    states.transform.translate(static_cast<float>(-m_TextBeforeSelection.getFont()->getKerning(m_DisplayedText[textBeforeSelectionLength-2], m_DisplayedText[textBeforeSelectionLength-1], m_TextSize)), 0);

                // Create the second selection background
                sf::RectangleShape selectionBackground2;
                selectionBackground2.setFillColor(m_SelectedTextBgrColor);

                // Draw the background rectangles of the selected text
                for (unsigned int i=0; i<m_MultilineSelectionRectWidth.size(); ++i)
                {
                    selectionBackground2.setSize(sf::Vector2f(m_MultilineSelectionRectWidth[i], static_cast<float>(m_LineHeight)));
                    target.draw(selectionBackground2, states);
                    selectionBackground2.move(0, static_cast<float>(m_LineHeight));
                }

                // Draw the second part of the selection
                states.transform.translate(0, -textShift.y);
                target.draw(m_TextSelection2, states);
                states.transform.translate(0, textShift.y);

                // Translate to the end of the selection
                states.transform.translate(m_TextSelection2.findCharacterPos(textSelection2Length));

                // Watch out for kerning
                if (m_DisplayedText.getSize() > textBeforeSelectionLength + textSelection1Length + textSelection2Length - 2)
                    states.transform.translate(static_cast<float>(m_TextBeforeSelection.getFont()->getKerning(m_DisplayedText[textBeforeSelectionLength + textSelection1Length + textSelection2Length - 3], m_DisplayedText[textBeforeSelectionLength + textSelection1Length + textSelection2Length - 2], m_TextSize)), 0);
            }
            else // The selection was only on one line
            {
                // Translate to the end of the selection
                states.transform.translate(m_TextSelection1.findCharacterPos(textSelection1Length).x, 0);

                // Watch out for kerning
                if ((m_DisplayedText.getSize() > textBeforeSelectionLength + textSelection1Length - 2) && (textBeforeSelectionLength + textSelection1Length > 2))
                    states.transform.translate(static_cast<float>(m_TextBeforeSelection.getFont()->getKerning(m_DisplayedText[textBeforeSelectionLength + textSelection1Length - 3], m_DisplayedText[textBeforeSelectionLength + textSelection1Length - 2], m_TextSize)), 0);
            }

            // Draw the first part of the text behind the selection
            states.transform.translate(0, -textShift.y);
            target.draw(m_TextAfterSelection1, states);
            states.transform.translate(textShift.x, textShift.y);

            // Check if there is a second part in the selection
            if (m_TextAfterSelection2.getString().getSize() > 0)
            {
                // Translate to the beginning of the next line
                if (m_TextSelection2.getString().getSize() > 0)
                {
                    // Undo the last translation
                    states.transform.translate(-m_TextSelection2.findCharacterPos(textSelection2Length).x, static_cast<float>(m_LineHeight));

                    // If there was a kerning correction then undo it now
                    if (m_DisplayedText.getSize() > textBeforeSelectionLength + textSelection1Length + textSelection2Length - 2)
                        states.transform.translate(static_cast<float>(-m_TextBeforeSelection.getFont()->getKerning(m_DisplayedText[textBeforeSelectionLength + textSelection1Length + textSelection2Length - 3], m_DisplayedText[textBeforeSelectionLength + textSelection1Length + textSelection2Length - 2], m_TextSize)), 0);
                }
                else
                {
                    // Undo the last translation
                    states.transform.translate(-m_TextSelection1.findCharacterPos(textSelection1Length).x - m_TextBeforeSelection.findCharacterPos(textBeforeSelectionLength).x, static_cast<float>(m_LineHeight));

                    // If there was a kerning correction then undo it now
                    if (textBeforeSelectionLength > 1)
                        states.transform.translate(static_cast<float>(-m_TextBeforeSelection.getFont()->getKerning(m_DisplayedText[textBeforeSelectionLength-2], m_DisplayedText[textBeforeSelectionLength-1], m_TextSize)), 0);

                    // If there was a kerning correction then undo it now
                    if ((m_DisplayedText.getSize() > textBeforeSelectionLength + textSelection1Length - 2) && (textBeforeSelectionLength + textSelection1Length > 2))
                        states.transform.translate(static_cast<float>(-m_TextBeforeSelection.getFont()->getKerning(m_DisplayedText[textBeforeSelectionLength + textSelection1Length - 3], m_DisplayedText[textBeforeSelectionLength + textSelection1Length - 2], m_TextSize)), 0);
                }

                // Draw the second part of the text after the selection
                states.transform.translate(-textShift.x, -textShift.y);
                target.draw(m_TextAfterSelection2, states);
                states.transform.translate(textShift.x, textShift.y);
            }
        }

        // Only draw the selection point if it has a width
        if (m_SelectionPointWidth > 0)
        {
            // Only draw it when needed
            if ((m_Focused) && (m_SelectionPointVisible))
            {
                // Reset the transformation
                states.transform = oldTransform;
                states.transform.translate(-textShift.x, 0);

                // Create the selection point rectangle
                sf::RectangleShape selectionPoint(sf::Vector2f(static_cast<float>(m_SelectionPointWidth), static_cast<float>(m_LineHeight)));
                selectionPoint.setPosition(m_SelectionPointPosition.x - (m_SelectionPointWidth * 0.5f), static_cast<float>(m_SelectionPointPosition.y));
                selectionPoint.setFillColor(m_SelectionPointColor);

                // Draw the selection point
                target.draw(selectionPoint, states);
            }
        }

        // Reset the old clipping area
        glScissor(scissor[0], scissor[1], scissor[2], scissor[3]);

        // Check if there is a scrollbar
        if (m_Scroll != nullptr)
        {
            // Reset the transformation
            states.transform = origTransform;
            states.transform.translate(m_Size.x - m_Scroll->getSize().x, 0);

            // Draw the scrollbar
            target.draw(*m_Scroll, states);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
