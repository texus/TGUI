////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus' Graphical User Interface
// Copyright (C) 2012-2026 Bruno Van de Velde (vdv_b@tgui.eu)
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
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <TGUI/Backend/Window/Backend.hpp>

#include <TGUI/Widgets/LinkLabel.hpp>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    LinkLabel::LinkLabel(const char* typeName, bool initRenderer) :
        Label{typeName, false}
    {
        m_mouseCursor = Cursor::Type::Hand;

        if (initRenderer)
        {
            m_renderer = makeCopied<LinkLabelRenderer>();
            setRenderer(Theme::getDefault()->getRendererNoThrow(m_type));

            setTextSize(getGlobalTextSize());
        }
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    LinkLabel::Ptr LinkLabel::create(const String& text, const String& url)
    {
        auto label = std::make_shared<LinkLabel>();

        if (!text.empty())
            label->setText(text);

        label->setUrl(url);
        return label;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    LinkLabel::Ptr LinkLabel::copy(const LinkLabel::ConstPtr& label)
    {
        if (label)
            return std::static_pointer_cast<LinkLabel>(label->clone());
        return nullptr;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    LinkLabelRenderer* LinkLabel::getSharedRenderer()
    {
        return downcast<LinkLabelRenderer*>(Widget::getSharedRenderer());
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const LinkLabelRenderer* LinkLabel::getSharedRenderer() const
    {
        return downcast<const LinkLabelRenderer*>(Widget::getSharedRenderer());
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    LinkLabelRenderer* LinkLabel::getRenderer()
    {
        return downcast<LinkLabelRenderer*>(Widget::getRenderer());
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void LinkLabel::setUrl(const String& url)
    {
        m_url = url;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const String& LinkLabel::getUrl() const
    {
        return m_url;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void LinkLabel::leftMouseReleased(Vector2f pos)
    {
        // The label only treats the release as a click if the mouse went down on the label and not on its scrollbar
        const bool clicked = m_mouseDown && (!m_scrollbar->isShown() || !m_scrollbar->isMouseDown());

        Label::leftMouseReleased(pos);

        if (clicked && !m_url.empty() && isBackendSet())
            getBackend()->openUrl(m_url);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void LinkLabel::mouseEnteredWidget()
    {
        Label::mouseEnteredWidget();
        updateTextColor();
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void LinkLabel::mouseLeftWidget()
    {
        Label::mouseLeftWidget();
        updateTextColor();
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void LinkLabel::rendererChanged(const String& property)
    {
        if (property == U"TextColorHover")
        {
            m_textColorHoverCached = getSharedRenderer()->getTextColorHover();
            updateTextColor();
        }
        else
        {
            Label::rendererChanged(property);

            // The label sets the normal text color on the text when the color changes
            if (property == U"TextColor")
                updateTextColor();
        }
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::unique_ptr<DataIO::Node> LinkLabel::save(SavingRenderersMap& renderers) const
    {
        auto node = Label::save(renderers);

        if (!m_url.empty())
            node->propertyValuePairs[U"Url"] = std::make_unique<DataIO::ValueNode>(Serializer::serialize(m_url));

        return node;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void LinkLabel::load(const std::unique_ptr<DataIO::Node>& node, const LoadingRenderersMap& renderers)
    {
        Label::load(node, renderers);

        if (node->propertyValuePairs[U"Url"])
            setUrl(Deserializer::deserialize(ObjectConverter::Type::String, node->propertyValuePairs[U"Url"]->value).getString());
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void LinkLabel::rearrangeText()
    {
        Label::rearrangeText();

        // The text pieces were recreated with the normal text color
        updateTextColor();
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void LinkLabel::updateTextColor()
    {
        const Color color = (m_mouseHover && m_textColorHoverCached.isSet()) ? m_textColorHoverCached : m_textColorCached;

        for (auto& line : m_lines)
        {
            for (auto& textPiece : line)
                textPiece.setColor(color);
        }
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Widget::Ptr LinkLabel::clone() const
    {
        return std::make_shared<LinkLabel>(*this);
    }
} // namespace tgui
