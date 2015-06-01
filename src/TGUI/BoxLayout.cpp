#include <TGUI/ClickableWidget.hpp>
#include <TGUI/BoxLayout.hpp>

namespace tgui
{
    void BoxLayout::add(const tgui::Widget::Ptr& widget, const sf::String& widgetName)
    {
        insert(m_layoutWidgets.size(), widget, widgetName);
    }

    void BoxLayout::insert(unsigned int position, const tgui::Widget::Ptr& widget, const sf::String& widgetName)
    {
        if (position > m_layoutWidgets.size())
            throw std::out_of_range("The given position is invalid for inserting in layout.");
        Container::add(widget, widgetName);
        m_layoutWidgets.insert(m_layoutWidgets.begin()+position, widget);
        m_widgetsRatio.insert(m_widgetsRatio.begin()+position, 1.f);
        updatePosition();
    }

    void BoxLayout::addSpace(const sf::String& widgetName)
    {
        auto space = std::make_shared<tgui::ClickableWidget>();
        space->hide();
        space->disable();
        add(space, widgetName);
    }

    void BoxLayout::insertSpace(unsigned int position, const sf::String& widgetName)
    {
        auto space = std::make_shared<tgui::ClickableWidget>();
        space->hide();
        space->disable();
        insert(position, space, widgetName);
    }

    void BoxLayout::setRatio(const Widget::Ptr& widget, float ratio)
    {
        unsigned int i = 0;
        while (i < m_layoutWidgets.size() and m_layoutWidgets[i] != widget)
            ++i;
        setRatio(i, ratio);
    }

    void BoxLayout::setRatio(unsigned int position, float ratio)
    {
        if (position >= m_layoutWidgets.size())
            throw std::out_of_range("The given element is invalid for setting ratio in layout.");
        m_widgetsRatio[position] = ratio;
        updatePosition();
    }

    void BoxLayout::remove(const tgui::Widget::Ptr& widget)
    {
        unsigned int i = 0;
        while (i < m_layoutWidgets.size() and m_layoutWidgets[i] != widget)
            ++i;
        remove(i);
    }

    void BoxLayout::remove(unsigned int position)
    {
        if (position >= m_layoutWidgets.size())
            throw std::out_of_range("The given element is invalid for removing in layout.");
        Container::remove(m_layoutWidgets[position]);
        m_layoutWidgets.erase(m_layoutWidgets.begin()+position);
        m_widgetsRatio.erase(m_widgetsRatio.begin()+position);
        updatePosition();
    }

    void BoxLayout::removeAllWidgets()
    {
        Container::removeAllWidgets();
        m_layoutWidgets.clear();
        m_widgetsRatio.clear();
    }

    Widget::Ptr BoxLayout::get(unsigned int position)
    {
        if (position >= m_layoutWidgets.size())
            throw std::out_of_range("The given position is invalid.");
        return m_layoutWidgets[position];
    }

    void BoxLayout::draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        states.transform.translate(getPosition());
        // Draw all widgets
        for (unsigned int i = 0; i < m_layoutWidgets.size(); ++i)
        {
            if (m_layoutWidgets[i].get())
                target.draw(*m_layoutWidgets[i], states);
        }
    }

    bool BoxLayout::mouseOnWidget(float x, float y)
    {
        //TODO Fix this function, mouse lefting seems to be broken.
        // Check if the mouse might be on top of the layout
        if ((x > getPosition().x) && (y > getPosition().y))
        {
            // Check if the mouse is on the layout
            if ((x < getPosition().x + getSize().x) && (y < getPosition().y + getSize().y))
                return true;
        }
        if (m_mouseHover)
        {
            mouseLeftWidget();
            // Tell the widgets inside the layout that the mouse is no longer on top of them
            for (unsigned int i = 0; i < m_widgets.size(); ++i)
                m_widgets[i]->mouseNotOnWidget();
        }
        return false;
    }
}
