/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus' Graphical User Interface
// Copyright (C) 2012-2022 Bruno Van de Velde (vdv_b@tgui.eu)
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
#include <TGUI/GuiBase.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    static std::pair<String, String> parseMinMaxExpresssion(const String& expression)
    {
        unsigned int bracketCount = 0;
        auto commaOrBracketPos = expression.find_first_of(",()");
        while (commaOrBracketPos != String::npos)
        {
            if (expression[commaOrBracketPos] == '(')
                bracketCount++;
            else if (expression[commaOrBracketPos] == ')')
            {
                if (bracketCount == 0)
                    break;

                bracketCount--;
            }
            else // if (expression[commaOrBracketPos] == ',')
            {
                if (bracketCount == 0)
                    return {expression.substr(0, commaOrBracketPos), expression.substr(commaOrBracketPos + 1)};
            }

            commaOrBracketPos = expression.find_first_of(",()", commaOrBracketPos + 1);
        }

        TGUI_PRINT_WARNING("bracket mismatch while parsing min or max in layout string '" << expression << "'.");
        return {};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Layout::Layout(String expression)
    {
        // Empty strings have value 0 (although this might indicate a mistake in the expression, it is valid for unary minus,
        // where "-x" is parsed as "" - "x").
        expression = expression.trim();
        if (expression.empty())
            return;

        auto searchPos = expression.find_first_of("+-/*()");

        // Extract the value from the string when there are no more operators
        if (searchPos == String::npos)
        {
            // Convert percentages to references to the parent widget
            if (expression.back() == '%')
            {
                // We don't know if we have to bind the width or height, so bind "size" and let the connectWidget function figure it out later
                if (expression == "100%")
                {
                    m_boundString = "&.innersize";
                    m_operation = Operation::BindingString;
                }
                else // value is a fraction of parent size
                {
                    *this = Layout{Layout::Operation::Multiplies,
                                   std::make_unique<Layout>(expression.substr(0, expression.length()-1).toFloat() / 100.f),
                                   std::make_unique<Layout>("&.innersize")};
                }
            }
            else
            {
                std::size_t startIndex = 0;
                auto lastDotIndex = expression.rfind('.');
                if (lastDotIndex != String::npos)
                    startIndex = lastDotIndex + 1;

                // partAfterDot can be entire string if there is no dot, e.g. if layout is just "width" (widget refering to self)
                const String& partAfterDot = expression.substr(startIndex);

                // The expression might reference to a widget instead of being a constant
                if ((partAfterDot == U"x") || (partAfterDot == U"left")
                 || (partAfterDot == U"y") || (partAfterDot == U"top")
                 || (partAfterDot == U"w") || (partAfterDot == U"width")
                 || (partAfterDot == U"h") || (partAfterDot == U"height")
                 || (partAfterDot == U"iw") || (partAfterDot == U"innerwidth") // width inside the container
                 || (partAfterDot == U"ih") || (partAfterDot == U"innerheight") // height inside the container
                 || (partAfterDot == U"pos") || (partAfterDot == U"position")
                 || (partAfterDot == U"size") || (partAfterDot == U"innersize"))
                {
                    // We can't search for the referenced widget yet as no widget is connected to the widget yet, so store the string for future parsing
                    m_boundString = expression;
                    m_operation = Operation::BindingString;
                }
                else if (partAfterDot == U"right")
                {
                    *this = Layout{Operation::Plus,
                                   std::make_unique<Layout>(expression.substr(0, expression.size()-5) + U"left"),
                                   std::make_unique<Layout>(expression.substr(0, expression.size()-5) + U"width")};
                }
                else if (partAfterDot == U"bottom")
                {
                    *this = Layout{Operation::Plus,
                                   std::make_unique<Layout>(expression.substr(0, expression.size()-6) + U"top"),
                                   std::make_unique<Layout>(expression.substr(0, expression.size()-6) + U"height")};
                }
                else // Constant value
                {
                    if (!expression.attemptToFloat(m_value))
                    {
                        TGUI_PRINT_WARNING("invalid value while parsing layout string '" << expression << "'.");
                    }
                }
            }

            return;
        }

        // The string contains an expression, so split it up in multiple layouts
        std::list<Layout> operands;
        std::vector<Operation> operators;
        decltype(searchPos) prevSearchPos = 0;
        while (searchPos != String::npos)
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
                while (bracketPos != String::npos)
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
                        else if ((searchPos == 3) && (bracketPos == expression.size()-1) && (expression.substr(0, 3) == "min"))
                        {
                            const auto& minSubExpressions = parseMinMaxExpresssion(expression.substr(4, expression.size() - 5));
                            *this = Layout{Operation::Minimum, std::make_unique<Layout>(minSubExpressions.first), std::make_unique<Layout>(minSubExpressions.second)};
                            return;
                        }
                        else if ((searchPos == 3) && (bracketPos == expression.size()-1) && (expression.substr(0, 3) == "max"))
                        {
                            const auto& maxSubExpressions = parseMinMaxExpresssion(expression.substr(4, expression.size() - 5));
                            *this = Layout{Operation::Maximum, std::make_unique<Layout>(maxSubExpressions.first), std::make_unique<Layout>(maxSubExpressions.second)};
                            return;
                        }
                        else // The brackets form a sub-expression
                            searchPos = bracketPos;

                        break;
                    }

                    bracketPos = expression.find_first_of("()", bracketPos + 1);
                }

                if (bracketPos == String::npos)
                {
                    TGUI_PRINT_WARNING("bracket mismatch while parsing layout string '" << expression << "'.");
                    return;
                }
                else
                {
                    // Search for the next operator, starting from the closing bracket, but keeping prevSearchPos before the opening bracket
                    searchPos = expression.find_first_of("+-/*()", searchPos + 1);
                    continue;
                }
            }
            case ')':
                TGUI_PRINT_WARNING("bracket mismatch while parsing layout string '" << expression << "'.");
                return;
            };

            prevSearchPos = searchPos + 1;
            searchPos = expression.find_first_of("+-/*()", searchPos + 1);
        }

        if (prevSearchPos == 0)
        {
            // We would get an infinite loop if we don't abort in this condition
            TGUI_PRINT_WARNING("error in expression '" << expression << "'.");
            return;
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
                TGUI_ASSERT(operandIt != operands.end(), "First operand not found in plus or minus operation");

                auto nextOperandIt = operandIt;
                std::advance(nextOperandIt, 1);

                TGUI_ASSERT(nextOperandIt != operands.end(), "Second operand not found in plus or minus operation");

                // Handle unary plus or minus
                if ((operandIt->m_operation == Operation::Value) && (nextOperandIt->m_operation == Operation::Value) && (operandIt->m_value == 0))
                {
                    if (operators[i] == Operation::Minus)
                        nextOperandIt->m_value = -nextOperandIt->m_value;

                    *operandIt = *nextOperandIt;
                }
                else // Normal addition or subtraction
                {
                    *operandIt = Layout{operators[i],
                                        std::make_unique<Layout>(*operandIt),
                                        std::make_unique<Layout>(*nextOperandIt)};
                }

                operands.erase(nextOperandIt);
            }
        }

        TGUI_ASSERT(operands.size() == 1, "Layout constructor should reduce expression to single result");
        *this = operands.front();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Layout::Layout(Operation operation, Widget* boundWidget) :
        m_operation  {operation},
        m_boundWidget{boundWidget}
    {
        TGUI_ASSERT(m_boundWidget != nullptr, "Bound widget passed to Layout constructor can't be a nullptr");
        TGUI_ASSERT((m_operation == Operation::BindingPosX) || (m_operation == Operation::BindingPosY)
               || (m_operation == Operation::BindingLeft) || (m_operation == Operation::BindingTop)
               || (m_operation == Operation::BindingWidth) || (m_operation == Operation::BindingHeight)
               || (m_operation == Operation::BindingInnerWidth) || (m_operation == Operation::BindingInnerHeight),
            "Layout constructor with bound widget must be called with an operation that involves the widget"
        );

        if (m_operation == Operation::BindingPosX)
            m_value = m_boundWidget->getPosition().x;
        else if (m_operation == Operation::BindingPosY)
            m_value = m_boundWidget->getPosition().y;
        else if (m_operation == Operation::BindingLeft)
            m_value = m_boundWidget->getPosition().x - (m_boundWidget->getOrigin().x * m_boundWidget->getSize().x);
        else if (m_operation == Operation::BindingTop)
            m_value = m_boundWidget->getPosition().y - (m_boundWidget->getOrigin().y * m_boundWidget->getSize().y);
        else if (m_operation == Operation::BindingWidth)
            m_value = m_boundWidget->getSize().x;
        else if (m_operation == Operation::BindingHeight)
            m_value = m_boundWidget->getSize().y;
        else if (m_operation == Operation::BindingInnerWidth)
        {
            const auto* boundContainer = dynamic_cast<Container*>(boundWidget);
            if (boundContainer)
                m_value = boundContainer->getInnerSize().x;
        }
        else if (m_operation == Operation::BindingInnerHeight)
        {
            const auto* boundContainer = dynamic_cast<Container*>(boundWidget);
            if (boundContainer)
                m_value = boundContainer->getInnerSize().y;
        }

        resetPointers();
        recalculateValue();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Layout::Layout(Operation operation, std::unique_ptr<Layout> leftOperand, std::unique_ptr<Layout> rightOperand) :
        m_operation   {operation},
        m_leftOperand {std::move(leftOperand)},
        m_rightOperand{std::move(rightOperand)}
    {
        TGUI_ASSERT(m_leftOperand != nullptr, "Left operand in layout constructor can't be a nullptr");
        TGUI_ASSERT(m_rightOperand != nullptr, "Right operand in layout constructor can't be a nullptr");

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

    String Layout::toString() const
    {
        if (m_operation == Operation::Value)
        {
            return String::fromNumber(m_value);
        }
        else if (m_operation == Operation::Minimum)
        {
            return U"min(" + m_leftOperand->toString() + U", " + m_rightOperand->toString() + U")";
        }
        else if (m_operation == Operation::Maximum)
        {
            return U"max(" + m_leftOperand->toString() + U", " + m_rightOperand->toString() + U")";
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

            auto subExpressionNeedsBrackets = [](const std::unique_ptr<Layout>& operand)
                {
                    if (!operand->m_leftOperand)
                        return false;

                    if ((operand->m_operation == Operation::Minimum) || (operand->m_operation == Operation::Maximum))
                        return false;

                    if ((operand->m_operation == Operation::Multiplies) && (operand->m_leftOperand->m_operation == Operation::Value) && (operand->m_rightOperand->toString() == U"100%"))
                        return false;

                    return true;
                };

            if (subExpressionNeedsBrackets(m_leftOperand) && subExpressionNeedsBrackets(m_rightOperand))
                return U"(" + m_leftOperand->toString() + U") " + operatorChar + U" (" + m_rightOperand->toString() + U")";
            else if (subExpressionNeedsBrackets(m_leftOperand))
                return U"(" + m_leftOperand->toString() + U") " + operatorChar + U" " + m_rightOperand->toString();
            else if (subExpressionNeedsBrackets(m_rightOperand))
                return m_leftOperand->toString() + U" " + operatorChar + U" (" + m_rightOperand->toString() + U")";
            else
            {
                if ((m_operation == Operation::Multiplies) && (m_leftOperand->m_operation == Operation::Value) && (m_rightOperand->toString() == U"100%"))
                    return String::fromNumber(m_leftOperand->getValue() * 100) + '%';
                else
                    return m_leftOperand->toString() + U" " + operatorChar + U" " + m_rightOperand->toString();
            }
        }
        else
        {
            if (m_boundString == "&.innersize")
                return "100%";

            // Hopefully the expression is stored in the bound string, otherwise (i.e. when bind functions were used) it is infeasible to turn it into a string
            if (!m_boundString.empty())
                return m_boundString;
            else
            {
                if ((m_operation == Operation::BindingPosX) || (m_operation == Operation::BindingPosY)
                 || (m_operation == Operation::BindingLeft) || (m_operation == Operation::BindingTop)
                 || (m_operation == Operation::BindingWidth) || (m_operation == Operation::BindingHeight)
                 || (m_operation == Operation::BindingInnerWidth) || (m_operation == Operation::BindingInnerHeight))
                {
                    TGUI_ASSERT(m_boundWidget != nullptr, "There has to be a bound widget with this operation type");
                    const String boundWidgetName = m_boundWidget->getWidgetName();
                    if (boundWidgetName.empty())
                    {
                        TGUI_PRINT_WARNING("layout can't be correctly converted to string because the bound widget has no name");
                    }

                    if (m_operation == Operation::BindingPosX)
                        return boundWidgetName + U".x";
                    else if (m_operation == Operation::BindingPosY)
                        return boundWidgetName + U".y";
                    else if (m_operation == Operation::BindingLeft)
                        return boundWidgetName + U".left";
                    else if (m_operation == Operation::BindingTop)
                        return boundWidgetName + U".top";
                    else if (m_operation == Operation::BindingWidth)
                        return boundWidgetName + U".width";
                    else if (m_operation == Operation::BindingHeight)
                        return boundWidgetName + U".height";
                    else if (m_operation == Operation::BindingInnerWidth)
                        return boundWidgetName + U".innerwidth";
                    else
                    {
                        TGUI_ASSERT(m_operation == Operation::BindingInnerHeight, "Operation can't be anything other than BindingInnerHeight here");
                        return boundWidgetName + U".innerheight";
                    }
                }

                // The layout contains an empty string
                TGUI_ASSERT(m_operation == Operation::BindingString, "All operation types should have been covered already");
                return m_boundString;
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Layout::unbindLayout()
    {
        if (m_boundWidget)
        {
            TGUI_ASSERT((m_operation == Operation::BindingPosX) || (m_operation == Operation::BindingPosY)
                   || (m_operation == Operation::BindingLeft) || (m_operation == Operation::BindingTop)
                   || (m_operation == Operation::BindingWidth) || (m_operation == Operation::BindingHeight)
                   || (m_operation == Operation::BindingInnerWidth) || (m_operation == Operation::BindingInnerHeight),
                "Layout with bound widget should have an operation that involves the widget"
            );

            if ((m_operation == Operation::BindingPosX) || (m_operation == Operation::BindingPosY) || (m_operation == Operation::BindingLeft) || (m_operation == Operation::BindingTop))
                m_boundWidget->unbindPositionLayout(this);
            else
                m_boundWidget->unbindSizeLayout(this);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Layout::resetPointers()
    {
        if (m_leftOperand != nullptr)
        {
            TGUI_ASSERT(m_rightOperand != nullptr, "If a left operand exists then there should also be a right operand");

            m_leftOperand->m_parent = this;
            m_rightOperand->m_parent = this;
        }

        if (m_boundWidget)
        {
            TGUI_ASSERT((m_operation == Operation::BindingPosX) || (m_operation == Operation::BindingPosY)
                   || (m_operation == Operation::BindingLeft) || (m_operation == Operation::BindingTop)
                   || (m_operation == Operation::BindingWidth) || (m_operation == Operation::BindingHeight)
                   || (m_operation == Operation::BindingInnerWidth) || (m_operation == Operation::BindingInnerHeight),
                "Layout with bound widget should have an operation that involves the widget"
            );

            if ((m_operation == Operation::BindingPosX) || (m_operation == Operation::BindingPosY) || (m_operation == Operation::BindingLeft) || (m_operation == Operation::BindingTop))
                m_boundWidget->bindPositionLayout(this);
            else
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
            case Operation::Minimum:
                m_value = std::min(m_leftOperand->getValue(), m_rightOperand->getValue());
                break;
            case Operation::Maximum:
                m_value = std::max(m_leftOperand->getValue(), m_rightOperand->getValue());
                break;
            case Operation::BindingPosX:
                m_value = m_boundWidget->getPosition().x;
                break;
            case Operation::BindingPosY:
                m_value = m_boundWidget->getPosition().y;
                break;
            case Operation::BindingLeft:
                m_value = m_boundWidget->getPosition().x - (m_boundWidget->getOrigin().x * m_boundWidget->getSize().x);
                break;
            case Operation::BindingTop:
                m_value = m_boundWidget->getPosition().y - (m_boundWidget->getOrigin().y * m_boundWidget->getSize().y);
                break;
            case Operation::BindingWidth:
                m_value = m_boundWidget->getSize().x;
                break;
            case Operation::BindingHeight:
                m_value = m_boundWidget->getSize().y;
                break;
            case Operation::BindingInnerWidth:
            {
                const auto* boundContainer = dynamic_cast<Container*>(m_boundWidget);
                if (boundContainer)
                    m_value = boundContainer->getInnerSize().x;
                break;
            }
            case Operation::BindingInnerHeight:
            {
                const auto* boundContainer = dynamic_cast<Container*>(m_boundWidget);
                if (boundContainer)
                    m_value = boundContainer->getInnerSize().y;
                break;
            }
            case Operation::BindingString:
                // The string should have already been parsed by now.
                // Passing here either means something is wrong with the string or the layout was not connected to a widget with a parent yet.
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
            TGUI_ASSERT(m_rightOperand != nullptr, "If a left operand exists then there should also be a right operand");

            m_leftOperand->parseBindingStringRecursive(widget, xAxis);
            m_rightOperand->parseBindingStringRecursive(widget, xAxis);
        }

        // Parse the string binding even when the referred widget was already found. The widget may be added to a different parent
        if (!m_boundString.empty())
            parseBindingString(m_boundString, widget, xAxis);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Layout::parseBindingString(const String& expression, Widget* widget, bool xAxis)
    {
        if (expression == "x")
        {
            m_operation = Operation::BindingPosX;
            m_boundWidget = widget;
        }
        else if (expression == "y")
        {
            m_operation = Operation::BindingPosY;
            m_boundWidget = widget;
        }
        else if (expression == "left")
        {
            m_operation = Operation::BindingLeft;
            m_boundWidget = widget;
        }
        else if (expression == "top")
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
        else if (expression == "iw" || expression == "innerwidth")
        {
            m_operation = Operation::BindingInnerWidth;
            m_boundWidget = widget;
        }
        else if (expression == "ih" || expression == "innerheight")
        {
            m_operation = Operation::BindingInnerHeight;
            m_boundWidget = widget;
        }
        else if (expression == "size")
        {
            if (xAxis)
                return parseBindingString("width", widget, xAxis);
            else
                return parseBindingString("height", widget, xAxis);
        }
        else if (expression == "innersize")
        {
            if (xAxis)
                return parseBindingString("innerwidth", widget, xAxis);
            else
                return parseBindingString("innerheight", widget, xAxis);
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
            if (dotPos != String::npos)
            {
                const String widgetName = expression.substr(0, dotPos);
                if (widgetName == "parent" || widgetName == "&")
                {
                    if (widget->getParent())
                        return parseBindingString(expression.substr(dotPos+1), widget->getParent(), xAxis);

                    // If there is no parent yet then patiently wait until the widget is added to its parent
                    return;
                }
                else if (!widgetName.empty())
                {
                    // If the widget is a container, search in its children first
                    Container* container = dynamic_cast<Container*>(widget);
                    if (container != nullptr)
                    {
                        const auto& widgets = container->getWidgets();
                        for (std::size_t i = 0; i < widgets.size(); ++i)
                        {
                            if (widgets[i]->getWidgetName().equalIgnoreCase(widgetName))
                                return parseBindingString(expression.substr(dotPos+1), widgets[i].get(), xAxis);
                        }
                    }

                    // If the widget has a parent, look for a sibling
                    if (widget->getParent())
                    {
                        const auto& widgets = widget->getParent()->getWidgets();
                        for (std::size_t i = 0; i < widgets.size(); ++i)
                        {
                            if (widgets[i]->getWidgetName().equalIgnoreCase(widgetName))
                                return parseBindingString(expression.substr(dotPos+1), widgets[i].get(), xAxis);
                        }
                    }
                    else // If there is no parent yet then patiently wait until the widget is added to its parent
                        return;
                }
            }

            // The referred widget was not found or there was something wrong with the string
            TGUI_PRINT_WARNING("failed to find bound widget in expression '" << expression << "'.");
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

        Layout bindPosX(Widget::Ptr widget)
        {
            return Layout{Layout::Operation::BindingPosX, widget.get()};
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        Layout bindPosY(Widget::Ptr widget)
        {
            return Layout{Layout::Operation::BindingPosY, widget.get()};
        }

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

        Layout bindInnerWidth(Container::Ptr container)
        {
            return Layout{Layout::Operation::BindingInnerWidth, container.get()};
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        Layout bindInnerHeight(Container::Ptr container)
        {
            return Layout{Layout::Operation::BindingInnerHeight, container.get()};
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
            return {bindPosX(widget), bindPosY(widget)};
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        Layout2d bindSize(Widget::Ptr widget)
        {
            return {bindWidth(widget), bindHeight(widget)};
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        Layout2d bindInnerSize(Container::Ptr container)
        {
            return {bindInnerWidth(container), bindInnerHeight(container)};
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        Layout bindWidth(GuiBase& gui)
        {
            return bindWidth(gui.getContainer());
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        Layout bindHeight(GuiBase& gui)
        {
            return bindHeight(gui.getContainer());
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        Layout2d bindSize(GuiBase& gui)
        {
            return bindSize(gui.getContainer());
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        Layout bindMin(const Layout& value1, const Layout& value2)
        {
            return Layout{Layout::Operation::Minimum, std::make_unique<Layout>(value1), std::make_unique<Layout>(value2)};
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        Layout bindMax(const Layout& value1, const Layout& value2)
        {
            return Layout{Layout::Operation::Maximum, std::make_unique<Layout>(value1), std::make_unique<Layout>(value2)};
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
