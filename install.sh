#####################################################################################################
#
#  TGUI - Texus's Graphical User Interface
#  Copyright (C) 2012 Bruno Van de Velde (VDV_B@hotmail.com)
# 
#  This software is provided 'as-is', without any express or implied warranty.
#  In no event will the authors be held liable for any damages arising from the use of this software.
# 
#  Permission is granted to anyone to use this software for any purpose,
#  including commercial applications, and to alter it and redistribute it freely,
#  subject to the following restrictions:
# 
#  1. The origin of this software must not be misrepresented;
#     you must not claim that you wrote the original software.
#     If you use this software in a product, an acknowledgment
#     in the product documentation would be appreciated but is not required.
# 
#  2. Altered source versions must be plainly marked as such,
#     and must not be misrepresented as being the original software.
# 
#  3. This notice may not be removed or altered from any source distribution.
# 
#####################################################################################################

if [ ! -d "lib" ]; then
    mkdir lib
fi

if [ ! -d "lib/obj" ]; then
    mkdir lib/obj
fi


if (command -v g++) then

    echo "g++ was found"

    
    echo "Compiling 'TGUI.cpp' ..."
        g++ -Wall -Wextra -Wshadow -Wno-long-long -pedantic -fPIC -DSFML_DYNAMIC -I"include" -c "src/TGUI/TGUI.cpp" -o "lib/obj/TGUI.o"

    echo "Compiling 'Objects.cpp' ..."
        g++ -Wall -Wall -Wextra -Wshadow -Wno-long-long -pedantic -fPIC -DSFML_DYNAMIC -I"include" -c "src/TGUI/Objects.cpp" -o "lib/obj/Objects.o"

    echo "Compiling 'Label.cpp' ..."
        g++ -Wall -Wall -Wextra -Wshadow -Wno-long-long -pedantic -fPIC -DSFML_DYNAMIC -I"include" -c "src/TGUI/Label.cpp" -o "lib/obj/Label.o"

    echo "Compiling 'Picture.cpp' ..."
        g++ -Wall -Wall -Wextra -Wshadow -Wno-long-long -pedantic -fPIC -DSFML_DYNAMIC -I"include" -c "src/TGUI/Picture.cpp" -o "lib/obj/Picture.o"

    echo "Compiling 'Button.cpp' ..."
        g++ -Wall -Wall -Wextra -Wshadow -Wno-long-long -pedantic -fPIC -DSFML_DYNAMIC -I"include" -c "src/TGUI/Button.cpp" -o "lib/obj/Button.o"

    echo "Compiling 'Checkbox.cpp' ..."
        g++ -Wall -Wall -Wextra -Wshadow -Wno-long-long -pedantic -fPIC -DSFML_DYNAMIC -I"include" -c "src/TGUI/Checkbox.cpp" -o "lib/obj/Checkbox.o"

    echo "Compiling 'RadioButton.cpp' ..."
        g++ -Wall -Wall -Wextra -Wshadow -Wno-long-long -pedantic -fPIC -DSFML_DYNAMIC -I"include" -c "src/TGUI/RadioButton.cpp" -o "lib/obj/RadioButton.o"

    echo "Compiling 'EditBox.cpp' ..."
        g++ -Wall -Wall -Wextra -Wshadow -Wno-long-long -pedantic -fPIC -DSFML_DYNAMIC -I"include" -c "src/TGUI/EditBox.cpp" -o "lib/obj/EditBox.o"

    echo "Compiling 'Slider.cpp' ..."
        g++ -Wall -Wall -Wextra -Wshadow -Wno-long-long -pedantic -fPIC -DSFML_DYNAMIC -I"include" -c "src/TGUI/Slider.cpp" -o "lib/obj/Slider.o"

    echo "Compiling 'Scrollbar.cpp' ..."
        g++ -Wall -Wall -Wextra -Wshadow -Wno-long-long -pedantic -fPIC -DSFML_DYNAMIC -I"include" -c "src/TGUI/Scrollbar.cpp" -o "lib/obj/Scrollbar.o"

    echo "Compiling 'Listbox.cpp' ..."
        g++ -Wall -Wall -Wextra -Wshadow -Wno-long-long -pedantic -fPIC -DSFML_DYNAMIC -I"include" -c "src/TGUI/Listbox.cpp" -o "lib/obj/Listbox.o"

    echo "Compiling 'LoadingBar.cpp' ..."
        g++ -Wall -Wall -Wextra -Wshadow -Wno-long-long -pedantic -fPIC -DSFML_DYNAMIC -I"include" -c "src/TGUI/LoadingBar.cpp" -o "lib/obj/LoadingBar.o"

    echo "Compiling 'ComboBox.cpp' ..."
        g++ -Wall -Wall -Wextra -Wshadow -Wno-long-long -pedantic -fPIC -DSFML_DYNAMIC -I"include" -c "src/TGUI/ComboBox.cpp" -o "lib/obj/ComboBox.o"

    echo "Compiling 'TextBox.cpp' ..."
        g++ -Wall -Wall -Wextra -Wshadow -Wno-long-long -pedantic -fPIC -DSFML_DYNAMIC -I"include" -c "src/TGUI/TextBox.cpp" -o "lib/obj/TextBox.o"

    echo "Compiling 'SpriteSheet.cpp' ..."
        g++ -Wall -Wall -Wextra -Wshadow -Wno-long-long -pedantic -fPIC -DSFML_DYNAMIC -I"include" -c "src/TGUI/SpriteSheet.cpp" -o "lib/obj/SpriteSheet.o"

    echo "Compiling 'AnimatedPicture.cpp' ..."
        g++ -Wall -Wall -Wextra -Wshadow -Wno-long-long -pedantic -fPIC -DSFML_DYNAMIC -I"include" -c "src/TGUI/AnimatedPicture.cpp" -o "lib/obj/AnimatedPicture.o"

    echo "Compiling 'AnimatedButton.cpp' ..."
        g++ -Wall -Wall -Wextra -Wshadow -Wno-long-long -pedantic -fPIC -DSFML_DYNAMIC -I"include" -c "src/TGUI/AnimatedButton.cpp" -o "lib/obj/AnimatedButton.o"

    echo "Compiling 'InfoFileParser.cpp' ..."
        g++ -Wall -Wall -Wextra -Wshadow -Wno-long-long -pedantic -fPIC -DSFML_DYNAMIC -I"include" -c "src/TGUI/InfoFileParser.cpp" -o "lib/obj/InfoFileParser.o"

    echo "Compiling 'EventManager.cpp' ..."
        g++ -Wall -Wall -Wextra -Wshadow -Wno-long-long -pedantic -fPIC -DSFML_DYNAMIC -I"include" -c "src/TGUI/EventManager.cpp" -o "lib/obj/EventManager.o"

    echo "Compiling 'TextureManager.cpp' ..."
        g++ -Wall -Wall -Wextra -Wshadow -Wno-long-long -pedantic -fPIC -DSFML_DYNAMIC -I"include" -c "src/TGUI/TextureManager.cpp" -o "lib/obj/TextureManager.o"

    echo "Compiling 'Group.cpp' ..."
        g++ -Wall -Wall -Wextra -Wshadow -Wno-long-long -pedantic -fPIC -DSFML_DYNAMIC -I"include" -c "src/TGUI/Group.cpp" -o "lib/obj/Group.o"

    echo "Compiling 'Panel.cpp' ..."
        g++ -Wall -Wall -Wextra -Wshadow -Wno-long-long -pedantic -fPIC -DSFML_DYNAMIC -I"include" -c "src/TGUI/Panel.cpp" -o "lib/obj/Panel.o"

    echo "Compiling 'ChildWindow.cpp' ..."
        g++ -Wall -Wall -Wextra -Wshadow -Wno-long-long -pedantic -fPIC -DSFML_DYNAMIC -I"include" -c "src/TGUI/ChildWindow.cpp" -o "lib/obj/ChildWindow.o"

    echo "Compiling 'Window.cpp' ..."
        g++ -Wall -Wall -Wextra -Wshadow -Wno-long-long -pedantic -fPIC -DSFML_DYNAMIC -I"include" -c "src/TGUI/Window.cpp" -o "lib/obj/Window.o"

    echo "Compiling 'Form.cpp' ..."
        g++ -Wall -Wall -Wextra -Wshadow -Wno-long-long -pedantic -fPIC -DSFML_DYNAMIC -I"include" -c "src/TGUI/Form.cpp" -o "lib/obj/Form.o"


    echo "Linking tgui ..."
        
        if ( ! (g++ -shared -o "lib/libtgui.so" \
                    "lib/obj/TGUI.o" \
                    "lib/obj/Objects.o" \
                    "lib/obj/Label.o" \
                    "lib/obj/Picture.o" \
                    "lib/obj/Button.o" \
                    "lib/obj/Checkbox.o" \
                    "lib/obj/RadioButton.o" \
                    "lib/obj/EditBox.o" \
                    "lib/obj/Slider.o" \
                    "lib/obj/Scrollbar.o" \
                    "lib/obj/Listbox.o" \
                    "lib/obj/LoadingBar.o" \
                    "lib/obj/ComboBox.o" \
                    "lib/obj/TextBox.o" \
                    "lib/obj/SpriteSheet.o" \
                    "lib/obj/AnimatedPicture.o" \
                    "lib/obj/AnimatedButton.o" \
                    "lib/obj/InfoFileParser.o" \
                    "lib/obj/EventManager.o" \
                    "lib/obj/TextureManager.o" \
                    "lib/obj/Group.o" \
                    "lib/obj/Panel.o" \
                    "lib/obj/ChildWindow.o" \
                    "lib/obj/Window.o" \
                    "lib/obj/Form.o")) then
            echo "Linking has failed. Make sure that sfml is installed on your system."
            echo "Aborting installer"
            exit
        fi

    echo "Root permissions are needed to install 'lib/libtgui.so' to '/usr/local/lib' and to run ldconfig."
        sudo cp "lib/libtgui.so" "/usr/local/lib"
        sudo ldconfig

    echo "Do you want to build the Form Builder? [Y/n]"
    
        while read inputline
        do
            if [ ${inputline} == "Y" ] || [ ${inputline} == "y" ]; then
                
                echo "Compiling 'FormBuilder/main.cpp' ..."
                    g++ -Wall -Wall -Wextra -Wshadow -Wno-long-long -pedantic -DSFML_DYNAMIC -I"include" -I"include/FormBuilder" -c "src/TGUI/FormBuilder/main.cpp" -o "lib/obj/main.o"

                echo "Compiling 'FormBuilder/Properties.cpp' ..."
                    g++ -Wall -Wall -Wextra -Wshadow -Wno-long-long -pedantic -DSFML_DYNAMIC -I"include" -I"include/FormBuilder" -c "src/TGUI/FormBuilder/Properties.cpp" -o "lib/obj/Properties.o"

                echo "Compiling 'FormBuilder/FormBuilder.cpp' ..."
                    g++ -Wall -Wall -Wextra -Wshadow -Wno-long-long -pedantic -DSFML_DYNAMIC -I"include" -I"include/FormBuilder" -c "src/TGUI/FormBuilder/FormBuilder.cpp" -o "lib/obj/FormBuilder.o"

                echo "Linking Form Builder ..."
                    if ( ! (g++ -o "Form Builder/FormBuilder" "lib/obj/main.o" "lib/obj/Properties.o" "lib/obj/FormBuilder.o" -ltgui -lsfml-graphics -lsfml-window -lsfml-system)) then
                        echo "Linking has failed. Did you provide root access to install tgui?"
                        echo "Aborting installer"
                        exit
                    fi

                break
            elif [ ${inputline} == "N" ] || [ ${inputline} == "n" ]; then
                break
            else
                echo ""
                echo "Do you want to build the Form Builder? [Y/n]" 
            fi
        done

    echo "Installation DONE"

else
    echo "g++ was NOT found"
    echo "Aborting installer"
fi

