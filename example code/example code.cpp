
#include <TGUI/TGUI.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////

void init(tgui::Window& window)
{
    // Create the picture.
    // As you can see you will receive a pointer to the picture.
    // This pointer may go out of scope, the real object is stored inside the window.
    // The load function for a picture points to a file, unlike the load function from other objects.
    tgui::Picture* picture = window.addPicture();
    picture->load("ThinkLinux.jpg");

    // Create the button.
    // This time the load function contains a pathname. The folder were it points to must
    // constain an info.txt file and the necessary images. The info.txt file is very complex
    // so it won't be discussed here (See the "read me" file inside the objects folder).
    tgui::Button* button = window.addButton();
    button->load("TGUI/objects/Button/Black");
    button->setText("Button 1");
    button->setScale(0.6f, 0.6f);
    button->setPosition(20, 4);

    // Create the checkbox.
    // This time we pass a parameter to the add function.
    // Outside the init function, our pointer will be gone, but of course you might
    // still need to access the object. You will need the string to do that.
    tgui::Checkbox* checkbox = window.addCheckbox("CHECK");
    checkbox->load("TGUI/objects/Checkbox/Black");
    checkbox->setText("Checkbox");
    checkbox->setPosition(20, 50);

    // Create the first radio button.
    // Only one radio button can be checked at a time, unless they have different parents.
    // In this example all radio buttons are added to the same parent, the window.
    // You can create multiple panels to make multiple radio button groups.
    tgui::RadioButton* radioButton1 = window.addRadioButton();
    radioButton1->load("TGUI/objects/RadioButton/Black");
    radioButton1->setText("Radio button 1");
    radioButton1->setPosition(20, 100);

    // Create the second radio button.
    tgui::RadioButton* radioButton2 = window.addRadioButton();
    radioButton2->load("TGUI/objects/RadioButton/Black");
    radioButton2->setText("Radio button 2");
    radioButton2->setPosition(20, 150);

    // Create an edit box.
    // The borders can be changed with the setBorders function.
    // You can call setTextSize to change the size of the text. If you do not do this and leave it 0,
    // like in this example, the text will be auto scaled and fit perfectly within those borders.
    tgui::EditBox* editBox = window.addEditBox();
    editBox->load("TGUI/objects/EditBox/Black");
    editBox->setBorders(6, 4, 6, 4);
    editBox->setText("This text no longer fits inside the EditBox.");
    editBox->setPosition(20, 300);
    editBox->setSize(360, 40); // Does the same as SetScale(3, 1), but can be called after setText.

    // Create the slider.
    // We want the slider to lie horizontal, so we set vertical scroll to false.
    tgui::Slider* slider = window.addSlider();
    slider->load("TGUI/objects/Slider/Black");
    slider->setVerticalScroll(false);
    slider->setPosition(30, 360);

    // Create the loading bar.
    // The maximum is changed, so the loading bar goes from 0 to 500.
    // Note that we change callbackID here. This ID is used to receive callbacks.
    // The ID is 0 by default which means that no callback is sent.
    tgui::LoadingBar* loadingBar = window.addLoadingBar("LOAD");
    loadingBar->load("TGUI/objects/LoadingBar/Black");
    loadingBar->setPosition(20, 400);
    loadingBar->setMaximum(500);
    loadingBar->callbackID = 1;

    // Create a scrollbar.
    // You can call the setLowValue function to change when the scrollbar should be visible.
    // The number is 1 by default and the scrollbar is only visible when the maximum is higher.
    // Here we set the maximum to 5.
    tgui::Scrollbar* scroll = window.addScrollbar();
    scroll->load("TGUI/objects/Scrollbar/Black");
    scroll->setLowValue(4);
    scroll->setMaximum(5);
    scroll->setVerticalScroll(false);
    scroll->setPosition(30, 480);


    // All the objects until now were loaded at the same way.
    // Although I'd rather don't have them, there are some exceptions.
    // The objects that are going to be created now have different parameters for the load function.
    // The explanation and the names of the parameters to pass should be clear enough though.

    // Create the listbox.
    // You must pass the width and height to the load function. The next parameter is the path to
    // the scrollbar. This path works like that from all the other objects.
    // The last parameter is the height of one item (if not provided this is a tenth of the height).
    // Again you can set the borders with the setBorders function.
    tgui::Listbox* listbox = window.addListbox();
    listbox->load(260, 200, "TGUI/objects/Scrollbar/Black", 30);
    listbox->setBorders(4, 4, 4, 4);
    listbox->setPosition(300, 20);
    listbox->addItem("Item 1");
    listbox->addItem("Item 2");
    listbox->addItem("Item 3");
    listbox->addItem("Item 4 is too long to fit inside the listbox and will be cropped.");
    listbox->addItem("Item 5");
    listbox->addItem("Item 6");
    listbox->addItem("Item 7");
    listbox->addItem("Item 8");
    listbox->addItem("Item 9");
    listbox->addItem("Item 10");

    // Create the combo box.
    // The first parameter of the load function is normal. It is the path were the arrow image can
    // be found. The second parameter changes the width of the combo box.
    // The next parameter would choose the amount of items in the list. If there is no scrollbar then
    // this will also be the maximum items to select from (this parameter is 10 by default).
    // If there is a scrollbar then it will become visible when you have more items than this number.
    // The last parameter would be the path to the scrollbar.
    tgui::ComboBox* comboBox = window.addComboBox();
    comboBox->load("TGUI/objects/ComboBox/Black", 200);
    comboBox->setBorders(4, 4, 4, 4);
    comboBox->setPosition(500, 330);
    comboBox->addItem("Item 1 is too long");
    comboBox->addItem("Item 2");
    comboBox->addItem("Item 3");
    comboBox->setSelectedItem(2); // "Item 2" will now be selected

    // In this case I am not happy with the default colors.
    // The listbox and combo box don't fit next to our other objects.
    listbox->changeColors(sf::Color( 50,  50,  50),  // Background color
                          sf::Color(200, 200, 200),  // Text color
                          sf::Color( 10, 110, 255),  // Background color of the selected item.
                          sf::Color(255, 255, 255)); // Text color of the selected item
                          // The next pareter would change the border color (black by default)

    // We can do exactly the same with our combo box (same function, same parameters).
    comboBox->changeColors(sf::Color( 50,  50,  50), sf::Color(200, 200, 200),
                           sf::Color( 10, 110, 255), sf::Color(255, 255, 255));
}

