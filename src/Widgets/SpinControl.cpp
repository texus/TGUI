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

#include <TGUI/Widgets/SpinControl.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
#if TGUI_COMPILED_WITH_CPP_VER < 17
    constexpr const char SpinControl::StaticWidgetType[];
#endif

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    SpinControl::SpinControl(const char* typeName, bool initRenderer) :
        SubwidgetContainer{typeName, initRenderer}
    {
        m_spinText->setInputValidator(EditBox::Validator::Float);
        m_spinText->setSize(m_spinText->getSize().x - m_spinButton->getSize().x, m_spinButton->getSize().y);

        m_container->add(m_spinText, "SpinText");
        m_container->add(m_spinButton, "SpinButton");

        init();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    SpinControl::SpinControl(const SpinControl& other) :
        SubwidgetContainer{other},
        onValueChange     {other.onValueChange},
        m_decimalPlaces   {other.m_decimalPlaces},
        m_useWideArrows   {other.m_useWideArrows},
        m_spinButton      {m_container->get<SpinButton>(U"SpinButton")},
        m_spinText        {m_container->get<EditBox>(U"SpinText")}
    {
        init();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    SpinControl::SpinControl(SpinControl&& other) noexcept :
        SubwidgetContainer{std::move(other)},
        onValueChange     {std::move(other.onValueChange)},
        m_decimalPlaces   {std::move(other.m_decimalPlaces)},
        m_useWideArrows   {std::move(other.m_useWideArrows)},
        m_spinButton      {std::move(other.m_spinButton)},
        m_spinText        {std::move(other.m_spinText)}
    {
        init();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    SpinControl& SpinControl::operator= (const SpinControl& other)
    {
        if (this != &other)
        {
            SubwidgetContainer::operator=(other);
            onValueChange   = other.onValueChange;
            m_decimalPlaces = other.m_decimalPlaces;
            m_useWideArrows = other.m_useWideArrows;
            m_spinButton    = m_container->get<SpinButton>(U"SpinButton");
            m_spinText      = m_container->get<EditBox>(U"SpinText");

            init();
        }

        return *this;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    SpinControl& SpinControl::operator= (SpinControl&& other) noexcept
    {
        if (this != &other)
        {
            onValueChange   = std::move(other.onValueChange);
            m_decimalPlaces = std::move(other.m_decimalPlaces);
            m_useWideArrows = std::move(other.m_useWideArrows);
            m_spinButton    = std::move(other.m_spinButton);
            m_spinText      = std::move(other.m_spinText);
            SubwidgetContainer::operator=(std::move(other));

            init();
        }

        return *this;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    SpinControl::Ptr SpinControl::create(float min, float max, float value, unsigned int decimal, float step)
    {
        auto spinControl = std::make_shared<SpinControl>();
        spinControl->setMinimum(min);
        spinControl->setMaximum(max);
        spinControl->setValue(value);
        spinControl->setString(String::fromNumberRounded(value, decimal));
        spinControl->setDecimalPlaces(decimal);
        spinControl->setStep(step);
        return spinControl;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    SpinControl::Ptr SpinControl::copy(const SpinControl::ConstPtr& spinCtrl)
    {
        if (spinCtrl)
            return std::static_pointer_cast<SpinControl>(spinCtrl->clone());
        else
            return nullptr;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    SpinButtonRenderer* SpinControl::getSpinButtonSharedRenderer()
    {
        return m_spinButton->getSharedRenderer();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const SpinButtonRenderer* SpinControl::getSpinButtonSharedRenderer() const
    {
        return m_spinButton->getSharedRenderer();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    SpinButtonRenderer* SpinControl::getSpinButtonRenderer()
    {
        return m_spinButton->getRenderer();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    EditBoxRenderer* SpinControl::getSpinTextSharedRenderer()
    {
        return m_spinText->getSharedRenderer();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const EditBoxRenderer* SpinControl::getSpinTextSharedRenderer() const
    {
        return m_spinText->getSharedRenderer();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    EditBoxRenderer* SpinControl::getSpinTextRenderer()
    {
        return m_spinText->getRenderer();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void SpinControl::setSize(const Layout2d& size)
    {
        SubwidgetContainer::setSize(size);

        if (getSize().x > getSize().y)
        {
            if (m_useWideArrows)
            {
                m_spinButton->setSize({getSize().y, getSize().y});
                m_spinText->setSize({getSize().x - getSize().y, getSize().y});
            }
            else
            {
                m_spinButton->setSize({getSize().y / 2, getSize().y});
                m_spinText->setSize({getSize().x - getSize().y / 2, getSize().y});
            }
        }
        else
        {
            m_spinButton->setSize({getSize().x, getSize().y});
            m_spinText->setSize({0, 0});
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void SpinControl::setMinimum(float minimum)
    {
        m_spinButton->setMinimum(minimum);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    float SpinControl::getMinimum() const
    {
        return m_spinButton->getMinimum();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void SpinControl::setMaximum(float maximum)
    {
        m_spinButton->setMaximum(maximum);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    float SpinControl::getMaximum() const
    {
        return m_spinButton->getMaximum();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool SpinControl::setValue(float value)
    {
        if (m_spinButton->getValue() != value && inRange(value))
        {
            m_spinButton->setValue(value);
            setString(String::fromNumberRounded(value, m_decimalPlaces));
            return true;
        }
        return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    float SpinControl::getValue() const
    {
        return m_spinButton->getValue();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void SpinControl::setStep(float step)
    {
        m_spinButton->setStep(step);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    float SpinControl::getStep() const
    {
        return m_spinButton->getStep();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void SpinControl::setDecimalPlaces(unsigned decimalPlaces)
    {
        m_decimalPlaces = decimalPlaces;
        setString(String::fromNumberRounded(getValue(), m_decimalPlaces));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned SpinControl::getDecimalPlaces() const
    {
        return m_decimalPlaces;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void SpinControl::setUseWideArrows(bool useWideArrows)
    {
        m_useWideArrows = useWideArrows;
        setSize(m_size);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool SpinControl::getUseWideArrows() const
    {
        return m_useWideArrows;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::unique_ptr<DataIO::Node> SpinControl::save(SavingRenderersMap& renderers) const
    {
        auto node = SubwidgetContainer::save(renderers);
        node->propertyValuePairs[U"DecimalPlaces"] = std::make_unique<DataIO::ValueNode>(String::fromNumber(m_decimalPlaces));
        node->propertyValuePairs[U"UseWideArrows"] = std::make_unique<DataIO::ValueNode>(Serializer::serialize(m_useWideArrows));
        return node;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void SpinControl::load(const std::unique_ptr<DataIO::Node>& node, const LoadingRenderersMap& renderers)
    {
        SubwidgetContainer::load(node, renderers);

        if (node->propertyValuePairs[U"DecimalPlaces"])
            setDecimalPlaces(node->propertyValuePairs[U"DecimalPlaces"]->value.toUInt());
        if (node->propertyValuePairs[U"UseWideArrows"])
            setUseWideArrows(Deserializer::deserialize(ObjectConverter::Type::Bool, node->propertyValuePairs[U"UseWideArrows"]->value).getBool());

        m_spinText = m_container->get<EditBox>("SpinText");
        m_spinButton = m_container->get<SpinButton>("SpinButton");

        if (!m_spinText || !m_spinButton)
            throw Exception{U"Failed to find SpinText and SpinButton children while loading SpinControl"};

        init();
        setString(String::fromNumberRounded(m_spinButton->getValue(), m_decimalPlaces));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void SpinControl::init()
    {
        m_spinButton->setPosition(bindRight(m_spinText), bindTop(m_spinText));

        m_spinButton->onValueChange.disconnectAll();
        m_spinButton->onValueChange([this](const float val) {
            setString(String::fromNumberRounded(val, m_decimalPlaces));
            onValueChange.emit(this, val);
        });

        m_spinText->onReturnOrUnfocus.disconnectAll();
        m_spinText->onReturnOrUnfocus([this](const String& text) {
            const float curValue = m_spinButton->getValue();
            const float defValue = m_spinButton->getMaximum() + 1;
            const float val = text.toFloat(defValue);
            if (val == defValue || !inRange(val))
            {
                setString(String::fromNumberRounded(curValue, m_decimalPlaces));
            }
            else if (curValue != val)
            {
                m_spinButton->setValue(val);

                // Display actual value because SpinButton can round entered number
                setString(String::fromNumberRounded(m_spinButton->getValue(), m_decimalPlaces));
            }
            else
                setString(String::fromNumberRounded(val, m_decimalPlaces));
        });

        const auto buttonSize = m_spinButton->getSize();
        const auto txtSize = m_spinText->getSize();
        SubwidgetContainer::setSize({buttonSize.x + txtSize.x, buttonSize.y});
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool SpinControl::inRange(const float value) const
    {
        return m_spinButton->getMinimum() <= value && value <= m_spinButton->getMaximum();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void SpinControl::setString(const String& str)
    {
        m_spinText->setText(str);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Signal& SpinControl::getSignal(String signalName)
    {
        if (signalName == onValueChange.getName())
            return onValueChange;
        else
            return Widget::getSignal(std::move(signalName));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Widget::Ptr SpinControl::clone() const
    {
        return std::make_shared<SpinControl>(*this);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
