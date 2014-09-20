
#include <TGUI/TGUI.hpp>

#define THEME_CONFIG_FILE "../../widgets/Black.conf"

int main()
{
    sf::RenderWindow window(sf::VideoMode(800, 600), "TGUI window");
    tgui::Gui gui(window);

    gui.setGlobalFont("../../fonts/DejaVuSans.ttf");

    gui.add(tgui::Picture::create("../RedBackground.jpg"));

    auto tab = tgui::Tab::create(THEME_CONFIG_FILE);
    tab->setTabHeight(30);
    tab->setPosition(70, 40);
    tab->add("Tab - 1");
    tab->add("Tab - 2");
    tab->add("Tab - 3");
    gui.add(tab);
    
    auto menu = tgui::MenuBar::create(THEME_CONFIG_FILE);
    menu->setSize(window.getSize().x, 22);
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
    
    auto label = tgui::Label::create(THEME_CONFIG_FILE);
    label->setText("This is a label.\nAnd these are radio buttons:");
    label->setPosition(10, 90);
    label->setTextSize(18);
    gui.add(label);

    auto radioButton = tgui::RadioButton::create(THEME_CONFIG_FILE);
    radioButton->setPosition(20, 140);
    radioButton->setText("Yep!");
    radioButton->setSize(25, 25);
    gui.add(radioButton);
    
    radioButton = tgui::RadioButton::create(THEME_CONFIG_FILE);
    radioButton->setPosition(20, 170);
    radioButton->setText("Nope!");
    radioButton->setSize(25, 25);
    gui.add(radioButton);
    
    radioButton = tgui::RadioButton::create(THEME_CONFIG_FILE);
    radioButton->setPosition(20, 200);
    radioButton->setText("Don't know!");
    radioButton->setSize(25, 25);
    gui.add(radioButton);
    
    label = tgui::Label::create(THEME_CONFIG_FILE);
    label->setText("We've got some edit boxes:");
    label->setPosition(10, 240);
    label->setTextSize(18);
    gui.add(label);
    
    auto editBox = tgui::EditBox::create(THEME_CONFIG_FILE);
    editBox->setSize(200, 25);
    editBox->setTextSize(18);
    editBox->setPosition(10, 270);
    editBox->setDefaultText("Click to edit text...");
    gui.add(editBox);
    
    label = tgui::Label::create(THEME_CONFIG_FILE);
    label->setText("And some list boxes too...");
    label->setPosition(10, 310);
    label->setTextSize(18);
    gui.add(label);
    
    auto listBox = tgui::ListBox::create(THEME_CONFIG_FILE);
    listBox->setSize(250, 120);
    listBox->setItemHeight(20);
    listBox->setPosition(10, 340);
    listBox->addItem("Item 1");
    listBox->addItem("Item 2");
    listBox->addItem("Item 3");
    gui.add(listBox);
    
    label = tgui::Label::create(THEME_CONFIG_FILE);
    label->setText("It's the progress bar below");
    label->setPosition(10, 470);
    label->setTextSize(18);
    gui.add(label);
    
    auto progressBar = tgui::ProgressBar::create(THEME_CONFIG_FILE);
    progressBar->setPosition(10, 500);
    progressBar->setSize(200, 20);
    progressBar->setValue(50);
    gui.add(progressBar);
    
    label = tgui::Label::create(THEME_CONFIG_FILE);
    label->setText(std::to_string(progressBar->getValue()) + "%");
    label->setPosition(220, 500);
    label->setTextSize(18);
    gui.add(label);
    
    label = tgui::Label::create(THEME_CONFIG_FILE);
    label->setText("That's the slider");
    label->setPosition(10, 530);
    label->setTextSize(18);
    gui.add(label);
    
    auto slider = tgui::Slider::create(THEME_CONFIG_FILE);
    slider->setPosition(10, 560);
    slider->setSize(200, 18);
    slider->setValue(4);
    gui.add(slider);
    
    auto scrollbar = tgui::Scrollbar::create(THEME_CONFIG_FILE);
    scrollbar->setPosition(380, 40);
    scrollbar->setSize(18, 540);
    scrollbar->setMaximum(100);
    scrollbar->setLowValue(70);
    gui.add(scrollbar);
    
    auto comboBox = tgui::ComboBox::create(THEME_CONFIG_FILE);
    comboBox->setSize(120, 21);
    comboBox->setPosition(420, 40);
    comboBox->addItem("Item 1");
    comboBox->addItem("Item 2");
    comboBox->addItem("Item 3");
    comboBox->setSelectedItem("Item 2");
    gui.add(comboBox);
    
    auto child = tgui::ChildWindow::create(THEME_CONFIG_FILE);
    child->setSize(250, 120);
    child->setPosition(420, 80);
    child->setTitle("Child window");
    gui.add(child);
    
    label = tgui::Label::create(THEME_CONFIG_FILE);
    label->setText("Hi! I'm a child window.");
    label->setPosition(30, 30);
    label->setTextSize(15);
    child->add(label);
    
    auto button = tgui::Button::create(THEME_CONFIG_FILE);
    button->setPosition(75, 70);
    button->setText("OK");
    button->setSize(100, 30);
    button->connect("pressed", [=](){ child->hide(); });
    child->add(button);
   
    auto checkbox = tgui::Checkbox::create(THEME_CONFIG_FILE);
    checkbox->setPosition(420, 240);
    checkbox->setText("Ok, I got it");
    checkbox->setSize(25, 25);
    gui.add(checkbox);
    
    checkbox = tgui::Checkbox::create(THEME_CONFIG_FILE);
    checkbox->setPosition(570, 240);
    checkbox->setText("No, I didn't");
    checkbox->setSize(25, 25);
    gui.add(checkbox);
    
    label = tgui::Label::create(THEME_CONFIG_FILE);
    label->setText("Chatbox");
    label->setPosition(420, 280);
    label->setTextSize(18);
    gui.add(label);
    
    auto chatbox = tgui::ChatBox::create(THEME_CONFIG_FILE);
    chatbox->setSize(300, 100);
    chatbox->setTextSize(18);
    chatbox->setPosition(420, 310);
    gui.add(chatbox);
    chatbox->addLine("texus : Hey, this is TGUI!", sf::Color::Green);
    chatbox->addLine("Me : Looks awesome! ;)", sf::Color::Yellow);
    chatbox->addLine("texus : Thanks! :)", sf::Color::Green);
    chatbox->addLine("Me : The widgets rock ^^", sf::Color::Yellow);
    
    sf::Texture texture;
    sf::Sprite  sprite;
    texture.loadFromFile("../ThinkLinux.jpg");
    sprite.setTexture(texture);
    sprite.setScale(200.f / texture.getSize().x, 140.f / texture.getSize().y);
    
    sf::Text text{"SFML Canvas", *gui.getGlobalFont(), 24};
    text.setPosition(25, 100);
    text.setColor({200, 200, 200});
    
    auto canvas = tgui::Canvas::create({200, 140});
    canvas->setPosition(420, 430);
    canvas->clear();
    canvas->draw(sprite);
    canvas->draw(text);
    canvas->display();
    gui.add(canvas);
   
    button = tgui::Button::create(THEME_CONFIG_FILE);
    button->setPosition(window.getSize().x - 115, window.getSize().y - 50);
    button->setText("Exit");
    button->setSize(100, 40);
    button->connect("pressed", [&](){ window.close(); });
    gui.add(button);

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

    return 0;
}

