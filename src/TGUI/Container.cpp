/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus's Graphical User Interface
// Copyright (C) 2012-2015 Bruno Van de Velde (vdv_b@tgui.eu)
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


#include <stack>
#include <cmath>
#include <cassert>

#include <TGUI/TGUI.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Container::Container() :
        m_FocusedWidget(nullptr)
    {
        m_ContainerWidget = true;
        m_AnimatedWidget = true;
        m_AllowFocus = true;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Container::Container(const Container& containerToCopy) :
        Widget                   (containerToCopy),
        m_FocusedWidget          (nullptr),
        m_GlobalFont             (containerToCopy.m_GlobalFont),
        m_GlobalCallbackFunctions(containerToCopy.m_GlobalCallbackFunctions)
    {
        // Copy all the widgets
        for (unsigned int i = 0; i < containerToCopy.m_Widgets.size(); ++i)
        {
            m_Widgets.push_back(containerToCopy.m_Widgets[i].clone());
            m_ObjName.push_back(containerToCopy.m_ObjName[i]);

            m_Widgets.back()->m_Parent = this;
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Container::~Container()
    {
        removeAllWidgets();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Container& Container::operator= (const Container& right)
    {
        // Make sure it is not the same widget
        if (this != &right)
        {
            Widget::operator=(right);

            // Copy the font and the callback functions
            m_FocusedWidget = nullptr;
            m_GlobalFont = right.m_GlobalFont;
            m_GlobalCallbackFunctions = right.m_GlobalCallbackFunctions;

            // Remove all the old widgets
            removeAllWidgets();

            // Copy all the widgets
            for (unsigned int i = 0; i < right.m_Widgets.size(); ++i)
            {
                m_Widgets.push_back(right.m_Widgets[i].clone());
                m_ObjName.push_back(right.m_ObjName[i]);

                m_Widgets.back()->m_Parent = this;
            }
        }

        return *this;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Container::setGlobalFont(const std::string& filename)
    {
        return m_GlobalFont.loadFromFile(getResourcePath() + filename);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Container::setGlobalFont(const sf::Font& font)
    {
        m_GlobalFont = font;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const sf::Font& Container::getGlobalFont() const
    {
        return m_GlobalFont;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const std::vector<Widget::Ptr>& Container::getWidgets()
    {
        return m_Widgets;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const std::vector<sf::String>& Container::getWidgetNames()
    {
        return m_ObjName;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Container::add(const Widget::Ptr& widgetPtr, const sf::String& widgetName)
    {
        assert(widgetPtr != nullptr);

        widgetPtr->initialize(this);
        m_Widgets.push_back(widgetPtr);
        m_ObjName.push_back(widgetName);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Widget::Ptr Container::get(const sf::String& widgetName, bool recursive) const
    {
        for (unsigned int i = 0; i < m_ObjName.size(); ++i)
        {
            if (m_ObjName[i] == widgetName)
            {
                return m_Widgets[i];
            }
            else if (recursive && m_Widgets[i]->m_ContainerWidget)
            {
                Widget::Ptr widget = Container::Ptr(m_Widgets[i])->get(widgetName, true);
                if (widget != nullptr)
                    return widget;
            }
        }

        return nullptr;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Widget::Ptr Container::copy(const Widget::Ptr& oldWidget, const sf::String& newWidgetName)
    {
        Widget::Ptr newWidget = oldWidget.clone();
        newWidget->initialize(this);
        m_Widgets.push_back(newWidget);
        m_ObjName.push_back(newWidgetName);
        return newWidget;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Container::remove(const Widget::Ptr& widget)
    {
        remove(widget.get());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Container::remove(Widget* widget)
    {
        // Loop through every widget
        for (unsigned int i = 0; i < m_Widgets.size(); ++i)
        {
            // Check if the pointer matches
            if (m_Widgets[i].get() == widget)
            {
                // Unfocus the widget if it was focused
                if (m_FocusedWidget == widget)
                    unfocusWidgets();

                // Remove the widget
                m_Widgets.erase(m_Widgets.begin() + i);

                // Also emove the name it from the list
                m_ObjName.erase(m_ObjName.begin() + i);

                break;
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Container::removeAllWidgets()
    {
        // Clear the lists
        m_Widgets.clear();
        m_ObjName.clear();

        // There are no more widgets, so none of the widgets can be focused
        m_FocusedWidget = nullptr;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Container::setWidgetName(const Widget::Ptr& widget, const std::string& name)
    {
        for (unsigned int i = 0; i < m_Widgets.size(); ++i)
        {
            if (m_Widgets[i] == widget)
            {
                m_ObjName[i] = name;
                return true;
            }
        }

        return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Container::getWidgetName(const Widget::Ptr& widget, std::string& name) const
    {
        for (unsigned int i = 0; i < m_Widgets.size(); ++i)
        {
            if (m_Widgets[i] == widget)
            {
                name = m_ObjName[i];
                return true;
            }
        }

        return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Container::focusWidget(const Widget::Ptr& widget)
    {
        focusWidget(widget.get());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Container::focusWidget(Widget *const widget)
    {
        // Loop all the widgets
        for (unsigned int i = 0; i < m_Widgets.size(); ++i)
        {
            // Search for the widget that has to be focused
            if (m_Widgets[i].get() == widget)
            {
                // Only continue when the widget wasn't already focused
                if (m_FocusedWidget != widget)
                {
                    // Unfocus the currently focused widget
                    if (m_FocusedWidget)
                    {
                        m_FocusedWidget->m_Focused = false;
                        m_FocusedWidget->widgetUnfocused();
                    }

                    // Focus the new widget
                    m_FocusedWidget = widget;
                    widget->m_Focused = true;
                    widget->widgetFocused();
                }

                break;
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Container::focusNextWidget()
    {
        // Find the focused widget in the list of widgets
        unsigned int index = 0;
        for (; index < m_Widgets.size(); ++index)
        {
            if (m_Widgets[index].get() == m_FocusedWidget)
                break;
        }

        // Loop all widgets behind the focused one
        for (unsigned int i = index+1; i < m_Widgets.size(); ++i)
        {
            // If you are not allowed to focus the widget, then skip it
            if (m_Widgets[i]->m_AllowFocus == true)
            {
                // Make sure that the widget is visible and enabled
                if ((m_Widgets[i]->m_Visible) && (m_Widgets[i]->m_Enabled))
                {
                    if (m_FocusedWidget)
                    {
                        // unfocus the current widget
                        m_FocusedWidget->m_Focused = false;
                        m_FocusedWidget->widgetUnfocused();
                    }

                    // Focus on the new widget
                    m_FocusedWidget = m_Widgets[i].get();
                    m_Widgets[i]->m_Focused = true;
                    m_Widgets[i]->widgetFocused();
                    return;
                }
            }
        }

        // None of the widgets behind the focused one could be focused, so loop the ones before it
        if (m_FocusedWidget)
        {
            for (unsigned int i = 0; i < index; ++i)
            {
                // If you are not allowed to focus the widget, then skip it
                if (m_Widgets[i]->m_AllowFocus == true)
                {
                    // Make sure that the widget is visible and enabled
                    if ((m_Widgets[i]->m_Visible) && (m_Widgets[i]->m_Enabled))
                    {
                        // unfocus the current widget
                        m_FocusedWidget->m_Focused = false;
                        m_FocusedWidget->widgetUnfocused();

                        // Focus on the new widget
                        m_FocusedWidget = m_Widgets[i].get();
                        m_Widgets[i]->m_Focused = true;
                        m_Widgets[i]->widgetFocused();

                        return;
                    }
                }
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Container::focusPreviousWidget()
    {
        // Find the focused widget in the list of widgets
        unsigned int index = 0;
        for (; index < m_Widgets.size(); ++index)
        {
            if (m_Widgets[index].get() == m_FocusedWidget)
                break;
        }

        // Loop the widgets before the focused one
        if (m_FocusedWidget)
        {
            for (unsigned int i = index; i > 0; --i)
            {
                // If you are not allowed to focus the widget, then skip it
                if (m_Widgets[i-1]->m_AllowFocus == true)
                {
                    // Make sure that the widget is visible and enabled
                    if ((m_Widgets[i-1]->m_Visible) && (m_Widgets[i-1]->m_Enabled))
                    {
                        // unfocus the current widget
                        m_FocusedWidget->m_Focused = false;
                        m_FocusedWidget->widgetUnfocused();

                        // Focus on the new widget
                        m_FocusedWidget = m_Widgets[i-1].get();
                        m_Widgets[i-1]->m_Focused = true;
                        m_Widgets[i-1]->widgetFocused();

                        return;
                    }
                }
            }
        }

        // None of the widgets before the focused one could be focused, so loop all widgets behind the focused one
        for (unsigned int i = m_Widgets.size(); i > index+1; --i)
        {
            // If you are not allowed to focus the widget, then skip it
            if (m_Widgets[i-1]->m_AllowFocus == true)
            {
                // Make sure that the widget is visible and enabled
                if ((m_Widgets[i-1]->m_Visible) && (m_Widgets[i-1]->m_Enabled))
                {
                    if (m_FocusedWidget)
                    {
                        // unfocus the current widget
                        m_FocusedWidget->m_Focused = false;
                        m_FocusedWidget->widgetUnfocused();
                    }

                    // Focus on the new widget
                    m_FocusedWidget = m_Widgets[i-1].get();
                    m_Widgets[i-1]->m_Focused = true;
                    m_Widgets[i-1]->widgetFocused();
                    return;
                }
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Container::unfocusWidgets()
    {
        if (m_FocusedWidget)
        {
            m_FocusedWidget->m_Focused = false;
            m_FocusedWidget->widgetUnfocused();
            m_FocusedWidget = nullptr;
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Container::uncheckRadioButtons()
    {
        // Loop through all radio buttons and uncheck them
        for (unsigned int i = 0; i < m_Widgets.size(); ++i)
        {
            if (m_Widgets[i]->m_Callback.widgetType == Type_RadioButton)
                static_cast<RadioButton::Ptr>(m_Widgets[i])->uncheck();
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Container::moveWidgetToFront(Widget *const widget)
    {
        // Loop through all widgets
        for (unsigned int i = 0; i < m_Widgets.size(); ++i)
        {
            // Check if the widget is found
            if (m_Widgets[i].get() == widget)
            {
                // Copy the widget
                m_Widgets.push_back(m_Widgets[i]);
                m_ObjName.push_back(m_ObjName[i]);

                // Focus the widget if no other widget was focused
                if (m_FocusedWidget == nullptr)
                    m_FocusedWidget = widget;

                // Remove the old widget
                m_Widgets.erase(m_Widgets.begin() + i);
                m_ObjName.erase(m_ObjName.begin() + i);

                break;
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Container::moveWidgetToBack(Widget *const widget)
    {
        // Loop through all widgets
        for (unsigned int i = 0; i < m_Widgets.size(); ++i)
        {
            // Check if the widget is found
            if (m_Widgets[i].get() == widget)
            {
                // Copy the widget
                Widget::Ptr obj = m_Widgets[i];
                std::string name = m_ObjName[i];
                m_Widgets.insert(m_Widgets.begin(), obj);
                m_ObjName.insert(m_ObjName.begin(), name);

                // Remove the old widget
                m_Widgets.erase(m_Widgets.begin() + i + 1);
                m_ObjName.erase(m_ObjName.begin() + i + 1);

                break;
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Container::setTransparency(unsigned char transparency)
    {
        Widget::setTransparency(transparency);

        for (unsigned int i = 0; i < m_Widgets.size(); ++i)
            m_Widgets[i]->setTransparency(transparency);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Container::bindGlobalCallback(std::function<void(const Callback&)> func)
    {
        m_GlobalCallbackFunctions.push_back(func);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Container::unbindGlobalCallback()
    {
        m_GlobalCallbackFunctions.clear();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Container::loadWidgetsFromFile(const std::string& filename)
    {
        #define COMPARE_WIDGET(length, name, widgetName) \
            if (line.substr(0, length).compare(name) == 0) \
            { \
                line.erase(0, length); \
              \
                widgetPtr.push_back(widgetName::Ptr(*static_cast<Container*>(widgetPtr.back()), line).get()); \
                progress.push(0); \
            }

        std::stack<unsigned int> progress;
        std::list<Widget*> widgetPtr;

        // Open the file
        std::ifstream m_File(getResourcePath() + filename);

        // Check if the file was not opened
        if (m_File.is_open() == false)
            return false;

        // Stop reading when we reach the end of the file or when something went wrong
        bool failed = false;
        while (!m_File.eof() && !failed)
        {
            std::string line;
            std::getline(m_File, line);

            if (!line.empty())
            {
                // Search for a quote
                std::string::size_type quotePos1 = line.find('"');

                // Check if the quote was found or not
                if (quotePos1 == std::string::npos)
                {
                    // Remove all spaces and tabs from the whole line
                    line.erase(std::remove(line.begin(), line.end(), ' '), line.end());
                    line.erase(std::remove(line.begin(), line.end(), '\t'), line.end());
                    line.erase(std::remove(line.begin(), line.end(), '\r'), line.end());

                    // Convert the whole line to lowercase
                    line = toLower(line);
                }
                else // A quote was found
                {
                    // Only remove spaces and tabs until the quote
                    line.erase(std::remove(line.begin(), line.begin() + quotePos1, ' '), line.begin() + quotePos1);
                    quotePos1 = line.find('"');
                    line.erase(std::remove(line.begin(), line.begin() + quotePos1, '\t'), line.begin() + quotePos1);
                    quotePos1 = line.find('"');

                    // Convert the part before the quote to lowercase
                    line = toLower(line.substr(0, quotePos1)) + line.substr(quotePos1);

                    // Search for a second quote
                    std::string::size_type quotePos2 = line.find('"', quotePos1 + 1);

                    // There must always be a second quote
                    if (quotePos2 != std::string::npos)
                    {
                        // Remove all spaces and tabs after the quote
                        line.erase(std::remove(line.begin() + quotePos2, line.end(), ' '), line.end());
                        line.erase(std::remove(line.begin() + quotePos2, line.end(), '\t'), line.end());
                        line.erase(std::remove(line.begin() + quotePos2, line.end(), '\r'), line.end());

                        // Search for the quote again, because the position might have changed
                        quotePos2 = line.find('"', quotePos1 + 1);

                        // Search for backslashes between the quotes
                        std::string::size_type backslashPos = line.find('\\', quotePos1);
                        while (backslashPos < quotePos2)
                        {
                            // Check for special characters
                            if (line[backslashPos + 1] == 'n')
                            {
                                line[backslashPos] = '\n';
                                line.erase(backslashPos + 1, 1);
                                --quotePos2;
                            }
                            else if (line[backslashPos + 1] == 't')
                            {
                                line[backslashPos] = '\t';
                                line.erase(backslashPos + 1, 1);
                                --quotePos2;
                            }
                            else if (line[backslashPos + 1] == '\\')
                            {
                                line.erase(backslashPos + 1, 1);
                                --quotePos2;
                            }
                            else if (line[backslashPos + 1] == '"')
                            {
                                line[backslashPos] = '"';
                                line.erase(backslashPos + 1, 1);

                                // Find the next quote
                                quotePos2 = line.find('"', backslashPos + 1);
                                if (quotePos2 == std::string::npos)
                                {
                                    failed = true;
                                    break;
                                }
                            }

                            // Find the next backslash
                            backslashPos = line.find('\\', backslashPos + 1);
                        }

                        // There may never be more than two quotes
                        if (line.find('"', quotePos2 + 1) != std::string::npos)
                            failed = true;

                        // Convert the part behind the quote to lowercase
                        line = line.substr(0, quotePos2 + 1) + toLower(line.substr(quotePos2 + 1));

                        // Remove the quotes from the string
                        line.erase(quotePos1, 1);
                        line.erase(quotePos2 - 1, 1);
                    }
                    else // The second quote is missing
                        failed = true;
                }
            }

            // Only continue when the line hasn't become empty and nothing went wrong so far
            if (!line.empty() && !failed)
            {
                // Check if this is the first line
                if (progress.empty())
                {
                    // The first line should contain the beginning of the window section
                    if (line.substr(0, 7).compare("window:") == 0)
                    {
                        widgetPtr.push_back(this);
                        progress.push(0);
                        continue;
                    }
                    else // The first line is wrong
                    {
                        failed = true;
                        break;
                    }
                }

                // Check for opening and closing brackets
                if (progress.top() == 0)
                {
                    if (line.compare("{") == 0)
                    {
                        progress.pop();
                        progress.push(1);
                        continue;
                    }
                    else
                    {
                        failed = true;
                        break;
                    }
                }
                else
                {
                    if (line.compare("}") == 0)
                    {
                        widgetPtr.pop_back();
                        progress.pop();
                        continue;
                    }
                }

                // The line doesn't contain a '}', so check if it contains another widget
                bool widgetFound = true;
                COMPARE_WIDGET(4, "tab:", Tab)
                else COMPARE_WIDGET(5, "grid:", Grid)
                else COMPARE_WIDGET(6, "panel:", Panel)
                else COMPARE_WIDGET(6, "label:", Label)
                else COMPARE_WIDGET(7, "button:", Button)
                else COMPARE_WIDGET(7, "slider:", Slider)
                else COMPARE_WIDGET(8, "picture:", Picture)
                else COMPARE_WIDGET(8, "listbox:", ListBox)
                else COMPARE_WIDGET(8, "editbox:", EditBox)
                else COMPARE_WIDGET(8, "textbox:", TextBox)
                else COMPARE_WIDGET(8, "chatbox:", ChatBox)
                else COMPARE_WIDGET(8, "menubar:", MenuBar)
                else COMPARE_WIDGET(9, "checkbox:", Checkbox)
                else COMPARE_WIDGET(9, "combobox:", ComboBox)
                else COMPARE_WIDGET(9, "slider2d:", Slider2d)
                else COMPARE_WIDGET(10, "scrollbar:", Scrollbar)
                else COMPARE_WIDGET(11, "loadingbar:", LoadingBar)
                else COMPARE_WIDGET(11, "spinbutton:", SpinButton)
                else COMPARE_WIDGET(12, "radiobutton:", RadioButton)
                else COMPARE_WIDGET(12, "childwindow:", ChildWindow)
                else COMPARE_WIDGET(12, "spritesheet:", SpriteSheet)
                else COMPARE_WIDGET(16, "animatedpicture:", AnimatedPicture)
                else
                    widgetFound = false;

                // The line didn't contain a bracket or a new widget, so it must contain a property
                if (!widgetFound)
                {
                    std::string::size_type equalSignPosition = line.find('=');
                    if (equalSignPosition == std::string::npos)
                    {
                        failed = true;
                        break;
                    }

                    if (!widgetPtr.back()->setProperty(line.substr(0, equalSignPosition), line.substr(equalSignPosition + 1)))
                        failed = true;
                }
            }
        }

        m_File.close();

        if (failed)
            return false;

        return true;

        #undef COMPARE_WIDGET
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Container::saveWidgetsToFile(const std::string& filename)
    {
        std::string tabs;

        // Open the file
        std::ofstream m_File(filename);

        // Check if the file was not opened
        if (m_File.is_open() == false)
            return false;

        m_File << tabs << "Window:" << std::endl;
        m_File << tabs << "{" << std::endl;
        tabs += "\t";

        std::function< void (const std::vector<sf::String>&, const std::vector<Widget::Ptr>&) > saveWidgets;
        saveWidgets = [this, &m_File, &tabs, &saveWidgets] (const std::vector<sf::String>& widgetName, const std::vector<Widget::Ptr>& widgets)
        {
            auto nameIt = widgetName.cbegin();
            for (auto widgetIt = widgets.cbegin(); widgetIt != widgets.cend(); ++widgetIt, ++nameIt)
            {
                switch ((*widgetIt)->getWidgetType())
                {
                    case Type_Tab:              m_File << tabs << "Tab: "; break;
                    case Type_Grid:             m_File << tabs << "Grid: "; break;
                    case Type_Panel:            m_File << tabs << "Panel: "; break;
                    case Type_Label:            m_File << tabs << "Label: "; break;
                    case Type_Button:           m_File << tabs << "Button: "; break;
                    case Type_Slider:           m_File << tabs << "Slider: "; break;
                    case Type_Picture:          m_File << tabs << "Picture: "; break;
                    case Type_ListBox:          m_File << tabs << "ListBox: "; break;
                    case Type_EditBox:          m_File << tabs << "EditBox: "; break;
                    case Type_TextBox:          m_File << tabs << "TextBox: "; break;
                    case Type_ChatBox:          m_File << tabs << "ChatBox: "; break;
                    case Type_MenuBar:          m_File << tabs << "MenuBar: "; break;
                    case Type_Checkbox:         m_File << tabs << "Checkbox: "; break;
                    case Type_ComboBox:         m_File << tabs << "ComboBox: "; break;
                    case Type_Slider2d:         m_File << tabs << "Slider2d: "; break;
                    case Type_Scrollbar:        m_File << tabs << "Scrollbar: "; break;
                    case Type_LoadingBar:       m_File << tabs << "LoadingBar: "; break;
                    case Type_SpinButton:       m_File << tabs << "SpinButton: "; break;
                    case Type_RadioButton:      m_File << tabs << "RadioButton: "; break;
                    case Type_ChildWindow:      m_File << tabs << "ChildWindow: "; break;
                    case Type_SpriteSheet:      m_File << tabs << "SpriteSheet: "; break;
                    case Type_AnimatedPicture:  m_File << tabs << "AnimatedPicture: "; break;
                    default:
                        continue;
                }
                m_File << "\"" << nameIt->toAnsiString() << "\"" << std::endl;

                m_File << tabs << "{" << std::endl;
                tabs += "\t";

                std::string value;
                if ((*widgetIt)->getProperty("Filename", value))
                {
                    m_File << tabs << "Filename = \"" << value << "\"" << std::endl;
                }
                else if ((*widgetIt)->getProperty("ConfigFile", value))
                {
                    m_File << tabs << "ConfigFile = \"" << value << "\"" << std::endl;
                }

                auto properties = (*widgetIt)->getPropertyList();
                for (auto propertyIt = properties.cbegin(); propertyIt != properties.cend(); ++propertyIt)
                {
                    if ((propertyIt->first != "Filename") && (propertyIt->first != "ConfigFile"))
                    {
                        (*widgetIt)->getProperty(propertyIt->first, value);

                        if (propertyIt->second == "string")
                            m_File << tabs << propertyIt->first << " = \"" << value << "\"" << std::endl;
                        else
                        {
                            if (!value.empty())
                                m_File << tabs << propertyIt->first << " = " << value << std::endl;
                        }
                    }
                }

                if ((*widgetIt)->m_ContainerWidget)
                {
                    m_File << std::endl;
                    saveWidgets(Container::Ptr(*widgetIt)->getWidgetNames(), Container::Ptr(*widgetIt)->getWidgets());
                }

                tabs.erase(tabs.length()-1);
                m_File << tabs << "}" << std::endl << std::endl;
            }
        };

        saveWidgets(m_ObjName, m_Widgets);

        tabs.erase(tabs.length()-1);
        m_File << tabs << "}" << std::endl;

        return true;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    sf::Vector2f Container::getWidgetsOffset() const
    {
        return sf::Vector2f(0, 0);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Container::addChildCallback(const Callback& callback)
    {
        // If there is no global callback function then send the callback to the parent
        if (m_GlobalCallbackFunctions.empty())
            m_Parent->addChildCallback(callback);
        else
        {
            // Loop through all callback functions and call them
            for (auto it = m_GlobalCallbackFunctions.cbegin(); it != m_GlobalCallbackFunctions.cend(); ++it)
                (*it)(callback);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Container::leftMousePressed(float x, float y)
    {
        sf::Event event;
        event.type = sf::Event::MouseButtonPressed;
        event.mouseButton.button = sf::Mouse::Left;
        event.mouseButton.x = static_cast<int>(x - getPosition().x);
        event.mouseButton.y = static_cast<int>(y - getPosition().y);

        // Let the event manager handle the event
        handleEvent(event);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Container::leftMouseReleased(float x , float y)
    {
        sf::Event event;
        event.type = sf::Event::MouseButtonReleased;
        event.mouseButton.button = sf::Mouse::Left;
        event.mouseButton.x = static_cast<int>(x - getPosition().x);
        event.mouseButton.y = static_cast<int>(y - getPosition().y);

        // Let the event manager handle the event
        handleEvent(event);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Container::mouseMoved(float x, float y)
    {
        sf::Event event;
        event.type = sf::Event::MouseMoved;
        event.mouseMove.x = static_cast<int>(x - getPosition().x);
        event.mouseMove.y = static_cast<int>(y - getPosition().y);

        // Let the event manager handle the event
        handleEvent(event);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Container::keyPressed(const sf::Event::KeyEvent& event)
    {
        sf::Event newEvent;
        newEvent.type = sf::Event::KeyPressed;
        newEvent.key = event;

        // Let the event manager handle the event
        handleEvent(newEvent);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Container::textEntered(sf::Uint32 key)
    {
        sf::Event event;
        event.type = sf::Event::TextEntered;
        event.text.unicode = key;

        // Let the event manager handle the event
        handleEvent(event);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Container::mouseWheelMoved(int delta, int x, int y)
    {
        sf::Event event;
        event.type = sf::Event::MouseWheelMoved;
        event.mouseWheel.delta = delta;
        event.mouseWheel.x = static_cast<int>(x - getPosition().x);
        event.mouseWheel.y = static_cast<int>(y - getPosition().y);

        // Let the event manager handle the event
        handleEvent(event);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Container::mouseNotOnWidget()
    {
        if (m_MouseHover == true)
        {
            mouseLeftWidget();

            for (unsigned int i = 0; i < m_Widgets.size(); ++i)
                m_Widgets[i]->mouseNotOnWidget();

            m_MouseHover = false;
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Container::mouseNoLongerDown()
    {
        Widget::mouseNoLongerDown();

        for (unsigned int i=0; i<m_Widgets.size(); ++i)
            m_Widgets[i]->mouseNoLongerDown();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Container::widgetUnfocused()
    {
        unfocusWidgets();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Container::initialize(Container *const parent)
    {
        m_Parent = parent;
        setGlobalFont(m_Parent->getGlobalFont());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Container::update()
    {
        // Loop through all widgets
        for (unsigned int i = 0; i < m_Widgets.size(); ++i)
        {
            // Check if the widget is a container or a widget that uses the time
            if (m_Widgets[i]->m_AnimatedWidget)
            {
                // Update the elapsed time
                m_Widgets[i]->m_AnimationTimeElapsed += m_AnimationTimeElapsed;
                m_Widgets[i]->update();
            }
        }

        m_AnimationTimeElapsed = sf::Time();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Container::handleEvent(sf::Event& event)
    {
        // Check if a mouse button has moved
        if (event.type == sf::Event::MouseMoved)
        {
            // Loop through all widgets
            for (unsigned int i=0; i<m_Widgets.size(); ++i)
            {
                // Check if the mouse went down on the widget
                if (m_Widgets[i]->m_MouseDown)
                {
                    // Some widgets should always receive mouse move events while dragging them, even if the mouse is no longer on top of them.
                    if ((m_Widgets[i]->m_DraggableWidget) || (m_Widgets[i]->m_ContainerWidget))
                    {
                        m_Widgets[i]->mouseMoved(static_cast<float>(event.mouseMove.x), static_cast<float>(event.mouseMove.y));
                        return true;
                    }
                }
            }

            // Check if the mouse is on top of a widget
            Widget::Ptr widget = mouseOnWhichWidget(static_cast<float>(event.mouseMove.x), static_cast<float>(event.mouseMove.y));
            if (widget != nullptr)
            {
                // Send the event to the widget
                widget->mouseMoved(static_cast<float>(event.mouseMove.x), static_cast<float>(event.mouseMove.y));
                return true;
            }

            return false;
        }

        // Check if a mouse button was pressed
        else if (event.type == sf::Event::MouseButtonPressed)
        {
            // Check if the left mouse was pressed
            if (event.mouseButton.button == sf::Mouse::Left)
            {
                // Check if the mouse is on top of a widget
                Widget::Ptr widget = mouseOnWhichWidget(static_cast<float>(event.mouseButton.x), static_cast<float>(event.mouseButton.y));
                if (widget != nullptr)
                {
                    // Focus the widget
                    focusWidget(widget.get());

                    // Check if the widget is a container
                    if (widget->m_ContainerWidget)
                    {
                        // If another widget was focused then unfocus it now
                        if ((m_FocusedWidget) && (m_FocusedWidget != widget.get()))
                        {
                            m_FocusedWidget->m_Focused = false;
                            m_FocusedWidget->widgetUnfocused();
                            m_FocusedWidget = nullptr;
                        }
                    }

                    widget->leftMousePressed(static_cast<float>(event.mouseButton.x), static_cast<float>(event.mouseButton.y));
                    return true;
                }
                else // The mouse didn't went down on a widget, so unfocus the focused widget
                    unfocusWidgets();
            }

            return false;
        }

        // Check if a mouse button was released
        else if (event.type == sf::Event::MouseButtonReleased)
        {
            // Check if the left mouse was released
            if (event.mouseButton.button == sf::Mouse::Left)
            {
                // Check if the mouse is on top of a widget
                Widget::Ptr widget = mouseOnWhichWidget(static_cast<float>(event.mouseButton.x), static_cast<float>(event.mouseButton.y));
                if (widget != nullptr)
                    widget->leftMouseReleased(static_cast<float>(event.mouseButton.x), static_cast<float>(event.mouseButton.y));

                // Tell all the other widgets that the mouse has gone up
                for (std::vector<Widget::Ptr>::iterator it = m_Widgets.begin(); it != m_Widgets.end(); ++it)
                {
                    if (*it != widget)
                        (*it)->mouseNoLongerDown();
                }

                if (widget != nullptr)
                    return true;
            }

            return false;
        }

        // Check if a key was pressed
        else if (event.type == sf::Event::KeyPressed)
        {
            // Only continue when the character was recognised
            if (event.key.code != sf::Keyboard::Unknown)
            {
                // Check if there is a focused widget
                if (m_FocusedWidget)
                {
                    // Tell the widget that the key was pressed
                    m_FocusedWidget->keyPressed(event.key);
                    return true;
                }
            }

            return false;
        }

        // Check if a key was released
        else if (event.type == sf::Event::KeyReleased)
        {
            // Change the focus to another widget when the tab key was pressed
            if (event.key.code == sf::Keyboard::Tab)
                return tabKeyPressed();
            else
                return false;
        }

        // Also check if text was entered (not a special key)
        else if (event.type == sf::Event::TextEntered)
        {
            // Check if the character that we pressed is allowed
            if ((event.text.unicode >= 32) && (event.text.unicode != 127))
            {
                // Tell the widget that the key was pressed
                if (m_FocusedWidget)
                {
                    m_FocusedWidget->textEntered(event.text.unicode);
                    return true;
                }
            }

            return false;
        }

        // Check for mouse wheel scrolling
        else if (event.type == sf::Event::MouseWheelMoved)
        {
            // Find the widget under the mouse
            Widget::Ptr widget = mouseOnWhichWidget(static_cast<float>(event.mouseWheel.x), static_cast<float>(event.mouseWheel.y));
            if (widget != nullptr)
            {
                // Send the event to the widget
                widget->mouseWheelMoved(event.mouseWheel.delta, event.mouseWheel.x,  event.mouseWheel.y);
                return true;
            }

            return false;
        }
        else // Event is ignored
            return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Container::focusNextWidgetInContainer()
    {
        // Don't do anything when the tab key usage is disabled
        if (TGUI_TabKeyUsageEnabled == false)
            return false;

        // Find the focused widget in the list of widgets
        unsigned int index = 0;
        for (; index < m_Widgets.size(); ++index)
        {
            if (m_Widgets[index].get() == m_FocusedWidget)
                break;
        }

        // Loop through all widgets
        for (unsigned int i = index+1; i < m_Widgets.size(); ++i)
        {
            // If you are not allowed to focus the widget, then skip it
            if (m_Widgets[i]->m_AllowFocus == true)
            {
                // Make sure that the widget is visible and enabled
                if ((m_Widgets[i]->m_Visible) && (m_Widgets[i]->m_Enabled))
                {
                    // Container widgets can only be focused it they contain focusable widgets
                    if ((!m_Widgets[i]->m_ContainerWidget) || (Container::Ptr(m_Widgets[i])->focusNextWidgetInContainer()))
                    {
                        if (m_FocusedWidget)
                        {
                            // Unfocus the current widget
                            m_FocusedWidget->m_Focused = false;
                            m_FocusedWidget->widgetUnfocused();
                        }

                        // Focus on the new widget
                        m_FocusedWidget = m_Widgets[i].get();
                        m_Widgets[i]->m_Focused = true;
                        m_Widgets[i]->widgetFocused();

                        return true;
                    }
                }
            }
        }

        // We have the highest id
        unfocusWidgets();
        return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Container::tabKeyPressed()
    {
        // Don't do anything when the tab key usage is disabled
        if (TGUI_TabKeyUsageEnabled == false)
            return false;

        // Check if a container is focused
        if (m_FocusedWidget)
        {
            if (m_FocusedWidget->m_ContainerWidget)
            {
                // Focus the next widget in container
                if (static_cast<Container*>(m_FocusedWidget)->focusNextWidgetInContainer())
                    return true;
            }
        }

        // Find the focused widget in the list of widgets
        unsigned int index = 0;
        for (; index < m_Widgets.size(); ++index)
        {
            if (m_Widgets[index].get() == m_FocusedWidget)
                break;
        }

        // Loop all widgets behind the focused one
        for (unsigned int i = index+1; i < m_Widgets.size(); ++i)
        {
            // If you are not allowed to focus the widget, then skip it
            if (m_Widgets[i]->m_AllowFocus == true)
            {
                // Make sure that the widget is visible and enabled
                if ((m_Widgets[i]->m_Visible) && (m_Widgets[i]->m_Enabled))
                {
                    if (m_FocusedWidget)
                    {
                        // unfocus the current widget
                        m_FocusedWidget->m_Focused = false;
                        m_FocusedWidget->widgetUnfocused();
                    }

                    // Focus on the new widget
                    m_FocusedWidget = m_Widgets[i].get();
                    m_Widgets[i]->m_Focused = true;
                    m_Widgets[i]->widgetFocused();
                    return true;
                }
            }
        }

        // None of the widgets behind the focused one could be focused, so loop the ones before it
        if (m_FocusedWidget)
        {
            for (unsigned int i = 0; i < index; ++i)
            {
                // If you are not allowed to focus the widget, then skip it
                if (m_Widgets[i]->m_AllowFocus == true)
                {
                    // Make sure that the widget is visible and enabled
                    if ((m_Widgets[i]->m_Visible) && (m_Widgets[i]->m_Enabled))
                    {
                        // unfocus the current widget
                        m_FocusedWidget->m_Focused = false;
                        m_FocusedWidget->widgetUnfocused();

                        // Focus on the new widget
                        m_FocusedWidget = m_Widgets[i].get();
                        m_Widgets[i]->m_Focused = true;
                        m_Widgets[i]->widgetFocused();
                        return true;
                    }
                }
            }
        }

        // If the currently focused container widget is the only widget to focus, then focus its next child widget
        if ((m_FocusedWidget) && (m_FocusedWidget->m_ContainerWidget))
        {
            static_cast<Container*>(m_FocusedWidget)->tabKeyPressed();
            return true;
        }

        return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Widget::Ptr Container::mouseOnWhichWidget(float x, float y)
    {
        bool widgetFound = false;
        Widget::Ptr widget = nullptr;

        // Loop through all widgets
        for (std::vector<Widget::Ptr>::reverse_iterator it = m_Widgets.rbegin(); it != m_Widgets.rend(); ++it)
        {
            // Check if the widget is visible and enabled
            if (((*it)->m_Visible) && ((*it)->m_Enabled))
            {
                if (widgetFound == false)
                {
                    // Return the widget if the mouse is on top of it
                    if ((*it)->mouseOnWidget(x, y))
                    {
                        widget = *it;
                        widgetFound = true;
                    }
                }
                else // The widget was already found, so tell the other widgets that the mouse can't be on them
                    (*it)->mouseNotOnWidget();
            }
        }

        return widget;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Container::drawWidgetContainer(sf::RenderTarget* target, const sf::RenderStates& states) const
    {
        // Draw all widgets when they are visible
        for (unsigned int i = 0; i < m_Widgets.size(); ++i)
        {
            if (m_Widgets[i]->m_Visible)
                m_Widgets[i]->draw(*target, states);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void GuiContainer::unbindGlobalCallback()
    {
        m_GlobalCallbackFunctions.erase(++m_GlobalCallbackFunctions.begin(), m_GlobalCallbackFunctions.end());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void GuiContainer::setSize(float, float)
    {
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    sf::Vector2f GuiContainer::getSize() const
    {
        return sf::Vector2f(m_Window->getSize());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    GuiContainer* GuiContainer::clone()
    {
        return nullptr;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool GuiContainer::mouseOnWidget(float, float)
    {
        return true;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void GuiContainer::draw(sf::RenderTarget&, sf::RenderStates) const
    {
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
