/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus's Graphical User Interface
// Copyright (C) 2012-2017 Bruno Van de Velde (vdv_b@tgui.eu)
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


#include <TGUI/Widget.hpp>
#include <TGUI/Gui.hpp>

#include <cassert>
#include <cctype>
#include <cmath>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    float getWidgetLeft(tgui::Widget* widget)
    {
        return widget->getPosition().x;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    float getWidgetTop(tgui::Widget* widget)
    {
        return widget->getPosition().y;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    float getWidgetWidth(tgui::Widget* widget)
    {
        return widget->getSize().x;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    float getWidgetHeight(tgui::Widget* widget)
    {
        return widget->getSize().y;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    float getWidgetRight(tgui::Widget* widget)
    {
        return widget->getPosition().x + widget->getSize().x;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    float getWidgetBottom(tgui::Widget* widget)
    {
        return widget->getPosition().y + widget->getSize().y;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void recalculateLayout(tgui::LayoutImpl* layout)
    {
        // Only the roots should be told to update as they will update their children with them
        if (layout->parents.empty())
        {
            layout->recalculate();
        }
        else
        {
            for (auto& parent : layout->parents)
                recalculateLayout(parent);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void resetLayout(std::shared_ptr<tgui::LayoutImpl> layout, float value)
    {
        layout->value = value;
        recalculateLayout(layout.get());

        for (auto& attachedLayout : layout->attachedLayouts)
            attachedLayout->update();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    tgui::Layout layoutOperator(tgui::Layout&& left, tgui::Layout&& right, tgui::LayoutImpl::Operation operation)
    {
        tgui::Layout result;
        result.getImpl()->operation = operation;
        result.getImpl()->operands.push_back(left.getImpl());
        result.getImpl()->operands.push_back(right.getImpl());
        left.getImpl()->parents.insert(result.getImpl().get());
        right.getImpl()->parents.insert(result.getImpl().get());
        result.getImpl()->recalculate();
        return result;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    LayoutImpl::~LayoutImpl()
    {
        for (auto& operand : operands)
            operand->parents.erase(this);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void LayoutImpl::recalculate()
    {
        for (auto& operand : operands)
            operand->recalculate();

        switch (operation)
        {
        case Operation::Value:
            break;
        case Operation::String:
            value = parseLayoutString(stringExpression);
            break;
        case Operation::Plus:
            value = operands[0]->value + operands[1]->value;
            break;
        case Operation::Minus:
            value = operands[0]->value - operands[1]->value;
            break;
        case Operation::Multiplies:
            value = operands[0]->value * operands[1]->value;
            break;
        case Operation::Divides:
            value = operands[0]->value / operands[1]->value;
            break;
        case Operation::Modulus:
            value = std::fmod(operands[0]->value, operands[1]->value);
            break;
        case Operation::And:
            value = operands[0]->value && operands[1]->value;
            break;
        case Operation::Or:
            value = operands[0]->value || operands[1]->value;
            break;
        case Operation::LessThan:
            value = operands[0]->value < operands[1]->value;
            break;
        case Operation::LessOrEqual:
            value = operands[0]->value <= operands[1]->value;
            break;
        case Operation::GreaterThan:
            value = operands[0]->value > operands[1]->value;
            break;
        case Operation::GreaterOrEqual:
            value = operands[0]->value >= operands[1]->value;
            break;
        case Operation::Equal:
            value = operands[0]->value == operands[1]->value;
            break;
        case Operation::NotEqual:
            value = operands[0]->value != operands[1]->value;
            break;
        case Operation::Minimum:
            value = std::min(operands[0]->value, operands[1]->value);
            break;
        case Operation::Maximum:
            value = std::max(operands[0]->value, operands[1]->value);
            break;
        case Operation::Conditional:
            value = operands[0]->value ? operands[1]->value : operands[2]->value;
            break;
        }

        // Alert the widgets that are using this layout
        for (auto& attachedLayout : attachedLayouts)
            attachedLayout->update();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    float LayoutImpl::parseLayoutString(std::string expression)
    {
        // Empty strings have value 0 (although this might indicate a mistake in the expression, it also happens on unary plus and minus)
        expression = tgui::trim(expression);
        if (expression.empty())
            return 0;

        // First calculate expressions withing brackets
        auto openBracketPos = expression.rfind('(');
        while (openBracketPos != std::string::npos)
        {
            auto closeBracketPos = expression.find(')', openBracketPos + 1);
            if (closeBracketPos == std::string::npos)
                return 0; // Opening bracket without matching closing bracket

            std::string newExpression;
            if ((openBracketPos >= 3) && (expression.substr(openBracketPos - 3, 3) == "max"))
            {
                newExpression += expression.substr(0, openBracketPos - 3);

                auto commaPos = expression.find(',', openBracketPos);
                if (commaPos < closeBracketPos)
                {
                    auto prevPos = openBracketPos;
                    float maxNum = -std::numeric_limits<float>::infinity();
                    while (commaPos < closeBracketPos)
                    {
                        maxNum = std::max(maxNum, parseLayoutString(expression.substr(prevPos + 1, commaPos - prevPos - 1)));

                        prevPos = commaPos;
                        commaPos = expression.find(',', prevPos + 1);
                    }

                    maxNum = std::max(maxNum, parseLayoutString(expression.substr(prevPos + 1, closeBracketPos - prevPos - 1)));
                    newExpression += tgui::to_string(maxNum);
                }
                else
                    newExpression += tgui::to_string(parseLayoutString(expression.substr(openBracketPos + 1, closeBracketPos - openBracketPos - 1)));
            }
            else if ((openBracketPos >= 3) && (expression.substr(openBracketPos - 3, 3) == "min"))
            {
                newExpression += expression.substr(0, openBracketPos - 3);

                auto commaPos = expression.find(',', openBracketPos);
                if (commaPos < closeBracketPos)
                {
                    auto prevPos = openBracketPos;
                    float maxNum = std::numeric_limits<float>::infinity();
                    while (commaPos < closeBracketPos)
                    {
                        maxNum = std::min(maxNum, parseLayoutString(expression.substr(prevPos + 1, commaPos - prevPos - 1)));

                        prevPos = commaPos;
                        commaPos = expression.find(',', prevPos + 1);
                    }

                    maxNum = std::min(maxNum, parseLayoutString(expression.substr(prevPos + 1, closeBracketPos - prevPos - 1)));
                    newExpression += tgui::to_string(maxNum);
                }
                else
                    newExpression += tgui::to_string(parseLayoutString(expression.substr(openBracketPos + 1, closeBracketPos - openBracketPos - 1)));
            }
            else if ((openBracketPos >= 5) && (expression.substr(openBracketPos - 5, 5) == "range"))
            {
                newExpression += expression.substr(0, openBracketPos - 5);

                auto firstCommaPos = expression.find(',', openBracketPos);
                if (firstCommaPos < closeBracketPos)
                {
                    auto secondCommaPos = expression.find(',', firstCommaPos + 1);
                    if (secondCommaPos < closeBracketPos)
                    {
                        auto thirdCommaPos = expression.find(',', secondCommaPos + 1);
                        if (thirdCommaPos > closeBracketPos)
                        {
                            float minValue = parseLayoutString(expression.substr(openBracketPos + 1, firstCommaPos - openBracketPos - 1));
                            float maxValue = parseLayoutString(expression.substr(firstCommaPos + 1, secondCommaPos - firstCommaPos - 1));
                            float wantedValue = parseLayoutString(expression.substr(secondCommaPos + 1, closeBracketPos - secondCommaPos - 1));
                            newExpression += tgui::to_string(std::max(std::min(wantedValue, maxValue), minValue));
                        }
                        else // There shouldn't be a third comma
                            newExpression += "0";
                    }
                    else // Second comma missing
                        newExpression += "0";
                }
                else // First comma missing
                    newExpression += "0";
            }
            else // Normal set of brackets
            {
                newExpression += expression.substr(0, openBracketPos);
                newExpression += tgui::to_string(parseLayoutString(expression.substr(openBracketPos + 1, closeBracketPos - openBracketPos - 1)));
            }
            newExpression += expression.substr(closeBracketPos + 1);

            expression = newExpression;
            openBracketPos = expression.rfind('(');
        }

        // Recursively solve the if-then-else statements and the use of the ?: operator
        auto ifPos = expression.find("if");
        auto questionMarkPos = expression.find('?');

        while ((ifPos != std::string::npos) && (ifPos > 0) && !::isspace(expression[ifPos-1]))
            ifPos = expression.find("if", ifPos + 2);
        while ((ifPos < expression.size()-2) && !::isspace(expression[ifPos+2]))
            ifPos = expression.find("if", ifPos + 2);

        if ((ifPos != std::string::npos) || (questionMarkPos != std::string::npos))
        {
            if ((ifPos == std::string::npos) || (questionMarkPos < ifPos))
            {
                auto matchingColonPos = expression.find(':', questionMarkPos + 1);
                auto nextQuestionMarkPos = expression.find('?', questionMarkPos + 1);
                while ((matchingColonPos != std::string::npos) && (nextQuestionMarkPos < matchingColonPos))
                {
                    matchingColonPos = expression.find(':', matchingColonPos + 1);
                    nextQuestionMarkPos = expression.find('?', matchingColonPos + 1);
                }

                if (matchingColonPos == std::string::npos)
                    return 0; // '?' without matching ':'

                if (parseLayoutString(expression.substr(0, questionMarkPos)) != 0)
                    return parseLayoutString(expression.substr(questionMarkPos + 1, matchingColonPos - questionMarkPos - 1));
                else
                    return parseLayoutString(expression.substr(matchingColonPos + 1));
            }
            else // if-then-else instead of ?:
            {
                auto thenPos = expression.find("then", ifPos + 2);
                auto nextifPos = expression.find("if", ifPos + 2);

                while ((nextifPos != std::string::npos) && (nextifPos > 0) && !::isspace(expression[nextifPos-1]))
                    nextifPos = expression.find("if", nextifPos + 2);
                while ((nextifPos < expression.size()-2) && !::isspace(expression[nextifPos+2]))
                    nextifPos = expression.find("if", nextifPos + 2);

                while ((thenPos != std::string::npos) && (thenPos > 0) && !::isspace(expression[thenPos-1]))
                    thenPos = expression.find("then", thenPos + 4);
                while ((thenPos < expression.size()-4) && !::isspace(expression[thenPos+4]))
                    thenPos = expression.find("then", thenPos + 4);

                while ((thenPos != std::string::npos) && (nextifPos < thenPos))
                {
                    nextifPos = expression.find("if", thenPos + 4);
                    while ((nextifPos != std::string::npos) && (nextifPos > 0) && !::isspace(expression[nextifPos-1]))
                        nextifPos = expression.find("if", nextifPos + 2);
                    while ((nextifPos < expression.size()-2) && !::isspace(expression[nextifPos+2]))
                        nextifPos = expression.find("if", nextifPos + 2);

                    thenPos = expression.find("then", thenPos + 4);
                    while ((thenPos != std::string::npos) && (thenPos > 0) && !::isspace(expression[thenPos-1]))
                        thenPos = expression.find("then", thenPos + 4);
                    while ((thenPos < expression.size()-4) && !::isspace(expression[thenPos+4]))
                        thenPos = expression.find("then", thenPos + 4);
                }

                if (thenPos == std::string::npos)
                    return 0; // 'if' without matching 'then'

                auto elsePos = expression.find("else", thenPos + 4);
                nextifPos = expression.find("if", thenPos + 4);

                while ((nextifPos != std::string::npos) && (nextifPos > 0) && !::isspace(expression[nextifPos-1]))
                    nextifPos = expression.find("if", nextifPos + 2);
                while ((nextifPos < expression.size()-2) && !::isspace(expression[nextifPos+2]))
                    nextifPos = expression.find("if", nextifPos + 2);

                while ((elsePos != std::string::npos) && (elsePos > 0) && !::isspace(expression[elsePos-1]))
                    elsePos = expression.find("else", elsePos + 4);
                while ((elsePos < expression.size()-4) && !::isspace(expression[elsePos+4]))
                    elsePos = expression.find("else", elsePos + 4);

                while ((elsePos != std::string::npos) && (nextifPos < elsePos))
                {
                    nextifPos = expression.find("if", elsePos + 4);
                    while ((nextifPos != std::string::npos) && (nextifPos > 0) && !::isspace(expression[nextifPos-1]))
                        nextifPos = expression.find("if", nextifPos + 2);
                    while ((nextifPos < expression.size()-2) && !::isspace(expression[nextifPos+2]))
                        nextifPos = expression.find("if", nextifPos + 2);

                    elsePos = expression.find("else", elsePos + 4);
                    while ((elsePos != std::string::npos) && (elsePos > 0) && !::isspace(expression[elsePos-1]))
                        elsePos = expression.find("else", elsePos + 4);
                    while ((elsePos < expression.size()-4) && !::isspace(expression[elsePos+4]))
                        elsePos = expression.find("else", elsePos + 4);
                }

                if (elsePos == std::string::npos)
                    return 0; // 'if' and 'then' found without matching 'else'

                if (parseLayoutString(expression.substr(ifPos + 2, thenPos - ifPos - 2)) != 0)
                    expression = expression.substr(0, ifPos) + tgui::to_string(parseLayoutString(expression.substr(thenPos + 4, elsePos - thenPos - 4)));
                else
                    expression = expression.substr(0, ifPos) + tgui::to_string(parseLayoutString(expression.substr(elsePos + 4)));
            }
        }

        // All brackets and conditionals should be remove by now
        if ((expression.find(')') != std::string::npos) || (expression.find(':') != std::string::npos))
            return 0;

        auto andPos = expression.rfind("&&");
        auto orPos = expression.rfind("||");
        if ((andPos != std::string::npos) || (orPos != std::string::npos))
        {
            if ((andPos == std::string::npos) || (orPos < andPos))
                return parseLayoutString(expression.substr(0, orPos)) || parseLayoutString(expression.substr(orPos + 2));
            else
                return parseLayoutString(expression.substr(0, andPos)) && parseLayoutString(expression.substr(andPos + 2));
        }

        andPos = expression.rfind("and");
        orPos = expression.rfind("or");
        if ((andPos != std::string::npos) || (orPos != std::string::npos))
        {
            if ((andPos == std::string::npos) || (orPos < andPos))
                return parseLayoutString(expression.substr(0, orPos)) || parseLayoutString(expression.substr(orPos + 2));
            else
                return parseLayoutString(expression.substr(0, andPos)) && parseLayoutString(expression.substr(andPos + 3));
        }

        auto equalsPos = expression.rfind("==");
        auto notEqualsPos = expression.rfind("!=");
        if ((equalsPos != std::string::npos) || (notEqualsPos != std::string::npos))
        {
            if ((equalsPos == std::string::npos) || (notEqualsPos < equalsPos))
                return parseLayoutString(expression.substr(0, notEqualsPos)) != parseLayoutString(expression.substr(notEqualsPos + 2));
            else
                return parseLayoutString(expression.substr(0, equalsPos)) == parseLayoutString(expression.substr(equalsPos + 2));
        }

        auto lessThanPos = expression.rfind('<');
        auto greaterThanPos = expression.rfind('>');
        auto lessEqualPos = expression.rfind("<=");
        auto greaterEqualPos = expression.rfind(">=");
        if ((lessThanPos != std::string::npos) || (greaterThanPos != std::string::npos))
        {
            if ((greaterThanPos != std::string::npos) && ((lessThanPos == std::string::npos) || (greaterThanPos < lessThanPos)))
            {
                if ((greaterEqualPos != std::string::npos) && (greaterEqualPos == greaterThanPos))
                    return parseLayoutString(expression.substr(0, greaterEqualPos)) >= parseLayoutString(expression.substr(greaterEqualPos + 2));
                else
                    return parseLayoutString(expression.substr(0, greaterThanPos)) > parseLayoutString(expression.substr(greaterThanPos + 1));
            }
            else // < or <=
            {
                if ((lessEqualPos != std::string::npos) && (lessEqualPos == lessThanPos))
                    return parseLayoutString(expression.substr(0, lessEqualPos)) <= parseLayoutString(expression.substr(lessEqualPos + 2));
                else
                    return parseLayoutString(expression.substr(0, lessThanPos)) < parseLayoutString(expression.substr(lessThanPos + 1));
            }
        }

        auto lastPos = std::string::npos;
        auto plusPos = expression.rfind('+', lastPos);
        auto minusPos = expression.rfind('-', lastPos);
        while ((plusPos != std::string::npos) || (minusPos != std::string::npos))
        {
            if ((plusPos != std::string::npos) && ((minusPos == std::string::npos) || (minusPos < plusPos)))
                return parseLayoutString(expression.substr(0, plusPos)) + parseLayoutString(expression.substr(plusPos + 1));
            else
            {
                // The minus might be a unary instead of a binary operator
                auto leftExpr = tgui::trim(expression.substr(0, minusPos));
                if (leftExpr.empty())
                    return -parseLayoutString(expression.substr(minusPos + 1));
                else if ((leftExpr.back() == '+') || (leftExpr.back() == '-') || (leftExpr.back() == '*') || (leftExpr.back() == '/') || (leftExpr.back() == '%'))
                    lastPos = minusPos - 1;
                else
                    return parseLayoutString(expression.substr(0, minusPos)) - parseLayoutString(expression.substr(minusPos + 1));
            }

            plusPos = expression.rfind('+', lastPos);
            minusPos = expression.rfind('-', lastPos);
        }

        auto multiplyPos = expression.rfind('*');
        auto dividePos = expression.rfind('/');
        auto modulusPos = expression.rfind('%');
        if ((multiplyPos != std::string::npos) || (dividePos != std::string::npos) || (modulusPos != std::string::npos))
        {
            if (multiplyPos != std::string::npos)
            {
                if ((dividePos == std::string::npos) || (dividePos < multiplyPos))
                {
                    if ((modulusPos == std::string::npos) || (modulusPos < multiplyPos))
                        return parseLayoutString(expression.substr(0, multiplyPos)) * parseLayoutString(expression.substr(multiplyPos + 1));
                }
            }
            if (dividePos != std::string::npos)
            {
                if ((multiplyPos == std::string::npos) || (multiplyPos < dividePos))
                {
                    if ((modulusPos == std::string::npos) || (modulusPos < dividePos))
                        return parseLayoutString(expression.substr(0, dividePos)) / parseLayoutString(expression.substr(dividePos + 1));
                }
            }
            if (modulusPos != std::string::npos)
            {
                if ((multiplyPos == std::string::npos) || (multiplyPos < modulusPos))
                {
                    if ((dividePos == std::string::npos) || (dividePos < modulusPos))
                        return std::fmod(parseLayoutString(expression.substr(0, modulusPos)),  parseLayoutString(expression.substr(modulusPos + 1)));
                }
            }
        }

        // The expression might reference to a widget instead of being a constant
        assert(!expression.empty());
        expression = tgui::toLower(tgui::trim(expression));
        if ((expression.substr(expression.size()-1) == "x")
         || (expression.substr(expression.size()-1) == "y")
         || (expression.substr(expression.size()-1) == "w") // width
         || (expression.substr(expression.size()-1) == "h") // height
         || (expression.size() >= 4 && expression.substr(expression.size()-4) == "left")
         || (expression.size() >= 3 && expression.substr(expression.size()-3) == "top")
         || (expression.size() >= 5 && expression.substr(expression.size()-5) == "width")
         || (expression.size() >= 6 && expression.substr(expression.size()-6) == "height")
         || (expression.size() >= 5 && expression.substr(expression.size()-5) == "right")
         || (expression.size() >= 6 && expression.substr(expression.size()-6) == "bottom"))
        {
            if (parentWidget)
                return parseWidgetName(expression, parentWidget);
            else
                return 0;
        }

        // The string no longer contains operators, so return the value that it contains
        return static_cast<float>(tgui::stof(expression));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    float LayoutImpl::parseWidgetName(const std::string& expression, Widget* widget, const std::string& alreadyParsedPart)
    {
        if (expression == "x" || expression == "left")
        {
            if (boundCallbacks.find(alreadyParsedPart + "position") == boundCallbacks.end())
            {
                boundCallbacks.insert(alreadyParsedPart + "position");
                widget->connect("PositionChanged", std::bind(resetLayout, shared_from_this(), std::bind(getWidgetLeft, widget)));
            }

            return widget->getPosition().x;
        }
        else if (expression == "y" || expression == "top")
        {
            if (boundCallbacks.find(alreadyParsedPart + "position") == boundCallbacks.end())
            {
                boundCallbacks.insert(alreadyParsedPart + "position");
                widget->connect("PositionChanged", std::bind(resetLayout, shared_from_this(), std::bind(getWidgetTop, widget)));
            }

            return widget->getPosition().y;
        }
        else if (expression == "w" || expression == "width")
        {
            if (boundCallbacks.find(alreadyParsedPart + "size") == boundCallbacks.end())
            {
                boundCallbacks.insert(alreadyParsedPart + "size");
                widget->connect("SizeChanged", std::bind(resetLayout, shared_from_this(), std::bind(getWidgetWidth, widget)));
            }

            return widget->getSize().x;
        }
        else if (expression == "h" || expression == "height")
        {
            if (boundCallbacks.find(alreadyParsedPart + "size") == boundCallbacks.end())
            {
                boundCallbacks.insert(alreadyParsedPart + "size");
                widget->connect("SizeChanged", std::bind(resetLayout, shared_from_this(), std::bind(getWidgetHeight, widget)));
            }

            return widget->getSize().y;
        }
        else if (expression == "right")
        {
            if (boundCallbacks.find(alreadyParsedPart + "position") == boundCallbacks.end())
            {
                boundCallbacks.insert(alreadyParsedPart + "position");
                widget->connect("PositionChanged", std::bind(resetLayout, shared_from_this(), std::bind(getWidgetLeft, widget)));
            }
            if (boundCallbacks.find(alreadyParsedPart + "size") == boundCallbacks.end())
            {
                boundCallbacks.insert(alreadyParsedPart + "size");
                widget->connect("SizeChanged", std::bind(resetLayout, shared_from_this(), std::bind(getWidgetWidth, widget)));
            }

            return widget->getPosition().x + widget->getSize().x;
        }
        else if (expression == "bottom")
        {
            if (boundCallbacks.find(alreadyParsedPart + "position") == boundCallbacks.end())
            {
                boundCallbacks.insert(alreadyParsedPart + "position");
                widget->connect("PositionChanged", std::bind(resetLayout, shared_from_this(), std::bind(getWidgetTop, widget)));
            }
            if (boundCallbacks.find(alreadyParsedPart + "size") == boundCallbacks.end())
            {
                boundCallbacks.insert(alreadyParsedPart + "size");
                widget->connect("SizeChanged", std::bind(resetLayout, shared_from_this(), std::bind(getWidgetHeight, widget)));
            }

            return widget->getPosition().y + widget->getSize().y;
        }

        auto dotPos = expression.find('.');
        if (dotPos != std::string::npos)
        {
            std::string widgetName = expression.substr(0, dotPos);
            if (widgetName == "parent" || widgetName == "&")
            {
                if (widget->getParent())
                    return parseWidgetName(expression.substr(dotPos+1), widget->getParent(), "parent.");
                else
                    return 0;
            }
            else if (!widgetName.empty())
            {
                // If the widget is a container, search in its children first
                Container* container = dynamic_cast<Container*>(widget);
                if (container != nullptr)
                {
                    auto widgetToBind = container->get(widgetName);
                    if (widgetToBind)
                        return parseWidgetName(expression.substr(dotPos+1), widgetToBind.get(), widgetName + ".");
                }

                // If the widget has a parent, look for a sibling
                if (widget->getParent())
                {
                    auto widgetToBind = widget->getParent()->get(widgetName);
                    if (widgetToBind)
                        return parseWidgetName(expression.substr(dotPos+1), widgetToBind.get(), widgetName + ".");
                }
            }
        }

        // Illegal expression
        return 0;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Layout::Layout()
    {
        m_impl->value = 0;
        m_impl->attachedLayouts.insert(this);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Layout::Layout(const char* expression) :
        Layout(std::string{expression})
    {
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Layout::Layout(const std::string& expression)
    {
        m_impl->stringExpression = expression;
        m_impl->operation = LayoutImpl::Operation::String;
        m_impl->attachedLayouts.insert(this);
        m_impl->recalculate();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Layout::Layout(const Layout& copy)
    {
        m_impl = copy.m_impl;
        m_impl->attachedLayouts.insert(this);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Layout::~Layout()
    {
        m_impl->attachedLayouts.erase(this);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Layout& Layout::operator=(const Layout& right)
    {
        if (&right != this)
        {
            m_impl->attachedLayouts.erase(this);
            right.m_impl->attachedLayouts.insert(this);
            m_impl = right.m_impl;
        }

        return *this;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    float Layout::getValue() const
    {
        return m_impl->value;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Layout::connectUpdateCallback(const std::function<void()>& callbackFunction)
    {
        m_callbackFunction = callbackFunction;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Layout::update() const
    {
        if (m_callbackFunction)
            m_callbackFunction();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::shared_ptr<LayoutImpl> Layout::getImpl() const
    {
        return m_impl;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Layout Layout::operator+()
    {
        return *this;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Layout Layout::operator-()
    {
        return 0 - std::move(*this);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Layout Layout::operator+=(Layout right)
    {
        return *this = std::move(*this) + std::move(right);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Layout Layout::operator-=(Layout right)
    {
        return *this = std::move(*this) - std::move(right);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Layout Layout::operator*=(Layout right)
    {
        return *this = std::move(*this) * std::move(right);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Layout Layout::operator/=(Layout right)
    {
        return *this = std::move(*this) / std::move(right);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Layout Layout::operator%=(Layout right)
    {
        return *this = std::move(*this) % std::move(right);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Layout2d::Layout2d(sf::Vector2f constant) :
        x{constant.x},
        y{constant.y}
    {
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Layout2d::Layout2d(Layout layoutX, Layout layoutY) :
        x{std::move(layoutX)},
        y{std::move(layoutY)}
    {
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Layout2d::Layout2d(const char* expression) :
        Layout2d(std::string{expression})
    {
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Layout2d::Layout2d(const std::string& expression)
    {
        std::string xExpr;
        std::string yExpr;

        std::size_t currentPos = 0;
        auto openingBracePos = expression.find('{', currentPos);
        auto bindPositionPos = expression.find("pos", currentPos);
        auto bindSizePos = expression.find("size", currentPos);
        while ((openingBracePos != std::string::npos) || (bindPositionPos != std::string::npos) || (bindSizePos != std::string::npos))
        {
            if ((openingBracePos != std::string::npos) && (openingBracePos < bindPositionPos) && (openingBracePos < bindSizePos))
            {
                auto closingBracePos = expression.find('}', openingBracePos + 1);
                if (closingBracePos == std::string::npos)
                    return; // No matching close brace found

                // There have to be two components
                auto commaPos = expression.find(',', openingBracePos + 1);
                if ((commaPos > closingBracePos) || (expression.find(',', commaPos + 1) < closingBracePos))
                    return;

                xExpr += expression.substr(currentPos, openingBracePos - currentPos) + expression.substr(openingBracePos + 1, commaPos - openingBracePos - 1);
                yExpr += expression.substr(currentPos, openingBracePos - currentPos) + expression.substr(commaPos + 1, closingBracePos - commaPos - 1);

                currentPos = closingBracePos + 1;
            }
            else if ((bindPositionPos != std::string::npos) && (bindPositionPos < openingBracePos) && (bindPositionPos < bindSizePos))
            {
                xExpr += expression.substr(currentPos, bindPositionPos - currentPos) + "left";
                yExpr += expression.substr(currentPos, bindPositionPos - currentPos) + "top";

                if (expression.find("position", bindPositionPos) == bindPositionPos)
                    currentPos = bindPositionPos + 8;
                else
                    currentPos = bindPositionPos + 3;
            }
            else if ((bindSizePos != std::string::npos) && (bindSizePos < bindPositionPos) && (bindSizePos < openingBracePos))
            {
                xExpr += expression.substr(currentPos, bindSizePos - currentPos) + "width";
                yExpr += expression.substr(currentPos, bindSizePos - currentPos) + "height";

                currentPos = bindSizePos + 4;
            }

            openingBracePos = expression.find('{', currentPos);
            bindPositionPos = expression.find("pos", currentPos);
            bindSizePos = expression.find("size", currentPos);
        }

        // All opening braces have been matched with a closing brace, there should no longer be braces
        if (expression.find('}', currentPos) != std::string::npos)
            return;

        xExpr += expression.substr(currentPos);
        yExpr += expression.substr(currentPos);

        x = {trim(xExpr)};
        y = {trim(yExpr)};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    sf::Vector2f Layout2d::getValue() const
    {
        return {x.getValue(), y.getValue()};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Layout2d Layout2d::operator+()
    {
        return *this;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Layout2d Layout2d::operator-()
    {
        return Layout2d{0, 0} - std::move(*this);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Layout2d Layout2d::operator+=(Layout2d right)
    {
        return *this = std::move(*this) + std::move(right);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Layout2d Layout2d::operator-=(Layout2d right)
    {
        return *this = std::move(*this) - std::move(right);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Layout2d Layout2d::operator*=(Layout right)
    {
        return *this = std::move(*this) * std::move(right);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Layout2d Layout2d::operator/=(Layout right)
    {
        return *this = std::move(*this) / std::move(right);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Layout2d Layout2d::operator%=(Layout right)
    {
        return *this = std::move(*this) % std::move(right);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Layout operator<(Layout left, Layout right)
    {
        return layoutOperator(std::move(left), std::move(right), LayoutImpl::Operation::LessThan);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Layout operator<=(Layout left, Layout right)
    {
        return layoutOperator(std::move(left), std::move(right), LayoutImpl::Operation::LessOrEqual);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Layout operator>(Layout left, Layout right)
    {
        return layoutOperator(std::move(left), std::move(right), LayoutImpl::Operation::GreaterThan);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Layout operator>=(Layout left, Layout right)
    {
        return layoutOperator(std::move(left), std::move(right), LayoutImpl::Operation::GreaterOrEqual);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Layout operator==(Layout left, Layout right)
    {
        return layoutOperator(std::move(left), std::move(right), LayoutImpl::Operation::Equal);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Layout operator!=(Layout left, Layout right)
    {
        return layoutOperator(std::move(left), std::move(right), LayoutImpl::Operation::NotEqual);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Layout operator+(Layout left, Layout right)
    {
        return layoutOperator(std::move(left), std::move(right), LayoutImpl::Operation::Plus);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Layout operator-(Layout left, Layout right)
    {
        return layoutOperator(std::move(left), std::move(right), LayoutImpl::Operation::Minus);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Layout operator*(Layout left, Layout right)
    {
        return layoutOperator(std::move(left), std::move(right), LayoutImpl::Operation::Multiplies);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Layout operator/(Layout left, Layout right)
    {
        return layoutOperator(std::move(left), std::move(right), LayoutImpl::Operation::Divides);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Layout operator%(Layout left, Layout right)
    {
        return layoutOperator(std::move(left), std::move(right), LayoutImpl::Operation::Modulus);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Layout operator&&(Layout left, Layout right)
    {
        return layoutOperator(std::move(left), std::move(right), LayoutImpl::Operation::And);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Layout operator||(Layout left, Layout right)
    {
        return layoutOperator(std::move(left), std::move(right), LayoutImpl::Operation::Or);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Layout operator==(Layout2d left, Layout2d right)
    {
        return {(left.x == right.x) && (left.y == right.y)};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Layout operator!=(Layout2d left, Layout2d right)
    {
        return {(left.x != right.x) || (left.y != right.y)};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Layout2d operator+(Layout2d left, Layout2d right)
    {
        return {left.x + right.x, left.y + right.y};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Layout2d operator-(Layout2d left, Layout2d right)
    {
        return {left.x - right.x, left.y - right.y};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Layout2d operator*(Layout2d left, Layout right)
    {
        return {left.x * right, left.y * right};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Layout2d operator/(Layout2d left, Layout right)
    {
        return {left.x / right, left.y / right};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Layout2d operator%(Layout2d left, Layout right)
    {
        return {left.x % right, left.y % right};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Layout2d operator*(Layout left, Layout2d right)
    {
        return {left * right.x, left * right.y};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Layout bindLeft(Widget::Ptr widget)
    {
        Layout result;
        result.getImpl()->value = widget->getPosition().x;
        widget->connect("PositionChanged", std::bind(resetLayout, result.getImpl(), std::bind(getWidgetLeft, widget.get())));
        return result;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Layout bindTop(Widget::Ptr widget)
    {
        Layout result;
        result.getImpl()->value = widget->getPosition().y;
        widget->connect("PositionChanged", std::bind(resetLayout, result.getImpl(), std::bind(getWidgetTop, widget.get())));
        return result;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Layout bindWidth(Widget::Ptr widget)
    {
        Layout result;
        result.getImpl()->value = widget->getSize().x;
        widget->connect("SizeChanged", std::bind(resetLayout, result.getImpl(), std::bind(getWidgetWidth, widget.get())));
        return result;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Layout bindHeight(Widget::Ptr widget)
    {
        Layout result;
        result.getImpl()->value = widget->getSize().y;
        widget->connect("SizeChanged", std::bind(resetLayout, result.getImpl(), std::bind(getWidgetHeight, widget.get())));
        return result;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Layout bindRight(Widget::Ptr widget)
    {
        Layout result;
        result.getImpl()->value = widget->getPosition().x + widget->getSize().x;
        widget->connect("PositionChanged", std::bind(resetLayout, result.getImpl(), std::bind(getWidgetRight, widget.get())));
        widget->connect("SizeChanged", std::bind(resetLayout, result.getImpl(), std::bind(getWidgetRight, widget.get())));
        return result;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Layout bindBottom(Widget::Ptr widget)
    {
        Layout result;
        result.getImpl()->value = widget->getPosition().y + widget->getSize().y;
        widget->connect("PositionChanged", std::bind(resetLayout, result.getImpl(), std::bind(getWidgetBottom, widget.get())));
        widget->connect("SizeChanged", std::bind(resetLayout, result.getImpl(), std::bind(getWidgetBottom, widget.get())));
        return result;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Layout2d bindPosition(Widget::Ptr widget)
    {
        return {bindLeft(widget), bindTop(widget)};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Layout2d bindSize(Widget::Ptr widget)
    {
        return {bindWidth(widget), bindHeight(widget)};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Layout bindLeft(Gui& gui)
    {
        return bindLeft(gui.getContainer());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Layout bindTop(Gui& gui)
    {
        return bindTop(gui.getContainer());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Layout bindWidth(Gui& gui)
    {
        return bindWidth(gui.getContainer());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Layout bindHeight(Gui& gui)
    {
        return bindHeight(gui.getContainer());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Layout bindRight(Gui& gui)
    {
        return bindRight(gui.getContainer());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Layout bindBottom(Gui& gui)
    {
        return bindBottom(gui.getContainer());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Layout2d bindPosition(Gui& gui)
    {
        return bindPosition(gui.getContainer());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Layout2d bindSize(Gui& gui)
    {
        return bindSize(gui.getContainer());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Layout bindMin(Layout value1, Layout value2)
    {
        return layoutOperator(std::move(value1), std::move(value2), LayoutImpl::Operation::Minimum);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Layout bindMax(Layout value1, Layout value2)
    {
        return layoutOperator(std::move(value1), std::move(value2), LayoutImpl::Operation::Maximum);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Layout bindRange(Layout minimum, Layout maximum, Layout value)
    {
        return bindMin(bindMax(value, minimum), maximum);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Layout bindIf(Layout condition, Layout trueExpr, Layout falseExpr)
    {
        tgui::Layout result;
        result.getImpl()->operation = LayoutImpl::Operation::Conditional;
        result.getImpl()->operands.push_back(condition.getImpl());
        result.getImpl()->operands.push_back(trueExpr.getImpl());
        result.getImpl()->operands.push_back(falseExpr.getImpl());
        condition.getImpl()->parents.insert(result.getImpl().get());
        trueExpr.getImpl()->parents.insert(result.getImpl().get());
        falseExpr.getImpl()->parents.insert(result.getImpl().get());
        result.getImpl()->recalculate();
        return result;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Layout2d bindIf(Layout condition, Layout2d trueExpr, Layout2d falseExpr)
    {
        return {bindIf(condition, trueExpr.x, falseExpr.x), bindIf(condition, trueExpr.y, falseExpr.y)};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Layout bindStr(const std::string& expression)
    {
        return {expression};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Layout bindStr(const char* expression)
    {
        return {std::string{expression}};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Layout2d bindStr2d(const std::string& expression)
    {
        return {expression};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Layout2d bindStr2d(const char* expression)
    {
        return {std::string{expression}};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
