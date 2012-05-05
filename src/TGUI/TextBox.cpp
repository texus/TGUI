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

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    TextBox::TextBox() :
    m_Size    (360, 200),
    m_Text    (""),
    m_TextSize(30),
    m_MaxChars(0),
    m_Scroll  (NULL)
    {
        m_ObjectType = textBox;
        
        changeColors();
        m_Loaded = true;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    TextBox::~TextBox()
    {
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

        // Calculate the height of one line (a little bigger than text height)
        unsigned int lineHeight = textSize * 10 / 8;

        // There is also a minimum height
        if (height < (lineHeight + m_TopBorder + m_BottomBorder))
        {
            height = lineHeight + m_TopBorder + m_BottomBorder;
        }
        else // The height is ok
        {
            // Calculate two perfect heights
            unsigned int height1 = ((height - m_TopBorder - m_BottomBorder) / lineHeight) * lineHeight;
            unsigned int height2 = (((height - m_TopBorder - m_BottomBorder) / lineHeight) + 1) * lineHeight;

            // Calculate the difference with the original one
            unsigned int difference1, difference2;

            if ((height - m_TopBorder - m_BottomBorder) > height1)
                difference1 = (height - m_TopBorder - m_BottomBorder) - height1;
            else
                difference1 = height1 - (height - m_TopBorder - m_BottomBorder);

            if ((height - m_TopBorder - m_BottomBorder) > height2)
                difference2 = (height - m_TopBorder - m_BottomBorder) - height2;
            else
                difference2 = height2 - (height - m_TopBorder - m_BottomBorder);

            // Find out which one is closest to the original height and adjust the height
            if (difference1 < difference2)
                height = height1 + m_TopBorder + m_BottomBorder;
            else
                height = height2 + m_TopBorder + m_BottomBorder;
        }

        // Store the values
        m_Size.x = width;
        m_Size.y = height;
        m_TextSize = textSize;
        m_LineHeight = lineHeight;

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
                // Tell the scrollbar that this text box is his parent
//                m_Scroll->m_Parent = this;

                // The scrollbar has to be vertical
                m_Scroll->verticalScroll = true;

                // Set the low value
                m_Scroll->setLowValue((m_Size.y - m_TopBorder - m_BottomBorder) / m_LineHeight);

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

/// The width is important here too. Text must be recalculated.

        // Reset the scale
        setScale(1, 1);

        // The calculations require an unsigned integer
        unsigned int uiHeight = static_cast<unsigned int>(height);

        // There is a minimum width
        if (m_Scroll == NULL)
            width = TGUI_MAXIMUM(50 + m_LeftBorder + m_RightBorder, width);
        else
            width = TGUI_MAXIMUM(50 + m_LeftBorder + m_RightBorder + m_Scroll->m_TextureArrowNormal->getSize().x, width);

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
            m_Scroll->setLowValue((m_Size.y - m_TopBorder - m_BottomBorder) / m_LineHeight);
        }

    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TextBox::setText(const std::string text)
    {
        // Don't do anything when the button wasn't loaded correctly
        if (m_Loaded == false)
            return;

        // Store the text
        m_Text = text;

/// Text must be recalculated.

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

    std::string TextBox::getText()
    {
        return m_Text;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TextBox::setTextFont(const sf::Font& font)
    {
        m_Font = font;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const sf::Font& TextBox::getTextFont()
    {
        return m_Font;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TextBox::setTextSize(const unsigned int size)
    {
        // Change the text size
        m_TextSize = size;

/// Text must be recalculated.

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

/// Text must be cropped if too small.
/// Text must be recalculated.

/*
        // If there is a character limit then check if it is exeeded
        if ((m_MaxChars > 0) && (m_DisplayedText.length() > m_MaxChars))
        {
            // Remove all the excess characters
            while (m_DisplayedText.length() > m_MaxChars)
            {
                m_Text.erase(m_Text.length()-1, 1);
                m_DisplayedText.erase(m_Text.length()-1, 1);
            }

            // If we passed here then the internal text has changed. We also need to change the one to display.
            m_TextBeforeSelection.setString(m_DisplayedText);
            m_TextSelection.setString(std::string(""));
            m_TextAfterSelection.setString(std::string(""));

            // Check if scrolling is enabled
            if (m_LimitTextWidth == false)
            {
                // Check if the right crop position lies behind the character limit
                if (m_RightTextCrop > m_MaxChars)
                {
                    float width;

                    // Calculate the width of the edit box
                    if (m_SplitImage)
                        width = ((m_TextureNormal_L->getSize().x + m_TextureNormal_L->getSize().x + m_TextureNormal_L->getSize().x) * getScale().x)
                                 - ((m_LeftBorder + m_RightBorder) * getScale().y);
                    else
                        width = (m_TextureNormal_M->getSize().x - m_LeftBorder - m_RightBorder) * getScale().x;

                    // Check if the right crop position lies behind the character limit
                    while (m_RightTextCrop > m_MaxChars)
                    {
                        // Lower the right crop position
                        --m_RightTextCrop;

                        // Check if the left crop position can be changed
                        while (m_LeftTextCrop > 0)
                        {
                            // Check if it may be changed
                            if ((m_TextBeforeSelection.findCharacterPos(m_RightTextCrop).x - m_TextBeforeSelection.findCharacterPos(m_LeftTextCrop - 1).x) <= width)
                            {
                                // Change the left crop position
                                --m_LeftTextCrop;
                            }
                            else // The left crop position may no longer be changed
                                break;
                        }
                    }
                }
            }
        }

        // Set the selection point behind the last character
        setTextSelectionPoint(m_DisplayedText.length());
*/
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int TextBox::getMaximumCharacters()
    {
        return m_MaxChars;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TextBox::setBorders(unsigned int leftBorder, unsigned int topBorder, unsigned int rightBorder, unsigned int bottomBorder)
    {
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
            // If there is a text then it should still fit inside the listbox
            if (m_Text.size() > 0)
            {
                if (m_Size.y < ((m_Lines.size() * m_LineHeight) - m_TopBorder - m_BottomBorder))
                    m_Size.y = (m_Lines.size() * m_LineHeight) - m_TopBorder - m_BottomBorder;
            }
            else // There are no items
            {
                // At least one item should fit inside the listbox
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
            m_Scroll->setLowValue((m_Size.y - m_TopBorder - m_BottomBorder) / m_LineHeight);

            // Tell the scrollbar how many items there are
            m_Scroll->setMaximum(m_Lines.size());
        }

    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TextBox::changeColors(const sf::Color& color,
                               const sf::Color& selectedColor,
                               const sf::Color& selectedBgrColor,
                               const sf::Color& unfocusedSelectedBgrColor,
                               const sf::Color& selectionPointColor)
    {
        m_TextColor                     = color;
        m_SelectedTextColor             = selectedColor;
        m_SelectedTextBgrColor          = selectedBgrColor;
        m_UnfocusedSelectedTextBgrColor = unfocusedSelectedBgrColor;
        m_SelectionPointColor           = selectionPointColor;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const sf::Color& TextBox::getSelectedTextColor()
    {
        return m_SelectedTextColor;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const sf::Color& TextBox::getSelectedTextBackgroundColor()
    {
        return m_SelectedTextBgrColor;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const sf::Color& TextBox::getUnfocusedSelectedTextBackgroundColor()
    {
        return m_UnfocusedSelectedTextBgrColor;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const sf::Color& TextBox::getSelectionPointColor()
    {
        return m_SelectionPointColor;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool TextBox::setScrollbar(const std::string scrollbarPathname)
    {
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
            // Tell the scrollbar that this text box is his parent
//            m_Scroll->m_Parent = this;

            // The scrollbar has to be vertical
            m_Scroll->verticalScroll = true;

            // Set the low value
            m_Scroll->setLowValue((m_Size.y - m_TopBorder - m_BottomBorder) / m_LineHeight);

            return true;
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TextBox::removeScrollbar()
    {
        // Delete the scrollbar
        delete m_Scroll;
        m_Scroll = NULL;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TextBox::setSelectionPointWidth(const unsigned int width)
    {
        m_SelectionPointWidth = width;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool TextBox::mouseOnObject(float x, float y)
    {

/// TODO: complete this

        TGUI_UNUSED_PARAM(x);
        TGUI_UNUSED_PARAM(y);
        return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TextBox::leftMousePressed(float x, float y)
    {

/// TODO: complete this

        TGUI_UNUSED_PARAM(x);
        TGUI_UNUSED_PARAM(y);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TextBox::leftMouseReleased(float x, float y)
    {

/// TODO: complete this

        TGUI_UNUSED_PARAM(x);
        TGUI_UNUSED_PARAM(y);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TextBox::mouseMoved(float x, float y)
    {

/// TODO: complete this

        TGUI_UNUSED_PARAM(x);
        TGUI_UNUSED_PARAM(y);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TextBox::draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        // Adjust the transformation
        states.transform *= getTransform();

/// TODO: complete this

        TGUI_UNUSED_PARAM(target);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
