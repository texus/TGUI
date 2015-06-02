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

    bool BoxLayout::insert(std::size_t index, const tgui::Widget::Ptr& widget, const sf::String& widgetName)
    {
        Container::add(widget, widgetName);

        if (index > m_layoutWidgets.size())
        {
            m_layoutWidgets.emplace_back(widget);
            m_widgetsRatio.emplace_back(1.f);
            m_widgetsFixedSizes.emplace_back(0.f);
            updateWidgetPositions();
            return false;
        }
        else
        {
            m_layoutWidgets.insert(m_layoutWidgets.begin() + index, widget);
            m_widgetsRatio.insert(m_widgetsRatio.begin() + index, 1.f);
            m_widgetsFixedSizes.insert(m_widgetsFixedSizes.begin() + index, 0.f);
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

    bool BoxLayout::insertSpace(std::size_t index, float ratio)
    {
        bool success = insert(index, tgui::ClickableWidget::create(), "");
        setRatio(index, ratio);
        return success;
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

    bool BoxLayout::remove(std::size_t index)
    {
        if (index >= m_layoutWidgets.size())
            return false;

        Container::remove(m_layoutWidgets[index]);
        m_layoutWidgets.erase(m_layoutWidgets.begin() + index);
        m_widgetsRatio.erase(m_widgetsRatio.begin() + index);
        m_widgetsFixedSizes.erase(m_widgetsFixedSizes.begin() + index);
        updateWidgetPositions();
        return true;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Widget::Ptr BoxLayout::get(std::size_t index)
    {
        if (index < m_layoutWidgets.size())
            return m_layoutWidgets[index];
        else
            return nullptr;
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

    bool BoxLayout::setRatio(std::size_t index, float ratio)
    {
        if (index >= m_layoutWidgets.size())
            return false;

        m_widgetsRatio[index] = ratio;
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

    bool BoxLayout::setFixedSize(std::size_t index, float size)
    {
        if (index >= m_layoutWidgets.size())
            return false;

        m_widgetsFixedSizes[index] = size;
        updateWidgetPositions();
        return true;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void BoxLayout::removeAllWidgets()
    {
        Container::removeAllWidgets();
        m_layoutWidgets.clear();
        m_widgetsRatio.clear();
        m_widgetsFixedSizes.clear();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void BoxLayout::draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        // Set the position
        states.transform.translate(getPosition());

        // Draw the background
        if (m_backgroundColor != sf::Color::Transparent)
        {
            sf::RectangleShape background(getSize());
            background.setFillColor(m_backgroundColor);
            target.draw(background, states);
        }

        // Draw the widgets
        drawWidgetContainer(&target, states);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
