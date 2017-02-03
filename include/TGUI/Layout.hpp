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


#ifndef TGUI_LAYOUT_HPP
#define TGUI_LAYOUT_HPP

#include <SFML/System/Vector2.hpp>
#include <TGUI/Config.hpp>
#include <functional>
#include <memory>
#include <vector>
#include <set>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    class Gui;
    class Widget;
    class Layout;

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @brief Shared information between layouts
    /// @internal
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    class TGUI_API LayoutImpl : public std::enable_shared_from_this<LayoutImpl>
    {
    public:

        /// Does the layout contain a value or an operation between other layouts?
        enum class Operation
        {
            Value,
            String,
            Plus,
            Minus,
            Multiplies,
            Divides,
            Modulus,
            And,
            Or,
            LessThan,
            LessOrEqual,
            GreaterThan,
            GreaterOrEqual,
            Equal,
            NotEqual,
            Minimum,
            Maximum,
            Conditional
        };


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    public:

        /// @brief Destructor
        ~LayoutImpl();

        /// @brief Recalculate the value
        void recalculate();


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    private:

        // Calculate the value of a layout defined by a string
        float parseLayoutString(std::string expression);

        // Parse references to widgets from the layout strings
        float parseWidgetName(const std::string& expression, Widget* widget, const std::string& alreadyParsedPart = "");


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    public:
        float value = 0; ///< Cached value of the layout
        std::set<Layout*> attachedLayouts; ///< Layout objects that use this shared object
        std::set<LayoutImpl*> parents; ///< Other layouts that make use of this layout in their expression

        Operation operation = Operation::Value; ///< Does the layout contain a value or an operation between other layouts?
        std::vector<std::shared_ptr<LayoutImpl>> operands; ///< Operands used in the operation that this object performs

        // These members are only used when operation == Operation::String
        std::string stringExpression; ///< String expression in this layout
        Widget* parentWidget = nullptr; ///< Widget connected to this layout

    private:
        std::set<std::string> boundCallbacks;
    };


    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @brief Class to store the left, top, width or height of a widget
    ///
    /// You don't have to create an instance of this class, numbers are implicitly cast to this class.
    ///
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    class TGUI_API Layout
    {
    public:

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Default constructor
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        Layout();


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Constructor to implicitly construct from numeric constant
        ///
        /// @param constant  Value of the layout
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        template <typename T, typename = typename std::enable_if<std::is_arithmetic<T>::value, T>::type>
        Layout(T constant)
        {
            m_impl->value = static_cast<float>(constant);
            m_impl->attachedLayouts.insert(this);
        }


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Construct the layout based on a string which will be parsed to determine the value of the layout
        ///
        /// @param expression  String to parse
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        Layout(const char* expression);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Construct the layout based on a string which will be parsed to determine the value of the layout
        ///
        /// @param expression  String to parse
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        Layout(const std::string& expression);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Copy constructor
        ///
        /// @param copy  Instance to copy
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        Layout(const Layout& copy);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Destructor
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        ~Layout();


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Overload of assignment operator
        ///
        /// @param right  Instance to assign
        ///
        /// @return Reference to itself
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        Layout& operator=(const Layout& right);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Return the cached value of the layout
        ///
        /// @return Value of the layout
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        float getValue() const;


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @internal
        /// @brief Connect a callback function to call when the layout is updated
        ///
        /// @param callbackFunction  Function to call when layout value changes
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void connectUpdateCallback(const std::function<void()>& callbackFunction);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @internal
        /// @brief Tells the layout that its value has been changed and that the callback function must be called
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void update() const;


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @internal
        /// @brief Access the layout implementation that is shared between layout objects
        ///
        /// @return Shared layout data
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        std::shared_ptr<LayoutImpl> getImpl() const;


        /// @brief Unary plus operator for the Layout class
        Layout operator+();

        /// @brief Unary minus operator for the Layout class
        Layout operator-();

        /// @brief += operator for the Layout class
        Layout operator+=(Layout right);

        /// @brief -= operator for the Layout class
        Layout operator-=(Layout right);

        /// @brief *= operator for the Layout class
        Layout operator*=(Layout right);

        /// @brief /= operator for the Layout class
        Layout operator/=(Layout right);

        /// @brief %= operator for the Layout class (floating point modulo)
        Layout operator%=(Layout right);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    private:
        std::shared_ptr<LayoutImpl> m_impl = std::make_shared<LayoutImpl>();
        std::function<void()> m_callbackFunction;
    };


    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @brief Class to store the position or size of a widget
    ///
    /// You don't have to create an instance of this class, sf::Vector2f is implicitly converted to this class.
    ///
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    class TGUI_API Layout2d
    {
    public:

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Default constructor to implicitly construct from an sf::Vector2f.
        ///
        /// @param constant  Value of the layout
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        Layout2d(sf::Vector2f constant = {0, 0});


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Constructor to create the Layout2d from two Layout classes
        ///
        /// @param layoutX  x component
        /// @param layoutY  y component
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        Layout2d(Layout layoutX, Layout layoutY);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Construct the layout based on a string which will be parsed to determine the value of the layout
        ///
        /// @param expression  String to parse
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        Layout2d(const char* expression);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Constructs the layout based on a string which will be parsed to determine the value of the layout
        ///
        /// @param expression  String to parse
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        Layout2d(const std::string& expression);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Return the cached value of the layout
        ///
        /// @return Value of the layout
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        sf::Vector2f getValue() const;


        /// @brief Unary plus operator for the Layout class
        Layout2d operator+();

        /// @brief Unary minus operator for the Layout class
        Layout2d operator-();

        /// @brief += operator for the Layout2d class
        Layout2d operator+=(Layout2d right);

        /// @brief -= operator for the Layout2d class
        Layout2d operator-=(Layout2d right);

        /// @brief *= operator for the Layout2d class
        Layout2d operator*=(Layout right);

        /// @brief /= operator for the Layout2d class
        Layout2d operator/=(Layout right);

        /// @brief %= operator for the Layout2d class (floating point modulo)
        Layout2d operator%=(Layout right);

    public:
        Layout x; ///< Layout to store the x component
        Layout y; ///< Layout to store the y component
    };

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    /// @brief \< operator for the Layout class
    TGUI_API Layout operator<(Layout left, Layout right);

    /// @brief <= operator for the Layout class
    TGUI_API Layout operator<=(Layout left, Layout right);

    /// @brief \> operator for the Layout class
    TGUI_API Layout operator>(Layout left, Layout right);

    /// @brief >= operator for the Layout class
    TGUI_API Layout operator>=(Layout left, Layout right);

    /// @brief == operator for the Layout class
    TGUI_API Layout operator==(Layout left, Layout right);

    /// @brief != operator for the Layout class
    TGUI_API Layout operator!=(Layout left, Layout right);

    /// @brief + operator for the Layout class
    TGUI_API Layout operator+(Layout left, Layout right);

    /// @brief - operator for the Layout class
    TGUI_API Layout operator-(Layout left, Layout right);

    /// @brief * operator for the Layout class
    TGUI_API Layout operator*(Layout left, Layout right);

    /// @brief / operator for the Layout class
    TGUI_API Layout operator/(Layout left, Layout right);

    /// @brief % operator for the Layout class
    TGUI_API Layout operator%(Layout left, Layout right);

    /// @brief && operator for the Layout class
    TGUI_API Layout operator&&(Layout left, Layout right);

    /// @brief || operator for the Layout class
    TGUI_API Layout operator||(Layout left, Layout right);

    /// @brief == operator for the Layout2d class
    TGUI_API Layout operator==(Layout2d left, Layout2d right);

    /// @brief != operator for the Layout2d class
    TGUI_API Layout operator!=(Layout2d left, Layout2d right);

    /// @brief + operator for the Layout2d class
    TGUI_API Layout2d operator+(Layout2d left, Layout2d right);

    /// @brief - operator for the Layout2d class
    TGUI_API Layout2d operator-(Layout2d left, Layout2d right);

    /// @brief * operator for the Layout2d class
    TGUI_API Layout2d operator*(Layout2d left, Layout right);

    /// @brief / operator for the Layout2d class
    TGUI_API Layout2d operator/(Layout2d left, Layout right);

    /// @brief % operator for the Layout2d class
    TGUI_API Layout2d operator%(Layout2d left, Layout right);

    /// @brief * operator for the Layout2d class
    TGUI_API Layout2d operator*(Layout left, Layout2d right);

    /// @brief Bind to the x position of the widget
    TGUI_API Layout bindLeft(std::shared_ptr<Widget> widget);

    /// @brief Bind to the y position of the widget
    TGUI_API Layout bindTop(std::shared_ptr<Widget> widget);

    /// @brief Bind to the width of the widget
    TGUI_API Layout bindWidth(std::shared_ptr<Widget> widget);

    /// @brief Bind to the height of the widget
    TGUI_API Layout bindHeight(std::shared_ptr<Widget> widget);

    /// @brief Bind to the right position of the widget
    TGUI_API Layout bindRight(std::shared_ptr<Widget> widget);

    /// @brief Bind to the bottom of the widget
    TGUI_API Layout bindBottom(std::shared_ptr<Widget> widget);

    /// @brief Bind to the position of the widget
    TGUI_API Layout2d bindPosition(std::shared_ptr<Widget> widget);

    /// @brief Bind to the size of the widget
    TGUI_API Layout2d bindSize(std::shared_ptr<Widget> widget);

    /// @brief Bind to the x position of the gui view
    TGUI_API Layout bindLeft(Gui& gui);

    /// @brief Bind to the y position of the gui view
    TGUI_API Layout bindTop(Gui& gui);

    /// @brief Bind to the width of the gui view
    TGUI_API Layout bindWidth(Gui& gui);

    /// @brief Bind to the height of the gui view
    TGUI_API Layout bindHeight(Gui& gui);

    /// @brief Bind to the right position of the gui view
    TGUI_API Layout bindRight(Gui& gui);

    /// @brief Bind to the bottom position of the gui view
    TGUI_API Layout bindBottom(Gui& gui);

    /// @brief Bind to the position of the gui view
    TGUI_API Layout2d bindPosition(Gui& gui);

    /// @brief Bind to the size of the gui view
    TGUI_API Layout2d bindSize(Gui& gui);

    /// @brief Bind to the minimum of two values
    TGUI_API Layout bindMin(Layout value1, Layout value2);

    /// @brief Bind to the maximum of two values
    TGUI_API Layout bindMax(Layout value1, Layout value2);

    /// @brief Bind to a value that remains between the minimum and maximum
    TGUI_API Layout bindRange(Layout minimum, Layout maximum, Layout value);

    /// @brief Bind conditionally to one of the two layouts
    TGUI_API Layout bindIf(Layout condition, Layout trueExpr, Layout falseExpr);

    /// @brief Bind conditionally to one of the two layouts
    TGUI_API Layout2d bindIf(Layout condition, Layout2d trueExpr, Layout2d falseExpr);

    /// @brief Bind a string for a layout (you can also just create the layout directly with the string)
    TGUI_API Layout bindStr(const std::string& expression);

    /// @brief Bind a string for a layout (you can also just create the layout directly with the string)
    TGUI_API Layout bindStr(const char* expression);

    /// @brief Bind a string for a layout (you can also just create the layout directly with the string)
    TGUI_API Layout2d bindStr2d(const std::string& expression);

    /// @brief Bind a string for a layout (you can also just create the layout directly with the string)
    TGUI_API Layout2d bindStr2d(const char* expression);

    // Put TGUI_IMPORT_LAYOUT_BIND_FUNCTIONS somewhere in your code to no longer have to put "tgui::" in front of the bind functions
    // without having to import everything from tgui in your namespace or writing all these using statements yourself.
    #define TGUI_IMPORT_LAYOUT_BIND_FUNCTIONS \
        using tgui::bindLeft; \
        using tgui::bindTop; \
        using tgui::bindWidth; \
        using tgui::bindHeight; \
        using tgui::bindRight; \
        using tgui::bindBottom; \
        using tgui::bindPosition; \
        using tgui::bindSize; \
        using tgui::bindMin; \
        using tgui::bindMax; \
        using tgui::bindRange; \
        using tgui::bindIf; \
        using tgui::bindStr; \
        using tgui::bindStr2d;

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif // TGUI_LAYOUT_HPP
