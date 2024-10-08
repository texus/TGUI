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

#include <TGUI/Widgets/EditBoxSlider.hpp>
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
#if TGUI_COMPILED_WITH_CPP_VER < 17
    constexpr const char EditBoxSlider::StaticWidgetType[];
#endif

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    EditBoxSlider::EditBoxSlider(const char* typeName, bool initRenderer) :
        SubwidgetContainer{typeName, initRenderer}
    {
        m_editBox->setInputValidator(EditBox::Validator::Float);

        m_container->add(m_editBox, "EditBox");
        m_container->add(m_slider, "Slider");

        m_slider->setSize(m_editBox->getSize().x, m_editBox->getSize().y / 3.f);
        m_slider->setOrientation(Orientation::Horizontal);
        init();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    EditBoxSlider::EditBoxSlider(const EditBoxSlider& other) :
        SubwidgetContainer{other},
        onValueChange     {other.onValueChange},
        m_decimalPlaces   {other.m_decimalPlaces},
        m_editBox         {m_container->get<EditBox>(U"EditBox")},
        m_slider          {m_container->get<Slider>(U"Slider")}
    {
        init();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    EditBoxSlider::EditBoxSlider(EditBoxSlider&& other) noexcept :
        SubwidgetContainer{std::move(other)},
        onValueChange     {std::move(other.onValueChange)},
        m_decimalPlaces   {std::move(other.m_decimalPlaces)},
        m_editBox         {std::move(other.m_editBox)},
        m_slider          {std::move(other.m_slider)}
    {
        init();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    EditBoxSlider& EditBoxSlider::operator= (const EditBoxSlider& other)
    {
        if (this != &other)
        {
            SubwidgetContainer::operator=(other);
            onValueChange   = other.onValueChange;
            m_decimalPlaces = other.m_decimalPlaces;
            m_editBox       = m_container->get<EditBox>(U"EditBox");
            m_slider        = m_container->get<Slider>(U"Slider");

            init();
        }

        return *this;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    EditBoxSlider& EditBoxSlider::operator= (EditBoxSlider&& other) noexcept
    {
        if (this != &other)
        {
            onValueChange   = std::move(other.onValueChange);
            m_decimalPlaces = std::move(other.m_decimalPlaces);
            m_editBox       = std::move(other.m_editBox);
            m_slider        = std::move(other.m_slider);
            SubwidgetContainer::operator=(std::move(other));

            init();
        }

        return *this;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    EditBoxSlider::Ptr EditBoxSlider::create(float min, float max, float value, unsigned int decimal, float step)
    {
        auto editBoxSlider = std::make_shared<EditBoxSlider>();

        editBoxSlider->setMinimum(min);
        editBoxSlider->setMaximum(max);
        editBoxSlider->setValue(value);
        editBoxSlider->setDecimalPlaces(decimal);
        editBoxSlider->setStep(step);

        return editBoxSlider;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    EditBoxSlider::Ptr EditBoxSlider::copy(const EditBoxSlider::ConstPtr& editBoxSlider)
    {
        if (editBoxSlider)
            return std::static_pointer_cast<EditBoxSlider>(editBoxSlider->clone());
        else
            return nullptr;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    EditBoxRenderer* EditBoxSlider::getEditBoxSharedRenderer()
    {
        return m_editBox->getSharedRenderer();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const EditBoxRenderer* EditBoxSlider::getEditBoxSharedRenderer() const
    {
        return m_editBox->getSharedRenderer();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    EditBoxRenderer* EditBoxSlider::getEditBoxRenderer()
    {
        return m_editBox->getRenderer();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    SliderRenderer* EditBoxSlider::getSliderSharedRenderer()
    {
        return m_slider->getSharedRenderer();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const SliderRenderer* EditBoxSlider::getSliderSharedRenderer() const
    {
        return m_slider->getSharedRenderer();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    SliderRenderer* EditBoxSlider::getSliderRenderer()
    {
        return m_slider->getRenderer();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void EditBoxSlider::setSize(const Layout2d& size)
    {
        SubwidgetContainer::setSize(size);
        m_editBox->setSize({getSize().x, getSize().y * 0.75f});
        m_slider->setSize({getSize().x, getSize().y * 0.25f});
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Vector2f EditBoxSlider::getFullSize() const
    {
        return {m_slider->getFullSize().x, m_editBox->getSize().y + m_slider->getSize().y + ((m_slider->getFullSize().y - m_slider->getSize().y) / 2.f)};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Vector2f EditBoxSlider::getWidgetOffset() const
    {
        return {m_slider->getWidgetOffset().x, 0};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void EditBoxSlider::setMinimum(float minimum)
    {
        m_slider->setMinimum(minimum);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    float EditBoxSlider::getMinimum() const
    {
        return m_slider->getMinimum();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void EditBoxSlider::setMaximum(float maximum)
    {
        m_slider->setMaximum(maximum);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    float EditBoxSlider::getMaximum() const
    {
        return m_slider->getMaximum();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool EditBoxSlider::setValue(float value)
    {
        if (m_slider->getValue() != value && inRange(value))
        {
            m_slider->setValue(value);
            setString(String::fromNumberRounded(value, m_decimalPlaces));
            return true;
        }
        return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    float EditBoxSlider::getValue() const
    {
        return m_slider->getValue();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void EditBoxSlider::setStep(float step)
    {
        m_slider->setStep(step);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    float EditBoxSlider::getStep() const
    {
        return m_slider->getStep();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void EditBoxSlider::setDecimalPlaces(unsigned decimalPlaces)
    {
        m_decimalPlaces = decimalPlaces;
        setString(String::fromNumberRounded(getValue(), m_decimalPlaces));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned EditBoxSlider::getDecimalPlaces() const
    {
        return m_decimalPlaces;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void EditBoxSlider::setTextAlignment(HorizontalAlignment alignment)
    {
        m_editBox->setAlignment(alignment);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    HorizontalAlignment EditBoxSlider::getTextAlignment() const
    {
        return m_editBox->getAlignment();;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::unique_ptr<DataIO::Node> EditBoxSlider::save(SavingRenderersMap& renderers) const
    {
        auto node = SubwidgetContainer::save(renderers);
        node->propertyValuePairs[U"DecimalPlaces"] = std::make_unique<DataIO::ValueNode>(String::fromNumber(m_decimalPlaces));
        return node;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void EditBoxSlider::load(const std::unique_ptr<DataIO::Node>& node, const LoadingRenderersMap& renderers)
    {
        SubwidgetContainer::load(node, renderers);

        if (node->propertyValuePairs[U"DecimalPlaces"])
            setDecimalPlaces(node->propertyValuePairs[U"DecimalPlaces"]->value.toUInt());

        m_editBox = m_container->get<EditBox>("EditBox");
        m_slider = m_container->get<Slider>("Slider");

        if (!m_editBox || !m_slider)
            throw Exception{U"Failed to find EditBox and Slider children while loading EditBoxSlider"};

        init();
        setString(String::fromNumberRounded(m_slider->getValue(), m_decimalPlaces));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void EditBoxSlider::init()
    {
        m_slider->setPosition(bindLeft(m_editBox), bindBottom(m_editBox));

        m_slider->onValueChange.disconnectAll();
        m_slider->onValueChange([this](const float val) {
            setString(String::fromNumberRounded(val, m_decimalPlaces));
            onValueChange.emit(this, val);
        });

        m_editBox->onTextChange.disconnectAll();
        m_editBox->onTextChange([this](const String &text) {
            float value = text.toFloat();

            if (inRange(value))
                m_slider->setValue(value);
        });

        m_editBox->onReturnOrUnfocus.disconnectAll();
        m_editBox->onReturnOrUnfocus([this](const String &text) {
            const float val = text.toFloat();

            if (!inRange(val))
                setString(String::fromNumberRounded(m_slider->getValue(), m_decimalPlaces));
        });

        const Vector2f editBoxSize = m_editBox->getSize();
        const Vector2f sliderSize = m_slider->getSize();
        SubwidgetContainer::setSize(editBoxSize.x, editBoxSize.y + sliderSize.y);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool EditBoxSlider::inRange(const float value) const
    {
        return m_slider->getMinimum() <= value && value <= m_slider->getMaximum();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void EditBoxSlider::setString(const String& str)
    {
        m_editBox->setText(str);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Signal& EditBoxSlider::getSignal(String signalName)
    {
        if (signalName == onValueChange.getName())
            return onValueChange;
        else
            return Widget::getSignal(std::move(signalName));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Widget::Ptr EditBoxSlider::clone() const
    {
        return std::make_shared<EditBoxSlider>(*this);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
