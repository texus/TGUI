/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus' Graphical User Interface
// Copyright (C) 2012-2020 Bruno Van de Velde (vdv_b@tgui.eu)
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
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    SpinControl::SpinControl(float min, float max, float value, unsigned decimal, float step) :
        m_decimalPlaces(decimal)
    {
        m_type = "SpinControl";

        m_spinButton = SpinButton::create();
        m_spinText = EditBox::create();
        m_spinText->setInputValidator(EditBox::Validator::Float);
        setString(String(value));

        m_spinButton->setMinimum(min);
        m_spinButton->setMaximum(max);
        m_spinButton->setValue(value);
        m_spinButton->setStep(step);

        m_spinText->setSize(m_spinText->getSize().x, m_spinButton->getSize().y);

        m_container->add(m_spinText, "SpinText");
        m_container->add(m_spinButton, "SpinButton");

        init();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    SpinControl::Ptr SpinControl::create(float min, float max, float value, unsigned decimal, float step)
    {
        return std::make_shared<SpinControl>(min, max, value, decimal, step);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    SpinControl::Ptr SpinControl::copy(SpinControl::ConstPtr spinCtrl)
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

    const SpinButtonRenderer* SpinControl::getSpinButtonRenderer() const
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

    const EditBoxRenderer* SpinControl::getSpinTextRenderer() const
    {
        return m_spinText->getRenderer();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void SpinControl::setSize(const Layout2d& size)
    {
        SubwidgetContainer::setSize(size);

        if (getSize().x > getSize().y)
        {
            m_spinButton->setSize({getSize().y / 2, getSize().y});
            m_spinText->setSize({getSize().x - getSize().y / 2, getSize().y});
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
            setString(String(value));
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
        setString(String(getValue()));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned SpinControl::getDecimalPlaces() const
    {
        return m_decimalPlaces;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::unique_ptr<DataIO::Node> SpinControl::save(SavingRenderersMap& renderers) const
    {
        auto node = SubwidgetContainer::save(renderers);
        node->propertyValuePairs["DecimalPlaces"] = std::make_unique<DataIO::ValueNode>(String::fromNumber(m_decimalPlaces));
        return node;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void SpinControl::load(const std::unique_ptr<DataIO::Node>& node, const LoadingRenderersMap& renderers)
    {
        SubwidgetContainer::load(node, renderers);

        if (node->propertyValuePairs["DecimalPlaces"])
            setDecimalPlaces(node->propertyValuePairs["DecimalPlaces"]->value.toUInt());

        m_spinText = m_container->get<tgui::EditBox>("SpinText");
        m_spinButton = m_container->get<tgui::SpinButton>("SpinButton");

        init();
        setString(String(m_spinButton->getValue()));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void SpinControl::init()
    {
        m_spinButton->setPosition(bindRight(m_spinText), bindTop(m_spinText));
        m_spinButton->onValueChange([this](const float val) {
            setString(String(val));
            onValueChange.emit(this, val);
        });

        m_spinText->onReturnOrUnfocus([this](const String& text) {
            const float curValue = m_spinButton->getValue();
            const float defValue = m_spinButton->getMaximum() + 1;
            const float val = text.toFloat(defValue);
            if (val == defValue || !inRange(val))
            {
                setString(String(curValue));
            }
            else if (curValue != val)
            {
                m_spinButton->setValue(val);

                // Display actual value because SpinButton can round entered number
                setString(String(m_spinButton->getValue()));
            }
            else
                setString(text);
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
        const auto pos = str.find('.');
        const auto integerPart = str.substr(0, pos);
        if (m_decimalPlaces == 0)
        {
            m_spinText->setText(integerPart);
            return;
        }

        String floatPart = ".";
        if (pos != String::npos)
        {
            floatPart = str.substr(pos);
        }
        const auto len = floatPart.size() - 1;

        if (len < m_decimalPlaces)
        {
            m_spinText->setText(integerPart + floatPart + String(m_decimalPlaces - len, '0'));
        }
        else if (len == m_decimalPlaces)
        {
            m_spinText->setText(str);
        }
        else
        {
            m_spinText->setText(integerPart + floatPart.substr(pos, m_decimalPlaces + 1));
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
