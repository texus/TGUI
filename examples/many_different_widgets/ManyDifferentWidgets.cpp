/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus' Graphical User Interface
// Copyright (C) 2012-2018 Bruno Van de Velde (vdv_b@tgui.eu)
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


#define SFML_NO_DEPRECATED_WARNINGS
#include <TGUI/TGUI.hpp>
#include <iostream>

int main()
{
    sf::RenderWindow window(sf::VideoMode(800, 600), "TGUI window");
    window.setFramerateLimit(60);

    tgui::Gui gui(window);

    try
    {
        tgui::Theme theme{"../../themes/Black.txt"};

        gui.add(tgui::Picture::create("../RedBackground.jpg"));

        auto tabs = tgui::Tabs::create();
        tabs->setRenderer(theme.getRenderer("Tabs"));
        tabs->setTabHeight(30);
        tabs->setPosition(70, 40);
        tabs->add("Tab - 1");
        tabs->add("Tab - 2");
        tabs->add("Tab - 3");
        gui.add(tabs);

        auto menu = tgui::MenuBar::create();
        menu->setRenderer(theme.getRenderer("MenuBar"));
        menu->setSize(static_cast<float>(window.getSize().x), 22.f);
        menu->addMenu("File");
        menu->addMenuItem("Load");
        menu->addMenuItem("Save");
        menu->addMenuItem("Exit");
        menu->addMenu("Edit");
        menu->addMenuItem("Copy");
        menu->addMenuItem("Paste");
        menu->addMenu("Help");
        menu->addMenuItem("About");
        gui.add(menu);

        auto label = tgui::Label::create();
        label->setRenderer(theme.getRenderer("Label"));
        label->setText("This is a label.\nAnd these are radio buttons:");
        label->setPosition(10, 90);
        label->setTextSize(18);
        gui.add(label);

        auto radioButton = tgui::RadioButton::create();
        radioButton->setRenderer(theme.getRenderer("RadioButton"));
        radioButton->setPosition(20, 140);
        radioButton->setText("Yep!");
        radioButton->setSize(25, 25);
        gui.add(radioButton);

        radioButton = tgui::RadioButton::create();
        radioButton->setRenderer(theme.getRenderer("RadioButton"));
        radioButton->setPosition(20, 170);
        radioButton->setText("Nope!");
        radioButton->setSize(25, 25);
        gui.add(radioButton);

        radioButton = tgui::RadioButton::create();
        radioButton->setRenderer(theme.getRenderer("RadioButton"));
        radioButton->setPosition(20, 200);
        radioButton->setText("Don't know!");
        radioButton->setSize(25, 25);
        gui.add(radioButton);

        label = tgui::Label::create();
        label->setRenderer(theme.getRenderer("Label"));
        label->setText("We've got some edit boxes:");
        label->setPosition(10, 240);
        label->setTextSize(18);
        gui.add(label);

        auto editBox = tgui::EditBox::create();
        editBox->setRenderer(theme.getRenderer("EditBox"));
        editBox->setSize(200, 25);
        editBox->setTextSize(18);
        editBox->setPosition(10, 270);
        editBox->setDefaultText("Click to edit text...");
        gui.add(editBox);

        label = tgui::Label::create();
        label->setRenderer(theme.getRenderer("Label"));
        label->setText("And some list boxes too...");
        label->setPosition(10, 310);
        label->setTextSize(18);
        gui.add(label);

        auto listBox = tgui::ListBox::create();
        listBox->setRenderer(theme.getRenderer("ListBox"));
        listBox->setSize(250, 120);
        listBox->setItemHeight(24);
        listBox->setPosition(10, 340);
        listBox->addItem("Item 1");
        listBox->addItem("Item 2");
        listBox->addItem("Item 3");
        gui.add(listBox);

        label = tgui::Label::create();
        label->setRenderer(theme.getRenderer("Label"));
        label->setText("It's the progress bar below");
        label->setPosition(10, 470);
        label->setTextSize(18);
        gui.add(label);

        auto progressBar = tgui::ProgressBar::create();
        progressBar->setRenderer(theme.getRenderer("ProgressBar"));
        progressBar->setPosition(10, 500);
        progressBar->setSize(200, 20);
        progressBar->setValue(50);
        gui.add(progressBar);

        label = tgui::Label::create();
        label->setRenderer(theme.getRenderer("Label"));
        label->setText(std::to_string(progressBar->getValue()) + "%");
        label->setPosition(220, 500);
        label->setTextSize(18);
        gui.add(label);

        label = tgui::Label::create();
        label->setRenderer(theme.getRenderer("Label"));
        label->setText("That's the slider");
        label->setPosition(10, 530);
        label->setTextSize(18);
        gui.add(label);

        auto slider = tgui::Slider::create();
        slider->setRenderer(theme.getRenderer("Slider"));
        slider->setPosition(10, 560);
        slider->setSize(200, 18);
        slider->setValue(4);
        gui.add(slider);

        auto scrollbar = tgui::Scrollbar::create();
        scrollbar->setRenderer(theme.getRenderer("Scrollbar"));
        scrollbar->setPosition(380, 40);
        scrollbar->setSize(18, 540);
        scrollbar->setMaximum(100);
        scrollbar->setViewportSize(70);
        gui.add(scrollbar);

        auto comboBox = tgui::ComboBox::create();
        comboBox->setRenderer(theme.getRenderer("ComboBox"));
        comboBox->setSize(120, 21);
        comboBox->setPosition(420, 40);
        comboBox->addItem("Item 1");
        comboBox->addItem("Item 2");
        comboBox->addItem("Item 3");
        comboBox->setSelectedItem("Item 2");
        gui.add(comboBox);

        auto child = tgui::ChildWindow::create();
        child->setRenderer(theme.getRenderer("ChildWindow"));
        child->setSize(250, 120);
        child->setPosition(420, 80);
        child->setTitle("Child window");
        gui.add(child);

        label = tgui::Label::create();
        label->setRenderer(theme.getRenderer("Label"));
        label->setText("Hi! I'm a child window.");
        label->setPosition(30, 30);
        label->setTextSize(15);
        child->add(label);

        auto button = tgui::Button::create();
        button->setRenderer(theme.getRenderer("Button"));
        button->setPosition(75, 70);
        button->setText("OK");
        button->setSize(100, 30);
        button->connect("pressed", [=](){ child->setVisible(false); });
        child->add(button);

        auto checkbox = tgui::CheckBox::create();
        checkbox->setRenderer(theme.getRenderer("CheckBox"));
        checkbox->setPosition(420, 240);
        checkbox->setText("Ok, I got it");
        checkbox->setSize(25, 25);
        gui.add(checkbox);

        checkbox = tgui::CheckBox::create();
        checkbox->setRenderer(theme.getRenderer("CheckBox"));
        checkbox->setPosition(570, 240);
        checkbox->setText("No, I didn't");
        checkbox->setSize(25, 25);
        gui.add(checkbox);

        label = tgui::Label::create();
        label->setRenderer(theme.getRenderer("Label"));
        label->setText("Chatbox");
        label->setPosition(420, 280);
        label->setTextSize(18);
        gui.add(label);

        auto chatbox = tgui::ChatBox::create();
        chatbox->setRenderer(theme.getRenderer("ChatBox"));
        chatbox->setSize(300, 100);
        chatbox->setTextSize(18);
        chatbox->setPosition(420, 310);
        chatbox->setLinesStartFromTop();
        chatbox->addLine("texus: Hey, this is TGUI!", sf::Color::Green);
        chatbox->addLine("Me: Looks awesome! ;)", sf::Color::Yellow);
        chatbox->addLine("texus: Thanks! :)", sf::Color::Green);
        chatbox->addLine("Me: The widgets rock ^^", sf::Color::Yellow);
        gui.add(chatbox);

        sf::Texture texture;
        sf::Sprite  sprite;
        texture.loadFromFile("../ThinkLinux.jpg");
        sprite.setTexture(texture);
        sprite.setScale(200.f / texture.getSize().x, 140.f / texture.getSize().y);

        sf::Text text{"SFML Canvas", *gui.getFont(), 24};
        text.setPosition(25, 100);
        text.setColor({200, 200, 200});

        auto canvas = tgui::Canvas::create({200, 140});
        canvas->setPosition(420, 430);
        canvas->clear();
        canvas->draw(sprite);
        canvas->draw(text);
        canvas->display();
        gui.add(canvas);

        button = tgui::Button::create();
        button->setRenderer(theme.getRenderer("Button"));
        button->setPosition(window.getSize().x - 115.f, window.getSize().y - 50.f);
        button->setText("Exit");
        button->setSize(100, 40);
        button->connect("pressed", [&](){ window.close(); });
        gui.add(button);
    }
    catch (const tgui::Exception& e)
    {
        std::cerr << "TGUI Exception: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();

            gui.handleEvent(event);
        }

        window.clear();
        gui.draw();
        window.display();
    }

    return EXIT_SUCCESS;
}

