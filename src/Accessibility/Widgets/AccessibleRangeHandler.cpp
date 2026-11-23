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

#include <TGUI/Accessibility/AccessibleWidgetHandler.hpp>
#include <TGUI/Accessibility/AccessibilityTree.hpp>
#include <TGUI/Widgets/Knob.hpp>
#include <TGUI/Widgets/ProgressBar.hpp>
#include <TGUI/Widgets/Scrollbar.hpp>
#include <TGUI/Widgets/Slider.hpp>
#include <TGUI/Widgets/SpinButton.hpp>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    namespace priv
    {
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        bool AccessibleRangeHandler::getState(const Widget& widget, const String& part, AccessibleState& state) const
        {
            if (!part.empty())
                return false;

            AccessibleRangeInfo info;
            if (const auto* slider = dynamic_cast<const Slider*>(&widget))
            {
                state.role = AccessibleRole::Slider;
                state.orientation = slider->getOrientation();
                info.value = static_cast<double>(slider->getValue());
                info.minimum = static_cast<double>(slider->getMinimum());
                info.maximum = static_cast<double>(slider->getMaximum());
                info.smallChange = (slider->getStep() > 0) ? static_cast<double>(slider->getStep())
                                                           : ((info.maximum - info.minimum) / 100.0);
            }
            else if (const auto* knob = dynamic_cast<const Knob*>(&widget))
            {
                state.role = AccessibleRole::Slider;
                info.value = static_cast<double>(knob->getValue());
                info.minimum = static_cast<double>(knob->getMinimum());
                info.maximum = static_cast<double>(knob->getMaximum());
                info.smallChange = (info.maximum - info.minimum) / 100.0;
            }
            else if (const auto* spinButton = dynamic_cast<const SpinButton*>(&widget))
            {
                state.role = AccessibleRole::Spinner;
                state.orientation = spinButton->getOrientation();
                info.value = static_cast<double>(spinButton->getValue());
                info.minimum = static_cast<double>(spinButton->getMinimum());
                info.maximum = static_cast<double>(spinButton->getMaximum());
                info.smallChange = (spinButton->getStep() > 0) ? static_cast<double>(spinButton->getStep()) : 1.0;
            }
            else if (const auto* progressBar = dynamic_cast<const ProgressBar*>(&widget))
            {
                // The user can't change the value of a progress bar
                state.role = AccessibleRole::ProgressBar;
                state.name = progressBar->getText();
                info.value = progressBar->getValue();
                info.minimum = progressBar->getMinimum();
                info.maximum = progressBar->getMaximum();
                info.smallChange = 1;
                info.readOnly = true;
            }
            else if (const auto* scrollbar = dynamic_cast<const Scrollbar*>(&widget))
            {
                state.role = AccessibleRole::ScrollBar;
                state.orientation = scrollbar->getOrientation();
                info.value = scrollbar->getValue();
                info.minimum = 0;
                info.maximum = scrollbar->getMaxValue();
                info.smallChange = scrollbar->getScrollAmount();
                info.largeChange = scrollbar->getViewportSize();
            }
            else
                return false;

            // When the widget has no natural page size, a tenth of the range is used
            if (info.largeChange <= 0)
                info.largeChange = (info.maximum - info.minimum) / 10.0;

            state.rangeInfo = info;
            return true;
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        bool AccessibleRangeHandler::setRangeValue(Widget& widget, const String& part, double value) const
        {
            if (!part.empty())
                return false;

            if (auto* slider = dynamic_cast<Slider*>(&widget))
                slider->setValue(static_cast<float>(value));
            else if (auto* knob = dynamic_cast<Knob*>(&widget))
                knob->setValue(static_cast<float>(value));
            else if (auto* spinButton = dynamic_cast<SpinButton*>(&widget))
                spinButton->setValue(static_cast<float>(value));
            else if (auto* scrollbar = dynamic_cast<Scrollbar*>(&widget))
                scrollbar->setValue(static_cast<unsigned int>(value + 0.5));
            else
                return false;

            return true;
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        void AccessibleRangeHandler::connectSignals(Widget& widget, AccessibilityTree& tree) const
        {
            AccessibleWidgetHandler::connectSignals(widget, tree);

            Signal* valueSignal = nullptr;
            if (auto* slider = dynamic_cast<Slider*>(&widget))
                valueSignal = &slider->onValueChange;
            else if (auto* knob = dynamic_cast<Knob*>(&widget))
                valueSignal = &knob->onValueChange;
            else if (auto* spinButton = dynamic_cast<SpinButton*>(&widget))
                valueSignal = &spinButton->onValueChange;
            else if (auto* progressBar = dynamic_cast<ProgressBar*>(&widget))
                valueSignal = &progressBar->onValueChange;
            else if (auto* scrollbar = dynamic_cast<Scrollbar*>(&widget))
                valueSignal = &scrollbar->onValueChange;

            if (valueSignal)
                connectSignal(*valueSignal, widget, tree, AccessibleEvent::PropertyChanged, AccessibleProperty::RangeValue);
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        void AccessibleRangeHandler::disconnectSignals(Widget& widget) const
        {
            AccessibleWidgetHandler::disconnectSignals(widget);

            if (auto* slider = dynamic_cast<Slider*>(&widget))
                disconnectSignal(slider->onValueChange);
            else if (auto* knob = dynamic_cast<Knob*>(&widget))
                disconnectSignal(knob->onValueChange);
            else if (auto* spinButton = dynamic_cast<SpinButton*>(&widget))
                disconnectSignal(spinButton->onValueChange);
            else if (auto* progressBar = dynamic_cast<ProgressBar*>(&widget))
                disconnectSignal(progressBar->onValueChange);
            else if (auto* scrollbar = dynamic_cast<Scrollbar*>(&widget))
                disconnectSignal(scrollbar->onValueChange);
        }
    } // namespace priv
} // namespace tgui
