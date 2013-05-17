/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus's Graphical User Interface
// Copyright (C) 2012 Bruno Van de Velde (vdv_b@hotmail.com)
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

    ChatBox::ChatBox() :
    m_TextSize               (15),
    m_BorderColor            (sf::Color::Black),
    m_Scroll                 (NULL),
    m_LoadedScrollbarPathname("")
    {
        m_ObjectType = chatBox;
        m_DraggableObject = true;

        m_Panel = new Panel();
        m_Panel->load(360, 200, sf::Color::White);

        // Load the chat box with default values
        m_Loaded = true;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ChatBox::ChatBox(const ChatBox& copy) :
    OBJECT                   (copy),
    OBJECT_BORDERS           (copy),
    m_TextSize               (copy.m_TextSize),
    m_BorderColor            (copy.m_BorderColor),
    m_LoadedScrollbarPathname(copy.m_LoadedScrollbarPathname)
    {
        m_Panel = new Panel(*copy.m_Panel);

        // If there is a scrollbar then copy it
        if (copy.m_Scroll != NULL)
            m_Scroll = new Scrollbar(*copy.m_Scroll);
        else
            m_Scroll = NULL;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ChatBox::~ChatBox()
    {
        delete m_Panel;

        if (m_Scroll != NULL)
            delete m_Scroll;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ChatBox& ChatBox::operator= (const ChatBox& right)
    {
        if (this != &right)
        {
            // If there already was a scrollbar then delete it now
            if (m_Scroll != NULL)
            {
                delete m_Scroll;
                m_Scroll = NULL;
            }

            ChatBox temp(right);
            this->OBJECT::operator=(right);
            this->OBJECT_BORDERS::operator=(right);

            std::swap(m_TextSize,                temp.m_TextSize);
            std::swap(m_BorderColor,             temp.m_BorderColor);
            std::swap(m_Panel,                   temp.m_Panel);
            std::swap(m_Scroll,                  temp.m_Scroll);
            std::swap(m_LoadedScrollbarPathname, temp.m_LoadedScrollbarPathname);
        }

        return *this;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChatBox::initialize()
    {
        setTextFont(m_Parent->globalFont);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ChatBox* ChatBox::clone()
    {
        return new ChatBox(*this);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ChatBox::load(float width, float height, unsigned int textSize, const std::string& scrollbarPathname)
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
        m_Panel->setSize(width, height);
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
                // Initialize the scrollbar
                m_Scroll->setVerticalScroll(true);
                m_Scroll->setLowValue(static_cast<unsigned int>(m_Panel->getSize().y - m_TopBorder - m_BottomBorder));
                m_Scroll->setSize(m_Scroll->getSize().x, m_Panel->getSize().y - m_TopBorder - m_BottomBorder);
                m_Scroll->setMaximum(static_cast<unsigned int>(m_Panel->getObjects().size() * m_TextSize * 1.4f));
            }
        }

        m_Loaded = true;
        return true;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChatBox::setSize(float width, float height)
    {
        // Don't do anything when the text box wasn't loaded correctly
        if (m_Loaded == false)
            return;

        // A negative size is not allowed for this object
        if (width  < 0) width  = -width;
        if (height < 0) height = -height;

        // There is a minimum width
        if (m_Scroll == NULL)
            width = TGUI_MAXIMUM(50 + m_LeftBorder + m_RightBorder, width);
        else
            width = TGUI_MAXIMUM(50 + m_LeftBorder + m_RightBorder + m_Scroll->getSize().x, width);

        // Get a valid height
        height = calculateValidHeight(height);

        // Remember the old height
        float oldHeight = m_Panel->getSize().y;

        // Set the new size
        m_Panel->setSize(width, height);

        // If there is a scrollbar then reinitialize it
        if (m_Scroll != NULL)
        {
            m_Scroll->setLowValue(static_cast<unsigned int>(m_Panel->getSize().y - m_TopBorder - m_BottomBorder));
            m_Scroll->setSize(m_Scroll->getSize().x, m_Panel->getSize().y - m_TopBorder - m_BottomBorder);
        }

        // Find out how much the height has changed
        float heightDiff = m_Panel->getSize().y - oldHeight;

        // Reposition all labels in the chatbox
        std::vector<OBJECT*>& labels = m_Panel->getObjects();
        for (std::vector<OBJECT*>::iterator it = labels.begin(); it != labels.end(); ++it)
            (*it)->setPosition((*it)->getPosition().x, (*it)->getPosition().y + heightDiff);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Vector2u ChatBox::getSize() const
    {
        return m_Panel->getSize();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Vector2f ChatBox::getScaledSize() const
    {
        return Vector2f(m_Panel->getSize().x * getScale().x, m_Panel->getSize().y * getScale().y);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::string ChatBox::getLoadedScrollbarPathname() const
    {
        return m_LoadedScrollbarPathname;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChatBox::addLine(const sf::String& text, const sf::Color& color)
    {
        tgui::Label* label = m_Panel->add<tgui::Label>();
        label->setText(text);
        label->setTextColor(color);
        label->setTextSize(m_TextSize);
        label->setPosition(m_LeftBorder + 2, m_Panel->getSize().y - (m_TextSize * 1.2f));

        float width;
        if (m_Scroll == NULL)
            width = m_Panel->getSize().x - m_LeftBorder - m_RightBorder;
        else
            width = m_Panel->getSize().x - m_LeftBorder - m_RightBorder - m_Scroll->getSize().x;

        if (width < 0)
            width = 0;

        // Split the label over multiple lines if necessary
        while (label->getSize().x > width)
        {
            tgui::Label* newLabel = m_Panel->copy(label);
            newLabel->setText("");

            while (label->getSize().x > width)
            {
                sf::String labelText = label->getText();
                newLabel->setText(labelText[labelText.getSize()-1] + newLabel->getText());

                labelText.erase(labelText.getSize()-1, 1);
                label->setText(labelText);
            }

            label = newLabel;
        }

        if (m_Scroll != NULL)
        {
            m_Scroll->setMaximum(static_cast<unsigned int>(m_Panel->getObjects().size() * m_TextSize * 1.4f));

            if (m_Scroll->getMaximum() > m_Scroll->getLowValue())
                m_Scroll->setValue(m_Scroll->getMaximum() - m_Scroll->getLowValue());
        }

        // Reposition the labels
        updateDisplayedText();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChatBox::setTextFont(const sf::Font& font)
    {
        m_Panel->globalFont = font;

        std::vector<OBJECT*>& labels = m_Panel->getObjects();
        for (std::vector<OBJECT*>::iterator it = labels.begin(); it != labels.end(); ++it)
            static_cast<tgui::Label*>(*it)->setTextFont(font);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const sf::Font* ChatBox::getTextFont() const
    {
        return &m_Panel->globalFont;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChatBox::setTextSize(unsigned int size)
    {
        // Store the new text size
        m_TextSize = size;

        // There is a minimum text size
        if (m_TextSize < 8)
            m_TextSize = 8;

        // Update the position of the labels
        updateDisplayedText();

        // The height might need to be changed
        m_Panel->setSize(m_Panel->getSize().x, calculateValidHeight(m_Panel->getSize().y));

        // If there is a scrollbar then reinitialize it
        if (m_Scroll != NULL)
        {
            m_Scroll->setLowValue(static_cast<unsigned int>(m_Panel->getSize().y) - m_TopBorder - m_BottomBorder);
            m_Scroll->setSize(m_Scroll->getSize().x, m_Panel->getSize().y - m_TopBorder - m_BottomBorder);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int ChatBox::getTextSize() const
    {
        return m_TextSize;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChatBox::setBorders(unsigned int leftBorder, unsigned int topBorder, unsigned int rightBorder, unsigned int bottomBorder)
    {
        // Set the new border size
        m_LeftBorder   = leftBorder;
        m_TopBorder    = topBorder;
        m_RightBorder  = rightBorder;
        m_BottomBorder = bottomBorder;

        // There is a minimum width
        float width = m_Panel->getSize().x;
        if (width < (50.f + m_LeftBorder + m_RightBorder))
            width = 50.f + m_LeftBorder + m_RightBorder;

        // Make sure that the panel has a valid size
        m_Panel->setSize(width, calculateValidHeight(m_Panel->getSize().y));

        // Reposition the labels
        std::vector<OBJECT*>& labels = m_Panel->getObjects();
        for (std::vector<OBJECT*>::iterator it = labels.begin(); it != labels.end(); ++it)
            (*it)->setPosition(m_LeftBorder + 2, (*it)->getPosition().y);

        // If there is a scrollbar then reinitialize it
        if (m_Scroll != NULL)
        {
            m_Scroll->setLowValue(static_cast<unsigned int>(m_Panel->getSize().y) - m_TopBorder - m_BottomBorder);
            m_Scroll->setSize(m_Scroll->getSize().x, m_Panel->getSize().y - m_TopBorder - m_BottomBorder);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChatBox::setBackgroundColor(const sf::Color& backgroundColor)
    {
        m_Panel->backgroundColor = backgroundColor;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChatBox::setBorderColor(const sf::Color& borderColor)
    {
        m_BorderColor = borderColor;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const sf::Color& ChatBox::getBackgroundColor() const
    {
        return m_Panel->backgroundColor;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const sf::Color& ChatBox::getBorderColor() const
    {
        return m_BorderColor;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ChatBox::setScrollbar(const std::string& scrollbarPathname)
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
            // Initialize the scrollbar
            m_Scroll->setVerticalScroll(true);
            m_Scroll->setSize(m_Scroll->getSize().x, m_Panel->getSize().y - m_TopBorder - m_BottomBorder);
            m_Scroll->setLowValue(static_cast<unsigned int>(m_Panel->getSize().y) - m_TopBorder - m_BottomBorder);
            m_Scroll->setMaximum(static_cast<unsigned int>(m_Panel->getObjects().size() * m_TextSize * 1.4f));

            return true;
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChatBox::removeScrollbar()
    {
        // Delete the scrollbar
        delete m_Scroll;
        m_Scroll = NULL;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ChatBox::mouseOnObject(float x, float y)
    {
        // Get the current position and scale
        Vector2f position = getPosition();
        Vector2f curScale = getScale();

        // Pass the event to the scrollbar (if there is one)
        if (m_Scroll != NULL)
        {
            // Temporarily set the position and scale of the scroll
            m_Scroll->setScale(curScale);
            m_Scroll->setPosition(position.x + ((m_Panel->getSize().x - m_RightBorder - m_Scroll->getSize().x) * curScale.x), position.y + (m_TopBorder * curScale.y));

            // Pass the event
            m_Scroll->mouseOnObject(x, y);

            // Reset the position and scale
            m_Scroll->setPosition(0, 0);
            m_Scroll->setScale(1, 1);
        }

        // Check if the mouse is on top of the list box
        if (getTransform().transformRect(sf::FloatRect(static_cast<float>(m_LeftBorder),
                                                       static_cast<float>(m_TopBorder),
                                                       m_Panel->getSize().x - m_LeftBorder - m_RightBorder,
                                                       m_Panel->getSize().y - m_TopBorder - m_BottomBorder)).contains(x, y))
            return true;
        else // The mouse is not on top of the list box
        {
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
        if (m_Scroll != NULL)
        {
            // Remember the old scrollbar value
            unsigned int oldValue = m_Scroll->getValue();

            // Get the current scale
            Vector2f curScale = getScale();

            // Temporarily set the position and scale of the scroll
            m_Scroll->setScale(curScale);
            m_Scroll->setPosition(getPosition().x + ((m_Panel->getSize().x - m_RightBorder - m_Scroll->getSize().x) * curScale.x), getPosition().y + (m_TopBorder * curScale.y));

            // Pass the event
            if (m_Scroll->mouseOnObject(x, y))
                m_Scroll->leftMousePressed(x, y);

            // Reset the position and scale
            m_Scroll->setPosition(0, 0);
            m_Scroll->setScale(1, 1);

            // If the value of the scrollbar has changed then update the text
            if (oldValue != m_Scroll->getValue())
                updateDisplayedText();
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChatBox::leftMouseReleased(float x, float y)
    {
        // If there is a scrollbar then pass it the event
        if (m_Scroll != NULL)
        {
            // Only pass the event when the scrollbar still thinks the mouse is down
            if (m_Scroll->m_MouseDown == true)
            {
                // Remember the old scrollbar value
                unsigned int oldValue = m_Scroll->getValue();

                // Get the current scale
                Vector2f curScale = getScale();

                // Temporarily set the position and scale of the scroll
                m_Scroll->setScale(curScale);
                m_Scroll->setPosition(getPosition().x + ((m_Panel->getSize().x - m_RightBorder - m_Scroll->getSize().x) * curScale.x), getPosition().y + (m_TopBorder * curScale.y));

                // Pass the event
                m_Scroll->leftMouseReleased(x, y);

                // Reset the position and scale
                m_Scroll->setPosition(0, 0);
                m_Scroll->setScale(1, 1);

                // If the value of the scrollbar has changed then update the text
                if (oldValue != m_Scroll->getValue())
                {
                    // Check if the scrollbar value was incremented (you have pressed on the down arrow)
                    if (m_Scroll->getValue() == oldValue + 1)
                    {
                        // Decrement the value
                        m_Scroll->setValue(m_Scroll->getValue()-1);

                        // Scroll down with the whole item height instead of with a single pixel
                        m_Scroll->setValue(m_Scroll->getValue() + static_cast<unsigned int>(m_TextSize * 1.4f + 0.5) - (m_Scroll->getValue() % static_cast<unsigned int>(m_TextSize * 1.4f + 0.5f)));
                    }
                    else if (m_Scroll->getValue() == oldValue - 1) // Check if the scrollbar value was decremented (you have pressed on the up arrow)
                    {
                        // increment the value
                        m_Scroll->setValue(m_Scroll->getValue()+1);

                        // Scroll up with the whole item height instead of with a single pixel
                        if (m_Scroll->getValue() % static_cast<unsigned int>(m_TextSize * 1.4f + 0.5f) > 0)
                            m_Scroll->setValue(m_Scroll->getValue() - (m_Scroll->getValue() % static_cast<unsigned int>(m_TextSize * 1.4f + 0.5f)));
                        else
                            m_Scroll->setValue(m_Scroll->getValue() - static_cast<unsigned int>(m_TextSize * 1.4f + 0.5f));
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
        // Set the mouse move flag
        m_MouseHover = true;

        // Get the current scale
        Vector2f curScale = getScale();

        // If there is a scrollbar then pass the event
        if (m_Scroll != NULL)
        {
            // Temporarily set the position and scale of the scroll
            m_Scroll->setScale(curScale);
            m_Scroll->setPosition(getPosition().x + ((m_Panel->getSize().x - m_RightBorder - m_Scroll->getSize().x) * curScale.x), getPosition().y + (m_TopBorder * curScale.y));

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
                if (m_Scroll->mouseOnObject(x, y))
                    m_Scroll->mouseMoved(x, y);
            }

            // Reset the position and scale
            m_Scroll->setPosition(0, 0);
            m_Scroll->setScale(1, 1);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChatBox::mouseNotOnObject()
    {
        m_MouseHover = false;

        if (m_Scroll != NULL)
            m_Scroll->m_MouseHover = false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChatBox::mouseNoLongerDown()
    {
        m_MouseDown = false;

        if (m_Scroll != NULL)
            m_Scroll->m_MouseDown = false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChatBox::mouseWheelMoved(int delta)
    {
        // Only do something when there is a scrollbar
        if (m_Scroll != NULL)
        {
            if (m_Scroll->getLowValue() < m_Scroll->getMaximum())
            {
                // Check if you are scrolling down
                if (delta < 0)
                {
                    // Scroll down
                    m_Scroll->setValue( m_Scroll->getValue() + (static_cast<unsigned int>(-delta) * (static_cast<unsigned int>(m_TextSize * 1.4f + 0.5) / 2)) );
                }
                else // You are scrolling up
                {
                    unsigned int change = static_cast<unsigned int>(delta) * (static_cast<unsigned int>(m_TextSize * 1.4f + 0.5) / 2);

                    // Scroll up
                    if (change < m_Scroll->getValue())
                        m_Scroll->setValue( m_Scroll->getValue() - change );
                    else
                        m_Scroll->setValue(0);
                }

                updateDisplayedText();
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    float ChatBox::calculateValidHeight(float height)
    {
        // There is also a minimum height. At least one label should fit inside the chat box
        if (height < ((m_TextSize * 1.4f) + m_TopBorder + m_BottomBorder))
        {
            height = (m_TextSize * 1.4f) + m_TopBorder + m_BottomBorder;
        }
        else // The height isn't too low
        {
            // Calculate two perfect heights
            float height1 = static_cast<unsigned int>((height - m_TopBorder - m_BottomBorder) / ((m_TextSize * 1.4f))) * (m_TextSize * 1.4f);
            float height2 = static_cast<unsigned int>((height - m_TopBorder - m_BottomBorder) / ((m_TextSize * 1.4f)) + 1) * (m_TextSize * 1.4f);

            // Calculate the difference with the original one
            float difference1, difference2;

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

        return height;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChatBox::updateDisplayedText()
    {
        float bottomPosition = m_Panel->getSize().y;
        std::vector<OBJECT*>& labels = m_Panel->getObjects();
        for (unsigned int i = labels.size(); i > 0; --i)
        {
            unsigned int index = i - 1;

            static_cast<tgui::Label*>(labels[index])->setTextSize(m_TextSize);

            if (m_Scroll != NULL)
                labels[index]->setPosition(m_LeftBorder + 2, bottomPosition - (m_TextSize * 1.4f) + m_Scroll->getMaximum() - m_Scroll->getLowValue() - m_Scroll->getValue());
            else
                labels[index]->setPosition(m_LeftBorder + 2, bottomPosition - (m_TextSize * 1.4f));

            bottomPosition -= m_TextSize * 1.4f;
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChatBox::draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        // Adjust the transformation
        states.transform *= getTransform();

        // Draw the panel
        target.draw(*m_Panel, states);

        // Draw left border
        sf::RectangleShape border(Vector2f(m_LeftBorder, m_Panel->getSize().y));
        border.setFillColor(m_BorderColor);
        target.draw(border, states);

        // Draw top border
        border.setSize(Vector2f(m_Panel->getSize().x, m_TopBorder));
        target.draw(border, states);

        // Draw right border
        border.setPosition(m_Panel->getSize().x - m_RightBorder, 0);
        border.setSize(Vector2f(m_RightBorder, m_Panel->getSize().y));
        target.draw(border, states);

        // Draw bottom border
        border.setPosition(0, m_Panel->getSize().y - m_BottomBorder);
        border.setSize(Vector2f(m_Panel->getSize().x, m_BottomBorder));
        target.draw(border, states);

        // Check if there is a scrollbar
        if (m_Scroll != NULL)
        {
            // Reset the transformation
            states.transform.translate(m_Panel->getSize().x - m_RightBorder - m_Scroll->getSize().x, m_TopBorder);

            // Draw the scrollbar
            target.draw(*m_Scroll, states);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
