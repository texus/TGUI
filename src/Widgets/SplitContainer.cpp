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

#include <TGUI/Widgets/SplitContainer.hpp>
#include <TGUI/Backend/Window/BackendGui.hpp>
#include <cmath> // floor

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
#if TGUI_COMPILED_WITH_CPP_VER < 17
    constexpr const char SplitContainer::StaticWidgetType[];
#endif

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    SplitContainer::SplitContainer(const char* typeName, bool initRenderer) :
        Group{typeName, false}
    {
        m_isolatedFocus = true;

        if (initRenderer)
        {
            m_renderer = aurora::makeCopied<SplitContainerRenderer>();
            setRenderer(Theme::getDefault()->getRendererNoThrow(m_type));
        }

        updateAvailableSize();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    SplitContainer::Ptr SplitContainer::create(const Layout2d& size)
    {
        auto contaier = std::make_shared<SplitContainer>();
        contaier->setSize(size);
        return contaier;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    SplitContainer::Ptr SplitContainer::copy(const SplitContainer::ConstPtr& contaier)
    {
        if (contaier)
            return std::static_pointer_cast<SplitContainer>(contaier->clone());
        else
            return nullptr;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    SplitContainerRenderer* SplitContainer::getSharedRenderer()
    {
        return aurora::downcast<SplitContainerRenderer*>(Widget::getSharedRenderer());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const SplitContainerRenderer* SplitContainer::getSharedRenderer() const
    {
        return aurora::downcast<const SplitContainerRenderer*>(Widget::getSharedRenderer());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    SplitContainerRenderer* SplitContainer::getRenderer()
    {
        return aurora::downcast<SplitContainerRenderer*>(Widget::getRenderer());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void SplitContainer::setSize(const Layout2d& size)
    {
        Group::setSize(size);
        updateAvailableSize();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void SplitContainer::add(const Widget::Ptr& widgetPtr, const String& widgetName)
    {
        Group::add(widgetPtr, widgetName);
        updateChildren();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool SplitContainer::setWidgetIndex(const Widget::Ptr& widget, std::size_t index)
    {
        if (!Group::setWidgetIndex(widget, index))
            return false; // Index was too high

        updateChildren();
        return true;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void SplitContainer::setOrientation(Orientation orientation)
    {
        m_orientation = orientation;
        updateAvailableSize();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Orientation SplitContainer::getOrientation() const
    {
        return m_orientation;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void SplitContainer::setMinValidSplitterOffset(AbsoluteOrRelativeValue minOffset)
    {
        m_splitterMinOffset = minOffset;
        updateAvailableSize();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    float SplitContainer::getMinValidSplitterOffset() const
    {
        return m_splitterMinOffset.getValue();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void SplitContainer::setMaxValidSplitterOffset(AbsoluteOrRelativeValue maxOffset)
    {
        m_splitterMaxOffset = maxOffset;
        updateAvailableSize();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    float SplitContainer::getMaxValidSplitterOffset() const
    {
        return m_splitterMaxOffset.getValue();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void SplitContainer::setSplitterOffset(AbsoluteOrRelativeValue offset)
    {
        m_splitterOffset = offset;
        updateAvailableSize();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    float SplitContainer::getSplitterOffset() const
    {
        return m_currentSplitterOffset;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void SplitContainer::setSplitterWidth(float width)
    {
        m_splitterWidth = width;
        updateChildren();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    float SplitContainer::getSplitterWidth() const
    {
        return m_splitterWidth;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void SplitContainer::setMinimumGrabWidth(float width)
    {
        m_minimumGrabWidth = width;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    float SplitContainer::getMinimumGrabWidth() const
    {
        return m_minimumGrabWidth;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool SplitContainer::isMouseOnWidget(Vector2f pos) const
    {
        return FloatRect{getPosition().x, getPosition().y, getSize().x, getSize().y}.contains(pos);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool SplitContainer::leftMousePressed(Vector2f pos)
    {
        const Vector2f localPos = pos - getPosition();

        if (getSplitterInteractRect().contains(localPos))
        {
            m_mouseDown = true;
            m_mouseDownOnSplitter = true;
            if (m_orientation == Orientation::Horizontal)
                m_mouseDownOnSplitterOffset = localPos.x - m_paddingCached.getLeft() - m_currentSplitterOffset;
            else
                m_mouseDownOnSplitterOffset = localPos.y - m_paddingCached.getTop() - m_currentSplitterOffset;

            return true;
        }
        else // The mouse is not on top of the thumb
        {
            m_mouseDownOnSplitter = false;
            return Group::leftMousePressed(pos);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void SplitContainer::mouseMoved(Vector2f pos)
    {
        const Vector2f localPos = pos - getPosition();

        if (!m_mouseDown || !m_mouseDownOnSplitter)
        {
            if (m_widgetWithLeftMouseDown)
            {
                m_mouseHoverOnSplitter = false;
                Group::mouseMoved(pos);
            }
            else
            {
                Cursor::Type requestedCursor;
                if (getSplitterInteractRect().contains(localPos))
                {
                    if (m_widgetBelowMouse)
                    {
                        m_widgetBelowMouse->mouseNoLongerOnWidget();
                        m_widgetBelowMouse = nullptr;
                    }

                    if (!m_mouseHover)
                        mouseEnteredWidget();

                    requestedCursor = Cursor::Type::SizeHorizontal;
                    if (m_currentSplitContainerMouseCursor != requestedCursor)
                    {
                        m_currentSplitContainerMouseCursor = requestedCursor;
                        if (m_parentGui)
                            m_parentGui->requestMouseCursor(requestedCursor);
                    }

                    m_mouseHoverOnSplitter = true;
                }
                else // Mouse not on splitter
                {
                    requestedCursor = m_mouseCursor;
                    if (m_currentSplitContainerMouseCursor != requestedCursor)
                    {
                        m_currentSplitContainerMouseCursor = requestedCursor;
                        if (m_parentGui)
                            m_parentGui->requestMouseCursor(requestedCursor);
                    }

                    m_mouseHoverOnSplitter = false;
                    Container::mouseMoved(pos);
                }
            }

            return;
        }

        if (m_orientation == Orientation::Horizontal)
            m_splitterOffset = localPos.x - m_paddingCached.getLeft() - m_mouseDownOnSplitterOffset;
        else
            m_splitterOffset = localPos.y - m_paddingCached.getTop() - m_mouseDownOnSplitterOffset;

        updateChildren();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void SplitContainer::mouseNoLongerOnWidget()
    {
        m_mouseHoverOnSplitter = false;
        Container::mouseNoLongerOnWidget();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void SplitContainer::draw(BackendRenderTarget& target, RenderStates states) const
    {
        Group::draw(target, states);

        Vector2f splitterSize;
        if (m_orientation == Orientation::Horizontal)
        {
            states.transform.translate({m_paddingCached.getLeft() + m_currentSplitterOffset, m_paddingCached.getTop()});
            splitterSize = {m_splitterWidth, getInnerSize().y};
        }
        else
        {
            states.transform.translate({m_paddingCached.getLeft(), m_paddingCached.getTop() + m_currentSplitterOffset});
            splitterSize = {getInnerSize().x, m_splitterWidth};
        }

        if (m_mouseHoverOnSplitter && m_splitterColorHoverCached.isSet())
            target.drawFilledRect(states, splitterSize, Color::applyOpacity(m_splitterColorHoverCached, m_opacityCached));
        else
            target.drawFilledRect(states, splitterSize, Color::applyOpacity(m_splitterColorCached, m_opacityCached));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void SplitContainer::rendererChanged(const String& property)
    {
        if (property == U"SplitterColor")
        {
            m_splitterColorCached = getSharedRenderer()->getSplitterColor();
        }
        else if (property == U"SplitterColorHover")
        {
            m_splitterColorHoverCached = getSharedRenderer()->getSplitterColorHover();
        }
        else
            Group::rendererChanged(property);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::unique_ptr<DataIO::Node> SplitContainer::save(SavingRenderersMap& renderers) const
    {
        auto node = Group::save(renderers);

        if (m_orientation == Orientation::Vertical)
            node->propertyValuePairs[U"Orientation"] = std::make_unique<DataIO::ValueNode>("Vertical");
        else
            node->propertyValuePairs[U"Orientation"] = std::make_unique<DataIO::ValueNode>("Horizontal");

        node->propertyValuePairs[U"MinValidSplitterOffset"] = std::make_unique<DataIO::ValueNode>(m_splitterMinOffset.toString());
        node->propertyValuePairs[U"MaxValidSplitterOffset"] = std::make_unique<DataIO::ValueNode>(m_splitterMaxOffset.toString());
        node->propertyValuePairs[U"SplitterOffset"] = std::make_unique<DataIO::ValueNode>(m_splitterOffset.toString());

        node->propertyValuePairs[U"SplitterWidth"] = std::make_unique<DataIO::ValueNode>(String::fromNumber(m_splitterWidth));
        node->propertyValuePairs[U"MinimumGrabWidth"] = std::make_unique<DataIO::ValueNode>(String::fromNumber(m_minimumGrabWidth));

        return node;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void SplitContainer::load(const std::unique_ptr<DataIO::Node>& node, const LoadingRenderersMap& renderers)
    {
        Group::load(node, renderers);

        if (node->propertyValuePairs[U"Orientation"])
        {
            if (node->propertyValuePairs[U"Orientation"]->value == U"Vertical")
                setOrientation(Orientation::Vertical);
            else if (node->propertyValuePairs[U"Orientation"]->value == U"Horizontal")
                setOrientation(Orientation::Horizontal);
            else
                throw Exception{U"Failed to parse Orientation property. Only the values Horizontal and Vertical are correct."};
        }

        if (node->propertyValuePairs[U"MinValidSplitterOffset"])
            setMinValidSplitterOffset(node->propertyValuePairs[U"MinValidSplitterOffset"]->value);
        if (node->propertyValuePairs[U"MaxValidSplitterOffset"])
            setMaxValidSplitterOffset(node->propertyValuePairs[U"MaxValidSplitterOffset"]->value);
        if (node->propertyValuePairs[U"SplitterOffset"])
            setSplitterOffset({node->propertyValuePairs[U"SplitterOffset"]->value});

        if (node->propertyValuePairs[U"SplitterWidth"])
            setSplitterWidth(Deserializer::deserialize(ObjectConverter::Type::Number, node->propertyValuePairs[U"SplitterWidth"]->value).getNumber());
        if (node->propertyValuePairs[U"MinimumGrabWidth"])
            setMinimumGrabWidth(Deserializer::deserialize(ObjectConverter::Type::Number, node->propertyValuePairs[U"MinimumGrabWidth"]->value).getNumber());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void SplitContainer::mouseEnteredWidget()
    {
#if TGUI_HAS_WINDOW_BACKEND_SFML && (SFML_VERSION_MAJOR == 2) && (SFML_VERSION_MINOR < 6)
        if (m_mouseCursor != Cursor::Type::Arrow)
        {
            // Container::mouseEnteredWidget() can't be called from here because of a bug in SFML < 2.6.
            // Calling the function from the base class would set the mouse cursor that was requested. If the mouse is on top
            // of the splitter then we need to replace it with a horizontal resize cursor afterwards. These cursor changes would
            // occur out of order though, causing the wrong cursor to show up when the mouse enters a border from the outside.
            m_mouseHover = true;
            onMouseEnter.emit(this);
            m_currentSplitContainerMouseCursor = Cursor::Type::Arrow;
            return;
        }
#endif

        Group::mouseEnteredWidget();

        // If the split container has a custom mouse cursor then the Container::mouseEnteredWidget() would have switched to it.
        // We should recheck whether the mouse is on top of the splitter to change it back into a horizontal resize arrow if needed.
        // The check would be called after mouseEnteredWidget() anyway, so we just make sure that the code realizes that
        // the mouse cursor has been changed by resetting the state.
        m_currentSplitContainerMouseCursor = m_mouseCursor;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void SplitContainer::mouseLeftWidget()
    {
        if (m_currentSplitContainerMouseCursor != Cursor::Type::Arrow)
        {
            m_currentSplitContainerMouseCursor = Cursor::Type::Arrow;
            if (m_parentGui)
                m_parentGui->requestMouseCursor(Cursor::Type::Arrow);
        }

        Group::mouseLeftWidget();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Widget::Ptr SplitContainer::clone() const
    {
        return std::make_shared<SplitContainer>(*this);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void SplitContainer::updateAvailableSize()
    {
        const float availableSize = (m_orientation == Orientation::Horizontal) ? getInnerSize().x : getInnerSize().y;
        m_splitterMinOffset.updateParentSize(availableSize);
        m_splitterMaxOffset.updateParentSize(availableSize);
        m_splitterOffset.updateParentSize(availableSize);

        updateChildren();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void SplitContainer::updateChildren()
    {
        // Make certain that the splitter offet respects the minimum and maximum values
        const Vector2f innerSize = getInnerSize();
        const float availableSize = (m_orientation == Orientation::Horizontal) ? innerSize.x : innerSize.y;
        const float maxAvailableOffset = std::max(0.f, availableSize - m_splitterWidth);
        const float minOffset = clamp(m_splitterMinOffset.getValue(), 0.f, maxAvailableOffset);
        const float maxOffset = clamp(m_splitterMaxOffset.getValue(), 0.f, maxAvailableOffset);
        m_currentSplitterOffset = std::floor(clamp(m_splitterOffset.getValue(), minOffset, maxOffset));

        if (m_widgets.empty())
            return;

        if (m_orientation == Orientation::Horizontal)
            m_widgets[0]->setSize({m_currentSplitterOffset, innerSize.y});
        else // m_orientation == Orientation::Vertical
            m_widgets[0]->setSize({innerSize.x, m_currentSplitterOffset});

        m_widgets[0]->setPosition({0, 0});

        if (m_widgets.size() < 2)
            return;

        if (m_orientation == Orientation::Horizontal)
        {
            m_widgets[1]->setSize({availableSize - m_splitterWidth - m_currentSplitterOffset, innerSize.y});
            m_widgets[1]->setPosition({m_currentSplitterOffset + m_splitterWidth, 0});
        }
        else // m_orientation == Orientation::Vertical
        {
            m_widgets[1]->setSize({innerSize.x, availableSize - m_splitterWidth - m_currentSplitterOffset});
            m_widgets[1]->setPosition({0, m_currentSplitterOffset + m_splitterWidth});
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    FloatRect SplitContainer::getSplitterInteractRect() const
    {
        if (m_orientation == Orientation::Horizontal)
        {
            float left = m_paddingCached.getLeft() + m_currentSplitterOffset;
            float width = m_splitterWidth;
            if (m_minimumGrabWidth > m_splitterWidth)
            {
                const float paddingAroundSplitter = (m_minimumGrabWidth - m_splitterWidth / 2.f);
                left -= paddingAroundSplitter;
                width += 2 * paddingAroundSplitter;
            }
            return {left, m_paddingCached.getTop(), width, getInnerSize().y};
        }
        else
        {
            float top = m_paddingCached.getTop() + m_currentSplitterOffset;
            float height = m_splitterWidth;
            if (m_minimumGrabWidth > m_splitterWidth)
            {
                const float paddingAroundSplitter = (m_minimumGrabWidth - m_splitterWidth / 2.f);
                top -= paddingAroundSplitter;
                height += 2 * paddingAroundSplitter;
            }
            return {m_paddingCached.getLeft(), top, getInnerSize().x, height};
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
