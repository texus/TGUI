#include <TGUI/VerticalLayout.hpp>

namespace tgui
{

VerticalLayout::VerticalLayout()
{
	m_callback.widgetType = "VerticalLayout";
}

VerticalLayout::VerticalLayout(const VerticalLayout& layoutToCopy) :
	BoxLayout{layoutToCopy}
{
}

VerticalLayout& VerticalLayout::operator=(const VerticalLayout& right)
{
	if (this != &right)
		BoxLayout::operator=(right);
	return *this;
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
	const float sumRatio{std::accumulate(m_widgetsRatio.begin(), m_widgetsRatio.end(), 0.f)};
	float actualPosition{0.f};
	for (unsigned int i = 0; i < m_layoutWidgets.size(); ++i)
	{
		if (m_layoutWidgets[i])
		{
			m_layoutWidgets[i]->setSize(m_size.x, m_size.y*m_widgetsRatio[i]/sumRatio);
			m_layoutWidgets[i]->setPosition(0.f, m_size.y*actualPosition);
			actualPosition += m_widgetsRatio[i]/sumRatio;
		}
	}
}

tgui::Widget::Ptr VerticalLayout::clone()
{
	return std::make_shared<VerticalLayout>(*this);
}
}
