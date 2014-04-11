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
    m_size                    (360, 200),
    m_text                    (""),
    m_displayedText           (""),
    m_textSize                (30),
    m_lineHeight              (40),
    m_lines                   (1),
    m_maxChars                (0),
    m_topLine                 (1),
    m_visibleLines            (1),
    m_selChars                (0),
    m_selStart                (0),
    m_selEnd                  (0),
    m_caretPosition           (0, 0),
    m_caretVisible            (true),
    m_caretColor              (110, 110, 255),
    m_caretWidth              (2),
    m_selectionTextsNeedUpdate(true),
    m_scroll                  (nullptr),
    m_possibleDoubleClick     (false),
    m_readOnly                (false)
    {
        m_callback.widgetType = Type_TextBox;
        m_animatedWidget = true;
        m_draggableWidget = true;

        changeColors();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    TextBox::TextBox(const TextBox& copy) :
    Widget                       (copy),
    WidgetBorders                (copy),
    m_loadedConfigFile           (copy.m_loadedConfigFile),
    m_size                       (copy.m_size),
    m_text                       (copy.m_text),
    m_displayedText              (copy.m_displayedText),
    m_textSize                   (copy.m_textSize),
    m_lineHeight                 (copy.m_lineHeight),
    m_lines                      (copy.m_lines),
    m_maxChars                   (copy.m_maxChars),
    m_topLine                    (copy.m_topLine),
    m_visibleLines               (copy.m_visibleLines),
    m_selChars                   (copy.m_selChars),
    m_selStart                   (copy.m_selStart),
    m_selEnd                     (copy.m_selEnd),
    m_caretPosition              (copy.m_caretPosition),
    m_caretVisible               (copy.m_caretVisible),
    m_caretColor                 (copy.m_caretColor),
    m_caretWidth                 (copy.m_caretWidth),
    m_selectionTextsNeedUpdate   (copy.m_selectionTextsNeedUpdate),
    m_backgroundColor            (copy.m_backgroundColor),
    m_selectedTextBgrColor       (copy.m_selectedTextBgrColor),
    m_borderColor                (copy.m_borderColor),
    m_textBeforeSelection        (copy.m_textBeforeSelection),
    m_textSelection1             (copy.m_textSelection1),
    m_textSelection2             (copy.m_textSelection2),
    m_textAfterSelection1        (copy.m_textAfterSelection1),
    m_textAfterSelection2        (copy.m_textAfterSelection2),
    m_multilineSelectionRectWidth(copy.m_multilineSelectionRectWidth),
    m_possibleDoubleClick        (copy.m_possibleDoubleClick),
    m_readOnly                   (copy.m_readOnly)
    {
        // If there is a scrollbar then copy it
        if (copy.m_scroll != nullptr)
            m_scroll = new Scrollbar(*copy.m_scroll);
        else
            m_scroll = nullptr;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    TextBox::~TextBox()
    {
        if (m_scroll != nullptr)
            delete m_scroll;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    TextBox& TextBox::operator= (const TextBox& right)
    {
        if (this != &right)
        {
            // If there already was a scrollbar then delete it now
            if (m_scroll != nullptr)
            {
                delete m_scroll;
                m_scroll = nullptr;
            }

            TextBox temp(right);
            this->Widget::operator=(right);
            this->WidgetBorders::operator=(right);

            std::swap(m_loadedConfigFile,            temp.m_loadedConfigFile);
            std::swap(m_size,                        temp.m_size);
            std::swap(m_text,                        temp.m_text);
            std::swap(m_displayedText,               temp.m_displayedText);
            std::swap(m_textSize,                    temp.m_textSize);
            std::swap(m_lineHeight,                  temp.m_lineHeight);
            std::swap(m_lines,                       temp.m_lines);
            std::swap(m_maxChars,                    temp.m_maxChars);
            std::swap(m_topLine,                     temp.m_topLine);
            std::swap(m_visibleLines,                temp.m_visibleLines);
            std::swap(m_selChars,                    temp.m_selChars);
            std::swap(m_selStart,                    temp.m_selStart);
            std::swap(m_selEnd,                      temp.m_selEnd);
            std::swap(m_caretPosition,               temp.m_caretPosition);
            std::swap(m_caretVisible,                temp.m_caretVisible);
            std::swap(m_caretColor,                  temp.m_caretColor);
            std::swap(m_caretWidth,                  temp.m_caretWidth);
            std::swap(m_selectionTextsNeedUpdate,    temp.m_selectionTextsNeedUpdate);
            std::swap(m_backgroundColor,             temp.m_backgroundColor);
            std::swap(m_selectedTextBgrColor,        temp.m_selectedTextBgrColor);
            std::swap(m_borderColor,                 temp.m_borderColor);
            std::swap(m_textBeforeSelection,         temp.m_textBeforeSelection);
            std::swap(m_textSelection1,              temp.m_textSelection1);
            std::swap(m_textSelection2,              temp.m_textSelection2);
            std::swap(m_textAfterSelection1,         temp.m_textAfterSelection1);
            std::swap(m_textAfterSelection2,         temp.m_textAfterSelection2);
            std::swap(m_multilineSelectionRectWidth, temp.m_multilineSelectionRectWidth);
            std::swap(m_scroll,                      temp.m_scroll);
            std::swap(m_possibleDoubleClick,         temp.m_possibleDoubleClick);
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

    void TextBox::load(const std::string& configFileFilename)
    {
        m_loadedConfigFile = getResourcePath() + configFileFilename;

        // If there already was a scrollbar then delete it now
        if (m_scroll != nullptr)
        {
            delete m_scroll;
            m_scroll = nullptr;
        }

        // Open the config file
        ConfigFile configFile(m_loadedConfigFile, "TextBox");

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
            else if (it->first == "textcolor")
            {
                setTextColor(configFile.readColor(it));
            }
            else if (it->first == "selectedtextbackgroundcolor")
            {
                setSelectedTextBackgroundColor(configFile.readColor(it));
            }
            else if (it->first == "selectedtextcolor")
            {
                setSelectedTextColor(configFile.readColor(it));
            }
            else if (it->first == "caretcolor")
            {
                setCaretColor(configFile.readColor(it));
            }
            else if (it->first == "bordercolor")
            {
                setBorderColor(configFile.readColor(it));
            }
            else if (it->first == "borders")
            {
                Borders borders;
                if (extractBorders(it->second, borders))
                    setBorders(borders.left, borders.top, borders.right, borders.bottom);
                else
                    throw Exception("Failed to parse the 'Borders' property in section TextBox in " + m_loadedConfigFile);
            }
            else if (it->first == "scrollbar")
            {
                if ((it->second.length() < 3) || (it->second[0] != '"') || (it->second[it->second.length()-1] != '"'))
                    throw Exception("Failed to parse value for Scrollbar in section TextBox in " + m_loadedConfigFile + ".");

                try
                {
                    // load the scrollbar
                    m_scroll = new Scrollbar();
                    m_scroll->load(configFileFolder + it->second.substr(1, it->second.length()-2));
                }
                catch (const Exception& e)
                {
                    // The scrollbar couldn't be loaded so it must be deleted
                    delete m_scroll;
                    m_scroll = nullptr;

                    throw Exception("Failed to create the internal scrollbar in TextBox. " + std::string(e.what()));
                }

                // Initialize the scrollbar
                m_scroll->setVerticalScroll(true);
                m_scroll->setLowValue(m_size.y);
                m_scroll->setSize(m_scroll->getSize().x, static_cast<float>(m_size.y));
            }
            else
                throw Exception("Unrecognized property '" + it->first + "' in section TextBox in " + m_loadedConfigFile + ".");
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const std::string& TextBox::getLoadedConfigFile() const
    {
        return m_loadedConfigFile;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TextBox::setSize(float width, float height)
    {
        // Don't continue when line height is 0
        if (m_lineHeight == 0)
            return;

        // A negative size is not allowed for this widget
        if (width  < 0) width  = -width;
        if (height < 0) height = -height;

        // There is a minimum width
        if (m_scroll == nullptr)
            width = TGUI_MAXIMUM(50, width);
        else
            width = TGUI_MAXIMUM(50 + m_scroll->getSize().x, width);

        // There is also a minimum height
        if (m_scroll == nullptr)
        {
            // If there is a text then it should still fit inside the text box
            if (m_text.getSize() > 0)
            {
                if (m_size.y < (m_lines * m_lineHeight))
                    m_size.y = (m_lines * m_lineHeight);
            }
            else // There are no items
            {
                // At least one item should fit inside the text box
                if (m_size.y < m_lineHeight)
                    m_size.y = m_lineHeight;
            }
        }
        else // There is a scrollbar
        {
            // At least one item should fit inside the text box
            if (m_size.y < m_lineHeight)
                m_size.y = m_lineHeight;
        }

        // There is also a minimum height
        if (height < m_lineHeight)
        {
            height = static_cast<float>(m_lineHeight);
        }

        // Store the values
        m_size.x = static_cast<unsigned int>(width);
        m_size.y = static_cast<unsigned int>(height);

        // If there is a scrollbar then reinitialize it
        if (m_scroll != nullptr)
        {
            m_scroll->setLowValue(m_size.y);
            m_scroll->setSize(m_scroll->getSize().x, static_cast<float>(m_size.y));
        }

        // The size of the textbox has changed, update the text
        m_selectionTextsNeedUpdate = true;
        updateDisplayedText();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    sf::Vector2f TextBox::getSize() const
    {
        return sf::Vector2f(static_cast<float>(m_size.x), static_cast<float>(m_size.y));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    sf::Vector2f TextBox::getFullSize() const
    {
        return sf::Vector2f(static_cast<float>(m_size.x + m_borders.left + m_borders.right),
                            static_cast<float>(m_size.y + m_borders.top + m_borders.bottom));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TextBox::setText(const sf::String& text)
    {
        // Store the text
        m_text = text;

        // Set the caret behind the last character
        setCaretPosition(m_text.getSize());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TextBox::addText(const sf::String& text)
    {
        // Add the text
        m_text += text;

        // Set the caret behind the last character
        setCaretPosition(m_text.getSize());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    sf::String TextBox::getText() const
    {
        return m_text;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TextBox::setTextFont(const sf::Font& font)
    {
        m_textBeforeSelection.setFont(font);
        m_textSelection1.setFont(font);
        m_textSelection2.setFont(font);
        m_textAfterSelection1.setFont(font);
        m_textAfterSelection2.setFont(font);

        setTextSize(getTextSize());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const sf::Font* TextBox::getTextFont() const
    {
        return m_textBeforeSelection.getFont();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TextBox::setTextSize(unsigned int size)
    {
        // Store the new text size
        m_textSize = size;

        // There is a minimum text size
        if (m_textSize < 8)
            m_textSize = 8;

        // Change the text size
        m_textBeforeSelection.setCharacterSize(m_textSize);
        m_textSelection1.setCharacterSize(m_textSize);
        m_textSelection2.setCharacterSize(m_textSize);
        m_textAfterSelection1.setCharacterSize(m_textSize);
        m_textAfterSelection2.setCharacterSize(m_textSize);

        // Calculate the height of one line
        if (getTextFont())
            m_lineHeight = m_textBeforeSelection.getFont()->getLineSpacing(m_textSize);

        // Don't continue when line height is 0
        if (m_lineHeight == 0)
            return;

        // There is also a minimum height
        if (m_size.y < m_lineHeight)
            m_size.y = m_lineHeight;

        // If there is a scrollbar then reinitialize it
        if (m_scroll != nullptr)
        {
            m_scroll->setLowValue(m_size.y);
            m_scroll->setSize(m_scroll->getSize().x, static_cast<float>(m_size.y));
        }

        // The size has changed, update the text
        m_selectionTextsNeedUpdate = true;
        updateDisplayedText();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int TextBox::getTextSize() const
    {
        return m_textSize;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TextBox::setMaximumCharacters(unsigned int maxChars)
    {
        // Set the new character limit ( 0 to disable the limit )
        m_maxChars = maxChars;

        // If there is a character limit then check if it is exeeded
        if ((m_maxChars > 0) && (m_text.getSize() > m_maxChars))
        {
            // Remove all the excess characters
            m_text.erase(m_maxChars, sf::String::InvalidPos);

            // Set the caret behind the last character
            setCaretPosition(m_text.getSize());
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int TextBox::getMaximumCharacters() const
    {
        return m_maxChars;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TextBox::setBorders(float leftBorder, float topBorder, float rightBorder, float bottomBorder)
    {
        m_borders.left   = leftBorder;
        m_borders.top    = topBorder;
        m_borders.right  = rightBorder;
        m_borders.bottom = bottomBorder;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TextBox::changeColors(const sf::Color& backgroundColor,
                               const sf::Color& color,
                               const sf::Color& selectedColor,
                               const sf::Color& selectedBgrColor,
                               const sf::Color& borderColor,
                               const sf::Color& caretColor)
    {
        m_textBeforeSelection.setColor(color);
        m_textSelection1.setColor(selectedColor);
        m_textSelection2.setColor(selectedColor);
        m_textAfterSelection1.setColor(color);
        m_textAfterSelection2.setColor(color);

        m_caretColor           = caretColor;
        m_backgroundColor      = backgroundColor;
        m_selectedTextBgrColor = selectedBgrColor;
        m_borderColor          = borderColor;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TextBox::setBackgroundColor(const sf::Color& backgroundColor)
    {
        m_backgroundColor = backgroundColor;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TextBox::setTextColor(const sf::Color& textColor)
    {
        m_textBeforeSelection.setColor(textColor);
        m_textAfterSelection1.setColor(textColor);
        m_textAfterSelection2.setColor(textColor);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TextBox::setSelectedTextColor(const sf::Color& selectedTextColor)
    {
        m_textSelection1.setColor(selectedTextColor);
        m_textSelection2.setColor(selectedTextColor);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TextBox::setSelectedTextBackgroundColor(const sf::Color& selectedTextBackgroundColor)
    {
        m_selectedTextBgrColor = selectedTextBackgroundColor;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TextBox::setBorderColor(const sf::Color& borderColor)
    {
        m_borderColor = borderColor;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TextBox::setCaretColor(const sf::Color& caretColor)
    {
        m_caretColor = caretColor;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const sf::Color& TextBox::getBackgroundColor() const
    {
        return m_backgroundColor;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const sf::Color& TextBox::getTextColor() const
    {
        return m_textBeforeSelection.getColor();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const sf::Color& TextBox::getSelectedTextColor() const
    {
        return m_textSelection1.getColor();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const sf::Color& TextBox::getSelectedTextBackgroundColor() const
    {
        return m_selectedTextBgrColor;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const sf::Color& TextBox::getBorderColor() const
    {
        return m_borderColor;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const sf::Color& TextBox::getCaretColor() const
    {
        return m_caretColor;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TextBox::setCaretPosition(unsigned int charactersBeforeCaret)
    {
        // The caret position has to stay inside the string
        if (charactersBeforeCaret > m_text.getSize())
            charactersBeforeCaret = m_text.getSize();

        // Don't continue when line height is 0
        if (m_lineHeight == 0)
            return;

        // Set the caret to the correct position
        m_selChars = 0;
        m_selStart = charactersBeforeCaret;
        m_selEnd = charactersBeforeCaret;

        // Change our three texts
        m_textBeforeSelection.setString(m_displayedText);
        m_textSelection1.setString("");
        m_textSelection2.setString("");
        m_textAfterSelection1.setString("");
        m_textAfterSelection2.setString("");

        // Update the text
        updateDisplayedText();

        // Check if there is a scrollbar
        if (m_scroll != nullptr)
        {
            unsigned int newlines = 0;
            unsigned int newlinesAdded = 0;
            unsigned int totalLines = 0;

            // Loop through all characters
            for (unsigned int i=0; i<m_selEnd; ++i)
            {
                // Make sure there is no newline in the text
                if (m_text[i] != '\n')
                {
                    // Check if there is a newline in the displayed text
                    if (m_displayedText[i + newlinesAdded] == '\n')
                    {
                        // A newline was added here
                        ++newlinesAdded;
                        ++totalLines;

                        if (i < m_selEnd)
                            ++newlines;
                    }
                }
                else // The text already contains a newline
                {
                    ++totalLines;

                    if (i < m_selEnd)
                        ++newlines;
                }
            }

            // Check if the caret is located above the view
            if ((newlines < m_topLine - 1) || ((newlines < m_topLine) && (m_scroll->getValue() % m_lineHeight > 0)))
            {
                m_scroll->setValue(newlines * m_lineHeight);
                updateDisplayedText();
            }

            // Check if the caret is below the view
            else if (newlines > m_topLine + m_visibleLines - 2)
            {
                m_scroll->setValue((newlines - m_visibleLines + 1) * m_lineHeight);
                updateDisplayedText();
            }
            else if ((newlines > m_topLine + m_visibleLines - 3) && (m_scroll->getValue() % m_lineHeight > 0))
            {
                m_scroll->setValue((newlines - m_visibleLines + 2) * m_lineHeight);
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
        if (m_scroll != nullptr)
            delete m_scroll;

        try
        {
            // load the scrollbar
            m_scroll = new Scrollbar();
            m_scroll->load(scrollbarConfigFileFilename);
        }
        catch (const Exception& e)
        {
            // The scrollbar couldn't be loaded so it must be deleted
            delete m_scroll;
            m_scroll = nullptr;

            return false;
        }

        // Initialize the scrollbar
        m_scroll->setVerticalScroll(true);
        m_scroll->setSize(m_scroll->getSize().x, static_cast<float>(m_size.y));
        m_scroll->setLowValue(m_size.y);
        m_scroll->setMaximum(m_lines * m_lineHeight);

        return true;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TextBox::removeScrollbar()
    {
        // Delete the scrollbar
        delete m_scroll;
        m_scroll = nullptr;

        m_topLine = 1;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TextBox::setCaretWidth(unsigned int width)
    {
        m_caretWidth = width;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int TextBox::getCaretWidth() const
    {
        return m_caretWidth;
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

        if (m_scroll != nullptr)
            m_scroll->setTransparency(m_opacity);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool TextBox::mouseOnWidget(float x, float y)
    {
        // Get the current position
        sf::Vector2f position = getPosition();

        // Pass the event to the scrollbar (if there is one)
        if (m_scroll != nullptr)
        {
            // Temporarily set the position of the scroll
            m_scroll->setPosition(position.x + m_size.x - m_scroll->getSize().x, position.y);

            // Pass the event
            m_scroll->mouseOnWidget(x, y);

            // Reset the position
            m_scroll->setPosition(0, 0);
        }

        // Check if the mouse is on top of the text box
        if (getTransform().transformRect(sf::FloatRect(0, 0, static_cast<float>(m_size.x), static_cast<float>(m_size.y))).contains(x, y))
            return true;
        else // The mouse is not on top of the text box
        {
            if (m_mouseHover)
                mouseLeftWidget();

            m_mouseHover = false;
            return false;
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TextBox::leftMousePressed(float x, float y)
    {
        // Set the mouse down flag
        m_mouseDown = true;

        // This will be true when the click didn't occur on the scrollbar
        bool clickedOnTextBox = true;

        // If there is a scrollbar then pass the event
        if (m_scroll != nullptr)
        {
            // Remember the old scrollbar value
            unsigned int oldValue = m_scroll->getValue();

            // Temporarily set the position of the scroll
            m_scroll->setPosition(getPosition().x + m_size.x - m_scroll->getSize().x, getPosition().y);

            // Pass the event
            if (m_scroll->mouseOnWidget(x, y))
            {
                m_scroll->leftMousePressed(x, y);
                clickedOnTextBox = false;
            }

            // Reset the position
            m_scroll->setPosition(0, 0);

            // If the value of the scrollbar has changed then update the text
            if (oldValue != m_scroll->getValue())
                updateDisplayedText();
        }

        // If the click occured on the text box
        if (clickedOnTextBox)
        {
            // Don't continue when line height is 0
            if (m_lineHeight == 0)
                return;

            unsigned int caretPosition = findCaretPosition(x - getPosition().x - 4, y - getPosition().y);

            // Check if this is a double click
            if ((m_possibleDoubleClick) && (m_selChars == 0) && (caretPosition == m_selEnd))
            {
                // The next click is going to be a normal one again
                m_possibleDoubleClick = false;

                // Select the whole text
                m_selStart = 0;
                m_selEnd = m_text.getSize();
                m_selChars = m_text.getSize();

                // Update the text
                m_selectionTextsNeedUpdate = true;
                updateDisplayedText();

                // Check if there is a scrollbar
                if (m_scroll != nullptr)
                {
                    unsigned int newlines = 0;
                    unsigned int newlinesAdded = 0;
                    unsigned int totalLines = 0;

                    // Loop through all characters
                    for (unsigned int i=0; i<m_selEnd; ++i)
                    {
                        // Make sure there is no newline in the text
                        if (m_text[i] != '\n')
                        {
                            // Check if there is a newline in the displayed text
                            if (m_displayedText[i + newlinesAdded] == '\n')
                            {
                                // A newline was added here
                                ++newlinesAdded;
                                ++totalLines;

                                if (i < m_selEnd)
                                    ++newlines;
                            }
                        }
                        else // The text already contains a newline
                        {
                            ++totalLines;

                            if (i < m_selEnd)
                                ++newlines;
                        }
                    }

                    // Check if the caret is located above the view
                    if ((newlines < m_topLine - 1) || ((newlines < m_topLine) && (m_scroll->getValue() % m_lineHeight > 0)))
                    {
                        m_scroll->setValue(newlines * m_lineHeight);
                        updateDisplayedText();
                    }

                    // Check if the caret is below the view
                    else if (newlines > m_topLine + m_visibleLines - 2)
                    {
                        m_scroll->setValue((newlines - m_visibleLines + 1) * m_lineHeight);
                        updateDisplayedText();
                    }
                    else if ((newlines > m_topLine + m_visibleLines - 3) && (m_scroll->getValue() % m_lineHeight > 0))
                    {
                        m_scroll->setValue((newlines - m_visibleLines + 2) * m_lineHeight);
                        updateDisplayedText();
                    }
                }
            }
            else // No double clicking
            {
                // Set the caret on the new position
                setCaretPosition(caretPosition);

                // If the next click comes soon enough then it will be a double click
                m_possibleDoubleClick = true;
            }

            // The caret should be visible
            m_caretVisible = true;
            m_animationTimeElapsed = sf::Time();
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TextBox::leftMouseReleased(float x, float y)
    {
        // If there is a scrollbar then pass it the event
        if (m_scroll != nullptr)
        {
            // Only pass the event when the scrollbar still thinks the mouse is down
            if (m_scroll->m_mouseDown == true)
            {
                // Don't continue when line height is 0
                if (m_lineHeight == 0)
                    return;

                // Remember the old scrollbar value
                unsigned int oldValue = m_scroll->getValue();

                // Temporarily set the position of the scroll
                m_scroll->setPosition(getPosition().x + m_size.x - m_scroll->getSize().x, getPosition().y);

                // Pass the event
                m_scroll->leftMouseReleased(x, y);

                // Reset the position
                m_scroll->setPosition(0, 0);

                // If the value of the scrollbar has changed then update the text
                if (oldValue != m_scroll->getValue())
                {
                    updateDisplayedText();

                    // Check if the scrollbar value was incremented (you have pressed on the down arrow)
                    if (m_scroll->getValue() == oldValue + 1)
                    {
                        // Decrement the value
                        m_scroll->setValue(m_scroll->getValue()-1);

                        // Scroll down with the whole item height instead of with a single pixel
                        m_scroll->setValue(m_scroll->getValue() + m_lineHeight - (m_scroll->getValue() % m_lineHeight));
                    }
                    else if (m_scroll->getValue() == oldValue - 1) // Check if the scrollbar value was decremented (you have pressed on the up arrow)
                    {
                        // increment the value
                        m_scroll->setValue(m_scroll->getValue()+1);

                        // Scroll up with the whole item height instead of with a single pixel
                        if (m_scroll->getValue() % m_lineHeight > 0)
                            m_scroll->setValue(m_scroll->getValue() - (m_scroll->getValue() % m_lineHeight));
                        else
                            m_scroll->setValue(m_scroll->getValue() - m_lineHeight);
                    }
                }
            }
        }

        m_mouseDown = false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TextBox::mouseMoved(float x, float y)
    {
        if (m_mouseHover == false)
            mouseEnteredWidget();

        // Set the mouse move flag
        m_mouseHover = true;

        // The mouse has moved so a double click is no longer possible
        m_possibleDoubleClick = false;

        // If there is a scrollbar then pass the event
        if (m_scroll != nullptr)
        {
            // Temporarily set the position of the scroll
            m_scroll->setPosition(getPosition().x + m_size.x - m_scroll->getSize().x, getPosition().y);

            // Check if you are dragging the thumb of the scrollbar
            if (m_scroll->m_mouseDown)
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

                // If the mouse is down then you are selecting text
                if (m_mouseDown)
                    selectText(x, y);
            }

            // Reset the position
            m_scroll->setPosition(0, 0);
        }
        else // There is no scrollbar
        {
            // If the mouse is down then you are selecting text
            if (m_mouseDown)
                selectText(x, y);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TextBox::mouseNotOnWidget()
    {
        if (m_mouseHover)
            mouseLeftWidget();

        m_mouseHover = false;

        if (m_scroll != nullptr)
            m_scroll->m_mouseHover = false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TextBox::mouseNoLongerDown()
    {
        m_mouseDown = false;

        if (m_scroll != nullptr)
            m_scroll->m_mouseDown = false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TextBox::keyPressed(const sf::Event::KeyEvent& event)
    {
        // Check if one of the correct keys was pressed
        if (event.code == sf::Keyboard::Left)
        {
            // Check if we have selected some text
            if (m_selChars > 0)
            {
                // We will not move the caret, but just undo the selection
                if (m_selStart < m_selEnd)
                    setCaretPosition(m_selStart);
                else
                    setCaretPosition(m_selEnd);
            }
            else // When we didn't select any text
            {
                // You don't have to do anything when the caret is at the beginning of the text
                if (m_selEnd > 0)
                {
                    // Move the caret to the left
                    setCaretPosition(m_selEnd - 1);
                }
            }

            // Our caret has moved, it should be visible
            m_caretVisible = true;
            m_animationTimeElapsed = sf::Time();
        }
        else if (event.code == sf::Keyboard::Right)
        {
            // Check if we have selected some text
            if (m_selChars > 0)
            {
                // We will not move the caret, but just undo the selection
                if (m_selStart < m_selEnd)
                    setCaretPosition(m_selEnd);
                else
                    setCaretPosition(m_selStart);
            }
            else // When we didn't select any text
            {
                // You don't have to do anything when the caret is at the end of the text
                if (m_selEnd < m_text.getSize())
                {
                    // Move the caret to the right
                    setCaretPosition(m_selEnd + 1);
                }
            }

            // Our caret has moved, it should be visible
            m_caretVisible = true;
            m_animationTimeElapsed = sf::Time();
        }
        else if (event.code == sf::Keyboard::Up)
        {
            sf::Text tempText(m_textBeforeSelection);
            tempText.setString(m_displayedText);

            bool newlineAdded = false;
            unsigned int newlinesAdded = 0;

            unsigned int character;
            unsigned int newTopPosition = 0;
            sf::Vector2u newPosition(0, 0);

            int distanceX;
            unsigned int previousdistanceX = m_size.x;

            // Loop through all characters
            for (unsigned int i = 0; i < m_selEnd; ++i)
            {
                // Make sure there is no newline in the text
                if (m_text[i] != '\n')
                {
                    // Check if there is a newline in the displayed text
                    if (m_displayedText[i + newlinesAdded] == '\n')
                    {
                        // A newline was added here
                        ++newlinesAdded;
                        newlineAdded = true;
                    }
                }
                else // The newline occurs in the text itself
                    newlineAdded = false;
            }

            // Get the position of the character behind the caret
            sf::Vector2u originalPosition = sf::Vector2u(tempText.findCharacterPos(m_selEnd + newlinesAdded));

            // Try to find the line above the caret
            for (character = m_selEnd; character > 0; --character)
            {
                // Get the top position of the character before it
                newTopPosition = static_cast<unsigned int>(tempText.findCharacterPos(character + newlinesAdded - 1).y);

                // Check if the the character is on the line above the original one
                if (newTopPosition < originalPosition.y)
                    break;
            }

            // Our caret has moved, it should be visible
            m_caretVisible = true;
            m_animationTimeElapsed = sf::Time();

            // Don't do anything when the caret is on the first line
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
                        setCaretPosition(character + newlineAdded);
                        return;
                    }

                    // Calculate the distance to the original position
                    distanceX = newPosition.x - originalPosition.x;

                    // Check if the distance is going away again
                    if (static_cast<unsigned int>(abs(distanceX)) > previousdistanceX)
                    {
                        // We have found the character that we were looking for
                        setCaretPosition(character + newlineAdded);
                        return;
                    }

                    // Remember the previous distance
                    previousdistanceX = abs(distanceX);
                }

                // Check if the caret should be behind the first character
                if (originalPosition.x > previousdistanceX)
                {
                    // We have found the character that we were looking for
                    setCaretPosition(character + newlineAdded);
                    return;
                }

                // If you pass here then the caret should be at the beginning of the text
                setCaretPosition(0);
            }
        }
        else if (event.code == sf::Keyboard::Down)
        {
            sf::Text tempText(m_textBeforeSelection);
            tempText.setString(m_displayedText);

            bool newlineAdded = false;
            unsigned int newlinesAdded = 0;

            unsigned int character;
            unsigned int newTopPosition = 0;
            sf::Vector2u newPosition(0, 0);

            int distanceX;
            int previousdistanceX = m_size.x;

            // Loop through all characters
            for (unsigned int i = 0; i < m_text.getSize(); ++i)
            {
                // Make sure there is no newline in the text
                if (m_text[i] != '\n')
                {
                    // Check if there is a newline in the displayed text
                    if (m_displayedText[i + newlinesAdded] == '\n')
                    {
                        // A newline was added here
                        ++newlinesAdded;
                        newlineAdded = true;

                        // Stop when the newline behind the caret was found
                        if (i > m_selEnd)
                            break;
                    }
                }
                else // The newline occurs in the text itself
                {
                    newlineAdded = false;

                    // Stop when the newline behind the caret was found
                    if (i > m_selEnd)
                        break;
                }
            }

            // Get the position of the character behind the caret
            sf::Vector2u originalPosition = sf::Vector2u(tempText.findCharacterPos(m_selEnd + newlinesAdded - newlineAdded));

            // Try to find the line below the caret
            for (character = m_selEnd; character < m_text.getSize(); ++character)
            {
                // Get the top position of the character after it
                newTopPosition = static_cast<unsigned int>(tempText.findCharacterPos(character + newlinesAdded - newlineAdded + 1).y);

                // Check if the the character is on the line below the original one
                if (newTopPosition > originalPosition.y)
                    break;
            }

            // Our caret has moved, it should be visible
            m_caretVisible = true;
            m_animationTimeElapsed = sf::Time();

            // Don't do anything when the caret is on the last line
            if (character < m_text.getSize())
            {
                // Try to find the closest character
                for ( ; character < m_text.getSize() + 1; ++character)
                {
                    // Get the position of the character after it
                    newPosition = sf::Vector2u(tempText.findCharacterPos(character + newlinesAdded - newlineAdded + 1));

                    // The character must remain on the same line
                    if (newPosition.y > newTopPosition)
                    {
                        // We have found the character that we were looking for
                        setCaretPosition(character - newlineAdded);
                        return;
                    }

                    // Calculate the distance to the original position
                    distanceX = newPosition.x - originalPosition.x;

                    // Check if the distance is going away again
                    if (abs(distanceX) > previousdistanceX)
                    {
                        // We have found the character that we were looking for
                        setCaretPosition(character - newlineAdded);
                        return;
                    }

                    // Remember the previous distance
                    previousdistanceX = abs(distanceX);
                }

                // If you pass here then the caret should be at the end of the text
                setCaretPosition(m_text.getSize());
            }
        }
        else if (event.code == sf::Keyboard::Home)
        {
            // Set the caret to the beginning of the text
            setCaretPosition(0);

            // Our caret has moved, it should be visible
            m_caretVisible = true;
            m_animationTimeElapsed = sf::Time();
        }
        else if (event.code == sf::Keyboard::End)
        {
            // Set the caret behind the text
            setCaretPosition(m_text.getSize());

            // Our caret has moved, it should be visible
            m_caretVisible = true;
            m_animationTimeElapsed = sf::Time();
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
            if (m_selChars == 0)
            {
                // We can't delete any characters when you are at the beginning of the string
                if (m_selEnd == 0)
                    return;

                // Erase the character
                m_text.erase(m_selEnd-1, 1);

                // Set the caret back on the correct position
                setCaretPosition(m_selEnd - 1);

                if (m_scroll != nullptr)
                {
                    // Check if the scrollbar is behind the text
                    if (m_scroll->getValue() > m_scroll->getMaximum() - m_scroll->getLowValue())
                    {
                        // Adjust the value of the scrollbar
                        m_scroll->setValue(m_scroll->getValue());

                        // The text has to be updated again
                        m_selectionTextsNeedUpdate = true;
                        updateDisplayedText();
                    }
                }
            }
            else // When you did select some characters then delete them
                deleteSelectedCharacters();

            // The caret should be visible again
            m_caretVisible = true;
            m_animationTimeElapsed = sf::Time();

            // Add the callback (if the user requested it)
            if (m_callbackFunctions[TextChanged].empty() == false)
            {
                m_callback.trigger = TextChanged;
                m_callback.text    = m_text;
                addCallback();
            }
        }
        else if (event.code == sf::Keyboard::Delete)
        {
            if (m_readOnly)
                return;

            // Make sure that no text is selected
            if (m_selChars == 0)
            {
                // When the caret is at the end of the line then you can't delete anything
                if (m_selEnd == m_text.getSize())
                    return;

                // Erase the character
                m_text.erase(m_selEnd, 1);

                // Set the caret back on the correct position
                setCaretPosition(m_selEnd);

                if (m_scroll != nullptr)
                {
                    // Check if there is a risk that the scrollbar is going to be behind the text
                    if ((m_scroll->getValue() == m_scroll->getMaximum() - m_scroll->getLowValue()) || (m_scroll->getValue() > m_scroll->getMaximum() - m_scroll->getLowValue() - m_lineHeight))
                    {
                        // Reset the value of the scroll. If it is too high then it will be automatically be adjusted.
                        m_scroll->setValue(m_scroll->getValue());

                        // The text has to be updated again
                        m_selectionTextsNeedUpdate = true;
                        updateDisplayedText();
                    }
                }
            }
            else // You did select some characters, so remove them
                deleteSelectedCharacters();

            // The caret should be visible again
            m_caretVisible = true;
            m_animationTimeElapsed = sf::Time();

            // Add the callback (if the user requested it)
            if (m_callbackFunctions[TextChanged].empty() == false)
            {
                m_callback.trigger = TextChanged;
                m_callback.text    = m_text;
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
                    TGUI_Clipboard.set(m_textSelection1.getString() + m_textSelection2.getString());
                }
                else if (event.code == sf::Keyboard::V)
                {
                    if (m_readOnly)
                        return;

                    auto clipboardContents = TGUI_Clipboard.get();

                    // Only continue pasting if you actually have to do something
                    if ((m_selChars > 0) || (clipboardContents.getSize() > 0))
                    {
                        deleteSelectedCharacters();

                        unsigned int oldCaretPos = m_selEnd;

                        if (m_text.getSize() > m_selEnd)
                            setText(m_text.toWideString().substr(0, m_selEnd) + TGUI_Clipboard.get() + m_text.toWideString().substr(m_selEnd, m_text.getSize() - m_selEnd));
                        else
                            setText(m_text + clipboardContents);

                        setCaretPosition(oldCaretPos + clipboardContents.getSize());

                        // Add the callback (if the user requested it)
                        if (m_callbackFunctions[TextChanged].empty() == false)
                        {
                            m_callback.trigger = TextChanged;
                            m_callback.text    = m_text;
                            addCallback();
                        }
                    }
                }
                else if (event.code == sf::Keyboard::X)
                {
                    TGUI_Clipboard.set(m_textSelection1.getString() + m_textSelection2.getString());

                    if (m_readOnly)
                        return;

                    deleteSelectedCharacters();
                }
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TextBox::textEntered(sf::Uint32 key)
    {
        if (m_readOnly)
            return;

        // If there were selected characters then delete them first
        deleteSelectedCharacters();

        // Make sure we don't exceed our maximum characters limit
        if ((m_maxChars > 0) && (m_text.getSize() + 1 > m_maxChars))
                return;

        // If there is a limit in the amount of lines then make a simulation
        if (m_scroll == nullptr)
        {
            // Don't continue when line height is 0
            if (m_lineHeight == 0)
                return;

            float maxLineWidth = TGUI_MAXIMUM(m_size.x - 4.0f, 0);

            // Make some preparations
            sf::Text tempText(m_textBeforeSelection);
            unsigned int beginChar = 0;
            unsigned int newlinesAdded = 0;

            sf::String text = m_text;
            text.insert(m_selEnd, key);

            sf::String displayedText = text;
            unsigned int lines = 1;

            // Loop through every character
            for (unsigned i=1; i < text.getSize() + 1; ++i)
            {
                // Make sure the character is not a newline
                if (text[i-1] != '\n')
                {
                    // Add the next character to the text widget
                    tempText.setString(text.toWideString().substr(beginChar, i - beginChar));

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
                if (lines > m_size.y / m_lineHeight)
                {
                    // The character can't be added
                    return;
                }
            }
        }

        // Insert our character
        m_text.insert(m_selEnd, key);

        // Move our caret forward
        setCaretPosition(m_selEnd + 1);

        // The caret should be visible again
        m_caretVisible = true;
        m_animationTimeElapsed = sf::Time();

        // Add the callback (if the user requested it)
        if (m_callbackFunctions[TextChanged].empty() == false)
        {
            m_callback.trigger = TextChanged;
            m_callback.text    = m_text;
            addCallback();
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TextBox::mouseWheelMoved(int delta, int, int)
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
                    m_scroll->setValue(m_scroll->getValue() + (static_cast<unsigned int>(-delta) * (m_lineHeight / 2)));
                }
                else // You are scrolling up
                {
                    unsigned int change = static_cast<unsigned int>(delta) * (m_lineHeight / 2);

                    // Scroll up
                    if (change < m_scroll->getValue())
                        m_scroll->setValue(m_scroll->getValue() - change);
                    else
                        m_scroll->setValue(0);
                }
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TextBox::widgetUnfocused()
    {
        // If there is a selection then undo it now
        if (m_selChars)
            setCaretPosition(m_selEnd);

        Widget::widgetUnfocused();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TextBox::setProperty(std::string property, const std::string& value)
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
            setTextSize(tgui::stoi(value));
        }
        else if (property == "maximumcharacters")
        {
            setMaximumCharacters(tgui::stoi(value));
        }
        else if (property == "borders")
        {
            Borders borders;
            if (extractBorders(value, borders))
                setBorders(borders.left, borders.top, borders.right, borders.bottom);
            else
                throw Exception("Failed to parse 'Borders' property.");
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
        else if (property == "caretcolor")
        {
            setCaretColor(extractColor(value));
        }
        else if (property == "caretwidth")
        {
            setCaretWidth(tgui::stoi(value));
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
            Widget::setProperty(property, value);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TextBox::getProperty(std::string property, std::string& value) const
    {
        property = toLower(property);

        if (property == "configfile")
            value = getLoadedConfigFile();
        else if (property == "text")
            encodeString(getText(), value);
        else if (property == "textsize")
            value = tgui::to_string(getTextSize());
        else if (property == "maximumcharacters")
            value = tgui::to_string(getMaximumCharacters());
        else if (property == "borders")
            value = "(" + tgui::to_string(getBorders().left) + "," + tgui::to_string(getBorders().top) + "," + tgui::to_string(getBorders().right) + "," + tgui::to_string(getBorders().bottom) + ")";
        else if (property == "backgroundcolor")
            value = "(" + tgui::to_string(int(getBackgroundColor().r)) + "," + tgui::to_string(int(getBackgroundColor().g)) + "," + tgui::to_string(int(getBackgroundColor().b)) + "," + tgui::to_string(int(getBackgroundColor().a)) + ")";
        else if (property == "textcolor")
            value = "(" + tgui::to_string(int(getTextColor().r)) + "," + tgui::to_string(int(getTextColor().g)) + "," + tgui::to_string(int(getTextColor().b)) + "," + tgui::to_string(int(getTextColor().a)) + ")";
        else if (property == "selectedtextcolor")
            value = "(" + tgui::to_string(int(getSelectedTextColor().r)) + "," + tgui::to_string(int(getSelectedTextColor().g))
                    + "," + tgui::to_string(int(getSelectedTextColor().b)) + "," + tgui::to_string(int(getSelectedTextColor().a)) + ")";
        else if (property == "selectedtextbackgroundcolor")
            value = "(" + tgui::to_string(int(getSelectedTextBackgroundColor().r)) + "," + tgui::to_string(int(getSelectedTextBackgroundColor().g))
                    + "," + tgui::to_string(int(getSelectedTextBackgroundColor().b)) + "," + tgui::to_string(int(getSelectedTextBackgroundColor().a)) + ")";
        else if (property == "bordercolor")
            value = "(" + tgui::to_string(int(getBorderColor().r)) + "," + tgui::to_string(int(getBorderColor().g)) + "," + tgui::to_string(int(getBorderColor().b)) + "," + tgui::to_string(int(getBorderColor().a)) + ")";
        else if (property == "caretcolor")
            value = "(" + tgui::to_string(int(getCaretColor().r)) + "," + tgui::to_string(int(getCaretColor().g)) + "," + tgui::to_string(int(getCaretColor().b)) + "," + tgui::to_string(int(getCaretColor().a)) + ")";
        else if (property == "caretwidth")
            value = tgui::to_string(getCaretWidth());
        else if (property == "callback")
        {
            std::string tempValue;
            Widget::getProperty(property, tempValue);

            std::vector<sf::String> callbacks;

            if ((m_callbackFunctions.find(TextChanged) != m_callbackFunctions.end()) && (m_callbackFunctions.at(TextChanged).size() == 1) && (m_callbackFunctions.at(TextChanged).front() == nullptr))
                callbacks.push_back("TextChanged");

            encodeList(callbacks, value);

            if (value.empty())
                value = tempValue;
            else if (!tempValue.empty())
                value += "," + tempValue;
        }
        else // The property didn't match
            Widget::getProperty(property, value);
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
        list.push_back(std::pair<std::string, std::string>("CaretColor", "color"));
        list.push_back(std::pair<std::string, std::string>("CaretWidth", "uint"));
        return list;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int TextBox::findCaretPosition(float posX, float posY)
    {
        // This code will crash when the text box is empty. We need to avoid this.
        if (m_text.isEmpty())
            return 0;

        // Don't continue when line height is 0
        if (m_lineHeight == 0)
            return 0;

        // Find out on which line you clicked
        unsigned int line;

        // Check if there is a scrollbar
        if (m_scroll == nullptr)
        {
            // If the position is negative then set the caret before the first character
            if (posY < 0)
                return 0;
            else
                line = static_cast<unsigned int>(posY / m_lineHeight + 1);
        }
        else // There is no scrollbar
        {
            // If the position is negative then set the caret before the first character
            if (posY + m_scroll->getValue() < 0)
                return 0;
            else
                line = static_cast<unsigned int>((posY + m_scroll->getValue()) / m_lineHeight + 1);
        }

        // Create a temporary text widget that contains the full text
        sf::Text fullText(m_textBeforeSelection);
        fullText.setString(m_displayedText);

        // Check if you clicked behind all characters
        if ((line > m_lines) || ((line == m_lines) && (posX > fullText.findCharacterPos(m_displayedText.getSize()).x)))
        {
            // The caret should be behind the last character
            return m_text.getSize();
        }
        else // You clicked inside the text
        {
            // Prepare to skip part of the text
            sf::String tempString = m_displayedText;
            std::size_t newlinePos = 0;

            // Only remove lines when there are lines to remove
            if (line > 1)
            {
                unsigned int i=1;

                // If the first character is a newline then remove it if needed
                if (m_text[0] == '\n')
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
            sf::Text tempText(m_textBeforeSelection);

            // We are going to calculate the number of newlines we have added
            unsigned int newlinesAdded = 0;
            unsigned int totalNewlinesAdded = 0;
            unsigned int beginChar = 0;
            sf::String   tempString2 = m_text;

            // Calculate the maximum line width
            float maxLineWidth = m_size.x - 4.0f;
            if (m_scroll != nullptr)
                maxLineWidth -= m_scroll->getSize().x;

            // If the width is negative then the text box is too small to be displayed
            if (maxLineWidth < 0)
                maxLineWidth = 0;

            // Loop through every character
            for (unsigned i = 1; (i < m_text.getSize() + 1) && (totalNewlinesAdded != line - 1); ++i)
            {
                // Make sure the character is not a newline
                if (m_text[i-1] != '\n')
                {
                    // Add the next character to the text widget
                    tempText.setString(m_text.toWideString().substr(beginChar, i - beginChar));

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

            // If the line contains nothing but a newline character then put the caret on that line
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

        // Something went wrong, don't move the caret position
        return m_selEnd;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TextBox::selectText(float posX, float posY)
    {
        // Don't continue when line height is 0
        if (m_lineHeight == 0)
            return;

        // Find out where the caret should be
        m_selEnd = findCaretPosition(posX - getPosition().x - 4, posY - getPosition().y);

        // Calculate how many character are being selected
        if (m_selEnd < m_selStart)
            m_selChars = m_selStart - m_selEnd;
        else
            m_selChars = m_selEnd - m_selStart;

        // Update the text
        m_selectionTextsNeedUpdate = true;
        updateDisplayedText();

        // Check if there is a scrollbar
        if (m_scroll != nullptr)
        {
            unsigned int newlines = 0;
            unsigned int newlinesAdded = 0;
            unsigned int totalLines = 0;

            // Loop through all characters
            for (unsigned int i=0; i<m_selEnd; ++i)
            {
                // Make sure there is no newline in the text
                if (m_text[i] != '\n')
                {
                    // Check if there is a newline in the displayed text
                    if (m_displayedText[i + newlinesAdded] == '\n')
                    {
                        // A newline was added here
                        ++newlinesAdded;
                        ++totalLines;

                        if (i < m_selEnd)
                            ++newlines;
                    }
                }
                else // The text already contains a newline
                {
                    ++totalLines;

                    if (i < m_selEnd)
                        ++newlines;
                }
            }

            // Check if the caret is located above the view
            if ((newlines < m_topLine - 1) || ((newlines < m_topLine) && (m_scroll->getValue() % m_lineHeight > 0)))
            {
                m_scroll->setValue(newlines * m_lineHeight);
                updateDisplayedText();
            }

            // Check if the caret is below the view
            else if (newlines > m_topLine + m_visibleLines - 2)
            {
                m_scroll->setValue((newlines - m_visibleLines + 1) * m_lineHeight);
                updateDisplayedText();
            }
            else if ((newlines > m_topLine + m_visibleLines - 3) && (m_scroll->getValue() % m_lineHeight > 0))
            {
                m_scroll->setValue((newlines - m_visibleLines + 2) * m_lineHeight);
                updateDisplayedText();
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TextBox::deleteSelectedCharacters()
    {
        // Nothing to delete when no text was selected
        if (m_selChars == 0)
            return;

        // Erase the characters
        m_text.erase(TGUI_MINIMUM(m_selStart, m_selEnd), m_selChars);

        // Set the caret back on the correct position
        setCaretPosition(TGUI_MINIMUM(m_selStart, m_selEnd));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TextBox::updateDisplayedText()
    {
        // Don't continue when line height is 0
        if (m_lineHeight == 0)
            return;

        float maxLineWidth = m_size.x - 4.0f;
        if (m_scroll != nullptr)
            maxLineWidth -= m_scroll->getSize().x;

        // If the width is negative then the text box is too small to be displayed
        if (maxLineWidth < 0)
            maxLineWidth = 0;

        // Make some preparations
        sf::Text tempText(m_textBeforeSelection);
        unsigned int beginChar = 0;
        unsigned int newlinesAdded = 0;
        unsigned int newlinesAddedBeforeSelection = 0;

        m_displayedText = m_text;
        m_lines = 1;

        // Loop through every character
        for (unsigned i=1; i < m_text.getSize() + 1; ++i)
        {
            // Make sure the character is not a newline
            if (m_text[i-1] != '\n')
            {
                // Add the next character to the text widget
                tempText.setString(m_text.toWideString().substr(beginChar, i - beginChar));

                // Check if the string still fits on the line
                if (tempText.findCharacterPos(i).x > maxLineWidth)
                {
                    // Insert the newline character
                    m_displayedText.insert(i + newlinesAdded - 1, '\n');

                    // Prepare to find the next line end
                    beginChar = i - 1;
                    ++newlinesAdded;
                    ++m_lines;
                }
            }
            else // The character was a newline
            {
                beginChar = i;
                ++m_lines;
            }

            // Find the position of the caret
            if (m_selEnd == i - 1)
                newlinesAddedBeforeSelection = newlinesAdded;

            // Check if there is a limit in the amount of lines
            if (m_scroll == nullptr)
            {
                // Check if you passed this limit
                if (m_lines > m_size.y / m_lineHeight)
                {
                    // Remove all exceeding lines
                    m_displayedText.erase(i + newlinesAdded - 1, sf::String::InvalidPos);
                    m_text.erase(i-1, sf::String::InvalidPos);

                    // We counted one line too much
                    --m_lines;

                    break;
                }
            }
        }

        // Find the position of the caret
        if (m_selEnd == m_text.getSize())
            newlinesAddedBeforeSelection = newlinesAdded;

        // Check if there is a scrollbar
        if (m_scroll != nullptr)
        {
            // Tell the scrollbar how many pixels the text contains
            m_scroll->setMaximum(m_lines * m_lineHeight);

            // Calculate the top line
            m_topLine = m_scroll->getValue() / m_lineHeight + 1;

            // Calculate the number of visible lines
            if ((m_scroll->getValue() % m_lineHeight) == 0)
                m_visibleLines = TGUI_MINIMUM(m_size.y / m_lineHeight, m_lines);
            else
                m_visibleLines = TGUI_MINIMUM((m_size.y / m_lineHeight) + 1, m_lines);
        }
        else // There is no scrollbar
        {
            // You are always at the top line
            m_topLine = 1;

            // Calculate the number of visible lines
            m_visibleLines = TGUI_MINIMUM(m_size.y / m_lineHeight, m_lines);
        }

        // Fill the temporary text widget with the whole text
        tempText.setString(m_displayedText);

        // Set the position of the caret
        m_caretPosition = sf::Vector2u(tempText.findCharacterPos(m_selEnd + newlinesAddedBeforeSelection));

        // Only do the check when the caret is not standing at the first character
        if ((m_selEnd > 0) && (m_selEnd + newlinesAddedBeforeSelection > 0))
        {
            // If you are at the end of the line then also set the caret there, instead of at the beginning of the next line
            if ((m_text[m_selEnd - 1] != '\n') && (m_displayedText[m_selEnd + newlinesAddedBeforeSelection - 1] == '\n'))
                m_caretPosition = sf::Vector2u(tempText.findCharacterPos(m_selEnd + newlinesAddedBeforeSelection - 1));
        }

        // Check if the text has to be redivided in five pieces
        if (m_selectionTextsNeedUpdate)
            updateSelectionTexts(maxLineWidth);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TextBox::updateSelectionTexts(float maxLineWidth)
    {
        // If there is no selection then just put the whole text in m_textBeforeSelection
        if (m_selChars == 0)
        {
            m_textBeforeSelection.setString(m_displayedText);
            m_textSelection1.setString("");
            m_textSelection2.setString("");
            m_textAfterSelection1.setString("");
            m_textAfterSelection2.setString("");

            // Clear the list of selection rectangle sizes
            m_multilineSelectionRectWidth.clear();
        }
        else // Some text is selected
        {
            // Make some preparations
            unsigned i;
            unsigned int selectionStart = TGUI_MINIMUM(m_selEnd, m_selStart);
            unsigned int selectionEnd = TGUI_MAXIMUM(m_selEnd, m_selStart);

            sf::Text tempText(m_textBeforeSelection);
            unsigned int beginChar = 0;
            unsigned int newlinesAddedBeforeSelection = 0;
            unsigned int newlinesAddedInsideSelection = 0;
            unsigned int lastNewlineBeforeSelection = 0;
            bool newlineFoundInsideSelection = false;

            // Clear the list of selection rectangle sizes
            m_multilineSelectionRectWidth.clear();

            // Loop through every character before the selection
            for (i=0; i < selectionStart; ++i)
            {
                // Make sure the character is not a newline
                if (m_text[i] != '\n')
                {
                    // Add the next character to the text widget
                    tempText.setString(m_text.toWideString().substr(beginChar, i - beginChar + 1));

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
                if (m_text[i] != '\n')
                {
                    // Add the next character to the text widget
                    tempText.setString(m_text.toWideString().substr(beginChar, i - beginChar + 1));

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
            if (m_selEnd < m_selStart)
            {
                selectionStart = m_selEnd + newlinesAddedBeforeSelection;
                selectionEnd = m_selStart + newlinesAddedBeforeSelection + newlinesAddedInsideSelection;
            }
            else
            {
                selectionStart = m_selStart + newlinesAddedBeforeSelection;
                selectionEnd = m_selEnd + newlinesAddedBeforeSelection + newlinesAddedInsideSelection;
            }

            // Create another temprary text widget
            sf::Text tempText2(m_textBeforeSelection);
            tempText2.setString(m_displayedText);

            // Loop through the selected characters again
            for (i=selectionStart; i<selectionEnd; ++i)
            {
                if (m_displayedText[i] == '\n')
                {
                    // Check if this is not the first newline
                    if (newlineFoundInsideSelection == true)
                    {
                        // Add a new rectangle to the selection
                        if (tempText2.findCharacterPos(i).x > 0)
                            m_multilineSelectionRectWidth.push_back(tempText2.findCharacterPos(i).x);
                        else
                            m_multilineSelectionRectWidth.push_back(2);
                    }
                    else // This is the first newline, skip it
                        newlineFoundInsideSelection = true;
                }
            }

            // Add the last selection rectangle
            m_multilineSelectionRectWidth.push_back(tempText2.findCharacterPos(i).x);

            // Set the text before selection
            m_textBeforeSelection.setString(m_displayedText.toWideString().substr(0, selectionStart));

            // Set the text that is selected. If it consists of multiple lines then it will be changed below.
            m_textSelection1.setString(m_displayedText.toWideString().substr(selectionStart, m_selChars));
            m_textSelection2.setString("");

            // Loop through every character inside the selection
            for (i=selectionStart; i < selectionEnd; ++i)
            {
                // Check if the character is a newline
                if (m_displayedText[i] == '\n')
                {
                    // Set the text that is selected
                    m_textSelection1.setString(m_displayedText.toWideString().substr(selectionStart, i - selectionStart));
                    m_textSelection2.setString(m_displayedText.toWideString().substr(i + 1, m_selChars + newlinesAddedInsideSelection + selectionStart - i - 1));
                    break;
                }
            }

            // Set the text after the selection. If it consists of multiple lines then it will be changed below.
            m_textAfterSelection1.setString(m_displayedText.toWideString().substr(selectionEnd, m_displayedText.getSize() - selectionEnd));
            m_textAfterSelection2.setString("");

            // Loop through every character after the selection
            for (i=selectionEnd; i < m_displayedText.getSize(); ++i)
            {
                // Check if the character is a newline
                if (m_displayedText[i] == '\n')
                {
                    // Set the text that is selected
                    m_textAfterSelection1.setString(m_displayedText.toWideString().substr(selectionEnd, i - selectionEnd));
                    m_textAfterSelection2.setString(m_displayedText.toWideString().substr(i + 1, m_displayedText.getSize() - i - 1));
                    break;
                }
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TextBox::initialize(Container *const parent)
    {
        Widget::initialize(parent);

        if (!getTextFont() && m_parent->getGlobalFont())
            setTextFont(*m_parent->getGlobalFont());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TextBox::update()
    {
        // Only show/hide the caret every half second
        if (m_animationTimeElapsed < sf::milliseconds(500))
            return;

        // Reset the elapsed time
        m_animationTimeElapsed = sf::Time();

        // Only update when the editbox is visible
        if (m_visible == false)
            return;

        // Switch the value of the visible flag
        m_caretVisible = !m_caretVisible;

        // Too slow for double clicking
        m_possibleDoubleClick = false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TextBox::draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        const sf::View& view = target.getView();

        // Calculate the scale factor of the view
        float scaleViewX = target.getSize().x / view.getSize().x;
        float scaleViewY = target.getSize().y / view.getSize().y;

        // Get the global position
        sf::Vector2f topLeftPosition = states.transform.transformPoint(((getPosition().x - view.getCenter().x + (view.getSize().x / 2.f)) * view.getViewport().width) + (view.getSize().x * view.getViewport().left),
                                                                       ((getPosition().y - view.getCenter().y + (view.getSize().y / 2.f)) * view.getViewport().height) + (view.getSize().y * view.getViewport().top));
        sf::Vector2f bottomRightPosition = states.transform.transformPoint((getPosition().x + m_size.x - view.getCenter().x + (view.getSize().x / 2.f)) * view.getViewport().width + (view.getSize().x * view.getViewport().left),
                                                                           (getPosition().y + m_size.y - view.getCenter().y + (view.getSize().y / 2.f)) * view.getViewport().height + (view.getSize().y * view.getViewport().top));

        // Adjust the transformation
        states.transform *= getTransform();

        // Store the current transform
        sf::Transform origTransform = states.transform;

        // Draw the borders
        {
            // Draw left border
            sf::RectangleShape border(sf::Vector2f(m_borders.left, static_cast<float>(m_size.y + m_borders.top)));
            border.setPosition(-m_borders.left, -m_borders.top);
            border.setFillColor(m_borderColor);
            target.draw(border, states);

            // Draw top border
            border.setSize(sf::Vector2f(static_cast<float>(m_size.x + m_borders.right), m_borders.top));
            border.setPosition(0, -m_borders.top);
            target.draw(border, states);

            // Draw right border
            border.setSize(sf::Vector2f(m_borders.right, static_cast<float>(m_size.y + m_borders.bottom)));
            border.setPosition(static_cast<float>(m_size.x), 0);
            target.draw(border, states);

            // Draw bottom border
            border.setSize(sf::Vector2f(static_cast<float>(m_size.x + m_borders.left), m_borders.bottom));
            border.setPosition(-m_borders.left, static_cast<float>(m_size.y));
            target.draw(border, states);
        }

        // Draw the background
        sf::RectangleShape front(sf::Vector2f(static_cast<float>(m_size.x), static_cast<float>(m_size.y)));
        front.setFillColor(m_backgroundColor);
        target.draw(front, states);

        // Set the text on the correct position
        if (m_scroll != nullptr)
            states.transform.translate(2, -static_cast<float>(m_scroll->getValue()));
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

        sf::Text tempText(m_textBeforeSelection);
        tempText.setString("kg");
        sf::Vector2f textShift(tempText.getLocalBounds().left, tempText.getLocalBounds().top);

        // Draw the text
        states.transform.translate(-textShift.x, -textShift.y);
        target.draw(m_textBeforeSelection, states);
        states.transform.translate(0, textShift.y);

        // Check if there is a selection
        if (m_selChars > 0)
        {
            // Store the lenghts of the texts
            unsigned int textBeforeSelectionLength = m_textBeforeSelection.getString().getSize() + 1;
            unsigned int textSelection1Length = m_textSelection1.getString().getSize() + 1;
            unsigned int textSelection2Length = m_textSelection2.getString().getSize() + 1;

            // Set the text on the correct position
            states.transform.translate(m_textBeforeSelection.findCharacterPos(textBeforeSelectionLength).x, m_textBeforeSelection.findCharacterPos(textBeforeSelectionLength).y);

            // Watch out for kerning
            if (textBeforeSelectionLength > 1)
                states.transform.translate(static_cast<float>(m_textBeforeSelection.getFont()->getKerning(m_displayedText[textBeforeSelectionLength-2], m_displayedText[textBeforeSelectionLength-1], m_textSize)), 0);

            // Create the selection background
            sf::RectangleShape selectionBackground1(sf::Vector2f(m_textSelection1.findCharacterPos(textSelection1Length).x, static_cast<float>(m_lineHeight)));
            selectionBackground1.setFillColor(m_selectedTextBgrColor);

            // Draw the selection background
            target.draw(selectionBackground1, states);

            // Draw the first part of the selected text
            states.transform.translate(0, -textShift.y);
            target.draw(m_textSelection1, states);
            states.transform.translate(0, textShift.y);

            // Check if there is a second part in the selection
            if (m_textSelection2.getString().getSize() > 0)
            {
                // Translate to the beginning of the next line
                states.transform.translate(-m_textBeforeSelection.findCharacterPos(textBeforeSelectionLength).x, static_cast<float>(m_lineHeight));

                // If there was a kerning correction then undo it now
                if (textBeforeSelectionLength > 1)
                    states.transform.translate(static_cast<float>(-m_textBeforeSelection.getFont()->getKerning(m_displayedText[textBeforeSelectionLength-2], m_displayedText[textBeforeSelectionLength-1], m_textSize)), 0);

                // Create the second selection background
                sf::RectangleShape selectionBackground2;
                selectionBackground2.setFillColor(m_selectedTextBgrColor);

                // Draw the background rectangles of the selected text
                for (unsigned int i=0; i<m_multilineSelectionRectWidth.size(); ++i)
                {
                    selectionBackground2.setSize(sf::Vector2f(m_multilineSelectionRectWidth[i], static_cast<float>(m_lineHeight)));
                    target.draw(selectionBackground2, states);
                    selectionBackground2.move(0, static_cast<float>(m_lineHeight));
                }

                // Draw the second part of the selection
                states.transform.translate(0, -textShift.y);
                target.draw(m_textSelection2, states);
                states.transform.translate(0, textShift.y);

                // Translate to the end of the selection
                states.transform.translate(m_textSelection2.findCharacterPos(textSelection2Length));

                // Watch out for kerning
                if (m_displayedText.getSize() > textBeforeSelectionLength + textSelection1Length + textSelection2Length - 2)
                    states.transform.translate(static_cast<float>(m_textBeforeSelection.getFont()->getKerning(m_displayedText[textBeforeSelectionLength + textSelection1Length + textSelection2Length - 3], m_displayedText[textBeforeSelectionLength + textSelection1Length + textSelection2Length - 2], m_textSize)), 0);
            }
            else // The selection was only on one line
            {
                // Translate to the end of the selection
                states.transform.translate(m_textSelection1.findCharacterPos(textSelection1Length).x, 0);

                // Watch out for kerning
                if ((m_displayedText.getSize() > textBeforeSelectionLength + textSelection1Length - 2) && (textBeforeSelectionLength + textSelection1Length > 2))
                    states.transform.translate(static_cast<float>(m_textBeforeSelection.getFont()->getKerning(m_displayedText[textBeforeSelectionLength + textSelection1Length - 3], m_displayedText[textBeforeSelectionLength + textSelection1Length - 2], m_textSize)), 0);
            }

            // Draw the first part of the text behind the selection
            states.transform.translate(0, -textShift.y);
            target.draw(m_textAfterSelection1, states);
            states.transform.translate(textShift.x, textShift.y);

            // Check if there is a second part in the selection
            if (m_textAfterSelection2.getString().getSize() > 0)
            {
                // Translate to the beginning of the next line
                if (m_textSelection2.getString().getSize() > 0)
                {
                    // Undo the last translation
                    states.transform.translate(-m_textSelection2.findCharacterPos(textSelection2Length).x, static_cast<float>(m_lineHeight));

                    // If there was a kerning correction then undo it now
                    if (m_displayedText.getSize() > textBeforeSelectionLength + textSelection1Length + textSelection2Length - 2)
                        states.transform.translate(static_cast<float>(-m_textBeforeSelection.getFont()->getKerning(m_displayedText[textBeforeSelectionLength + textSelection1Length + textSelection2Length - 3], m_displayedText[textBeforeSelectionLength + textSelection1Length + textSelection2Length - 2], m_textSize)), 0);
                }
                else
                {
                    // Undo the last translation
                    states.transform.translate(-m_textSelection1.findCharacterPos(textSelection1Length).x - m_textBeforeSelection.findCharacterPos(textBeforeSelectionLength).x, static_cast<float>(m_lineHeight));

                    // If there was a kerning correction then undo it now
                    if (textBeforeSelectionLength > 1)
                        states.transform.translate(static_cast<float>(-m_textBeforeSelection.getFont()->getKerning(m_displayedText[textBeforeSelectionLength-2], m_displayedText[textBeforeSelectionLength-1], m_textSize)), 0);

                    // If there was a kerning correction then undo it now
                    if ((m_displayedText.getSize() > textBeforeSelectionLength + textSelection1Length - 2) && (textBeforeSelectionLength + textSelection1Length > 2))
                        states.transform.translate(static_cast<float>(-m_textBeforeSelection.getFont()->getKerning(m_displayedText[textBeforeSelectionLength + textSelection1Length - 3], m_displayedText[textBeforeSelectionLength + textSelection1Length - 2], m_textSize)), 0);
                }

                // Draw the second part of the text after the selection
                states.transform.translate(-textShift.x, -textShift.y);
                target.draw(m_textAfterSelection2, states);
                states.transform.translate(textShift.x, textShift.y);
            }
        }

        // Only draw the caret if it has a width
        if (m_caretWidth > 0)
        {
            // Only draw it when needed
            if ((m_focused) && (m_caretVisible))
            {
                // Reset the transformation
                states.transform = oldTransform;
                states.transform.translate(-textShift.x, 0);

                // Create the caret rectangle
                sf::RectangleShape caret(sf::Vector2f(static_cast<float>(m_caretWidth), static_cast<float>(m_lineHeight)));
                caret.setPosition(m_caretPosition.x - (m_caretWidth * 0.5f), static_cast<float>(m_caretPosition.y));
                caret.setFillColor(m_caretColor);

                // Draw the caret
                target.draw(caret, states);
            }
        }

        // Reset the old clipping area
        glScissor(scissor[0], scissor[1], scissor[2], scissor[3]);

        // Check if there is a scrollbar
        if (m_scroll != nullptr)
        {
            // Reset the transformation
            states.transform = origTransform;
            states.transform.translate(m_size.x - m_scroll->getSize().x, 0);

            // Draw the scrollbar
            target.draw(*m_scroll, states);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
