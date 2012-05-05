/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus's Graphical User Interface
// Copyright (C) 2012 Bruno Van de Velde (VDV_B@hotmail.com)
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


#ifndef _TGUI_GROUP_INCLUDED_
#define _TGUI_GROUP_INCLUDED_

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{    
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    
    struct TGUI_API Group
    {
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Default constructor
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        Group();
        
        
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Copy constructor
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        Group(const Group& copy);
        
        
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Destructor
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual ~Group();
        
        
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Overload of assignment operator
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        Group& operator= (const Group& right);
                

        //////////////////////////////////////////////////////////////////////////////////////////////////////
        // Create and add an object to the panel.
        // You can optionally pass an object name.
        // If you want to access the object later then you must do this with the object name.
        //
        // return:  a pointer to the created object
        //////////////////////////////////////////////////////////////////////////////////////////////////////
        Label*       addLabel(const std::string objectName = "");
        Picture*     addPicture(const std::string objectName = "");
        Button*      addButton(const std::string objectName = "");
        Checkbox*    addCheckbox(const std::string objectName = "");
        RadioButton* addRadioButton(const std::string objectName = "");
        EditBox*     addEditBox(const std::string objectName = "");
        Slider*      addSlider(const std::string objectName = "");
        Scrollbar*   addScrollbar(const std::string objectName = "");
        Listbox*     addListbox(const std::string objectName = "");
        LoadingBar*  addLoadingBar(const std::string objectName = "");
        Panel*       addPanel(const std::string objectName = "");
        ComboBox*    addComboBox(const std::string objectName = "");
        TextBox*     addTextBox(const std::string objectName = "");
        
        
        //////////////////////////////////////////////////////////////////////////////////////////////////////
        // These functions are used to get back the pointer to an object.
        // The object name is the same as when you created the objects (with the addObject functions).
        // When a wrong objectName was passed then nothing will happen. The returned object is empty and the
        // changes done to it will have no effect whatsoever.
        //
        // return: a pointer to the object
        //////////////////////////////////////////////////////////////////////////////////////////////////////
        Label*       getLabel(const std::string objectName);
        Picture*     getPicture(const std::string objectName);
        Button*      getButton(const std::string objectName);
        Checkbox*    getCheckbox(const std::string objectName);
        RadioButton* getRadioButton(const std::string objectName);
        EditBox*     getEditBox(const std::string objectName);
        Slider*      getSlider(const std::string objectName);
        Scrollbar*   getScrollbar(const std::string objectName);
        Listbox*     getListbox(const std::string objectName);
        LoadingBar*  getLoadingBar(const std::string objectName);
        Panel*       getPanel(const std::string objectName);
        ComboBox*    getComboBox(const std::string objectName);
        TextBox*     getTextBox(const std::string objectName);

        
        template <typename T>
        T* copyObject(T* oldObject, const std::string newObjectName = "");
 
        Label*       copyLabel(const std::string oldObjectName, const std::string newObjectName = "");
        Picture*     copyPicture(const std::string oldObjectName, const std::string newObjectName = "");
        Button*      copyButton(const std::string oldObjectName, const std::string newObjectName = "");
        Checkbox*    copyCheckbox(const std::string oldObjectName, const std::string newObjectName = "");
        RadioButton* copyRadioButton(const std::string oldObjectName, const std::string newObjectName = "");
        EditBox*     copyEditBox(const std::string oldObjectName, const std::string newObjectName = "");
        Slider*      copySlider(const std::string oldObjectName, const std::string newObjectName = "");
        Scrollbar*   copyScrollbar(const std::string oldObjectName, const std::string newObjectName = "");
        Listbox*     copyListbox(const std::string oldObjectName, const std::string newObjectName = "");
        LoadingBar*  copyLoadingBar(const std::string oldObjectName, const std::string newObjectName = "");
        Panel*       copyPanel(const std::string oldObjectName, const std::string newObjectName = "");
        ComboBox*    copyComboBox(const std::string oldObjectName, const std::string newObjectName = "");
        TextBox*     copyTextBox(const std::string oldObjectName, const std::string newObjectName = "");

        
        
        std::vector<OBJECT*>& getObjects();
        
        
        
        
        
        
        //////////////////////////////////////////////////////////////////////////////////////////////////////
        // Removes a single object that was added to the panel.
        // If there are multiple objects with the same name then only the first matching object
        // will be removed.
        //////////////////////////////////////////////////////////////////////////////////////////////////////
        void removeObject(const std::string objectName);
        
        
        //////////////////////////////////////////////////////////////////////////////////////////////////////
        // Removes all objects that were added to the panel.
        //////////////////////////////////////////////////////////////////////////////////////////////////////
        void removeAllObjects();
        
  
        
        void focus(OBJECT* object);
        
        void unfocus(OBJECT* object);
        
        void uncheckRadioButtons();
        
        virtual void handleEvent(sf::Event& event) = 0;
        
        virtual void addCallback(Callback& callback) = 0;
        
        
        void updateTime(const sf::Time& elapsedTime);
        
        
        void drawObjectGroup(sf::RenderTarget* target, const sf::RenderStates& states) const;
        
        
        
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Places an object before all other objects.
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void moveObjectToFront(OBJECT* object);
        
        
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Places an object behind all other objects.
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void moveObjectToBack(OBJECT* object);
        
        
        
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    public:
        
        // The internal font, used by all objects by default. If not changed then this is the default SFML font.
        sf::Font globalFont;
        
        
        // There are some empty object inside the panel, because the getObject functions must always return something.
        // When the objectName was wrong then these empty objects are returned.
        static Label       m_EmptyLabel;
        static Picture     m_EmptyPicture;
        static Button      m_EmptyButton;
        static Checkbox    m_EmptyCheckbox;
        static RadioButton m_EmptyRadioButton;
        static EditBox     m_EmptyEditBox;
        static Slider      m_EmptySlider;
        static Scrollbar   m_EmptyScrollbar;
        static Listbox     m_EmptyListbox;
        static LoadingBar  m_EmptyLoadingBar;
        static Panel       m_EmptyPanel;
        static ComboBox    m_EmptyComboBox;
        static TextBox     m_EmptyTextBox;

        
        
        std::vector<OBJECT*>      m_Objects;
        std::vector<std::string>  m_ObjName;
        
        
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    protected:

        // The internal event manager
        EventManager m_EventManager;
        
        // The internal animation manager to handle animation in objects
//        AnimationManager m_AnimationManager;
        
        // Is the group focused? If so, then one of the objects inside the group may be focused
        bool m_GroupFocused;

        
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    };

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif //_TGUI_GROUP_INCLUDED_
