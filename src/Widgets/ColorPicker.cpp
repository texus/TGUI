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


#include <TGUI/Widgets/ColorPicker.hpp>
#include <TGUI/Widgets/EditBox.hpp>
#include <TGUI/Widgets/Label.hpp>
#include <cmath>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    const unsigned int colorWheelSize = 200;

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    static Color hsv2rgb(float h, float s, float v)
    {
        /**
         * vec3 hsv2rgb(vec3 c) {
         *      vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
         *      vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
         *      return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
         * }
        **/
        auto fract = [](float x) { return x - std::floor(x); };
        auto mix = [](float x, float y, float a) { return x * (1.0f - a) + y * a; };
        auto clamp = [](float x, float minVal, float maxVal) {
            return std::min(std::max(x, minVal), maxVal);
        };

        h = clamp(h, 0.f, 1.f);
        s = clamp(s, 0.f, 1.f);
        v = clamp(v, 0.f, 1.f);

        float K[] = {1.0f, 2.0f / 3.0f, 1.0f / 3.0f, 3.0f};

        float p[] = {std::abs(fract(h + K[0]) * 6.0f - K[3]),
                     std::abs(fract(h + K[1]) * 6.0f - K[3]),
                     std::abs(fract(h + K[2]) * 6.0f - K[3])};

        float C[] = {v * mix(K[0], clamp(p[0] - K[0], 0.f, 1.f), s),
                     v * mix(K[0], clamp(p[1] - K[0], 0.f, 1.f), s),
                     v * mix(K[0], clamp(p[2] - K[0], 0.f, 1.f), s)};

        return {static_cast<std::uint8_t>(clamp(255 * C[0], 0, 255)),
                static_cast<std::uint8_t>(clamp(255 * C[1], 0, 255)),
                static_cast<std::uint8_t>(clamp(255 * C[2], 0, 255))};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    static Color calculateColor(Vector2f position, float v, float a)
    {
        /**
         * vec2 position = ( gl_FragCoord.xy / resolution.xy );
         * vec2 p2 = position - vec2(0.5, 0.5);

         * float S = length(p2*2.0);
         * if(S > 1. && S < 1.01){
         *      discard;
         * }

         * float V = 1.;
         * float H = atan(-p2.y, -p2.x);

         * H /= 2.*Pi;
         * H += 0.5;
         * gl_FragColor.rgb = hsv2rgb(vec3(H, S, V));
         * gl_FragColor.a = 1.0;
         */

        auto length = [](Vector2f x) {
            return std::sqrt(x.x * x.x + x.y * x.y);
        };

        float s = length(position);

        float h = atan2(position.y, -position.x);

        constexpr float Pi = 3.14159265359f;

        h /= 2.f * Pi;
        h += 0.5f;

        return Color::applyOpacity(hsv2rgb(h, s, v), a);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    static float logInvCurve(float x)
    {
        /**
         * 0.1  - normal curve
         * e-1  - e curve (e^x-1)/(e-1)
         * +    - bigger curve
         */
        const double a = std::exp(1.0) - 1.0;
        return static_cast<float>((std::exp(std::log(a + 1.0) * static_cast<double>(x)) - 1.0) / a);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ColorPicker::ColorPicker(const char* typeName, bool initRenderer) :
        ChildWindow{typeName, false}
    {
        if (initRenderer)
        {
            m_renderer = aurora::makeCopied<ColorPickerRenderer>();
            setRenderer(Theme::getDefault()->getRendererNoThrow(m_type));
        }

        setTitleButtons(ChildWindow::TitleButton::None);
        Container::setTextSize(getGlobalTextSize());

        auto pixels = std::make_unique<std::uint8_t[]>(colorWheelSize * colorWheelSize * 4);
        for (unsigned int y = 0; y < colorWheelSize; ++y)
        {
            for (unsigned int x = 0; x < colorWheelSize; ++x)
            {
                const Vector2f position = {((static_cast<float>(x) / colorWheelSize) - 0.5f) * 2.f,
                                           ((static_cast<float>(y) / colorWheelSize) - 0.5f) * 2.f};
                if (std::sqrt(position.x * position.x + position.y * position.y) <= 1.f)
                {
                    const Color pixelColor = calculateColor(position, 1, 1);
                    const unsigned int pixelIndex = ((y * colorWheelSize) + x) * 4;
                    pixels[pixelIndex] = pixelColor.getRed();
                    pixels[pixelIndex+1] = pixelColor.getGreen();
                    pixels[pixelIndex+2] = pixelColor.getBlue();
                    pixels[pixelIndex+3] = pixelColor.getAlpha();
                }
                else // Pixel lies outside the circle, draw a transparent pixel
                {
                    const unsigned int pixelIndex = ((y * colorWheelSize) + x) * 4;
                    pixels[pixelIndex] = 0;
                    pixels[pixelIndex+1] = 0;
                    pixels[pixelIndex+2] = 0;
                    pixels[pixelIndex+3] = 0;
                }
            }
        }

        m_colorWheelTexture.loadFromPixelData({colorWheelSize, colorWheelSize}, pixels.get());
        m_colorWheelSprite.setTexture(m_colorWheelTexture);
        pixels = nullptr;

        Container::add(Label::create("R"), "#TGUI_INTERNAL$ColorPickerLR#");
        Container::add(m_red, "#TGUI_INTERNAL$ColorPickerRed#");
        auto redBox = EditBox::create();
        redBox->setSize(50, 20);
        redBox->setInputValidator(EditBox::Validator::Int);
        Container::add(redBox, "#TGUI_INTERNAL$ColorPickerRedBox#");

        Container::add(Label::create("G"), "#TGUI_INTERNAL$ColorPickerLG#");
        Container::add(m_green, "#TGUI_INTERNAL$ColorPickerGreen#");
        auto greenBox = EditBox::create();
        greenBox->setSize(50, 20);
        greenBox->setInputValidator(EditBox::Validator::Int);
        Container::add(greenBox, "#TGUI_INTERNAL$ColorPickerGreenBox#");

        Container::add(Label::create("B"), "#TGUI_INTERNAL$ColorPickerLB#");
        Container::add(m_blue, "#TGUI_INTERNAL$ColorPickerBlue#");
        auto blueBox = EditBox::create();
        blueBox->setSize(50, 20);
        blueBox->setInputValidator(EditBox::Validator::Int);
        Container::add(blueBox, "#TGUI_INTERNAL$ColorPickerBlueBox#");

        Container::add(Label::create("A"), "#TGUI_INTERNAL$ColorPickerLA#");
        Container::add(m_alpha, "#TGUI_INTERNAL$ColorPickerAlpha#");
        auto alphaBox = EditBox::create();
        alphaBox->setSize(50, 20);
        alphaBox->setInputValidator(EditBox::Validator::Int);
        Container::add(alphaBox, "#TGUI_INTERNAL$ColorPickerAlphaBox#");

        Container::add(m_value, "#TGUI_INTERNAL$ColorPickerValue#");
        m_value->setValue(m_value->getMaximum());
        m_value->setVerticalScroll(true);

        Container::add(Label::create("Last:"), "#TGUI_INTERNAL$ColorPickerLabelLast#");
        Container::add(m_last, "#TGUI_INTERNAL$ColorPickerLast#");
        Container::add(Label::create("Current:"), "#TGUI_INTERNAL$ColorPickerLabelCurrent#");
        Container::add(m_current, "#TGUI_INTERNAL$ColorPickerCurrent#");

        m_current->getRenderer()->setBackgroundColor(Color::Black);
        m_last->getRenderer()->setBackgroundColor(Color::Black);

        Container::add(Button::create("Reset"), "#TGUI_INTERNAL$ColorPickerReset#");
        Container::add(Button::create("OK"), "#TGUI_INTERNAL$ColorPickerOK#");
        Container::add(Button::create("Cancel"), "#TGUI_INTERNAL$ColorPickerCancel#");

        ChildWindow::setClientSize({535, 220});

        rearrange();
        identifyButtonsAndConnect();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ColorPicker::ColorPicker(const ColorPicker &other) :
            ChildWindow{other},
            onColorChange{other.onColorChange},
            onOkPress{other.onOkPress},
            m_colorWheelTexture{other.m_colorWheelTexture},
            m_colorWheelSprite{other.m_colorWheelSprite},
            m_red{nullptr},
            m_green{nullptr},
            m_blue{nullptr},
            m_alpha{nullptr},
            m_value{nullptr},
            m_last{nullptr},
            m_current{nullptr}
    {
        identifyButtonsAndConnect();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ColorPicker::ColorPicker(ColorPicker &&other) :
            ChildWindow{std::move(other)},
            onColorChange{std::move(other.onColorChange)},
            onOkPress{std::move(other.onOkPress)},
            m_colorWheelTexture{std::move(other.m_colorWheelTexture)},
            m_colorWheelSprite{std::move(other.m_colorWheelSprite)},
            m_red{nullptr},
            m_green{nullptr},
            m_blue{nullptr},
            m_alpha{nullptr},
            m_value{nullptr},
            m_last{nullptr},
            m_current{nullptr}
    {
        identifyButtonsAndConnect();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ColorPicker &ColorPicker::operator=(const ColorPicker &other)
    {
        if (this != &other)
        {
            ColorPicker temp(other);
            ChildWindow::operator=(temp);

            std::swap(onColorChange, temp.onColorChange);
            std::swap(onOkPress, temp.onOkPress);
            std::swap(m_colorWheelTexture, temp.m_colorWheelTexture);
            std::swap(m_colorWheelSprite, temp.m_colorWheelSprite);

            identifyButtonsAndConnect();
        }

        return *this;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ColorPicker &ColorPicker::operator=(ColorPicker &&other)
    {
        if (this != &other)
        {
            ChildWindow::operator=(std::move(other));

            onColorChange = std::move(other.onColorChange);
            onOkPress = std::move(other.onOkPress);
            m_colorWheelTexture = std::move(other.m_colorWheelTexture);
            m_colorWheelSprite = std::move(other.m_colorWheelSprite),

            identifyButtonsAndConnect();
        }

        return *this;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ColorPicker::Ptr ColorPicker::create(String title, Color color)
    {
        auto colorPicker = std::make_shared<ColorPicker>();
        colorPicker->setTitle(title);
        colorPicker->setColor(color);

        return colorPicker;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ColorPicker::Ptr ColorPicker::copy(ColorPicker::ConstPtr colorPicker)
    {
        if (colorPicker)
            return std::static_pointer_cast<ColorPicker>(colorPicker->clone());
        else
            return nullptr;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ColorPickerRenderer *ColorPicker::getSharedRenderer()
    {
        return aurora::downcast<ColorPickerRenderer *>(Widget::getSharedRenderer());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const ColorPickerRenderer *ColorPicker::getSharedRenderer() const
    {
        return aurora::downcast<const ColorPickerRenderer *>(Widget::getSharedRenderer());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ColorPickerRenderer *ColorPicker::getRenderer()
    {
        return aurora::downcast<ColorPickerRenderer *>(Widget::getRenderer());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const ColorPickerRenderer *ColorPicker::getRenderer() const
    {
        return aurora::downcast<const ColorPickerRenderer *>(Widget::getRenderer());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ColorPicker::setColor(const Color &color)
    {
        const auto colorLast = m_current->getRenderer()->getBackgroundColor();
        m_last->getRenderer()->setBackgroundColor(color);
        m_current->getRenderer()->setBackgroundColor(color);

        onColorChange.setEnabled(false);
        m_red->setValue(color.getRed());
        m_green->setValue(color.getGreen());
        m_blue->setValue(color.getBlue());
        m_alpha->setValue(color.getAlpha());
        onColorChange.setEnabled(true);

        if (colorLast != color)
            onColorChange.emit(this, color);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Color ColorPicker::getColor() const
    {
        return m_current->getRenderer()->getBackgroundColor();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ColorPicker::leftMousePressed(Vector2f pos)
    {
        ChildWindow::leftMousePressed(pos);

        const Vector2f originalPos = pos;
        pos -= getPosition() + getChildWidgetsOffset();

        if (FloatRect(m_colorWheelSprite.getPosition(), m_colorWheelSprite.getSize()).contains(pos))
        {
            m_colorRead = true;

            auto length = [](Vector2f vec) {
                return std::sqrt(vec.x * vec.x + vec.y * vec.y);
            };

            Vector2f position = {(pos.x - m_colorWheelSprite.getPosition().x) / m_colorWheelSprite.getSize().x,
                                 (pos.y - m_colorWheelSprite.getPosition().y) / m_colorWheelSprite.getSize().y};
            position -= {0.5f, 0.5f};
            position *= 2.0f;

            if (length(position) > 1.f)
            {
                m_colorRead = false;
                return;
            }

            mouseMoved(originalPos);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ColorPicker::leftMouseButtonNoLongerDown()
    {
        ChildWindow::leftMouseButtonNoLongerDown();

        m_colorRead = false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ColorPicker::mouseMoved(Vector2f pos)
    {
        ChildWindow::mouseMoved(pos);

        pos -= getPosition() + getChildWidgetsOffset();

        if (m_colorRead)
        {
            Vector2f position = {(pos.x - m_colorWheelSprite.getPosition().x) / m_colorWheelSprite.getSize().x,
                                 (pos.y - m_colorWheelSprite.getPosition().y) / m_colorWheelSprite.getSize().y};
            position -= {0.5f, 0.5f};
            position *= 2.0f;

            const float v = logInvCurve(m_value->getValue() / m_value->getMaximum());
            const auto c = calculateColor(position, v, m_alpha->getValue() / 255.f);

            m_current->getRenderer()->setBackgroundColor(c);
            onColorChange.setEnabled(false);
            m_red->setValue(static_cast<float>(c.getRed()));
            m_green->setValue(static_cast<float>(c.getGreen()));
            m_blue->setValue(static_cast<float>(c.getBlue()));
            m_alpha->setValue(static_cast<float>(c.getAlpha()));
            onColorChange.setEnabled(true);

            onColorChange.emit(this, c);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ColorPicker::rearrange()
    {
        m_colorWheelSprite.setPosition({10, 10});
        m_value->setPosition(2 * m_colorWheelSprite.getPosition().x + colorWheelSize, 10);

        get<Label>("#TGUI_INTERNAL$ColorPickerLR#")->setPosition("#TGUI_INTERNAL$ColorPickerValue#.right + 10", 10);
        get<Label>("#TGUI_INTERNAL$ColorPickerLG#")->setPosition("#TGUI_INTERNAL$ColorPickerValue#.right + 10",
                                                                 "#TGUI_INTERNAL$ColorPickerLR#.bottom + 10");
        get<Label>("#TGUI_INTERNAL$ColorPickerLB#")->setPosition("#TGUI_INTERNAL$ColorPickerValue#.right + 10",
                                                                 "#TGUI_INTERNAL$ColorPickerLG#.bottom + 10");
        get<Label>("#TGUI_INTERNAL$ColorPickerLA#")->setPosition("#TGUI_INTERNAL$ColorPickerValue#.right + 10",
                                                                 "#TGUI_INTERNAL$ColorPickerLB#.bottom + 10");

        m_red->setPosition("#TGUI_INTERNAL$ColorPickerLR#.right + 10", "#TGUI_INTERNAL$ColorPickerLR#.top");
        m_green->setPosition("#TGUI_INTERNAL$ColorPickerLR#.right + 10", "#TGUI_INTERNAL$ColorPickerLG#.top");
        m_blue->setPosition("#TGUI_INTERNAL$ColorPickerLR#.right + 10", "#TGUI_INTERNAL$ColorPickerLB#.top");
        m_alpha->setPosition("#TGUI_INTERNAL$ColorPickerLR#.right + 10", "#TGUI_INTERNAL$ColorPickerLA#.top");

        get<EditBox>("#TGUI_INTERNAL$ColorPickerRedBox#")->setPosition("#TGUI_INTERNAL$ColorPickerRed#.right + 10",
                                                                       "#TGUI_INTERNAL$ColorPickerRed#.top");
        get<EditBox>("#TGUI_INTERNAL$ColorPickerGreenBox#")->setPosition("#TGUI_INTERNAL$ColorPickerGreen#.right + 10",
                                                                         "#TGUI_INTERNAL$ColorPickerGreen#.top");
        get<EditBox>("#TGUI_INTERNAL$ColorPickerBlueBox#")->setPosition("#TGUI_INTERNAL$ColorPickerBlue#.right + 10",
                                                                        "#TGUI_INTERNAL$ColorPickerBlue#.top");
        get<EditBox>("#TGUI_INTERNAL$ColorPickerAlphaBox#")->setPosition("#TGUI_INTERNAL$ColorPickerAlpha#.right + 10",
                                                                         "#TGUI_INTERNAL$ColorPickerAlpha#.top");

        auto labelLast = get<Label>("#TGUI_INTERNAL$ColorPickerLabelLast#");
        labelLast->setSize(70, 20);
        labelLast->setPosition("#TGUI_INTERNAL$ColorPickerLA#.left",
                               "#TGUI_INTERNAL$ColorPickerLast#.top");

        auto labelCurrent = get<Label>("#TGUI_INTERNAL$ColorPickerLabelCurrent#");
        labelCurrent->setSize(70, 20);
        labelCurrent->setPosition("#TGUI_INTERNAL$ColorPickerLA#.left",
                                  "#TGUI_INTERNAL$ColorPickerCurrent#.top");

        m_last->setPosition("#TGUI_INTERNAL$ColorPickerLabelLast#.right + 10",
                            "#TGUI_INTERNAL$ColorPickerLA#.bottom + 10");
        m_current->setPosition("#TGUI_INTERNAL$ColorPickerLabelCurrent#.right + 10",
                               "#TGUI_INTERNAL$ColorPickerLast#.bottom");

#ifdef TGUI_SYSTEM_WINDOWS
        get<Button>("#TGUI_INTERNAL$ColorPickerReset#")->setPosition("#TGUI_INTERNAL$ColorPickerOK#.left - width - 10",
                                                                     "#TGUI_INTERNAL$ColorPickerCancel#.top");
        get<Button>("#TGUI_INTERNAL$ColorPickerOK#")->setPosition("#TGUI_INTERNAL$ColorPickerCancel#.left - width - 10",
                                                                  "#TGUI_INTERNAL$ColorPickerCancel#.top");
        get<Button>("#TGUI_INTERNAL$ColorPickerCancel#")->setPosition("&.iw - w - 10",
                                                                      "&.ih - h - 10");
#else
        get<Button>("#TGUI_INTERNAL$ColorPickerReset#")->setPosition("#TGUI_INTERNAL$ColorPickerCancel#.left - width - 10",
                                                                     "#TGUI_INTERNAL$ColorPickerOK#.top");
        get<Button>("#TGUI_INTERNAL$ColorPickerCancel#")->setPosition("#TGUI_INTERNAL$ColorPickerOK#.left - width - 10",
                                                                      "#TGUI_INTERNAL$ColorPickerOK#.top");
        get<Button>("#TGUI_INTERNAL$ColorPickerOK#")->setPosition("&.iw - w - 10",
                                                                  "&.ih - h - 10");
#endif

    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Signal& ColorPicker::getSignal(String signalName)
    {
        if (signalName == onColorChange.getName())
            return onColorChange;
        else if (signalName == onOkPress.getName())
            return onOkPress;
        else
            return ChildWindow::getSignal(std::move(signalName));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ColorPicker::rendererChanged(const String& property)
    {
        if (property == "Button")
        {
            const auto &renderer = getSharedRenderer()->getButton();

            get<Button>("#TGUI_INTERNAL$ColorPickerReset#")->setRenderer(renderer);
            get<Button>("#TGUI_INTERNAL$ColorPickerOk#")->setRenderer(renderer);
            get<Button>("#TGUI_INTERNAL$ColorPickerCancel#")->setRenderer(renderer);
        }
        else if (property == "Label")
        {
            const auto &renderer = getSharedRenderer()->getLabel();

            for (const auto &it : getWidgets())
            {
                auto label = std::dynamic_pointer_cast<Label>(it);
                if (label)
                {
                    label->setRenderer(renderer);
                }
            }
        }
        else if (property == "Slider")
        {
            const auto &renderer = getSharedRenderer()->getSlider();

            m_red->setRenderer(renderer);
            m_green->setRenderer(renderer);
            m_blue->setRenderer(renderer);
            m_alpha->setRenderer(renderer);

            m_value->setRenderer(renderer);
        }
        else if ((property == "Opacity") || (property == "OpacityDisabled"))
        {
            Widget::rendererChanged(property);
            m_colorWheelSprite.setOpacity(m_opacityCached);
            ChildWindow::rendererChanged(property);
        }
        else
            ChildWindow::rendererChanged(property);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::unique_ptr<DataIO::Node> ColorPicker::save(SavingRenderersMap &renderers) const
    {
        // Labels, buttons and sliders are saved indirectly by saving the child window
        return ChildWindow::save(renderers);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ColorPicker::load(const std::unique_ptr<DataIO::Node> &node, const LoadingRenderersMap &renderers)
    {
        // Remove the widgets that the ColorPicker constructor creates because they will be created when loading the child window
        removeAllWidgets();

        ChildWindow::load(node, renderers);

        identifyButtonsAndConnect();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ColorPicker::draw(BackendRenderTargetBase& target, RenderStates states) const
    {
        ChildWindow::draw(target, states);

        states.transform.translate(getChildWidgetsOffset());
        target.drawSprite(states, m_colorWheelSprite);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ColorPicker::identifyButtonsAndConnect()
    {
        auto redBox = get<EditBox>("#TGUI_INTERNAL$ColorPickerRedBox#");
        auto greenBox = get<EditBox>("#TGUI_INTERNAL$ColorPickerGreenBox#");
        auto blueBox = get<EditBox>("#TGUI_INTERNAL$ColorPickerBlueBox#");
        auto alphaBox = get<EditBox>("#TGUI_INTERNAL$ColorPickerAlphaBox#");

        m_red = get<Slider>("#TGUI_INTERNAL$ColorPickerRed#");
        m_green = get<Slider>("#TGUI_INTERNAL$ColorPickerGreen#");
        m_blue = get<Slider>("#TGUI_INTERNAL$ColorPickerBlue#");
        m_alpha = get<Slider>("#TGUI_INTERNAL$ColorPickerAlpha#");
        m_value = get<Slider>("#TGUI_INTERNAL$ColorPickerValue#");
        m_last = get<Panel>("#TGUI_INTERNAL$ColorPickerLast#");
        m_current = get<Panel>("#TGUI_INTERNAL$ColorPickerCurrent#");

        auto recalculateColor = [this]() {
            m_current->getRenderer()->setBackgroundColor(
                    {static_cast<std::uint8_t>(m_red->getValue()), static_cast<std::uint8_t>(m_green->getValue()),
                     static_cast<std::uint8_t>(m_blue->getValue()), static_cast<std::uint8_t>(m_alpha->getValue())});
            onColorChange.emit(this, getColor());
        };

        redBox->onTextChange.disconnectAll();
        redBox->setText(String(m_red->getValue()));
        redBox->onTextChange([this](const String &s) { m_red->setValue(s.toFloat()); });

        greenBox->onTextChange.disconnectAll();
        greenBox->setText(String(m_green->getValue()));
        greenBox->onTextChange([this](const String &s) { m_green->setValue(s.toFloat()); });

        blueBox->onTextChange.disconnectAll();
        blueBox->setText(String(m_blue->getValue()));
        blueBox->onTextChange([this](const String &s) { m_blue->setValue(s.toFloat()); });

        alphaBox->onTextChange.disconnectAll();
        alphaBox->setText(String(m_alpha->getValue()));
        alphaBox->onTextChange([this](const String &s) { m_alpha->setValue(s.toFloat()); });

        m_red->onValueChange.disconnectAll();
        m_red->onValueChange(
                       [=](float v) {
                           redBox->setText(String(v));
                           recalculateColor();
                       });

        m_green->onValueChange.disconnectAll();
        m_green->onValueChange(
                         [=](float v) {
                             greenBox->setText(String(v));
                             recalculateColor();
                         });

        m_blue->onValueChange.disconnectAll();
        m_blue->onValueChange(
                        [=](float v) {
                            blueBox->setText(String(v));
                            recalculateColor();
                        });

        m_alpha->onValueChange.disconnectAll();
        m_alpha->onValueChange(
                         [=](float v) {
                             alphaBox->setText(String(v));
                             recalculateColor();
                         });

        auto reset = get<Button>("#TGUI_INTERNAL$ColorPickerReset#");
        reset->onPress.disconnectAll();
        reset->onPress([this]() {
            const auto color = m_last->getRenderer()->getBackgroundColor();
            m_red->setValue(color.getRed());
            m_green->setValue(color.getGreen());
            m_blue->setValue(color.getBlue());
            m_alpha->setValue(color.getAlpha());
        });
        auto ok = get<Button>("#TGUI_INTERNAL$ColorPickerOK#");
        ok->onPress.disconnectAll();
        ok->onPress([this]() {
            const auto color = m_current->getRenderer()->getBackgroundColor();
            m_last->getRenderer()->setBackgroundColor(color);
            onOkPress.emit(this, color);

            close();
        });

        auto closeButton = get<Button>("#TGUI_INTERNAL$ColorPickerCancel#");
        closeButton->onPress.disconnectAll();
        closeButton->onPress([this]{
            const auto color = m_last->getRenderer()->getBackgroundColor();
            setColor(color);

            close();
        });

        auto valueChangeFunc = [this](float value){
            const std::uint8_t factor = static_cast<std::uint8_t>(255 * logInvCurve(value / m_value->getMaximum()));
            m_colorWheelTexture.setColor({factor, factor, factor});
            m_colorWheelSprite.setTexture(m_colorWheelTexture);
        };

        m_value->onValueChange.disconnectAll();
        m_value->onValueChange(valueChangeFunc);
        valueChangeFunc(m_value->getValue());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
