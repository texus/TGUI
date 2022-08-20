/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus' Graphical User Interface
// Copyright (C) 2012-2022 Bruno Van de Velde (vdv_b@tgui.eu)
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
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    TabContainer::TabContainer(const char* typeName, bool initRenderer) :
        SubwidgetContainer{typeName, initRenderer}
    {
        m_tabs = Tabs::create();
        m_tabs->setWidth("100%");
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

    TabContainer::Ptr TabContainer::copy(TabContainer::ConstPtr tabContainer)
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

    const TabsRenderer* TabContainer::getTabsRenderer() const
    {
        return m_tabs->getRenderer();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TabContainer::setSize(const Layout2d& size)
    {
        SubwidgetContainer::setSize(size);

        for (auto& ptr : m_panels)
            ptr->setSize({ size.x , size.y - m_tabs->getSize().y });
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TabContainer::setTabsHeight(Layout height)
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
        m_tabs->add(name, selectPanel);
        layoutTabs();

        m_container->add(panel);
        if (selectPanel)
            select(m_panels.size() - 1, false);
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

        m_panels.insert(m_panels.begin() + index, panel);
        m_tabs->insert(index, name, selectPanel);
        layoutTabs();

        m_container->add(panel);
        m_container->setWidgetIndex(panel, index);

        if (selectPanel)
            select(m_panels.size() - 1, false);
        else
        {
            panel->setVisible(false);
            if (static_cast<int>(index) <= m_index)
                ++m_index;
        }

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

        if ((std::size_t)m_tabs->getSelectedIndex() >= m_panels.size() - 1)
            select(m_panels.size() - 2);
        m_container->remove(m_panels[index]);
        m_panels.erase(m_panels.begin() + index);
        return true;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TabContainer::select(std::size_t index, bool genEvents)
    {
        if (index >= m_panels.size() || index == static_cast<std::size_t>(m_index))
            return;

        if (genEvents)
        {
            bool isVetoed = false;
            onSelectionChanging.emit(this, static_cast<int>(index), &isVetoed);
            if (isVetoed)
                return;
        }

        if (m_index != -1)
            m_panels[m_index]->setVisible(false);

        m_index = static_cast<int>(index);
        m_tabs->select(index);
        m_panels[index]->setVisible(true);
        if (genEvents)
            onSelectionChanged.emit(this, m_index);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::size_t TabContainer::getPanelCount() const
    {
        return m_panels.size();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    int TabContainer::getIndex(Panel::Ptr ptr)
    {
        for (std::size_t i = 0; i < m_panels.size(); i++)
        {
            if (m_panels[i] == ptr)
                return static_cast<int>(i);
        }

        return -1;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Panel::Ptr TabContainer::getSelected()
    {
        return getPanel(m_index);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    int TabContainer::getSelectedIndex() const
    {
        return m_index;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Panel::Ptr TabContainer::getPanel(int index)
    {
        if (index < 0 || index >= static_cast<int>(m_panels.size()))
            return nullptr;

        return m_panels[index];
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

    void TabContainer::setTabAlign(TabAlignment align)
    {
        if (m_tabAlign == align)
            return;

        m_tabAlign = align;

        layoutTabs();

        for (std::size_t i = 0; i < m_panels.size(); i++)
            layoutPanel(m_panels[i]);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TabContainer::layoutTabs()
    {
        if (m_tabAlign == TabAlign::Top || m_tabAlign == TabAlign::TopFixedWidth)
        {
            m_tabs->setPosition(0.0f, 0.0f);
            if (m_tabAlign == TabAlign::Top)
                m_tabs->setWidth("100%");
            else
                m_tabs->setWidth(bindNumberOfTabs(m_tabs, m_tabFixedSize));
        }
        else
        {
            tgui::Layout layoutY(tgui::Layout::Operation::Minus,
                                 std::make_unique<Layout>(tgui::Layout::Operation::BindingInnerHeight, m_container.get()),
                                 std::make_unique<Layout>(tgui::Layout::Operation::BindingHeight, m_tabs.get()));
            m_tabs->setPosition(0.0f, layoutY);
            if (m_tabAlign == TabAlign::Bottom)
                m_tabs->setWidth("100%");
            else
                m_tabs->setWidth(bindNumberOfTabs(m_tabs, m_tabFixedSize));
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TabContainer::layoutPanel(Panel::Ptr panel)
    {
        if (m_tabAlign == TabAlign::Top || m_tabAlign == TabAlign::TopFixedWidth)
            panel->setPosition(0.0f, bindBottom(m_tabs));
        else
            panel->setPosition(0.0f, Layout{tgui::Layout::Operation::BindingTop, m_container.get()});
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    TabAlignment TabContainer::getTabAlign() const
    {
        return m_tabAlign;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TabContainer::setTabFixedSize(float fixedSize)
    {
        m_tabFixedSize = fixedSize;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    float TabContainer::getTabFixedSize()
    {
        return m_tabFixedSize;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool TabContainer::changeTabText(std::size_t index, const String& text)
    {
        return m_tabs->changeText(index, text);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TabContainer::load(const std::unique_ptr<DataIO::Node>& node, const LoadingRenderersMap& renderers)
    {
        SubwidgetContainer::load(node, renderers);

        m_index = -1;
        m_tabs = m_container->get<Tabs>("Tabs");
        auto tabAlign = m_tabAlign;
        m_tabAlign = TabAlignment(TabAlign::Top);

        auto widgets = m_container->getWidgets();
        m_panels.resize(widgets.size() - 1);
        auto size = getSizeLayout();
        for (std::size_t i = 1; i < widgets.size(); i++)
        {
            m_panels[i - 1] = std::static_pointer_cast<Panel>(widgets[i]);
            m_panels[i - 1]->setSize({ size.x , size.y - m_tabs->getSize().y });
            m_panels[i - 1]->setPosition({ 0.0f, bindBottom(m_tabs) });
        }

        setTabAlign(tabAlign);
        select(m_tabs->getSelectedIndex());
        init();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TabContainer::init()
    {
        m_tabs->onTabSelect([this](){
            auto cur = m_tabs->getSelectedIndex();
            select(cur);
            if (m_tabs->getSelectedIndex() != m_index)
                m_tabs->select(m_index);
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
