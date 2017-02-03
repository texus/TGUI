/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus's Graphical User Interface
// Copyright (C) 2012-2017 Bruno Van de Velde (vdv_b@tgui.eu)
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
#include <TGUI/Loading/Theme.hpp>
#include <TGUI/Widgets/ComboBox.hpp>
#include <TGUI/Clipping.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ComboBox::ComboBox()
    {
        m_callback.widgetType = "ComboBox";
        m_draggableWidget = true;

        addSignal<sf::String, TypeSet<sf::String, sf::String>>("ItemSelected");

        initListBox();

        m_renderer = std::make_shared<ComboBoxRenderer>(this);
        reload();

        setSize({150, 24});
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ComboBox::ComboBox(const ComboBox& listBoxToCopy) :
        Widget              {listBoxToCopy},
        m_nrOfItemsToDisplay{listBoxToCopy.m_nrOfItemsToDisplay},
        m_listBox           {ListBox::copy(listBoxToCopy.m_listBox)},
        m_text              {listBoxToCopy.m_text}
    {
        if (m_listBox != nullptr)
        {
            m_listBox->disconnectAll();
            initListBox();
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ComboBox& ComboBox::operator= (const ComboBox& right)
    {
        if (this != &right)
        {
            ComboBox temp{right};
            Widget::operator=(right);

            std::swap(m_nrOfItemsToDisplay, temp.m_nrOfItemsToDisplay);
            std::swap(m_listBox,            temp.m_listBox);
            std::swap(m_text,               temp.m_text);
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

    void ComboBox::setPosition(const Layout2d& position)
    {
        Widget::setPosition(position);

        Padding padding = getRenderer()->getScaledPadding();

        getRenderer()->m_backgroundTexture.setPosition(getPosition());

        getRenderer()->m_textureArrowUpNormal.setPosition({getPosition().x + getSize().x - getRenderer()->m_textureArrowUpNormal.getSize().x - padding.right, getPosition().y + padding.top});
        getRenderer()->m_textureArrowDownNormal.setPosition({getPosition().x + getSize().x - getRenderer()->m_textureArrowDownNormal.getSize().x - padding.right, getPosition().y + padding.top});

        getRenderer()->m_textureArrowUpHover.setPosition(getRenderer()->m_textureArrowUpNormal.getPosition());
        getRenderer()->m_textureArrowDownHover.setPosition(getRenderer()->m_textureArrowUpNormal.getPosition());

        m_text.setPosition(getPosition().x + padding.left,
                           getPosition().y + padding.top + (((getSize().y - padding.top - padding.bottom) - m_text.getSize().y) / 2.0f));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ComboBox::setSize(const Layout2d& size)
    {
        Widget::setSize(size);

        getRenderer()->m_backgroundTexture.setSize(getSize());

        Padding listBoxPadding = m_listBox->getRenderer()->getScaledPadding();
        Padding padding = getRenderer()->getScaledPadding();
        float height = getSize().y - padding.top - padding.bottom;

        m_listBox->setItemHeight(static_cast<unsigned int>(height));

        if (m_nrOfItemsToDisplay > 0)
            m_listBox->setSize({getSize().x, (m_listBox->getItemHeight() * (std::min<std::size_t>(m_nrOfItemsToDisplay, std::max<std::size_t>(m_listBox->getItemCount(), 1)))) + listBoxPadding.top + listBoxPadding.bottom});
        else
            m_listBox->setSize({getSize().x, (m_listBox->getItemHeight() * std::max<std::size_t>(m_listBox->getItemCount(), 1)) + listBoxPadding.top + listBoxPadding.bottom});

        if (getRenderer()->m_textureArrowUpNormal.isLoaded() && getRenderer()->m_textureArrowDownNormal.isLoaded())
        {
            getRenderer()->m_textureArrowUpNormal.setSize({getRenderer()->m_textureArrowUpNormal.getImageSize().x * (height / getRenderer()->m_textureArrowUpNormal.getImageSize().y), height});
            getRenderer()->m_textureArrowDownNormal.setSize({getRenderer()->m_textureArrowDownNormal.getImageSize().x * (height / getRenderer()->m_textureArrowDownNormal.getImageSize().y), height});

            getRenderer()->m_textureArrowUpHover.setSize(getRenderer()->m_textureArrowUpNormal.getSize());
            getRenderer()->m_textureArrowDownHover.setSize(getRenderer()->m_textureArrowUpNormal.getSize());
        }

        m_text.setTextSize(m_listBox->getTextSize());

        updatePosition();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    sf::Vector2f ComboBox::getFullSize() const
    {
        return {getSize().x + getRenderer()->getBorders().left + getRenderer()->getBorders().right,
                getSize().y + getRenderer()->getBorders().top + getRenderer()->getBorders().bottom};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ComboBox::setFont(const Font& font)
    {
        Widget::setFont(font);

        m_text.setFont(font);
        m_listBox->setFont(font);
        m_text.setTextSize(m_listBox->getTextSize());

        updatePosition();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ComboBox::setItemsToDisplay(std::size_t nrOfItemsInList)
    {
        m_nrOfItemsToDisplay = nrOfItemsInList;

        Padding padding = m_listBox->getRenderer()->getScaledPadding();
        if (m_nrOfItemsToDisplay < m_listBox->m_items.size())
            m_listBox->setSize({m_listBox->getSize().x, (m_nrOfItemsToDisplay * m_listBox->getItemHeight()) + padding.top + padding.bottom});
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ComboBox::addItem(const sf::String& item, const sf::String& id)
    {
        // Make room to add another item, until there are enough items
        if ((m_nrOfItemsToDisplay == 0) || (m_listBox->getItemCount() < m_nrOfItemsToDisplay))
        {
            Padding padding = m_listBox->getRenderer()->getScaledPadding();
            m_listBox->setSize({m_listBox->getSize().x, (m_listBox->getItemHeight() * (m_listBox->getItemCount() + 1)) + padding.top + padding.bottom});
        }

        // Add the item
        return m_listBox->addItem(item, id);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ComboBox::setSelectedItem(const sf::String& itemName)
    {
        bool ret = m_listBox->setSelectedItem(itemName);
        m_text.setText(m_listBox->getSelectedItem());
        return ret;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ComboBox::setSelectedItemById(const sf::String& id)
    {
        bool ret = m_listBox->setSelectedItemById(id);
        m_text.setText(m_listBox->getSelectedItem());
        return ret;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ComboBox::setSelectedItemByIndex(std::size_t index)
    {
        bool ret = m_listBox->setSelectedItemByIndex(index);
        m_text.setText(m_listBox->getSelectedItem());
        return ret;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ComboBox::deselectItem()
    {
        m_text.setText("");
        m_listBox->deselectItem();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ComboBox::removeItem(const sf::String& itemName)
    {
        bool ret = m_listBox->removeItem(itemName);

        m_text.setText(m_listBox->getSelectedItem());

        // Shrink the list size
        if ((m_nrOfItemsToDisplay == 0) || (m_listBox->getItemCount() < m_nrOfItemsToDisplay))
        {
            Padding padding = m_listBox->getRenderer()->getScaledPadding();
            m_listBox->setSize({m_listBox->getSize().x, (m_listBox->getItemHeight() * std::max<std::size_t>(m_listBox->getItemCount(), 1)) + padding.top + padding.bottom});
        }

        return ret;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ComboBox::removeItemById(const sf::String& id)
    {
        bool ret = m_listBox->removeItemById(id);

        m_text.setText(m_listBox->getSelectedItem());

        // Shrink the list size
        if ((m_nrOfItemsToDisplay == 0) || (m_listBox->getItemCount() < m_nrOfItemsToDisplay))
        {
            Padding padding = m_listBox->getRenderer()->getScaledPadding();
            m_listBox->setSize({m_listBox->getSize().x, (m_listBox->getItemHeight() * std::max<std::size_t>(m_listBox->getItemCount(), 1)) + padding.top + padding.bottom});
        }

        return ret;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ComboBox::removeItemByIndex(std::size_t index)
    {
        bool ret = m_listBox->removeItemByIndex(index);

        m_text.setText(m_listBox->getSelectedItem());

        // Shrink the list size
        if ((m_nrOfItemsToDisplay == 0) || (m_listBox->getItemCount() < m_nrOfItemsToDisplay))
        {
            Padding padding = m_listBox->getRenderer()->getScaledPadding();
            m_listBox->setSize({m_listBox->getSize().x, (m_listBox->getItemHeight() * std::max<std::size_t>(m_listBox->getItemCount(), 1)) + padding.top + padding.bottom});
        }

        return ret;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ComboBox::removeAllItems()
    {
        m_text.setText("");
        m_listBox->removeAllItems();

        // Shrink the list size
        Padding padding = m_listBox->getRenderer()->getScaledPadding();
        m_listBox->setSize({m_listBox->getSize().x, m_listBox->getItemHeight() + padding.top + padding.bottom});
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ComboBox::changeItem(const sf::String& originalValue, const sf::String& newValue)
    {
        bool ret = m_listBox->changeItem(originalValue, newValue);
        m_text.setText(m_listBox->getSelectedItem());
        return ret;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ComboBox::changeItemById(const sf::String& id, const sf::String& newValue)
    {
        bool ret = m_listBox->changeItemById(id, newValue);
        m_text.setText(m_listBox->getSelectedItem());
        return ret;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ComboBox::changeItemByIndex(std::size_t index, const sf::String& newValue)
    {
        bool ret = m_listBox->changeItemByIndex(index, newValue);
        m_text.setText(m_listBox->getSelectedItem());
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

    void ComboBox::setScrollbar(Scrollbar::Ptr scrollbar)
    {
        m_listBox->setScrollbar(scrollbar);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Scrollbar::Ptr ComboBox::getScrollbar() const
    {
        return m_listBox->getScrollbar();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ComboBox::setListBox(ListBox::Ptr listBox)
    {
        m_listBox = listBox;

        Padding listBoxPadding = m_listBox->getRenderer()->getScaledPadding();
        Padding padding = getRenderer()->getScaledPadding();
        float height = getSize().y - padding.top - padding.bottom;

        m_listBox->setItemHeight(static_cast<unsigned int>(height));

        if (m_nrOfItemsToDisplay > 0)
            m_listBox->setSize({getSize().x, (m_listBox->getItemHeight() * (std::min<std::size_t>(m_nrOfItemsToDisplay, std::max<std::size_t>(m_listBox->getItemCount(), 1)))) + listBoxPadding.top + listBoxPadding.bottom});
        else
            m_listBox->setSize({getSize().x, (m_listBox->getItemHeight() * std::max<std::size_t>(m_listBox->getItemCount(), 1)) + listBoxPadding.top + listBoxPadding.bottom});

        if (m_listBox->getSelectedItemIndex() >= 0)
            m_listBox->setSelectedItemByIndex(static_cast<std::size_t>(m_listBox->getSelectedItemIndex()));
        else
            m_listBox->deselectItem();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ListBox::Ptr ComboBox::getListBox() const
    {
        return m_listBox;
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
        m_text.setTextSize(m_listBox->getTextSize());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int ComboBox::getTextSize() const
    {
        return m_listBox->getTextSize();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ComboBox::setOpacity(float opacity)
    {
        Widget::setOpacity(opacity);

        m_listBox->setOpacity(m_opacity);

        getRenderer()->m_backgroundTexture.setColor({getRenderer()->m_backgroundTexture.getColor().r, getRenderer()->m_backgroundTexture.getColor().g, getRenderer()->m_backgroundTexture.getColor().b, static_cast<sf::Uint8>(m_opacity * 255)});
        getRenderer()->m_textureArrowUpNormal.setColor({getRenderer()->m_textureArrowUpNormal.getColor().r, getRenderer()->m_textureArrowUpNormal.getColor().g, getRenderer()->m_textureArrowUpNormal.getColor().b, static_cast<sf::Uint8>(m_opacity * 255)});
        getRenderer()->m_textureArrowDownNormal.setColor({getRenderer()->m_textureArrowDownNormal.getColor().r, getRenderer()->m_textureArrowDownNormal.getColor().g, getRenderer()->m_textureArrowDownNormal.getColor().b, static_cast<sf::Uint8>(m_opacity * 255)});
        getRenderer()->m_textureArrowUpHover.setColor({getRenderer()->m_textureArrowUpHover.getColor().r, getRenderer()->m_textureArrowUpHover.getColor().g, getRenderer()->m_textureArrowUpHover.getColor().b, static_cast<sf::Uint8>(m_opacity * 255)});
        getRenderer()->m_textureArrowDownHover.setColor({getRenderer()->m_textureArrowDownHover.getColor().r, getRenderer()->m_textureArrowDownHover.getColor().g, getRenderer()->m_textureArrowDownHover.getColor().b, static_cast<sf::Uint8>(m_opacity * 255)});

        m_text.setTextColor(calcColorOpacity(getRenderer()->m_textColor, getOpacity()));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    sf::Vector2f ComboBox::getWidgetOffset() const
    {
        return {getRenderer()->getBorders().left, getRenderer()->getBorders().top};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ComboBox::setParent(Container* parent)
    {
        hideListBox();
        Widget::setParent(parent);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ComboBox::mouseOnWidget(float x, float y) const
    {
        return sf::FloatRect{getPosition().x - getRenderer()->getBorders().left,
                             getPosition().y - getRenderer()->getBorders().top,
                             getSize().x + getRenderer()->getBorders().left + getRenderer()->getBorders().right,
                             getSize().y + getRenderer()->getBorders().top + getRenderer()->getBorders().bottom}.contains(x, y);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ComboBox::leftMousePressed(float, float)
    {
        m_mouseDown = true;

        // If the list wasn't visible then open it
        if (!m_listBox->isVisible())
        {
            // Show the list
            showListBox();

            // Check if there is a scrollbar
            if (m_listBox->m_scroll != nullptr)
            {
                // If the selected item is not visible then change the value of the scrollbar
                if (m_nrOfItemsToDisplay > 0)
                {
                    if (static_cast<std::size_t>(m_listBox->m_selectedItem + 1) > m_nrOfItemsToDisplay)
                        m_listBox->m_scroll->setValue(static_cast<unsigned int>((m_listBox->m_selectedItem + 1 - m_nrOfItemsToDisplay) * m_listBox->getItemHeight()));
                    else
                        m_listBox->m_scroll->setValue(0);

                    m_listBox->updatePosition();
                }
            }
        }
        else // This list was already open, so close it now
            hideListBox();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ComboBox::mouseWheelMoved(int delta, int, int)
    {
        // The list isn't visible
        if (!m_listBox->isVisible())
        {
            // Check if you are scrolling down
            if (delta < 0)
            {
                // select the next item
                if (static_cast<std::size_t>(m_listBox->m_selectedItem + 1) < m_listBox->m_items.size())
                    m_listBox->setSelectedItemByIndex(static_cast<std::size_t>(m_listBox->m_selectedItem+1));
            }
            else // You are scrolling up
            {
                // select the previous item
                if (m_listBox->m_selectedItem > 0)
                    m_listBox->setSelectedItemByIndex(static_cast<std::size_t>(m_listBox->m_selectedItem-1));
            }

            m_text.setText(m_listBox->getSelectedItem());
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ComboBox::reload(const std::string& primary, const std::string& secondary, bool force)
    {
        m_listBox->reload();
        getRenderer()->setBorders({2, 2, 2, 2});
        getRenderer()->setBackgroundColor({245, 245, 245});
        getRenderer()->setArrowBackgroundColorNormal({245, 245, 245});
        getRenderer()->setArrowBackgroundColorHover({255, 255, 255});
        getRenderer()->setArrowColorNormal({60, 60, 60});
        getRenderer()->setArrowColorHover({0, 0, 0});
        getRenderer()->setTextColor({0, 0, 0});
        getRenderer()->setBorderColor({0, 0, 0});
        getRenderer()->setBackgroundTexture({});
        getRenderer()->setArrowUpTexture({});
        getRenderer()->setArrowDownTexture({});
        getRenderer()->setArrowUpHoverTexture({});
        getRenderer()->setArrowDownHoverTexture({});

        if (m_theme && primary != "")
        {
            getRenderer()->setBorders({0, 0, 0, 0});
            Widget::reload(primary, secondary, force);

            if (force)
            {
                if (getRenderer()->m_backgroundTexture.isLoaded())
                    setSize(5 * getRenderer()->m_backgroundTexture.getImageSize().x, getRenderer()->m_backgroundTexture.getImageSize().y);
            }

            updateSize();
        }
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

            m_listBox->setPosition({getAbsolutePosition().x, getAbsolutePosition().y + getSize().y + getRenderer()->getBorders().top});
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

            // Find the gui in order to remove the ListBox from it
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
        m_text.setText(m_listBox->getSelectedItem());

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
        // Draw the background
        getRenderer()->draw(target, states);

        Padding padding = getRenderer()->getScaledPadding();

        float arrowWidth;
        if (getRenderer()->m_textureArrowUpNormal.isLoaded() && getRenderer()->m_textureArrowDownNormal.isLoaded())
            arrowWidth = getRenderer()->m_textureArrowDownNormal.getSize().x * (getSize().y / getRenderer()->m_textureArrowDownNormal.getSize().y);
        else
            arrowWidth = getSize().y - padding.top - padding.bottom;

        // Set the clipping for all draw calls that happen until this clipping object goes out of scope
        Clipping clipping{target, states, {getPosition().x + padding.left, getPosition().y + padding.top}, {getSize().x - padding.left - padding.right - arrowWidth, getSize().y - padding.top - padding.bottom}};

        // Draw the selected item
        target.draw(m_text, states);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ComboBoxRenderer::setProperty(std::string property, const std::string& value)
    {
        property = toLower(property);

        if (property == "borders")
            setBorders(Deserializer::deserialize(ObjectConverter::Type::Borders, value).getBorders());
        else if (property == "padding")
            setPadding(Deserializer::deserialize(ObjectConverter::Type::Borders, value).getBorders());
        else if (property == "backgroundcolor")
            setBackgroundColor(Deserializer::deserialize(ObjectConverter::Type::Color, value).getColor());
        else if (property == "arrowbackgroundcolor")
            setArrowBackgroundColor(Deserializer::deserialize(ObjectConverter::Type::Color, value).getColor());
        else if (property == "arrowbackgroundcolornormal")
            setArrowBackgroundColorNormal(Deserializer::deserialize(ObjectConverter::Type::Color, value).getColor());
        else if (property == "arrowbackgroundcolorhover")
            setArrowBackgroundColorHover(Deserializer::deserialize(ObjectConverter::Type::Color, value).getColor());
        else if (property == "arrowcolor")
            setArrowColor(Deserializer::deserialize(ObjectConverter::Type::Color, value).getColor());
        else if (property == "arrowcolornormal")
            setArrowColorNormal(Deserializer::deserialize(ObjectConverter::Type::Color, value).getColor());
        else if (property == "arrowcolorhover")
            setArrowColorHover(Deserializer::deserialize(ObjectConverter::Type::Color, value).getColor());
        else if (property == "textcolor")
            setTextColor(Deserializer::deserialize(ObjectConverter::Type::Color, value).getColor());
        else if (property == "bordercolor")
            setBorderColor(Deserializer::deserialize(ObjectConverter::Type::Color, value).getColor());
        else if (property == "backgroundimage")
            setBackgroundTexture(Deserializer::deserialize(ObjectConverter::Type::Texture, value).getTexture());
        else if (property == "arrowupimage")
            setArrowUpTexture(Deserializer::deserialize(ObjectConverter::Type::Texture, value).getTexture());
        else if (property == "arrowuphoverimage")
            setArrowUpHoverTexture(Deserializer::deserialize(ObjectConverter::Type::Texture, value).getTexture());
        else if (property == "arrowdownimage")
            setArrowDownTexture(Deserializer::deserialize(ObjectConverter::Type::Texture, value).getTexture());
        else if (property == "arrowdownhoverimage")
            setArrowDownHoverTexture(Deserializer::deserialize(ObjectConverter::Type::Texture, value).getTexture());
        else if (property == "listbox")
        {
            if (m_comboBox->getTheme() == nullptr)
                throw Exception{"Failed to load the internal list box, ComboBox has no connected theme to load the list box with"};

            if (toLower(value) == "default")
                m_comboBox->m_listBox = std::make_shared<ListBox>();
            else
            {
                m_comboBox->m_listBox = m_comboBox->getTheme()->internalLoad(m_comboBox->getPrimaryLoadingParameter(),
                                                                             Deserializer::deserialize(ObjectConverter::Type::String, value).getString());
            }

            m_comboBox->initListBox();
        }
        else
            WidgetRenderer::setProperty(property, value);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ComboBoxRenderer::setProperty(std::string property, ObjectConverter&& value)
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
            else if (property == "arrowbackgroundcolor")
                setArrowBackgroundColor(value.getColor());
            else if (property == "arrowbackgroundcolornormal")
                setArrowBackgroundColorNormal(value.getColor());
            else if (property == "arrowbackgroundcolorhover")
                setArrowBackgroundColorHover(value.getColor());
            else if (property == "arrowcolor")
                setArrowColor(value.getColor());
            else if (property == "arrowcolornormal")
                setArrowColorNormal(value.getColor());
            else if (property == "arrowcolorhover")
                setArrowColorHover(value.getColor());
            else if (property == "textcolor")
                setTextColor(value.getColor());
            else if (property == "bordercolor")
                setBorderColor(value.getColor());
            else
                WidgetRenderer::setProperty(property, std::move(value));
        }
        else if (value.getType() == ObjectConverter::Type::Texture)
        {
            if (property == "backgroundimage")
                setBackgroundTexture(value.getTexture());
            else if (property == "arrowupimage")
                setArrowUpTexture(value.getTexture());
            else if (property == "arrowuphoverimage")
                setArrowUpHoverTexture(value.getTexture());
            else if (property == "arrowdownimage")
                setArrowDownTexture(value.getTexture());
            else if (property == "arrowdownhoverimage")
                setArrowDownHoverTexture(value.getTexture());
            else
                WidgetRenderer::setProperty(property, std::move(value));
        }
        else if (value.getType() == ObjectConverter::Type::String)
        {
            if (property == "listbox")
            {
                if (toLower(value.getString()) == "default")
                    m_comboBox->m_listBox = std::make_shared<ListBox>();
                else
                {
                    if (m_comboBox->getTheme() == nullptr)
                        throw Exception{"Failed to load the internal list box, ComboBox has no connected theme to load the list box with"};

                    m_comboBox->m_listBox = m_comboBox->getTheme()->internalLoad(m_comboBox->getPrimaryLoadingParameter(), value.getString());
                }

                m_comboBox->initListBox();
            }
        }
        else
            WidgetRenderer::setProperty(property, std::move(value));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ObjectConverter ComboBoxRenderer::getProperty(std::string property) const
    {
        property = toLower(property);

        if (property == "borders")
            return m_borders;
        else if (property == "padding")
            return m_padding;
        else if (property == "backgroundcolor")
            return getListBox()->m_backgroundColor;
        else if (property == "arrowbackgroundcolor")
            return m_arrowBackgroundColorNormal;
        else if (property == "arrowbackgroundcolornormal")
            return m_arrowBackgroundColorNormal;
        else if (property == "arrowbackgroundcolorhover")
            return m_arrowBackgroundColorHover;
        else if (property == "arrowcolor")
            return m_arrowColorNormal;
        else if (property == "arrowcolornormal")
            return m_arrowColorNormal;
        else if (property == "arrowcolorhover")
            return m_arrowColorHover;
        else if (property == "textcolor")
            return m_textColor;
        else if (property == "bordercolor")
            return getListBox()->m_borderColor;
        else if (property == "backgroundimage")
            return m_backgroundTexture;
        else if (property == "arrowupimage")
            return m_textureArrowUpNormal;
        else if (property == "arrowuphoverimage")
            return m_textureArrowUpHover;
        else if (property == "arrowdownimage")
            return m_textureArrowDownNormal;
        else if (property == "arrowdownhoverimage")
            return m_textureArrowDownHover;
        else
            return WidgetRenderer::getProperty(property);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::map<std::string, ObjectConverter> ComboBoxRenderer::getPropertyValuePairs() const
    {
        auto pairs = WidgetRenderer::getPropertyValuePairs();

        if (m_backgroundTexture.isLoaded())
            pairs["BackgroundImage"] = m_backgroundTexture;
        else
            pairs["BackgroundColor"] = getListBox()->m_backgroundColor;

        if (m_textureArrowUpNormal.isLoaded() && m_textureArrowDownNormal.isLoaded())
        {
            pairs["ArrowUpImage"] = m_textureArrowUpNormal;
            pairs["ArrowDownImage"] = m_textureArrowDownNormal;
            if (m_textureArrowUpHover.isLoaded())
                pairs["ArrowUpHoverImage"] = m_textureArrowUpHover;
            if (m_textureArrowDownHover.isLoaded())
                pairs["ArrowDownHoverImage"] = m_textureArrowDownHover;
        }
        else
        {
            pairs["ArrowBackgroundColorNormal"] = m_arrowBackgroundColorNormal;
            pairs["ArrowBackgroundColorHover"] = m_arrowBackgroundColorHover;
            pairs["ArrowColorNormal"] = m_arrowColorNormal;
            pairs["ArrowColorHover"] = m_arrowColorHover;
        }

        pairs["TextColor"] = m_textColor;
        pairs["BorderColor"] = getListBox()->m_borderColor;
        pairs["Borders"] = m_borders;
        pairs["Padding"] = m_padding;
        return pairs;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ComboBoxRenderer::setBackgroundColor(const Color& backgroundColor)
    {
        getListBox()->setBackgroundColor(backgroundColor);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ComboBoxRenderer::setArrowBackgroundColor(const Color& color)
    {
        setArrowBackgroundColorNormal(color);
        setArrowBackgroundColorHover(color);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ComboBoxRenderer::setArrowBackgroundColorNormal(const Color& color)
    {
        m_arrowBackgroundColorNormal = color;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ComboBoxRenderer::setArrowBackgroundColorHover(const Color& color)
    {
        m_arrowBackgroundColorHover = color;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ComboBoxRenderer::setArrowColor(const Color& color)
    {
        setArrowColorNormal(color);
        setArrowColorHover(color);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ComboBoxRenderer::setArrowColorNormal(const Color& color)
    {
        m_arrowColorNormal = color;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ComboBoxRenderer::setArrowColorHover(const Color& color)
    {
        m_arrowColorHover = color;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ComboBoxRenderer::setTextColor(const Color& textColor)
    {
        m_textColor = textColor;
        m_comboBox->m_text.setTextColor(calcColorOpacity(m_textColor, m_comboBox->getOpacity()));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ComboBoxRenderer::setBorderColor(const Color& borderColor)
    {
        getListBox()->setBorderColor(borderColor);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ComboBoxRenderer::setBackgroundTexture(const Texture& texture)
    {
        m_backgroundTexture = texture;
        if (m_backgroundTexture.isLoaded())
        {
            m_backgroundTexture.setPosition(m_comboBox->getPosition());
            m_backgroundTexture.setSize(m_comboBox->getSize());
            m_backgroundTexture.setColor({m_backgroundTexture.getColor().r, m_backgroundTexture.getColor().g, m_backgroundTexture.getColor().b, static_cast<sf::Uint8>(m_comboBox->getOpacity() * 255)});
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ComboBoxRenderer::setArrowUpTexture(const Texture& texture)
    {
        m_textureArrowUpNormal = texture;
        if (m_textureArrowUpNormal.isLoaded())
        {
            Padding padding = getScaledPadding();
            float height = m_comboBox->getSize().y - padding.top - padding.bottom;

            m_textureArrowUpNormal.setSize({m_textureArrowUpNormal.getImageSize().x * (height / m_textureArrowUpNormal.getImageSize().y), height});
            m_textureArrowUpNormal.setPosition({m_comboBox->getPosition().x + m_comboBox->getSize().x - m_textureArrowUpNormal.getSize().x - padding.right, m_comboBox->getPosition().y + padding.top});
            m_textureArrowUpNormal.setColor({m_textureArrowUpNormal.getColor().r, m_textureArrowUpNormal.getColor().g, m_textureArrowUpNormal.getColor().b, static_cast<sf::Uint8>(m_comboBox->getOpacity() * 255)});
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ComboBoxRenderer::setArrowDownTexture(const Texture& texture)
    {
        m_textureArrowDownNormal = texture;
        if (m_textureArrowDownNormal.isLoaded())
        {
            Padding padding = getScaledPadding();
            float height = m_comboBox->getSize().y - padding.top - padding.bottom;

            m_textureArrowDownNormal.setSize({m_textureArrowDownNormal.getImageSize().x * (height / m_textureArrowDownNormal.getImageSize().y), height});
            m_textureArrowDownNormal.setPosition({m_comboBox->getPosition().x + m_comboBox->getSize().x - m_textureArrowDownNormal.getSize().x - padding.right, m_comboBox->getPosition().y + padding.top});
            m_textureArrowDownNormal.setColor({m_textureArrowDownNormal.getColor().r, m_textureArrowDownNormal.getColor().g, m_textureArrowDownNormal.getColor().b, static_cast<sf::Uint8>(m_comboBox->getOpacity() * 255)});
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ComboBoxRenderer::setArrowUpHoverTexture(const Texture& texture)
    {
        m_textureArrowUpHover = texture;
        if (m_textureArrowUpHover.isLoaded())
        {
            Padding padding = getScaledPadding();
            float height = m_comboBox->getSize().y - padding.top - padding.bottom;

            m_textureArrowUpHover.setSize({m_textureArrowUpHover.getImageSize().x * (height / m_textureArrowUpHover.getImageSize().y), height});
            m_textureArrowUpHover.setPosition({m_comboBox->getPosition().x + m_comboBox->getSize().x - m_textureArrowUpHover.getSize().x - padding.right, m_comboBox->getPosition().y + padding.top});
            m_textureArrowUpHover.setColor({m_textureArrowUpHover.getColor().r, m_textureArrowUpHover.getColor().g, m_textureArrowUpHover.getColor().b, static_cast<sf::Uint8>(m_comboBox->getOpacity() * 255)});
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ComboBoxRenderer::setArrowDownHoverTexture(const Texture& texture)
    {
        m_textureArrowDownHover = texture;
        if (m_textureArrowDownHover.isLoaded())
        {
            Padding padding = getScaledPadding();
            float height = m_comboBox->getSize().y - padding.top - padding.bottom;

            m_textureArrowDownHover.setSize({m_textureArrowDownHover.getImageSize().x * (height / m_textureArrowDownHover.getImageSize().y), height});
            m_textureArrowDownHover.setPosition({m_comboBox->getPosition().x + m_comboBox->getSize().x - m_textureArrowDownHover.getSize().x - padding.right, m_comboBox->getPosition().y + padding.top});
            m_textureArrowDownHover.setColor({m_textureArrowDownHover.getColor().r, m_textureArrowDownHover.getColor().g, m_textureArrowDownHover.getColor().b, static_cast<sf::Uint8>(m_comboBox->getOpacity() * 255)});
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ComboBoxRenderer::setBorders(const Borders& borders)
    {
        WidgetBorders::setBorders(borders);
        getListBox()->setBorders({m_borders.left, 0, m_borders.right, m_borders.bottom});
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ComboBoxRenderer::setPadding(const Padding& padding)
    {
        WidgetPadding::setPadding(padding);
        m_comboBox->updateSize();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::shared_ptr<ListBoxRenderer> ComboBoxRenderer::getListBox() const
    {
        return m_comboBox->m_listBox->getRenderer();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ComboBoxRenderer::draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        sf::Vector2f position = m_comboBox->getPosition();
        sf::Vector2f size = m_comboBox->getSize();

        if (m_backgroundTexture.isLoaded())
            target.draw(m_backgroundTexture, states);
        else
        {
            sf::RectangleShape front(size);
            front.setPosition(position);
            front.setFillColor(calcColorOpacity(getListBox()->m_backgroundColor, m_comboBox->getOpacity()));
            target.draw(front, states);
        }

        // Draw the borders
        if (m_borders != Borders{0, 0, 0, 0})
        {
            // Draw left border
            sf::RectangleShape border({m_borders.left, size.y + m_borders.top});
            border.setPosition(position.x - m_borders.left, position.y - m_borders.top);
            border.setFillColor(calcColorOpacity(getListBox()->m_borderColor, m_comboBox->getOpacity()));
            target.draw(border, states);

            // Draw top border
            border.setSize({size.x + m_borders.right, m_borders.top});
            border.setPosition(position.x, position.y - m_borders.top);
            target.draw(border, states);

            // Draw right border
            border.setSize({m_borders.right, size.y + m_borders.bottom});
            border.setPosition(position.x + size.x, position.y);
            target.draw(border, states);

            // Draw bottom border
            border.setSize({size.x + m_borders.left, m_borders.bottom});
            border.setPosition(position.x - m_borders.left, position.y + size.y);
            target.draw(border, states);
        }

        // Check if we have textures for the arrow
        if (m_textureArrowUpNormal.isLoaded() && m_textureArrowDownNormal.isLoaded())
        {
            // Set the arrow like it should (down when list box is invisible, up when it is visible)
            if (m_comboBox->m_listBox->isVisible())
            {
                if (m_comboBox->m_mouseHover && m_textureArrowUpHover.isLoaded())
                    target.draw(m_textureArrowUpHover, states);
                else
                    target.draw(m_textureArrowUpNormal, states);
            }
            else
            {
                if (m_comboBox->m_mouseHover && m_textureArrowDownHover.isLoaded())
                    target.draw(m_textureArrowDownHover, states);
                else
                    target.draw(m_textureArrowDownNormal, states);
            }
        }
        else // There are no textures for the arrow
        {
            Padding padding = getScaledPadding();
            float arrowSize = size.y - padding.top - padding.bottom;

            sf::RectangleShape arrowBackground({arrowSize, arrowSize});
            arrowBackground.setPosition({position.x + size.x - padding.right - arrowSize, position.y + padding.top});

            sf::ConvexShape arrow{3};
            if (m_comboBox->m_listBox->isVisible())
            {
                arrow.setPoint(0, {arrowBackground.getPosition().x + (arrowSize / 5), arrowBackground.getPosition().y + (arrowSize * 4/5)});
                arrow.setPoint(1, {arrowBackground.getPosition().x + (arrowSize / 2), arrowBackground.getPosition().y + (arrowSize / 5)});
                arrow.setPoint(2, {arrowBackground.getPosition().x + (arrowSize * 4/5), arrowBackground.getPosition().y + (arrowSize * 4/5)});
            }
            else // The listbox is not visible, the arrow points down
            {
                arrow.setPoint(0, {arrowBackground.getPosition().x + (arrowSize / 5), arrowBackground.getPosition().y + (arrowSize / 5)});
                arrow.setPoint(1, {arrowBackground.getPosition().x + (arrowSize / 2), arrowBackground.getPosition().y + (arrowSize * 4/5)});
                arrow.setPoint(2, {arrowBackground.getPosition().x + (arrowSize * 4/5), arrowBackground.getPosition().y + (arrowSize / 5)});
            }

            if (m_comboBox->m_mouseHover)
            {
                arrow.setFillColor(calcColorOpacity(m_arrowColorHover, m_comboBox->getOpacity()));
                arrowBackground.setFillColor(calcColorOpacity(m_arrowBackgroundColorHover, m_comboBox->getOpacity()));
            }
            else
            {
                arrow.setFillColor(calcColorOpacity(m_arrowColorNormal, m_comboBox->getOpacity()));
                arrowBackground.setFillColor(calcColorOpacity(m_arrowBackgroundColorNormal, m_comboBox->getOpacity()));
            }

            target.draw(arrowBackground, states);
            target.draw(arrow, states);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Padding ComboBoxRenderer::getScaledPadding() const
    {
        Padding padding = getPadding();
        Padding scaledPadding = padding;

        auto& texture = m_backgroundTexture;
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

        return scaledPadding;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::shared_ptr<WidgetRenderer> ComboBoxRenderer::clone(Widget* widget)
    {
        auto renderer = std::make_shared<ComboBoxRenderer>(*this);
        renderer->m_comboBox = static_cast<ComboBox*>(widget);
        return renderer;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
