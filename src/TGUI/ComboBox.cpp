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


#include <SFML/OpenGL.hpp>

#include <TGUI/Scrollbar.hpp>
#include <TGUI/ListBox.hpp>
#include <TGUI/Container.hpp>
#include <TGUI/ChildWindow.hpp>
#include <TGUI/ComboBox.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ComboBox::ComboBox()
    {
        m_callback.widgetType = "ComboBox";
        m_draggableWidget = true;

        addSignal<sf::String, TypeSet<sf::String, sf::String>>("ItemSelected");

        initListBox();

        m_renderer = std::make_shared<ComboBoxRenderer>(this);

        getRenderer()->setBorders({2, 2, 2, 2});

        setSize({50, 24});

        m_text.setTextColor(sf::Color::Black);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ComboBox::ComboBox(const ComboBox& listBoxToCopy) :
        Widget              {listBoxToCopy},
        m_nrOfItemsToDisplay{listBoxToCopy.m_nrOfItemsToDisplay},
        m_listBox           {ListBox::copy(listBoxToCopy.m_listBox)},
        m_text              {listBoxToCopy.m_text}
    {
        if (m_listBox != nullptr)
        {
            m_listBox->disconnectAll();
            initListBox();
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ComboBox& ComboBox::operator= (const ComboBox& right)
    {
        if (this != &right)
        {
            ComboBox temp{right};
            Widget::operator=(right);

            std::swap(m_nrOfItemsToDisplay, temp.m_nrOfItemsToDisplay);
            std::swap(m_listBox,            temp.m_listBox);
            std::swap(m_text,               temp.m_text);
        }

        return *this;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ComboBox::Ptr ComboBox::create(const std::string& themeFileFilename, const std::string& section)
    {
        auto comboBox = std::make_shared<ComboBox>();

        if (themeFileFilename != "")
        {
            comboBox->getRenderer()->setBorders({0, 0, 0, 0});

            std::string loadedThemeFile = getResourcePath() + themeFileFilename;
            ThemeFileParser themeFile{loadedThemeFile, section};

            // Find the folder that contains the theme file
            std::string themeFileFolder = "";
            std::string::size_type slashPos = themeFileFilename.find_last_of("/\\");
            if (slashPos != std::string::npos)
                themeFileFolder = themeFileFilename.substr(0, slashPos+1);

            // Handle the read properties
            for (auto it = themeFile.getProperties().cbegin(); it != themeFile.getProperties().cend(); ++it)
            {
                try
                {
                    if (it->first == "listbox")
                    {
                        if ((it->second.length() < 3) || (it->second[0] != '"') || (it->second[it->second.length()-1] != '"'))
                            throw Exception{"Failed to parse value for 'ListBox' property."};

                        comboBox->m_listBox = ListBox::create(themeFileFilename, it->second.substr(1, it->second.length()-2));
                        comboBox->initListBox();
                    }
                    else
                        comboBox->getRenderer()->setProperty(it->first, it->second, themeFileFolder);
                }
                catch (const Exception& e)
                {
                    throw Exception{std::string(e.what()) + " In section '" + section + "' in " + loadedThemeFile + "."};
                }
            }
        }

        return comboBox;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ComboBox::Ptr ComboBox::copy(ComboBox::ConstPtr comboBox)
    {
        if (comboBox)
            return std::make_shared<ComboBox>(*comboBox);
        else
            return nullptr;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ComboBox::setPosition(const Layout& position)
    {
        Widget::setPosition(position);

        Padding padding = getRenderer()->getScaledPadding();

        getRenderer()->m_backgroundTexture.setPosition(getPosition());

        getRenderer()->m_textureArrowUpNormal.setPosition({getPosition().x + getSize().x - getRenderer()->m_textureArrowUpNormal.getSize().x - padding.right, getPosition().y + padding.top});
        getRenderer()->m_textureArrowDownNormal.setPosition({getPosition().x + getSize().x - getRenderer()->m_textureArrowDownNormal.getSize().x - padding.right, getPosition().y + padding.top});

        getRenderer()->m_textureArrowUpHover.setPosition(getRenderer()->m_textureArrowUpNormal.getPosition());
        getRenderer()->m_textureArrowDownHover.setPosition(getRenderer()->m_textureArrowUpNormal.getPosition());

        float textHeight;
        if (m_font)
            textHeight = sf::Text{"kg", *m_font, m_text.getTextSize()}.getLocalBounds().height;
        else
            textHeight = 0;

        m_text.setPosition(getPosition().x + padding.left + (m_text.getTextSize() / 10.0f),
                           getPosition().y + padding.top + ((getSize().y - textHeight) / 2.0f));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ComboBox::setSize(const Layout& size)
    {
        Widget::setSize(size);

        getRenderer()->m_backgroundTexture.setSize(getSize());

        Padding listBoxPadding = m_listBox->getRenderer()->getScaledPadding();
        Padding padding = getRenderer()->getScaledPadding();
        float height = getSize().y - padding.top - padding.bottom;

        m_listBox->setItemHeight(static_cast<unsigned int>(height));

        if (m_nrOfItemsToDisplay > 0)
            m_listBox->setSize({getSize().x, (m_listBox->getItemHeight() * (std::min<std::size_t>(m_nrOfItemsToDisplay, std::max<std::size_t>(m_listBox->getItemCount(), 1)))) + listBoxPadding.top + listBoxPadding.bottom});
        else
            m_listBox->setSize({getSize().x, (m_listBox->getItemHeight() * std::max<std::size_t>(m_listBox->getItemCount(), 1)) + listBoxPadding.top + listBoxPadding.bottom});

        if (getRenderer()->m_textureArrowUpNormal.getData() && getRenderer()->m_textureArrowDownNormal.getData())
        {
            getRenderer()->m_textureArrowUpNormal.setSize({getRenderer()->m_textureArrowUpNormal.getImageSize().x * (height / getRenderer()->m_textureArrowUpNormal.getImageSize().y), height});
            getRenderer()->m_textureArrowDownNormal.setSize({getRenderer()->m_textureArrowDownNormal.getImageSize().x * (height / getRenderer()->m_textureArrowDownNormal.getImageSize().y), height});

            getRenderer()->m_textureArrowUpHover.setSize(getRenderer()->m_textureArrowUpNormal.getSize());
            getRenderer()->m_textureArrowDownHover.setSize(getRenderer()->m_textureArrowUpNormal.getSize());
        }

        m_text.setTextSize(static_cast<unsigned int>(height * 0.8f));

        updatePosition();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    sf::Vector2f ComboBox::getFullSize() const
    {
        return {getSize().x + getRenderer()->getBorders().left + getRenderer()->getBorders().right,
                getSize().y + getRenderer()->getBorders().top + getRenderer()->getBorders().bottom};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ComboBox::setItemsToDisplay(unsigned int nrOfItemsInList)
    {
        m_nrOfItemsToDisplay = nrOfItemsInList;

        Padding padding = m_listBox->getRenderer()->getScaledPadding();
        if (m_nrOfItemsToDisplay < m_listBox->m_items.size())
            m_listBox->setSize({m_listBox->getSize().x, (m_nrOfItemsToDisplay * m_listBox->getItemHeight()) + padding.top + padding.bottom});
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ComboBox::addItem(const sf::String& item, const sf::String& id)
    {
        // Make room to add another item, until there are enough items
        if ((m_nrOfItemsToDisplay == 0) || (m_listBox->getItemCount() < m_nrOfItemsToDisplay))
        {
            Padding padding = m_listBox->getRenderer()->getScaledPadding();
            m_listBox->setSize({m_listBox->getSize().x, (m_listBox->getItemHeight() * (m_listBox->getItemCount() + 1)) + padding.top + padding.bottom});
        }

        // Add the item
        return m_listBox->addItem(item, id);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ComboBox::setSelectedItem(const sf::String& itemName)
    {
        bool ret = m_listBox->setSelectedItem(itemName);
        m_text.setText(m_listBox->getSelectedItem());
        return ret;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ComboBox::setSelectedItemById(const sf::String& id)
    {
        bool ret = m_listBox->setSelectedItemById(id);
        m_text.setText(m_listBox->getSelectedItem());
        return ret;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ComboBox::deselectItem()
    {
        m_text.setText("");
        m_listBox->deselectItem();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ComboBox::removeItem(const sf::String& itemName)
    {
        bool ret = m_listBox->removeItem(itemName);

        m_text.setText(m_listBox->getSelectedItem());

        // Shrink the list size
        if ((m_nrOfItemsToDisplay == 0) || (m_listBox->getItemCount() < m_nrOfItemsToDisplay))
        {
            Padding padding = m_listBox->getRenderer()->getScaledPadding();
            m_listBox->setSize({m_listBox->getSize().x, (m_listBox->getItemHeight() * std::max<std::size_t>(m_listBox->getItemCount(), 1)) + padding.top + padding.bottom});
        }

        return ret;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ComboBox::removeItemById(const sf::String& id)
    {
        bool ret = m_listBox->removeItemById(id);

        m_text.setText(m_listBox->getSelectedItem());

        // Shrink the list size
        if ((m_nrOfItemsToDisplay == 0) || (m_listBox->getItemCount() < m_nrOfItemsToDisplay))
        {
            Padding padding = m_listBox->getRenderer()->getScaledPadding();
            m_listBox->setSize({m_listBox->getSize().x, (m_listBox->getItemHeight() * std::max<std::size_t>(m_listBox->getItemCount(), 1)) + padding.top + padding.bottom});
        }

        return ret;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ComboBox::removeAllItems()
    {
        m_text.setText("");
        m_listBox->removeAllItems();

        // Shrink the list size
        Padding padding = m_listBox->getRenderer()->getScaledPadding();
        m_listBox->setSize({m_listBox->getSize().x, m_listBox->getItemHeight() + padding.top + padding.bottom});
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ComboBox::changeItem(const sf::String& originalValue, const sf::String& newValue)
    {
        bool ret = m_listBox->changeItem(originalValue, newValue);
        m_text.setText(m_listBox->getSelectedItem());
        return ret;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ComboBox::changeItemById(const sf::String& id, const sf::String& newValue)
    {
        bool ret = m_listBox->changeItemById(id, newValue);
        m_text.setText(m_listBox->getSelectedItem());
        return ret;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ComboBox::removeScrollbar()
    {
        m_listBox->removeScrollbar();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ComboBox::setMaximumItems(unsigned int maximumItems)
    {
        m_listBox->setMaximumItems(maximumItems);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int ComboBox::getMaximumItems() const
    {
        return m_listBox->getMaximumItems();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ComboBox::setTransparency(unsigned char transparency)
    {
        Widget::setTransparency(transparency);

        m_listBox->setTransparency(m_opacity);

        getRenderer()->m_textureArrowUpNormal.setColor(sf::Color(255, 255, 255, m_opacity));
        getRenderer()->m_textureArrowDownNormal.setColor(sf::Color(255, 255, 255, m_opacity));
        getRenderer()->m_textureArrowUpHover.setColor(sf::Color(255, 255, 255, m_opacity));
        getRenderer()->m_textureArrowDownHover.setColor(sf::Color(255, 255, 255, m_opacity));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ComboBox::mouseOnWidget(float x, float y)
    {
        // Check if the mouse is on top of the combo box
        if ((x > getPosition().x - getRenderer()->getBorders().left) && (x < getPosition().x + getSize().x + getRenderer()->getBorders().right)
         && (y > getPosition().y - getRenderer()->getBorders().top) && (y < getPosition().y + getSize().y + getRenderer()->getBorders().bottom))
        {
            return true;
        }

        if (m_mouseHover)
            mouseLeftWidget();

        return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ComboBox::leftMousePressed(float, float)
    {
        m_mouseDown = true;

        // If the list wasn't visible then open it
        if (!m_listBox->isVisible())
        {
            // Show the list
            showListBox();

            // Check if there is a scrollbar
            if (m_listBox->m_scroll != nullptr)
            {
                // If the selected item is not visible then change the value of the scrollbar
                if (m_nrOfItemsToDisplay > 0)
                {
                    if (static_cast<unsigned int>(m_listBox->m_selectedItem + 1) > m_nrOfItemsToDisplay)
                        m_listBox->m_scroll->setValue(static_cast<unsigned int>((m_listBox->m_selectedItem + 1 - m_nrOfItemsToDisplay) * m_listBox->getItemHeight()));
                    else
                        m_listBox->m_scroll->setValue(0);

                    m_listBox->updatePosition();
                }
            }
        }
        else // This list was already open, so close it now
            hideListBox();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ComboBox::leftMouseReleased(float, float)
    {
        m_mouseDown = false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ComboBox::mouseWheelMoved(int delta, int, int)
    {
        // The list isn't visible
        if (!m_listBox->isVisible())
        {
            // Check if you are scrolling down
            if (delta < 0)
            {
                // select the next item
                if (static_cast<unsigned int>(m_listBox->m_selectedItem + 1) < m_listBox->m_items.size())
                    m_listBox->setSelectedItem(static_cast<unsigned int>(m_listBox->m_selectedItem+1));
            }
            else // You are scrolling up
            {
                // select the previous item
                if (m_listBox->m_selectedItem > 0)
                    m_listBox->setSelectedItem(static_cast<unsigned int>(m_listBox->m_selectedItem-1));
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ComboBox::mouseNoLongerDown()
    {
        m_mouseDown = false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ComboBox::initialize(Container *const parent)
    {
        Widget::initialize(parent);

        if (!m_font && m_parent->getGlobalFont())
            getRenderer()->setTextFont(m_parent->getGlobalFont());

        m_text.initialize(parent);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ComboBox::showListBox()
    {
        if (!m_listBox->isVisible() && getParent())
        {
            m_listBox->show();

            // Find the GuiContainer that contains the combo box
            Container* container = getParent();
            while (container->getParent() != nullptr)
                container = container->getParent();

            m_listBox->setPosition({getAbsolutePosition().x, getAbsolutePosition().y + getSize().y + getRenderer()->getBorders().top});
            container->add(m_listBox, "#TGUI_INTERNAL$ComboBoxListBox#");
            m_listBox->focus();
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ComboBox::hideListBox()
    {
        // If the list was open then close it now
        if (m_listBox->isVisible())
        {
            m_listBox->hide();

            // Find the gui in order to remove the ListBox from it
            Widget* container = this;
            while (container->getParent() != nullptr)
                container = container->getParent();

            if (container != this)
                static_cast<Container*>(container)->remove(m_listBox);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ComboBox::initListBox()
    {
        m_listBox->hide();
        m_listBox->connect("ItemSelected", &ComboBox::newItemSelectedCallbackFunction, this);
        m_listBox->connect("Unfocused", &ComboBox::listBoxUnfocusedCallbackFunction, this);
        m_listBox->connect("MouseReleased", &ComboBox::hideListBox, this);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ComboBox::newItemSelectedCallbackFunction()
    {
        m_text.setText(m_listBox->getSelectedItem());

        m_callback.text   = m_listBox->getSelectedItem();
        m_callback.itemId = m_listBox->getSelectedItemId();
        sendSignal("ItemSelected", m_listBox->getSelectedItem(), m_listBox->getSelectedItemId());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ComboBox::listBoxUnfocusedCallbackFunction()
    {
        if (m_mouseHover == false)
            hideListBox();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ComboBox::draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        // Draw the background
        getRenderer()->draw(target, states);

        const sf::View& view = target.getView();

        // Calculate the scale factor of the view
        float scaleViewX = target.getSize().x / view.getSize().x;
        float scaleViewY = target.getSize().y / view.getSize().y;

        Padding padding = getRenderer()->getScaledPadding();

        float arrowWidth;
        if (getRenderer()->m_textureArrowUpNormal.getData() && getRenderer()->m_textureArrowDownNormal.getData())
            arrowWidth = getRenderer()->m_textureArrowDownNormal.getSize().x * (getSize().y / getRenderer()->m_textureArrowDownNormal.getSize().y);
        else
            arrowWidth = getSize().y - padding.top - padding.bottom;

        // Get the global position
        sf::Vector2f topLeftPosition = {((getAbsolutePosition().x + padding.left - view.getCenter().x + (view.getSize().x / 2.f)) * view.getViewport().width) + (view.getSize().x * view.getViewport().left),
                                        ((getAbsolutePosition().y + padding.top - view.getCenter().y + (view.getSize().y / 2.f)) * view.getViewport().height) + (view.getSize().y * view.getViewport().top)};
        sf::Vector2f bottomRightPosition = {(getAbsolutePosition().x + (getSize().x - padding.right - arrowWidth) - view.getCenter().x + (view.getSize().x / 2.f)) * view.getViewport().width + (view.getSize().x * view.getViewport().left),
                                            (getAbsolutePosition().y + (getSize().y - padding.bottom) - view.getCenter().y + (view.getSize().y / 2.f)) * view.getViewport().height + (view.getSize().y * view.getViewport().top)};

        // Get the old clipping area
        GLint scissor[4];
        glGetIntegerv(GL_SCISSOR_BOX, scissor);

        // Calculate the clipping area
        GLint scissorLeft = std::max(static_cast<GLint>(topLeftPosition.x * scaleViewX), scissor[0]);
        GLint scissorTop = std::max(static_cast<GLint>(topLeftPosition.y * scaleViewY), static_cast<GLint>(target.getSize().y) - scissor[1] - scissor[3]);
        GLint scissorRight = std::min(static_cast<GLint>(bottomRightPosition.x * scaleViewX), scissor[0] + scissor[2]);
        GLint scissorBottom = std::min(static_cast<GLint>(bottomRightPosition.y * scaleViewY), static_cast<GLint>(target.getSize().y) - scissor[1]);

        if (scissorRight < scissorLeft)
            scissorRight = scissorLeft;
        else if (scissorBottom < scissorTop)
            scissorTop = scissorBottom;

        // Set the clipping area
        glScissor(scissorLeft, target.getSize().y - scissorBottom, scissorRight - scissorLeft, scissorBottom - scissorTop);

        // Draw the selected item
        target.draw(m_text, states);

        // Reset the old clipping area
        glScissor(scissor[0], scissor[1], scissor[2], scissor[3]);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ComboBoxRenderer::setProperty(std::string property, const std::string& value, const std::string& rootPath)
    {
        if (property == "backgroundimage")
        {
            extractTextureFromString(property, value, rootPath, m_comboBox->m_listBox->getRenderer()->m_backgroundTexture);
            extractTextureFromString(property, value, rootPath, m_backgroundTexture);
        }
        else if (property == "arrowupnormalimage")
            extractTextureFromString(property, value, rootPath, m_textureArrowUpNormal);
        else if (property == "arrowuphoverimage")
            extractTextureFromString(property, value, rootPath, m_textureArrowUpHover);
        else if (property == "arrowdownnormalimage")
            extractTextureFromString(property, value, rootPath, m_textureArrowDownNormal);
        else if (property == "arrowdownhoverimage")
            extractTextureFromString(property, value, rootPath, m_textureArrowDownHover);
        else if (property == "backgroundcolor")
            setBackgroundColor(extractColorFromString(property, value));
        else if (property == "arrowbackgroundcolor")
            setArrowBackgroundColor(extractColorFromString(property, value));
        else if (property == "arrowbackgroundcolornormal")
            setArrowBackgroundColorNormal(extractColorFromString(property, value));
        else if (property == "arrowbackgroundcolorhover")
            setArrowBackgroundColorHover(extractColorFromString(property, value));
        else if (property == "arrowcolor")
            setArrowColor(extractColorFromString(property, value));
        else if (property == "arrowcolornormal")
            setArrowColorNormal(extractColorFromString(property, value));
        else if (property == "arrowcolorhover")
            setArrowColorHover(extractColorFromString(property, value));
        else if (property == "textcolor")
            setTextColor(extractColorFromString(property, value));
        else if (property == "bordercolor")
            setBorderColor(extractColorFromString(property, value));
        else if (property == "borders")
            setBorders(extractBordersFromString(property, value));
        else if (property == "padding")
            setPadding(extractBordersFromString(property, value));
        else
            throw Exception{"Unrecognized property '" + property + "'."};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ComboBoxRenderer::setBackgroundImage(const std::string& filename, const sf::IntRect& partRect, const sf::IntRect& middlePart, bool repeated)
    {
        if (filename != "")
            m_backgroundTexture.load(filename, partRect, middlePart, repeated);
        else
            m_backgroundTexture = {};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ComboBoxRenderer::setArrowUpNormalImage(const std::string& filename, const sf::IntRect& partRect, const sf::IntRect& middlePart, bool repeated)
    {
        if (filename != "")
            m_comboBox->getRenderer()->m_textureArrowUpNormal.load(filename, partRect, middlePart, repeated);
        else
            m_comboBox->getRenderer()->m_textureArrowUpNormal = {};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ComboBoxRenderer::setArrowDownNormalImage(const std::string& filename, const sf::IntRect& partRect, const sf::IntRect& middlePart, bool repeated)
    {
        if (filename != "")
            m_comboBox->getRenderer()->m_textureArrowDownNormal.load(filename, partRect, middlePart, repeated);
        else
            m_comboBox->getRenderer()->m_textureArrowDownNormal = {};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ComboBoxRenderer::setArrowUpHoverImage(const std::string& filename, const sf::IntRect& partRect, const sf::IntRect& middlePart, bool repeated)
    {
        if (filename != "")
            m_comboBox->getRenderer()->m_textureArrowUpHover.load(filename, partRect, middlePart, repeated);
        else
            m_comboBox->getRenderer()->m_textureArrowUpHover = {};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ComboBoxRenderer::setArrowDownHoverImage(const std::string& filename, const sf::IntRect& partRect, const sf::IntRect& middlePart, bool repeated)
    {
        if (filename != "")
            m_comboBox->getRenderer()->m_textureArrowDownHover.load(filename, partRect, middlePart, repeated);
        else
            m_comboBox->getRenderer()->m_textureArrowDownHover = {};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ComboBoxRenderer::setBackgroundColor(const sf::Color& backgroundColor)
    {
        m_comboBox->m_listBox->getRenderer()->setBackgroundColor(backgroundColor);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ComboBoxRenderer::setArrowBackgroundColor(const sf::Color& color)
    {
        setArrowBackgroundColorNormal(color);
        setArrowBackgroundColorHover(color);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ComboBoxRenderer::setArrowBackgroundColorNormal(const sf::Color& color)
    {
        m_arrowBackgroundColorNormal = color;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ComboBoxRenderer::setArrowBackgroundColorHover(const sf::Color& color)
    {
        m_arrowBackgroundColorHover = color;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ComboBoxRenderer::setArrowColor(const sf::Color& color)
    {
        setArrowColorNormal(color);
        setArrowColorHover(color);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ComboBoxRenderer::setArrowColorNormal(const sf::Color& color)
    {
        m_arrowColorNormal = color;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ComboBoxRenderer::setArrowColorHover(const sf::Color& color)
    {
        m_arrowColorHover = color;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ComboBoxRenderer::setTextColor(const sf::Color& textColor)
    {
        m_comboBox->m_text.setTextColor(textColor);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ComboBoxRenderer::setBorderColor(const sf::Color& borderColor)
    {
        m_comboBox->m_listBox->getRenderer()->setBorderColor(borderColor);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ComboBoxRenderer::setTextFont(std::shared_ptr<sf::Font> font)
    {
        m_comboBox->m_font = font;
        m_comboBox->m_text.setTextFont(font);

        m_comboBox->updatePosition();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ComboBoxRenderer::setBorders(const Borders& borders)
    {
        WidgetBorders::setBorders(borders);
        m_comboBox->m_listBox->getRenderer()->setBorders({m_borders.left, 0, m_borders.right, m_borders.bottom});
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ComboBoxRenderer::setPadding(const Padding& padding)
    {
        WidgetPadding::setPadding(padding);
        m_comboBox->updateSize();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::shared_ptr<ListBoxRenderer> ComboBoxRenderer::getListBox() const
    {
        return m_comboBox->m_listBox->getRenderer();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ComboBoxRenderer::draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        sf::Vector2f position = m_comboBox->getPosition();
        sf::Vector2f size = m_comboBox->getSize();

        if (m_backgroundTexture.getData() == nullptr)
        {
            sf::RectangleShape front(size);
            front.setPosition(position);
            front.setFillColor(m_comboBox->m_listBox->getRenderer()->m_backgroundColor);
            target.draw(front, states);
        }
        else
            target.draw(m_backgroundTexture, states);

        // Draw the borders
        if (m_borders != Borders{0, 0, 0, 0})
        {
            // Draw left border
            sf::RectangleShape border({m_borders.left, size.y + m_borders.top});
            border.setPosition(position.x - m_borders.left, position.y - m_borders.top);
            border.setFillColor(m_comboBox->m_listBox->getRenderer()->m_borderColor);
            target.draw(border, states);

            // Draw top border
            border.setSize({size.x + m_borders.right, m_borders.top});
            border.setPosition(position.x, position.y - m_borders.top);
            target.draw(border, states);

            // Draw right border
            border.setSize({m_borders.right, size.y + m_borders.bottom});
            border.setPosition(position.x + size.x, position.y);
            target.draw(border, states);

            // Draw bottom border
            border.setSize({size.x + m_borders.left, m_borders.bottom});
            border.setPosition(position.x - m_borders.left, position.y + size.y);
            target.draw(border, states);
        }

        // Check if we have textures for the arrow
        if (m_textureArrowUpNormal.getData() && m_textureArrowDownNormal.getData())
        {
            // Set the arrow like it should (down when list box is invisible, up when it is visible)
            if (m_comboBox->m_listBox->isVisible())
            {
                if (m_comboBox->m_mouseHover && m_textureArrowUpHover.getData())
                    target.draw(m_textureArrowUpHover, states);
                else
                    target.draw(m_textureArrowUpNormal, states);
            }
            else
            {
                if (m_comboBox->m_mouseHover && m_textureArrowDownHover.getData())
                    target.draw(m_textureArrowDownHover, states);
                else
                    target.draw(m_textureArrowDownNormal, states);
            }
        }
        else // There are no textures for the arrow
        {
            Padding padding = getScaledPadding();
            float arrowSize = size.y - padding.top - padding.bottom;

            sf::RectangleShape arrowBackground({arrowSize, arrowSize});
            arrowBackground.setPosition({position.x + size.x - padding.right - arrowSize, position.y + padding.top});

            sf::ConvexShape arrow{3};
            if (m_comboBox->m_listBox->isVisible())
            {
                arrow.setPoint(0, {arrowBackground.getPosition().x + (arrowSize / 5), arrowBackground.getPosition().y + (arrowSize * 4/5)});
                arrow.setPoint(1, {arrowBackground.getPosition().x + (arrowSize / 2), arrowBackground.getPosition().y + (arrowSize / 5)});
                arrow.setPoint(2, {arrowBackground.getPosition().x + (arrowSize * 4/5), arrowBackground.getPosition().y + (arrowSize * 4/5)});
            }
            else // The listbox is not visible, the arrow points down
            {
                arrow.setPoint(0, {arrowBackground.getPosition().x + (arrowSize / 5), arrowBackground.getPosition().y + (arrowSize / 5)});
                arrow.setPoint(1, {arrowBackground.getPosition().x + (arrowSize / 2), arrowBackground.getPosition().y + (arrowSize * 4/5)});
                arrow.setPoint(2, {arrowBackground.getPosition().x + (arrowSize * 4/5), arrowBackground.getPosition().y + (arrowSize / 5)});
            }

            if (m_comboBox->m_mouseHover)
            {
                arrow.setFillColor(m_arrowColorHover);
                arrowBackground.setFillColor(m_arrowBackgroundColorHover);
            }
            else
            {
                arrow.setFillColor(m_arrowColorNormal);
                arrowBackground.setFillColor(m_arrowBackgroundColorNormal);
            }

            target.draw(arrowBackground, states);
            target.draw(arrow, states);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Padding ComboBoxRenderer::getScaledPadding() const
    {
        Padding padding = getPadding();
        Padding scaledPadding = padding;

        auto& texture = m_backgroundTexture;
        if (texture.getData() != nullptr)
        {
            switch (texture.getScalingType())
            {
            case Texture::ScalingType::Normal:
                scaledPadding.left = padding.left * (texture.getSize().x / texture.getImageSize().x);
                scaledPadding.right = padding.right * (texture.getSize().x / texture.getImageSize().x);
                scaledPadding.top = padding.top * (texture.getSize().y / texture.getImageSize().y);
                scaledPadding.bottom = padding.bottom * (texture.getSize().y / texture.getImageSize().y);
                break;

            case Texture::ScalingType::Horizontal:
                scaledPadding.left = padding.left * (texture.getSize().y / texture.getImageSize().y);
                scaledPadding.right = padding.right * (texture.getSize().y / texture.getImageSize().y);
                scaledPadding.top = padding.top * (texture.getSize().y / texture.getImageSize().y);
                scaledPadding.bottom = padding.bottom * (texture.getSize().y / texture.getImageSize().y);
                break;

            case Texture::ScalingType::Vertical:
                scaledPadding.left = padding.left * (texture.getSize().x / texture.getImageSize().x);
                scaledPadding.right = padding.right * (texture.getSize().x / texture.getImageSize().x);
                scaledPadding.top = padding.top * (texture.getSize().x / texture.getImageSize().x);
                scaledPadding.bottom = padding.bottom * (texture.getSize().x / texture.getImageSize().x);
                break;

            case Texture::ScalingType::NineSliceScaling:
                break;
            }
        }

        return scaledPadding;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::shared_ptr<WidgetRenderer> ComboBoxRenderer::clone(Widget* widget)
    {
        auto renderer = std::shared_ptr<ComboBoxRenderer>(new ComboBoxRenderer{*this});
        renderer->m_comboBox = static_cast<ComboBox*>(widget);
        return renderer;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
