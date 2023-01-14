/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus' Graphical User Interface
// Copyright (C) 2012-2023 Bruno Van de Velde (vdv_b@tgui.eu)
//
// This software is provided 'as-is', without any express or implied warranty.
// In no event will the authors be held liable for any damages arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it freely,
// subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented;
//    you must not claim that you wrote the original software.
//    If you use this software in a product, an acknowledgment
//    in the product documentation would be appreciated but is not required.
//
// 2. Altered source versions must be plainly marked as such,
//    and must not be misrepresented as being the original software.
//
// 3. This notice may not be removed or altered from any source distribution.
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


#include <TGUI/Widgets/BoxLayoutRatios.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    BoxLayoutRatios::BoxLayoutRatios(const char* typeName, bool initRenderer) :
        BoxLayout{typeName, initRenderer}
    {
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void BoxLayoutRatios::add(const Widget::Ptr& widget, const String& widgetName)
    {
        insert(m_widgets.size(), widget, 1, widgetName);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void BoxLayoutRatios::add(const Widget::Ptr& widget, float ratio, const String& widgetName)
    {
        insert(m_widgets.size(), widget, ratio, widgetName);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void BoxLayoutRatios::insert(std::size_t index, const Widget::Ptr& widget, const String& widgetName)
    {
        insert(index, widget, 1, widgetName);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void BoxLayoutRatios::insert(std::size_t index, const Widget::Ptr& widget, float ratio, const String& widgetName)
    {
        if (index < m_ratios.size())
            m_ratios.insert(m_ratios.begin() + static_cast<std::ptrdiff_t>(index), ratio);
        else
            m_ratios.push_back(ratio);

        BoxLayout::insert(index, widget, widgetName);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool BoxLayoutRatios::remove(std::size_t index)
    {
        if (index < m_ratios.size())
            m_ratios.erase(m_ratios.begin() + static_cast<std::ptrdiff_t>(index));

        return BoxLayout::remove(index);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void BoxLayoutRatios::removeAllWidgets()
    {
        BoxLayout::removeAllWidgets();
        m_ratios.clear();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void BoxLayoutRatios::addSpace(float ratio)
    {
        insertSpace(m_widgets.size(), ratio);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void BoxLayoutRatios::insertSpace(std::size_t index, float ratio)
    {
        insert(index, Group::create(), ratio, "#TGUI_INTERNAL$HorizontalLayoutSpace#");
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool BoxLayoutRatios::setRatio(const Widget::Ptr& widget, float ratio)
    {
        for (std::size_t i = 0; i < m_widgets.size(); ++i)
        {
            if (m_widgets[i] == widget)
                return setRatio(i, ratio);
        }

        return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool BoxLayoutRatios::setRatio(std::size_t index, float ratio)
    {
        if (index >= m_ratios.size())
            return false;

        m_ratios[index] = ratio;
        updateWidgets();
        return true;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    float BoxLayoutRatios::getRatio(const Widget::Ptr& widget) const
    {
        for (std::size_t i = 0; i < m_widgets.size(); ++i)
        {
            if (m_widgets[i] == widget)
                return getRatio(i);
        }

        return 0;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    float BoxLayoutRatios::getRatio(std::size_t index) const
    {
        if (index >= m_ratios.size())
            return 0;

        return m_ratios[index];
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::unique_ptr<DataIO::Node> BoxLayoutRatios::save(SavingRenderersMap& renderers) const
    {
        auto node = BoxLayout::save(renderers);

        if (m_widgets.size() > 0)
        {
            String ratioList = "[" + Serializer::serialize(m_ratios[0]);
            for (std::size_t i = 1; i < m_widgets.size(); ++i)
                ratioList += ", " + Serializer::serialize(m_ratios[i]);

            ratioList += "]";
            node->propertyValuePairs[U"Ratios"] = std::make_unique<DataIO::ValueNode>(ratioList);
        }

        return node;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void BoxLayoutRatios::load(const std::unique_ptr<DataIO::Node>& node, const LoadingRenderersMap& renderers)
    {
        BoxLayout::load(node, renderers);

        if (node->propertyValuePairs[U"Ratios"])
        {
            if (!node->propertyValuePairs[U"Ratios"]->listNode)
                throw Exception{U"Failed to parse 'Ratios' property, expected a list as value"};

            if (node->propertyValuePairs[U"Ratios"]->valueList.size() != getWidgets().size())
                throw Exception{U"Amounts of values for 'Ratios' differs from the amount in child widgets"};

            for (std::size_t i = 0; i < node->propertyValuePairs[U"Ratios"]->valueList.size(); ++i)
                setRatio(i, Deserializer::deserialize(ObjectConverter::Type::Number, node->propertyValuePairs[U"Ratios"]->valueList[i]).getNumber());
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
