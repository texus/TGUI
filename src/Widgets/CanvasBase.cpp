/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus' Graphical User Interface
// Copyright (C) 2012-2024 Bruno Van de Velde (vdv_b@tgui.eu)
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

#include <TGUI/Widgets/CanvasBase.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    CanvasBase::CanvasBase(const char* typeName, bool initRenderer) :
        ClickableWidget{typeName, initRenderer}
    {
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void CanvasBase::ignoreMouseEvents(bool ignore)
    {
        m_ignoringMouseEvents = ignore;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool CanvasBase::isIgnoringMouseEvents() const
    {
        return m_ignoringMouseEvents;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool CanvasBase::canGainFocus() const
    {
        return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool CanvasBase::isMouseOnWidget(Vector2f pos) const
    {
        if (m_ignoringMouseEvents)
            return false;

        return ClickableWidget::isMouseOnWidget(pos);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::unique_ptr<DataIO::Node> CanvasBase::save(SavingRenderersMap& renderers) const
    {
        auto node = ClickableWidget::save(renderers);

        if (m_ignoringMouseEvents)
            node->propertyValuePairs[U"IgnoreMouseEvents"] = std::make_unique<DataIO::ValueNode>(Serializer::serialize(m_ignoringMouseEvents));

        return node;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void CanvasBase::load(const std::unique_ptr<DataIO::Node>& node, const LoadingRenderersMap& renderers)
    {
        ClickableWidget::load(node, renderers);

        if (node->propertyValuePairs[U"IgnoreMouseEvents"])
            m_ignoringMouseEvents = Deserializer::deserialize(ObjectConverter::Type::Bool, node->propertyValuePairs[U"IgnoreMouseEvents"]->value).getBool();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
