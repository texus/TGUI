#include <TGUI/HorizontalLayout.hpp>
#include <numeric>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    HorizontalLayout::HorizontalLayout()
    {
        m_callback.widgetType = "HorizontalLayout";
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    HorizontalLayout::Ptr HorizontalLayout::create()
    {
        return std::make_shared<HorizontalLayout>();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    HorizontalLayout::Ptr HorizontalLayout::copy(HorizontalLayout::ConstPtr layout)
    {
        if (layout)
            return std::static_pointer_cast<HorizontalLayout>(layout->clone());
        else
            return nullptr;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void HorizontalLayout::updateWidgetPositions()
    {
        float sumRatio = 0;
        for (std::size_t i = 0; i < m_widgetsRatio.size(); ++i)
        {
            if (m_widgetsFixedSizes[i] == 0)
                sumRatio += m_widgetsRatio[i];
        }

        float currentRatio = 0;
        float currentOffset = 0;
        const float sumFixedSize = std::accumulate(m_widgetsFixedSizes.begin(), m_widgetsFixedSizes.end(), 0.f);
        for (std::size_t i = 0; i < m_widgets.size(); ++i)
        {
            m_widgets[i]->setPosition((getSize().x - sumFixedSize) * currentRatio + currentOffset, 0.f);
            if (m_widgetsFixedSizes[i])
            {
                m_widgets[i]->setSize(m_widgetsFixedSizes[i], getSize().y);
                currentOffset += m_widgetsFixedSizes[i];
            }
            else
            {
                m_widgets[i]->setSize((getSize().x - sumFixedSize) * m_widgetsRatio[i] / sumRatio, getSize().y);
                currentRatio += m_widgetsRatio[i] / sumRatio;
            }

            // Correct the size for widgets that have borders around it or a text next to them
            if (m_widgets[i]->getFullSize() != m_widgets[i]->getSize())
            {
                sf::Vector2f newSize = m_widgets[i]->getSize() - (m_widgets[i]->getFullSize() - m_widgets[i]->getSize());
                if (newSize.x > 0 && newSize.y > 0)
                {
                    m_widgets[i]->setSize(newSize);
                    m_widgets[i]->setPosition(m_widgets[i]->getPosition() + m_widgets[i]->getWidgetOffset());
                }
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
