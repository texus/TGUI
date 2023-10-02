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


#include <TGUI/Widgets/TreeView.hpp>
#include <TGUI/Keyboard.hpp>

#include <cassert>
#if !TGUI_EXPERIMENTAL_USE_STD_MODULE
    #include <cmath>
#endif

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    namespace
    {
        void setTextSizeImpl(std::vector<std::shared_ptr<TreeView::Node>>& nodes, unsigned int textSize)
        {
            for (auto& node : nodes)
            {
                node->text.setCharacterSize(textSize);
                if (!node->nodes.empty())
                    setTextSizeImpl(node->nodes, textSize);
            }
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        void setTextOpacityImpl(std::vector<std::shared_ptr<TreeView::Node>>& nodes, float opacity)
        {
            for (auto& node : nodes)
            {
                node->text.setOpacity(opacity);
                if (!node->nodes.empty())
                    setTextOpacityImpl(node->nodes, opacity);
            }
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        void setTextFontImpl(std::vector<std::shared_ptr<TreeView::Node>>& nodes, const Font& font)
        {
            for (auto& node : nodes)
            {
                node->text.setFont(font);
                if (!node->nodes.empty())
                    setTextFontImpl(node->nodes, font);
            }
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        std::shared_ptr<TreeView::Node> cloneNode(const std::shared_ptr<TreeView::Node>& oldNode, TreeView::Node* parent)
        {
            auto newNode = std::make_shared<TreeView::Node>();
            newNode->text = oldNode->text;
            newNode->depth = oldNode->depth;
            newNode->expanded = oldNode->expanded;
            newNode->parent = parent;

            for (const auto& oldChild : oldNode->nodes)
                newNode->nodes.push_back(cloneNode(oldChild, newNode.get()));

            return newNode;
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        void cloneVisibleNodeList(const std::vector<std::shared_ptr<TreeView::Node>>& oldNodes,
                                  std::vector<std::shared_ptr<TreeView::Node>>& newNodes,
                                  const std::vector<std::shared_ptr<TreeView::Node>>& oldList,
                                  std::vector<std::shared_ptr<TreeView::Node>>& newList)
        {
            TGUI_ASSERT(oldNodes.size() == newNodes.size(), "Nodes must have been allocated before calling cloneVisibleNodeList");
            TGUI_ASSERT(oldList.size() == newList.size(), "Nodes must have been allocated before calling cloneVisibleNodeList");

            for (unsigned int i = 0; i < oldNodes.size(); ++i)
            {
                for (unsigned int j = 0; j < oldList.size(); ++j)
                {
                    if (oldNodes[i] == oldList[j])
                        newList[j] = newNodes[i];
                }

                cloneVisibleNodeList(oldNodes[i]->nodes, newNodes[i]->nodes, oldList, newList);
            }
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        void expandOrCollapseAll(std::vector<std::shared_ptr<TreeView::Node>>& nodes, bool expandNode)
        {
            for (auto& node : nodes)
            {
                if (!node->nodes.empty())
                {
                    node->expanded = expandNode;
                    expandOrCollapseAll(node->nodes, expandNode);
                }
            }
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        bool removeItemImpl(const std::vector<String>& hierarchy, bool removeParentsWhenEmpty, unsigned int parentIndex, std::vector<std::shared_ptr<TreeView::Node>>& nodes)
        {
            for (auto it = nodes.begin(); it != nodes.end(); ++it)
            {
                if ((*it)->text.getString() != hierarchy[parentIndex])
                    continue;

                if (parentIndex + 1 == hierarchy.size())
                {
                    nodes.erase(it);
                    return true;
                }
                else
                {
                    // Return false if some menu in the hierarchy couldn't be found
                    if (!removeItemImpl(hierarchy, removeParentsWhenEmpty, parentIndex + 1, (*it)->nodes))
                        return false;

                    // If parents don't have to be removed as well then we are done
                    if (!removeParentsWhenEmpty)
                        return true;

                    // Also delete the parent if empty
                    if ((*it)->nodes.empty())
                        nodes.erase(it);

                    return true;
                }
            }

            // The hierarchy doesn't exist
            return false;
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        std::vector<TreeView::ConstNode> convertNodesToConstNodes(const std::vector<std::shared_ptr<TreeView::Node>>& nodes)
        {
            std::vector<TreeView::ConstNode> constNodes;
            for (const auto& node : nodes)
            {
                TreeView::ConstNode constNode;
                constNode.expanded = node->expanded;
                constNode.text = node->text.getString();
                constNode.nodes = convertNodesToConstNodes(node->nodes);
                constNodes.push_back(std::move(constNode));
            }
            return constNodes;
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        TreeView::Node* findNode(const std::vector<std::shared_ptr<TreeView::Node>>& nodes, const std::vector<String>& hierarchy, unsigned int parentIndex)
        {
            for (auto& node : nodes)
            {
                if (node->text.getString() != hierarchy[parentIndex])
                    continue;
                else if (parentIndex + 1 == hierarchy.size())
                    return node.get();
                else
                    return findNode(node->nodes, hierarchy, parentIndex + 1);
            }

            return nullptr;
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        void saveItems(const std::unique_ptr<DataIO::Node>& parentNode, const std::vector<std::shared_ptr<TreeView::Node>>& items)
        {
            for (const auto& item : items)
            {
                auto itemNode = std::make_unique<DataIO::Node>();
                itemNode->name = "Item";

                itemNode->propertyValuePairs[U"Text"] = std::make_unique<DataIO::ValueNode>(Serializer::serialize(item->text.getString()));

                if (!item->nodes.empty())
                {
                    // Save as nested 'Item' sections only when needed, use the more compact string list when just storing the leaf items
                    bool recursionNeeded = false;
                    for (const auto& childItem : item->nodes)
                    {
                        if (!childItem->nodes.empty())
                        {
                            recursionNeeded = true;
                            break;
                        }
                    }

                    if (recursionNeeded)
                        saveItems(itemNode, item->nodes);
                    else
                    {
                        String itemList = "[" + Serializer::serialize(item->nodes[0]->text.getString());
                        for (std::size_t i = 1; i < item->nodes.size(); ++i)
                            itemList += ", " + Serializer::serialize(item->nodes[i]->text.getString());
                        itemList += "]";

                        itemNode->propertyValuePairs[U"Items"] = std::make_unique<DataIO::ValueNode>(itemList);
                    }
                }

                parentNode->children.push_back(std::move(itemNode));
            }
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    }

#if TGUI_COMPILED_WITH_CPP_VER < 17
    constexpr const char TreeView::StaticWidgetType[];
#endif

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    TreeView::TreeView(const char* typeName, bool initRenderer) :
        Widget{typeName, false}
    {
        // Rotate the horizontal scrollbar
        m_horizontalScrollbar->setSize(m_horizontalScrollbar->getSize().y, m_horizontalScrollbar->getSize().x);

        if (initRenderer)
        {
            m_renderer = aurora::makeCopied<TreeViewRenderer>();
            setRenderer(Theme::getDefault()->getRendererNoThrow(m_type));

            setTextSize(getGlobalTextSize());
            setItemHeight(static_cast<unsigned int>(std::round(Text::getLineHeight(m_fontCached, m_textSizeCached) * 1.25f)));
            setSize({Text::getLineHeight(m_fontCached, m_textSizeCached) * 10,
                     (m_itemHeight * 7) + m_paddingCached.getTop() + m_paddingCached.getBottom() + m_bordersCached.getTop() + m_bordersCached.getBottom()});
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    TreeView::TreeView(const TreeView& other) :
        Widget                              {other},
        onItemSelect                        {other.onItemSelect},
        onDoubleClick                       {other.onDoubleClick},
        onExpand                            {other.onExpand},
        onCollapse                          {other.onCollapse},
        onRightClick                        {other.onRightClick},
        m_selectedItem                      {other.m_selectedItem},
        m_hoveredItem                       {other.m_hoveredItem},
        m_itemHeight                        {other.m_itemHeight},
        m_maxRight                          {other.m_maxRight},
        m_iconBounds                        {other.m_iconBounds},
        m_verticalScrollbar                 {other.m_verticalScrollbar},
        m_horizontalScrollbar               {other.m_horizontalScrollbar},
        m_possibleDoubleClick               {other.m_possibleDoubleClick},
        m_doubleClickNodeIndex              {other.m_doubleClickNodeIndex},
        m_spriteBranchExpanded              {other.m_spriteBranchExpanded},
        m_spriteBranchCollapsed             {other.m_spriteBranchCollapsed},
        m_spriteLeaf                        {other.m_spriteLeaf},
        m_bordersCached                     {other.m_bordersCached},
        m_paddingCached                     {other.m_paddingCached},
        m_borderColorCached                 {other.m_borderColorCached},
        m_backgroundColorCached             {other.m_backgroundColorCached},
        m_textColorCached                   {other.m_textColorCached},
        m_textColorHoverCached              {other.m_textColorHoverCached},
        m_selectedTextColorCached           {other.m_selectedTextColorCached},
        m_selectedTextColorHoverCached      {other.m_selectedTextColorHoverCached},
        m_selectedBackgroundColorCached     {other.m_selectedBackgroundColorCached},
        m_selectedBackgroundColorHoverCached{other.m_selectedBackgroundColorHoverCached},
        m_backgroundColorHoverCached        {other.m_backgroundColorHoverCached},
        m_textStyleCached                   {other.m_textStyleCached}
    {
        for (const auto& node : other.m_nodes)
            m_nodes.push_back(cloneNode(node, nullptr));

        m_visibleNodes.resize(other.m_visibleNodes.size(), nullptr);
        cloneVisibleNodeList(other.m_nodes, m_nodes, other.m_visibleNodes, m_visibleNodes);

        TGUI_ASSERT(std::count(m_visibleNodes.begin(), m_visibleNodes.end(), nullptr) == 0, "Cloned nodes can't contain a nullptr");
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    TreeView& TreeView::operator= (const TreeView& other)
    {
        if (this != &other)
        {
            TreeView temp(other);
            Widget::operator=(temp);

            std::swap(onItemSelect,                         temp.onItemSelect);
            std::swap(onDoubleClick,                        temp.onDoubleClick);
            std::swap(onExpand,                             temp.onExpand);
            std::swap(onCollapse,                           temp.onCollapse);
            std::swap(onRightClick,                         temp.onRightClick);
            std::swap(m_nodes,                              temp.m_nodes);
            std::swap(m_visibleNodes,                       temp.m_visibleNodes);
            std::swap(m_selectedItem,                       temp.m_selectedItem);
            std::swap(m_hoveredItem,                        temp.m_hoveredItem);
            std::swap(m_itemHeight,                         temp.m_itemHeight);
            std::swap(m_maxRight,                           temp.m_maxRight);
            std::swap(m_iconBounds,                         temp.m_iconBounds);
            std::swap(m_verticalScrollbar,                  temp.m_verticalScrollbar);
            std::swap(m_horizontalScrollbar,                temp.m_horizontalScrollbar);
            std::swap(m_possibleDoubleClick,                temp.m_possibleDoubleClick);
            std::swap(m_doubleClickNodeIndex,               temp.m_doubleClickNodeIndex);
            std::swap(m_spriteBranchExpanded,               temp.m_spriteBranchExpanded);
            std::swap(m_spriteBranchCollapsed,              temp.m_spriteBranchCollapsed);
            std::swap(m_spriteLeaf,                         temp.m_spriteLeaf);
            std::swap(m_bordersCached,                      temp.m_bordersCached);
            std::swap(m_paddingCached,                      temp.m_paddingCached);
            std::swap(m_borderColorCached,                  temp.m_borderColorCached);
            std::swap(m_backgroundColorCached,              temp.m_backgroundColorCached);
            std::swap(m_textColorCached,                    temp.m_textColorCached);
            std::swap(m_textColorHoverCached,               temp.m_textColorHoverCached);
            std::swap(m_selectedTextColorCached,            temp.m_selectedTextColorCached);
            std::swap(m_selectedTextColorHoverCached,       temp.m_selectedTextColorHoverCached);
            std::swap(m_selectedBackgroundColorCached,      temp.m_selectedBackgroundColorCached);
            std::swap(m_selectedBackgroundColorHoverCached, temp.m_selectedBackgroundColorHoverCached);
            std::swap(m_backgroundColorHoverCached,         temp.m_backgroundColorHoverCached);
            std::swap(m_textStyleCached,                    temp.m_textStyleCached);
        }

        return *this;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    TreeView::Ptr TreeView::create()
    {
        return std::make_shared<TreeView>();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    TreeView::Ptr TreeView::copy(const TreeView::ConstPtr& treeView)
    {
        if (treeView)
            return std::static_pointer_cast<TreeView>(treeView->clone());
        else
            return nullptr;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    TreeViewRenderer* TreeView::getSharedRenderer()
    {
        return aurora::downcast<TreeViewRenderer*>(Widget::getSharedRenderer());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const TreeViewRenderer* TreeView::getSharedRenderer() const
    {
        return aurora::downcast<const TreeViewRenderer*>(Widget::getSharedRenderer());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    TreeViewRenderer* TreeView::getRenderer()
    {
        return aurora::downcast<TreeViewRenderer*>(Widget::getRenderer());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TreeView::setSize(const Layout2d& size)
    {
        Widget::setSize(size);

        updateIconBounds();
        m_bordersCached.updateParentSize(getSize());
        m_paddingCached.updateParentSize(getSize());

        m_spriteBackground.setSize(getInnerSize());

        markNodesDirty();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool TreeView::addItem(const std::vector<String>& hierarchy, bool createParents)
    {
        if (hierarchy.empty())
            return false;

        if (hierarchy.size() >= 2)
        {
            auto* node = findParentNode(hierarchy, 0, m_nodes, nullptr, createParents);
            if (!node)
                return false;

            createNode(node->nodes, node, hierarchy.back());
        }
        else // Root node
            createNode(m_nodes, nullptr, hierarchy.back());

        markNodesDirty();
        return true;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TreeView::expand(const std::vector<String>& hierarchy)
    {
        expandOrCollapse(hierarchy, true);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TreeView::expandAll()
    {
        expandOrCollapseAll(m_nodes, true);
        markNodesDirty();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TreeView::collapse(const std::vector<String>& hierarchy)
    {
        expandOrCollapse(hierarchy, false);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TreeView::collapseAll()
    {
        expandOrCollapseAll(m_nodes, false);
        markNodesDirty();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool TreeView::selectItem(const std::vector<String>& hierarchy)
    {
        // Make sure the parent of the item we are selecting is expanded
        if (hierarchy.size() >= 2)
        {
            if (!expandOrCollapse(std::vector<String>(hierarchy.begin(), hierarchy.end()-1), true))
                return false;
        }

        auto* node = findNode(m_nodes, hierarchy, 0);
        if (!node)
            return false;

        for (std::size_t i = 0; i < m_visibleNodes.size(); ++i)
        {
            if (m_visibleNodes[i].get() == node)
            {
                updateSelectedItem(static_cast<int>(i));
                return true;
            }
        }

        return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TreeView::deselectItem()
    {
        updateSelectedItem(-1);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool TreeView::removeItem(const std::vector<String>& hierarchy, bool removeParentsWhenEmpty)
    {
        const bool ret = removeItemImpl(hierarchy, removeParentsWhenEmpty, 0, m_nodes);
        markNodesDirty();
        return ret;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TreeView::removeAllItems()
    {
        m_nodes.clear();
        markNodesDirty();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::vector<String> TreeView::getSelectedItem() const
    {
        std::vector<String> hierarchy;

        if (m_selectedItem < 0)
            return hierarchy;

        const auto* node = m_visibleNodes[static_cast<std::size_t>(m_selectedItem)].get();
        assert(node != nullptr);
        while (node)
        {
            hierarchy.insert(hierarchy.begin(), node->text.getString());
            node = node->parent;
        }

        return hierarchy;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TreeView::toggleNodeInternal(std::size_t index)
    {
        if (index >= m_visibleNodes.size())
            return;

        if (m_visibleNodes[index]->nodes.empty())
            return;

        std::vector<String> hierarchy;
        auto* node = m_visibleNodes[index].get();
        assert(node != nullptr);
        while (node)
        {
            hierarchy.insert(hierarchy.begin(), node->text.getString());
            node = node->parent;
        }

        m_visibleNodes[index]->expanded = !m_visibleNodes[index]->expanded;
        if (m_visibleNodes[index]->expanded)
            onExpand.emit(this, hierarchy.back(), hierarchy);
        else
            onCollapse.emit(this, hierarchy.back(), hierarchy);

        markNodesDirty();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Vector2f TreeView::getInnerSize() const
    {
        return {std::max(0.f, getSize().x - m_bordersCached.getLeft() - m_bordersCached.getRight()),
                std::max(0.f, getSize().y - m_bordersCached.getTop() - m_bordersCached.getBottom())};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TreeView::updateIconBounds()
    {
        if (m_spriteBranchCollapsed.isSet() || m_spriteBranchExpanded.isSet() || m_spriteLeaf.isSet())
        {
            m_iconBounds =
                {
                    std::max(std::max(m_spriteBranchCollapsed.getSize().x, m_spriteBranchExpanded.getSize().x), m_spriteLeaf.getSize().x),
                    std::max(std::max(m_spriteBranchCollapsed.getSize().y, m_spriteBranchExpanded.getSize().y), m_spriteLeaf.getSize().y)
                };
        }
        else
        {
            const float wantedIconSize = std::round(m_itemHeight / 2.f);
            const float lineThickness = std::round(m_itemHeight / 10.f);
            const float iconSize = (std::floor((wantedIconSize - lineThickness) / 2.f) * 2.f) + lineThickness; // "+" sign should be symmetric
            m_iconBounds = {iconSize, iconSize};
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::vector<TreeView::ConstNode> TreeView::getNodes() const
    {
        return convertNodesToConstNodes(m_nodes);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TreeView::setItemHeight(unsigned int itemHeight)
    {
        m_itemHeight = itemHeight;
        if ((m_textSize == 0) && !getSharedRenderer()->getTextSize())
            updateTextSize();

        m_verticalScrollbar->setScrollAmount(m_itemHeight);
        m_horizontalScrollbar->setScrollAmount(m_itemHeight);
        markNodesDirty();
        updateIconBounds();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int TreeView::getItemHeight() const
    {
        return m_itemHeight;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TreeView::updateTextSize()
    {
        if ((m_textSize == 0) && !getSharedRenderer()->getTextSize())
            m_textSizeCached = Text::findBestTextSize(m_fontCached, m_itemHeight * 0.8f);

        setTextSizeImpl(m_nodes, m_textSizeCached);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TreeView::setVerticalScrollbarValue(unsigned int value)
    {
        m_verticalScrollbar->setValue(value);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int TreeView::getVerticalScrollbarValue() const
    {
        return m_verticalScrollbar->getValue();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TreeView::setHorizontalScrollbarValue(unsigned int value)
    {
        m_horizontalScrollbar->setValue(value);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int TreeView::getHorizontalScrollbarValue() const
    {
        return m_horizontalScrollbar->getValue();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool TreeView::isMouseOnWidget(Vector2f pos) const
    {
        if (FloatRect{getPosition().x, getPosition().y, getSize().x, getSize().y}.contains(pos))
        {
            if (!m_transparentTextureCached || !m_spriteBackground.isSet() || !m_spriteBackground.isTransparentPixel(pos - getPosition() - m_bordersCached.getOffset()))
                return true;
        }

        return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool TreeView::leftMousePressed(Vector2f pos)
    {
        Widget::leftMousePressed(pos);

        pos -= getPosition();

        bool isDragging = false;
        if (m_verticalScrollbar->isMouseOnWidget(pos))
            isDragging = m_verticalScrollbar->leftMousePressed(pos);
        else if (m_horizontalScrollbar->isMouseOnWidget(pos))
            isDragging = m_horizontalScrollbar->leftMousePressed(pos);
        else
        {
            float maxItemWidth = getInnerSize().x - m_paddingCached.getLeft() - m_paddingCached.getRight();
            if (m_verticalScrollbar->isShown())
                maxItemWidth -= m_verticalScrollbar->getSize().x;

            if (FloatRect{m_bordersCached.getLeft() + m_paddingCached.getLeft(), m_bordersCached.getTop() + m_paddingCached.getTop(),
                          maxItemWidth, getInnerSize().y - m_paddingCached.getTop() - m_paddingCached.getBottom()}.contains(pos))
            {
                pos.y -= m_bordersCached.getTop() + m_paddingCached.getTop();

                // NOLINTNEXTLINE(bugprone-integer-division)
                int selectedItem = static_cast<int>(((pos.y - (m_itemHeight - (m_verticalScrollbar->getValue() % m_itemHeight))) / m_itemHeight) + (m_verticalScrollbar->getValue() / m_itemHeight) + 1);
                if (selectedItem >= static_cast<int>(m_visibleNodes.size()))
                    selectedItem = -1;

                updateSelectedItem(selectedItem);
            }
        }

        return isDragging;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TreeView::leftMouseReleased(Vector2f pos)
    {
        pos -= getPosition();
        auto childPos = pos;
        if (m_mouseDown && !m_verticalScrollbar->isMouseDown() && !m_horizontalScrollbar->isMouseDown())
        {
            m_mouseDown = false;

            float maxItemWidth = getInnerSize().x - m_paddingCached.getLeft() - m_paddingCached.getRight();
            if (m_verticalScrollbar->isShown())
                maxItemWidth -= m_verticalScrollbar->getSize().x;

            bool iconPressed = false;
            int selectedIndex = -1;
            if (FloatRect{m_bordersCached.getLeft() + m_paddingCached.getLeft(), m_bordersCached.getTop() + m_paddingCached.getTop(),
                          maxItemWidth, getInnerSize().y - m_paddingCached.getTop() - m_paddingCached.getBottom()}.contains(pos))
            {
                pos.y -= m_bordersCached.getTop() + m_paddingCached.getTop();

                // NOLINTNEXTLINE(bugprone-integer-division)
                selectedIndex = static_cast<int>(((pos.y - (m_itemHeight - (m_verticalScrollbar->getValue() % m_itemHeight))) / m_itemHeight) + (m_verticalScrollbar->getValue() / m_itemHeight) + 1);
                if ((selectedIndex >= 0) && (selectedIndex == m_selectedItem))
                {
                    // Expand or colapse the node when clicking the icon
                    const float iconPaddingX = (m_iconBounds.x / 4.f);
                    const float iconOffsetX = iconPaddingX + ((m_iconBounds.x + iconPaddingX) * m_visibleNodes[static_cast<std::size_t>(selectedIndex)]->depth);
                    const float iconOffsetY = (m_itemHeight - m_iconBounds.y) / 2.f;
                    if (FloatRect{iconOffsetX + m_bordersCached.getLeft() + m_paddingCached.getLeft() - m_horizontalScrollbar->getValue(),
                                  iconOffsetY + (static_cast<unsigned int>(selectedIndex) * m_itemHeight) + m_bordersCached.getTop() + m_paddingCached.getTop() - m_verticalScrollbar->getValue(),
                                  m_iconBounds.x,
                                  m_iconBounds.y}.contains(pos))
                    {
                        toggleNodeInternal(static_cast<std::size_t>(selectedIndex));
                        m_possibleDoubleClick = false;
                        iconPressed = true;
                    }
                }
            }

            if (m_possibleDoubleClick)
            {
                m_possibleDoubleClick = false;

                if ((selectedIndex >= 0) && (selectedIndex == m_doubleClickNodeIndex) && (selectedIndex < static_cast<int>(m_visibleNodes.size())))
                {
                    toggleNodeInternal(static_cast<std::size_t>(selectedIndex));

                    // Send double click if this was a leaf node
                    if (m_visibleNodes[static_cast<std::size_t>(selectedIndex)]->nodes.empty())
                    {
                        std::vector<String> hierarchy;
                        auto* node = m_visibleNodes[static_cast<std::size_t>(selectedIndex)].get();
                        assert(node != nullptr);
                        while (node)
                        {
                            hierarchy.insert(hierarchy.begin(), node->text.getString());
                            node = node->parent;
                        }

                        onDoubleClick.emit(this, hierarchy.back(), hierarchy);
                    }
                }
            }
            else if (!iconPressed)
            {
                m_animationTimeElapsed = {};
                m_possibleDoubleClick = true;
                m_doubleClickNodeIndex = selectedIndex;
            }
        }

        m_verticalScrollbar->leftMouseReleased(childPos);
        m_horizontalScrollbar->leftMouseReleased(childPos);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TreeView::rightMousePressed(Vector2f pos)
    {
        pos -= getPosition();

        float maxItemWidth = getInnerSize().x - m_paddingCached.getLeft() - m_paddingCached.getRight();
        if (m_verticalScrollbar->isShown())
            maxItemWidth -= m_verticalScrollbar->getSize().x;

        if (FloatRect{m_bordersCached.getLeft() + m_paddingCached.getLeft(), m_bordersCached.getTop() + m_paddingCached.getTop(),
                      maxItemWidth, getInnerSize().y - m_paddingCached.getTop() - m_paddingCached.getBottom()}.contains(pos))
        {
            pos.y -= m_bordersCached.getTop() + m_paddingCached.getTop();

            // NOLINTNEXTLINE(bugprone-integer-division)
            int selectedItem = static_cast<int>(((pos.y - (m_itemHeight - (m_verticalScrollbar->getValue() % m_itemHeight))) / m_itemHeight) + (m_verticalScrollbar->getValue() / m_itemHeight) + 1);
            if ((selectedItem >= 0) && (selectedItem < static_cast<int>(m_visibleNodes.size())))
            {
                updateSelectedItem(selectedItem);

                std::vector<String> hierarchy;
                auto* node = m_visibleNodes[static_cast<std::size_t>(selectedItem)].get();
                assert(node != nullptr);
                while (node)
                {
                    hierarchy.insert(hierarchy.begin(), node->text.getString());
                    node = node->parent;
                }

                onRightClick.emit(this, hierarchy.back(), hierarchy);
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TreeView::mouseMoved(Vector2f pos)
    {
        pos -= getPosition();

        if (!m_mouseHover)
            mouseEnteredWidget();

        if ((m_verticalScrollbar->isMouseDown() && m_verticalScrollbar->isMouseDownOnThumb()) || m_verticalScrollbar->isMouseOnWidget(pos))
            m_verticalScrollbar->mouseMoved(pos);
        else if ((m_horizontalScrollbar->isMouseDown() && m_horizontalScrollbar->isMouseDownOnThumb()) || m_horizontalScrollbar->isMouseOnWidget(pos))
            m_horizontalScrollbar->mouseMoved(pos);
        else
        {
            m_verticalScrollbar->mouseNoLongerOnWidget();
            m_horizontalScrollbar->mouseNoLongerOnWidget();

            float maxItemWidth = getInnerSize().x - m_paddingCached.getLeft() - m_paddingCached.getRight();
            if (m_verticalScrollbar->isShown())
                maxItemWidth -= m_verticalScrollbar->getSize().x;

            if (FloatRect{m_bordersCached.getLeft() + m_paddingCached.getLeft(), m_bordersCached.getTop() + m_paddingCached.getTop(),
                          maxItemWidth, getInnerSize().y - m_paddingCached.getTop() - m_paddingCached.getBottom()}.contains(pos))
            {
                pos.y -= m_bordersCached.getTop() + m_paddingCached.getTop();

                // NOLINTNEXTLINE(bugprone-integer-division)
                int hoveredItem = static_cast<int>(((pos.y - (m_itemHeight - (m_verticalScrollbar->getValue() % m_itemHeight))) / m_itemHeight) + (m_verticalScrollbar->getValue() / m_itemHeight) + 1);
                if (hoveredItem >= static_cast<int>(m_visibleNodes.size()))
                    hoveredItem = -1;

                updateHoveredItem(hoveredItem);
            }
            else // Mouse is on top of padding or borders
                updateHoveredItem(-1);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool TreeView::scrolled(float delta, Vector2f pos, bool touch)
    {
        bool scrollbarMoved = false;
        if (m_horizontalScrollbar->isShown()
         && !touch
         && (!m_verticalScrollbar->isShown() || m_horizontalScrollbar->isMouseOnWidget(pos - getPosition()) || keyboard::isShiftPressed()))
        {
            scrollbarMoved = m_horizontalScrollbar->scrolled(delta, pos - getPosition(), touch);
        }
        else if (m_verticalScrollbar->isShown())
        {
            scrollbarMoved = m_verticalScrollbar->scrolled(delta, pos - getPosition(), touch);
        }

        if (scrollbarMoved)
            mouseMoved(pos);

        return scrollbarMoved;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TreeView::mouseNoLongerOnWidget()
    {
        Widget::mouseNoLongerOnWidget();
        m_verticalScrollbar->mouseNoLongerOnWidget();
        m_horizontalScrollbar->mouseNoLongerOnWidget();

        updateHoveredItem(-1);
        m_possibleDoubleClick = false;
        updateSelectedAndHoveringItemColors();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TreeView::leftMouseButtonNoLongerDown()
    {
        Widget::leftMouseButtonNoLongerDown();
        m_verticalScrollbar->leftMouseButtonNoLongerDown();
        m_horizontalScrollbar->leftMouseButtonNoLongerDown();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TreeView::keyPressed(const Event::KeyEvent& event)
    {
        // Don't do anything when no item is selected
        if (m_selectedItem < 0)
            return;

        const auto selectedItemIdx = static_cast<std::size_t>(m_selectedItem);
        if (event.code == Event::KeyboardKey::Up)
        {
            // Select the item above
            if (m_selectedItem > 0)
                updateSelectedItem(m_selectedItem - 1);
        }
        else if (event.code == Event::KeyboardKey::Down)
        {
            // Select the item below
            if (selectedItemIdx + 1 < m_visibleNodes.size())
                updateSelectedItem(m_selectedItem + 1);
        }
        else if (event.code == Event::KeyboardKey::Left)
        {
            // If item is an expanded node then collapse it. Otherwise select the parent node, or the previous sibling node if it has no parent.
            TGUI_ASSERT(selectedItemIdx <= m_visibleNodes.size(), "Selected item index has to be in range");
            if (!m_visibleNodes[selectedItemIdx]->nodes.empty() && m_visibleNodes[selectedItemIdx]->expanded)
            {
                m_visibleNodes[selectedItemIdx]->expanded = false;
                markNodesDirty();
            }
            else if (m_visibleNodes[selectedItemIdx]->parent)
            {
                for (std::size_t i = 0; i < m_visibleNodes.size(); ++i)
                {
                    if (m_visibleNodes[i].get() == m_visibleNodes[selectedItemIdx]->parent)
                    {
                        updateSelectedItem(static_cast<int>(i));
                        break;
                    }
                }
            }
            else if (m_selectedItem > 0)
            {
                std::size_t nodeIndex = 0;
                for (std::size_t i = 0; i < m_nodes.size(); ++i)
                {
                    if (m_nodes[i] == m_visibleNodes[selectedItemIdx])
                    {
                        nodeIndex = i;
                        break;
                    }
                }

                TGUI_ASSERT(nodeIndex > 0, "Index can't be 0 as this is not the top item");
                for (std::size_t i = 0; i < m_visibleNodes.size(); ++i)
                {
                    if (m_visibleNodes[i] == m_nodes[nodeIndex - 1])
                    {
                        updateSelectedItem(static_cast<int>(i));
                        break;
                    }
                }
            }
        }
        else if (event.code == Event::KeyboardKey::Right)
        {
            // If item is a collapsed node then expand it. Otherwise simply select the next item.
            TGUI_ASSERT(selectedItemIdx <= m_visibleNodes.size(), "Selected item index has to be in range");
            if (!m_visibleNodes[selectedItemIdx]->nodes.empty() && !m_visibleNodes[selectedItemIdx]->expanded)
            {
                m_visibleNodes[selectedItemIdx]->expanded = true;
                markNodesDirty();
            }
            else if (selectedItemIdx + 1 < m_visibleNodes.size())
                updateSelectedItem(m_selectedItem + 1);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool TreeView::canHandleKeyPress(const Event::KeyEvent& event)
    {
        if ((event.code == Event::KeyboardKey::Up)
         || (event.code == Event::KeyboardKey::Down)
         || (event.code == Event::KeyboardKey::Left)
         || (event.code == Event::KeyboardKey::Right))
        {
            return true;
        }
        else
            return Widget::canHandleKeyPress(event);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Signal& TreeView::getSignal(String signalName)
    {
        if (signalName == onItemSelect.getName())
            return onItemSelect;
        else if (signalName == onDoubleClick.getName())
            return onDoubleClick;
        else if (signalName == onExpand.getName())
            return onExpand;
        else if (signalName == onCollapse.getName())
            return onCollapse;
        else if (signalName == onRightClick.getName())
            return onRightClick;
        else
            return Widget::getSignal(std::move(signalName));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TreeView::rendererChanged(const String& property)
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
        else if (property == U"BackgroundColor")
        {
            m_backgroundColorCached = getSharedRenderer()->getBackgroundColor();
        }
        else if (property == U"SelectedBackgroundColor")
        {
            m_selectedBackgroundColorCached = getSharedRenderer()->getSelectedBackgroundColor();
        }
        else if (property == U"BackgroundColorHover")
        {
            m_backgroundColorHoverCached = getSharedRenderer()->getBackgroundColorHover();
        }
        else if (property == U"SelectedBackgroundColorHover")
        {
            m_selectedBackgroundColorHoverCached = getSharedRenderer()->getSelectedBackgroundColorHover();
        }
        else if (property == U"BorderColor")
        {
            m_borderColorCached = getSharedRenderer()->getBorderColor();
        }
        else if (property == U"TextureBackground")
        {
            m_spriteBackground.setTexture(getSharedRenderer()->getTextureBackground());
        }
        else if (property == U"TextureBranchExpanded")
        {
            m_spriteBranchExpanded.setTexture(getSharedRenderer()->getTextureBranchExpanded());
            updateIconBounds();
            markNodesDirty();
        }
        else if (property == U"TextureBranchCollapsed")
        {
            m_spriteBranchCollapsed.setTexture(getSharedRenderer()->getTextureBranchCollapsed());
            updateIconBounds();
            markNodesDirty();
        }
        else if (property == U"TextureLeaf")
        {
            m_spriteLeaf.setTexture(getSharedRenderer()->getTextureLeaf());
            updateIconBounds();
            markNodesDirty();
        }
        else if (property == U"TextColor")
        {
            m_textColorCached = getSharedRenderer()->getTextColor();
            updateTextColors(m_nodes);
            updateSelectedAndHoveringItemColors();
        }
        else if (property == U"TextColorHover")
        {
            m_textColorHoverCached = getSharedRenderer()->getTextColorHover();
            updateTextColors(m_nodes);
            updateSelectedAndHoveringItemColors();
        }
        else if (property == U"SelectedTextColor")
        {
            m_selectedTextColorCached = getSharedRenderer()->getSelectedTextColor();
            updateTextColors(m_nodes);
            updateSelectedAndHoveringItemColors();
        }
        else if (property == U"SelectedTextColorHover")
        {
            m_selectedTextColorHoverCached = getSharedRenderer()->getSelectedTextColorHover();
            updateTextColors(m_nodes);
            updateSelectedAndHoveringItemColors();
        }
        else if (property == U"Scrollbar")
        {
            m_verticalScrollbar->setRenderer(getSharedRenderer()->getScrollbar());
            m_horizontalScrollbar->setRenderer(getSharedRenderer()->getScrollbar());

            // If no scrollbar width was set then we may need to use the one from the texture
            if (getSharedRenderer()->getScrollbarWidth() == 0)
            {
                const float width = m_verticalScrollbar->getDefaultWidth();
                m_verticalScrollbar->setSize({width, m_verticalScrollbar->getSize().y});
                m_horizontalScrollbar->setSize({m_horizontalScrollbar->getSize().x, width});
                markNodesDirty();
            }
        }
        else if (property == U"ScrollbarWidth")
        {
            const float width = (getSharedRenderer()->getScrollbarWidth() != 0) ? getSharedRenderer()->getScrollbarWidth() : m_verticalScrollbar->getDefaultWidth();
            m_verticalScrollbar->setSize({width, m_verticalScrollbar->getSize().y});
            m_horizontalScrollbar->setSize({m_horizontalScrollbar->getSize().x, width});
            markNodesDirty();
        }
        else if ((property == U"Opacity") || (property == U"OpacityDisabled"))
        {
            Widget::rendererChanged(property);

            setTextOpacityImpl(m_nodes, m_opacityCached);

            m_spriteBackground.setOpacity(m_opacityCached);
            m_spriteBranchExpanded.setOpacity(m_opacityCached);
            m_spriteBranchCollapsed.setOpacity(m_opacityCached);
            m_spriteLeaf.setOpacity(m_opacityCached);

            m_verticalScrollbar->setInheritedOpacity(m_opacityCached);
            m_horizontalScrollbar->setInheritedOpacity(m_opacityCached);
        }
        else if (property == U"Font")
        {
            Widget::rendererChanged(property);
            setTextFontImpl(m_nodes, m_fontCached);
        }
        else
            Widget::rendererChanged(property);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::unique_ptr<DataIO::Node> TreeView::save(SavingRenderersMap& renderers) const
    {
        auto node = Widget::save(renderers);
        node->propertyValuePairs[U"ItemHeight"] = std::make_unique<DataIO::ValueNode>(String::fromNumber(m_itemHeight));
        saveItems(node, m_nodes);
        return node;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TreeView::load(const std::unique_ptr<DataIO::Node>& node, const LoadingRenderersMap& renderers)
    {
        Widget::load(node, renderers);

        if (node->propertyValuePairs[U"ItemHeight"])
            setItemHeight(node->propertyValuePairs[U"ItemHeight"]->value.toUInt());

        loadItems(node, m_nodes, nullptr);

        // Remove the 'Item' nodes as they have been processed
        node->children.erase(std::remove_if(node->children.begin(), node->children.end(),
            [](const std::unique_ptr<DataIO::Node>& child){ return child->name == U"Item"; }), node->children.end());

        // Update the visible nodes
        markNodesDirty();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool TreeView::updateTime(Duration elapsedTime)
    {
        const bool screenRefreshRequired = Widget::updateTime(elapsedTime);

        if (m_animationTimeElapsed >= getDoubleClickTime())
        {
            m_animationTimeElapsed = {};
            m_possibleDoubleClick = false;
        }

        return screenRefreshRequired;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TreeView::loadItems(const std::unique_ptr<DataIO::Node>& node, std::vector<std::shared_ptr<Node>>& items, Node* parent)
    {
        for (const auto& childNode : node->children)
        {
            if (childNode->name != U"Item")
                continue;

            if (!childNode->propertyValuePairs[U"Text"])
                throw Exception{U"Failed to parse 'Item' property, expected a nested 'Text' propery"};

            const String itemText = Deserializer::deserialize(ObjectConverter::Type::String, childNode->propertyValuePairs[U"Text"]->value).getString();
            createNode(items, parent, itemText);

            // Recursively handle the menu nodes
            if (!childNode->children.empty())
                loadItems(childNode, items.back()->nodes, items.back().get());

            // Menu items can also be stored in an string array in the 'Items' property instead of as a nested Menu section
            if (childNode->propertyValuePairs[U"Items"])
            {
                if (!childNode->propertyValuePairs[U"Items"]->listNode)
                    throw Exception{U"Failed to parse 'Items' property inside 'Item' property, expected a list as value"};

                for (std::size_t i = 0; i < childNode->propertyValuePairs[U"Items"]->valueList.size(); ++i)
                {
                    const String subItemText = Deserializer::deserialize(ObjectConverter::Type::String, childNode->propertyValuePairs[U"Items"]->valueList[i]).getString();
                    createNode(items.back()->nodes, items.back().get(), subItemText);
                }
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int TreeView::updateVisibleNodes(std::vector<std::shared_ptr<Node>>& nodes, Node* selectedNode, float textPadding, unsigned int pos)
    {
        for (auto& node : nodes)
        {
            m_visibleNodes.push_back(node);
            if (selectedNode == node.get())
                m_selectedItem = static_cast<int>(pos);

            const float iconPadding = (m_iconBounds.x / 4.f);
            const float iconOffset = iconPadding + ((m_iconBounds.x + iconPadding) * node->depth);
            node->text.setPosition({iconOffset + m_iconBounds.x + iconPadding + textPadding,
                                    (pos * m_itemHeight) + ((m_itemHeight - node->text.getSize().y) / 2.f)});

            const float right = node->text.getPosition().x + node->text.getSize().x + m_paddingCached.getRight();
            if (right > m_maxRight)
                m_maxRight = right;

            pos++;
            if (node->expanded && !node->nodes.empty())
                pos = updateVisibleNodes(node->nodes, selectedNode, textPadding, pos);
        }

        return pos;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TreeView::markNodesDirty()
    {
        Node* selectedNode = nullptr;
        if (m_selectedItem >= 0 && static_cast<std::size_t>(m_selectedItem) < m_visibleNodes.size())
            selectedNode = m_visibleNodes[static_cast<std::size_t>(m_selectedItem)].get();

        int oldHoveredItem = m_hoveredItem;

        m_maxRight = 0;
        m_hoveredItem = -1;
        m_selectedItem = -1;
        m_visibleNodes.clear();
        updateVisibleNodes(m_nodes, selectedNode, Text::getExtraHorizontalPadding(m_fontCached, m_textSizeCached), 0);

        if (oldHoveredItem >= 0)
        {
            if (static_cast<std::size_t>(oldHoveredItem) < m_visibleNodes.size())
                m_hoveredItem = oldHoveredItem;

            updateTextColors(m_nodes);
            updateSelectedAndHoveringItemColors();
        }

        m_verticalScrollbar->setMaximum(static_cast<unsigned int>(m_itemHeight * m_visibleNodes.size()));
        m_horizontalScrollbar->setMaximum(static_cast<unsigned int>(m_maxRight));

        if ((m_maxRight + m_verticalScrollbar->getSize().x) > (getInnerSize().x - m_paddingCached.getLeft() - m_paddingCached.getRight()))
        {
            m_verticalScrollbar->setSize({m_verticalScrollbar->getSize().x, std::max(0.f, getInnerSize().y - m_horizontalScrollbar->getSize().y)});
            m_verticalScrollbar->setViewportSize(static_cast<unsigned int>(getInnerSize().y - m_horizontalScrollbar->getSize().y - m_paddingCached.getTop() - m_paddingCached.getBottom()));
        }
        else
        {
            m_verticalScrollbar->setSize({m_verticalScrollbar->getSize().x, std::max(0.f, getInnerSize().y)});
            m_verticalScrollbar->setViewportSize(static_cast<unsigned int>(getInnerSize().y - m_paddingCached.getTop() - m_paddingCached.getBottom()));
        }

        if (m_verticalScrollbar->isShown())
        {
            m_horizontalScrollbar->setSize({std::max(0.f, getInnerSize().x - m_verticalScrollbar->getSize().x), m_horizontalScrollbar->getSize().y});
            m_horizontalScrollbar->setViewportSize(static_cast<unsigned int>(getInnerSize().x - m_verticalScrollbar->getSize().x - m_paddingCached.getLeft() - m_paddingCached.getRight()));
        }
        else
        {
            m_horizontalScrollbar->setSize({std::max(0.f, getInnerSize().x), m_horizontalScrollbar->getSize().y});
            m_horizontalScrollbar->setViewportSize(static_cast<unsigned int>(getInnerSize().x - m_paddingCached.getLeft() - m_paddingCached.getRight()));
        }

        // If the horizontal scrollbar is shown then add a little padding at the end of the longest line
        if (m_horizontalScrollbar->isShown())
            m_horizontalScrollbar->setMaximum(static_cast<unsigned int>(m_horizontalScrollbar->getMaximum() + (m_iconBounds.x / 4.f)));

        m_verticalScrollbar->setPosition(getSize().x - m_bordersCached.getRight() - m_verticalScrollbar->getSize().x, m_bordersCached.getTop());
        m_horizontalScrollbar->setPosition(m_bordersCached.getLeft(), getSize().y - m_bordersCached.getBottom() - m_horizontalScrollbar->getSize().y);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TreeView::draw(BackendRenderTarget& target, RenderStates states) const
    {
        RenderStates statesForScrollbars = states;

        if (m_bordersCached != Borders{0})
        {
            target.drawBorders(states, m_bordersCached, getSize(), Color::applyOpacity(m_borderColorCached, m_opacityCached));
            states.transform.translate(m_bordersCached.getOffset());
        }

        // Draw the background
        if (m_spriteBackground.isSet())
            target.drawSprite(states, m_spriteBackground);
        else
            target.drawFilledRect(states, getInnerSize(), Color::applyOpacity(m_backgroundColorCached, m_opacityCached));

        float maxItemWidth = getInnerSize().x - m_paddingCached.getLeft() - m_paddingCached.getRight();
        if (m_verticalScrollbar->isShown())
            maxItemWidth -= m_verticalScrollbar->getSize().x;

        target.addClippingLayer(states, {{m_paddingCached.getLeft(), m_paddingCached.getTop()},
            {maxItemWidth, getInnerSize().y - m_paddingCached.getTop() - m_paddingCached.getBottom()}});

        std::size_t firstNode = 0;
        std::size_t lastNode = m_visibleNodes.size();
        if (m_verticalScrollbar->getViewportSize() < m_verticalScrollbar->getMaximum())
        {
            firstNode = m_verticalScrollbar->getValue() / m_itemHeight;
            lastNode = (m_verticalScrollbar->getValue() + m_verticalScrollbar->getViewportSize()) / m_itemHeight;

            // Show another item when the scrollbar is standing between two items
            if ((m_verticalScrollbar->getValue() + m_verticalScrollbar->getViewportSize()) % m_itemHeight != 0)
                ++lastNode;
        }

        states.transform.translate({m_paddingCached.getLeft() - m_horizontalScrollbar->getValue(), m_paddingCached.getTop() - m_verticalScrollbar->getValue()});

        // Draw the background of the selected item
        if ((m_selectedItem >= static_cast<int>(firstNode)) && (m_selectedItem < static_cast<int>(lastNode)))
        {
            states.transform.translate({static_cast<float>(m_horizontalScrollbar->getValue()), m_selectedItem * static_cast<float>(m_itemHeight)});

            const Vector2f size = {getInnerSize().x - m_paddingCached.getLeft() - m_paddingCached.getRight(), static_cast<float>(m_itemHeight)};
            if ((m_selectedItem == m_hoveredItem) && m_selectedBackgroundColorHoverCached.isSet())
                target.drawFilledRect(states, size, Color::applyOpacity(m_selectedBackgroundColorHoverCached, m_opacityCached));
            else
                target.drawFilledRect(states, size, Color::applyOpacity(m_selectedBackgroundColorCached, m_opacityCached));

            states.transform.translate({-static_cast<float>(m_horizontalScrollbar->getValue()), -m_selectedItem * static_cast<float>(m_itemHeight)});
        }

        // Draw the background of the item on which the mouse is standing
        if ((m_hoveredItem >= static_cast<int>(firstNode)) && (m_hoveredItem < static_cast<int>(lastNode)) && (m_hoveredItem != m_selectedItem) && m_backgroundColorHoverCached.isSet())
        {
            states.transform.translate({static_cast<float>(m_horizontalScrollbar->getValue()), m_hoveredItem * static_cast<float>(m_itemHeight)});
            target.drawFilledRect(states, {getInnerSize().x - m_paddingCached.getLeft() - m_paddingCached.getRight(), static_cast<float>(m_itemHeight)}, Color::applyOpacity(m_backgroundColorHoverCached, m_opacityCached));
            states.transform.translate({-static_cast<float>(m_horizontalScrollbar->getValue()), -m_hoveredItem * static_cast<float>(m_itemHeight)});
        }

        // Draw the icons
        for (std::size_t i = firstNode; i < lastNode; ++i)
        {
            auto statesForIcon = states;
            const float iconPadding = (m_iconBounds.x / 4.f);
            const float iconOffset = iconPadding + ((m_iconBounds.x + iconPadding) * m_visibleNodes[i]->depth);
            statesForIcon.transform.translate({std::round(iconOffset), std::round((i * m_itemHeight) + ((m_itemHeight - m_iconBounds.y) / 2.f))});

            // Draw an icon for the leaf node if a texture is set
            if (m_visibleNodes[i]->nodes.empty())
            {
                if (m_spriteLeaf.isSet())
                    target.drawSprite(statesForIcon, m_spriteLeaf);
            }
            else // Branch node
            {
                if (m_spriteLeaf.isSet() || m_spriteBranchExpanded.isSet() || m_spriteBranchCollapsed.isSet())
                {
                    const Sprite* iconSprite = nullptr;
                    if (m_visibleNodes[i]->expanded)
                    {
                        if (m_spriteBranchExpanded.isSet())
                            iconSprite = &m_spriteBranchExpanded;
                        else if (m_spriteBranchCollapsed.isSet())
                            iconSprite = &m_spriteBranchCollapsed;
                        else
                            iconSprite = &m_spriteLeaf;
                    }
                    else // Collapsed node
                    {
                        if (m_spriteBranchCollapsed.isSet())
                            iconSprite = &m_spriteBranchCollapsed;
                        else if (m_spriteBranchExpanded.isSet())
                            iconSprite = &m_spriteBranchExpanded;
                        else
                            iconSprite = &m_spriteLeaf;
                    }

                    target.drawSprite(statesForIcon, *iconSprite);
                }
                else // No textures are used
                {
                    Color iconColor = m_textColorCached;
                    if (static_cast<int>(i) == m_selectedItem)
                    {
                        if ((m_selectedItem == m_hoveredItem) && m_selectedTextColorHoverCached.isSet())
                            iconColor = m_selectedTextColorHoverCached;
                        else if (m_selectedTextColorCached.isSet())
                            iconColor = m_selectedTextColorCached;
                    }
                    if ((static_cast<int>(i) == m_hoveredItem) && (m_selectedItem != m_hoveredItem))
                    {
                        if (m_textColorHoverCached.isSet())
                            iconColor = m_textColorHoverCached;
                    }

                    const float thickness = std::max(1.f, std::round(m_itemHeight / 10.f));
                    if (m_visibleNodes[i]->expanded)
                    {
                        // Draw "-"
                        statesForIcon.transform.translate({0, (m_iconBounds.y - thickness) / 2.f});
                        target.drawFilledRect(statesForIcon, {m_iconBounds.x, thickness}, Color::applyOpacity(iconColor, m_opacityCached));
                    }
                    else // Collapsed node
                    {
                        // Draw "+"
                        statesForIcon.transform.translate({0, (m_iconBounds.y - thickness) / 2.f});
                        target.drawFilledRect(statesForIcon, {m_iconBounds.x, thickness}, Color::applyOpacity(iconColor, m_opacityCached));
                        statesForIcon.transform.translate({(m_iconBounds.x - thickness) / 2.f, -(m_iconBounds.y - thickness) / 2.f});
                        target.drawFilledRect(statesForIcon, {thickness, m_iconBounds.y}, Color::applyOpacity(iconColor, m_opacityCached));
                    }
                }
            }
        }

        // Draw the texts
        for (std::size_t i = firstNode; i < lastNode; ++i)
            target.drawText(states, m_visibleNodes[i]->text);

        target.removeClippingLayer();

        m_horizontalScrollbar->draw(target, statesForScrollbars);
        m_verticalScrollbar->draw(target, statesForScrollbars);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TreeView::createNode(std::vector<std::shared_ptr<Node>>& nodes, Node* parent, const String& text)
    {
        auto newNode = std::make_shared<Node>();
        newNode->text.setFont(m_fontCached);
        newNode->text.setColor(m_textColorCached);
        newNode->text.setOpacity(m_opacityCached);
        newNode->text.setCharacterSize(m_textSizeCached);
        newNode->text.setString(text);
        newNode->expanded = true;
        newNode->parent = parent;

        if (parent)
            newNode->depth = parent->depth + 1;
        else
            newNode->depth = 0;

        nodes.push_back(std::move(newNode));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool TreeView::expandOrCollapse(const std::vector<String>& hierarchy, bool expandNode)
    {
        if (hierarchy.empty())
            return false;

        if (hierarchy.size() >= 2)
        {
            auto* node = findNode(m_nodes, hierarchy, 0);
            if (!node)
                return false;

            bool nodeChanged = false;
            if (expandNode)
            {
                // When expanding, also expand all parents
                auto* nodeToExpand = node;
                while (nodeToExpand)
                {
                    if (nodeToExpand->expanded != expandNode)
                    {
                        nodeToExpand->expanded = expandNode;
                        nodeChanged = true;
                    }

                    nodeToExpand = nodeToExpand->parent;
                }
            }
            else // Collapsing
            {
                if (node->expanded != expandNode)
                {
                    node->expanded = expandNode;
                    nodeChanged = true;
                }
            }


            if (nodeChanged)
                markNodesDirty();

            return true;
        }
        else // Root node
        {
            for (const auto& node : m_nodes)
            {
                if (node->text.getString() != hierarchy.back())
                    continue;

                if (node->expanded != expandNode)
                {
                    node->expanded = expandNode;
                    markNodesDirty();
                }

                return true;
            }
        }

        return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TreeView::updateTextColors(std::vector<std::shared_ptr<Node>>& nodes)
    {
        for (auto& node : nodes)
        {
            node->text.setColor(m_textColorCached);
            updateTextColors(node->nodes);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TreeView::updateSelectedAndHoveringItemColors()
    {
        if (m_selectedItem >= 0)
        {
            if ((m_selectedItem == m_hoveredItem) && m_selectedTextColorHoverCached.isSet())
                m_visibleNodes[static_cast<std::size_t>(m_selectedItem)]->text.setColor(m_selectedTextColorHoverCached);
            else if (m_selectedTextColorCached.isSet())
                m_visibleNodes[static_cast<std::size_t>(m_selectedItem)]->text.setColor(m_selectedTextColorCached);
        }

        if ((m_hoveredItem >= 0) && (m_selectedItem != m_hoveredItem))
        {
            if (m_textColorHoverCached.isSet())
                m_visibleNodes[static_cast<std::size_t>(m_hoveredItem)]->text.setColor(m_textColorHoverCached);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TreeView::updateHoveredItem(int item)
    {
        if (m_hoveredItem == item)
            return;

        if (m_hoveredItem >= 0)
        {
            if ((m_selectedItem == m_hoveredItem) && m_selectedTextColorCached.isSet())
                m_visibleNodes[static_cast<std::size_t>(m_hoveredItem)]->text.setColor(m_selectedTextColorCached);
            else
                m_visibleNodes[static_cast<std::size_t>(m_hoveredItem)]->text.setColor(m_textColorCached);
        }

        m_hoveredItem = item;
        updateSelectedAndHoveringItemColors();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TreeView::updateSelectedItem(int item)
    {
        if (m_selectedItem == item)
            return;

        if (m_selectedItem >= 0)
        {
            if ((m_selectedItem == m_hoveredItem) && m_textColorHoverCached.isSet())
                m_visibleNodes[static_cast<std::size_t>(m_selectedItem)]->text.setColor(m_textColorHoverCached);
            else
                m_visibleNodes[static_cast<std::size_t>(m_selectedItem)]->text.setColor(m_textColorCached);
        }

        m_selectedItem = item;
        if (m_selectedItem >= 0)
        {
            std::vector<String> hierarchy;
            auto* node = m_visibleNodes[static_cast<std::size_t>(m_selectedItem)].get();
            assert(node != nullptr);
            while (node)
            {
                hierarchy.insert(hierarchy.begin(), node->text.getString());
                node = node->parent;
            }

            onItemSelect.emit(this, hierarchy.back(), hierarchy);
        }
        else
            onItemSelect.emit(this, "", {});

        updateSelectedAndHoveringItemColors();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    TreeView::Node* TreeView::findParentNode(const std::vector<String>& hierarchy, unsigned int parentIndex, std::vector<std::shared_ptr<Node>>& nodes, Node* parent, bool createParents)
    {
        for (auto& node : nodes)
        {
            if (node->text.getString() != hierarchy[parentIndex])
                continue;
            else if (parentIndex + 2 == hierarchy.size())
                return node.get();
            else
                return findParentNode(hierarchy, parentIndex + 1, node->nodes, node.get(), createParents);
        }

        if (createParents)
        {
            createNode(nodes, parent, hierarchy[parentIndex]);
            if (parentIndex + 2 == hierarchy.size())
                return nodes.back().get();
            else
                return findParentNode(hierarchy, parentIndex + 1, nodes.back()->nodes, nodes.back().get(), createParents);
        }

        return nullptr;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Widget::Ptr TreeView::clone() const
    {
        return std::make_shared<TreeView>(*this);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

