/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
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
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "Tests.hpp"

#include <TGUI/Accessibility/AccessibilityTree.hpp>
#include <TGUI/Accessibility/AccessibleObject.hpp>

#include <algorithm>

namespace
{
    // Records what the tree reports, like a platform backend would receive it
    class RecordingListener : public tgui::priv::AccessibilityEventListener
    {
    public:
        struct Event
        {
            tgui::priv::AccessibleEvent event;
            tgui::priv::AccessibleObject::Ptr object;
            tgui::priv::AccessibleProperty property;
        };

        void accessibleObjectsChanged(const std::vector<tgui::priv::AccessibleObject::Ptr>& objects) override
        {
            changedObjects.insert(changedObjects.end(), objects.begin(), objects.end());
        }

        void accessibleEventOccurred(tgui::priv::AccessibleEvent event,
                                     const tgui::priv::AccessibleObject::Ptr& object,
                                     tgui::priv::AccessibleProperty property) override
        {
            events.push_back({event, object, property});
        }

        // Returns how many events of the given type were reported for the object
        [[nodiscard]] std::size_t count(tgui::priv::AccessibleEvent event, const tgui::priv::AccessibleObject::Ptr& object) const
        {
            return static_cast<std::size_t>(std::count_if(events.begin(), events.end(), [&](const Event& e)
                                                          { return (e.event == event) && (e.object == object); }));
        }

        // Returns how many PropertyChanged events for the given property were reported for the object
        [[nodiscard]] std::size_t countProperty(tgui::priv::AccessibleProperty property,
                                                const tgui::priv::AccessibleObject::Ptr& object) const
        {
            return static_cast<std::size_t>(std::count_if(
                events.begin(),
                events.end(),
                [&](const Event& e)
                { return (e.event == tgui::priv::AccessibleEvent::PropertyChanged) && (e.object == object) && (e.property == property); }));
        }

        void clear()
        {
            events.clear();
            changedObjects.clear();
        }

        std::vector<Event> events;
        std::vector<tgui::priv::AccessibleObject::Ptr> changedObjects;
    };
} // anonymous namespace

