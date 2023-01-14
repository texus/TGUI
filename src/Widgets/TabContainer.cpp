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

#include <TGUI/Widgets/TabContainer.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
#if TGUI_COMPILED_WITH_CPP_VER < 17
    constexpr const char TabContainer::StaticWidgetType[];
#endif

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    TabContainer::TabContainer(const char* typeName, bool initRenderer) :
        SubwidgetContainer{typeName, initRenderer}
    {
        m_container->add(m_tabs, "Tabs");
        init();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    TabContainer::Ptr TabContainer::create(const Layout2d& size)
    {
        auto tabControl = std::make_shared<TabContainer>();
        tabControl->setSize(size);
        return tabControl;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    TabContainer::Ptr TabContainer::copy(const TabContainer::ConstPtr& tabContainer)
    {
        if (tabContainer)
            return std::static_pointer_cast<TabContainer>(tabContainer->clone());
        else
            return nullptr;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    TabsRenderer* TabContainer::getTabsSharedRenderer()
    {
        return m_tabs->getSharedRenderer();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const TabsRenderer* TabContainer::getTabsSharedRenderer() const
    {
        return m_tabs->getSharedRenderer();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    TabsRenderer* TabContainer::getTabsRenderer()
    {
        return m_tabs->getRenderer();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TabContainer::setSize(const Layout2d& size)
    {
        SubwidgetContainer::setSize(size);

        for (auto& panel : m_panels)
            panel->setSize({getSize().x, getSize().y - m_tabs->getSize().y});

        layoutTabs();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TabContainer::setTabsHeight(const Layout& height)
    {
        m_tabs->setHeight(height);
        setSize(getSizeLayout());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Panel::Ptr TabContainer::addTab(const String& name, bool selectPanel)
    {
        auto panel = Panel::create();
        panel->setSize({getSize().x , getSize().y - m_tabs->getSize().y});
        layoutPanel(panel);

        m_panels.push_back(panel);
        m_tabs->add(name, false);
        layoutTabs();

        m_container->add(panel, name);
        if (selectPanel)
            select(m_panels.size() - 1);
        else
            panel->setVisible(false);

        return panel;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Panel::Ptr TabContainer::insertTab(std::size_t index, const String& name, bool selectPanel)
    {
        if (index > m_panels.size())
            index = m_panels.size();

        auto panel = Panel::create();
        panel->setSize({getSize().x , getSize().y - m_tabs->getSize().y});
        layoutPanel(panel);

        m_panels.insert(m_panels.begin() + static_cast<std::ptrdiff_t>(index), panel);
        m_tabs->insert(index, name, false);
        layoutTabs();

        m_container->add(panel);
        m_container->setWidgetIndex(panel, index + 1); // Plus one because first widget is Tabs

        if (selectPanel)
            select(index);
        else
            panel->setVisible(false);

        return panel;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool TabContainer::removeTab(const String& text)
    {
        for (std::size_t i = 0; i < m_panels.size(); ++i)
        {
            if (m_tabs->getText(i) == text)
                return removeTab(i);
        }

        return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool TabContainer::removeTab(std::size_t index)
    {
        if (index >= m_panels.size())
            return false;

        m_tabs->remove(index);
        layoutTabs();

        m_container->remove(m_panels[index]);
        m_panels.erase(m_panels.begin() + static_cast<std::ptrdiff_t>(index));

        if (m_tabs->getSelectedIndex() >= 0)
            select(static_cast<std::size_t>(m_tabs->getSelectedIndex()));
        else
        {
            // Select the last tab when the selected tab is removed
            if (!m_panels.empty())
                select(m_panels.size() - 1);
            else
                m_selectedPanel = nullptr;
        }

        return true;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TabContainer::select(std::size_t index)
    {
        if ((index >= m_panels.size()) || (m_selectedPanel == m_panels[index]))
            return;

        bool isVetoed = false;
        onSelectionChanging.emit(this, static_cast<int>(index), &isVetoed);
        if (isVetoed)
            return;

        if (m_selectedPanel)
            m_selectedPanel->setVisible(false);

        m_panels[index]->setVisible(true);
        m_selectedPanel = m_panels[index];

        m_tabs->select(index);

        onSelectionChanged.emit(this, static_cast<int>(index));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::size_t TabContainer::getPanelCount() const
    {
        return m_panels.size();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    int TabContainer::getIndex(const Panel::Ptr& ptr)
    {
        for (std::size_t i = 0; i < m_panels.size(); ++i)
        {
            if (m_panels[i] == ptr)
                return static_cast<int>(i);
        }

        return -1;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Panel::Ptr TabContainer::getSelected()
    {
        return m_selectedPanel;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    int TabContainer::getSelectedIndex() const
    {
        return m_tabs->getSelectedIndex();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Panel::Ptr TabContainer::getPanel(int index)
    {
        if (index < 0 || index >= static_cast<int>(m_panels.size()))
            return nullptr;

        return m_panels[static_cast<std::size_t>(index)];
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Tabs::Ptr TabContainer::getTabs()
    {
        return m_tabs;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    String TabContainer::getTabText(std::size_t index) const
    {
        return m_tabs->getText(index);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TabContainer::layoutTabs()
    {
        if (m_tabFixedSize > 0.0f)
            m_tabs->setWidth(m_tabFixedSize * getPanelCount());
        else
            m_tabs->setWidth(getSize().x);

        if (m_tabAlign == TabContainer::TabAlign::Top)
            m_tabs->setPosition(0.0f, 0.0f);
        else
            m_tabs->setPosition(0.0f, getSize().y - m_tabs->getSize().y);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TabContainer::layoutPanel(const Panel::Ptr& panel)
    {
        if (m_tabAlign == TabContainer::TabAlign::Top)
            panel->setPosition(0.0f, bindBottom(m_tabs));
        else
            panel->setPosition(0.0f, 0.0f);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TabContainer::setTabAlignment(TabAlign align)
    {
        if (m_tabAlign == align)
            return;

        m_tabAlign = align;

        layoutTabs();

        for (const auto& panel : m_panels)
            layoutPanel(panel);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    TabContainer::TabAlign TabContainer::getTabAlignment() const
    {
        return m_tabAlign;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TabContainer::setTabFixedSize(float fixedSize)
    {
        if (m_tabFixedSize == fixedSize)
            return;

        m_tabFixedSize = fixedSize;

        layoutTabs();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    float TabContainer::getTabFixedSize() const
    {
        return m_tabFixedSize;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool TabContainer::changeTabText(std::size_t index, const String& text)
    {
        return m_tabs->changeText(index, text);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::unique_ptr<DataIO::Node> TabContainer::save(SavingRenderersMap& renderers) const
    {
        auto node = SubwidgetContainer::save(renderers);

        if (m_tabAlign == tgui::TabContainer::TabAlign::Top)
            node->propertyValuePairs[U"TabAlignment"] = std::make_unique<DataIO::ValueNode>("Top");
        else
            node->propertyValuePairs[U"TabAlignment"] = std::make_unique<DataIO::ValueNode>("Bottom");

        node->propertyValuePairs[U"TabFixedSize"] = std::make_unique<DataIO::ValueNode>(Serializer::serialize(m_tabFixedSize));

        return node;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TabContainer::load(const std::unique_ptr<DataIO::Node>& node, const LoadingRenderersMap& renderers)
    {
        SubwidgetContainer::load(node, renderers);

        // Buffer the value to apply it after child widget creation with default align.
        auto tabAlign = tgui::TabContainer::TabAlign::Top;
        if (node->propertyValuePairs[U"TabAlignment"])
        {
            String alignment = Deserializer::deserialize(ObjectConverter::Type::String, node->propertyValuePairs[U"TabAlignment"]->value).getString();
            if (alignment == U"Bottom")
                tabAlign = tgui::TabContainer::TabAlign::Bottom;
            else if (alignment != U"Top")
                throw Exception{U"Failed to parse TabAlignment property, found unknown value."};
        }

        // Buffer the value to apply it after child widget creation with default size.
        float tabFixedSize = Deserializer::deserialize(ObjectConverter::Type::Number, node->propertyValuePairs[U"TabFixedSize"]->value).getNumber();

        m_panels.clear();
        m_selectedPanel = nullptr;

        m_tabs = m_container->get<Tabs>(U"Tabs");
        m_tabAlign = TabAlign::Top;
        if (!m_tabs)
            throw Exception{U"Failed to find Tabs child when loading TabContainer"};

        auto widgets = m_container->getWidgets();
        for (const auto& widget : widgets)
        {
            auto panel = std::dynamic_pointer_cast<Panel>(widget);
            if (!panel)
                continue;

            panel->setSize({getSize().x, getSize().y - m_tabs->getSize().y});
            layoutPanel(panel);
            m_panels.push_back(panel);
        }

        // Apply buffered values.
        setTabAlignment(tabAlign);
        setTabFixedSize(tabFixedSize);
        if (m_tabs->getSelectedIndex())
            select(static_cast<std::size_t>(m_tabs->getSelectedIndex()));
        init();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TabContainer::init()
    {
        layoutTabs();
        m_tabs->onTabSelect([this](){
            TGUI_ASSERT(m_tabs->getSelectedIndex() >= 0, "TabContainer relies on Tabs::onTabSelect not firing on deselect");
            select(static_cast<std::size_t>(m_tabs->getSelectedIndex()));
        });
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Widget::Ptr TabContainer::clone() const
    {
        return std::make_shared<TabContainer>(*this);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
