#include <TGUI/BoxLayout.hpp>
#include <TGUI/Widgets/ClickableWidget.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    BoxLayout::BoxLayout()
    {
        setBackgroundColor(sf::Color::Transparent);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void BoxLayout::setSize(const Layout2d& size)
    {
        Panel::setSize(size);
        updateWidgetPositions();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void BoxLayout::setFont(const Font& font)
    {
        Panel::setFont(font);
        updateWidgetPositions();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool BoxLayout::insert(std::size_t index, const tgui::Widget::Ptr& widget, const sf::String& widgetName)
    {
        Container::add(widget, widgetName);

        if (index >= m_widgets.size())
        {
            m_widgetsRatio.emplace_back(1.f);
            m_widgetsFixedSizes.emplace_back(0.f);
            updateWidgetPositions();
            return false;
        }
        else
        {
            m_widgets.insert(m_widgets.begin() + index, widget);
            m_widgets.pop_back(); // The widget was added at the back with Container::add

            m_widgetsRatio.insert(m_widgetsRatio.begin() + index, 1.f);
            m_widgetsFixedSizes.insert(m_widgetsFixedSizes.begin() + index, 0.f);
            updateWidgetPositions();
            return true;
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void BoxLayout::add(const tgui::Widget::Ptr& widget, const sf::String& widgetName)
    {
        insert(m_widgets.size(), widget, widgetName);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void BoxLayout::addSpace(float ratio)
    {
        add(std::make_shared<ClickableWidget>(), "");
        setRatio(m_widgets.size()-1, ratio);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool BoxLayout::insertSpace(std::size_t index, float ratio)
    {
        bool success = insert(index, std::make_shared<ClickableWidget>(), "");
        setRatio(index, ratio);
        return success;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool BoxLayout::remove(const tgui::Widget::Ptr& widget)
    {
        for (std::size_t i = 0; i < m_widgets.size(); ++i)
        {
            if (m_widgets[i] == widget)
                return remove(i);
        }

        return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool BoxLayout::remove(std::size_t index)
    {
        if (index >= m_widgets.size())
            return false;

        Container::remove(m_widgets[index]);
        m_widgetsRatio.erase(m_widgetsRatio.begin() + index);
        m_widgetsFixedSizes.erase(m_widgetsFixedSizes.begin() + index);
        updateWidgetPositions();
        return true;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Widget::Ptr BoxLayout::get(std::size_t index)
    {
        if (index < m_widgets.size())
            return m_widgets[index];
        else
            return nullptr;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool BoxLayout::setRatio(const Widget::Ptr& widget, float ratio)
    {
        for (std::size_t i = 0; i < m_widgets.size(); ++i)
        {
            if (m_widgets[i] == widget)
                return setRatio(i, ratio);
        }

        return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool BoxLayout::setRatio(std::size_t index, float ratio)
    {
        if (index >= m_widgets.size())
            return false;

        m_widgetsRatio[index] = ratio;
        updateWidgetPositions();
        return true;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    float BoxLayout::getRatio(const Widget::Ptr& widget)
    {
        for (std::size_t i = 0; i < m_widgets.size(); ++i)
        {
            if (m_widgets[i] == widget)
                return getRatio(i);
        }

        return 0;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    float BoxLayout::getRatio(std::size_t index)
    {
        if (index >= m_widgets.size())
            return 0;

        return m_widgetsRatio[index];
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool BoxLayout::setFixedSize(const Widget::Ptr& widget, float size)
    {
        for (std::size_t i = 0; i < m_widgets.size(); ++i)
        {
            if (m_widgets[i] == widget)
                return setFixedSize(i, size);
        }

        return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool BoxLayout::setFixedSize(std::size_t index, float size)
    {
        if (index >= m_widgets.size())
            return false;

        m_widgetsFixedSizes[index] = size;
        updateWidgetPositions();
        return true;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    float BoxLayout::getFixedSize(const Widget::Ptr& widget)
    {
        for (std::size_t i = 0; i < m_widgets.size(); ++i)
        {
            if (m_widgets[i] == widget)
                return getFixedSize(i);
        }

        return 0;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    float BoxLayout::getFixedSize(std::size_t index)
    {
        if (index >= m_widgets.size())
            return 0;

        return m_widgetsFixedSizes[index];
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void BoxLayout::removeAllWidgets()
    {
        Container::removeAllWidgets();
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