/////////////////////////////////////////////////////////////////////////////////////////////////////

int main()
{
    // The window is created like you would create a sf::Window
    tgui::Window window(sf::VideoMode(800, 600), "TGUI v0.3.6");

    // In the init function the objects will be created
    init(window);

    // Instead of calling init, you could have loaded the objects from a file.
    // The init call can be replaced by the following line:
    // window.loadObjectsFromFile("form.txt");

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
        while (window.getCallback(callback))
        {
            // Here you can handle the callback.
            // In this example we will check if the value of the loading bar has changed.
            // First we check the ID. This ID 1 was given to the object inside the init function.
            if (callback.callbackID == 1)
            {
                // Now check if the loading bar is full. If it is then close the window.
                // You only had a pointer to the loading bar inside the init function.
                // To get it back you must use getLoadingBar.
                // The "LOAD" name was chosen inside the init function.
                if (callback.value == window.getLoadingBar("LOAD")->getMaximum())
                    window.close();
            }
        }

        // You only had a pointer to the object inside the init function.
        // To get it back you must use one of the get functions.
        // Here we first check if our checkbox is checked.
        // If it is checked then the value of the loading bar is incremented.
        // The "CHECK" and "LOAD" names were chosen inside the init function.
        if (window.getCheckbox("CHECK")->isChecked())
            window.getLoadingBar("LOAD")->incrementValue();

        window.clear();

        // Draw all the objects on the window.
        window.drawGUI();

        window.display();

        sf::sleep(sf::milliseconds(10));
    }

    return EXIT_SUCCESS;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////

