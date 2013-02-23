
#include <TGUI/TGUI.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////

int main()
{
    // The window is created like you would create a sf::Window
    tgui::Window window(sf::VideoMode(800, 600), "TGUI demo");

    // Set the font for all the objects.
    if (window.setGlobalFont("../../Fonts/DejaVuSans.ttf") == false)
       return 1;

    // Create the picture.
    // The load function of picture takes a filename, unlike the load function from other objects.
    tgui::Picture::Ptr picture(window);
    picture->load("ThinkLinux.jpg");
    picture->setSize(800, 600);

    // Create the button.
    // This time the load function takes a pathname. The folder were it points to must
    // constain an info.txt file and the necessary images. The info.txt file is very complex
    // so it won't be discussed here.
    tgui::Button::Ptr button(window);
    button->load("../../objects/Button/Black");
    button->setText("Button 1");
    button->setSize(160, 40);
    button->setPosition(20, 4);

    // Create the checkbox
    tgui::Checkbox::Ptr checkbox(window);
    checkbox->load("../../objects/Checkbox/Black");
    checkbox->setText("Checkbox");
    checkbox->setPosition(20, 50);

    // Create the first radio button.
    // Only one radio button can be checked at a time, unless they have different parents.
    // In this example all radio buttons are added to the same parent (the window).
    // You can use panels to make multiple radio button groups.
    tgui::RadioButton::Ptr radioButton1(window);
    radioButton1->load("../../objects/RadioButton/Black");
    radioButton1->setText("Radio button 1");
    radioButton1->setPosition(20, 100);

    // Create the second radio button.
    // It is loaded with the same path as the previous radio button,
    // but the images won't be loaded again, they will simply be reused.
    tgui::RadioButton::Ptr radioButton2(window);
    radioButton2->load("../../objects/RadioButton/Black");
    radioButton2->setText("Radio button 2");
    radioButton2->setPosition(20, 150);

    // Create an edit box.
    // You can call setTextSize to change the size of the text. If you do not do this and leave it 0,
    // like in this example, the text will be auto scaled and fit perfectly within the edit box.
    tgui::EditBox::Ptr editBox(window);
    editBox->load("../../objects/EditBox/Black");
    editBox->setText("This text no longer fits inside the EditBox.");
    editBox->setPosition(20, 300);
    editBox->setSize(360, 40);

    // Create the slider.
    // We want the slider to lie horizontal, so we set vertical scroll to false.
    tgui::Slider::Ptr slider(window);
    slider->load("../../objects/Slider/Black");
    slider->setVerticalScroll(false);
    slider->setPosition(30, 360);

    // Create the loading bar.
    // The maximum is changed, so the loading bar goes from 0 to 500 (instead of 0 to 100).
    // Note that we change the callback id here. This id is used to filter received callbacks.
    // Depending on how the callback is bound, you will receive multiple callbacks in one place.
    // By setting the id you will be able to distinguish the callbacks from each other.
    // In this example, we just bind a callback to be polled later from the window.
    tgui::LoadingBar::Ptr loadingBar(window);
    loadingBar->load("../../objects/LoadingBar/Black");
    loadingBar->setPosition(20, 400);
    loadingBar->setMaximum(500);
    loadingBar->setCallbackId(1);
    loadingBar->bindCallback(tgui::LoadingBar::ValueChanged);

    // Create a scrollbar.
    // You can call the setLowValue function to change when the scrollbar should be visible.
    // The number is 1 by default and the scrollbar is only visible when the maximum is higher.
    // Here we set the maximum to 5.
    tgui::Scrollbar::Ptr scroll(window);
    scroll->load("../../objects/Scrollbar/Black");
    scroll->setLowValue(4);
    scroll->setMaximum(5);
    scroll->setVerticalScroll(false);
    scroll->setPosition(30, 480);


    // All the objects until now were loaded at the same way (pathname as parameter to load).
    // Although I'd rather don't have them, there are some exceptions.
    // The objects that are going to be created now have different parameters for the load function.
    // The explanation and the names of the parameters to pass should be clear enough though.

    // Create the list box.
    // You must pass the width and height to the load function. The next parameter is the path to
    // the scrollbar. This path works like that from all the other objects.
    // The last parameter is the height of one item (if not provided this is a tenth of the height).
    // You can set the borders with the setBorders function.
    tgui::ListBox::Ptr listBox(window);
    listBox->load(260, 200, "../../objects/Scrollbar/Black", 30);
    listBox->setBorders(4, 4, 4, 4);
    listBox->setPosition(300, 20);
    listBox->addItem("Item 1");
    listBox->addItem("Item 2");
    listBox->addItem("Item 3");
    listBox->addItem("Item 4 is too long to fit inside the list box and will be cropped.");
    listBox->addItem("Item 5");
    listBox->addItem("Item 6");
    listBox->addItem("Item 7");
    listBox->addItem("Item 8");
    listBox->addItem("Item 9");
    listBox->addItem("Item 10");

    // Create the combo box.
    // The first parameter of the load function is normal. It is the path were the arrow image can
    // be found. The second and third parameters change the width and height of the combo box.
    // The next parameter would choose the amount of items in the list. If there is no scrollbar then
    // this will also be the maximum items to select from (this parameter is 10 by default).
    // If there is a scrollbar then it will become visible when you have more items than this number.
    // The last parameter would be the path to the scrollbar.
    tgui::ComboBox::Ptr comboBox(window);
    comboBox->load("../../objects/ComboBox/Black", 200, 40);
    comboBox->setBorders(4, 4, 4, 4);
    comboBox->setPosition(500, 330);
    comboBox->addItem("Item 1 is too long");
    comboBox->addItem("Item 2");
    comboBox->addItem("Item 3");
    comboBox->setSelectedItem(2); // "Item 2" will now be selected

    // In this case I am not happy with the default colors.
    // The list box and combo box don't fit next to our other objects.
    listBox->changeColors(sf::Color( 50,  50,  50),  // Background color
                          sf::Color(200, 200, 200),  // Text color
                          sf::Color( 10, 110, 255),  // Background color of the selected item.
                          sf::Color(255, 255, 255)); // Text color of the selected item
                          // The next pareter would change the border color (black by default)

    // We can do exactly the same with our combo box (same function, same parameters).
    comboBox->changeColors(sf::Color( 50,  50,  50), sf::Color(200, 200, 200),
                           sf::Color( 10, 110, 255), sf::Color(255, 255, 255));


    // There are still a lot of objects missing in this example code.

    // Main loop
    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();

            // The events must be passed to TGUI in order to make the objects work
            window.handleEvent(event);
        }
        
        // Callback in TGUI is just like events in SFML
        tgui::Callback callback;
        while (window.pollCallback(callback))
        {
            // Here you can handle the callback.
            // In this example we will check if the value of the loading bar has changed.
            // First we check the id. This id 1 was given to the object when it was created.
            if (callback.callbackId == 1)
            {
                // Now check if the loading bar is full. If it is then close the window.
                if (callback.value == loadingBar->getMaximum())
                    window.close();
            }
        }


        // Here we first check if our checkbox is checked.
        // If it is checked then the value of the loading bar is incremented.
        if (checkbox->isChecked())
            loadingBar->incrementValue();

        window.clear();

        // Draw all the objects on the window.
        window.drawGUI();

        window.display();

        // Give the cpu some rest
        sf::sleep(sf::milliseconds(1));
    }

    return EXIT_SUCCESS;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////

