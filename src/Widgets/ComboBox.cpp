/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus' Graphical User Interface
// Copyright (C) 2012-2023 Bruno Van de Velde (vdv_b@tgui.eu)
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

#if !TGUI_EXPERIMENTAL_USE_STD_MODULE
    #include <cmath>
#endif

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
#if TGUI_COMPILED_WITH_CPP_VER < 17
    constexpr const char ComboBox::StaticWidgetType[];
#endif

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ComboBox::ComboBox(const char* typeName, bool initRenderer) :
        Widget{typeName, false}
    {
        m_text.setFont(m_fontCached);
        m_defaultText.setFont(m_fontCached);

        initListBox();

        if (initRenderer)
        {
            m_renderer = aurora::makeCopied<ComboBoxRenderer>();
            setRenderer(Theme::getDefault()->getRendererNoThrow(m_type));

            setTextSize(getGlobalTextSize());
            setSize({m_text.getLineHeight() * 10,
                     std::round(m_text.getLineHeight() * 1.25f) + m_paddingCached.getTop() + m_paddingCached.getBottom() + m_bordersCached.getTop() + m_bordersCached.getBottom()});
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ComboBox::ComboBox(const ComboBox& other) :
        Widget                           {other},
        onItemSelect                     {other.onItemSelect},
        m_nrOfItemsToDisplay             {other.m_nrOfItemsToDisplay},
        m_listBox                        {ListBox::copy(other.m_listBox)},
        m_text                           {other.m_text},
        m_defaultText                    {other.m_defaultText},
        m_changeItemOnScroll             {other.m_changeItemOnScroll},
        m_expandDirection                {other.m_expandDirection},
        m_spriteBackground               {other.m_spriteBackground},
        m_spriteArrow                    {other.m_spriteArrow},
        m_spriteArrowHover               {other.m_spriteArrowHover},
        m_bordersCached                  {other.m_bordersCached},
        m_paddingCached                  {other.m_paddingCached},
        m_borderColorCached              {other.m_borderColorCached},
        m_backgroundColorCached          {other.m_backgroundColorCached},
        m_arrowColorCached               {other.m_arrowColorCached},
        m_arrowColorHoverCached          {other.m_arrowColorHoverCached},
        m_arrowBackgroundColorCached     {other.m_arrowBackgroundColorCached},
        m_arrowBackgroundColorHoverCached{other.m_arrowBackgroundColorHoverCached}
    {
        initListBox();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ComboBox::ComboBox(ComboBox&& other) noexcept :
        Widget                           {std::move(other)},
        onItemSelect                     {std::move(other.onItemSelect)},
        m_nrOfItemsToDisplay             {std::move(other.m_nrOfItemsToDisplay)},
        m_listBox                        {std::move(other.m_listBox)},
        m_text                           {std::move(other.m_text)},
        m_defaultText                    {std::move(other.m_defaultText)},
        m_changeItemOnScroll             {std::move(other.m_changeItemOnScroll)},
        m_expandDirection                {std::move(other.m_expandDirection)},
        m_spriteBackground               {std::move(other.m_spriteBackground)},
        m_spriteArrow                    {std::move(other.m_spriteArrow)},
        m_spriteArrowHover               {std::move(other.m_spriteArrowHover)},
        m_bordersCached                  {std::move(other.m_bordersCached)},
        m_paddingCached                  {std::move(other.m_paddingCached)},
        m_borderColorCached              {std::move(other.m_borderColorCached)},
        m_backgroundColorCached          {std::move(other.m_backgroundColorCached)},
        m_arrowColorCached               {std::move(other.m_arrowColorCached)},
        m_arrowColorHoverCached          {std::move(other.m_arrowColorHoverCached)},
        m_arrowBackgroundColorCached     {std::move(other.m_arrowBackgroundColorCached)},
        m_arrowBackgroundColorHoverCached{std::move(other.m_arrowBackgroundColorHoverCached)}
    {
        initListBox();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ComboBox& ComboBox::operator= (const ComboBox& other)
    {
        if (this != &other)
        {
            ComboBox temp{other};
            Widget::operator=(other);

            std::swap(onItemSelect,                      temp.onItemSelect);
            std::swap(m_nrOfItemsToDisplay,              temp.m_nrOfItemsToDisplay);
            std::swap(m_listBox,                         temp.m_listBox);
            std::swap(m_text,                            temp.m_text);
            std::swap(m_defaultText,                     temp.m_defaultText);
            std::swap(m_changeItemOnScroll,              temp.m_changeItemOnScroll);
            std::swap(m_expandDirection,                 temp.m_expandDirection);
            std::swap(m_spriteBackground,                temp.m_spriteBackground);
            std::swap(m_spriteArrow,                     temp.m_spriteArrow);
            std::swap(m_spriteArrowHover,                temp.m_spriteArrowHover);
            std::swap(m_bordersCached,                   temp.m_bordersCached);
            std::swap(m_paddingCached,                   temp.m_paddingCached);
            std::swap(m_borderColorCached,               temp.m_borderColorCached);
            std::swap(m_backgroundColorCached,           temp.m_backgroundColorCached);
            std::swap(m_arrowColorCached,                temp.m_arrowColorCached);
            std::swap(m_arrowColorHoverCached,           temp.m_arrowColorHoverCached);
            std::swap(m_arrowBackgroundColorCached,      temp.m_arrowBackgroundColorCached);
            std::swap(m_arrowBackgroundColorHoverCached, temp.m_arrowBackgroundColorHoverCached);
        }

        return *this;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ComboBox& ComboBox::operator= (ComboBox&& other) noexcept
    {
        if (this != &other)
        {
            onItemSelect                      = std::move(other.onItemSelect);
            m_nrOfItemsToDisplay              = std::move(other.m_nrOfItemsToDisplay);
            m_listBox                         = std::move(other.m_listBox);
            m_text                            = std::move(other.m_text);
            m_defaultText                     = std::move(other.m_defaultText);
            m_changeItemOnScroll              = std::move(other.m_changeItemOnScroll);
            m_expandDirection                 = std::move(other.m_expandDirection);
            m_spriteBackground                = std::move(other.m_spriteBackground);
            m_spriteArrow                     = std::move(other.m_spriteArrow);
            m_spriteArrowHover                = std::move(other.m_spriteArrowHover);
            m_bordersCached                   = std::move(other.m_bordersCached);
            m_paddingCached                   = std::move(other.m_paddingCached);
            m_borderColorCached               = std::move(other.m_borderColorCached);
            m_backgroundColorCached           = std::move(other.m_backgroundColorCached);
            m_arrowColorCached                = std::move(other.m_arrowColorCached);
            m_arrowColorHoverCached           = std::move(other.m_arrowColorHoverCached);
            m_arrowBackgroundColorCached      = std::move(other.m_arrowBackgroundColorCached);
            m_arrowBackgroundColorHoverCached = std::move(other.m_arrowBackgroundColorHoverCached);
            Widget::operator=(std::move(other));
        }

        return *this;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ComboBox::Ptr ComboBox::create()
    {
        return std::make_shared<ComboBox>();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ComboBox::Ptr ComboBox::copy(const ComboBox::ConstPtr& comboBox)
    {
        if (comboBox)
            return std::static_pointer_cast<ComboBox>(comboBox->clone());
        else
            return nullptr;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ComboBoxRenderer* ComboBox::getSharedRenderer()
    {
        return aurora::downcast<ComboBoxRenderer*>(Widget::getSharedRenderer());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const ComboBoxRenderer* ComboBox::getSharedRenderer() const
    {
        return aurora::downcast<const ComboBoxRenderer*>(Widget::getSharedRenderer());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ComboBoxRenderer* ComboBox::getRenderer()
    {
        return aurora::downcast<ComboBoxRenderer*>(Widget::getRenderer());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ComboBox::setSize(const Layout2d& size)
    {
        Widget::setSize(size);

        m_bordersCached.updateParentSize(getSize());
        m_paddingCached.updateParentSize(getSize());

        m_spriteBackground.setSize(getInnerSize());
        m_spriteBackgroundDisabled.setSize(getInnerSize());

        const float height = getInnerSize().y - m_paddingCached.getTop() - m_paddingCached.getBottom();

        if (height > 0)
        {
            m_listBox->setItemHeight(static_cast<unsigned int>(height));
            updateListBoxHeight();
        }

        if (m_spriteArrow.isSet())
        {
            m_spriteArrow.setSize({m_spriteArrow.getTexture().getImageSize().x * (height / m_spriteArrow.getTexture().getImageSize().y), height});
            m_spriteArrowHover.setSize(m_spriteArrow.getSize());
            m_spriteArrowDisabled.setSize(m_spriteArrow.getSize());
        }

        m_text.setCharacterSize(m_listBox->getTextSize());
        m_defaultText.setCharacterSize(m_listBox->getTextSize());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ComboBox::setEnabled(bool enabled)
    {
        Widget::setEnabled(enabled);
        if (m_enabled || !m_textColorDisabledCached.isSet())
            m_text.setColor(m_textColorCached);
        else
            m_text.setColor(m_textColorDisabledCached);
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

    std::size_t ComboBox::addItem(const String& item, const String& id)
    {
        const std::size_t itemIndex = m_listBox->addItem(item, id);
        updateListBoxHeight();
        return itemIndex;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ComboBox::setSelectedItem(const String& itemName)
    {
        const int previousSelectedItemIndex = m_listBox->getSelectedItemIndex();

        const bool ret = m_listBox->setSelectedItem(itemName);
        m_text.setString(m_listBox->getSelectedItem());

        if (previousSelectedItemIndex != m_listBox->getSelectedItemIndex())
            onItemSelect.emit(this, m_listBox->getSelectedItemIndex(), m_listBox->getSelectedItem(), m_listBox->getSelectedItemId());

        return ret;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ComboBox::setSelectedItemById(const String& id)
    {
        const int previousSelectedItemIndex = m_listBox->getSelectedItemIndex();

        const bool ret = m_listBox->setSelectedItemById(id);
        m_text.setString(m_listBox->getSelectedItem());

        if (previousSelectedItemIndex != m_listBox->getSelectedItemIndex())
            onItemSelect.emit(this, m_listBox->getSelectedItemIndex(), m_listBox->getSelectedItem(), m_listBox->getSelectedItemId());

        return ret;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ComboBox::setSelectedItemByIndex(std::size_t index)
    {
        const int previousSelectedItemIndex = m_listBox->getSelectedItemIndex();

        const bool ret = m_listBox->setSelectedItemByIndex(index);
        m_text.setString(m_listBox->getSelectedItem());

        if (previousSelectedItemIndex != m_listBox->getSelectedItemIndex())
            onItemSelect.emit(this, m_listBox->getSelectedItemIndex(), m_listBox->getSelectedItem(), m_listBox->getSelectedItemId());

        return ret;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ComboBox::deselectItem()
    {
        m_text.setString("");
        m_listBox->deselectItem();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ComboBox::removeItem(const String& itemName)
    {
        // Implemented like removeItemByIndex instead of like removeItemById because m_listBox->getItems() may be slow

        const bool ret = m_listBox->removeItem(itemName);

        m_text.setString(m_listBox->getSelectedItem());

        // Shrink the list size
        if ((m_nrOfItemsToDisplay == 0) || (m_listBox->getItemCount() < m_nrOfItemsToDisplay))
            updateListBoxHeight();

        return ret;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ComboBox::removeItemById(const String& id)
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
        const bool ret = m_listBox->removeItemByIndex(index);

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

    String ComboBox::getItemById(const String& id) const
    {
        return m_listBox->getItemById(id);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    String ComboBox::getSelectedItem() const
    {
        return m_listBox->getSelectedItem();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    String ComboBox::getSelectedItemId() const
    {
        return m_listBox->getSelectedItemId();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    int ComboBox::getSelectedItemIndex() const
    {
        return m_listBox->getSelectedItemIndex();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ComboBox::changeItem(const String& originalValue, const String& newValue)
    {
        const bool ret = m_listBox->changeItem(originalValue, newValue);
        m_text.setString(m_listBox->getSelectedItem());
        return ret;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ComboBox::changeItemById(const String& id, const String& newValue)
    {
        const bool ret = m_listBox->changeItemById(id, newValue);
        m_text.setString(m_listBox->getSelectedItem());
        return ret;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ComboBox::changeItemByIndex(std::size_t index, const String& newValue)
    {
        const bool ret = m_listBox->changeItemByIndex(index, newValue);
        m_text.setString(m_listBox->getSelectedItem());
        return ret;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::vector<String> ComboBox::getItems() const
    {
        return m_listBox->getItems();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::vector<String> ComboBox::getItemIds() const
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

    void ComboBox::updateTextSize()
    {
        m_listBox->setTextSize(m_textSizeCached);
        m_text.setCharacterSize(m_textSizeCached);
        m_defaultText.setCharacterSize(m_textSizeCached);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ComboBox::setDefaultText(const String& defaultText)
    {
        m_defaultText.setString(defaultText);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const String& ComboBox::getDefaultText() const
    {
        return m_defaultText.getString();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ComboBox::setExpandDirection(ExpandDirection direction)
    {
        m_expandDirection = direction;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ComboBox::ExpandDirection ComboBox::getExpandDirection() const
    {
        return m_expandDirection;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ComboBox::contains(const String& item) const
    {
        const auto& items = getItems();
        return std::find(items.begin(), items.end(), item) != items.end();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ComboBox::containsId(const String& id) const
    {
        const auto& ids = getItemIds();
        return std::find(ids.begin(), ids.end(), id) != ids.end();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ComboBox::setChangeItemOnScroll(bool changeOnScroll)
    {
        m_changeItemOnScroll = changeOnScroll;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ComboBox::getChangeItemOnScroll() const
    {
        return m_changeItemOnScroll;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ComboBox::setParent(Container* parent)
    {
        hideListBox();
        Widget::setParent(parent);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ComboBox::isMouseOnWidget(Vector2f pos) const
    {
        if (FloatRect{getPosition().x, getPosition().y, getSize().x, getSize().y}.contains(pos))
        {
            if (!m_transparentTextureCached || !m_spriteBackground.isSet() || !m_spriteBackground.isTransparentPixel(pos - getPosition() - m_bordersCached.getOffset()))
                return true;
        }

        return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ComboBox::leftMousePressed(Vector2f pos)
    {
        Widget::leftMousePressed(pos);

        // If the list wasn't visible then open it
        if (!m_listBox->isVisible())
        {
            // Show the list
            showListBox();

            // Reselect the selected item to make sure it is always among the visible items when the list opens
            if (m_listBox->getSelectedItemIndex() >= 0)
                m_listBox->setSelectedItemByIndex(static_cast<std::size_t>(m_listBox->getSelectedItemIndex()));
        }
        else // This list was already open, so close it now
            hideListBox();

        return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ComboBox::scrolled(float delta, Vector2f, bool touch)
    {
        // Don't react to two finger scrolling
        if (touch)
            return false;

        // Don't do anything when changing item on scroll is disabled
        if (!m_changeItemOnScroll)
            return false;

        // Only act to scrolling when the list is not being shown
        if (m_listBox->isVisible())
            return false;

        if (m_listBox->getItemCount() == 0)
            return false;

        // Check if you are scrolling down
        if (delta < 0)
        {
            // Select the next item
            const std::size_t nextIndex = (m_listBox->getSelectedItemIndex() < 0) ? 0 : static_cast<std::size_t>(m_listBox->getSelectedItemIndex()) + 1;
            if (nextIndex < m_listBox->getItemCount())
            {
                m_listBox->setSelectedItemByIndex(nextIndex);
                m_text.setString(m_listBox->getSelectedItem());
                onItemSelect.emit(this, m_listBox->getSelectedItemIndex(), m_listBox->getSelectedItem(), m_listBox->getSelectedItemId());
            }
        }
        else // You are scrolling up
        {
            // Select the previous item
            if (m_listBox->getSelectedItemIndex() > 0)
            {
                m_listBox->setSelectedItemByIndex(static_cast<std::size_t>(m_listBox->getSelectedItemIndex() - 1));
                m_text.setString(m_listBox->getSelectedItem());
                onItemSelect.emit(this, m_listBox->getSelectedItemIndex(), m_listBox->getSelectedItem(), m_listBox->getSelectedItemId());
            }
        }

        return true;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Signal& ComboBox::getSignal(String signalName)
    {
        if (signalName == onItemSelect.getName())
            return onItemSelect;
        else
            return Widget::getSignal(std::move(signalName));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ComboBox::rendererChanged(const String& property)
    {
        if (property == U"Borders")
        {
            m_bordersCached = getSharedRenderer()->getBorders();
            setSize(m_size);
        }
        else if (property == U"Padding")
        {
            m_paddingCached = getSharedRenderer()->getPadding();
            setSize(m_size);
        }
        else if (property == U"TextColor")
        {
            m_textColorCached = getSharedRenderer()->getTextColor();
            if (m_enabled || !m_textColorDisabledCached.isSet())
                m_text.setColor(m_textColorCached);
            if (!getSharedRenderer()->getDefaultTextColor().isSet())
                m_defaultText.setColor(getSharedRenderer()->getTextColor());
        }
        else if (property == U"TextColorDisabled")
        {
            m_textColorDisabledCached = getSharedRenderer()->getTextColorDisabled();
            if (!m_enabled && m_textColorDisabledCached.isSet())
                m_text.setColor(m_textColorDisabledCached);
        }
        else if (property == U"TextStyle")
        {
            m_text.setStyle(getSharedRenderer()->getTextStyle());
            if (!getSharedRenderer()->getDefaultTextStyle().isSet())
                m_defaultText.setStyle(getSharedRenderer()->getTextStyle());
        }
        else if (property == U"DefaultTextColor")
        {
            if (getSharedRenderer()->getDefaultTextColor().isSet())
                m_defaultText.setColor(getSharedRenderer()->getDefaultTextColor());
            else
                m_defaultText.setColor(getSharedRenderer()->getTextColor());
        }
        else if (property == U"DefaultTextStyle")
        {
            if (getSharedRenderer()->getDefaultTextStyle().isSet())
                m_defaultText.setStyle(getSharedRenderer()->getDefaultTextStyle());
            else
                m_defaultText.setStyle(getSharedRenderer()->getTextStyle());
        }
        else if (property == U"TextureBackground")
        {
            m_spriteBackground.setTexture(getSharedRenderer()->getTextureBackground());
        }
        else if (property == U"TextureBackgroundDisabled")
        {
            m_spriteBackgroundDisabled.setTexture(getSharedRenderer()->getTextureBackgroundDisabled());
        }
        else if (property == U"TextureArrow")
        {
            m_spriteArrow.setTexture(getSharedRenderer()->getTextureArrow());
            setSize(m_size);
        }
        else if (property == U"TextureArrowHover")
        {
            m_spriteArrowHover.setTexture(getSharedRenderer()->getTextureArrowHover());
        }
        else if (property == U"TextureArrowDisabled")
        {
            m_spriteArrowDisabled.setTexture(getSharedRenderer()->getTextureArrowDisabled());
        }
        else if (property == U"ListBox")
        {
            m_listBox->setRenderer(getSharedRenderer()->getListBox());
        }
        else if (property == U"BorderColor")
        {
            m_borderColorCached = getSharedRenderer()->getBorderColor();
        }
        else if (property == U"BackgroundColor")
        {
            m_backgroundColorCached = getSharedRenderer()->getBackgroundColor();
        }
        else if (property == U"BackgroundColorDisabled")
        {
            m_backgroundColorDisabledCached = getSharedRenderer()->getBackgroundColorDisabled();
        }
        else if (property == U"ArrowBackgroundColor")
        {
            m_arrowBackgroundColorCached = getSharedRenderer()->getArrowBackgroundColor();
        }
        else if (property == U"ArrowBackgroundColorHover")
        {
            m_arrowBackgroundColorHoverCached = getSharedRenderer()->getArrowBackgroundColorHover();
        }
        else if (property == U"ArrowBackgroundColorDisabled")
        {
            m_arrowBackgroundColorDisabledCached = getSharedRenderer()->getArrowBackgroundColorDisabled();
        }
        else if (property == U"ArrowColor")
        {
            m_arrowColorCached = getSharedRenderer()->getArrowColor();
        }
        else if (property == U"ArrowColorHover")
        {
            m_arrowColorHoverCached = getSharedRenderer()->getArrowColorHover();
        }
        else if (property == U"ArrowColorDisabled")
        {
            m_arrowColorDisabledCached = getSharedRenderer()->getArrowColorDisabled();
        }
        else if ((property == U"Opacity") || (property == U"OpacityDisabled"))
        {
            Widget::rendererChanged(property);

            m_spriteBackground.setOpacity(m_opacityCached);
            m_spriteBackgroundDisabled.setOpacity(m_opacityCached);
            m_spriteArrow.setOpacity(m_opacityCached);
            m_spriteArrowHover.setOpacity(m_opacityCached);
            m_spriteArrowDisabled.setOpacity(m_opacityCached);

            m_text.setOpacity(m_opacityCached);
            m_defaultText.setOpacity(m_opacityCached);
        }
        else if (property == U"Font")
        {
            Widget::rendererChanged(property);

            m_text.setFont(m_fontCached);
            m_defaultText.setFont(m_fontCached);
            m_listBox->setInheritedFont(m_fontCached);

            setSize(m_size);
        }
        else
            Widget::rendererChanged(property);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::unique_ptr<DataIO::Node> ComboBox::save(SavingRenderersMap& renderers) const
    {
        auto node = Widget::save(renderers);

        if (getItemCount() > 0)
        {
            const auto& items = getItems();
            const auto& ids = getItemIds();

            bool itemIdsUsed = false;
            String itemList = "[" + Serializer::serialize(items[0]);
            String itemIdList = "[" + Serializer::serialize(ids[0]);
            for (std::size_t i = 1; i < items.size(); ++i)
            {
                itemList += ", " + Serializer::serialize(items[i]);
                itemIdList += ", " + Serializer::serialize(ids[i]);

                if (!ids[i].empty())
                    itemIdsUsed = true;
            }
            itemList += "]";
            itemIdList += "]";

            node->propertyValuePairs[U"Items"] = std::make_unique<DataIO::ValueNode>(itemList);
            if (itemIdsUsed)
                node->propertyValuePairs[U"ItemIds"] = std::make_unique<DataIO::ValueNode>(itemIdList);
        }

        node->propertyValuePairs[U"ItemsToDisplay"] = std::make_unique<DataIO::ValueNode>(String::fromNumber(getItemsToDisplay()));
        node->propertyValuePairs[U"MaximumItems"] = std::make_unique<DataIO::ValueNode>(String::fromNumber(getMaximumItems()));
        node->propertyValuePairs[U"ChangeItemOnScroll"] = std::make_unique<DataIO::ValueNode>(Serializer::serialize(m_changeItemOnScroll));

        if (getExpandDirection() == ComboBox::ExpandDirection::Down)
            node->propertyValuePairs[U"ExpandDirection"] = std::make_unique<DataIO::ValueNode>("Down");
        else if (getExpandDirection() == ComboBox::ExpandDirection::Up)
            node->propertyValuePairs[U"ExpandDirection"] = std::make_unique<DataIO::ValueNode>("Up");

        if (m_listBox->getSelectedItemIndex() >= 0)
            node->propertyValuePairs[U"SelectedItemIndex"] = std::make_unique<DataIO::ValueNode>(String::fromNumber(m_listBox->getSelectedItemIndex()));

        return node;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ComboBox::load(const std::unique_ptr<DataIO::Node>& node, const LoadingRenderersMap& renderers)
    {
        Widget::load(node, renderers);

        if (node->propertyValuePairs[U"Items"])
        {
            if (!node->propertyValuePairs[U"Items"]->listNode)
                throw Exception{U"Failed to parse 'Items' property, expected a list as value"};

            if (node->propertyValuePairs[U"ItemIds"])
            {
                if (!node->propertyValuePairs[U"ItemIds"]->listNode)
                    throw Exception{U"Failed to parse 'ItemIds' property, expected a list as value"};

                if (node->propertyValuePairs[U"Items"]->valueList.size() != node->propertyValuePairs[U"ItemIds"]->valueList.size())
                    throw Exception{U"Amounts of values for 'Items' differs from the amount in 'ItemIds'"};

                for (std::size_t i = 0; i < node->propertyValuePairs[U"Items"]->valueList.size(); ++i)
                {
                    addItem(Deserializer::deserialize(ObjectConverter::Type::String, node->propertyValuePairs[U"Items"]->valueList[i]).getString(),
                            Deserializer::deserialize(ObjectConverter::Type::String, node->propertyValuePairs[U"ItemIds"]->valueList[i]).getString());
                }
            }
            else // There are no item ids
            {
                for (const auto& item : node->propertyValuePairs[U"Items"]->valueList)
                    addItem(Deserializer::deserialize(ObjectConverter::Type::String, item).getString());
            }
        }
        else // If there are no items, there should be no item ids
        {
            if (node->propertyValuePairs[U"ItemIds"])
                throw Exception{U"Found 'ItemIds' property while there is no 'Items' property"};
        }

        if (node->propertyValuePairs[U"ItemsToDisplay"])
            setItemsToDisplay(node->propertyValuePairs[U"ItemsToDisplay"]->value.toUInt());
        if (node->propertyValuePairs[U"MaximumItems"])
            setMaximumItems(node->propertyValuePairs[U"MaximumItems"]->value.toUInt());
        if (node->propertyValuePairs[U"SelectedItemIndex"])
            setSelectedItemByIndex(node->propertyValuePairs[U"SelectedItemIndex"]->value.toUInt());
        if (node->propertyValuePairs[U"ChangeItemOnScroll"])
            m_changeItemOnScroll = Deserializer::deserialize(ObjectConverter::Type::Bool, node->propertyValuePairs[U"ChangeItemOnScroll"]->value).getBool();

        if (node->propertyValuePairs[U"ExpandDirection"])
        {
            if (node->propertyValuePairs[U"ExpandDirection"]->value == U"Up")
                setExpandDirection(ComboBox::ExpandDirection::Up);
            else if (node->propertyValuePairs[U"ExpandDirection"]->value == U"Down")
                setExpandDirection(ComboBox::ExpandDirection::Down);
            else if (node->propertyValuePairs[U"ExpandDirection"]->value == U"Automatic")
                setExpandDirection(ComboBox::ExpandDirection::Automatic);
            else
                throw Exception{U"Failed to parse ExpandDirection property. Only the values Up, Down and Automatic are correct."};
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Vector2f ComboBox::getInnerSize() const
    {
        return {std::max(0.f, getSize().x - m_bordersCached.getLeft() - m_bordersCached.getRight()),
                std::max(0.f, getSize().y - m_bordersCached.getTop() - m_bordersCached.getBottom())};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ComboBox::updateListBoxHeight()
    {
        const Borders borders = m_listBox->getSharedRenderer()->getBorders();
        const Padding padding = m_listBox->getSharedRenderer()->getPadding();

        if (m_nrOfItemsToDisplay > 0)
            m_listBox->setSize({getSize().x, (m_listBox->getItemHeight() * (std::min<std::size_t>(m_nrOfItemsToDisplay, std::max<std::size_t>(m_listBox->getItemCount(), 1))))
                                             + borders.getTop() + borders.getBottom() + padding.getTop() + padding.getBottom()});
        else
            m_listBox->setSize({getSize().x, (m_listBox->getItemHeight() * std::max<std::size_t>(m_listBox->getItemCount(), 1))
                                             + borders.getTop() + borders.getBottom() + padding.getTop() + padding.getBottom()});

    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ComboBox::showListBox()
    {
        if (m_listBox->isVisible() || !getParent())
            return;

        m_previousSelectedItemIndex = m_listBox->getSelectedItemIndex();

        // Find the RootContainer that contains the combo box
        Vector2f scale = getScale();
        Container* container = m_parent;
        while (container->getParent() != nullptr)
        {
            scale.x *= container->getScale().x;
            scale.y *= container->getScale().y;
            container = container->getParent();
        }

        m_listBox->setScale(scale);
        m_listBox->setVisible(true);

        ExpandDirection direction = m_expandDirection;
        if (direction == ExpandDirection::Automatic)
        {
            if ((getAbsolutePosition().y + getSize().y + m_listBox->getSize().y - m_bordersCached.getBottom() > container->getSize().y)
             && (getAbsolutePosition().y - m_listBox->getSize().y + m_bordersCached.getTop() > 0))
                direction = ExpandDirection::Up;
            else
                direction = ExpandDirection::Down;
        }

        if (direction == ExpandDirection::Down)
            m_listBox->setPosition(getAbsolutePosition({0, getSize().y - m_bordersCached.getBottom()}));
        else if (direction == ExpandDirection::Up)
            m_listBox->setPosition(getAbsolutePosition({0, m_bordersCached.getTop() - m_listBox->getSize().y}));

        container->add(m_listBox, "#TGUI_INTERNAL$ComboBoxListBox#");

        // Adding the list box to the container might have altered it, so make sure it still inherits the properties of the combo box
        if (m_fontCached != m_listBox->getInheritedFont())
            m_listBox->setInheritedFont(m_fontCached);
        if (m_opacityCached != m_listBox->getInheritedOpacity())
            m_listBox->setInheritedOpacity(m_opacityCached);
        if (m_textSizeCached != m_listBox->getTextSize())
            m_listBox->setTextSize(m_textSizeCached);

        m_listBox->setFocused(true);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ComboBox::hideListBox()
    {
        if (!m_listBox->isVisible())
            return;

        m_listBox->setVisible(false);
        m_listBox->mouseNoLongerOnWidget();
        m_listBox->leftMouseButtonNoLongerDown();

        if (m_listBox->getParent())
            m_listBox->getParent()->remove(m_listBox);

        const int selectedItemIndex = m_listBox->getSelectedItemIndex();
        if (selectedItemIndex != m_previousSelectedItemIndex)
        {
            m_text.setString(m_listBox->getSelectedItem());
            onItemSelect.emit(this, m_listBox->getSelectedItemIndex(), m_listBox->getSelectedItem(), m_listBox->getSelectedItemId());
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ComboBox::initListBox()
    {
        m_listBox->setVisible(false);

        m_listBox->onUnfocus([this](){
            if (!m_mouseHover)
                hideListBox();
        });

        m_listBox->onMouseRelease([this](){ hideListBox(); });
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ComboBox::draw(BackendRenderTarget& target, RenderStates states) const
    {
        // Draw the borders
        if (m_bordersCached != Borders{0})
        {
            target.drawBorders(states, m_bordersCached, getSize(), Color::applyOpacity(m_borderColorCached, m_opacityCached));
            states.transform.translate(m_bordersCached.getOffset());
        }

        RenderStates statesForText = states;

        // Draw the background
        if (m_spriteBackground.isSet())
        {
            if (!m_enabled && m_spriteBackgroundDisabled.isSet())
                target.drawSprite(states, m_spriteBackgroundDisabled);
            else
                target.drawSprite(states, m_spriteBackground);
        }
        else
        {
            if (!m_enabled && m_backgroundColorDisabledCached.isSet())
                target.drawFilledRect(states, getInnerSize(), Color::applyOpacity(m_backgroundColorDisabledCached, m_opacityCached));
            else
                target.drawFilledRect(states, getInnerSize(), Color::applyOpacity(m_backgroundColorCached, m_opacityCached));
        }

        // Check if we have a texture for the arrow
        float arrowSize;
        if (m_spriteArrow.isSet())
        {
            arrowSize = m_spriteArrow.getSize().x;
            states.transform.translate({getInnerSize().x - m_paddingCached.getRight() - arrowSize, m_paddingCached.getTop()});

            if (!m_enabled && m_spriteArrowDisabled.isSet())
                target.drawSprite(states, m_spriteArrowDisabled);
            else if (m_mouseHover && m_spriteArrowHover.isSet())
                target.drawSprite(states, m_spriteArrowHover);
            else
                target.drawSprite(states, m_spriteArrow);
        }
        else // There are no textures for the arrow
        {
            arrowSize = getInnerSize().y - m_paddingCached.getTop() - m_paddingCached.getBottom();
            states.transform.translate({getInnerSize().x - m_paddingCached.getRight() - arrowSize, m_paddingCached.getTop()});

            if (!m_enabled && m_arrowBackgroundColorDisabledCached.isSet())
                target.drawFilledRect(states, {arrowSize, arrowSize}, Color::applyOpacity(m_arrowBackgroundColorDisabledCached, m_opacityCached));
            else if (m_mouseHover && m_arrowBackgroundColorHoverCached.isSet())
                target.drawFilledRect(states, {arrowSize, arrowSize}, Color::applyOpacity(m_arrowBackgroundColorHoverCached, m_opacityCached));
            else
                target.drawFilledRect(states, {arrowSize, arrowSize}, Color::applyOpacity(m_arrowBackgroundColorCached, m_opacityCached));

            Vertex::Color arrowVertexColor;
            if (!m_enabled && m_arrowColorDisabledCached.isSet())
                arrowVertexColor = Vertex::Color(m_arrowColorDisabledCached);
            else if (m_mouseHover && m_arrowColorHoverCached.isSet())
                arrowVertexColor = Vertex::Color(m_arrowColorHoverCached);
            else
                arrowVertexColor = Vertex::Color(m_arrowColorCached);

            target.drawTriangle(states,
                {{arrowSize / 5, arrowSize / 4}, arrowVertexColor},
                {{arrowSize / 2, arrowSize * 3/4}, arrowVertexColor},
                {{arrowSize * 4/5, arrowSize / 4}, arrowVertexColor}
            );
        }

        // Draw the selected item
        const int selectedItemIndex = getSelectedItemIndex();
        if (((selectedItemIndex >= 0) && !m_text.getString().empty()) || ((selectedItemIndex < 0) && !m_defaultText.getString().empty()))
        {
            target.addClippingLayer(statesForText, {{m_paddingCached.getLeft(), m_paddingCached.getTop()},
                {getInnerSize().x - m_paddingCached.getLeft() - m_paddingCached.getRight() - arrowSize, getInnerSize().y - m_paddingCached.getTop() - m_paddingCached.getBottom()}});

            statesForText.transform.translate({m_paddingCached.getLeft() + m_text.getExtraHorizontalPadding(),
                                               m_paddingCached.getTop() + (((getInnerSize().y - m_paddingCached.getTop() - m_paddingCached.getBottom()) - m_text.getSize().y) / 2.0f)});

            if (selectedItemIndex >= 0)
                target.drawText(statesForText, m_text);
            else
                target.drawText(statesForText, m_defaultText);

            target.removeClippingLayer();
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Widget::Ptr ComboBox::clone() const
    {
        return std::make_shared<ComboBox>(*this);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
