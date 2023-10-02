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


#include <TGUI/Widgets/CheckBox.hpp>

#if !TGUI_EXPERIMENTAL_USE_STD_MODULE
    #include <cmath>

    #if defined(__cpp_lib_math_constants) && (__cpp_lib_math_constants >= 201907L)
        #include <numbers>
    #endif
#endif

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
#if TGUI_COMPILED_WITH_CPP_VER < 17
    constexpr const char CheckBox::StaticWidgetType[];
#endif

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    CheckBox::CheckBox(const char* typeName, bool initRenderer) :
        RadioButton{typeName, false}
    {
        if (initRenderer)
        {
            m_renderer = aurora::makeCopied<CheckBoxRenderer>();
            setRenderer(Theme::getDefault()->getRendererNoThrow(m_type));

            setTextSize(getGlobalTextSize());
            setSize({m_text.getLineHeight() + m_bordersCached.getLeft() + m_bordersCached.getRight(),
                     m_text.getLineHeight() + m_bordersCached.getTop() + m_bordersCached.getBottom()});
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    CheckBox::Ptr CheckBox::create(const String& text)
    {
        auto checkBox = std::make_shared<CheckBox>();

        if (!text.empty())
            checkBox->setText(text);

        return checkBox;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    CheckBox::Ptr CheckBox::copy(const CheckBox::ConstPtr& checkbox)
    {
        if (checkbox)
            return std::static_pointer_cast<CheckBox>(checkbox->clone());
        else
            return nullptr;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    CheckBoxRenderer* CheckBox::getSharedRenderer()
    {
        return aurora::downcast<CheckBoxRenderer*>(Widget::getSharedRenderer());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const CheckBoxRenderer* CheckBox::getSharedRenderer() const
    {
        return aurora::downcast<const CheckBoxRenderer*>(Widget::getSharedRenderer());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    CheckBoxRenderer* CheckBox::getRenderer()
    {
        return aurora::downcast<CheckBoxRenderer*>(Widget::getRenderer());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Vector2f CheckBox::getFullSize() const
    {
        if (m_spriteUnchecked.isSet() && m_spriteChecked.isSet()
         && (m_textureUncheckedCached.getImageSize() != m_textureCheckedCached.getImageSize()))
        {
            Vector2f sizeDiff = m_spriteChecked.getSize() - m_spriteUnchecked.getSize();
            if (getText().empty())
                return getSize() + Vector2f{std::max(0.f, sizeDiff.x - m_bordersCached.getRight()), std::max(0.f, sizeDiff.y - m_bordersCached.getTop())};
            else
                return getSize() + Vector2f{(getSize().x * m_textDistanceRatioCached) + m_text.getSize().x, std::max(0.f, std::max((m_text.getSize().y - getSize().y) / 2, sizeDiff.y - m_bordersCached.getTop()))};
        }
        else
        {
            if (getText().empty())
                return getSize();
            else
                return {getSize().x + (getSize().x * m_textDistanceRatioCached) + m_text.getSize().x, std::max(getSize().y, m_text.getSize().y)};
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Vector2f CheckBox::getWidgetOffset() const
    {
        float yOffset = 0;
        if (m_spriteUnchecked.isSet() && m_spriteChecked.isSet()
         && (m_textureUncheckedCached.getImageSize() != m_textureCheckedCached.getImageSize()))
        {
            const float sizeDiff = m_spriteChecked.getSize().y - m_spriteUnchecked.getSize().y;
            if (sizeDiff > m_bordersCached.getTop())
                yOffset = sizeDiff - m_bordersCached.getTop();
        }

        if (getText().empty() || (getSize().y >= m_text.getSize().y))
            return {0, -yOffset};
        else
            return {0, -std::max(yOffset, (m_text.getSize().y - getSize().y) / 2)};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void CheckBox::setChecked(bool checked)
    {
        if (checked && !m_checked)
        {
            m_checked = true;

            updateTextColor();
            if (m_textStyleCheckedCached.isSet())
                m_text.setStyle(m_textStyleCheckedCached);
            else
                m_text.setStyle(m_textStyleCached);

            onCheck.emit(this, true);
            onChange.emit(this, true);
        }
        else
            RadioButton::setChecked(checked);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void CheckBox::leftMouseReleased(Vector2f pos)
    {
        const bool mouseDown = m_mouseDown;

        ClickableWidget::leftMouseReleased(pos); // NOLINT(bugprone-parent-virtual-call)

        // Check or uncheck when we clicked on the checkbox (not just mouse release)
        if (mouseDown)
            setChecked(!m_checked);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void CheckBox::keyPressed(const Event::KeyEvent& event)
    {
        // Check or uncheck the checkbox if the space key or the return key was pressed
        if ((event.code == Event::KeyboardKey::Space) || (event.code == Event::KeyboardKey::Enter))
            setChecked(!m_checked);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool CheckBox::canHandleKeyPress(const Event::KeyEvent& event)
    {
        if ((event.code == Event::KeyboardKey::Space) || (event.code == Event::KeyboardKey::Enter))
            return true;
        else
            return RadioButton::canHandleKeyPress(event);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void CheckBox::rendererChanged(const String& property)
    {
        if (property == U"TextureUnchecked")
            m_textureUncheckedCached = getSharedRenderer()->getTextureUnchecked();
        else if (property == U"TextureChecked")
            m_textureCheckedCached = getSharedRenderer()->getTextureChecked();

        RadioButton::rendererChanged(property);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void CheckBox::updateTextureSizes()
    {
        if (m_spriteUnchecked.isSet())
        {
            m_spriteUnchecked.setSize(getInnerSize());
            m_spriteUncheckedHover.setSize(m_spriteUnchecked.getSize());
            m_spriteUncheckedDisabled.setSize(m_spriteUnchecked.getSize());
            m_spriteUncheckedFocused.setSize(m_spriteUnchecked.getSize());
        }

        if (m_spriteChecked.isSet())
        {
            if (m_spriteUnchecked.isSet())
            {
                m_spriteChecked.setSize(
                    {getInnerSize().x + ((m_textureCheckedCached.getImageSize().x - m_textureUncheckedCached.getImageSize().x) * (getInnerSize().x / m_textureUncheckedCached.getImageSize().x)),
                     getInnerSize().y + ((m_textureCheckedCached.getImageSize().y - m_textureUncheckedCached.getImageSize().y) * (getInnerSize().y / m_textureUncheckedCached.getImageSize().y))}
                );
            }
            else
                m_spriteChecked.setSize(getInnerSize());

            m_spriteCheckedHover.setSize(m_spriteChecked.getSize());
            m_spriteCheckedDisabled.setSize(m_spriteChecked.getSize());
            m_spriteCheckedFocused.setSize(m_spriteChecked.getSize());
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void CheckBox::draw(BackendRenderTarget& target, RenderStates states) const
    {
        // Draw the borders
        if (m_bordersCached != Borders{0})
            target.drawBorders(states, m_bordersCached, getSize(), Color::applyOpacity(getCurrentBorderColor(), m_opacityCached));

        states.transform.translate({m_bordersCached.getLeft(), m_bordersCached.getTop()});
        if (!m_checked && m_spriteUnchecked.isSet())
        {
            if (!m_enabled && m_spriteUncheckedDisabled.isSet())
                target.drawSprite(states, m_spriteUncheckedDisabled);
            else if (m_mouseHover && m_spriteUncheckedHover.isSet())
                target.drawSprite(states, m_spriteUncheckedHover);
            else if (m_focused && m_spriteUncheckedFocused.isSet())
                target.drawSprite(states, m_spriteUncheckedFocused);
            else
                target.drawSprite(states, m_spriteUnchecked);
        }
        else if (m_checked && m_spriteChecked.isSet())
        {
            const Sprite* checkedSprite;
            if (!m_enabled && m_spriteCheckedDisabled.isSet())
                checkedSprite = &m_spriteCheckedDisabled;
            else if (m_mouseHover && m_spriteCheckedHover.isSet())
                checkedSprite = &m_spriteCheckedHover;
            else if (m_focused && m_spriteCheckedFocused.isSet())
                checkedSprite = &m_spriteCheckedFocused;
            else
                checkedSprite = &m_spriteChecked;

            // The image may need to be shifted when the check leaves the box
            if (getInnerSize().y != checkedSprite->getSize().y)
            {
                float diff = getInnerSize().y - checkedSprite->getSize().y;

                states.transform.translate({0, diff});
                target.drawSprite(states, *checkedSprite);
                states.transform.translate({0, -diff});
            }
            else // Draw the checked texture normally
                target.drawSprite(states, *checkedSprite);
        }
        else // There are no images
        {
            target.drawFilledRect(states, getInnerSize(), Color::applyOpacity(getCurrentBackgroundColor(), m_opacityCached));

            if (m_checked)
            {
#if defined(__cpp_lib_math_constants) && (__cpp_lib_math_constants >= 201907L)
                const float pi = std::numbers::pi_v<float>;
#else
                const float pi = 3.14159265359f;
#endif
                const Vertex::Color checkColor = Vertex::Color(getCurrentCheckColor());
                const Vector2f size = getInnerSize();
                const float lineThickness = std::min(size.x, size.y) / 5;
                const Vector2f leftPoint = {0.14f * size.x, 0.4f * size.y};
                const Vector2f middlePoint = {0.44f * size.x, 0.7f * size.y};
                const Vector2f rightPoint = {0.86f * size.x, 0.28f * size.y};
                const float x = (lineThickness / 2.f) * std::cos(pi / 4.f);
                const float y = (lineThickness / 2.f) * std::sin(pi / 4.f);
                const std::array<Vertex, 6> vertices = {{
                    {{leftPoint.x - x, leftPoint.y + y}, checkColor},
                    {{leftPoint.x + x, leftPoint.y - y}, checkColor},
                    {{middlePoint.x, middlePoint.y + 2*y}, checkColor},
                    {{middlePoint.x, middlePoint.y - 2*y}, checkColor},
                    {{rightPoint.x + x, rightPoint.y + y}, checkColor},
                    {{rightPoint.x - x, rightPoint.y - y}, checkColor}
                }};
                const std::array<unsigned int, 3u*4u> indices = {{
                    0, 1, 2,
                    1, 2, 3,
                    2, 3, 4,
                    3, 4, 5
                }};
                target.drawVertexArray(states, vertices.data(), vertices.size(), indices.data(), indices.size(), nullptr);
            }
        }
        states.transform.translate({-m_bordersCached.getLeft(), -m_bordersCached.getTop()});

        if (!getText().empty())
        {
            states.transform.translate({(1 + m_textDistanceRatioCached) * getSize().x, (getSize().y - m_text.getSize().y) / 2.0f});
            target.drawText(states, m_text);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Widget::Ptr CheckBox::clone() const
    {
        return std::make_shared<CheckBox>(*this);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
