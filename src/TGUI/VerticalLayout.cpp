#include <TGUI/VerticalLayout.hpp>
#include <numeric>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    VerticalLayout::VerticalLayout()
    {
        m_callback.widgetType = "VerticalLayout";
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    VerticalLayout::Ptr VerticalLayout::copy(VerticalLayout::ConstPtr layout)
    {
        if (layout)
            return std::static_pointer_cast<VerticalLayout>(layout->clone());
        else
            return nullptr;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void VerticalLayout::updateWidgetPositions()
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
        for (std::size_t i = 0; i < m_layoutWidgets.size(); ++i)
        {
            m_layoutWidgets[i]->setPosition(0.f, (getSize().y - sumFixedSize) * currentRatio + currentOffset);
            if (m_widgetsFixedSizes[i])
            {
                m_layoutWidgets[i]->setSize(getSize().x, m_widgetsFixedSizes[i]);
                currentOffset += m_widgetsFixedSizes[i];
            }
            else
            {
                m_layoutWidgets[i]->setSize(getSize().x, (getSize().y - sumFixedSize) * m_widgetsRatio[i] / sumRatio);
                currentRatio += m_widgetsRatio[i] / sumRatio;
            }

            // Correct the size for widgets that have borders around it or a text next to them
            if (m_layoutWidgets[i]->getFullSize() != m_layoutWidgets[i]->getSize())
            {
                sf::Vector2f newSize = m_layoutWidgets[i]->getSize() - (m_layoutWidgets[i]->getFullSize() - m_layoutWidgets[i]->getSize());
                if (newSize.x > 0 && newSize.y > 0)
                {
                    m_layoutWidgets[i]->setSize(newSize);
                    m_layoutWidgets[i]->setPosition(m_layoutWidgets[i]->getPosition() + m_layoutWidgets[i]->getWidgetOffset());
                }
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
