
#include <TGUI/TGUI.hpp>

int main()
{
    sf::RenderWindow window(sf::VideoMode(800, 600), "TGUI window");
    window.setFramerateLimit(60);

    tgui::Gui gui(window);

    try
    {
        tgui::Theme::Ptr theme = tgui::Theme::create("../../widgets/Black.txt");

        gui.add(tgui::Picture::create("../RedBackground.jpg"));

        tgui::Tab::Ptr tab = theme->load("tab");
        tab->setTabHeight(30);
        tab->setPosition(70, 40);
        tab->add("Tab - 1");
        tab->add("Tab - 2");
        tab->add("Tab - 3");
        gui.add(tab);

        tgui::MenuBar::Ptr menu = theme->load("MenuBar");
        menu->setSize((float)window.getSize().x, 22.f);
        menu->addMenu("MenuOption-1");
        menu->addMenuItem("MenuOption-1", "Load");
        menu->addMenuItem("MenuOption-1", "Save");
        menu->addMenuItem("MenuOption-1", "Exit");
        menu->addMenu("MenuOption-2");
        menu->addMenuItem("MenuOption-2", "Copy");
        menu->addMenuItem("MenuOption-2", "Paste");
        menu->addMenu("MenuOption-3");
        menu->addMenuItem("MenuOption-3", "About");
        gui.add(menu);

        tgui::Label::Ptr label = theme->load("label");
        label->setText("This is a label.\nAnd these are radio buttons:");
        label->setPosition(10, 90);
        label->setTextSize(18);
        gui.add(label);

        tgui::RadioButton::Ptr radioButton = theme->load("RadioButton");
        radioButton->setPosition(20, 140);
        radioButton->setText("Yep!");
        radioButton->setSize(25, 25);
        gui.add(radioButton);

        radioButton = theme->load("RadioButton");
        radioButton->setPosition(20, 170);
        radioButton->setText("Nope!");
        radioButton->setSize(25, 25);
        gui.add(radioButton);

        radioButton = theme->load("RadioButton");
        radioButton->setPosition(20, 200);
        radioButton->setText("Don't know!");
        radioButton->setSize(25, 25);
        gui.add(radioButton);

        label = theme->load("label");
        label->setText("We've got some edit boxes:");
        label->setPosition(10, 240);
        label->setTextSize(18);
        gui.add(label);

        tgui::EditBox::Ptr editBox = theme->load("EditBox");
        editBox->setSize(200, 25);
        editBox->setTextSize(18);
        editBox->setPosition(10, 270);
        editBox->setDefaultText("Click to edit text...");
        gui.add(editBox);

        label = theme->load("label");
        label->setText("And some list boxes too...");
        label->setPosition(10, 310);
        label->setTextSize(18);
        gui.add(label);

        tgui::ListBox::Ptr listBox = theme->load("ListBox");
        listBox->setSize(250, 120);
        listBox->setItemHeight(20);
        listBox->setPosition(10, 340);
        listBox->addItem("Item 1");
        listBox->addItem("Item 2");
        listBox->addItem("Item 3");
        gui.add(listBox);

        label = theme->load("label");
        label->setText("It's the progress bar below");
        label->setPosition(10, 470);
        label->setTextSize(18);
        gui.add(label);

        tgui::ProgressBar::Ptr progressBar = theme->load("ProgressBar");
        progressBar->setPosition(10, 500);
        progressBar->setSize(200, 20);
        progressBar->setValue(50);
        gui.add(progressBar);

        label = theme->load("label");
        label->setText(std::to_string(progressBar->getValue()) + "%");
        label->setPosition(220, 500);
        label->setTextSize(18);
        gui.add(label);

        label = theme->load("label");
        label->setText("That's the slider");
        label->setPosition(10, 530);
        label->setTextSize(18);
        gui.add(label);

        tgui::Slider::Ptr slider = theme->load("slider");
        slider->setPosition(10, 560);
        slider->setSize(200, 18);
        slider->setValue(4);
        gui.add(slider);

        tgui::Scrollbar::Ptr scrollbar = theme->load("scrollbar");
        scrollbar->setPosition(380, 40);
        scrollbar->setSize(18, 540);
        scrollbar->setMaximum(100);
        scrollbar->setLowValue(70);
        gui.add(scrollbar);

        tgui::ComboBox::Ptr comboBox = theme->load("ComboBox");
        comboBox->setSize(120, 21);
        comboBox->setPosition(420, 40);
        comboBox->addItem("Item 1");
        comboBox->addItem("Item 2");
        comboBox->addItem("Item 3");
        comboBox->setSelectedItem("Item 2");
        gui.add(comboBox);

        tgui::ChildWindow::Ptr child = theme->load("ChildWindow");
        child->setSize(250, 120);
        child->setPosition(420, 80);
        child->setTitle("Child window");
        gui.add(child);

        label = theme->load("label");
        label->setText("Hi! I'm a child window.");
        label->setPosition(30, 30);
        label->setTextSize(15);
        child->add(label);

        tgui::Button::Ptr button = theme->load("button");
        button->setPosition(75, 70);
        button->setText("OK");
        button->setSize(100, 30);
        button->connect("pressed", [=](){ child->hide(); });
        child->add(button);

        tgui::CheckBox::Ptr checkbox = theme->load("checkbox");
        checkbox->setPosition(420, 240);
        checkbox->setText("Ok, I got it");
        checkbox->setSize(25, 25);
        gui.add(checkbox);

        checkbox = theme->load("checkbox");
        checkbox->setPosition(570, 240);
        checkbox->setText("No, I didn't");
        checkbox->setSize(25, 25);
        gui.add(checkbox);

        label = theme->load("label");
        label->setText("Chatbox");
        label->setPosition(420, 280);
        label->setTextSize(18);
        gui.add(label);

        tgui::ChatBox::Ptr chatbox = theme->load("ChatBox");
        chatbox->setSize(300, 100);
        chatbox->setTextSize(18);
        chatbox->setPosition(420, 310);
        chatbox->setLinesStartFromTop();
        chatbox->addLine("texus : Hey, this is TGUI!", sf::Color::Green);
        chatbox->addLine("Me : Looks awesome! ;)", sf::Color::Yellow);
        chatbox->addLine("texus : Thanks! :)", sf::Color::Green);
        chatbox->addLine("Me : The widgets rock ^^", sf::Color::Yellow);
        gui.add(chatbox);

        sf::Texture texture;
        sf::Sprite  sprite;
        texture.loadFromFile("../ThinkLinux.jpg");
        sprite.setTexture(texture);
        sprite.setScale(200.f / texture.getSize().x, 140.f / texture.getSize().y);

        sf::Text text{"SFML Canvas", *gui.getFont(), 24};
        text.setPosition(25, 100);
        text.setColor({200, 200, 200});

        tgui::Canvas::Ptr canvas = tgui::Canvas::create({200.f, 140.f});
        canvas->setPosition(420, 430);
        canvas->clear();
        canvas->draw(sprite);
        canvas->draw(text);
        canvas->display();
        gui.add(canvas);

        button = theme->load("button");
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

