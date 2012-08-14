/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus's Graphical User Interface
// Copyright (C) 2012 Bruno Van de Velde (VDV_B@hotmail.com)
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


#include <TGUI/TGUI.hpp>

#include <SFML/OpenGL.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// TODO: Fix behavior problem: When pressing the down arrow when the selection point is at the beginning of the text,
///                             the selection point moves at the end of the line because it is not allowed to be in front.
///                             This problem will be hard to correct: m_DisplayedText may not be used as reference to m_SelEnd.
///                             This bug can thus only be solved after implementing word wrapping.

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    TextBox::TextBox() :
    selectionPointColor       (110, 110, 255),
    selectionPointWidth       (2),
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
    m_SelectionTextsNeedUpdate(true),
    m_Scroll                  (NULL),
    m_LoadedScrollbarPathname (""),
    m_PossibleDoubleClick     (false)
    {
        m_ObjectType = textBox;
        m_AnimatedObject = true;
        m_DraggableObject = true;

        changeColors();

        // Load the text box with default values
        m_Loaded = true;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    TextBox::TextBox(const TextBox& copy) :
    OBJECT                       (copy),
    OBJECT_BORDERS               (copy),
    OBJECT_ANIMATION             (copy),
    selectionPointColor          (copy.selectionPointColor),
    selectionPointWidth          (copy.selectionPointWidth),
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
    m_LoadedScrollbarPathname    (copy.m_LoadedScrollbarPathname),
    m_PossibleDoubleClick        (copy.m_PossibleDoubleClick)
    {
        // If there is a scrollbar then copy it
        if (copy.m_Scroll != NULL)
            m_Scroll = new Scrollbar(*copy.m_Scroll);
        else
            m_Scroll = NULL;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    TextBox::~TextBox()
    {
        if (m_Scroll != NULL)
            delete m_Scroll;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    TextBox& TextBox::operator= (const TextBox& right)
    {
        if (this != &right)
        {
            // If there already was a scrollbar then delete it now
            if (m_Scroll != NULL)
            {
                delete m_Scroll;
                m_Scroll = NULL;
            }

            TextBox temp(right);
            this->OBJECT::operator=(right);
            this->OBJECT_BORDERS::operator=(right);
            this->OBJECT_ANIMATION::operator=(right);

            std::swap(selectionPointColor,           temp.selectionPointColor);
            std::swap(selectionPointWidth,           temp.selectionPointWidth);
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
            std::swap(m_BackgroundColor,             temp.m_BackgroundColor);
            std::swap(m_SelectionTextsNeedUpdate,    temp.m_SelectionTextsNeedUpdate);
            std::swap(m_SelectedTextBgrColor,        temp.m_SelectedTextBgrColor);
            std::swap(m_BorderColor,                 temp.m_BorderColor);
            std::swap(m_TextBeforeSelection,         temp.m_TextBeforeSelection);
            std::swap(m_TextSelection1,              temp.m_TextSelection1);
            std::swap(m_TextSelection2,              temp.m_TextSelection2);
            std::swap(m_TextAfterSelection1,         temp.m_TextAfterSelection1);
            std::swap(m_TextAfterSelection2,         temp.m_TextAfterSelection2);
            std::swap(m_MultilineSelectionRectWidth, temp.m_MultilineSelectionRectWidth);
            std::swap(m_Scroll,                      temp.m_Scroll);
            std::swap(m_LoadedScrollbarPathname,     temp.m_LoadedScrollbarPathname);
            std::swap(m_PossibleDoubleClick,         temp.m_PossibleDoubleClick);
        }

        return *this;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TextBox::initialize()
    {
        setTextFont(m_Parent->globalFont);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    TextBox* TextBox::clone()
    {
        return new TextBox(*this);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool TextBox::load(unsigned int width, unsigned int height, unsigned int textSize, const std::string scrollbarPathname)
    {
        // When everything is loaded successfully, this will become true.
        m_Loaded = false;

        // If there already was a scrollbar then delete it now
        if (m_Scroll != NULL)
        {
            delete m_Scroll;
            m_Scroll = NULL;
        }

        // There is a minimum width
        if (width < (50 + m_LeftBorder + m_RightBorder))
            width = 50 + m_LeftBorder + m_RightBorder;

        // There is a minimum text size
        if (textSize < 8)
            textSize = 8;

        // Store the values
        m_Size.x = width;
        m_Size.y = height;
        m_LoadedScrollbarPathname = scrollbarPathname;

        // Set the text size
        setTextSize(textSize);

        // If there is a scrollbar then load it
        if (scrollbarPathname.empty() == false)
        {
            // load the scrollbar and check if it failed
            m_Scroll = new Scrollbar();
            if (m_Scroll->load(scrollbarPathname) == false)
            {
                // The scrollbar couldn't be loaded so it must be deleted
                delete m_Scroll;
                m_Scroll = NULL;

                return false;
            }
            else // The scrollbar was loaded successfully
            {
                // The scrollbar has to be vertical
                m_Scroll->verticalScroll = true;

                // Set the low value
                m_Scroll->setLowValue(m_Size.y - m_TopBorder - m_BottomBorder);

                m_Loaded = true;
                return true;
            }
        }
        else
        {
            m_Loaded = true;
            return true;
        }
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

        // A negative size is not allowed for this object
        if (width  < 0) width  = -width;
        if (height < 0) height = -height;

        // The calculations require an unsigned integer
        unsigned int uiHeight = static_cast<unsigned int>(height);

        // There is a minimum width
        if (m_Scroll == NULL)
            width = TGUI_MAXIMUM(50 + m_LeftBorder + m_RightBorder, width);
        else
            width = TGUI_MAXIMUM(50 + m_LeftBorder + m_RightBorder + m_Scroll->m_TextureArrowNormal->getSize().x, width);

        // There is also a minimum height
        if (m_Scroll == NULL)
        {
            // If there is a text then it should still fit inside the text box
            if (m_Text.getSize() > 0)
            {
                if (m_Size.y < ((m_Lines * m_LineHeight) - m_TopBorder - m_BottomBorder))
                    m_Size.y = (m_Lines * m_LineHeight) - m_TopBorder - m_BottomBorder;
            }
            else // There are no items
            {
                // At least one item should fit inside the text box
                if (m_Size.y < (m_LineHeight - m_TopBorder - m_BottomBorder))
                    m_Size.y = m_LineHeight - m_TopBorder - m_BottomBorder;
            }
        }
        else // There is a scrollbar
        {
            // At least one item should fit inside the text box
            if (m_Size.y < (m_LineHeight - m_TopBorder - m_BottomBorder))
                m_Size.y = m_LineHeight - m_TopBorder - m_BottomBorder;
        }

        // There is also a minimum height
        if (uiHeight < (m_LineHeight + m_TopBorder + m_BottomBorder))
        {
            uiHeight = m_LineHeight + m_TopBorder + m_BottomBorder;
        }
        else // The height isn't too low
        {
            // Calculate two perfect heights
            unsigned int height1 = ((uiHeight - m_TopBorder - m_BottomBorder) / m_LineHeight) * m_LineHeight;
            unsigned int height2 = (((uiHeight - m_TopBorder - m_BottomBorder) / m_LineHeight) + 1) * m_LineHeight;

            // Calculate the difference with the original one
            unsigned int difference1, difference2;

            if ((uiHeight - m_TopBorder - m_BottomBorder) > height1)
                difference1 = (uiHeight - m_TopBorder - m_BottomBorder) - height1;
            else
                difference1 = height1 - (uiHeight - m_TopBorder - m_BottomBorder);

            if ((uiHeight - m_TopBorder - m_BottomBorder) > height2)
                difference2 = (uiHeight - m_TopBorder - m_BottomBorder) - height2;
            else
                difference2 = height2 - (uiHeight - m_TopBorder - m_BottomBorder);

            // Find out which one is closest to the original height and adjust the height
            if (difference1 < difference2)
                uiHeight = height1 + m_TopBorder + m_BottomBorder;
            else
                uiHeight = height2 + m_TopBorder + m_BottomBorder;
        }

        // Store the values
        m_Size.x = static_cast<unsigned int>(width);
        m_Size.y = uiHeight;

        // If there is a scrollbar then change it
        if (m_Scroll != NULL)
        {
            // Set the low value
            m_Scroll->setLowValue(m_Size.y - m_TopBorder - m_BottomBorder);
        }

        // The size of the textbox has changed, update the text
        m_SelectionTextsNeedUpdate = true;
        updateDisplayedText();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Vector2u TextBox::getSize() const
    {
        return m_Size;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Vector2f TextBox::getScaledSize() const
    {
        return Vector2f(m_Size.x * getScale().x, m_Size.y * getScale().y);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::string TextBox::getLoadedScrollbarPathname()
    {
        return m_LoadedScrollbarPathname;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TextBox::setText(const sf::String text)
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

    void TextBox::addText(const sf::String text)
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

    sf::String TextBox::getText()
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
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const sf::Font* TextBox::getTextFont()
    {
        return m_TextBeforeSelection.getFont();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TextBox::setTextSize(const unsigned int size)
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

        // Calculate the height of one line
        m_LineHeight = m_TextBeforeSelection.getFont()->getLineSpacing(m_TextSize);

        // Don't continue when line height is 0
        if (m_LineHeight == 0)
            return;

        // There is also a minimum height
        if (m_Size.y < (m_LineHeight + m_TopBorder + m_BottomBorder))
        {
            m_Size.y = m_LineHeight + m_TopBorder + m_BottomBorder;
        }
        else // The height is ok
        {
            // Calculate two perfect heights
            unsigned int height1 = ((m_Size.y - m_TopBorder - m_BottomBorder) / m_LineHeight) * m_LineHeight;
            unsigned int height2 = (((m_Size.y - m_TopBorder - m_BottomBorder) / m_LineHeight) + 1) * m_LineHeight;

            // Calculate the difference with the original one
            unsigned int difference1, difference2;

            if ((m_Size.y - m_TopBorder - m_BottomBorder) > height1)
                difference1 = (m_Size.y - m_TopBorder - m_BottomBorder) - height1;
            else
                difference1 = height1 - (m_Size.y - m_TopBorder - m_BottomBorder);

            if ((m_Size.y - m_TopBorder - m_BottomBorder) > height2)
                difference2 = (m_Size.y - m_TopBorder - m_BottomBorder) - height2;
            else
                difference2 = height2 - (m_Size.y - m_TopBorder - m_BottomBorder);

            // Find out which one is closest to the original height and adjust the height
            if (difference1 < difference2)
                m_Size.y = height1 + m_TopBorder + m_BottomBorder;
            else
                m_Size.y = height2 + m_TopBorder + m_BottomBorder;
        }

        // If there is a scrollbar then update the low value (in case the height of the text box has changed)
        if (m_Scroll != NULL)
            m_Scroll->setLowValue(m_Size.y - m_TopBorder - m_BottomBorder);

        // The size has changed, update the text
        m_SelectionTextsNeedUpdate = true;
        updateDisplayedText();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int TextBox::getTextSize()
    {
        return m_TextSize;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TextBox::setMaximumCharacters(const unsigned int maxChars)
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

    unsigned int TextBox::getMaximumCharacters()
    {
        return m_MaxChars;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TextBox::setBorders(unsigned int leftBorder, unsigned int topBorder, unsigned int rightBorder, unsigned int bottomBorder)
    {
        // Don't do anything when the text box wasn't loaded correctly
        if (m_Loaded == false)
            return;

        // Don't continue when line height is 0
        if (m_LineHeight == 0)
            return;

        // Set the new border size
        m_LeftBorder   = leftBorder;
        m_TopBorder    = topBorder;
        m_RightBorder  = rightBorder;
        m_BottomBorder = bottomBorder;

        // There is a minimum width
        if (m_Size.x < (50 + m_LeftBorder + m_RightBorder))
            m_Size.x = 50 + m_LeftBorder + m_RightBorder;

        // There is also a minimum height (when there is no scrollbar)
        if (m_Scroll == NULL)
        {
            // If there is a text then it should still fit inside the text box
            if (m_Text.getSize() > 0)
            {
                if (m_Size.y < ((m_Lines * m_LineHeight) - m_TopBorder - m_BottomBorder))
                    m_Size.y = (m_Lines * m_LineHeight) - m_TopBorder - m_BottomBorder;
            }
            else // There are no items
            {
                // At least one item should fit inside the text box
                if (m_Size.y < (m_LineHeight - m_TopBorder - m_BottomBorder))
                    m_Size.y = m_LineHeight - m_TopBorder - m_BottomBorder;
            }
        }

        // Calculate two perfect heights
        unsigned int height1 = ((m_Size.y - m_TopBorder - m_BottomBorder) / m_LineHeight) * m_LineHeight;
        unsigned int height2 = (((m_Size.y - m_TopBorder - m_BottomBorder) / m_LineHeight) + 1) * m_LineHeight;

        // Calculate the difference with the original one
        unsigned int difference1, difference2;

        if ((m_Size.y - m_TopBorder - m_BottomBorder) > height1)
            difference1 = (m_Size.y - m_TopBorder - m_BottomBorder) - height1;
        else
            difference1 = height1 - (m_Size.y - m_TopBorder - m_BottomBorder);

        if ((m_Size.y - m_TopBorder - m_BottomBorder) > height2)
            difference2 = (m_Size.y - m_TopBorder - m_BottomBorder) - height2;
        else
            difference2 = height2 - (m_Size.y - m_TopBorder - m_BottomBorder);

        // Find out which one is closest to the original height and adjust the height
        if (difference1 < difference2)
            m_Size.y = height1 + m_TopBorder + m_BottomBorder;
        else
            m_Size.y = height2 + m_TopBorder + m_BottomBorder;

        // Check if there is a scrollbar
        if (m_Scroll != NULL)
        {
            // Set the low value
            m_Scroll->setLowValue(m_Size.y - m_TopBorder - m_BottomBorder);
        }

        // The space for the text has changed, so update the text
        m_SelectionTextsNeedUpdate = true;
        updateDisplayedText();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TextBox::changeColors(const sf::Color& backgroundColor,
                               const sf::Color& color,
                               const sf::Color& selectedColor,
                               const sf::Color& selectedBgrColor,
                               const sf::Color& borderColor,
                               const sf::Color& newSelectionPointColor)
    {
        m_TextBeforeSelection.setColor(color);
        m_TextSelection1.setColor(selectedColor);
        m_TextSelection2.setColor(selectedColor);
        m_TextAfterSelection1.setColor(color);
        m_TextAfterSelection2.setColor(color);

        selectionPointColor             = newSelectionPointColor;
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

    const sf::Color& TextBox::getBackgroundColor()
    {
        return m_BackgroundColor;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const sf::Color& TextBox::getTextColor()
    {
        return m_TextBeforeSelection.getColor();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const sf::Color& TextBox::getSelectedTextColor()
    {
        return m_TextSelection1.getColor();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const sf::Color& TextBox::getSelectedTextBackgroundColor()
    {
        return m_SelectedTextBgrColor;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const sf::Color& TextBox::getBorderColor()
    {
        return m_BorderColor;
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
        if (m_Scroll != NULL)
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
            if ((newlines < m_TopLine - 1) || ((newlines < m_TopLine) && (m_Scroll->m_Value % m_LineHeight > 0)))
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
            else if ((newlines > m_TopLine + m_VisibleLines - 3) && (m_Scroll->m_Value % m_LineHeight > 0))
            {
                m_Scroll->setValue((newlines - m_VisibleLines + 2) * m_LineHeight);
                updateDisplayedText();
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool TextBox::setScrollbar(const std::string scrollbarPathname)
    {
        // Store the pathname
        m_LoadedScrollbarPathname = scrollbarPathname;

        // Do nothing when the string is empty
        if (scrollbarPathname.empty() == true)
            return false;

        // If the scrollbar was already created then delete it first
        if (m_Scroll != NULL)
            delete m_Scroll;

        // load the scrollbar and check if it failed
        m_Scroll = new Scrollbar();
        if(m_Scroll->load(scrollbarPathname) == false)
        {
            // The scrollbar couldn't be loaded so it must be deleted
            delete m_Scroll;
            m_Scroll = NULL;

            return false;
        }
        else // The scrollbar was loaded successfully
        {
            // The scrollbar has to be vertical
            m_Scroll->verticalScroll = true;

            // Set the low value
            m_Scroll->setLowValue(m_Size.y - m_TopBorder - m_BottomBorder);

            // Tell the scrollbar how many pixels the text contains
            m_Scroll->setMaximum(m_Lines * m_LineHeight);

            return true;
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TextBox::removeScrollbar()
    {
        // Delete the scrollbar
        delete m_Scroll;
        m_Scroll = NULL;

        m_TopLine = 1;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TextBox::setSelectionPointWidth(const unsigned int width)
    {
        selectionPointWidth = width;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool TextBox::mouseOnObject(float x, float y)
    {
        // Don't do anything when the text box wasn't loaded correctly
        if (m_Loaded == false)
            return false;

        // Get the current position and scale
        Vector2f position = getPosition();
        Vector2f curScale = getScale();

        // Pass the event to the scrollbar (if there is one)
        if (m_Scroll != NULL)
        {
            // Temporarily set the position and scale of the scroll
            m_Scroll->setPosition(position.x + ((m_Size.x - m_RightBorder) * curScale.x) - m_Scroll->getSize().x, position.y + (m_TopBorder * curScale.y));
            m_Scroll->setScale(1, (curScale.y * (m_Size.y- m_TopBorder - m_BottomBorder)) / m_Scroll->getSize().y);

            // Pass the event
            m_Scroll->mouseOnObject(x, y);

            // Reset the position and scale
            m_Scroll->setPosition(0, 0);
            m_Scroll->setScale(1, 1);
        }

        // Check if the mouse is on top of the listbox
        if (getTransform().transformRect(sf::FloatRect(static_cast<float>(m_LeftBorder), static_cast<float>(m_TopBorder), static_cast<float>(m_Size.x - m_LeftBorder - m_RightBorder), static_cast<float>(m_Size.y - m_TopBorder - m_BottomBorder))).contains(x, y))
        {
            return true;
        }
        else // The mouse is not on top of the listbox
        {
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

        // Set the mouse down flag to true
        m_MouseDown = true;

        // This will be true when the click didn't occur on the scrollbar
        bool clickedOnTextBox = true;

        // If there is a scrollbar then pass the event
        if (m_Scroll != NULL)
        {
            // Remember the old scrollbar value
            unsigned int oldValue = m_Scroll->m_Value;

            // Get the current scale
            Vector2f curScale = getScale();

            // Temporarily set the position and scale of the scroll
            m_Scroll->setPosition(getPosition().x + ((m_Size.x - m_RightBorder) * curScale.x) - m_Scroll->getSize().x, getPosition().y + (m_TopBorder * curScale.y));
            m_Scroll->setScale(1, (curScale.y * (m_Size.y- m_TopBorder - m_BottomBorder)) / m_Scroll->getSize().y);

            // Pass the event
            if (m_Scroll->mouseOnObject(x, y))
            {
                m_Scroll->leftMousePressed(x, y);
                clickedOnTextBox = false;
            }

            // Reset the position and scale
            m_Scroll->setPosition(0, 0);
            m_Scroll->setScale(1, 1);

            // If the value of the scrollbar has changed then update the text
            if (oldValue != m_Scroll->m_Value)
                updateDisplayedText();
        }

        // If the click occured on the text box
        if (clickedOnTextBox)
        {
            // Don't continue when line height is 0
            if (m_LineHeight == 0)
                return;

            unsigned int selectionPointPosition = findSelectionPointPosition(x - getPosition().x - m_LeftBorder - 4, y - getPosition().y - m_TopBorder);

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
                if (m_Scroll != NULL)
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
                    if ((newlines < m_TopLine - 1) || ((newlines < m_TopLine) && (m_Scroll->m_Value % m_LineHeight > 0)))
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
                    else if ((newlines > m_TopLine + m_VisibleLines - 3) && (m_Scroll->m_Value % m_LineHeight > 0))
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

            // Set the mouse down flag
            m_MouseDown = true;

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
        if (m_Scroll != NULL)
        {
            // Only pass the event when the scrollbar still thinks the mouse is down
            if (m_Scroll->m_MouseDown == true)
            {
                // Don't continue when line height is 0
                if (m_LineHeight == 0)
                    return;

                // Remember the old scrollbar value
                unsigned int oldValue = m_Scroll->m_Value;

                // Get the current scale
                Vector2f curScale = getScale();

                // Temporarily set the position and scale of the scroll
                m_Scroll->setPosition(getPosition().x + ((m_Size.x - m_RightBorder) * curScale.x) - m_Scroll->getSize().x, getPosition().y + (m_TopBorder * curScale.y));
                m_Scroll->setScale(1, (curScale.y * (m_Size.y- m_TopBorder - m_BottomBorder)) / m_Scroll->getSize().y);

                // Pass the event
                m_Scroll->leftMouseReleased(x, y);

                // Reset the position and scale
                m_Scroll->setPosition(0, 0);
                m_Scroll->setScale(1, 1);

                // If the value of the scrollbar has changed then update the text
                if (oldValue != m_Scroll->m_Value)
                {
                    updateDisplayedText();

                    // Check if the scrollbar value was incremented (you have pressed on the down arrow)
                    if (m_Scroll->m_Value == oldValue + 1)
                    {
                        // Decrement the value
                        --m_Scroll->m_Value;

                        // Scroll down with the whole item height instead of with a single pixel
                        m_Scroll->setValue(m_Scroll->m_Value + m_LineHeight - (m_Scroll->m_Value % m_LineHeight));
                    }
                    else if (m_Scroll->m_Value == oldValue - 1) // Check if the scrollbar value was decremented (you have pressed on the up arrow)
                    {
                        // increment the value
                        ++m_Scroll->m_Value;

                        // Scroll up with the whole item height instead of with a single pixel
                        if (m_Scroll->m_Value % m_LineHeight > 0)
                            m_Scroll->setValue(m_Scroll->m_Value - (m_Scroll->m_Value % m_LineHeight));
                        else
                            m_Scroll->setValue(m_Scroll->m_Value - m_LineHeight);
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

        // Set the mouse move flag
        m_MouseHover = true;

        // Get the current scale
        Vector2f curScale = getScale();

        // If there is a scrollbar then pass the event
        if (m_Scroll != NULL)
        {
            // Temporarily set the position and scale of the scroll
            m_Scroll->setPosition(getPosition().x + ((m_Size.x - m_RightBorder) * curScale.x) - m_Scroll->getSize().x, getPosition().y + (m_TopBorder * curScale.y));
            m_Scroll->setScale(1, (curScale.y * (m_Size.y- m_TopBorder - m_BottomBorder)) / m_Scroll->getSize().y);

            // Check if you are dragging the thumb of the scrollbar
            if ((m_Scroll->m_MouseDown) && (m_Scroll->m_MouseDownOnThumb))
            {
                // Remember the old scrollbar value
                unsigned int oldValue = m_Scroll->m_Value;

                // Pass the event, even when the mouse is not on top of the scrollbar
                m_Scroll->mouseMoved(x, y);

                // If the value of the scrollbar has changed then update the text
                if (oldValue != m_Scroll->m_Value)
                    updateDisplayedText();
            }
            else // You are just moving the mouse
            {
                // When the mouse is on top of the scrollbar then pass the mouse move event
                if (m_Scroll->mouseOnObject(x, y))
                    m_Scroll->mouseMoved(x, y);

                // If the mouse is down then you are selecting text
                if (m_MouseDown)
                    selectText(x, y);
            }

            // Reset the position and scale
            m_Scroll->setPosition(0, 0);
            m_Scroll->setScale(1, 1);
        }
        else // There is no scrollbar
        {
            // If the mouse is down then you are selecting text
            if (m_MouseDown)
                selectText(x, y);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TextBox::mouseNotOnObject()
    {
        m_MouseHover = false;

        if (m_Scroll != NULL)
            m_Scroll->m_MouseHover = false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TextBox::mouseNoLongerDown()
    {
        m_MouseDown = false;

        if (m_Scroll != NULL)
            m_Scroll->m_MouseDown = false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TextBox::keyPressed(sf::Keyboard::Key key)
    {
        // Don't do anything when the edit box wasn't loaded correctly
        if (m_Loaded == false)
            return;

        // Check if one of the correct keys was pressed
        if (key == sf::Keyboard::Left)
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
        else if (key == sf::Keyboard::Right)
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
        else if (key == sf::Keyboard::Up)
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
        else if (key == sf::Keyboard::Down)
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
        else if (key == sf::Keyboard::Home)
        {
            // Set the selection point to the beginning of the text
            setSelectionPointPosition(0);

            // Our selection point has moved, it should be visible
            m_SelectionPointVisible = true;
            m_AnimationTimeElapsed = sf::Time();
        }
        else if (key == sf::Keyboard::End)
        {
            // Set the selection point behind the text
            setSelectionPointPosition(m_Text.getSize());

            // Our selection point has moved, it should be visible
            m_SelectionPointVisible = true;
            m_AnimationTimeElapsed = sf::Time();
        }
        else if (key == sf::Keyboard::Return)
        {
            // Add a newline
            textEntered('\n');
        }
        else if (key == sf::Keyboard::BackSpace)
        {
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

                if (m_Scroll != NULL)
                {
                    // Check if the scrollbar is behind the text
                    if (m_Scroll->m_Value > m_Scroll->m_Maximum - m_Scroll->m_LowValue)
                    {
                        // Adjust the value of the scrollbar
                        m_Scroll->setValue(m_Scroll->m_Value);

                        // The text has to be updated again
                        m_SelectionTextsNeedUpdate = true;
                        updateDisplayedText();
                    }
                }
            }
            else // When you did select some characters
            {
              deleteSelectedCharacters:

                // Erase the characters
                m_Text.erase(TGUI_MINIMUM(m_SelStart, m_SelEnd), m_SelChars);

                // Set the selection point back on the correct position
                setSelectionPointPosition(TGUI_MINIMUM(m_SelStart, m_SelEnd));
            }

            // The selection point should be visible again
            m_SelectionPointVisible = true;
            m_AnimationTimeElapsed = sf::Time();

            // Add the callback (if the user requested it)
            if (callbackID > 0)
            {
                Callback callback;
                callback.object     = this;
                callback.callbackID = callbackID;
                callback.trigger    = Callback::textChanged;
                callback.text       = m_Text;
                m_Parent->addCallback(callback);
            }
        }
        else if (key == sf::Keyboard::Delete)
        {
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

                if (m_Scroll != NULL)
                {
                    // Check if there is a risk that the scrollbar is going to be behind the text
                    if ((m_Scroll->m_Value == m_Scroll->m_Maximum - m_Scroll->m_LowValue) || (m_Scroll->m_Value > m_Scroll->m_Maximum - m_Scroll->m_LowValue - m_LineHeight))
                    {
                        // Reset the value of the scroll. If it is too high then it will be automatically be adjusted.
                        m_Scroll->setValue(m_Scroll->m_Value);

                        // The text has to be updated again
                        m_SelectionTextsNeedUpdate = true;
                        updateDisplayedText();
                    }
                }
            }
            else // You did select some characters
            {
                // This code is exactly the same as when pressing backspace
                goto deleteSelectedCharacters;
            }

            // The selection point should be visible again
            m_SelectionPointVisible = true;
            m_AnimationTimeElapsed = sf::Time();

            // Add the callback (if the user requested it)
            if (callbackID > 0)
            {
                Callback callback;
                callback.object     = this;
                callback.callbackID = callbackID;
                callback.trigger    = Callback::textChanged;
                callback.text       = m_Text;
                m_Parent->addCallback(callback);
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TextBox::textEntered(sf::Uint32 key)
    {
        // Don't do anything when the text box wasn't loaded correctly
        if (m_Loaded == false)
            return;

        // If there were selected characters then delete them first
        if (m_SelChars != 0)
            keyPressed(sf::Keyboard::BackSpace);

        // Make sure we don't exceed our maximum characters limit
        if ((m_MaxChars > 0) && (m_Text.getSize() + 1 > m_MaxChars))
                return;

        // If there is a limit in the amount of lines then make a simulation
        if (m_Scroll == NULL)
        {
            // Don't continue when line height is 0
            if (m_LineHeight == 0)
                return;

            float maxLineWidth;

            // Calculate the maximum line width
            if (m_Scroll == NULL)
                maxLineWidth = static_cast<float>(m_Size.x - m_LeftBorder - m_RightBorder - 4);
            else
                maxLineWidth = m_Size.x - m_LeftBorder - m_TopBorder - m_Scroll->getScaledSize().x - 4;

            // If the width is negative then the text box is too small to be displayed
            if (maxLineWidth < 0)
                maxLineWidth = 0;

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
                    // Add the next character to the text object
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
                if (lines > (m_Size.y - m_TopBorder - m_BottomBorder) / m_LineHeight)
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
        if (callbackID > 0)
        {
            Callback callback;
            callback.object     = this;
            callback.callbackID = callbackID;
            callback.trigger    = Callback::textChanged;
            callback.text       = m_Text;
            m_Parent->addCallback(callback);
        }
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
        if (m_Scroll == NULL)
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
            if (posY + m_Scroll->m_Value < 0)
                return 0;
            else
                line = static_cast<unsigned int>((posY + m_Scroll->m_Value) / m_LineHeight + 1);
        }

        // Create a temporary text object that contains the full text
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

            // Create a temporary text object
            sf::Text tempText(m_TextBeforeSelection);

            // We are going to calculate the number of newlines we have added
            unsigned int newlinesAdded = 0;
            unsigned int totalNewlinesAdded = 0;
            unsigned int beginChar = 0;
            sf::String   tempString2 = m_Text;

            // Calculate the maximum line width
            float maxLineWidth;

            if (m_Scroll == NULL)
                maxLineWidth = static_cast<float>(m_Size.x - m_LeftBorder - m_RightBorder - 4);
            else
                maxLineWidth = m_Size.x - m_LeftBorder - m_RightBorder - 4 - m_Scroll->getScaledSize().x;

            // If the width is negative then the text box is too small to be displayed
            if (maxLineWidth < 0)
                maxLineWidth = 0;

            // Loop through every character
            for (unsigned i=1; (i < m_Text.getSize() + 1) && (totalNewlinesAdded != line - 1); ++i)
            {
                // Make sure the character is not a newline
                if (m_Text[i-1] != '\n')
                {
                    // Add the next character to the text object
                    tempText.setString(m_Text.toWideString().substr(beginChar, i - beginChar));

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

            // Store the single line that we found a while ago in the temporary text object
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
            for (unsigned int i=1; i<=tempString.getSize(); ++i)
            {
                if (posX < (tempText.findCharacterPos(i-1).x + tempText.findCharacterPos(i).x) / 2.f)
                {
                    if (line > 1)
                        return newlinePos + i - newlinesAdded;
                    else
                        return newlinePos + i - 1;
                }

            }

            // Check if you clicked behind the last character on the line
            if (tempText.findCharacterPos(tempString.getSize()).x - (((3.f * tempText.findCharacterPos(tempString.getSize()-1).x) + tempText.findCharacterPos(tempString.getSize()).x) / 2.f))
            {
                if (line > 1)
                    return newlinePos + tempString.getSize() + 1 - newlinesAdded;
                else
                    return newlinePos + tempString.getSize();
            }
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
        m_SelEnd = findSelectionPointPosition(posX - getPosition().x - m_LeftBorder - 4, posY - getPosition().y - m_TopBorder);

        // Calculate how many character are being selected
        if (m_SelEnd < m_SelStart)
            m_SelChars = m_SelStart - m_SelEnd;
        else
            m_SelChars = m_SelEnd - m_SelStart;

        // Update the text
        m_SelectionTextsNeedUpdate = true;
        updateDisplayedText();

        // Check if there is a scrollbar
        if (m_Scroll != NULL)
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
            if ((newlines < m_TopLine - 1) || ((newlines < m_TopLine) && (m_Scroll->m_Value % m_LineHeight > 0)))
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
            else if ((newlines > m_TopLine + m_VisibleLines - 3) && (m_Scroll->m_Value % m_LineHeight > 0))
            {
                m_Scroll->setValue((newlines - m_VisibleLines + 2) * m_LineHeight);
                updateDisplayedText();
            }
        }
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

        float maxLineWidth;

        // Calculate the maximum line width
        if (m_Scroll == NULL)
            maxLineWidth = static_cast<float>(m_Size.x - m_LeftBorder - m_RightBorder - 4);
        else
            maxLineWidth = m_Size.x - m_LeftBorder - m_TopBorder - m_Scroll->getScaledSize().x - 4;

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
                // Add the next character to the text object
                tempText.setString(m_Text.toWideString().substr(beginChar, i - beginChar));

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
            if (m_Scroll == NULL)
            {
                // Check if you passed this limit
                if (m_Lines > (m_Size.y - m_TopBorder - m_BottomBorder) / m_LineHeight)
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
        if (m_Scroll != NULL)
        {
            // Tell the scrollbar how many pixels the text contains
            m_Scroll->setMaximum(m_Lines * m_LineHeight);

            // Calculate the top line
            m_TopLine = m_Scroll->m_Value / m_LineHeight + 1;

            // Calculate the number of visible lines
            if ((m_Scroll->m_Value % m_LineHeight) == 0)
                m_VisibleLines = TGUI_MINIMUM((m_Size.y - m_LeftBorder - m_TopBorder) / m_LineHeight, m_Lines);
            else
                m_VisibleLines = TGUI_MINIMUM(((m_Size.y - m_LeftBorder - m_TopBorder) / m_LineHeight) + 1, m_Lines);
        }
        else // There is no scrollbar
        {
            // You are always at the top line
            m_TopLine = 1;

            // Calculate the number of visible lines
            m_VisibleLines = TGUI_MINIMUM((m_Size.y - m_LeftBorder - m_TopBorder) / m_LineHeight, m_Lines);
        }

        // Fill the temporary text object with the whole text
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
                    // Add the next character to the text object
                    tempText.setString(m_Text.toWideString().substr(beginChar, i - beginChar + 1));

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
                    // Add the next character to the text object
                    tempText.setString(m_Text.toWideString().substr(beginChar, i - beginChar + 1));

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

            // Create another temprary text object
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
            m_TextBeforeSelection.setString(m_DisplayedText.toWideString().substr(0, selectionStart));

            // Set the text that is selected. If it consists of multiple lines then it will be changed below.
            m_TextSelection1.setString(m_DisplayedText.toWideString().substr(selectionStart, m_SelChars));
            m_TextSelection2.setString("");

            // Loop through every character inside the selection
            for (i=selectionStart; i < selectionEnd; ++i)
            {
                // Check if the character is a newline
                if (m_DisplayedText[i] == '\n')
                {
                    // Set the text that is selected
                    m_TextSelection1.setString(m_DisplayedText.toWideString().substr(selectionStart, i - selectionStart));
                    m_TextSelection2.setString(m_DisplayedText.toWideString().substr(i + 1, m_SelChars + newlinesAddedInsideSelection + selectionStart - i - 1));
                    break;
                }
            }

            // Set the text after the selection. If it consists of multiple lines then it will be changed below.
            m_TextAfterSelection1.setString(m_DisplayedText.toWideString().substr(selectionEnd, m_DisplayedText.getSize() - selectionEnd));
            m_TextAfterSelection2.setString("");

            // Loop through every character after the selection
            for (i=selectionEnd; i < m_DisplayedText.getSize(); ++i)
            {
                // Check if the character is a newline
                if (m_DisplayedText[i] == '\n')
                {
                    // Set the text that is selected
                    m_TextAfterSelection1.setString(m_DisplayedText.toWideString().substr(selectionEnd, i - selectionEnd));
                    m_TextAfterSelection2.setString(m_DisplayedText.toWideString().substr(i + 1, m_DisplayedText.getSize() - i - 1));
                    break;
                }
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TextBox::objectUnfocused()
    {
        // If there is a selection then undo it now
        if (m_SelChars)
            setSelectionPointPosition(m_SelEnd);
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
        m_SelectionPointVisible = m_SelectionPointVisible ? false : true;

        // Too slow for double clicking
        m_PossibleDoubleClick = false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TextBox::draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        // Don't draw anything when the text box wasn't loaded correctly
        if (m_Loaded == false)
            return;

        // Calculate the scale factor of the view
        float scaleViewX = target.getSize().x / target.getView().getSize().x;
        float scaleViewY = target.getSize().y / target.getView().getSize().y;

        // Get the global translation
        Vector2f globalTranslation = states.transform.transformPoint(getPosition() - target.getView().getCenter() + (target.getView().getSize() / 2.f));

        // Store the
        sf::Transform origTransform = states.transform;

        // Adjust the transformation
        states.transform *= getTransform();

        // Draw the borders
        sf::RectangleShape back(Vector2f(static_cast<float>(m_Size.x), static_cast<float>(m_Size.y)));
        back.setFillColor(m_BorderColor);
        target.draw(back, states);

        // Don't draw on top of the borders
        states.transform.translate(static_cast<float>(m_LeftBorder), static_cast<float>(m_TopBorder));

        // Draw the background
        sf::RectangleShape front(Vector2f(static_cast<float>(m_Size.x - m_LeftBorder - m_RightBorder), static_cast<float>(m_Size.y - m_TopBorder - m_BottomBorder)));
        front.setFillColor(m_BackgroundColor);
        target.draw(front, states);

        // Set the text on the correct position
        if (m_Scroll != NULL)
            states.transform.translate(2, -static_cast<float>(m_Scroll->m_Value));
        else
            states.transform.translate(2, 0);

        // Remeber this tranformation
        sf::Transform oldTransform = states.transform;

        // Get the old clipping area
        GLint scissor[4];
        glGetIntegerv(GL_SCISSOR_BOX, scissor);

        // Calculate the clipping area
        GLint scissorLeft = TGUI_MAXIMUM(static_cast<GLint>((globalTranslation.x + m_LeftBorder) * scaleViewX), scissor[0]);
        GLint scissorTop = TGUI_MAXIMUM(static_cast<GLint>((globalTranslation.y + m_TopBorder) * scaleViewY), static_cast<GLint>(target.getSize().y) - scissor[1] - scissor[3]);
        GLint scissorRight = TGUI_MINIMUM(static_cast<GLint>((globalTranslation.x + m_Size.x - m_RightBorder) * scaleViewX), scissor[0] + scissor[2]);
        GLint scissorBottom = TGUI_MINIMUM(static_cast<GLint>((globalTranslation.y + m_Size.y - m_BottomBorder) * scaleViewY), static_cast<GLint>(target.getSize().y) - scissor[1]);

        // If the object outside the window then don't draw anything
        if (scissorRight < scissorLeft)
            scissorRight = scissorLeft;
        else if (scissorBottom < scissorTop)
            scissorTop = scissorBottom;

        // Set the clipping area
        glScissor(scissorLeft, target.getSize().y - scissorBottom, scissorRight - scissorLeft, scissorBottom - scissorTop);

        // Draw the text
        target.draw(m_TextBeforeSelection, states);

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
            target.draw(m_TextSelection1, states);

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
                target.draw(m_TextSelection2, states);

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
            target.draw(m_TextAfterSelection1, states);

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
                target.draw(m_TextAfterSelection2, states);
            }
        }

        // Only draw the selection point if it has a width
        if (selectionPointWidth > 0)
        {
            // Only draw it when needed
            if ((m_Focused) && (m_SelectionPointVisible))
            {
                // Reset the transformation
                states.transform = oldTransform;

                // Create the selection point rectangle
                sf::RectangleShape selectionPoint(sf::Vector2f(static_cast<float>(selectionPointWidth), static_cast<float>(m_LineHeight)));
                selectionPoint.setPosition(m_SelectionPointPosition.x - (selectionPointWidth * 0.5f), static_cast<float>(m_SelectionPointPosition.y));
                selectionPoint.setFillColor(selectionPointColor);

                // Draw the selection point
                target.draw(selectionPoint, states);
            }
        }

        // Reset the old clipping area
        glScissor(scissor[0], scissor[1], scissor[2], scissor[3]);

        // Check if there is a scrollbar
        if (m_Scroll != NULL)
        {
            // Reset the transformation
            states.transform = origTransform;
            states.transform.translate(getPosition().x + ((m_Size.x - m_RightBorder) * getScale().x) - m_Scroll->getSize().x, getPosition().y + m_TopBorder * getScale().y);
            m_Scroll->setScale(1, (getScale().y * (m_Size.y - m_TopBorder - m_BottomBorder)) / m_Scroll->getSize().y);

            // Draw the scrollbar
            target.draw(*m_Scroll, states);

            // Reset the scale of the scrollbar
            m_Scroll->setScale(1, 1);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
