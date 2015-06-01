#include <TGUI/ClickableWidget.hpp>
#include <TGUI/BoxLayout.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    BoxLayout::BoxLayout()
    {
        setBackgroundColor(sf::Color::Transparent);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void BoxLayout::setSize(const Layout& size)
    {
        Panel::setSize(size);
        updateWidgetPositions();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool BoxLayout::insert(unsigned int index, const tgui::Widget::Ptr& widget, const sf::String& widgetName)
    {
        Container::add(widget, widgetName);

        if (index > m_layoutWidgets.size())
        {
            m_layoutWidgets.emplace_back(widget);
            m_widgetsRatio.emplace_back(1);
            m_widgetsfixedSizes.emplace_back(0);
            updateWidgetPositions();
            return false;
        }
        else
        {
            m_layoutWidgets.insert(m_layoutWidgets.begin() + index, widget);
            m_widgetsRatio.insert(m_widgetsRatio.begin() + index, 1);
            m_widgetsfixedSizes.insert(m_widgetsfixedSizes.begin() + index, 0);
            updateWidgetPositions();
            return true;
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void BoxLayout::add(const tgui::Widget::Ptr& widget, const sf::String& widgetName)
    {
        insert(m_layoutWidgets.size(), widget, widgetName);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void BoxLayout::addSpace(float ratio)
    {
        add(tgui::ClickableWidget::create(), "");
        setRatio(m_layoutWidgets.size()-1, ratio);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool BoxLayout::insertSpace(unsigned int index, float ratio)
    {
        bool success = insert(index, tgui::ClickableWidget::create(), "");
        setRatio(index, ratio);
        return success;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool BoxLayout::setRatio(const Widget::Ptr& widget, float ratio)
    {
        for (unsigned int i = 0; i < m_layoutWidgets.size(); ++i)
        {
            if (m_layoutWidgets[i] == widget)
                return setRatio(i, ratio);
        }

        return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool BoxLayout::setRatio(unsigned int index, float ratio)
    {
        if (index >= m_layoutWidgets.size())
            return false;

        m_widgetsRatio[index] = ratio;
        updateWidgetPositions();
        return true;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool BoxLayout::remove(const tgui::Widget::Ptr& widget)
    {
        for (unsigned int i = 0; i < m_layoutWidgets.size(); ++i)
        {
            if (m_layoutWidgets[i] == widget)
                return remove(i);
        }

        return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool BoxLayout::remove(unsigned int index)
    {
        if (index >= m_layoutWidgets.size())
            return false;

        Container::remove(m_layoutWidgets[index]);
        m_layoutWidgets.erase(m_layoutWidgets.begin() + index);
        m_widgetsRatio.erase(m_widgetsRatio.begin() + index);
        m_widgetsfixedSizes.erase(m_widgetsfixedSizes.begin() + index);
        updateWidgetPositions();
        return true;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool BoxLayout::setFixedSize(const Widget::Ptr& widget, float size)
    {
        for (unsigned int i = 0; i < m_layoutWidgets.size(); ++i)
        {
            if (m_layoutWidgets[i] == widget)
                return setFixedSize(i, size);
        }

        return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool BoxLayout::setFixedSize(unsigned int index, float size)
    {
        if (index >= m_layoutWidgets.size())
            return false;

        m_widgetsfixedSizes[index] = size;
        updateWidgetPositions();
        return true;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void BoxLayout::removeAllWidgets()
    {
        Container::removeAllWidgets();
        m_layoutWidgets.clear();
        m_widgetsRatio.clear();
        m_widgetsfixedSizes.clear();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Widget::Ptr BoxLayout::get(unsigned int index)
    {
        if (index < m_layoutWidgets.size())
            return m_layoutWidgets[index];
        else
            return nullptr;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
