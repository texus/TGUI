/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus' Graphical User Interface
// Copyright (C) 2012-2018 Bruno Van de Velde (vdv_b@tgui.eu)
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


#include <TGUI/Layout.hpp>
#include <TGUI/Widget.hpp>
#include <TGUI/Gui.hpp>
#include <TGUI/to_string.hpp>
#include <SFML/System/Err.hpp>
#include <cassert>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Layout::Layout(std::string expression)
    {
        // Empty strings have value 0 (although this might indicate a mistake in the expression, it is valid for unary minus)
        expression = trim(expression);
        if (expression.empty())
            return;

        auto searchPos = expression.find_first_of("+-*/()");

        // Extract the value from the string when there are no more operators
        if (searchPos == std::string::npos)
        {
            // Convert percentages to references to the parent widget
            if (expression.back() == '%')
            {
                // We don't know if we have to bind the width or height, so bind "size" and let the connectWidget function figure it out later
                if (expression == "100%")
                {
                    m_boundString = "&.size";
                    m_operation = Operation::BindingString;
                }
                else // value is a fraction of parent size
                {
                    *this = Layout{Layout::Operation::Multiplies,
                                   std::make_unique<Layout>(tgui::stof(expression.substr(0, expression.length()-1)) / 100.f),
                                   std::make_unique<Layout>("&.size")};
                }
            }
            else
            {
                // The expression might reference to a widget instead of being a constant
                expression = toLower(expression);
                if ((expression.substr(expression.size()-1) == "x")
                 || (expression.substr(expression.size()-1) == "y")
                 || (expression.substr(expression.size()-1) == "w") // width
                 || (expression.substr(expression.size()-1) == "h") // height
                 || (expression.size() >= 4 && expression.substr(expression.size()-4) == "left")
                 || (expression.size() >= 3 && expression.substr(expression.size()-3) == "top")
                 || (expression.size() >= 5 && expression.substr(expression.size()-5) == "width")
                 || (expression.size() >= 6 && expression.substr(expression.size()-6) == "height")
                 || (expression.size() >= 4 && expression.substr(expression.size()-4) == "size")
                 || (expression.size() >= 3 && expression.substr(expression.size()-3) == "pos")
                 || (expression.size() >= 8 && expression.substr(expression.size()-8) == "position"))
                {
                    // We can't search for the referenced widget yet as no widget is connected to the widget yet, so store the string for future parsing
                    m_boundString = expression;
                    m_operation = Operation::BindingString;
                }
                else if (expression.size() >= 5 && expression.substr(expression.size()-5) == "right")
                {
                    *this = Layout{Operation::Plus,
                                   std::make_unique<Layout>(expression.substr(0, expression.size()-5) + "left"),
                                   std::make_unique<Layout>(expression.substr(0, expression.size()-5) + "width")};
                }
                else if (expression.size() >= 6 && expression.substr(expression.size()-6) == "bottom")
                {
                    *this = Layout{Operation::Plus,
                                   std::make_unique<Layout>(expression.substr(0, expression.size()-6) + "top"),
                                   std::make_unique<Layout>(expression.substr(0, expression.size()-6) + "height")};
                }
                else // Constant value
                    m_value = tgui::stof(expression);
            }

            return;
        }

        // The string contains an expression, so split it up in multiple layouts
        std::list<Layout> operands;
        std::vector<Operation> operators;
        decltype(searchPos) prevSearchPos = 0;
        while (searchPos != std::string::npos)
        {
            switch (expression[searchPos])
            {
            case '+':
                operators.push_back(Operation::Plus);
                operands.emplace_back(expression.substr(prevSearchPos, searchPos - prevSearchPos));
                break;
            case '-':
                operators.push_back(Operation::Minus);
                operands.emplace_back(expression.substr(prevSearchPos, searchPos - prevSearchPos));
                break;
            case '*':
                operators.push_back(Operation::Multiplies);
                operands.emplace_back(expression.substr(prevSearchPos, searchPos - prevSearchPos));
                break;
            case '/':
                operators.push_back(Operation::Divides);
                operands.emplace_back(expression.substr(prevSearchPos, searchPos - prevSearchPos));
                break;
            case '(':
            {
                // Find corresponding closing bracket
                unsigned int bracketCount = 0;
                auto bracketPos = expression.find_first_of("()", searchPos + 1);
                while (bracketPos != std::string::npos)
                {
                    if (expression[bracketPos] == '(')
                        bracketCount++;
                    else if (bracketCount > 0)
                        bracketCount--;
                    else
                    {
                        // If the entire layout was in brackets then remove these brackets
                        if ((searchPos == 0) && (bracketPos == expression.size()-1))
                        {
                            *this = Layout{expression.substr(1, expression.size()-2)};
                            return;
                        }
                        else // The brackets form a sub-expression
                            searchPos = bracketPos;

                        break;
                    }

                    bracketPos = expression.find_first_of("()", bracketPos + 1);
                }

                if (bracketPos == std::string::npos)
                {
                    TGUI_PRINT_WARNING("bracket mismatch while parsing layout string '" << expression << "'.");
                    return;
                }
                else
                {
                    // Search for the next operator, starting from the closing bracket, but keeping prevSearchPos before the opening bracket
                    searchPos = expression.find_first_of("+-*/()", searchPos + 1);
                    continue;
                }
            }
            case ')':
                TGUI_PRINT_WARNING("bracket mismatch while parsing layout string '" << expression << "'.");
                return;
            };

            prevSearchPos = searchPos + 1;
            searchPos = expression.find_first_of("+-*/()", searchPos + 1);
        }

        operands.emplace_back(expression.substr(prevSearchPos));

        // First perform all * and / operations
        auto operandIt = operands.begin();
        for (std::size_t i = 0; i < operators.size(); ++i)
        {
            if ((operators[i] == Operation::Multiplies) || (operators[i] == Operation::Divides))
            {
                auto nextOperandIt = operandIt;
                std::advance(nextOperandIt, 1);

                (*operandIt) = Layout{operators[i],
                                      std::make_unique<Layout>(*operandIt),
                                      std::make_unique<Layout>(*nextOperandIt)};

                operands.erase(nextOperandIt);
            }
            else
                ++operandIt;
        }

        // Now perform all + and - operations
        operandIt = operands.begin();
        for (std::size_t i = 0; i < operators.size(); ++i)
        {
            if ((operators[i] == Operation::Plus) || (operators[i] == Operation::Minus))
            {
                assert(operandIt != operands.end());

                auto nextOperandIt = operandIt;
                std::advance(nextOperandIt, 1);

                assert(nextOperandIt != operands.end());

                (*operandIt) = Layout{operators[i],
                                      std::make_unique<Layout>(*operandIt),
                                      std::make_unique<Layout>(*nextOperandIt)};

                operands.erase(nextOperandIt);
            }
        }

        assert(operands.size() == 1);
        *this = operands.front();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Layout::Layout(Operation operation, Widget* boundWidget) :
        m_operation  {operation},
        m_boundWidget{boundWidget}
    {
        assert((m_operation == Operation::BindingLeft) || (m_operation == Operation::BindingTop) || (m_operation == Operation::BindingWidth) || (m_operation == Operation::BindingHeight));
        assert(m_boundWidget != nullptr);

        if (m_operation == Operation::BindingLeft)
            m_value = m_boundWidget->getPosition().x;
        else if (m_operation == Operation::BindingTop)
            m_value = m_boundWidget->getPosition().y;
        else if (m_operation == Operation::BindingWidth)
            m_value = m_boundWidget->getSize().x;
        else if (m_operation == Operation::BindingHeight)
            m_value = m_boundWidget->getSize().y;

        resetPointers();
        recalculateValue();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Layout::Layout(Operation operation, std::unique_ptr<Layout> leftOperand, std::unique_ptr<Layout> rightOperand) :
        m_operation   {operation},
        m_leftOperand {std::move(leftOperand)},
        m_rightOperand{std::move(rightOperand)}
    {
        assert(m_leftOperand != nullptr);
        assert(m_rightOperand != nullptr);
        resetPointers();
        recalculateValue();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Layout::Layout(const Layout& other) :
        m_value          {other.m_value},
        m_parent         {other.m_parent},
        m_operation      {other.m_operation},
        m_leftOperand    {other.m_leftOperand ? std::make_unique<Layout>(*other.m_leftOperand) : nullptr},
        m_rightOperand   {other.m_rightOperand ? std::make_unique<Layout>(*other.m_rightOperand) : nullptr},
        m_boundWidget    {other.m_boundWidget},
        m_boundString    {other.m_boundString}
    {
        // Disconnect the bound widget if a string was used, the same name may apply to a different widget now
        if (!m_boundString.empty())
            m_boundWidget = nullptr;

        resetPointers();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Layout::Layout(Layout&& other) :
        m_value          {std::move(other.m_value)},
        m_parent         {std::move(other.m_parent)},
        m_operation      {other.m_operation},
        m_leftOperand    {std::move(other.m_leftOperand)},
        m_rightOperand   {std::move(other.m_rightOperand)},
        m_boundWidget    {other.m_boundWidget},
        m_boundString    {std::move(other.m_boundString)}
    {
        resetPointers();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Layout& Layout::operator=(const Layout& other)
    {
        if (this != &other)
        {
            unbindLayout();

            m_value           = other.m_value;
            m_parent          = other.m_parent;
            m_operation       = other.m_operation;
            m_leftOperand     = other.m_leftOperand ? std::make_unique<Layout>(*other.m_leftOperand) : nullptr;
            m_rightOperand    = other.m_rightOperand ? std::make_unique<Layout>(*other.m_rightOperand) : nullptr;
            m_boundWidget     = other.m_boundWidget;
            m_boundString     = other.m_boundString;

            // Disconnect the bound widget if a string was used, the same name may apply to a different widget now
            if (!m_boundString.empty())
                m_boundWidget = nullptr;

            resetPointers();
        }

        return *this;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Layout& Layout::operator=(Layout&& other)
    {
        if (this != &other)
        {
            unbindLayout();

            m_value           = std::move(other.m_value);
            m_parent          = std::move(other.m_parent);
            m_operation       = other.m_operation;
            m_leftOperand     = std::move(other.m_leftOperand);
            m_rightOperand    = std::move(other.m_rightOperand);
            m_boundWidget     = other.m_boundWidget;
            m_boundString     = std::move(other.m_boundString);

            resetPointers();
        }

        return *this;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Layout::~Layout()
    {
        unbindLayout();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::string Layout::toString() const
    {
        if (m_operation == Operation::Value)
        {
            return to_string(m_value);
        }
        else if ((m_operation == Operation::Plus) || (m_operation == Operation::Minus) || (m_operation == Operation::Multiplies) || (m_operation == Operation::Divides))
        {
            char operatorChar;
            if (m_operation == Operation::Plus)
                operatorChar = '+';
            else if (m_operation == Operation::Minus)
                operatorChar = '-';
            else if (m_operation == Operation::Multiplies)
                operatorChar = '*';
            else // if (m_operation == Operation::Divides)
                operatorChar = '/';

            if ((m_leftOperand->m_leftOperand) && (m_rightOperand->m_leftOperand))
                return "(" + m_leftOperand->toString() + ") " + operatorChar + " (" + m_rightOperand->toString() + ")";
            else if (m_leftOperand->m_leftOperand)
                return "(" + m_leftOperand->toString() + ") " + operatorChar + " " + m_rightOperand->toString();
            else if (m_rightOperand->m_leftOperand)
                return m_leftOperand->toString() + " " + operatorChar + " (" + m_rightOperand->toString() + ")";
            else
                return m_leftOperand->toString() + " " + operatorChar + " " + m_rightOperand->toString();
        }
        else
        {
            // Hopefully the expression is stored in the bound string, otherwise (i.e. when bind functions were used) it is infeasible to turn it into a string
            return m_boundString;
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Layout::unbindLayout()
    {
        if (m_boundWidget)
        {
            assert((m_operation == Operation::BindingLeft) || (m_operation == Operation::BindingTop) || (m_operation == Operation::BindingWidth) || (m_operation == Operation::BindingHeight));

            if ((m_operation == Operation::BindingLeft) || (m_operation == Operation::BindingTop))
                m_boundWidget->unbindPositionLayout(this);
            else // if ((m_operation == Operation::BindingWidth) || (m_operation == Operation::BindingHeight))
                m_boundWidget->unbindSizeLayout(this);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Layout::resetPointers()
    {
        if (m_leftOperand != nullptr)
        {
            assert(m_rightOperand != nullptr);

            m_leftOperand->m_parent = this;
            m_rightOperand->m_parent = this;
        }

        if (m_boundWidget)
        {
            assert((m_operation == Operation::BindingLeft) || (m_operation == Operation::BindingTop) || (m_operation == Operation::BindingWidth) || (m_operation == Operation::BindingHeight));

            if ((m_operation == Operation::BindingLeft) || (m_operation == Operation::BindingTop))
                m_boundWidget->bindPositionLayout(this);
            else // if ((m_operation == Operation::BindingWidth) || (m_operation == Operation::BindingHeight))
                m_boundWidget->bindSizeLayout(this);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Layout::connectWidget(Widget* widget, bool xAxis, std::function<void()> valueChangedCallbackHandler)
    {
        const float oldValue = m_value;

        // No callbacks must be made while parsing, a single callback will be made when done if needed
        m_connectedWidgetCallback = nullptr;

        parseBindingStringRecursive(widget, xAxis);

        // Restore the callback function
        m_connectedWidgetCallback = valueChangedCallbackHandler;

        if (m_value != oldValue)
        {
            if (m_connectedWidgetCallback)
                m_connectedWidgetCallback();
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Layout::unbindWidget()
    {
        m_boundWidget = nullptr;

        if (!m_boundString.empty())
            m_operation = Operation::BindingString;
        else
        {
            m_value = 0;
            m_operation = Operation::Value;
        }

        recalculateValue();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Layout::recalculateValue()
    {
        const float oldValue = m_value;

        switch (m_operation)
        {
            case Operation::Value:
                break;
            case Operation::Plus:
                m_value = m_leftOperand->getValue() + m_rightOperand->getValue();
                break;
            case Operation::Minus:
                m_value = m_leftOperand->getValue() - m_rightOperand->getValue();
                break;
            case Operation::Multiplies:
                m_value = m_leftOperand->getValue() * m_rightOperand->getValue();
                break;
            case Operation::Divides:
                if (m_rightOperand->getValue() != 0)
                    m_value = m_leftOperand->getValue() / m_rightOperand->getValue();
                else
                    m_value = 0;
                break;
            case Operation::BindingLeft:
                m_value = m_boundWidget->getPosition().x;
                break;
            case Operation::BindingTop:
                m_value = m_boundWidget->getPosition().y;
                break;
            case Operation::BindingWidth:
                m_value = m_boundWidget->getSize().x;
                break;
            case Operation::BindingHeight:
                m_value = m_boundWidget->getSize().y;
                break;
            case Operation::BindingString:
                // Passing here either means something is wrong with the string or the layout was not connected to a widget with a parent yet
                break;
        };

        if (m_value != oldValue)
        {
            if (m_parent)
                m_parent->recalculateValue();
            else
            {
                // The topmost layout must tell the connected widget about the new value
                if (m_connectedWidgetCallback)
                    m_connectedWidgetCallback();
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Layout::parseBindingStringRecursive(Widget* widget, bool xAxis)
    {
        if (m_leftOperand)
        {
            assert(m_rightOperand != nullptr);

            m_leftOperand->parseBindingStringRecursive(widget, xAxis);
            m_rightOperand->parseBindingStringRecursive(widget, xAxis);
        }

        // Parse the string binding even when the referred widget was already found. The widget may be added to a different parent
        if (!m_boundString.empty())
            parseBindingString(m_boundString, widget, xAxis);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Layout::parseBindingString(const std::string& expression, Widget* widget, bool xAxis)
    {
        if (expression == "x" || expression == "left")
        {
            m_operation = Operation::BindingLeft;
            m_boundWidget = widget;
        }
        else if (expression == "y" || expression == "top")
        {
            m_operation = Operation::BindingTop;
            m_boundWidget = widget;
        }
        else if (expression == "w" || expression == "width")
        {
            m_operation = Operation::BindingWidth;
            m_boundWidget = widget;
        }
        else if (expression == "h" || expression == "height")
        {
            m_operation = Operation::BindingHeight;
            m_boundWidget = widget;
        }
        else if (expression == "size")
        {
            if (xAxis)
                return parseBindingString("width", widget, xAxis);
            else
                return parseBindingString("height", widget, xAxis);
        }
        else if ((expression == "pos") || (expression == "position"))
        {
            if (xAxis)
                return parseBindingString("x", widget, xAxis);
            else
                return parseBindingString("y", widget, xAxis);
        }
        else
        {
            const auto dotPos = expression.find('.');
            if (dotPos != std::string::npos)
            {
                const std::string widgetName = expression.substr(0, dotPos);
                if (widgetName == "parent" || widgetName == "&")
                {
                    if (widget->getParent())
                        return parseBindingString(expression.substr(dotPos+1), widget->getParent(), xAxis);
                }
                else if (!widgetName.empty())
                {
                    // If the widget is a container, search in its children first
                    Container* container = dynamic_cast<Container*>(widget);
                    if (container != nullptr)
                    {
                        const auto& widgets = container->getWidgets();
                        const auto& widgetNames = container->getWidgetNames();
                        for (std::size_t i = 0; i < widgets.size(); ++i)
                        {
                            if (toLower(widgetNames[i]) == widgetName)
                                return parseBindingString(expression.substr(dotPos+1), widgets[i].get(), xAxis);
                        }
                    }

                    // If the widget has a parent, look for a sibling
                    if (widget->getParent())
                    {
                        const auto& widgets = widget->getParent()->getWidgets();
                        const auto& widgetNames = widget->getParent()->getWidgetNames();
                        for (std::size_t i = 0; i < widgets.size(); ++i)
                        {
                            if (toLower(widgetNames[i]) == widgetName)
                                return parseBindingString(expression.substr(dotPos+1), widgets[i].get(), xAxis);
                        }
                    }
                }
            }

            // The referred widget was not found or there was something wrong with the string
            return;
        }

        resetPointers();
        recalculateValue();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Layout operator-(Layout right)
    {
        return Layout{Layout::Operation::Minus, std::make_unique<Layout>(), std::make_unique<Layout>(std::move(right))};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Layout operator+(Layout left, Layout right)
    {
        return Layout{Layout::Operation::Plus, std::make_unique<Layout>(std::move(left)), std::make_unique<Layout>(std::move(right))};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Layout operator-(Layout left, Layout right)
    {
        return Layout{Layout::Operation::Minus, std::make_unique<Layout>(std::move(left)), std::make_unique<Layout>(std::move(right))};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Layout operator*(Layout left, Layout right)
    {
        return Layout{Layout::Operation::Multiplies, std::make_unique<Layout>(std::move(left)), std::make_unique<Layout>(std::move(right))};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Layout operator/(Layout left, Layout right)
    {
        return Layout{Layout::Operation::Divides, std::make_unique<Layout>(std::move(left)), std::make_unique<Layout>(std::move(right))};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Layout2d operator-(Layout2d right)
    {
        return Layout2d{-std::move(right.x), -std::move(right.y)};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Layout2d operator+(Layout2d left, Layout2d right)
    {
        return Layout2d{std::move(left.x) + std::move(right.x), std::move(left.y) + std::move(right.y)};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Layout2d operator-(Layout2d left, Layout2d right)
    {
        return Layout2d{std::move(left.x) - std::move(right.x), std::move(left.y) - std::move(right.y)};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Layout2d operator*(Layout2d left, const Layout& right)
    {
        return Layout2d{std::move(left.x) * right, std::move(left.y) * right};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Layout2d operator*(const Layout& left, Layout2d right)
    {
        return Layout2d{left * std::move(right.x), left * std::move(right.y)};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Layout2d operator/(Layout2d left, const Layout& right)
    {
        return Layout2d{std::move(left.x) / right, std::move(left.y) / right};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    inline namespace bind_functions
    {
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        Layout bindLeft(Widget::Ptr widget)
        {
            return Layout{Layout::Operation::BindingLeft, widget.get()};
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        Layout bindTop(Widget::Ptr widget)
        {
            return Layout{Layout::Operation::BindingTop, widget.get()};
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        Layout bindWidth(Widget::Ptr widget)
        {
            return Layout{Layout::Operation::BindingWidth, widget.get()};
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        Layout bindHeight(Widget::Ptr widget)
        {
            return Layout{Layout::Operation::BindingHeight, widget.get()};
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        Layout bindRight(Widget::Ptr widget)
        {
            return Layout{Layout::Operation::Plus,
                          std::make_unique<Layout>(Layout::Operation::BindingLeft, widget.get()),
                          std::make_unique<Layout>(Layout::Operation::BindingWidth, widget.get())};
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        Layout bindBottom(Widget::Ptr widget)
        {
            return Layout{Layout::Operation::Plus,
                          std::make_unique<Layout>(Layout::Operation::BindingTop, widget.get()),
                          std::make_unique<Layout>(Layout::Operation::BindingHeight, widget.get())};
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        Layout2d bindPosition(Widget::Ptr widget)
        {
            return {bindLeft(widget), bindTop(widget)};
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        Layout2d bindSize(Widget::Ptr widget)
        {
            return {bindWidth(widget), bindHeight(widget)};
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        Layout bindWidth(Gui& gui)
        {
            return bindWidth(gui.getContainer());
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        Layout bindHeight(Gui& gui)
        {
            return bindHeight(gui.getContainer());
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        Layout2d bindSize(Gui& gui)
        {
            return bindSize(gui.getContainer());
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
