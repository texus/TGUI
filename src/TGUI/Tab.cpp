/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus's Graphical User Interface
// Copyright (C) 2012 Bruno Van de Velde (vdv_b@tgui.eu)
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


#include <TGUI/Objects.hpp>
#include <TGUI/ClickableObject.hpp>
#include <TGUI/Tab.hpp>

#include <SFML/OpenGL.hpp>
#include <cmath>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Tab::Tab() :
    m_LoadedPathname       (""),
    m_SplitImage           (false),
    m_SeparateSelectedImage(true),
    m_TabHeight            (0),
    m_TextSize             (0),
    m_MaximumTabWidth      (0),
    m_DistanceToSide       (0),
    m_SelectedTab          (0),
    m_TextureNormal_L      (NULL),
    m_TextureNormal_M      (NULL),
    m_TextureNormal_R      (NULL),
    m_TextureSelected_L    (NULL),
    m_TextureSelected_M    (NULL),
    m_TextureSelected_R    (NULL)
    {
        m_Callback.objectType = Type_Tab;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Tab::Tab(const Tab& copy) :
    Object                 (copy),
    m_LoadedPathname       (copy.m_LoadedPathname),
    m_SplitImage           (copy.m_SplitImage),
    m_SeparateSelectedImage(copy.m_SeparateSelectedImage),
    m_TabHeight            (copy.m_TabHeight),
    m_TextSize             (copy.m_TextSize),
    m_MaximumTabWidth      (copy.m_MaximumTabWidth),
    m_DistanceToSide       (copy.m_DistanceToSide),
    m_SelectedTab          (copy.m_SelectedTab),
    m_TabNames             (copy.m_TabNames),
    m_NameWidth            (copy.m_NameWidth),
    m_Text                 (copy.m_Text)
    {
        if (TGUI_TextureManager.copyTexture(copy.m_TextureNormal_L, m_TextureNormal_L))      m_SpriteNormal_L.setTexture(*m_TextureNormal_L);
        if (TGUI_TextureManager.copyTexture(copy.m_TextureNormal_M, m_TextureNormal_M))      m_SpriteNormal_M.setTexture(*m_TextureNormal_M);
        if (TGUI_TextureManager.copyTexture(copy.m_TextureNormal_R, m_TextureNormal_R))      m_SpriteNormal_R.setTexture(*m_TextureNormal_R);

        if (TGUI_TextureManager.copyTexture(copy.m_TextureSelected_L, m_TextureSelected_L))  m_SpriteSelected_L.setTexture(*m_TextureSelected_L);
        if (TGUI_TextureManager.copyTexture(copy.m_TextureSelected_M, m_TextureSelected_M))  m_SpriteSelected_M.setTexture(*m_TextureSelected_M);
        if (TGUI_TextureManager.copyTexture(copy.m_TextureSelected_R, m_TextureSelected_R))  m_SpriteSelected_R.setTexture(*m_TextureSelected_R);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Tab::~Tab()
    {
        if (m_TextureNormal_L != NULL)    TGUI_TextureManager.removeTexture(m_TextureNormal_L);
        if (m_TextureNormal_M != NULL)    TGUI_TextureManager.removeTexture(m_TextureNormal_M);
        if (m_TextureNormal_R != NULL)    TGUI_TextureManager.removeTexture(m_TextureNormal_R);

        if (m_TextureSelected_L != NULL)  TGUI_TextureManager.removeTexture(m_TextureSelected_L);
        if (m_TextureSelected_M != NULL)  TGUI_TextureManager.removeTexture(m_TextureSelected_M);
        if (m_TextureSelected_R != NULL)  TGUI_TextureManager.removeTexture(m_TextureSelected_R);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Tab& Tab::operator= (const Tab& right)
    {
        // Make sure it is not the same object
        if (this != &right)
        {
            Tab temp(right);
            this->Object::operator=(right);

            std::swap(m_LoadedPathname,        temp.m_LoadedPathname);
            std::swap(m_SplitImage,            temp.m_SplitImage);
            std::swap(m_SeparateSelectedImage, temp.m_SeparateSelectedImage);
            std::swap(m_TabHeight,             temp.m_TabHeight);
            std::swap(m_TextSize,              temp.m_TextSize);
            std::swap(m_MaximumTabWidth,       temp.m_MaximumTabWidth);
            std::swap(m_DistanceToSide,        temp.m_DistanceToSide);
            std::swap(m_SelectedTab,           temp.m_SelectedTab);
            std::swap(m_TabNames,              temp.m_TabNames);
            std::swap(m_NameWidth,             temp.m_NameWidth);
            std::swap(m_TextureNormal_L,       temp.m_TextureNormal_L);
            std::swap(m_TextureNormal_M,       temp.m_TextureNormal_M);
            std::swap(m_TextureNormal_R,       temp.m_TextureNormal_R);
            std::swap(m_TextureSelected_L,     temp.m_TextureSelected_L);
            std::swap(m_TextureSelected_M,     temp.m_TextureSelected_M);
            std::swap(m_TextureSelected_R,     temp.m_TextureSelected_R);
            std::swap(m_SpriteNormal_L,        temp.m_SpriteNormal_L);
            std::swap(m_SpriteNormal_M,        temp.m_SpriteNormal_M);
            std::swap(m_SpriteNormal_R,        temp.m_SpriteNormal_R);
            std::swap(m_SpriteSelected_L,      temp.m_SpriteSelected_L);
            std::swap(m_SpriteSelected_M,      temp.m_SpriteSelected_M);
            std::swap(m_SpriteSelected_R,      temp.m_SpriteSelected_R);
            std::swap(m_Text,                  temp.m_Text);
        }

        return *this;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Tab* Tab::clone()
    {
        return new Tab(*this);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Tab::load(const std::string& pathname)
    {
        // Until the loading succeeds, the tab object will be marked as unloaded
        m_Loaded = false;

        // Make sure that the pathname isn't empty
        if (pathname.empty())
            return false;

        // Store the filename
        m_LoadedPathname = pathname;

        // When the pathname does not end with a "/" then we will add it
        if (m_LoadedPathname[m_LoadedPathname.length()-1] != '/')
            m_LoadedPathname.push_back('/');

        // Open the info file
        InfoFileParser infoFile;
        if (infoFile.openFile(m_LoadedPathname + "info.txt") == false)
        {
            TGUI_OUTPUT("TGUI error: Failed to open \"" + m_LoadedPathname + "info.txt\".");
            return false;
        }

        std::string property;
        std::string value;

        // Set some default settings
        m_SplitImage = false;
        m_SeparateSelectedImage = true;
        m_DistanceToSide = 5;
        std::string imageExtension = "png";

        // Read untill the end of the file
        while (infoFile.readProperty(property, value))
        {
            // Check what the property is
            if (property.compare("splitimage") == 0)
            {
                if ((value.compare("true") == 0) || (value.compare("1") == 0))
                    m_SplitImage = true;
                else
                {
                    if ((value.compare("false") != 0) && (value.compare("0") != 0))
                        TGUI_OUTPUT("TGUI warning: Wrong value passed to SplitImage: \"" + value + "\".");
                }
            }
            else if (property.compare("separateselectedimage") == 0)
            {
                if ((value.compare("false") == 0) || (value.compare("0") == 0))
                    m_SeparateSelectedImage = false;
                else
                {
                    if ((value.compare("true") != 0) && (value.compare("1") != 0))
                        TGUI_OUTPUT("TGUI warning: Wrong value passed to SeparateSelectedImage: \"" + value + "\".");
                }
            }
            else if (property.compare("phases") == 0)
            {
                // Get and store the different phases
                extractPhases(value);
            }
            else if (property.compare("extension") == 0)
            {
                imageExtension = value;
            }
            else if (property.compare("distancetoside") == 0)
            {
                m_DistanceToSide = atoi(value.c_str());
            }
            else if (property.compare("maximumtabwidth") == 0)
            {
                m_MaximumTabWidth = atoi(value.c_str());
            }
            else if (property.compare("textcolor") == 0)
            {
                m_Text.setColor(extractColor(value));
            }
            else
                TGUI_OUTPUT("TGUI warning: Option not recognised: \"" + property + "\".");
        }

        // Close the info file
        infoFile.closeFile();

        // Remove the textures when they were loaded before
        if (m_TextureNormal_L != NULL)    TGUI_TextureManager.removeTexture(m_TextureNormal_L);
        if (m_TextureNormal_M != NULL)    TGUI_TextureManager.removeTexture(m_TextureNormal_M);
        if (m_TextureNormal_R != NULL)    TGUI_TextureManager.removeTexture(m_TextureNormal_R);
        if (m_TextureSelected_L != NULL)  TGUI_TextureManager.removeTexture(m_TextureSelected_L);
        if (m_TextureSelected_M != NULL)  TGUI_TextureManager.removeTexture(m_TextureSelected_M);
        if (m_TextureSelected_R != NULL)  TGUI_TextureManager.removeTexture(m_TextureSelected_R);

        // Clear the vectors
        m_TabNames.clear();
        m_NameWidth.clear();

        // Check if the tab image is split
        if (m_SplitImage)
        {
            // Load the required textures
            if ((TGUI_TextureManager.getTexture(m_LoadedPathname + "L_Normal." + imageExtension, m_TextureNormal_L))
             && (TGUI_TextureManager.getTexture(m_LoadedPathname + "M_Normal." + imageExtension, m_TextureNormal_M))
             && (TGUI_TextureManager.getTexture(m_LoadedPathname + "R_Normal." + imageExtension, m_TextureNormal_R)))
            {
                m_SpriteNormal_L.setTexture(*m_TextureNormal_L, true);
                m_SpriteNormal_M.setTexture(*m_TextureNormal_M, true);
                m_SpriteNormal_R.setTexture(*m_TextureNormal_R, true);

                m_TabHeight = m_TextureNormal_M->getSize().y;
            }
            else
            {
                TGUI_OUTPUT("TGUI error: Failed to load \"" + m_LoadedPathname + "*_Normal." + imageExtension + "\".");
                return false;
            }

            // load the optional texture
            if (m_ObjectPhase & ObjectPhase_Selected)
            {
                if ((TGUI_TextureManager.getTexture(m_LoadedPathname + "L_Selected." + imageExtension, m_TextureSelected_L))
                 && (TGUI_TextureManager.getTexture(m_LoadedPathname + "M_Selected." + imageExtension, m_TextureSelected_M))
                 && (TGUI_TextureManager.getTexture(m_LoadedPathname + "R_Selected." + imageExtension, m_TextureSelected_R)))
                {
                    m_SpriteSelected_L.setTexture(*m_TextureSelected_L, true);
                    m_SpriteSelected_M.setTexture(*m_TextureSelected_M, true);
                    m_SpriteSelected_R.setTexture(*m_TextureSelected_R, true);
                }
                else
                {
                    TGUI_OUTPUT("TGUI error: Failed to load \"" + m_LoadedPathname + "*_Selected." + imageExtension + "\".");
                    return false;
                }
            }
        }
        else // The tab image isn't split
        {
            // Load the required texture
            if (TGUI_TextureManager.getTexture(m_LoadedPathname + "Normal." + imageExtension, m_TextureNormal_M))
            {
                 m_SpriteNormal_M.setTexture(*m_TextureNormal_M, true);
                 m_TabHeight = m_TextureNormal_M->getSize().y;
            }
            else
            {
                TGUI_OUTPUT("TGUI error: Failed to load \"" + m_LoadedPathname + "Normal." + imageExtension + "\".");
                return false;
            }

            // load the optional texture
            if (m_ObjectPhase & ObjectPhase_Selected)
            {
                if (TGUI_TextureManager.getTexture(m_LoadedPathname + "Selected." + imageExtension, m_TextureSelected_M))
                    m_SpriteSelected_M.setTexture(*m_TextureSelected_M, true);
                else
                {
                    TGUI_OUTPUT("TGUI error: Failed to load \"" + m_LoadedPathname + "Selected." + imageExtension + "\".");
                    return false;
                }
            }
        }

        // Recalculate the size when auto sizing
        if (m_TextSize == 0)
            setTextSize(0);

        // When there is no error we will return true
        m_Loaded = true;
        return true;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Vector2f Tab::getSize() const
    {
        // Make sure the tab has been loaded
        if (m_Loaded == false)
            return Vector2f(0, 0);

        // Add the width of all the tabs together
        float width = 0;
        for (unsigned int i=0; i<m_NameWidth.size(); ++i)
        {
            if (m_SplitImage)
                width += TGUI_MAXIMUM(m_MaximumTabWidth ? TGUI_MINIMUM(m_NameWidth[i] + (2 * m_DistanceToSide), m_MaximumTabWidth) : m_NameWidth[i] + (2 * m_DistanceToSide),
                                      (m_TextureNormal_L->getSize().x + m_TextureNormal_R->getSize().x) * (m_TabHeight / static_cast<float>(m_TextureNormal_M->getSize().y)));
            else
                width += m_MaximumTabWidth ? TGUI_MINIMUM(m_NameWidth[i] + (2 * m_DistanceToSide), m_MaximumTabWidth) : m_NameWidth[i] + (2 * m_DistanceToSide);
        }

        return Vector2f(width, m_TabHeight);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::string Tab::getLoadedPathname() const
    {
        return m_LoadedPathname;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int Tab::add(const sf::String& name, bool selectTab)
    {
        // Add the tab
        m_TabNames.push_back(name);

        // Calculate the width of the tab
        m_Text.setString(name);
        m_NameWidth.push_back(m_Text.getLocalBounds().width);

        // If the tab has to be selected then do so
        if (selectTab)
            m_SelectedTab = m_TabNames.size()-1;

        // Return the index of the new tab
        return m_TabNames.size()-1;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Tab::select(const sf::String& name)
    {
        // Loop through all tabs
        for (unsigned int i=0; i<m_TabNames.size(); ++i)
        {
            // Find the tab that should be selected
            if (m_TabNames[i] == name)
            {
                // Select the tab
                m_SelectedTab = i;
                return;
            }
        }

        TGUI_OUTPUT("TGUI warning: Failed to select the tab. The name didn't match any tab.");
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Tab::select(unsigned int index)
    {
        // If the index is too big then do nothing
        if (index > m_TabNames.size()-1)
        {
            TGUI_OUTPUT("TGUI warning: Failed to select the tab. The index was too high.");
            return;
        }

        // Select the tab
        m_SelectedTab = index;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Tab::deselect()
    {
        m_SelectedTab = -1;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Tab::remove(const sf::String& name)
    {
        // Loop through all tabs
        for (unsigned int i=0; i<m_TabNames.size(); ++i)
        {
            // Check if you found the tab to remove
            if (m_TabNames[i] == name)
            {
                // Remove the tab
                m_TabNames.erase(m_TabNames.begin() + i);
                m_NameWidth.erase(m_NameWidth.begin() + i);

                // Check if the selected tab should change
                if (m_SelectedTab == static_cast<int>(i))
                    m_SelectedTab = -1;
                else if (m_SelectedTab > static_cast<int>(i))
                    --m_SelectedTab;

                return;
            }
        }

        TGUI_OUTPUT("TGUI warning: Failed to remove the tab. The name didn't match any tab.");
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Tab::remove(unsigned int index)
    {
        // The index can't be too high
        if (index > m_TabNames.size()-1)
        {
            TGUI_OUTPUT("TGUI warning: Failed to remove the tab. The index was too high.");
            return;
        }

        // Remove the tab
        m_TabNames.erase(m_TabNames.begin() + index);
        m_NameWidth.erase(m_NameWidth.begin() + index);

        // Check if the selected tab should change
        if (m_SelectedTab == static_cast<int>(index))
            m_SelectedTab = -1;
        else if (m_SelectedTab > static_cast<int>(index))
            --m_SelectedTab;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Tab::removeAll()
    {
        m_TabNames.clear();
        m_NameWidth.clear();
        m_SelectedTab = -1;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    sf::String Tab::getSelected() const
    {
        if (m_SelectedTab == -1)
            return "";
        else
            return m_TabNames[m_SelectedTab];
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    int Tab::getSelectedIndex() const
    {
        return m_SelectedTab;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Tab::setTextFont(const sf::Font& font)
    {
        m_Text.setFont(font);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const sf::Font* Tab::getTextFont() const
    {
        return m_Text.getFont();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Tab::setTextColor(const sf::Color& color)
    {
        m_Text.setColor(color);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const sf::Color& Tab::getTextColor() const
    {
        return m_Text.getColor();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Tab::setTextSize(unsigned int size)
    {
        // Change the text size
        m_TextSize = size;

        // Check if the text is auto sized
        if (m_TextSize == 0)
        {
            // Calculate the text size
            m_Text.setString("kg");
            m_Text.setCharacterSize(static_cast<unsigned int>(m_TabHeight * 0.85f));
            m_Text.setCharacterSize(static_cast<unsigned int>(m_Text.getCharacterSize() - m_Text.getLocalBounds().top));
        }
        else // When the text has a fixed size
        {
            // Set the text size
            m_Text.setCharacterSize(m_TextSize);
        }

        // Recalculate the name widths
        for (unsigned int i=0; i<m_NameWidth.size(); ++i)
        {
            m_Text.setString(m_TabNames[i]);
            m_NameWidth[i] = m_Text.getLocalBounds().width;
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int Tab::getTextSize() const
    {
        return m_Text.getCharacterSize();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Tab::setTabHeight(unsigned int height)
    {
        // Make sure that the height changed
        if (m_TabHeight != height)
        {
            m_TabHeight = height;

            // Recalculate the size when auto sizing
            if (m_TextSize == 0)
                setTextSize(0);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int Tab::getTabHeight() const
    {
        return m_TabHeight;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Tab::setMaximumTabWidth(unsigned int maximumWidth)
    {
        m_MaximumTabWidth = maximumWidth;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int Tab::getMaximumTabWidth() const
    {
        return m_MaximumTabWidth;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Tab::setDistanceToSide(unsigned int distanceToSide)
    {
        m_DistanceToSide = distanceToSide;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int Tab::getDistanceToSide() const
    {
        return m_DistanceToSide;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Tab::mouseOnObject(float x, float y)
    {
        // Check if the mouse is on top of the tab
        if (m_Loaded)
        {
            sf::Vector2f size = getSize();
            size.x /= getScale().x;
            size.y /= getScale().y;
            if (getTransform().transformRect(sf::FloatRect(0, 0, size.x, size.y)).contains(x, y))
                return true;
        }

        if (m_MouseHover)
            mouseLeftObject();

        m_MouseHover = false;
        return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Tab::leftMousePressed(float x, float y)
    {
        float width = getPosition().x;

        // Loop through all tabs
        for (unsigned int i=0; i<m_NameWidth.size(); ++i)
        {
            // Append the width of the tab
            if (m_SplitImage)
                width += (TGUI_MAXIMUM(m_MaximumTabWidth ? TGUI_MINIMUM(m_NameWidth[i] + (2 * m_DistanceToSide), m_MaximumTabWidth) : m_NameWidth[i] + (2 * m_DistanceToSide),
                                       (m_TextureNormal_L->getSize().x + m_TextureNormal_R->getSize().x) * (m_TabHeight / static_cast<float>(m_TextureNormal_M->getSize().y)))) * getScale().x;
            else
                width += (m_MaximumTabWidth ? TGUI_MINIMUM(m_NameWidth[i] + (2 * m_DistanceToSide), m_MaximumTabWidth) : m_NameWidth[i] + (2 * m_DistanceToSide)) * getScale().x;

            // Check if the mouse went down on the tab
            if (x < width)
            {
                // Select this tab
                m_SelectedTab = i;

                // Add the callback (if the user requested it)
                if (m_CallbackFunctions[TabChanged].empty() == false)
                {
                    m_Callback.trigger = TabChanged;
                    m_Callback.value   = m_SelectedTab;
                    m_Callback.text    = m_TabNames[i];
                    m_Callback.mouse.x = x - getPosition().x;
                    m_Callback.mouse.y = y - getPosition().y;
                    addCallback();
                }

                // The tab was found
                break;
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Tab::initialize(tgui::Group *const parent)
    {
        m_Parent = parent;
        m_Text.setFont(m_Parent->getGlobalFont());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Tab::draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        // Don't draw when the child window wasn't created
        if (m_Loaded == false)
            return;

        // Get the old clipping area
        GLint scissor[4];
        glGetIntegerv(GL_SCISSOR_BOX, scissor);

        // Calculate the scale factor of the view
        float scaleViewX = target.getSize().x / target.getView().getSize().x;
        float scaleViewY = target.getSize().y / target.getView().getSize().y;

        // Apply the transformations
        states.transform *= getTransform();

        float scalingY = m_TabHeight / static_cast<float>(m_TextureNormal_M->getSize().y);
        bool clippingRequired = false;
        unsigned int tabWidth;
        sf::FloatRect realRect;
        sf::FloatRect defaultRect;
        sf::Text tempText(m_Text);

        // Calculate the height and top of all strings
        tempText.setString("kg");
        defaultRect = tempText.getLocalBounds();

        // Loop through all tabs
        for (unsigned int i=0; i<m_TabNames.size(); ++i)
        {
            // Find the tab height
            if (m_MaximumTabWidth)
            {
                if (m_MaximumTabWidth < m_NameWidth[i] + (2 * m_DistanceToSide))
                {
                    tabWidth = m_MaximumTabWidth;
                    clippingRequired = true;
                }
                else
                    tabWidth = static_cast<unsigned int>(m_NameWidth[i] + (2 * m_DistanceToSide));
            }
            else
                tabWidth = static_cast<unsigned int>(m_NameWidth[i] + (2 * m_DistanceToSide));

            // There is a minimum tab width
            if (tabWidth < 2 * m_DistanceToSide)
                tabWidth = 2 * m_DistanceToSide;

            // Check if the image is split
            if (m_SplitImage)
            {
                // There is another minimum when using SplitImage
                float minimumWidth = (m_TextureNormal_L->getSize().x + m_TextureNormal_R->getSize().x) * scalingY;
                if (tabWidth < minimumWidth)
                    tabWidth = static_cast<unsigned int>(minimumWidth);

                // Set the scaling
                states.transform.scale(scalingY, scalingY);

                // Draw the left tab image
                if (m_SeparateSelectedImage)
                {
                    if ((m_SelectedTab == static_cast<int>(i)) && (m_ObjectPhase & ObjectPhase_Selected))
                        target.draw(m_SpriteSelected_L, states);
                    else
                        target.draw(m_SpriteNormal_L, states);
                }
                else // There is no separate selected image
                {
                    target.draw(m_SpriteNormal_L, states);

                    if ((m_SelectedTab == static_cast<int>(i)) && (m_ObjectPhase & ObjectPhase_Selected))
                        target.draw(m_SpriteSelected_L, states);
                }

                // Check if the middle image may be drawn
                if ((scalingY * (m_TextureNormal_L->getSize().x + m_TextureNormal_R->getSize().x)) < tabWidth)
                {
                    // Calculate the scale for our middle image
                    float scaleX = (tabWidth / static_cast<float>(m_TextureNormal_M->getSize().x)) - (((m_TextureNormal_L->getSize().x + m_TextureNormal_R->getSize().x) * scalingY) / m_TextureNormal_M->getSize().x);

                    // Put the middle image on the correct position
                    states.transform.translate(static_cast<float>(m_TextureNormal_L->getSize().x), 0);

                    // Set the scale for the middle image
                    states.transform.scale(scaleX / scalingY, 1);

                    // Draw the middle tab image
                    if (m_SeparateSelectedImage)
                    {
                        if ((m_SelectedTab == static_cast<int>(i)) && (m_ObjectPhase & ObjectPhase_Selected))
                            target.draw(m_SpriteSelected_M, states);
                        else
                            target.draw(m_SpriteNormal_M, states);
                    }
                    else // There is no separate selected image
                    {
                        target.draw(m_SpriteNormal_M, states);

                        if ((m_SelectedTab == static_cast<int>(i)) && (m_ObjectPhase & ObjectPhase_Selected))
                            target.draw(m_SpriteSelected_M, states);
                    }

                    // Put the right image on the correct position
                    states.transform.translate(static_cast<float>(m_TextureNormal_M->getSize().x), 0);

                    // Set the scale for the right image
                    states.transform.scale(scalingY / scaleX, 1);

                    // Draw the right tab image
                    if (m_SeparateSelectedImage)
                    {
                        if ((m_SelectedTab == static_cast<int>(i)) && (m_ObjectPhase & ObjectPhase_Selected))
                            target.draw(m_SpriteSelected_R, states);
                        else
                            target.draw(m_SpriteNormal_R, states);
                    }
                    else // There is no separate selected image
                    {
                        target.draw(m_SpriteNormal_R, states);

                        if ((m_SelectedTab == static_cast<int>(i)) && (m_ObjectPhase & ObjectPhase_Selected))
                            target.draw(m_SpriteSelected_R, states);
                    }

                    // Undo the translation
                    states.transform.translate(-(m_TextureNormal_L->getSize().x + (m_TextureNormal_M->getSize().x * scaleX / scalingY)), 0);
                }
                else // The edit box isn't width enough, we will draw it at minimum size
                {
                    // Put the right image on the correct position
                    states.transform.translate(static_cast<float>(m_TextureNormal_L->getSize().x), 0);

                    // Draw the right tab image
                    if (m_SeparateSelectedImage)
                    {
                        if ((m_SelectedTab == static_cast<int>(i)) && (m_ObjectPhase & ObjectPhase_Selected))
                            target.draw(m_SpriteSelected_R, states);
                        else
                            target.draw(m_SpriteNormal_R, states);
                    }
                    else // There is no separate selected image
                    {
                        target.draw(m_SpriteNormal_R, states);

                        if ((m_SelectedTab == static_cast<int>(i)) && (m_ObjectPhase & ObjectPhase_Selected))
                            target.draw(m_SpriteSelected_R, states);
                    }

                    // Undo the translation
                    states.transform.translate(-static_cast<float>(m_TextureNormal_L->getSize().x), 0);
                }

                // Undo the scaling of the split images
                states.transform.scale(1.f/scalingY, 1.f/scalingY);
            }
            else // The image isn't split
            {
                // Set the scaling
                states.transform.scale(tabWidth / static_cast<float>(m_TextureNormal_M->getSize().x), scalingY);

                // Draw the tab image
                if (m_SeparateSelectedImage)
                {
                    if ((m_SelectedTab == static_cast<int>(i)) && (m_ObjectPhase & ObjectPhase_Selected))
                        target.draw(m_SpriteSelected_M, states);
                    else
                        target.draw(m_SpriteNormal_M, states);
                }
                else // There is no separate selected image
                {
                    target.draw(m_SpriteNormal_M, states);

                    if ((m_SelectedTab == static_cast<int>(i)) && (m_ObjectPhase & ObjectPhase_Selected))
                        target.draw(m_SpriteSelected_M, states);
                }

                // Undo the scaling
                states.transform.scale(static_cast<float>(m_TextureNormal_M->getSize().x) / tabWidth, 1.f / scalingY);
            }

            // Draw the text
            {
                // Get the current size of the text, so that we can recalculate the position
                tempText.setString(m_TabNames[i]);
                realRect = tempText.getLocalBounds();

                // Calculate the new position for the text
                if ((m_SplitImage) && (tabWidth == (m_TextureNormal_L->getSize().x + m_TextureNormal_R->getSize().x) * scalingY))
                    realRect.left = ((tabWidth - realRect.width) / 2.f) - realRect.left;
                else
                    realRect.left = m_DistanceToSide - realRect.left;
                realRect.top = ((m_TabHeight - defaultRect.height) / 2.f) - defaultRect.top;

                // Move the text to the correct position
                states.transform.translate(std::floor(realRect.left + 0.5f), std::floor(realRect.top + 0.5f));

                // Check if clipping is required for this text
                if (clippingRequired)
                {
                    // Undo the current scaling
                    states.transform.scale(1.f / getScale().x, 1.f / getScale().y);

                    // Get the global position
                    Vector2f topLeftPosition = states.transform.transformPoint((target.getView().getSize() / 2.f) - target.getView().getCenter());
                    Vector2f bottomRightPosition = states.transform.transformPoint(Vector2f((tabWidth - (2 * m_DistanceToSide)) * getScale().x, ((m_TabHeight + defaultRect.height) / 2.f) * getScale().y) - target.getView().getCenter() + (target.getView().getSize() / 2.f));

                    // Calculate the clipping area
                    GLint scissorLeft = TGUI_MAXIMUM(static_cast<GLint>(topLeftPosition.x * scaleViewX), scissor[0]);
                    GLint scissorTop = TGUI_MAXIMUM(static_cast<GLint>(topLeftPosition.y * scaleViewY), static_cast<GLint>(target.getSize().y) - scissor[1] - scissor[3]);
                    GLint scissorRight = TGUI_MINIMUM(static_cast<GLint>(bottomRightPosition.x * scaleViewX), scissor[0] + scissor[2]);
                    GLint scissorBottom = TGUI_MINIMUM(static_cast<GLint>(bottomRightPosition.y * scaleViewY), static_cast<GLint>(target.getSize().y) - scissor[1]);

                    // If the object outside the window then don't draw anything
                    if (scissorRight < scissorLeft)
                        scissorRight = scissorLeft;
                    else if (scissorBottom < scissorTop)
                        scissorTop = scissorBottom;

                    // Set the clipping area
                    glScissor(scissorLeft, target.getSize().y - scissorBottom, scissorRight - scissorLeft, scissorBottom - scissorTop);

                    // Redo the scaling
                    states.transform.scale(getScale());
                }

                // Draw the text
                target.draw(tempText, states);

                // Undo the translation of the text
                states.transform.translate(-std::floor(realRect.left + 0.5f), -std::floor(realRect.top + 0.5f));

                // Reset the old clipping area when needed
                if (clippingRequired)
                {
                    clippingRequired = false;
                    glScissor(scissor[0], scissor[1], scissor[2], scissor[3]);
                }
            }

            // Set the next tab on the correct position
            states.transform.translate(static_cast<float>(tabWidth), 0);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
