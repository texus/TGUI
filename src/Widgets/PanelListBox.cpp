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


#include <TGUI/Widgets/PanelListBox.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    namespace
    {
        const float defaultWidth = 160;
        const float defaultHeight = 140;
        const float defaultPanelHeight = 40;
    }

#if TGUI_COMPILED_WITH_CPP_VER < 17
    constexpr const char PanelListBox::StaticWidgetType[];
#endif

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    PanelListBox::PanelListBox(const char* typeName, const bool initRenderer) :
        ScrollablePanel(typeName, false),
        m_maxItems(0),
        m_panelTemplate(Panel::create()),
        m_selectedItem(-1),
        m_hoveringItem(-1)
    {
        setHorizontalScrollbarPolicy(Scrollbar::Policy::Never);

        if (initRenderer)
        {
            m_renderer = aurora::makeCopied<PanelListBoxRenderer>();
            setRenderer(Theme::getDefault()->getRendererNoThrow(m_type));
        }

        m_panelTemplate->setHeight(defaultPanelHeight);
        setSize({defaultWidth, defaultHeight});
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    PanelListBox::Ptr PanelListBox::create()
    {
        return std::make_shared<PanelListBox>();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    PanelListBox::Ptr PanelListBox::copy(const ConstPtr& panelListBox)
    {
        if (panelListBox)
            return std::static_pointer_cast<PanelListBox>(panelListBox->clone());

        return nullptr;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    PanelListBoxRenderer* PanelListBox::getSharedRenderer()
    {
        return aurora::downcast<PanelListBoxRenderer*>(Widget::getSharedRenderer());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const PanelListBoxRenderer* PanelListBox::getSharedRenderer() const
    {
        return aurora::downcast<const PanelListBoxRenderer*>(Widget::getSharedRenderer());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    PanelListBoxRenderer* PanelListBox::getRenderer()
    {
        return aurora::downcast<PanelListBoxRenderer*>(Widget::getRenderer());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void PanelListBox::setSize(const Layout2d& size)
    {
        ScrollablePanel::setSize(size);
        m_panelTemplate->setSize(size.x, m_panelTemplate->getSize().y);

        updateItemsSize();
        updateItemsPositions();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Panel::Ptr PanelListBox::addItem(const String& id, int index)
    {
        if (m_maxItems > 0 && m_items.size() >= m_maxItems)
            return nullptr;

        auto newPanel = Panel::copy(m_panelTemplate);

        index = index != -1 ? index : static_cast<int>(getItemCount());
        m_items.insert(m_items.begin() + index, Item{newPanel, id});

        updateItemsPositions();

        ScrollablePanel::add(newPanel, id);

        return newPanel;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Panel::Ptr PanelListBox::getPanelTemplate()
    {
        return m_panelTemplate;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Layout PanelListBox::getItemsWidth() const
    {
        return m_panelTemplate->getSize().x;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void PanelListBox::setItemsHeight(const Layout& height) const
    {
        m_panelTemplate->setSize(m_panelTemplate->getSize().x, height);

        updateItemsSize();
        updateItemsPositions();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Layout PanelListBox::getItemsHeight() const
    {
        return m_panelTemplate->getSize().y;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool PanelListBox::setSelectedItem(const Panel::Ptr& panelPtr)
    {
        for (std::size_t i = 0; i < m_items.size(); ++i)
        {
            if (m_items[i].panel == panelPtr)
                return setSelectedItemByIndex(i);
        }

        deselectItem();
        return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool PanelListBox::setSelectedItemById(const String& id)
    {
        for (std::size_t i = 0; i < m_items.size(); ++i)
        {
            if (m_items[i].id == id)
                return setSelectedItemByIndex(i);
        }

        deselectItem();
        return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool PanelListBox::setSelectedItemByIndex(const std::size_t index)
    {
        if (index >= m_items.size())
        {
            deselectItem();
            return false;
        }

        updateSelectedItem(static_cast<int>(index));

        return true;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void PanelListBox::deselectItem()
    {
        updateSelectedItem(-1);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool PanelListBox::removeItem(const Panel::Ptr& panelPtr)
    {
        for (std::size_t i = 0; i < m_items.size(); ++i)
        {
            if (m_items[i].panel == panelPtr)
                return removeItemByIndex(i);
        }

        return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool PanelListBox::removeItemById(const String& id)
    {
        for (std::size_t i = 0; i < m_items.size(); ++i)
        {
            if (m_items[i].id == id)
                return removeItemByIndex(i);
        }

        return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool PanelListBox::removeItemByIndex(const std::size_t index)
    {
        if (index >= m_items.size())
            return false;

        updateHoveringItem(-1);
        if (m_selectedItem == static_cast<int>(index))
        {
            updateSelectedItem(-1);
        }
        else if (m_selectedItem > static_cast<int>(index))
        {
            m_selectedItem -= 1;
        }

        ScrollablePanel::remove(m_items[index].panel);
        m_items.erase(m_items.begin() + static_cast<std::ptrdiff_t>(index));

        updateItemsPositions();

        return true;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void PanelListBox::removeAllItems()
    {
        updateHoveringItem(-1);
        updateSelectedItem(-1);

        m_items.clear();
        ScrollablePanel::removeAllWidgets();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Panel::Ptr PanelListBox::getItemById(const String& id) const
    {
        for (const auto& item : m_items)
        {
            if (item.id == id)
                return item.panel;
        }

        return nullptr;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Panel::Ptr PanelListBox::getItemByIndex(const std::size_t index) const
    {
        if (index >= m_items.size())
            return nullptr;

        return m_items[index].panel;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    int PanelListBox::getIndexById(const String& id) const
    {
        for (std::size_t i = 0; i < m_items.size(); ++i)
        {
            if (m_items[i].id == id)
                return static_cast<int>(i);
        }

        return -1;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    int PanelListBox::getIndexByItem(const Panel::Ptr& panelPtr) const
    {
        for (std::size_t i = 0; i < m_items.size(); ++i)
        {
            if (m_items[i].panel == panelPtr)
                return static_cast<int>(i);
        }

        return -1;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    String PanelListBox::getIdByIndex(const std::size_t index) const
    {
        if (index >= m_items.size())
            return {};

        return m_items[index].id;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Panel::Ptr PanelListBox::getSelectedItem() const
    {
        return m_selectedItem >= 0 ? m_items[static_cast<std::size_t>(m_selectedItem)].panel : nullptr;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    String PanelListBox::getSelectedItemId() const
    {
        return m_selectedItem >= 0 ? m_items[static_cast<std::size_t>(m_selectedItem)].id : std::string();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    int PanelListBox::getSelectedItemIndex() const
    {
        return m_selectedItem;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::size_t PanelListBox::getItemCount() const
    {
        return m_items.size();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::vector<Panel::Ptr> PanelListBox::getItems() const
    {
        std::vector<Panel::Ptr> items;
        items.reserve(m_items.size());

        for (const auto &item : m_items)
            items.push_back(item.panel);

        return items;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::vector<String> PanelListBox::getItemIds() const
    {
        std::vector<String> ids;
        ids.reserve(m_items.size());
        for (const auto &item : m_items)
            ids.push_back(item.id);

        return ids;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void PanelListBox::setMaximumItems(const std::size_t maximumItems)
    {
        m_maxItems = maximumItems;

        if (m_maxItems > 0 && m_maxItems < m_items.size())
        {
            if (m_hoveringItem >= static_cast<int>(maximumItems))
                updateHoveringItem(-1);

            if (m_selectedItem >= static_cast<int>(maximumItems))
                updateSelectedItem(-1);

            for (std::size_t i = m_maxItems; i < getItemCount(); ++i)
                ScrollablePanel::remove(m_items[i].panel);

            m_items.erase(m_items.begin() + static_cast<std::ptrdiff_t>(m_maxItems), m_items.end());

            updateItemsPositions();
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::size_t PanelListBox::getMaximumItems() const
    {
        return m_maxItems;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool PanelListBox::contains(const Panel::Ptr& panelPtr) const
    {
        return std::find_if(m_items.begin(), m_items.end(), [panelPtr](const Item& item) { return item.panel == panelPtr; }) != m_items.end();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool PanelListBox::containsId(const String& id) const
    {
        return std::find_if(m_items.begin(), m_items.end(), [id](const Item& item) { return item.id == id; }) != m_items.end();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void PanelListBox::mouseMoved(const Vector2f pos)
    {
        ScrollablePanel::mouseMoved(pos);

        updateHoveringItem(-1);

        if (m_widgetBelowMouse)
        {
            const auto widgetBelowMouseIndex = getIndexByItem(std::dynamic_pointer_cast<Panel>(m_widgetBelowMouse));

            updateHoveringItem(widgetBelowMouseIndex);

            if (m_mouseDown && m_selectedItem != m_hoveringItem)
                updateSelectedItem(m_hoveringItem);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void PanelListBox::mouseNoLongerOnWidget()
    {
        ScrollablePanel::mouseNoLongerOnWidget();
        updateHoveringItem(-1);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool PanelListBox::leftMousePressed(const Vector2f pos)
    {
        const bool isDragging = ScrollablePanel::leftMousePressed(pos);

        const auto widgetBelowMouseIndex = getIndexByItem(std::dynamic_pointer_cast<Panel>(m_widgetBelowMouse));

        updateHoveringItem(widgetBelowMouseIndex);

        if (m_selectedItem != m_hoveringItem)
            updateSelectedItem(m_hoveringItem);

        return isDragging;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Signal& PanelListBox::getSignal(String signalName)
    {
        if (signalName == onItemSelect.getName())
            return onItemSelect;

        return ScrollablePanel::getSignal(std::move(signalName));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void PanelListBox::rendererChanged(const String& property)
    {
        if (property == "ItemsBackgroundColor")
        {
            m_itemsBackgroundColorCached = getSharedRenderer()->getItemsBackgroundColor();
            m_panelTemplate->getSharedRenderer()->setBackgroundColor(m_itemsBackgroundColorCached);
        }
        else if (property == "ItemsBackgroundColorHover")
        {
            m_itemsBackgroundColorHoverCached = getSharedRenderer()->getItemsBackgroundColorHover();
            updateSelectedAndHoveringItemColorsAndStyle();
        }
        else if (property == "SelectedItemsBackgroundColor")
        {
            m_selectedItemsBackgroundColorCached = getSharedRenderer()->getSelectedItemsBackgroundColor();
            updateSelectedAndHoveringItemColorsAndStyle();
        }
        else if (property == "SelectedItemsBackgroundColorHover")
        {
            m_selectedItemsBackgroundColorHoverCached = getSharedRenderer()->getSelectedItemsBackgroundColorHover();
            updateSelectedAndHoveringItemColorsAndStyle();
        }
        else
        {
            ScrollablePanel::rendererChanged(property);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::unique_ptr<DataIO::Node> PanelListBox::save(SavingRenderersMap& renderers) const
    {
        auto node = ScrollablePanel::save(renderers);

        if (m_selectedItem >= 0)
            node->propertyValuePairs[U"SelectedItemIndex"] = std::make_unique<DataIO::ValueNode>(String::fromNumber(getSelectedItemIndex()));

        node->propertyValuePairs[U"ItemsHeight"] = std::make_unique<DataIO::ValueNode>(String::fromNumber(getItemsHeight().getValue()));
        node->propertyValuePairs[U"ItemsWidth"] = std::make_unique<DataIO::ValueNode>(String::fromNumber(getItemsWidth().getValue()));
        node->propertyValuePairs[U"MaximumItems"] = std::make_unique<DataIO::ValueNode>(String::fromNumber(getMaximumItems()));

        return node;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void PanelListBox::load(const std::unique_ptr<DataIO::Node>& node, const LoadingRenderersMap& renderers)
    {
        ScrollablePanel::load(node, renderers);

        for (const auto& widget : getWidgets())
        {
            if (widget->getWidgetType() == "Panel")
            {
                m_items.emplace_back();
                m_items.back().panel = std::dynamic_pointer_cast<Panel>(widget);
                m_items.back().id = widget->getWidgetName();
            }
        }

        if (node->propertyValuePairs[U"ItemsHeight"])
            setItemsHeight(node->propertyValuePairs[U"ItemsHeight"]->value);

        if (node->propertyValuePairs[U"MaximumItems"])
        {
            unsigned max_items(0);
            if (node->propertyValuePairs[U"MaximumItems"]->value.attemptToUInt(max_items))
            {
                setMaximumItems(max_items);
            }
            else
            {
                throw Exception{ U"Failed to parse MaximumItems property, found unknown value '" + node->propertyValuePairs[U"MaximumItems"]->value + U"'." };
            }
        }

        if (node->propertyValuePairs[U"SelectedItemIndex"])
        {
            int selected_item(-1);
            if (node->propertyValuePairs[U"SelectedItemIndex"]->value.attemptToInt(selected_item))
            {
                setSelectedItemByIndex(static_cast<std::size_t>(selected_item));
            }
            else
            {
                throw Exception{ U"Failed to parse SelectedItemIndex property, found unknown value '" + node->propertyValuePairs[U"SelectedItemIndex"]->value + U"'." };
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Widget::Ptr PanelListBox::clone() const
    {
        return std::make_shared<PanelListBox>(*this);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void PanelListBox::updateItemsPositions() const
    {
        for (std::size_t i = 0; i < getItemCount(); ++i)
        {
            const auto itemVerticalPosition = i * getItemsHeight();
            m_items[i].panel->setPosition(0, itemVerticalPosition);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void PanelListBox::updateItemsSize() const
    {
        for (const auto& item : m_items)
            item.panel->setSize(m_panelTemplate->getSize());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    float PanelListBox::getAllItemsHeight() const
    {
        float itemsHeight(0);
        for (const auto& item : m_items)
            itemsHeight += item.panel->getSize().y;

        return itemsHeight;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    float PanelListBox::getItemsHeightUpToIndex(const std::size_t index) const
    {
        float itemsHeight(0);
        for (std::size_t i = 0; i < index; ++i)
            itemsHeight += m_items[i].panel->getSize().y;

        return itemsHeight;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void PanelListBox::updateSelectedItem(const int item)
    {
        if (m_selectedItem == item)
            return;

        if (m_selectedItem >= 0)
            clearItemStyle(m_selectedItem);

        m_selectedItem = item;
        if (m_selectedItem >= 0)
        {
            const auto& selectedItemObj = m_items[static_cast<std::size_t>(m_selectedItem)];
            onItemSelect.emit(this, m_selectedItem, selectedItemObj.panel, selectedItemObj.id);
        }

        updateSelectedAndHoveringItemColorsAndStyle();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void PanelListBox::updateHoveringItem(const int item)
    {
        if (m_hoveringItem != item)
        {
            if (m_hoveringItem >= 0)
                clearItemStyle(m_hoveringItem);

            m_hoveringItem = item;

            updateSelectedAndHoveringItemColorsAndStyle();
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void PanelListBox::updateSelectedAndHoveringItemColorsAndStyle() const
    {
        if (m_selectedItem >= 0 && m_selectedItemsBackgroundColorCached.isSet())
            m_items[static_cast<std::size_t>(m_selectedItem)].panel->getRenderer()->setBackgroundColor(m_selectedItemsBackgroundColorCached);

        if (m_hoveringItem >= 0)
        {
            if (m_selectedItem == m_hoveringItem && m_selectedItemsBackgroundColorHoverCached.isSet())
            {
                m_items[static_cast<std::size_t>(m_hoveringItem)].panel->getRenderer()->setBackgroundColor(m_selectedItemsBackgroundColorHoverCached);
            }
            else if (m_itemsBackgroundColorHoverCached.isSet())
            {
                m_items[static_cast<std::size_t>(m_hoveringItem)].panel->getRenderer()->setBackgroundColor(m_itemsBackgroundColorHoverCached);
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void PanelListBox::clearItemStyle(const int item) const
    {
        const auto& defaultColor = m_panelTemplate->getSharedRenderer()->getBackgroundColor();
        m_items[static_cast<std::size_t>(item)].panel->getRenderer()->setBackgroundColor(defaultColor);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void PanelListBox::clearAllItemsStyle() const
    {
        for (std::size_t i = 0; i < getItemCount(); ++i)
            clearItemStyle(static_cast<int>(i));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
