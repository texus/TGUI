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


#include <TGUI/Widgets/devel/RichTextLabel.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace
{
    struct Chunk
    {
        Chunk()
        {
            style = sf::Text::Regular;
            color = sf::Color::White;
            endsInNewline = false;
        }

        sf::String text;
        unsigned int style;
        sf::Color color;
        bool endsInNewline;
    };

    void newChunk(std::vector<Chunk>& chunks, Chunk*& currentChunk, Chunk& lastChunk)
    {
        chunks.push_back(Chunk());
        currentChunk = &(chunks[chunks.size() - 1]);

        // only carry information over if currentChunk and lastChunk aren't the same
        // this only happens for the first chunk, but it causes random errors
        if (chunks.size() > 2)
        {
            currentChunk->style = lastChunk.style;
            currentChunk->color = lastChunk.color;
        }
    }

    void processFormatting(Chunk& lastChunk, Chunk* currentChunk, sf::Text::Style style)
    {
        if ((lastChunk.style & style) != 0)
        {
            currentChunk->style ^= style;
        }
        else
        {
            currentChunk->style |= style;
        }
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    RichTextLabel::RichTextLabel()
    {
        m_callback.widgetType = "RichTextLabel";

        m_background.setFillColor(sf::Color::Transparent);

        m_colors["default"] = sf::Color::White;
        m_colors["black"] = sf::Color::Black;
        m_colors["blue"] = sf::Color::Blue;
        m_colors["cyan"] = sf::Color::Cyan;
        m_colors["green"] = sf::Color::Green;
        m_colors["magenta"] = sf::Color::Magenta;
        m_colors["red"] = sf::Color::Red;
        m_colors["white"] = sf::Color::White;
        m_colors["yellow"] = sf::Color::Yellow;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    RichTextLabel::RichTextLabel(const sf::String& text, unsigned int textSize) :
        RichTextLabel{}
    {
        m_textSize = textSize;
        setText(text);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    RichTextLabel::Ptr RichTextLabel::copy(RichTextLabel::ConstPtr label)
    {
        if (label)
            return std::static_pointer_cast<RichTextLabel>(label->clone());
        else
            return nullptr;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void RichTextLabel::setPosition(const Layout2d& position)
    {
        Widget::setPosition(position);
        m_background.setPosition(getPosition());
        setText(m_string);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void RichTextLabel::setSize(const Layout2d&)
    {
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void RichTextLabel::setFont(const Font& font)
    {
        Widget::setFont(font);
        setText(m_string);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void RichTextLabel::setText(const sf::String& string)
    {
        m_string = string;

        if (!m_font)
            return;

        m_texts.clear();

        std::vector<Chunk> chunks;
        chunks.push_back(Chunk());

        Chunk* currentChunk = &(chunks[0]);
        bool escaped = false;

        for (std::size_t i = 0; i < m_string.getSize(); ++i)
        {
            Chunk& lastChunk = *currentChunk;

            switch (m_string[i])
            {
                case '~': // italics
                {
                    if (escaped)
                    {
                        currentChunk->text += m_string[i];
                        escaped = false;
                        break;
                    }

                    newChunk(chunks, currentChunk, lastChunk);

                    processFormatting(lastChunk, currentChunk, sf::Text::Italic);

                    currentChunk->color = lastChunk.color;
                    break;
                }
                case '*': // bold
                {
                    if (escaped)
                    {
                        currentChunk->text += m_string[i];
                        escaped = false;
                        break;
                    }

                    newChunk(chunks, currentChunk, lastChunk);

                    processFormatting(lastChunk, currentChunk, sf::Text::Bold);

                    currentChunk->color = lastChunk.color;
                    break;
                }
                case '_': // underline
                {
                    if (escaped)
                    {
                        currentChunk->text += m_string[i];
                        escaped = false;
                        break;
                    }

                    newChunk(chunks, currentChunk, lastChunk);


                    processFormatting(lastChunk, currentChunk, sf::Text::Underlined);

                    currentChunk->color = lastChunk.color;
                    break;
                }
                case '#': // color
                {
                    if (escaped)
                    {
                        currentChunk->text += m_string[i];
                        escaped = false;
                        break;
                    }

                    // seek forward until the next whitespace
                    std::size_t length = 0;
                    while (!isspace(m_string[++i]))
                        ++length;

                    newChunk(chunks, currentChunk, lastChunk);
                    currentChunk->color = getColor(m_string.substring(i + 1, length));;
                    break;

                }
                case '\\': // escape sequence for escaping formatting characters
                {
                    if (i < m_string.getSize())
                    {
                        switch (m_string[i + 1])
                        {
                            case '~':
                            case '*':
                            case '_':
                            case '#':
                            {
                                escaped = true;
                                break;
                            }
                            default:
                                break;
                        }
                    }

                    if (!escaped)
                    {
                        currentChunk->text += m_string[i];
                    }

                    break;
                }
                case '\n': // make a new chunk in the case of a newline
                {
                    currentChunk->endsInNewline = true;
                    newChunk(chunks, currentChunk, lastChunk);
                    break;
                }
                default:
                {
                    escaped = false;
                    currentChunk->text += m_string[i];
                    break;
                }
            }
        }

        sf::String totalString = "";
        for (std::size_t i = 0; i < chunks.size(); ++i)
        {
            if (!chunks[i].endsInNewline && chunks[i].text.getSize() == 0)
            {
                continue;
            }

            totalString += chunks[i].text + (chunks[i].endsInNewline ? "\n" : "");
        }

        sf::Text text;
        text.setString(totalString);
        text.setCharacterSize(m_textSize);
        text.setPosition(getPosition());
        text.setFont(*m_font);

        m_size = {text.getLocalBounds().left + text.getLocalBounds().width, text.getLocalBounds().top + text.getLocalBounds().height};
        m_background.setSize(getSize());

        std::size_t cursor = 0;
        Chunk* lastChunk = nullptr;
        for (std::size_t i = 0; i < chunks.size(); ++i)
        {
            sf::Text tempText;
#if SFML_VERSION_MAJOR > 2 || (SFML_VERSION_MAJOR == 2 && SFML_VERSION_MINOR >= 4)
            tempText.setFillColor(chunks[i].color);
#else
            tempText.setColor(chunks[i].color);
#endif
            tempText.setString(chunks[i].text);
            tempText.setStyle(chunks[i].style);
            tempText.setCharacterSize(m_textSize);
            tempText.setFont(*m_font);

            tempText.setPosition(text.findCharacterPos(cursor));

            if (lastChunk != nullptr && lastChunk->endsInNewline)
            {
                tempText.setPosition(0, tempText.getPosition().y + m_font->getLineSpacing(m_textSize));
                ++cursor;
            }

            m_texts.push_back(tempText);
            cursor += chunks[i].text.getSize();
            lastChunk = &chunks[i];
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void RichTextLabel::setTextSize(unsigned int size)
    {
        m_textSize = size;
        setText(m_string);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int RichTextLabel::getTextSize() const
    {
        return m_textSize;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void RichTextLabel::setBackgroundColor(const sf::Color& color)
    {
        m_background.setFillColor(color);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const sf::Color& RichTextLabel::getBackgroundColor() const
    {
        return m_background.getFillColor();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void RichTextLabel::addColor(const sf::String& name, const sf::Color& color)
    {
        m_colors[name] = color;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void RichTextLabel::addColor(const sf::String& name, unsigned int argbHex)
    {
        m_colors[name] = getColor(argbHex);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    sf::Color RichTextLabel::getColor(const sf::String& source) const
    {
        std::map<sf::String, sf::Color>::const_iterator result = m_colors.find(source);
        if (result == m_colors.end())
        {
            unsigned int hex = 0x0;
            if (!(std::istringstream(source) >> std::hex >> hex))
            {
                // Error parsing; return default
                return sf::Color::White;
            };

            return getColor(hex);
        }

        return result->second;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    sf::Color RichTextLabel::getColor(unsigned int argbHex) const
    {
        argbHex |= 0xff000000;
        return sf::Color(argbHex >> 16 & 0xFF, argbHex >> 8 & 0xFF, argbHex >> 0 & 0xFF, argbHex >> 24 & 0xFF);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void RichTextLabel::draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        if (m_background.getFillColor() != sf::Color::Transparent)
            target.draw(m_background, states);

        for(std::size_t i = 0; i < m_texts.size(); ++i)
            target.draw(m_texts[i], states);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
