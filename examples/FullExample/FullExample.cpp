
#include <TGUI/TGUI.hpp>

#define THEME_CONFIG_FILE "../../widgets/Black.conf"

int main()
{
    sf::RenderWindow window(sf::VideoMode(800, 600), "TGUI window");
    tgui::Gui gui(window);

    if (gui.setGlobalFont("../../fonts/DejaVuSans.ttf") == false)
        return 1;

    tgui::Picture::Ptr picture(gui);
    picture->load("../Linux.jpg");

    tgui::Button::Ptr button(gui);
    button->load(THEME_CONFIG_FILE);
    button->setPosition(40, 25);
    button->setText("Quit");
    button->setCallbackId(1);
    button->bindCallback(tgui::Button::LeftMouseClicked);
    button->setSize(300, 40);

    tgui::ChatBox::Ptr chatbox(gui);
    chatbox->load(THEME_CONFIG_FILE);
    chatbox->setSize(200, 100);
    chatbox->setTextSize(20);
    chatbox->setPosition(400, 25);
    chatbox->addLine("Line 1", sf::Color::Red);
    chatbox->addLine("Line 2", sf::Color::Blue);
    chatbox->addLine("Line 3", sf::Color::Green);
    chatbox->addLine("Line 4", sf::Color::Yellow);
    chatbox->addLine("Line 5", sf::Color::Cyan);
    chatbox->addLine("Line 6", sf::Color::Magenta);

    tgui::Checkbox::Ptr checkbox(gui);
    checkbox->load(THEME_CONFIG_FILE);
    checkbox->setPosition(40, 80);
    checkbox->setText("Checkbox");
    checkbox->setSize(32, 32);

    tgui::ChildWindow::Ptr child(gui);
    child->load(THEME_CONFIG_FILE);
    child->setSize(200, 100);
    child->setBackgroundColor(sf::Color(80, 80, 80));
    child->setPosition(400, 460);
    child->setTitle("Child window");
    child->setIcon("../icon.jpg");

    tgui::ComboBox::Ptr comboBox(gui);
    comboBox->load(THEME_CONFIG_FILE);
    comboBox->setSize(120, 21);
    comboBox->setPosition(210, 440);
    comboBox->addItem("Item 1");
    comboBox->addItem("Item 2");
    comboBox->addItem("Item 3");
    comboBox->setSelectedItem("Item 2");

    tgui::EditBox::Ptr editBox(gui);
    editBox->load(THEME_CONFIG_FILE);
    editBox->setPosition(40, 200);
    editBox->setSize(300, 30);

    tgui::Label::Ptr label(gui);
    label->load(THEME_CONFIG_FILE);
    label->setText("Label");
    label->setPosition(40, 160);
    label->setTextColor(sf::Color(200, 200, 200));
    label->setTextSize(24);

    tgui::ListBox::Ptr listBox(gui);
    listBox->load(THEME_CONFIG_FILE);
    listBox->setSize(150, 120);
    listBox->setItemHeight(20);
    listBox->setPosition(40, 440);
    listBox->addItem("Item 1");
    listBox->addItem("Item 2");
    listBox->addItem("Item 3");

    tgui::LoadingBar::Ptr loadingbar(gui);
    loadingbar->load(THEME_CONFIG_FILE);
    loadingbar->setPosition(40, 330);
    loadingbar->setSize(300, 30);
    loadingbar->setValue(35);

    tgui::MenuBar::Ptr menu(gui);
    menu->load(THEME_CONFIG_FILE);
    menu->setSize(window.getSize().x, 20);
    menu->addMenu("File");
    menu->addMenuItem("File", "Load");
    menu->addMenuItem("File", "Save");
    menu->addMenuItem("File", "Exit");
    menu->bindCallback(tgui::MenuBar::MenuItemClicked);
    menu->setCallbackId(2);

    sf::Texture texture;
    texture.loadFromFile("../ThinkLinux.jpg");

    tgui::Panel::Ptr panel(gui);
    panel->setSize(200, 140);
    panel->setPosition(400, 150);
    panel->setBackgroundTexture(&texture);

    tgui::RadioButton::Ptr radioButton(gui);
    radioButton->load(THEME_CONFIG_FILE);
    radioButton->setPosition(40, 120);
    radioButton->setText("Radio Button");
    radioButton->setSize(32, 32);

    tgui::Slider::Ptr slider(gui);
    slider->load(THEME_CONFIG_FILE);
    slider->setVerticalScroll(false);
    slider->setPosition(40, 250);
    slider->setSize(300, 25);
    slider->setValue(2);

    tgui::Scrollbar::Ptr scrollbar(gui);
    scrollbar->load(THEME_CONFIG_FILE);
    scrollbar->setVerticalScroll(false);
    scrollbar->setPosition(40, 290);
    scrollbar->setSize(300, 25);
    scrollbar->setMaximum(5);
    scrollbar->setLowValue(3);

    tgui::Slider2d::Ptr slider2d(gui);
    slider2d->load("../../widgets/Slider2d/Black.conf");
    slider2d->setPosition(400, 300);
    slider2d->setSize(200, 150);

    tgui::SpinButton::Ptr spinButton(gui);
    spinButton->load(THEME_CONFIG_FILE);
    spinButton->setPosition(40, 410);
    spinButton->setVerticalScroll(false);
    spinButton->setSize(40, 20);

    tgui::SpriteSheet::Ptr spritesheet(gui);
    spritesheet->load("../ThinkLinux.jpg");
    spritesheet->setCells(4, 4);
    spritesheet->setVisibleCell(2, 3);
    spritesheet->setSize(160, 120);
    spritesheet->setPosition(620, 25);

    tgui::Tab::Ptr tab(gui);
    tab->load(THEME_CONFIG_FILE);
    tab->setPosition(40, 370);
    tab->add("Item 1");
    tab->add("Item 2");
    tab->add("Item 3");

    tgui::TextBox::Ptr textBox(gui);
    textBox->load(THEME_CONFIG_FILE);
    textBox->setPosition(210, 470);
    textBox->setSize(180, 120);
    textBox->setTextSize(16);

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();

            gui.handleEvent(event);
        }

        tgui::Callback callback;
        while (gui.pollCallback(callback))
        {
            if (callback.id == 1)
                window.close();

            else if (callback.id == 2)
            {
                if (callback.text == "Exit")
                    window.close();
            }
        }

        window.clear();
        gui.draw();
        window.display();
    }

    return 0;
}

