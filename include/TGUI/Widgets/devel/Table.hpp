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


#ifndef TGUI_TABLE_HPP
#define TGUI_TABLE_HPP

#include <TGUI/BoxLayout.hpp>
#include <TGUI/Widgets/devel/TableRow.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    class TGUI_API Table: public BoxLayout
    {
    public:
        typedef std::shared_ptr<Table> Ptr; ///< Shared widget pointer
        typedef std::shared_ptr<const Table> ConstPtr; ///< Shared constant widget pointer

        Table();

        virtual Widget::Ptr clone() const override
        {
            return std::make_shared<Table>(*this);
        }

        virtual void setFont(const Font& font) override;
        void setTextSize(unsigned int size);

        virtual void setSize(const Layout2d& size) override;
        using Transformable::setSize;

        bool insert(std::size_t index, const tgui::Widget::Ptr& widget, const sf::String& widgetName = "");
        virtual void add(const tgui::Widget::Ptr& widget, const sf::String& widgetName = "") override;
        void addRow(const std::vector<std::string>& columns);

        void setHeader(const tgui::TableRow::Ptr& row);
        void setHeaderColumns(const std::vector<std::string>& columns);
        // getHeader()

        void setFixedColumnWidth(std::size_t column, float size);
        void setColumnRatio(std::size_t column, float ratio);

        // virtual void setFixedRowsHeight(float size);

        // void showTableBorders(bool show = true);
        // void showColumnsBorders(bool show = true);
        // void setTableBordersColor(const sf::Color& color);
        // void setColumnsBorder(const sf::Color& color);

        void setRowsColor(const sf::Color& color);
        void setStripesRowsColor(const sf::Color& evenColor, const sf::Color& oddColor);

        void setTextColor(const sf::Color& color);

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    private:
        virtual void updateWidgetPositions() override;
        void calculateLabelHeight();
        void updateColumnsDelimitatorsPosition();
        void updateColumnsDelimitatorsSize();

        virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    private:
        tgui::TableRow::Ptr m_header = std::make_shared<tgui::TableRow>();

        float m_rowHeight = 20;

        sf::RectangleShape m_tableBorder;
        sf::RectangleShape m_headerSeparator;
        std::vector<sf::RectangleShape> m_columnsDelimitators;
        sf::Color m_bordersColor;

        sf::Color m_rowsOddColor;
        sf::Color m_rowsEvenColor = sf::Color::Transparent;

        unsigned int m_characterSize = 18;

        sf::Color m_normalTextColor = sf::Color::Black;
    };

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif // TGUI_TABLE_HPP
