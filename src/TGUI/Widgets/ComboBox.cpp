/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus' Graphical User Interface
// Copyright (C) 2012-2016 Bruno Van de Velde (vdv_b@tgui.eu)
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
#include <TGUI/Widgets/ComboBox.hpp>
#include <TGUI/Clipping.hpp>
#include <SFML/Graphics/ConvexShape.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ComboBox::ComboBox()
    {
        m_callback.widgetType = "ComboBox";
        m_type = "ComboBox";

        m_draggableWidget = true;

        addSignal<sf::String, TypeSet<sf::String, sf::String>>("ItemSelected");

        initListBox();

        m_renderer = aurora::makeCopied<ComboBoxRenderer>();
        setRenderer(m_renderer->getData());

        getRenderer()->setBorders({2});
        getRenderer()->setPadding({2, 0, 0, 0});
        getRenderer()->setBackgroundColor({245, 245, 245});
        getRenderer()->setArrowBackgroundColor({245, 245, 245});
        getRenderer()->setArrowBackgroundColorHover(sf::Color::White);
        getRenderer()->setArrowColor({60, 60, 60});
        getRenderer()->setArrowColorHover(sf::Color::Black);

        setSize({150, 24});
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ComboBox::ComboBox(const ComboBox& other) :
        Widget              {other},
        m_nrOfItemsToDisplay{other.m_nrOfItemsToDisplay},
        m_listBox           {ListBox::copy(other.m_listBox)},
        m_text              {other.m_text}
    {
        if (m_listBox != nullptr)
        {
            m_listBox->disconnectAll();
            initListBox();
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ComboBox::ComboBox(ComboBox&& other) :
        Widget              {std::move(other)},
        m_nrOfItemsToDisplay{std::move(other.m_nrOfItemsToDisplay)},
        m_listBox           {std::move(other.m_listBox)},
        m_text              {std::move(other.m_text)}
    {
        if (m_listBox != nullptr)
        {
            m_listBox->disconnectAll();
            initListBox();
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ComboBox& ComboBox::operator= (const ComboBox& other)
    {
        if (this != &other)
        {
            ComboBox temp{other};
            Widget::operator=(other);

            std::swap(m_nrOfItemsToDisplay, temp.m_nrOfItemsToDisplay);
            std::swap(m_listBox,            temp.m_listBox);
            std::swap(m_text,               temp.m_text);
        }

        return *this;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ComboBox& ComboBox::operator= (ComboBox&& other)
    {
        if (this != &other)
        {
            Widget::operator=(std::move(other));
            std::swap(m_nrOfItemsToDisplay, other.m_nrOfItemsToDisplay);
            std::swap(m_listBox,            other.m_listBox);
            std::swap(m_text,               other.m_text);
        }

        return *this;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ComboBox::Ptr ComboBox::create()
    {
        return std::make_shared<ComboBox>();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ComboBox::Ptr ComboBox::copy(ComboBox::ConstPtr comboBox)
    {
        if (comboBox)
            return std::static_pointer_cast<ComboBox>(comboBox->clone());
        else
            return nullptr;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ComboBox::setSize(const Layout2d& size)
    {
        Widget::setSize(size);

        getRenderer()->getTextureBackground().setSize(getInnerSize());

        Padding padding = getRenderer()->getPadding();
        float height = getInnerSize().y - padding.top - padding.bottom;

        if (height > 0)
        {
            m_listBox->setItemHeight(static_cast<unsigned int>(height));
            updateListBoxHeight();
        }

        if (getRenderer()->getTextureArrowUp().isLoaded() && getRenderer()->getTextureArrowDown().isLoaded())
        {
            getRenderer()->getTextureArrowUp().setSize({getRenderer()->getTextureArrowUp().getImageSize().x * (height / getRenderer()->getTextureArrowUp().getImageSize().y), height});
            getRenderer()->getTextureArrowDown().setSize({getRenderer()->getTextureArrowDown().getImageSize().x * (height / getRenderer()->getTextureArrowDown().getImageSize().y), height});

            getRenderer()->getTextureArrowUpHover().setSize(getRenderer()->getTextureArrowUp().getSize());
            getRenderer()->getTextureArrowDownHover().setSize(getRenderer()->getTextureArrowDown().getSize());
        }

        m_text.setCharacterSize(m_listBox->getTextSize());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ComboBox::setItemsToDisplay(std::size_t nrOfItemsInList)
    {
        m_nrOfItemsToDisplay = nrOfItemsInList;

        if (m_nrOfItemsToDisplay < m_listBox->getItemCount())
            updateListBoxHeight();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::size_t ComboBox::getItemCount() const
    {
        return m_listBox->getItemCount();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::size_t ComboBox::getItemsToDisplay() const
    {
        return m_nrOfItemsToDisplay;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ComboBox::addItem(const sf::String& item, const sf::String& id)
    {
        // Make room to add another item, until there are enough items
        if ((m_nrOfItemsToDisplay == 0) || (m_listBox->getItemCount() < m_nrOfItemsToDisplay))
            updateListBoxHeight();

        return m_listBox->addItem(item, id);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ComboBox::setSelectedItem(const sf::String& itemName)
    {
        bool ret = m_listBox->setSelectedItem(itemName);
        m_text.setString(m_listBox->getSelectedItem());
        return ret;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ComboBox::setSelectedItemById(const sf::String& id)
    {
        bool ret = m_listBox->setSelectedItemById(id);
        m_text.setString(m_listBox->getSelectedItem());
        return ret;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ComboBox::setSelectedItemByIndex(std::size_t index)
    {
        bool ret = m_listBox->setSelectedItemByIndex(index);
        m_text.setString(m_listBox->getSelectedItem());
        return ret;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ComboBox::deselectItem()
    {
        m_text.setString("");
        m_listBox->deselectItem();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ComboBox::removeItem(const sf::String& itemName)
    {
        // Implemented like removeItemByIndex instead of like removeItemById because m_listBox->getItems() may be slow

        bool ret = m_listBox->removeItem(itemName);

        m_text.setString(m_listBox->getSelectedItem());

        // Shrink the list size
        if ((m_nrOfItemsToDisplay == 0) || (m_listBox->getItemCount() < m_nrOfItemsToDisplay))
            updateListBoxHeight();

        return ret;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ComboBox::removeItemById(const sf::String& id)
    {
        const auto& ids = m_listBox->getItemIds();
        for (std::size_t i = 0; i < ids.size(); ++i)
        {
            if (ids[i] == id)
                return removeItemByIndex(i);
        }

        return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ComboBox::removeItemByIndex(std::size_t index)
    {
        bool ret = m_listBox->removeItemByIndex(index);

        m_text.setString(m_listBox->getSelectedItem());

        // Shrink the list size
        if ((m_nrOfItemsToDisplay == 0) || (m_listBox->getItemCount() < m_nrOfItemsToDisplay))
            updateListBoxHeight();

        return ret;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ComboBox::removeAllItems()
    {
        m_text.setString("");
        m_listBox->removeAllItems();

        updateListBoxHeight();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    sf::String ComboBox::getItemById(const sf::String& id) const
    {
        return m_listBox->getItemById(id);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    sf::String ComboBox::getSelectedItem() const
    {
        return m_listBox->getSelectedItem();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    sf::String ComboBox::getSelectedItemId() const
    {
        return m_listBox->getSelectedItemId();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    int ComboBox::getSelectedItemIndex() const
    {
        return m_listBox->getSelectedItemIndex();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ComboBox::changeItem(const sf::String& originalValue, const sf::String& newValue)
    {
        bool ret = m_listBox->changeItem(originalValue, newValue);
        m_text.setString(m_listBox->getSelectedItem());
        return ret;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ComboBox::changeItemById(const sf::String& id, const sf::String& newValue)
    {
        bool ret = m_listBox->changeItemById(id, newValue);
        m_text.setString(m_listBox->getSelectedItem());
        return ret;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ComboBox::changeItemByIndex(std::size_t index, const sf::String& newValue)
    {
        bool ret = m_listBox->changeItemByIndex(index, newValue);
        m_text.setString(m_listBox->getSelectedItem());
        return ret;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::vector<sf::String> ComboBox::getItems()
    {
        return m_listBox->getItems();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const std::vector<sf::String>& ComboBox::getItemIds()
    {
        return m_listBox->getItemIds();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ComboBox::setMaximumItems(std::size_t maximumItems)
    {
        m_listBox->setMaximumItems(maximumItems);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::size_t ComboBox::getMaximumItems() const
    {
        return m_listBox->getMaximumItems();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ComboBox::setTextSize(unsigned int textSize)
    {
        m_listBox->setTextSize(textSize);
        m_text.setCharacterSize(m_listBox->getTextSize());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int ComboBox::getTextSize() const
    {
        return m_listBox->getTextSize();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ComboBox::setParent(Container* parent)
    {
        hideListBox();
        Widget::setParent(parent);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ComboBox::mouseOnWidget(sf::Vector2f pos) const
    {
        return sf::FloatRect{0, 0, getSize().x, getSize().y}.contains(pos);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ComboBox::leftMousePressed(sf::Vector2f)
    {
        m_mouseDown = true;

        // If the list wasn't visible then open it
        if (!m_listBox->isVisible())
        {
            // Show the list
            showListBox();

            // Reselect the selected item to make sure it is always among the visible items when the list opens
            if (m_listBox->getSelectedItemIndex() >= 0)
                m_listBox->setSelectedItemByIndex(m_listBox->getSelectedItemIndex());
        }
        else // This list was already open, so close it now
            hideListBox();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ComboBox::mouseWheelMoved(int delta, int, int)
    {
        // Only act to scrolling when the list is not being shown
        if (!m_listBox->isVisible())
        {
            // Check if you are scrolling down
            if (delta < 0)
            {
                // select the next item
                if (static_cast<std::size_t>(m_listBox->getSelectedItemIndex() + 1) < m_listBox->getItemCount())
                {
                    m_listBox->setSelectedItemByIndex(static_cast<std::size_t>(m_listBox->getSelectedItemIndex() + 1));
                    m_text.setString(m_listBox->getSelectedItem());
                }
            }
            else // You are scrolling up
            {
                // select the previous item
                if (m_listBox->getSelectedItemIndex() > 0)
                {
                    m_listBox->setSelectedItemByIndex(static_cast<std::size_t>(m_listBox->getSelectedItemIndex() - 1));
                    m_text.setString(m_listBox->getSelectedItem());
                }
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ComboBox::rendererChanged(const std::string& property, ObjectConverter& value)
    {
        if ((property == "borders") || (property == "padding"))
        {
            updateSize();
        }
        else if (property == "textcolor")
        {
            m_text.setColor(value.getColor());
        }
        else if (property == "textstyle")
        {
            m_text.setStyle(value.getTextStyle());
        }
        else if (property == "texturebackground")
        {
            value.getTexture().setSize(getInnerSize());
            value.getTexture().setOpacity(getRenderer()->getOpacity());
        }
        else if ((property == "texturearrowup") || (property == "texturearrowuphover") || (property == "texturearrowdown") || (property == "texturearrowdownhover"))
        {
            value.getTexture().setOpacity(getRenderer()->getOpacity());
            updateSize();
        }
        else if (property == "listbox")
        {
            m_listBox->setRenderer(value.getRenderer());
        }
        else if (property == "opacity")
        {
            float opacity = value.getNumber();

            getRenderer()->getTextureBackground().setOpacity(opacity);
            getRenderer()->getTextureArrowUp().setOpacity(opacity);
            getRenderer()->getTextureArrowUpHover().setOpacity(opacity);
            getRenderer()->getTextureArrowDown().setOpacity(opacity);
            getRenderer()->getTextureArrowDownHover().setOpacity(opacity);

            m_text.setOpacity(opacity);
        }
        else if (property == "font")
        {
            m_text.setFont(value.getFont());

            if (m_listBox->getRenderer()->getFont() == nullptr)
                m_listBox->getRenderer()->setFont(value.getFont());

            updateSize();
        }
        else if ((property != "bordercolor")
              && (property != "backgroundcolor")
              && (property != "arrowbackgroundcolor")
              && (property != "arrowbackgroundcolorhover")
              && (property != "arrowcolor")
              && (property != "arrowcolorhover"))
        {
            Widget::rendererChanged(property, value);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    sf::Vector2f ComboBox::getInnerSize() const
    {
        Borders borders = getRenderer()->getBorders();
        return {getSize().x - borders.left - borders.right, getSize().y - borders.top - borders.bottom};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ComboBox::updateListBoxHeight()
    {
        Borders borders = m_listBox->getRenderer()->getBorders();
        Padding padding = m_listBox->getRenderer()->getPadding();

        if (m_nrOfItemsToDisplay > 0)
            m_listBox->setSize({getSize().x, (m_listBox->getItemHeight() * (std::min<std::size_t>(m_nrOfItemsToDisplay, std::max<std::size_t>(m_listBox->getItemCount(), 1))))
                                             + borders.top + borders.bottom + padding.top + padding.bottom});
        else
            m_listBox->setSize({getSize().x, (m_listBox->getItemHeight() * std::max<std::size_t>(m_listBox->getItemCount(), 1))
                                             + borders.top + borders.bottom + padding.top + padding.bottom});

    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ComboBox::showListBox()
    {
        if (!m_listBox->isVisible() && getParent())
        {
            m_listBox->show();

            // Find the GuiContainer that contains the combo box
            Container* container = getParent();
            while (container->getParent() != nullptr)
                container = container->getParent();

            m_listBox->setPosition({getAbsolutePosition().x, getAbsolutePosition().y + getSize().y - getRenderer()->getBorders().bottom});
            container->add(m_listBox, "#TGUI_INTERNAL$ComboBoxListBox#");
            m_listBox->focus();
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ComboBox::hideListBox()
    {
        // If the list was open then close it now
        if (m_listBox->isVisible())
        {
            m_listBox->hide();
            m_listBox->mouseNoLongerOnWidget();

            // Find the GuiContainer in order to remove the ListBox from it
            Widget* container = this;
            while (container->getParent() != nullptr)
                container = container->getParent();

            if (container != this)
                static_cast<Container*>(container)->remove(m_listBox);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ComboBox::initListBox()
    {
        m_listBox->hide();
        m_listBox->connect("ItemSelected", &ComboBox::newItemSelectedCallbackFunction, this);
        m_listBox->connect("Unfocused", &ComboBox::listBoxUnfocusedCallbackFunction, this);
        m_listBox->connect("MouseReleased", &ComboBox::hideListBox, this);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ComboBox::newItemSelectedCallbackFunction()
    {
        m_text.setString(m_listBox->getSelectedItem());

        m_callback.text   = m_listBox->getSelectedItem();
        m_callback.itemId = m_listBox->getSelectedItemId();
        sendSignal("ItemSelected", m_listBox->getSelectedItem(), m_listBox->getSelectedItem(), m_listBox->getSelectedItemId());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ComboBox::listBoxUnfocusedCallbackFunction()
    {
        if (!m_mouseHover)
            hideListBox();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ComboBox::draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        states.transform.translate(getPosition());

        // Draw the borders
        Borders borders = getRenderer()->getBorders();
        if (borders != Borders{0})
        {
            drawBorders(target, states, borders, getSize(), getRenderer()->getBorderColor());
            states.transform.translate({borders.left, borders.top});
        }

        sf::RenderStates statesForText = states;

        // Draw the background
        if (getRenderer()->getTextureBackground().isLoaded())
            getRenderer()->getTextureBackground().draw(target, states);
        else
            drawRectangleShape(target, states, getInnerSize(), getRenderer()->getBackgroundColor());

        // Check if we have textures for the arrow
        float arrowSize;
        Padding padding = getRenderer()->getPadding();
        if (getRenderer()->getTextureArrowUp().isLoaded() && getRenderer()->getTextureArrowDown().isLoaded())
        {
            // Set the arrow like it should (down when list box is invisible, up when it is visible)
            if (m_listBox->isVisible())
            {
                arrowSize = getRenderer()->getTextureArrowUp().getSize().x;
                states.transform.translate({getInnerSize().x - padding.right - arrowSize, padding.top});

                if (m_mouseHover && getRenderer()->getTextureArrowUpHover().isLoaded())
                    getRenderer()->getTextureArrowUpHover().draw(target, states);
                else
                    getRenderer()->getTextureArrowUp().draw(target, states);
            }
            else
            {
                arrowSize = getRenderer()->getTextureArrowDown().getSize().x;
                states.transform.translate({getInnerSize().x - padding.right - arrowSize, padding.top});

                if (m_mouseHover && getRenderer()->getTextureArrowDownHover().isLoaded())
                    getRenderer()->getTextureArrowDownHover().draw(target, states);
                else
                    getRenderer()->getTextureArrowDown().draw(target, states);
            }
        }
        else // There are no textures for the arrow
        {
            arrowSize = getInnerSize().y - padding.top - padding.bottom;
            states.transform.translate({getInnerSize().x - padding.right - arrowSize, padding.top});

            if (m_mouseHover && getRenderer()->getArrowBackgroundColorHover().isSet())
                drawRectangleShape(target, states, {arrowSize, arrowSize}, getRenderer()->getArrowBackgroundColorHover());
            else
                drawRectangleShape(target, states, {arrowSize, arrowSize}, getRenderer()->getArrowBackgroundColor());

            sf::ConvexShape arrow{3};
            if (m_listBox->isVisible())
            {
                arrow.setPoint(0, {arrowSize / 5, arrowSize * 4/5});
                arrow.setPoint(1, {arrowSize / 2, arrowSize / 5});
                arrow.setPoint(2, {arrowSize * 4/5, arrowSize * 4/5});
            }
            else // The listbox is not visible, the arrow points down
            {
                arrow.setPoint(0, {arrowSize / 5, arrowSize / 5});
                arrow.setPoint(1, {arrowSize / 2, arrowSize * 4/5});
                arrow.setPoint(2, {arrowSize * 4/5, arrowSize / 5});
            }

            if (m_mouseHover && getRenderer()->getArrowColorHover().isSet())
                arrow.setFillColor(getRenderer()->getArrowColorHover());
            else
                arrow.setFillColor(getRenderer()->getArrowColor());

            target.draw(arrow, states);
        }

        // Draw the selected item
        if (!m_text.getString().isEmpty())
        {
            Clipping clipping{target, statesForText, {padding.left, padding.top}, {getInnerSize().x - padding.left - padding.right - arrowSize, getInnerSize().y - padding.top - padding.bottom}};

            statesForText.transform.translate(padding.left, padding.top + (((getInnerSize().y - padding.top - padding.bottom) - m_text.getSize().y) / 2.0f));
            m_text.draw(target, statesForText);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
