#include <TGUI/VerticalLayout.hpp>

namespace tgui
{

VerticalLayout::VerticalLayout()
{
    m_callback.widgetType = "VerticalLayout";
}

VerticalLayout::Ptr VerticalLayout::create()
{
    return std::make_shared<VerticalLayout>();
}

VerticalLayout::Ptr VerticalLayout::copy(ConstPtr layout)
{
    if (layout)
        return std::make_shared<VerticalLayout>(*layout);
    else
        return nullptr;
}

void VerticalLayout::updatePosition()
{
    float sumRatio{0.f};
    for (unsigned int i = 0; i < m_widgetsRatio.size(); ++i)
        if (not m_widgetsfixedSizes[i])
            sumRatio += m_widgetsRatio[i];
    const float sumFixedSize{std::accumulate(m_widgetsfixedSizes.begin(), m_widgetsfixedSizes.end(), 0.f)};
    float currentRatio{0.f};
    float currentOffset{0.f};
    for (unsigned int i = 0; i < m_layoutWidgets.size(); ++i)
    {
        if (m_layoutWidgets[i])
        {
            m_layoutWidgets[i]->setPosition(0.f, m_size.y*currentRatio+currentOffset);
            if (m_widgetsfixedSizes[i])
            {
                m_layoutWidgets[i]->setSize(m_size.x, m_widgetsfixedSizes[i]);
                currentOffset += m_widgetsfixedSizes[i];
            }
            else
            {
                m_layoutWidgets[i]->setSize(m_size.x, (m_size.y-sumFixedSize)*m_widgetsRatio[i]/sumRatio);
                currentRatio += m_widgetsRatio[i]/sumRatio;
            }
        }
    }
}

tgui::Widget::Ptr VerticalLayout::clone()
{
    return std::make_shared<VerticalLayout>(*this);
}
}
