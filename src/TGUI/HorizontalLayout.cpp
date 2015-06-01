#include <TGUI/HorizontalLayout.hpp>

namespace tgui
{

HorizontalLayout::HorizontalLayout()
{
	m_callback.widgetType = "HorizontalLayout";
}

HorizontalLayout::HorizontalLayout(const HorizontalLayout& layoutToCopy) :
	BoxLayout{layoutToCopy}
{
}

HorizontalLayout& HorizontalLayout::operator=(const HorizontalLayout& right)
{
	if (this != &right)
		BoxLayout::operator=(right);
	return *this;
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
	const float sumRatio{std::accumulate(m_widgetsRatio.begin(), m_widgetsRatio.end(), 0.f)};
	float actualPosition{0.f};
	for (unsigned int i{0}; i < m_layoutWidgets.size(); ++i)
	{
		if (m_layoutWidgets[i])
		{
			m_layoutWidgets[i]->setSize(m_size.x*m_widgetsRatio[i]/sumRatio, m_size.y);
			m_layoutWidgets[i]->setPosition(m_size.x*actualPosition, 0.f);
			actualPosition += m_widgetsRatio[i]/sumRatio;
		}
	}
}

tgui::Widget::Ptr HorizontalLayout::clone()
{
	return std::make_shared<HorizontalLayout>(*this);
}
}
