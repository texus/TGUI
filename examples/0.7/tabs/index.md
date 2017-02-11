---
layout: page
title: Tabs
breadcrumb: tabs
redirect_from: /example-code/v0.7/tabs/
---
In the example below, two tabs are created with a corresponding panel for each. Only one of the panels is shown at a time depending on which tab is selected.

Note that this example uses the [optional unbound parameter](https://tgui.eu/tutorials/v0.7/signals-optional-parameters/) feature of the connect function, it will not compile on VS2013.

{% highlight c++ %}
#include <TGUI/TGUI.hpp>

void onTabSelected(tgui::Gui& gui, std::string selectedTab)
{
    // Show the correct panel
    if (selectedTab == "First")
    {
        gui.get("FirstPanel")->show();
        gui.get("SecondPanel")->hide();
    }
    else if (selectedTab == "Second")
    {
        gui.get("FirstPanel")->hide();
        gui.get("SecondPanel")->show();
    }
}

int main()
{
    sf::RenderWindow window(sf::VideoMode(800, 600), "TGUI Tabs Example");
    tgui::Gui gui{window};

    tgui::Tab::Ptr tabs = tgui::Tab::create();
    tabs->add("First");
    tabs->add("Second");
    tabs->setPosition(20, 20);
    gui.add(tabs);

    // Create the first panel
    tgui::Panel::Ptr panel1 = tgui::Panel::create();
    panel1->setSize(400, 300);
    panel1->setPosition(tabs->getPosition().x, tabs->getPosition().y + tabs->getTabHeight());
    gui.add(panel1, "FirstPanel");

    // Create the second panel (by copying of first one)
    tgui::Panel::Ptr panel2 = tgui::Panel::copy(panel1);
    gui.add(panel2, "SecondPanel");

    // Add some widgets to the panels
    panel1->add(tgui::Picture::create("xubuntu_bg_aluminium.jpg"));
    panel2->add(tgui::Picture::create("Linux.jpg"));

    // Enable callback when another tab is selected (pass reference to the gui as first parameter)
    tabs->connect("TabSelected", onTabSelected, std::ref(gui));

    // Select the first tab and only show the first panel
    tabs->select("First");
    panel1->show();
    panel2->hide();

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
}
{% endhighlight %}
