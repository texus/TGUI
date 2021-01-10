/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus' Graphical User Interface
// Copyright (C) 2012-2021 Bruno Van de Velde (vdv_b@tgui.eu)
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
        m_tabs = tgui::Tabs::create();
        m_container->add(m_tabs, "Tabs");
        init();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    TabContainer::Ptr TabContainer::create(const tgui::Layout2d& size)
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

        m_tabs->setWidth(size.x);
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

    void TabContainer::addPanel(Panel::Ptr ptr, const tgui::String& name, bool selectPanel)
    {
        auto size = getSizeLayout();
        ptr->setSize({ size.x , size.y - m_tabs->getSize().y });
        ptr->setPosition({ tgui::bindLeft(m_tabs), tgui::bindBottom(m_tabs) });

        m_panels.push_back(ptr);
        m_tabs->add(name, selectPanel);
        m_container->add(ptr);
        if (selectPanel)
            select(m_panels.size() - 1, false);
        else
            ptr->setVisible(false);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool TabContainer::insertPanel(tgui::Panel::Ptr ptr, const tgui::String& name, std::size_t index, bool selectPanel)
    {
        if (index > m_panels.size())
            return false;

        addPanel(ptr, name, selectPanel);
        auto size = m_panels.size();
        if (index != size)
        {
            std::swap(m_panels[index], m_panels[size - 1]);
            if (!selectPanel)
                m_index = static_cast<int>(size - 1);
        }
        return true;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TabContainer::removePanel(tgui::Panel::Ptr ptr)
    {
        if (ptr != nullptr)
        {
            auto idx = getIndex(ptr);
            if (idx != -1)
            {
                m_tabs->remove(idx);
                m_container->remove(m_panels[idx]);
                m_panels.erase(m_panels.begin() + idx);
                select(m_tabs->getSelectedIndex());
            }
        }
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

    int TabContainer::getIndex(tgui::Panel::Ptr ptr)
    {
        for (std::size_t i = 0; i < m_panels.size(); i++)
        {
            if (m_panels[i] == ptr)
                return static_cast<int>(i);
        }

        return -1;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    tgui::Panel::Ptr TabContainer::getSelected()
    {
        return getPanel(m_index);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    int TabContainer::getSelectedIndex() const
    {
        return m_index;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    tgui::Panel::Ptr TabContainer::getPanel(int index)
    {
        if (index < 0 || index >= static_cast<int>(m_panels.size()))
            return nullptr;

        return m_panels[index];
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    tgui::Tabs::Ptr TabContainer::getTabs()
    {
        return m_tabs;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    tgui::String TabContainer::getTabText(std::size_t index) const
    {
        return m_tabs->getText(index);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool TabContainer::changeTabText(std::size_t index, const tgui::String& text)
    {
        return m_tabs->changeText(index, text);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TabContainer::load(const std::unique_ptr<DataIO::Node>& node, const LoadingRenderersMap& renderers)
    {
        SubwidgetContainer::load(node, renderers);

        m_index = -1;
        m_tabs = m_container->get<tgui::Tabs>("Tabs");

        auto widgets = m_container->getWidgets();
        m_panels.resize(widgets.size() - 1);
        auto size = getSizeLayout();
        for (std::size_t i = 1; i < widgets.size(); i++)
        {
            m_panels[i - 1] = std::static_pointer_cast<Panel>(widgets[i]);
            m_panels[i - 1]->setSize({ size.x , size.y - m_tabs->getSize().y });
            m_panels[i - 1]->setPosition({ tgui::bindLeft(m_tabs), tgui::bindBottom(m_tabs) });
        }

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
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
