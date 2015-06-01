#include <TGUI/HorizontalLayout.hpp>

namespace tgui
{

HorizontalLayout::HorizontalLayout()
{
    m_callback.widgetType = "HorizontalLayout";
}

HorizontalLayout::Ptr HorizontalLayout::create()
{
    return std::make_shared<HorizontalLayout>();
}

HorizontalLayout::Ptr HorizontalLayout::copy(ConstPtr layout)
{
    if (layout)
        return std::make_shared<HorizontalLayout>(*layout);
    else
        return nullptr;
}

void HorizontalLayout::updatePosition()
{
    float sumRatio{0.f};
    for (unsigned int i = 0; i < m_widgetsRatio.size(); ++i)
        if (not m_widgetsfixedSizes[i])
            sumRatio += m_widgetsRatio[i];
    const float sumFixedSize{std::accumulate(m_widgetsfixedSizes.begin(), m_widgetsfixedSizes.end(), 0.f)};
    float currentRatio{0.f};
    float currentOffset{0.f};
    for (unsigned int i{0}; i < m_layoutWidgets.size(); ++i)
    {
        if (m_layoutWidgets[i])
        {
            m_layoutWidgets[i]->setPosition(m_size.x*currentRatio+currentOffset, 0.f);
            if(m_widgetsfixedSizes[i])
            {
                m_layoutWidgets[i]->setSize(m_widgetsfixedSizes[i], m_size.y);
                currentOffset += m_widgetsfixedSizes[i];
            }
            else
            {
                m_layoutWidgets[i]->setSize((m_size.x-sumFixedSize)*m_widgetsRatio[i]/sumRatio, m_size.y);
                currentRatio += m_widgetsRatio[i]/sumRatio;
            }
        }
    }
}

tgui::Widget::Ptr HorizontalLayout::clone()
{
    return std::make_shared<HorizontalLayout>(*this);
}
}