TEST_CASE("[Accessibility]")
{
    using tgui::priv::AccessibleEvent;
    using tgui::priv::AccessibleProperty;

    GuiNull gui;
    RecordingListener listener;
    tgui::priv::AccessibilityTree tree{gui, listener};

    auto button = tgui::Button::create("OK");
    gui.add(button, "OkButton");

    auto editBox = tgui::EditBox::create();
    editBox->setText("Hello");
    gui.add(editBox, "NameEdit");

    auto checkBox = tgui::CheckBox::create("Sound");
    gui.add(checkBox, "SoundCheck");

    SECTION("Roles and names")
    {
        const auto buttonObject = tree.findWidgetObject(button);
        REQUIRE(buttonObject);
        REQUIRE(buttonObject->getRole() == tgui::AccessibleRole::Button);
        REQUIRE(buttonObject->getName() == "OK");

        const auto editBoxObject = tree.findWidgetObject(editBox);
        REQUIRE(editBoxObject->getRole() == tgui::AccessibleRole::Edit);
        REQUIRE(editBoxObject->getState().value == tgui::String{"Hello"});

        auto textArea = tgui::TextArea::create();
        gui.add(textArea);
        REQUIRE(tree.findWidgetObject(textArea)->getRole() == tgui::AccessibleRole::MultiLineEdit);

        const auto checkBoxObject = tree.findWidgetObject(checkBox);
        REQUIRE(checkBoxObject->getRole() == tgui::AccessibleRole::CheckBox);
        REQUIRE(checkBoxObject->getState().toggleState == tgui::priv::AccessibleToggleState::Off);
        checkBox->setChecked(true);
        REQUIRE(checkBoxObject->getState().toggleState == tgui::priv::AccessibleToggleState::On);

        // The properties that were set on the widget take priority over what the widget type provides
        editBox->setAccessibleName("Player name");
        editBox->setAccessibleDescription("Shown on the scoreboard");
        REQUIRE(editBoxObject->getName() == "Player name");
        REQUIRE(editBoxObject->getDescription() == "Shown on the scoreboard");

        button->setAccessibleRole(tgui::AccessibleRole::ToggleButton);
        REQUIRE(buttonObject->getRole() == tgui::AccessibleRole::ToggleButton);
    }

    SECTION("Tree structure")
    {
        const auto root = tree.getRoot();
        REQUIRE(root);
        REQUIRE(root->getRole() == tgui::AccessibleRole::Window);

        auto children = root->getChildren();
        REQUIRE(children.size() == 3);
        REQUIRE(children[0] == tree.findWidgetObject(button));
        REQUIRE(children[1] == tree.findWidgetObject(editBox));
        REQUIRE(children[2] == tree.findWidgetObject(checkBox));
        REQUIRE(children[1]->getParent() == root);

        // The same widget is always represented by the same object
        REQUIRE(tree.findWidgetObject(button) == tree.findWidgetObject(button));

        // Hidden widgets aren't part of the tree
        editBox->setVisible(false);
        children = root->getChildren();
        REQUIRE(children.size() == 2);
        REQUIRE(std::find(children.begin(), children.end(), tree.findWidgetObject(editBox)) == children.end());

        // Widgets inside a container are children of the object of that container
        auto panel = tgui::Panel::create();
        auto innerButton = tgui::Button::create("Inner");
        panel->add(innerButton);
        gui.add(panel);
        const auto panelObject = tree.findWidgetObject(panel);
        REQUIRE(panelObject->getChildren().size() == 1);
        REQUIRE(panelObject->getChildren()[0] == tree.findWidgetObject(innerButton));
    }

    SECTION("Unique ids")
    {
        // Ids are never reused, not even by a different tree for the same gui
        const unsigned int buttonId = tree.findWidgetObject(button)->getUniqueId();
        RecordingListener otherListener;
        tgui::priv::AccessibilityTree otherTree{gui, otherListener};
        REQUIRE(otherTree.findWidgetObject(button)->getUniqueId() != buttonId);
        REQUIRE(tree.findWidgetObject(editBox)->getUniqueId() != buttonId);
    }

    SECTION("Events are queued until they are flushed")
    {
        const auto buttonObject = tree.findWidgetObject(button);
        tree.flushEvents(true);
        listener.clear();

        button->setText("Cancel");
        REQUIRE(tree.hasQueuedEvents());
        REQUIRE(listener.events.empty());

        tree.flushEvents(true);
        REQUIRE(!tree.hasQueuedEvents());
        REQUIRE(listener.countProperty(AccessibleProperty::Name, buttonObject) == 1);

        // Objects are reported as changed before the events about them, so that platforms can update their caches
        REQUIRE(std::find(listener.changedObjects.begin(), listener.changedObjects.end(), buttonObject)
                != listener.changedObjects.end());

        // Events can be discarded
        listener.clear();
        button->setText("Close");
        tree.flushEvents(false);
        REQUIRE(listener.events.empty());
        REQUIRE(!tree.hasQueuedEvents());
    }

    SECTION("Repeated changes are reported once")
    {
        const auto checkBoxObject = tree.findWidgetObject(checkBox);
        tree.flushEvents(true);
        listener.clear();

        checkBox->setChecked(true);
        checkBox->setChecked(false);
        checkBox->setChecked(true);
        tree.flushEvents(true);
        REQUIRE(listener.countProperty(AccessibleProperty::ToggleState, checkBoxObject) == 1);
    }

    SECTION("Focus")
    {
        tree.flushEvents(true);
        listener.clear();

        editBox->setFocused(true);
        tree.flushEvents(true);
        const auto editBoxObject = tree.findWidgetObject(editBox);
        REQUIRE(listener.count(AccessibleEvent::FocusChanged, editBoxObject) == 1);
        REQUIRE(tree.getReportedFocusedObject() == editBoxObject);

        // The focus isn't reported again when it didn't change, unless the platform asks for it
        listener.clear();
        tree.flushEvents(true);
        REQUIRE(listener.count(AccessibleEvent::FocusChanged, editBoxObject) == 0);
        tree.invalidateFocus();
        tree.flushEvents(true);
        REQUIRE(listener.count(AccessibleEvent::FocusChanged, editBoxObject) == 1);

        // Labels can't be focused
        auto label = tgui::Label::create("Name:");
        gui.add(label);
        REQUIRE(!tree.findWidgetObject(label)->isFocusable());
        REQUIRE(tree.findWidgetObject(button)->isFocusable());
        REQUIRE(tree.findWidgetObject(button)->setFocus());
        REQUIRE(button->isFocused());
    }

    SECTION("Actions")
    {
        unsigned int pressCount = 0;
        button->onPress([&pressCount] { ++pressCount; });
        REQUIRE(tree.findWidgetObject(button)->performAction(tgui::priv::AccessibleAction::Invoke));
        REQUIRE(pressCount == 1);

        REQUIRE(tree.findWidgetObject(checkBox)->performAction(tgui::priv::AccessibleAction::Toggle));
        REQUIRE(checkBox->isChecked());

        REQUIRE(tree.findWidgetObject(editBox)->setValue("World"));
        REQUIRE(editBox->getText() == "World");

        // Disabled widgets can't be used
        button->setEnabled(false);
        REQUIRE(!tree.findWidgetObject(button)->isEnabled());
        REQUIRE(!tree.findWidgetObject(button)->performAction(tgui::priv::AccessibleAction::Invoke));
        REQUIRE(pressCount == 1);
    }

    SECTION("Labels")
    {
        // A widget without a name of its own is named by the label directly in front of it
        auto label = tgui::Label::create("Volume");
        auto slider = tgui::Slider::create();
        gui.add(label);
        gui.add(slider);
        const auto sliderObject = tree.findWidgetObject(slider);
        REQUIRE(sliderObject->getName() == "Volume");
        REQUIRE(sliderObject->getLabel() == tree.findWidgetObject(label));

        // Hidden widgets in between are skipped, but a different widget in between means that the label belongs to that one
        auto hiddenButton = tgui::Button::create("Hidden");
        hiddenButton->setVisible(false);
        gui.add(hiddenButton);
        gui.setWidgetIndex(hiddenButton, gui.getWidgets().size() - 2);
        REQUIRE(sliderObject->getName() == "Volume");
        auto otherSlider = tgui::Slider::create();
        gui.add(otherSlider);
        gui.setWidgetIndex(otherSlider, gui.getWidgets().size() - 2);
        REQUIRE(sliderObject->getLabel() == nullptr);
        REQUIRE(tree.findWidgetObject(otherSlider)->getName() == "Volume");

        // Widgets with a name of their own aren't named by a label
        auto checkBoxLabel = tgui::Label::create("Options");
        auto namedCheckBox = tgui::CheckBox::create("Music");
        gui.add(checkBoxLabel);
        gui.add(namedCheckBox);
        REQUIRE(tree.findWidgetObject(namedCheckBox)->getName() == "Music");
        REQUIRE(tree.findWidgetObject(namedCheckBox)->getLabel() == nullptr);

        // The accessible name has priority over the label
        slider->setAccessibleName("Sound volume");
        REQUIRE(sliderObject->getName() == "Sound volume");
        REQUIRE(sliderObject->getLabel() == nullptr);
    }

    SECTION("Radio button groups")
    {
        auto small = tgui::RadioButton::create();
        small->setText("Small");
        auto large = tgui::RadioButton::create();
        large->setText("Large");
        gui.add(small);
        gui.add(large);

        const auto largeState = tree.findWidgetObject(large)->getState();
        REQUIRE(largeState.positionInSet == 2);
        REQUIRE(largeState.sizeOfSet == 2);
    }

    SECTION("Removed widgets")
    {
        const auto buttonObject = tree.findWidgetObject(button);
        tree.flushEvents(true);
        listener.clear();

        gui.remove(button);
        tree.flushEvents(true);
        REQUIRE(listener.count(AccessibleEvent::ObjectRemoved, buttonObject) == 1);
        REQUIRE(!buttonObject->isAlive());
        REQUIRE(tree.getRoot()->getChildren().size() == 2);
    }
}
