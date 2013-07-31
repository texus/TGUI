/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus's Graphical User Interface
// Copyright (C) 2012-2013 Bruno Van de Velde (vdv_b@tgui.eu)
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


#include <TGUI/TGUI.hpp>

#include <stack>
#include <cmath>
#include <cassert>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Container::Container()
    {
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Container::Container(const Container& containerToCopy) :
    m_GlobalFont             (containerToCopy.m_GlobalFont),
    m_GlobalCallbackFunctions(containerToCopy.m_GlobalCallbackFunctions)
    {
        // Copy all the widgets
        for (unsigned int i = 0; i < containerToCopy.m_EventManager.m_Widgets.size(); ++i)
        {
            m_EventManager.m_Widgets.push_back(containerToCopy.m_EventManager.m_Widgets[i].clone());
            m_ObjName.push_back(containerToCopy.m_ObjName[i]);

            m_EventManager.m_Widgets.back()->m_Parent = this;
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
            // Copy the font and the callback functions
            m_GlobalFont = right.m_GlobalFont;
            m_GlobalCallbackFunctions = right.m_GlobalCallbackFunctions;

            // Remove all the old widgets
            removeAllWidgets();

            // Copy all the widgets
            for (unsigned int i=0; i<right.m_EventManager.m_Widgets.size(); ++i)
            {
                m_EventManager.m_Widgets.push_back(right.m_EventManager.m_Widgets[i].clone());
                m_ObjName.push_back(right.m_ObjName[i]);

                m_EventManager.m_Widgets.back()->m_Parent = this;
            }
        }

        return *this;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Container::setGlobalFont(const std::string& filename)
    {
        return m_GlobalFont.loadFromFile(filename);
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

    struct Operation
    {
        enum Ops
        {
            Add,
            Subtract,
            Multiply,
            Divide
        };
    };

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    float evaluate(std::string expression)
    {
        /// TODO: Rewrite to use postfix instead of infix

        std::string::size_type openingBracketPos;
        std::string::size_type closingBracketPos;

        std::vector<float> numbers;
        std::vector<Operation::Ops> operations;

        bool readingNumber = false;
        bool commaFound = false;
        bool numbersBehindComma = 0;

        // Search for an opening bracket
        openingBracketPos = expression.rfind('(');

        // Check if a bracket was found
        while (openingBracketPos != std::string::npos)
        {
            // There has to be a closing bracket
            closingBracketPos = expression.find(')', openingBracketPos+1);

            // Give an error when there was no closing bracket
            if (closingBracketPos == std::string::npos)
            {
                TGUI_OUTPUT("TGUI error: Failed to parse string, missing closing bracket.");
                return 0;
            }

            // Evaluate the sting between the brackets
            float factor = evaluate(expression.substr(openingBracketPos + 1, closingBracketPos - openingBracketPos - 1));

            // Replace the brackets with the factor
            expression.erase(openingBracketPos, closingBracketPos - openingBracketPos + 1);
            expression.insert(openingBracketPos, to_string(factor));

            // Find the next opening bracket
            openingBracketPos = expression.rfind('(');
        }

        // Loop through all characters
        for (std::string::iterator it = expression.begin(); it != expression.end(); ++it)
        {
            // Read the string character by character
            char nextChar = *it;

            // Check if the character is a number
            if ((nextChar > 47) && (nextChar < 58))
            {
                // Check if we were already reading a number
                if (readingNumber)
                {
                    // Check if a comma was found before the character
                    if (commaFound)
                    {
                        // Add the number
                        ++numbersBehindComma;
                        numbers.back() += (nextChar - 48) / pow(10.f, numbersBehindComma);
                    }
                    else // No comma
                    {
                        // Append the number
                        numbers.back() *= 10;
                        numbers.back() += nextChar - 48;
                    }
                }
                else // This is the start of the number
                {
                    // Add this new number
                    numbers.push_back(nextChar - 48.f);
                    readingNumber = true;
                }
            }

            // Check if the character is a comma
            else if ((nextChar == 44) || (nextChar == 46))
            {
                // From now on, all new numbers will be behind the comma
                commaFound = true;

                // If your number starts with a comma then add a zero
                if (readingNumber == false)
                {
                    numbers.push_back(0);
                    readingNumber = true;
                }
            }

            // The character wasn't a number
            else
            {
                // Ignore spaces
                if (nextChar == 32)
                    continue;

                // If you were not reading a number yet then this might be a unary operator
                if (readingNumber == false)
                {
                    // Ignore a plus
                    if (nextChar == 43)
                        break;

                    // Handle subtraction
                    else if (nextChar == 45)
                    {
                        numbers.push_back(0);
                        operations.push_back(Operation::Subtract);
                    }
                }
                else // We were reading a number
                {
                    // Reset some data
                    readingNumber = false;
                    commaFound = false;
                    numbersBehindComma = 0;
                }

                // Find out what the operation is
                if (nextChar == 43)
                    operations.push_back(Operation::Add);
                else if (nextChar == 45)
                    operations.push_back(Operation::Subtract);
                else if ((nextChar == 42) || (nextChar == 120))
                    operations.push_back(Operation::Multiply);
                else if (nextChar == 47)
                    operations.push_back(Operation::Divide);
                else
                {
                    // An unknow character was found
                    TGUI_OUTPUT(std::string("TGUI error: Failed to parse string, unknown character found: '") + nextChar + "'.");
                    return 0;
                }
            }
        }

        // Make sure the number of operators is correct
        if (numbers.size() != operations.size() + 1)
        {
            TGUI_OUTPUT("TGUI error: Failed to parse string, incorrect number of operators.");
            return 0;
        }

        // Look for the multiplication or divide operations
        unsigned int i = 0;
        while (i<operations.size())
        {
            if (operations[i] == Operation::Multiply)
            {
                // Multiply the numbers
                numbers[i] *= numbers[i+1];

                // Remove it from the list
                numbers.erase(numbers.begin()+i+1);
                operations.erase(operations.begin()+i);
            }
            else if (operations[i] == Operation::Divide)
            {
                // Divide the numbers
                numbers[i] /= numbers[i+1];

                // Remove it from the list
                numbers.erase(numbers.begin()+i+1);
                operations.erase(operations.begin()+i);
            }
            else
                ++i;
        }

        // Look for the add or subtract operations
        i = 0;
        while (i<operations.size())
        {
            if (operations[i] == Operation::Add)
            {
                // Add the numbers
                numbers[i] += numbers[i+1];

                // Remove it from the list
                numbers.erase(numbers.begin()+i+1);
                operations.erase(operations.begin()+i);
            }
            else if (operations[i] == Operation::Subtract)
            {
                // Subtract the numbers
                numbers[i] -= numbers[i+1];

                // Remove it from the list
                numbers.erase(numbers.begin()+i+1);
                operations.erase(operations.begin()+i);
            }
            else
                ++i;
        }

        // When passing here, i should be 0, there should be one answer left and there shouldn't be any operations left
        if ((i == 0) && (numbers.size() == 1) && (operations.empty()))
            return numbers[0];
        else
        {
            TGUI_OUTPUT("TGUI error: Failed to parse string, incorrect result.");
            return 0;
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    float readFloat(std::string expression)
    {
        // Check if the expression is empty
        if (expression.empty())
        {
            TGUI_OUTPUT("TGUI error: Empty expression, using 0 as result value.");
            return 0;
        }

        // Look for quotes
        if ((expression[0] == '"') && (expression[expression.length()-1] == '"'))
        {
            // When quotes are found then erase them
            expression.erase(0, 1);
            expression.erase(expression.length()-1, 1);

            // The expression should still contain something
            if (expression.empty())
            {
                TGUI_OUTPUT("TGUI error: Expression with empty string or single quote.");
                return 0;
            }

            // Calculate the value
            return evaluate(expression);
        }
        else
            return std::stof(expression);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    int readInt(std::string expression)
    {
        // Check if the expression is empty
        if (expression.empty())
        {
            TGUI_OUTPUT("TGUI error: Empty expression, using 0 as result value.");
            return 0;
        }

        // Look for quotes
        if ((expression[0] == '"') && (expression[expression.length()-1] == '"'))
        {
            // When quotes are found then erase them
            expression.erase(0, 1);
            expression.erase(expression.length()-1, 1);

            // The expression should still contain something
            if (expression.empty())
            {
                TGUI_OUTPUT("TGUI error: Expression with empty string or single quote.");
                return 0;
            }

            // Calculate the value
            return static_cast<int>(evaluate(expression) + 0.5f);
        }
        else
            return std::stoi(expression);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Container::loadWidgetsFromFile(const std::string& filename)
    {
/// \todo This function should be rewritten.
/// \todo All widgets should be capable of loading themselves out of a string.
/// \todo Perhaps it would be better to switch to xml parsing.

        // I wrote these defines to avoid having the same code over and over again
        #define CHECK_SHARED_PROPERTIES(name) \
            if (line.substr(0, 5).compare("size=") == 0) \
            { \
                Vector2f size; \
                if (extractVector2f(line.erase(0, 5), size)) \
                    name->setSize(size.x, size.y); \
                else \
                    failed = true; \
            } \
            else if (line.substr(0, 9).compare("position=") == 0) \
            { \
                line.erase(0, 9); \
                Vector2f position; \
                if (extractVector2f(line, position)) \
                    name->setPosition(position); \
                else \
                    failed = true; \
            } \
            else if (line.substr(0, 11).compare("callbackid=") == 0) \
            { \
                line.erase(0, 11); \
                name->setCallbackId(readInt(line)); \
            }

        #define CHECK_FOR_QUOTES \
            if (line.empty()) \
            { \
                failed = true; \
                break; \
            } \
             \
            if ((line[0] == '"') && (line[line.length()-1] == '"')) \
            { \
                line.erase(0, 1); \
                line.erase(line.length()-1, 1); \
            } \
            else \
            { \
                failed = true; \
                break; \
            } \

        #define CHECK_BOOL(boolean) \
            if (line.compare("true") == 0) \
                boolean = true; \
            else if (line.compare("false") == 0) \
                boolean = false; \
            else \
            { \
                if (std::stoi(line)) \
                    boolean = true; \
                else \
                    boolean = false; \
            }

         #define COMPARE_WIDGET(length, name, widgetName) \
            if (line.substr(0, length).compare(name) == 0) \
            { \
                line.erase(0, length); \
              \
                if (line.empty() == false) \
                { \
                    CHECK_FOR_QUOTES \
                } \
              \
                extraPtr = widgetName::Ptr(*parentPtr.top(), line); \
                widgetID = Type_##widgetName + 1; \
                progress.push(0); \
            }

        #define START_LOADING_WIDGET \
            if (progress.top() == 0) \
            { \
                if (line.compare("{") == 0) \
                { \
                    progress.pop(); \
                    progress.push(1); \
                    break; \
                } \
                else \
                { \
                    failed = true; \
                    break; \
                } \
            } \
            else \
            { \
                if (line.compare("}") == 0) \
                { \
                    widgetID = parentID.top(); \
                    progress.pop(); \
                    break; \
                } \
            }

        // During the process some variables are needed to store what exactly was going on.
        bool failed = false;
        std::stack<Container*> parentPtr;
        std::stack<unsigned int> parentID;
        std::stack<unsigned int> progress;
        unsigned int widgetID = 0;
        SharedWidgetPtr<Widget> extraPtr;
        bool multilineComment = false;

        std::vector<std::string> defineTokens;
        std::vector<std::string> defineValues;

        // Create a file widget
        std::ifstream m_File;

        // Open the file
        m_File.open(filename, std::ifstream::in);

        // Check if the file was not opened
        if (m_File.is_open() == false)
            return false;

        // Stop reading when we reach the end of the file
        while (!m_File.eof() && !failed)
        {
            // Get the next line
            std::string line;
            std::getline(m_File, line);

            // Check if there is a multiline comment
            if (multilineComment)
            {
                // Search for an asterisk
                std::string::size_type commentPos = line.find('*');

                // Check if there is an asterisk
                if (commentPos != std::string::npos)
                {
                    // Make sure the asterisk is not the last character on the line
                    if (line.length() > commentPos + 1)
                    {
                        // Check if the next character is a slash
                        if (line[commentPos+1] == '/')
                        {
                            // Erase the first part of the line
                            line.erase(0, commentPos + 2);

                            // The multiline comment has been processed
                            multilineComment = false;
                        }
                        else // There is no end of the comment in this line
                            line = "";
                    }
                    else // There is no end of the comment in this line
                        line = "";
                }
                else // There is no end of the comment in this line
                    line = "";
            }

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

                    // Search for comments
                    std::string::size_type commentPos = line.find('/');

                    // Check if a slash was found
                    if (commentPos != std::string::npos)
                    {
                        // Make sure the slash is not the last character on the line
                        if (line.length() > commentPos + 1)
                        {
                            // Erase the comment (if there is one)
                            if (line[commentPos+1] == '/')
                                line.erase(commentPos);
                            else if (line[commentPos+1] == '*')
                            {
                                // Remove the rest of the line
                                line.erase(commentPos);

                                // From now on, everything is comment
                                multilineComment = true;
                            }
                            else // There is a slash in the middle of nowhere. It shouldn't be there.
                                failed = true;
                        }
                        else // There is a slash on the end of the line. It shouldn't be there.
                            failed = true;
                    }

                    // Convert the whole line to lowercase
                    for (unsigned int i = 0; i < line.length(); i++)
                    {
                        if ((line[i] > 64) && (line[i] < 91))
                            line[i] += 32;
                    }
                }
                else // A quote was found
                {
                    // Only remove spaces until the quote
                    line.erase(std::remove(line.begin(), line.begin() + quotePos1, ' '), line.begin() + quotePos1);

                    // Search for the quote again, because the position might have changed
                    quotePos1 = line.find('"');

                    // Only remove tabs until the quote
                    line.erase(std::remove(line.begin(), line.begin() + quotePos1, '\t'), line.begin() + quotePos1);

                    // Search for the quote again, because the position might have changed
                    quotePos1 = line.find('"');

                    // Search for comments
                    std::string::size_type commentPos = line.substr(0, quotePos1).find('/');

                    // Check if a slash was found
                    if (commentPos != std::string::npos)
                    {
                        // Erase the comment (if there is one)
                        if (line[commentPos+1] == '/')
                            line.erase(commentPos);
                        else if (line[commentPos+1] == '*')
                        {
                            // Remove the rest of the line
                            line.erase(commentPos);

                            // From now on, everything is comment
                            multilineComment = true;
                        }
                        else // There was a slash in the middle of nowhere
                        {
                            failed = true;
                            break;
                        }
                    }

                    // Search for the quote again, because the position might have changed
                    quotePos1 = line.find('"');

                    // The quote might have been behind the comment
                    if (quotePos1 != std::string::npos)
                    {
                        // Convert the part before the quote to lowercase
                        for (unsigned int i = 0; i < quotePos1; i++)
                        {
                            if ((line[i] > 64) && (line[i] < 91))
                                line[i] += 32;
                        }

                        // Search for a second quote
                        std::string::size_type quotePos2 = line.find('"', quotePos1 + 1);

                        // There must always be a second quote
                        if (quotePos2 != std::string::npos)
                        {
                            // Remove all spaces and tabs after the quote
                            line.erase(std::remove(line.begin() + quotePos2, line.end(), ' '), line.end());
                            line.erase(std::remove(line.begin() + quotePos2, line.end(), '\t'), line.end());
                            line.erase(std::remove(line.begin() + quotePos2, line.end(), '\r'), line.end());

                            // Search for comments
                            commentPos = line.find('/', quotePos2 + 1);

                            // Check if a slash was found
                            if (commentPos != std::string::npos)
                            {
                                // Make sure the slash is not the last character on the line
                                if (line.length() > commentPos + 1)
                                {
                                    // Erase the comment (if there is one)
                                    if (line[commentPos+1] == '/')
                                        line.erase(commentPos);
                                    else if (line[commentPos+1] == '*')
                                    {
                                        // Remove the rest of the line
                                        line.erase(commentPos);

                                        // From now on, everything is comment
                                        multilineComment = true;
                                    }
                                    else // There is a slash in the middle of nowhere. It shouldn't be there.
                                        failed = true;
                                }
                                else // There is a slash on the end of the line. It shouldn't be there.
                                    failed = true;
                            }

                            if (failed)
                                break;

                            // Search for the quote again, because the position might have changed
                            quotePos2 = line.find('"', quotePos1 + 1);

                            // Search for backslashes between the quotes
                            std::string::size_type backslashPos = line.find('\\', quotePos1);

                            // Check if a backlash was found before the second quote
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
                            for (std::string::size_type i = quotePos2; i < line.length(); i++)
                            {
                                if ((line[i] > 64) && (line[i] < 91))
                                    line[i] += 32;
                            }
                        }
                        else // The second quote is missing
                            failed = true;
                    }
                }
            }

            if (failed)
                break;

            // Only continue when the line is not empty
            if (!line.empty())
            {
                // Check if something was defined
                if (defineTokens.empty() == false)
                {
                    // Loop through all tokens
                    for (unsigned int i=0; i<defineTokens.size(); ++i)
                    {
                        // Search for every token in the line
                        std::string::size_type tokenPos = line.find(defineTokens[i]);

                        // Check if a token was found
                        while (tokenPos != std::string::npos)
                        {
                            // Replace the token with the corresponding value
                            line.erase(tokenPos, defineTokens[i].length());
                            line.insert(tokenPos, defineValues[i]);

                            // Search again for the next token
                            tokenPos = line.find(defineTokens[i]);
                        }
                    }
                }

                // What happens now depends on the process
                switch (widgetID)
                {
                    case 0: // Done nothing yet
                    {
                        // Check if this is the first line
                        if (progress.empty())
                        {
                            // The first line should contain 'window' or 'define'
                            if (line.substr(0, 7).compare("window:") == 0)
                            {
                                widgetID = 0;
                                progress.push(1);
                            }
                            else if (line.substr(0, 7).compare("define:") == 0)
                            {
                                line.erase(0, 7);

                                // Search the equals sign
                                std::string::size_type equalsSignPos = line.find('=');
                                if (equalsSignPos != std::string::npos)
                                {
                                    // Store the define
                                    defineTokens.push_back(line.substr(0, equalsSignPos));
                                    defineValues.push_back(line.erase(0, equalsSignPos + 1));
                                }
                                else // The equals sign is missing
                                    failed = true;
                            }
                            else // The second line is wrong
                                failed = true;
                        }
                        else // This is the second line
                        {
                            // The second line should contain "{"
                            if (line.compare("{") == 0)
                            {
                                widgetID = Type_Unknown + 1;
                                progress.pop();
                            }
                            else // The second line is wrong
                                failed = true;
                        }

                        break;
                    }
                    case Type_Unknown + 1: // The window was found
                    {
                        // Find out if the loading is done
                        if (line.compare("}") == 0)
                            break;

                        // The next widget will have the window as its parent
                        parentID.push(Type_Unknown + 1);
                        parentPtr.push(this);

                        // The line doesn't contain a '}', so check what widget it contains
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
                        else // The line was wrong
                            failed = true;

                        break;
                    }
                    case Type_Tab + 1:
                    {
                        START_LOADING_WIDGET

                        // Get the pointer to the tab back
                        Tab::Ptr tab = extraPtr;

                        // Find out what the next property is
                        if (line.substr(0, 11).compare("configfile=") == 0)
                        {
                            // Remove the first part of the line
                            line.erase(0, 11);

                            // The filename must start and end with quotes
                            CHECK_FOR_QUOTES

                            // Load the tab
                            tab->load(line);
                        }
                        else if (line.substr(0, 10).compare("tabheight=") == 0)
                        {
                            // Remove the first part of the line
                            line.erase(0, 10);
                            tab->setTabHeight(std::stoi(line));
                        }
                        else if (line.substr(0, 10).compare("textcolor=") == 0)
                        {
                            tab->setTextColor(extractColor(line.erase(0, 10)));
                        }
                        else if (line.substr(0, 9).compare("textsize=") == 0)
                        {
                            tab->setTextSize(std::stoi(line.erase(0, 9)));
                        }
                        else if (line.substr(0, 15).compare("distancetoside=") == 0)
                        {
                            line.erase(0, 15);
                            tab->setDistanceToSide(std::stoi(line));
                        }
                        else if (line.substr(0, 4).compare("tab=") == 0)
                        {
                            // Remove the first part of the line
                            line.erase(0, 4);

                            // The name must start and end with quotes
                            CHECK_FOR_QUOTES

                            // Add the item to the list box
                            tab->add(line);
                        }
                        else if (line.substr(0, 12).compare("selectedtab=") == 0)
                        {
                            // Remove the first part of the line
                            line.erase(0, 12);

                            // The line must contain something
                            if (!line.empty())
                            {
                                // Check if there are quotes
                                if ((line[0] == '"') && (line[line.length()-1] == '"'))
                                {
                                    line.erase(0, 1);
                                    line.erase(line.length()-1, 1);

                                    // Select the item
                                    tab->select(line);
                                }
                                else // There were no quotes
                                {
                                    // Select the item
                                    tab->select(std::stoi(line));
                                }
                            }
                        }
                        else if (line.substr(0, 9).compare("position=") == 0)
                        {
                            line.erase(0, 9);
                            Vector2f position;
                            if (extractVector2f(line, position))
                                tab->setPosition(position);
                            else
                                failed = true;
                        }
                        else if (line.substr(0, 11).compare("callbackid=") == 0)
                        {
                            line.erase(0, 11);
                            tab->setCallbackId(readInt(line));
                        }
                        else // The line was wrong
                            failed = true;

                        break;
                    }
                    case Type_Grid + 1:
                    {
                        START_LOADING_WIDGET

                        TGUI_OUTPUT("TGUI warning: Grid cannot be loaded from an Widget File yet.");
                    }
                    case Type_Panel + 1:
                    {
                        // Get the pointer to the panel back
                        Panel::Ptr panelPtr = extraPtr;

                        if (progress.top() == 0)
                        {
                            if (line.compare("{") == 0)
                            {
                                progress.pop();
                                progress.push(1);

                                // All newly created widgets must be part of the panel
                                parentID.push(Type_Panel + 1);
                                parentPtr.push(panelPtr.get());
                                break;
                            }
                            else
                                failed = true;
                        }
                        else
                        {
                            if (line.compare("}") == 0)
                            {
                                widgetID = parentID.top();
                                progress.pop();

                                parentID.pop();
                                parentPtr.pop();
                                break;
                            }
                        }

                        CHECK_SHARED_PROPERTIES(panelPtr)
                        else if (line.substr(0, 16).compare("backgroundcolor=") == 0)
                        {
                            // Change the background color (black on error)
                            panelPtr->setBackgroundColor(extractColor(line.erase(0, 16)));
                        }
                        else
                        {
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
                            else // The line was wrong
                                failed = true;
                        }

                        break;
                    }
                    case Type_Label + 1:
                    {
                        START_LOADING_WIDGET

                        // Get the pointer to the label back
                        Label::Ptr label = extraPtr;

                        if (line.substr(0, 5).compare("text=") == 0)
                        {
                            // Remove the first part of the line
                            line.erase(0, 5);

                            // The text must start and end with quotes
                            CHECK_FOR_QUOTES

                            // Change the text
                            label->setText(line);
                        }
                        else if (line.substr(0, 9).compare("textsize=") == 0)
                        {
                            // Change the text size
                            label->setTextSize(std::stoi(line.erase(0, 9)));
                        }
                        else if (line.substr(0, 10).compare("textcolor=") == 0)
                        {
                            // Change the text color (black on error)
                            label->setTextColor(extractColor(line.erase(0, 10)));
                        }
                        else if (line.substr(0, 16).compare("backgroundcolor=") == 0)
                        {
                            // Change the background color (black on error)
                            label->setBackgroundColor(extractColor(line.erase(0, 16)));
                        }
                        else CHECK_SHARED_PROPERTIES(label)
                        else // The line was wrong
                            failed = true;

                        break;
                    }
                    case Type_Button + 1:
                    {
                        START_LOADING_WIDGET

                        // Get the pointer to the button back
                        Button::Ptr button = extraPtr;

                        // Find out what the next property is
                        if (line.substr(0, 11).compare("configfile=") == 0)
                        {
                            // Remove the first part of the line
                            line.erase(0, 11);

                            // The filename must start and end with quotes
                            CHECK_FOR_QUOTES

                            // Load the button
                            button->load(line);
                        }
                        else if (line.substr(0, 5).compare("text=") == 0)
                        {
                            // Remove the first part of the line
                            line.erase(0, 5);

                            // The text must start and end with quotes
                            CHECK_FOR_QUOTES

                            // Change the caption
                            button->setText(line);
                        }
                        else if (line.substr(0, 9).compare("textsize=") == 0)
                        {
                            // Change the text size
                            button->setTextSize(std::stoi(line.erase(0, 9)));
                        }
                        else if (line.substr(0, 10).compare("textcolor=") == 0)
                        {
                            // Change the text color (black on error)
                            button->setTextColor(extractColor(line.erase(0, 10)));
                        }
                        else CHECK_SHARED_PROPERTIES(button)
                        else // The line was wrong
                            failed = true;

                        break;
                    }
                    case Type_Slider + 1:
                    {
                        START_LOADING_WIDGET

                        // Get the pointer to the slider back
                        Slider::Ptr slider = extraPtr;

                        // Find out what the next property is
                        if (line.substr(0, 11).compare("configfile=") == 0)
                        {
                            // Remove the first part of the line
                            line.erase(0, 11);

                            // The filename must start and end with quotes
                            CHECK_FOR_QUOTES

                            // Load the slider
                            slider->load(line);
                        }
                        else if (line.substr(0, 6).compare("value=") == 0)
                        {
                            slider->setValue(std::stoi(line.erase(0, 6)));
                        }
                        else if (line.substr(0, 8).compare("minimum=") == 0)
                        {
                            slider->setMinimum(std::stoi(line.erase(0, 8)));
                        }
                        else if (line.substr(0, 8).compare("maximum=") == 0)
                        {
                            slider->setMaximum(std::stoi(line.erase(0, 8)));
                        }
                        else if (line.substr(0, 15).compare("verticalscroll=") == 0)
                        {
                            // Remove the first part of the line
                            line.erase(0, 15);

                            // Check if the value is true or false
                            bool vericalScroll;
                            CHECK_BOOL(vericalScroll)

                            slider->setVerticalScroll(vericalScroll);
                        }
                        else CHECK_SHARED_PROPERTIES(slider)
                        else // The line was wrong
                            failed = true;

                        break;
                    }
                    case Type_Picture + 1:
                    {
                        START_LOADING_WIDGET

                        // Get the pointer to the picture back
                        Picture::Ptr picture = extraPtr;

                        // Find out what the next property is
                        if (line.substr(0, 9).compare("filename=") == 0)
                        {
                            // Remove the first part of the line
                            line.erase(0, 9);

                            // The filename must start and end with quotes
                            CHECK_FOR_QUOTES

                            // Load the picture
                            picture->load(line);
                        }
                        else CHECK_SHARED_PROPERTIES(picture)
                        else // The line was wrong
                            failed = true;

                        break;
                    }
                    case Type_ListBox + 1:
                    {
                        START_LOADING_WIDGET

                        // Get the pointer to the list box back
                        ListBox::Ptr listBox = extraPtr;

                        // Find out what the next property is
                        if (line.substr(0, 11).compare("configfile=") == 0)
                        {
                            // Remove the first part of the line
                            line.erase(0, 11);

                            // The filename must start and end with quotes
                            CHECK_FOR_QUOTES

                            // Load the list box
                            listBox->load(line);
                        }
                        else if (line.substr(0, 11).compare("itemheight=") == 0)
                        {
                            // Remove the first part of the line
                            line.erase(0, 11);

                            // Set the item height
                            listBox->setItemHeight(std::stoi(line));
                        }
                        else if (line.substr(0, 20).compare("scrollbarconfigfile=") == 0)
                        {
                            // Remove the first part of the line
                            line.erase(0, 20);

                            // The filename must start and end with quotes
                            CHECK_FOR_QUOTES

                            // Load the scrollbar
                            listBox->setScrollbar(line);
                        }
                        else if (line.substr(0, 8).compare("borders=") == 0)
                        {
                            // Remove the first part of the line
                            line.erase(0, 8);

                            // Get the borders
                            Vector4u borders;
                            if (extractVector4u(line, borders))
                                listBox->setBorders(borders.x1, borders.x2, borders.x3, borders.x4);
                            else
                                failed = true;
                        }
                        else if (line.substr(0, 16).compare("backgroundcolor=") == 0)
                        {
                            listBox->setBackgroundColor(extractColor(line.erase(0, 16)));
                        }
                        else if (line.substr(0, 10).compare("textcolor=") == 0)
                        {
                            listBox->setTextColor(extractColor(line.erase(0, 10)));
                        }
                        else if (line.substr(0, 24).compare("selectedbackgroundcolor=") == 0)
                        {
                            listBox->setSelectedBackgroundColor(extractColor(line.erase(0, 24)));
                        }
                        else if (line.substr(0, 18).compare("selectedtextcolor=") == 0)
                        {
                            listBox->setSelectedTextColor(extractColor(line.erase(0, 18)));
                        }
                        else if (line.substr(0, 12).compare("bordercolor=") == 0)
                        {
                            listBox->setBorderColor(extractColor(line.erase(0, 12)));
                        }
                        else if (line.substr(0, 13).compare("maximumitems=") == 0)
                        {
                            // Remove the first part of the line
                            line.erase(0, 13);

                            // Set the maximum items
                            listBox->setMaximumItems(std::stoi(line));
                        }
                        else if (line.substr(0, 5).compare("item=") == 0)
                        {
                            // Remove the first part of the line
                            line.erase(0, 5);

                            // The name must start and end with quotes
                            CHECK_FOR_QUOTES

                            // Add the item to the list box
                            listBox->addItem(line);
                        }
                        else if (line.substr(0, 13).compare("selecteditem=") == 0)
                        {
                            // Remove the first part of the line
                            line.erase(0, 13);

                            // The line must contain something
                            if (!line.empty())
                            {
                                // Check if there are quotes
                                if ((line[0] == '"') && (line[line.length()-1] == '"'))
                                {
                                    line.erase(0, 1);
                                    line.erase(line.length()-1, 1);

                                    // Select the item
                                    listBox->setSelectedItem(line);
                                }
                                else // There were no quotes
                                {
                                    // Select the item
                                    listBox->setSelectedItem(std::stoi(line));
                                }
                            }
                            else
                                failed = true;
                        }
                        else CHECK_SHARED_PROPERTIES(listBox)
                        else // The line was wrong
                            failed = true;

                        break;
                    }
                    case Type_EditBox + 1:
                    {
                        START_LOADING_WIDGET

                        // Get the pointer to the edit box back
                        EditBox::Ptr editBox = extraPtr;

                        // Find out what the next property is
                        if (line.substr(0, 11).compare("configfile=") == 0)
                        {
                            // Remove the first part of the line
                            line.erase(0, 11);

                            // The filename must start and end with quotes
                            CHECK_FOR_QUOTES

                            // Load the edit box
                            editBox->load(line);
                        }
                        else if (line.substr(0, 8).compare("borders=") == 0)
                        {
                            // Remove the first part of the line
                            line.erase(0, 8);

                            // Get the borders
                            Vector4u borders;
                            if (extractVector4u(line, borders))
                                editBox->setBorders(borders.x1, borders.x2, borders.x3, borders.x4);
                            else
                                failed = true;
                        }
                        else if (line.substr(0, 5).compare("text=") == 0)
                        {
                            // Remove the first part of the line
                            line.erase(0, 5);

                            // The text must start and end with quotes
                            CHECK_FOR_QUOTES

                            // Change the text
                            editBox->setText(line);
                        }
                        else if (line.substr(0, 9).compare("textsize=") == 0)
                        {
                            // Change the text size
                            editBox->setTextSize(std::stoi(line.erase(0, 9)));
                        }
                        else if (line.substr(0, 10).compare("textcolor=") == 0)
                        {
                            // Change the text color (black on error)
                            editBox->setTextColor(extractColor(line.erase(0, 10)));
                        }
                        else if (line.substr(0, 18).compare("selectedtextcolor=") == 0)
                        {
                            // Change the selected text color (black on error)
                            editBox->setSelectedTextColor(extractColor(line.erase(0, 18)));
                        }
                        else if (line.substr(0, 28).compare("selectedtextbackgroundcolor=") == 0)
                        {
                            // Change the selected text background color (black on error)
                            editBox->setSelectedTextBackgroundColor(extractColor(line.erase(0, 28)));
                        }
                        else if (line.substr(0, 20).compare("selectionpointcolor=") == 0)
                        {
                            // Change the selection pointer color (black on error)
                            editBox->setSelectionPointColor(extractColor(line.erase(0, 20)));
                        }
                        else if (line.substr(0, 13).compare("passwordchar=") == 0)
                        {
                            // Remove the first part of the line
                            line.erase(0, 13);

                            // The text must start and end with quotes
                            CHECK_FOR_QUOTES

                            // Make sure that the string is not empty
                            if (line.empty() == false)
                            {
                                // Set the password character
                                editBox->setPasswordCharacter(line[0]);
                            }
                            else // The string is empty
                                editBox->setPasswordCharacter('\0');
                        }
                        else if (line.substr(0, 20).compare("selectionpointwidth=") == 0)
                        {
                            // Remove the first part of the line
                            line.erase(0, 20);

                            // Read the selection point width (0 and thus no selection point when it goes wrong)
                            editBox->setSelectionPointWidth(std::stoi(line));
                        }
                        else if (line.substr(0, 18).compare("maximumcharacters=") == 0)
                        {
                            // Remove the first part of the line
                            line.erase(0, 18);

                            // Read the maximum characters (0 and thus no limit when it goes wrong)
                            editBox->setMaximumCharacters(std::stoi(line));
                        }
                        else CHECK_SHARED_PROPERTIES(editBox)
                        else // The line was wrong
                            failed = true;

                        break;
                    }
                    case Type_TextBox + 1:
                    {
                        START_LOADING_WIDGET

                        // Get the pointer to the text box back
                        TextBox::Ptr textBox = extraPtr;

                        // Find out what the next property is
                        if (line.substr(0, 11).compare("configfile=") == 0)
                        {
                            // Remove the first part of the line
                            line.erase(0, 11);

                            // The filename must start and end with quotes
                            CHECK_FOR_QUOTES

                            // Load the text box
                            textBox->load(line);
                        }
                        else if (line.substr(0, 20).compare("scrollbarconfigfile=") == 0)
                        {
                            // Remove the first part of the line
                            line.erase(0, 20);

                            // The filename must start and end with quotes
                            CHECK_FOR_QUOTES

                            // Load the scrollbar
                            textBox->setScrollbar(line);
                        }
                        else if (line.substr(0, 8).compare("borders=") == 0)
                        {
                            // Remove the first part of the line
                            line.erase(0, 8);

                            // Get the borders
                            Vector4u borders;
                            if (extractVector4u(line, borders))
                                textBox->setBorders(borders.x1, borders.x2, borders.x3, borders.x4);
                            else
                                failed = true;
                        }
                        else if (line.substr(0, 5).compare("text=") == 0)
                        {
                            // Remove the first part of the line
                            line.erase(0, 5);

                            // The text must start and end with quotes
                            CHECK_FOR_QUOTES

                            // Change the text
                            textBox->setText(line);
                        }
                        else if (line.substr(0, 9).compare("textsize=") == 0)
                        {
                            // Change the text size
                            textBox->setTextSize(std::stoi(line.erase(0, 9)));
                        }
                        else if (line.substr(0, 16).compare("backgroundcolor=") == 0)
                        {
                            // Change the background color (black on error)
                            textBox->setBackgroundColor(extractColor(line.erase(0, 16)));
                        }
                        else if (line.substr(0, 10).compare("textcolor=") == 0)
                        {
                            // Change the text color (black on error)
                            textBox->setTextColor(extractColor(line.erase(0, 10)));
                        }
                        else if (line.substr(0, 18).compare("selectedtextcolor=") == 0)
                        {
                            // Change the selected text color (black on error)
                            textBox->setSelectedTextColor(extractColor(line.erase(0, 18)));
                        }
                        else if (line.substr(0, 28).compare("selectedtextbackgroundcolor=") == 0)
                        {
                            // Change the selected text background color (black on error)
                            textBox->setSelectedTextBackgroundColor(extractColor(line.erase(0, 28)));
                        }
                        else if (line.substr(0, 12).compare("bordercolor=") == 0)
                        {
                            // Change the border color (black on error)
                            textBox->setBorderColor(extractColor(line.erase(0, 12)));
                        }
                        else if (line.substr(0, 20).compare("selectionpointcolor=") == 0)
                        {
                            // Change the selection pointer color (black on error)
                            textBox->setSelectionPointColor(extractColor(line.erase(0, 20)));
                        }
                        else if (line.substr(0, 20).compare("selectionpointwidth=") == 0)
                        {
                            // Remove the first part of the line
                            line.erase(0, 20);

                            // Read the selection point width (0 and thus no selection point when it goes wrong)
                            textBox->setSelectionPointWidth(std::stoi(line));
                        }
                        else if (line.substr(0, 18).compare("maximumcharacters=") == 0)
                        {
                            // Remove the first part of the line
                            line.erase(0, 18);

                            // Read the maximum characters (0 and thus no limit when it goes wrong)
                            textBox->setMaximumCharacters(std::stoi(line));
                        }
                        else CHECK_SHARED_PROPERTIES(textBox)
                        else // The line was wrong
                            failed = true;

                        break;
                    }
                    case Type_Checkbox + 1:
                    {
                        START_LOADING_WIDGET

                        // Get the pointer to the checkbox back
                        Checkbox::Ptr checkbox = extraPtr;

                        // Find out what the next property is
                        if (line.substr(0, 11).compare("configfile=") == 0)
                        {
                            // Remove the first part of the line
                            line.erase(0, 11);

                            // The filename must start and end with quotes
                            CHECK_FOR_QUOTES

                            // Load the checkbox
                            checkbox->load(line);
                        }
                        else if (line.substr(0, 5).compare("text=") == 0)
                        {
                            // Remove the first part of the line
                            line.erase(0, 5);

                            // The text must start and end with quotes
                            CHECK_FOR_QUOTES

                            // Change the caption
                            checkbox->setText(line);
                        }
                        else if (line.substr(0, 8).compare("checked=") == 0)
                        {
                            // Remove the first part of the string
                            line.erase(0, 8);

                            // Check if the value is true or false
                            bool checked;
                            CHECK_BOOL(checked)

                            // Check or uncheck the checkbox
                            if (checked)
                                checkbox->check();
                            else
                                checkbox->uncheck();
                        }
                        else if (line.substr(0, 9).compare("textsize=") == 0)
                        {
                            // Change the text size
                            checkbox->setTextSize(std::stoi(line.erase(0, 9)));
                        }
                        else if (line.substr(0, 10).compare("textcolor=") == 0)
                        {
                            // Change the text color (black on error)
                            checkbox->setTextColor(extractColor(line.erase(0, 10)));
                        }
                        else CHECK_SHARED_PROPERTIES(checkbox)
                        else // The line was wrong
                            failed = true;

                        break;
                    }
                    case Type_ComboBox + 1:
                    {
                        START_LOADING_WIDGET

                        // Get the pointer to the combo box back
                        ComboBox::Ptr comboBox = extraPtr;

                        // Find out what the next property is
                        if (line.substr(0, 11).compare("configfile=") == 0)
                        {
                            // Remove the first part of the line
                            line.erase(0, 11);

                            // The filename must start and end with quotes
                            CHECK_FOR_QUOTES

                            // Load the combo box
                            comboBox->load(line);
                        }
                        else if (line.substr(0, 20).compare("scrollbarconfigfile=") == 0)
                        {
                            // Remove the first part of the line
                            line.erase(0, 20);

                            // The filename must start and end with quotes
                            CHECK_FOR_QUOTES

                            // Load the scrollbar
                            comboBox->setScrollbar(line);
                        }
                        else if (line.substr(0, 8).compare("borders=") == 0)
                        {
                            // Remove the first part of the line
                            line.erase(0, 8);

                            // Get the borders
                            Vector4u borders;
                            if (extractVector4u(line, borders))
                                comboBox->setBorders(borders.x1, borders.x2, borders.x3, borders.x4);
                            else
                                failed = true;
                        }
                        else if (line.substr(0, 16).compare("backgroundcolor=") == 0)
                        {
                            comboBox->setBackgroundColor(extractColor(line.erase(0, 16)));
                        }
                        else if (line.substr(0, 10).compare("textcolor=") == 0)
                        {
                            comboBox->setTextColor(extractColor(line.erase(0, 10)));
                        }
                        else if (line.substr(0, 24).compare("selectedbackgroundcolor=") == 0)
                        {
                            comboBox->setSelectedBackgroundColor(extractColor(line.erase(0, 24)));
                        }
                        else if (line.substr(0, 18).compare("selectedtextcolor=") == 0)
                        {
                            comboBox->setSelectedTextColor(extractColor(line.erase(0, 18)));
                        }
                        else if (line.substr(0, 12).compare("bordercolor=") == 0)
                        {
                            comboBox->setBorderColor(extractColor(line.erase(0, 12)));
                        }
                        else if (line.substr(0, 15).compare("itemstodisplay=") == 0)
                        {
                            // Remove the first part of the line
                            line.erase(0, 15);

                            // Set the nr of items to display
                            comboBox->setItemsToDisplay(std::stoi(line));
                        }
                        else if (line.substr(0, 5).compare("item=") == 0)
                        {
                            // Remove the first part of the line
                            line.erase(0, 5);

                            // The name must start and end with quotes
                            CHECK_FOR_QUOTES

                            // Add the item to the combo box
                            comboBox->addItem(line);
                        }
                        else if (line.substr(0, 13).compare("selecteditem=") == 0)
                        {
                            // Remove the first part of the line
                            line.erase(0, 13);

                            // The line must contain something
                            if (!line.empty())
                            {
                                // Check if there are quotes
                                if ((line[0] == '"') && (line[line.length()-1] == '"'))
                                {
                                    line.erase(0, 1);
                                    line.erase(line.length()-1, 1);

                                    // Select the item
                                    comboBox->setSelectedItem(line);
                                }
                                else // There were no quotes
                                {
                                    // Select the item
                                    comboBox->setSelectedItem(std::stoi(line));
                                }
                            }
                            else
                                failed = true;
                        }
                        else CHECK_SHARED_PROPERTIES(comboBox)
                        else // The line was wrong
                            failed = true;

                        break;
                    }
                    case Type_Slider2d + 1:
                    {
                        START_LOADING_WIDGET

                        // Get the pointer to the slider back
                        Slider2d::Ptr slider = extraPtr;

                        // Find out what the next property is
                        if (line.substr(0, 11).compare("configfile=") == 0)
                        {
                            // Remove the first part of the line
                            line.erase(0, 11);

                            // The filename must start and end with quotes
                            CHECK_FOR_QUOTES

                            // Load the slider
                            slider->load(line);
                        }
                        else if (line.substr(0, 6).compare("value=") == 0)
                        {
                            Vector2f value;
                            if (extractVector2f(line.erase(0, 6), value))
                                slider->setValue(value);
                            else
                                failed = true;
                        }
                        else if (line.substr(0, 8).compare("minimum=") == 0)
                        {
                            Vector2f minimum;
                            if (extractVector2f(line.erase(0, 8), minimum))
                                slider->setMinimum(minimum);
                            else
                                failed = true;
                        }
                        else if (line.substr(0, 8).compare("maximum=") == 0)
                        {
                            Vector2f maximum;
                            if (extractVector2f(line.erase(0, 8), maximum))
                                slider->setMaximum(maximum);
                            else
                                failed = true;
                        }
                        else if (line.substr(0, 15).compare("returntocenter=") == 0)
                        {
                            // Remove the first part of the line
                            line.erase(0, 15);

                            // Check if the value is true or false
                            bool returnToCenter;
                            CHECK_BOOL(returnToCenter)

                            slider->enableThumbCenter(returnToCenter);
                        }
                        else if (line.substr(0, 15).compare("fixedthumbsize=") == 0)
                        {
                            // Remove the first part of the line
                            line.erase(0, 15);

                            // Check if the value is true or false
                            bool fixedThumbSize;
                            CHECK_BOOL(fixedThumbSize)

                            slider->setFixedThumbSize(fixedThumbSize);
                        }
                        else CHECK_SHARED_PROPERTIES(slider)
                        else // The line was wrong
                            failed = true;

                        break;
                    }
                    case Type_Scrollbar + 1:
                    {
                        START_LOADING_WIDGET

                        // Get the pointer to the scrollbar back
                        Scrollbar::Ptr scrollbar = extraPtr;

                        // Find out what the next property is
                        if (line.substr(0, 11).compare("configfile=") == 0)
                        {
                            // Remove the first part of the line
                            line.erase(0, 11);

                            // The filename must start and end with quotes
                            CHECK_FOR_QUOTES

                            // Load the scrollbar
                            scrollbar->load(line);
                        }
                        else if (line.substr(0, 6).compare("value=") == 0)
                        {
                            scrollbar->setValue(std::stoi(line.erase(0, 6)));
                        }
                        else if (line.substr(0, 8).compare("maximum=") == 0)
                        {
                            scrollbar->setMaximum(std::stoi(line.erase(0, 8)));
                        }
                        else if (line.substr(0, 9).compare("lowvalue=") == 0)
                        {
                            scrollbar->setLowValue(std::stoi(line.erase(0, 9)));
                        }
                        else if (line.substr(0, 15).compare("verticalscroll=") == 0)
                        {
                            // Remove the first part of the line
                            line.erase(0, 15);

                            // Check if the value is true or false
                            bool vericalScroll;
                            CHECK_BOOL(vericalScroll)

                            scrollbar->setVerticalScroll(vericalScroll);
                        }
                        else CHECK_SHARED_PROPERTIES(scrollbar)
                        else // The line was wrong
                            failed = true;

                        break;
                    }
                    case Type_LoadingBar + 1:
                    {
                        START_LOADING_WIDGET

                        // Get the pointer to the loading bar back
                        LoadingBar::Ptr loadingBar = extraPtr;

                        // Find out what the next property is
                        if (line.substr(0, 11).compare("configfile=") == 0)
                        {
                            // Remove the first part of the line
                            line.erase(0, 11);

                            // The filename must start and end with quotes
                            CHECK_FOR_QUOTES

                            // Load the loading bar
                            loadingBar->load(line);
                        }
                        else if (line.substr(0, 6).compare("value=") == 0)
                        {
                            loadingBar->setValue(std::stoi(line.erase(0, 6)));
                        }
                        else if (line.substr(0, 8).compare("minimum=") == 0)
                        {
                            loadingBar->setMinimum(std::stoi(line.erase(0, 8)));
                        }
                        else if (line.substr(0, 8).compare("maximum=") == 0)
                        {
                            loadingBar->setMaximum(std::stoi(line.erase(0, 8)));
                        }
                        else CHECK_SHARED_PROPERTIES(loadingBar)
                        else // The line was wrong
                            failed = true;

                        break;
                    }
                    case Type_SpinButton + 1:
                    {
                        START_LOADING_WIDGET

                        // Get the pointer to the spin button back
                        SpinButton::Ptr spinButton = extraPtr;

                        // Find out what the next property is
                        if (line.substr(0, 11).compare("configfile=") == 0)
                        {
                            // Remove the first part of the line
                            line.erase(0, 11);

                            // The filename must start and end with quotes
                            CHECK_FOR_QUOTES

                            // Load the spin button
                            spinButton->load(line);
                        }
                        else if (line.substr(0, 6).compare("value=") == 0)
                        {
                            spinButton->setValue(std::stoi(line.erase(0, 6)));
                        }
                        else if (line.substr(0, 8).compare("minimum=") == 0)
                        {
                            spinButton->setMinimum(std::stoi(line.erase(0, 8)));
                        }
                        else if (line.substr(0, 8).compare("maximum=") == 0)
                        {
                            spinButton->setMaximum(std::stoi(line.erase(0, 8)));
                        }
                        else if (line.substr(0, 15).compare("verticalscroll=") == 0)
                        {
                            // Remove the first part of the line
                            line.erase(0, 15);

                            // Check if the value is true or false
                            bool vericalScroll;
                            CHECK_BOOL(vericalScroll)

                            spinButton->setVerticalScroll(vericalScroll);
                        }
                        else CHECK_SHARED_PROPERTIES(spinButton)
                        else // The line was wrong
                            failed = true;

                        break;
                    }
                    case Type_RadioButton + 1:
                    {
                        START_LOADING_WIDGET

                        // Get the pointer to the radio button back
                        RadioButton::Ptr radioButton = extraPtr;

                        // Find out what the next property is
                        if (line.substr(0, 11).compare("configfile=") == 0)
                        {
                            // Remove the first part of the line
                            line.erase(0, 11);

                            // The filename must start and end with quotes
                            CHECK_FOR_QUOTES

                            // Load the radio button
                            radioButton->load(line);
                        }
                        else if (line.substr(0, 5).compare("text=") == 0)
                        {
                            // Remove the first part of the line
                            line.erase(0, 5);

                            // The text must start and end with quotes
                            CHECK_FOR_QUOTES

                            // Change the caption
                            radioButton->setText(line);
                        }
                        else if (line.substr(0, 8).compare("checked=") == 0)
                        {
                            // Remove the first part of the string
                            line.erase(0, 8);

                            // Check if the value is true or false
                            bool checked;
                            CHECK_BOOL(checked)

                            // Check or uncheck the radio button
                            if (checked)
                                radioButton->check();
                            else
                                radioButton->uncheck();
                        }
                        else if (line.substr(0, 9).compare("textsize=") == 0)
                        {
                            // Change the text size
                            radioButton->setTextSize(std::stoi(line.erase(0, 9)));
                        }
                        else if (line.substr(0, 10).compare("textcolor=") == 0)
                        {
                            // Change the text color (black on error)
                            radioButton->setTextColor(extractColor(line.erase(0, 10)));
                        }
                        else CHECK_SHARED_PROPERTIES(radioButton)
                        else // The line was wrong
                            failed = true;

                        break;
                    }
                    case Type_ChildWindow + 1:
                    {
                        // Get the pointer to the child window back
                        ChildWindow::Ptr child = extraPtr;

                        if (progress.top() == 0)
                        {
                            if (line.compare("{") == 0)
                            {
                                progress.pop();
                                progress.push(1);

                                // All newly created widgets must be part of the panel
                                parentID.push(Type_ChildWindow + 1);
                                parentPtr.push(child.get());
                                break;
                            }
                            else
                                failed = true;
                        }
                        else
                        {
                            if (line.compare("}") == 0)
                            {
                                widgetID = parentID.top();
                                progress.pop();

                                parentID.pop();
                                parentPtr.pop();
                                break;
                            }
                        }

                        CHECK_SHARED_PROPERTIES(child)
                        else if (line.substr(0, 11).compare("configfile=") == 0)
                        {
                            // Remove the first part of the line
                            line.erase(0, 11);

                            // The filename must start and end with quotes
                            CHECK_FOR_QUOTES

                            // Load the child window
                            if (child->load(line) == false)
                                failed = true;
                        }
                        else if (line.substr(0, 16).compare("backgroundcolor=") == 0)
                        {
                            // Change the background color (black on error)
                            child->setBackgroundColor(extractColor(line.erase(0, 16)));
                        }
                        else if (line.substr(0, 8).compare("borders=") == 0)
                        {
                            // Remove the first part of the line
                            line.erase(0, 8);

                            // Get the borders
                            Vector4u borders;
                            if (extractVector4u(line, borders))
                                child->setBorders(borders.x1, borders.x2, borders.x3, borders.x4);
                            else
                                failed = true;
                        }
                        else if (line.substr(0, 13).compare("transparency=") == 0)
                        {
                            child->setTransparency(static_cast<unsigned char>(std::stoi(line.erase(0, 13))));
                        }
                        else if (line.substr(0, 15).compare("titlebarheight=") == 0)
                        {
                            child->setTitleBarHeight(std::stoi(line.erase(0, 15)));
                        }
                        else
                        {
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
                            else // The line was wrong
                                failed = true;
                        }

                        break;
                    }
                    case Type_SpriteSheet + 1:
                    {
                        START_LOADING_WIDGET

                        // Get the pointer to the sprite sheet back
                        SpriteSheet::Ptr spriteSheet = extraPtr;

                        // Find out what the next property is
                        if (line.substr(0, 9).compare("filename=") == 0)
                        {
                            // Remove the first part of the line
                            line.erase(0, 9);

                            // The filename must start and end with quotes
                            CHECK_FOR_QUOTES

                            // Load the sprite sheet
                            spriteSheet->load(line);
                        }
                        else if (line.substr(0, 5).compare("rows=") == 0)
                        {
                            spriteSheet->setRows(std::stoi(line.erase(0, 5)));
                        }
                        else if (line.substr(0, 8).compare("columns=") == 0)
                        {
                            spriteSheet->setColumns(std::stoi(line.erase(0, 8)));
                        }
                        else if (line.substr(0, 6).compare("cells=") == 0)
                        {
                            Vector2u cells;
                            if (extractVector2u(line.erase(0, 6), cells))
                                spriteSheet->setCells(cells.x, cells.y);
                            else
                                failed = true;
                        }
                        else if (line.substr(0, 12).compare("visiblecell=") == 0)
                        {
                            Vector2u cell;
                            if (extractVector2u(line.erase(0, 12), cell))
                                spriteSheet->setVisibleCell(cell.x, cell.y);
                            else
                                failed = true;
                        }
                        else CHECK_SHARED_PROPERTIES(spriteSheet)
                        else // The line was wrong
                            failed = true;

                        break;
                    }
                    case Type_AnimatedPicture + 1:
                    {
                        START_LOADING_WIDGET

                        // Get the pointer to the sprite animated picture
                        AnimatedPicture::Ptr animatedPicture = extraPtr;

                        // Find out what the next property is
                        if (line.substr(0, 6).compare("frame=") == 0)
                        {
                            line.erase(0, 6);

                            // Make sure that the string isn't empty
                            if (line.empty() == false)
                            {
                                // The first and last character have to be brackets
                                if ((line[0] == '(') && (line[line.length()-1] == ')'))
                                {
                                    // Remove the brackets
                                    line.erase(0, 1);
                                    line.erase(line.length()-1);

                                    // Search for the first comma
                                    std::string::size_type commaPos = line.find(',');
                                    if (commaPos != std::string::npos)
                                    {
                                        // Get the frame duration
                                        int duration = std::stoi(line.substr(commaPos+1));
                                        line.erase(commaPos);

                                        // The filename must start and end with quotes
                                        CHECK_FOR_QUOTES

                                        // Add the frame to the animated picture
                                        if (animatedPicture->addFrame(line, sf::milliseconds(duration)) == false)
                                            failed = true;
                                    }
                                    else
                                        failed = true;
                                }
                                else
                                    failed = true;
                            }
                            else
                                failed = true;
                        }
                        else if (line.substr(0, 5).compare("loop=") == 0)
                        {
                            // Remove the first part of the string
                            line.erase(0, 5);

                            // Check if the value is true or false
                            bool loop;
                            CHECK_BOOL(loop)

                            animatedPicture->setLooping(loop);
                        }
                        else if (line.substr(0, 8).compare("playing=") == 0)
                        {
                            // Remove the first part of the string
                            line.erase(0, 8);

                            // Check if the value is true or false
                            bool playing;
                            CHECK_BOOL(playing)

                            if (playing)
                                animatedPicture->play();
                        }
                        else if (line.substr(0, 13).compare("currentframe=") == 0)
                        {
                            animatedPicture->setFrame(static_cast<unsigned int>(std::stoi(line.erase(0, 13))));
                        }
                        else CHECK_SHARED_PROPERTIES(animatedPicture)
                        else // The line was wrong
                            failed = true;

                        break;
                    }
                };
            }
        }

        m_File.close();

        if (failed)
            return false;

        return true;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Container::add(const Widget::Ptr& widgetPtr, const sf::String& widgetName)
    {
        assert(widgetPtr != nullptr);

        widgetPtr->initialize(this);
        m_EventManager.m_Widgets.push_back(widgetPtr);
        m_ObjName.push_back(widgetName);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Widget::Ptr Container::get(const sf::String& widgetName) const
    {
        for (unsigned int i = 0; i < m_ObjName.size(); ++i)
        {
            if (m_ObjName[i] == widgetName)
                return m_EventManager.m_Widgets[i];
        }

        return Widget::Ptr();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Widget::Ptr Container::copy(const Widget::Ptr& oldWidget, const sf::String& newWidgetName)
    {
        Widget::Ptr newWidget = oldWidget.clone();
        add(newWidget, newWidgetName);
        return newWidget;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::vector<Widget::Ptr>& Container::getWidgets()
    {
        return m_EventManager.m_Widgets;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::vector<sf::String>& Container::getWidgetNames()
    {
        return m_ObjName;
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
        for (unsigned int i = 0; i < m_EventManager.m_Widgets.size(); ++i)
        {
            // Check if the pointer matches
            if (m_EventManager.m_Widgets[i].get() == widget)
            {
                // Unfocus the widget, just in case it was focused
                m_EventManager.unfocusWidget(widget);

                // Remove the widget
                m_EventManager.m_Widgets.erase(m_EventManager.m_Widgets.begin() + i);

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
        m_EventManager.m_Widgets.clear();
        m_ObjName.clear();

        // There are no more widgets, so none of the widgets can be focused
        m_EventManager.m_FocusedWidget = 0;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Container::uncheckRadioButtons()
    {
        // Loop through all radio buttons and uncheck them
        for (unsigned int i = 0; i < m_EventManager.m_Widgets.size(); ++i)
        {
            if (m_EventManager.m_Widgets[i]->m_Callback.widgetType == Type_RadioButton)
                static_cast<RadioButton::Ptr>(m_EventManager.m_Widgets[i])->forceUncheck();
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Container::focusWidget(Widget *const widget)
    {
        m_EventManager.focusWidget(widget);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Container::unfocusWidget(Widget *const widget)
    {
        m_EventManager.unfocusWidget(widget);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Container::unfocusAllWidgets()
    {
        m_EventManager.unfocusAllWidgets();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Container::moveWidgetToFront(Widget *const widget)
    {
        // Loop through all widgets
        for (unsigned int i = 0; i < m_EventManager.m_Widgets.size(); ++i)
        {
            // Check if the widget is found
            if (m_EventManager.m_Widgets[i].get() == widget)
            {
                // Copy the widget
                m_EventManager.m_Widgets.push_back(m_EventManager.m_Widgets[i]);
                m_ObjName.push_back(m_ObjName[i]);

                // Focus the correct widget
                if ((m_EventManager.m_FocusedWidget == 0) || (m_EventManager.m_FocusedWidget == i+1))
                    m_EventManager.m_FocusedWidget = m_EventManager.m_Widgets.size()-1;
                else if (m_EventManager.m_FocusedWidget > i+1)
                    --m_EventManager.m_FocusedWidget;

                // Remove the old widget
                m_EventManager.m_Widgets.erase(m_EventManager.m_Widgets.begin() + i);
                m_ObjName.erase(m_ObjName.begin() + i);

                break;
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Container::moveWidgetToBack(Widget *const widget)
    {
        // Loop through all widgets
        for (unsigned int i = 0; i < m_EventManager.m_Widgets.size(); ++i)
        {
            // Check if the widget is found
            if (m_EventManager.m_Widgets[i].get() == widget)
            {
                // Copy the widget
                Widget::Ptr obj = m_EventManager.m_Widgets[i];
                std::string name = m_ObjName[i];
                m_EventManager.m_Widgets.insert(m_EventManager.m_Widgets.begin(), obj);
                m_ObjName.insert(m_ObjName.begin(), name);

                // Focus the correct widget
                if (m_EventManager.m_FocusedWidget == i + 1)
                    m_EventManager.m_FocusedWidget = 1;
                else if (m_EventManager.m_FocusedWidget)
                    ++m_EventManager.m_FocusedWidget;

                // Remove the old widget
                m_EventManager.m_Widgets.erase(m_EventManager.m_Widgets.begin() + i + 1);
                m_ObjName.erase(m_ObjName.begin() + i + 1);

                break;
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Container::bindGlobalCallback(std::function<void(const tgui::Callback&)> func)
    {
        m_GlobalCallbackFunctions.push_back(func);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Container::unbindGlobalCallback()
    {
        m_GlobalCallbackFunctions.clear();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Container::drawWidgetContainer(sf::RenderTarget* target, const sf::RenderStates& states) const
    {
        // Draw all widgets when they are visible
        for (unsigned int i = 0; i < m_EventManager.m_Widgets.size(); ++i)
        {
            if (m_EventManager.m_Widgets[i]->m_Visible)
                m_EventManager.m_Widgets[i]->draw(*target, states);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
