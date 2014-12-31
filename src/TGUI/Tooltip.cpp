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


#include <TGUI/Tooltip.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Tooltip::Tooltip()
    {
        m_callback.widgetType = WidgetType::Tooltip;

        m_background.setFillColor({245, 245, 245});
        m_text.setColor({60, 60, 60});

        setBorders(1);
        setPadding(2);

        setTextSize(16);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Tooltip::Ptr Tooltip::create(const std::string& themeFileFilename, const std::string& section)
    {
        auto tooltip = std::make_shared<Tooltip>();

        if (themeFileFilename != "")
        {
            tooltip->setBorders(0);
            tooltip->setPadding(0);

            std::string loadedThemeFile = getResourcePath() + themeFileFilename;

            // Open the theme file
            ThemeFileParser themeFile{loadedThemeFile, section};

            // Handle the read properties
            for (auto it = themeFile.getProperties().cbegin(); it != themeFile.getProperties().cend(); ++it)
            {
                if (it->first == "textcolor")
                    tooltip->setTextColor(extractColorFromString(it->first, it->second));
                else if (it->first == "backgroundcolor")
                    tooltip->setBackgroundColor(extractColorFromString(it->first, it->second));
                else if (it->first == "bordercolor")
                    tooltip->setBorderColor(extractColorFromString(it->first, it->second));
                else if (it->first == "borders")
                    tooltip->setBorders(extractBordersFromString(it->first, it->second));
                else if (it->first == "padding")
                    tooltip->setPadding(extractBordersFromString(it->first, it->second));
                else
                    throw Exception{"Unrecognized property '" + it->first + "' in section '" + section + "' in " + loadedThemeFile + "."};
            }
        }

        return tooltip;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Tooltip::Ptr Tooltip::copy(Tooltip::ConstPtr tooltip)
    {
        if (tooltip)
            return std::make_shared<Tooltip>(*tooltip);
        else
            return nullptr;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
