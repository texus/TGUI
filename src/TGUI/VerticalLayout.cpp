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

    VerticalLayout::Ptr VerticalLayout::copy(ConstPtr layout)
    {
        if (layout)
            return std::make_shared<VerticalLayout>(*layout);
        else
            return nullptr;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void VerticalLayout::updateWidgetPositions()
    {
        float sumRatio = 0;
        for (unsigned int i = 0; i < m_widgetsRatio.size(); ++i)
        {
            if (m_widgetsFixedSizes[i] == 0)
                sumRatio += m_widgetsRatio[i];
        }

        float currentRatio = 0;
        float currentOffset = 0;
        const float sumFixedSize = std::accumulate(m_widgetsFixedSizes.begin(), m_widgetsFixedSizes.end(), 0.f);
        for (unsigned int i = 0; i < m_layoutWidgets.size(); ++i)
        {
            m_layoutWidgets[i]->setPosition(0.f, (m_size.y - sumFixedSize) * currentRatio + currentOffset);
            if (m_widgetsFixedSizes[i])
            {
                m_layoutWidgets[i]->setSize(m_size.x, m_widgetsFixedSizes[i]);
                currentOffset += m_widgetsFixedSizes[i];
            }
            else
            {
                m_layoutWidgets[i]->setSize(m_size.x, (m_size.y - sumFixedSize) * m_widgetsRatio[i] / sumRatio);
                currentRatio += m_widgetsRatio[i] / sumRatio;
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    tgui::Widget::Ptr VerticalLayout::clone()
    {
        return std::make_shared<VerticalLayout>(*this);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
