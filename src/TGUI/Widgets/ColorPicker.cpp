/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus' Graphical User Interface
// Copyright (C) 2012-2020 Bruno Van de Velde (vdv_b@tgui.eu)
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
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    sf::Color hsv2rgb(sf::Vector3f c /**hsv*/)
    {
        /**
         * vec3 hsv2rgb(vec3 c) {
         *      vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
         *      vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
         *      return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
         * }
        **/
        auto fract = [](auto &&x) { return x - std::floor(x); };
        auto mix = [](auto &&x, auto &&y, auto &&a) { return x * (1.0f - a) + y * a; };
        auto clamp = [](auto &&x, auto &&minVal, auto &&maxVal) {
            return std::min(std::max(x, minVal), maxVal);
        };

        c.x = clamp(c.x, 0.f, 1.f);
        c.y = clamp(c.y, 0.f, 1.f);
        c.z = clamp(c.z, 0.f, 1.f);

        ///xyzw
        ///rgba
        ///vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
        float K[] = {1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0};

        ///vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
        float p[] = {std::abs(fract(c.x + K[0]) * 6.0f - K[3]),
                     std::abs(fract(c.x + K[1]) * 6.0f - K[3]),
                     std::abs(fract(c.x + K[2]) * 6.0f - K[3])};

        ///return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
        float C[] = {c.z * mix(K[0], clamp(p[0] - K[0], 0.f, 1.f), c.y),
                     c.z * mix(K[0], clamp(p[1] - K[0], 0.f, 1.f), c.y),
                     c.z * mix(K[0], clamp(p[2] - K[0], 0.f, 1.f), c.y)};

        return {static_cast<uint8_t>(clamp(static_cast<int>(255 * C[0]), 0, 255)),
                static_cast<uint8_t>(clamp(static_cast<int>(255 * C[1]), 0, 255)),
                static_cast<uint8_t>(clamp(static_cast<int>(255 * C[2]), 0, 255))};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    sf::Color calculateColor(sf::Vector2f position, float V, int A)
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

        auto length = [](sf::Vector2f x) {
            return std::sqrt(x.x * x.x + x.y * x.y);
        };

        float S = length(position);

        float H = atan2(position.y, -position.x);

        constexpr float Pi = 3.14159265359f;

        H /= 2.f * Pi;
        H += 0.5f;

        auto c = hsv2rgb({H, S, V});
        c.a = A;
        return c;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    float logInvCurve(float x)
    {
        /**
         * 0.1  - normal curve
         * e-1  - e curve (e^x-1)/(e-1)
         * +    - bigger curve
         */
        const double a = std::exp(1.0) - 1.0;
        return (std::exp(std::log(a + 1.0) * static_cast<double>(x)) - 1.0) / a;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ColorPicker::ColorPicker()
    {
        m_type = "ColorPicker";

        m_renderer = aurora::makeCopied<ColorPickerRenderer>();
        setRenderer(Theme::getDefault()->getRendererNoThrow(m_type));

        setTitleButtons(ChildWindow::TitleButton::None);
        Container::setTextSize(getGlobalTextSize());

        m_HSV = std::make_shared<sf::Shader>();
        m_HSV->loadFromMemory("#version 110\n"
                              "\n"
                              "uniform float V;\n"
                              "uniform vec2 resolution;\n"
                              "\n"
                              "const float Pi = 3.14159265359;\n"
                              "\n"
                              "vec3 hsv2rgb(vec3 c) {\n"
                              "      vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);\n"
                              "      vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);\n"
                              "      return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);\n"
                              "}\n"
                              "\n"
                              "void main( void ) {\n"
                              "      vec2 position = ( gl_FragCoord.xy / resolution.xy );\n"
                              "      vec2 p2 = position - vec2(0.5, 0.5);//<-0.5,0.5>\n"
                              "      \n"
                              "      float S = length(p2*2.0);\n"
                              "      if(S > 1.0){\n"
                              "          discard;\n"
                              "      }\n"
                              "      \n"
                              "      float H = atan(-p2.y, -p2.x);\n"
                              "      \n"
                              "      H /= 2.*Pi;\n"
                              "      H += 0.5;\n"
                              "      gl_FragColor.rgb = hsv2rgb(vec3(H, S, V));\n"
                              "      gl_FragColor.a = 1.0;\n"
                              "}", sf::Shader::Fragment);

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

        Container::add(m_canvas, "#TGUI_INTERNAL$ColorPickerCanvas#");
        Container::add(m_value, "#TGUI_INTERNAL$ColorPickerValue#");
        m_value->setValue(m_value->getMaximum());
        m_value->setVerticalScroll(true);

        Container::add(Label::create("Last:"), "#TGUI_INTERNAL$ColorPickerLabelLast#");
        Container::add(m_last, "#TGUI_INTERNAL$ColorPickerLast#");
        Container::add(Label::create("Current:"), "#TGUI_INTERNAL$ColorPickerLabelCurrent#");
        Container::add(m_current, "#TGUI_INTERNAL$ColorPickerCurrent#");

        m_current->getRenderer()->setBackgroundColor(Color::Black);
        m_last->getRenderer()->setBackgroundColor(Color::Black);

        Container::add(Button::create("Reset"), "#TGUI_INTERNAL$ColorPickerBack#");
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
            onOkPressed{other.onOkPressed},
            m_HSV{other.m_HSV}
    {
        identifyButtonsAndConnect();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ColorPicker::ColorPicker(ColorPicker &&other) :
            ChildWindow{std::move(other)},
            onColorChange{std::move(other.onColorChange)},
            onOkPressed{std::move(other.onOkPressed)},
            m_HSV{std::move(other.m_HSV)}
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
            std::swap(onOkPressed, temp.onOkPressed);
            std::swap(m_HSV, temp.m_HSV);

            identifyButtonsAndConnect();
        }

        return *this;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ColorPicker &ColorPicker::operator=(ColorPicker &&other)
    {
        if (this != &other)
        {
            onColorChange = std::move(other.onColorChange);
            onOkPressed = std::move(other.onOkPressed);
            m_canvas = std::move(other.m_canvas);
            m_red = std::move(other.m_red);
            m_green = std::move(other.m_green);
            m_blue = std::move(other.m_blue);
            m_alpha = std::move(other.m_alpha);
            m_value = std::move(other.m_value);
            m_last = std::move(other.m_last);
            m_current = std::move(other.m_current);
            m_HSV = std::move(other.m_HSV);

            ChildWindow::operator=(std::move(other));

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
        auto colorLast = m_current->getRenderer()->getBackgroundColor();
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

        pos -= getPosition() + getChildWidgetsOffset();

        if (m_canvas->isMouseOnWidget(pos))
        {
            m_colorRead = true;

            auto length = [](sf::Vector2f x) {
                return std::sqrt(x.x * x.x + x.y * x.y);
            };

            sf::Vector2f position = {(pos.x - m_canvas->getPosition().x) / m_canvas->getSize().x,
                                     (pos.y - m_canvas->getPosition().y) / m_canvas->getSize().y};

            position -= {0.5f, 0.5f};
            position *= 2.0f;

            if (length(position) > 1.f)
            {
                m_colorRead = false;
                return;
            }

            float V = m_value->getValue() / m_value->getMaximum();
            V = logInvCurve(V);

            auto c = calculateColor(position, V, m_alpha->getValue());

            m_current->getRenderer()->setBackgroundColor(c);
            onColorChange.setEnabled(false);
            m_red->setValue(c.r);
            m_green->setValue(c.g);
            m_blue->setValue(c.b);
            m_alpha->setValue(c.a);
            onColorChange.setEnabled(true);

            onColorChange.emit(this, c);
        }

    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ColorPicker::leftMouseButtonNoLongerDown()
    {
        ChildWindow::leftMouseButtonNoLongerDown();

        if (m_colorRead)
        {
            m_colorRead = false;
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ColorPicker::mouseMoved(Vector2f pos)
    {
        ChildWindow::mouseMoved(pos);

        pos -= getPosition() + getChildWidgetsOffset();

        if (m_colorRead)
        {

            sf::Vector2f position = {(pos.x - m_canvas->getPosition().x) / m_canvas->getSize().x,
                                     (pos.y - m_canvas->getPosition().y) / m_canvas->getSize().y};

            position -= {0.5f, 0.5f};
            position *= 2.0f;

            float V = m_value->getValue() / m_value->getMaximum();
            V = logInvCurve(V);

            auto c = calculateColor(position, V, m_alpha->getValue());

            m_current->getRenderer()->setBackgroundColor(c);
            onColorChange.setEnabled(false);
            m_red->setValue(c.r);
            m_green->setValue(c.g);
            m_blue->setValue(c.b);
            m_alpha->setValue(c.a);
            onColorChange.setEnabled(true);

            onColorChange.emit(this, c);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ColorPicker::rearrange()
    {
        m_canvas->setPosition(10, 10);
        m_value->setPosition("#TGUI_INTERNAL$ColorPickerCanvas#.right + 10", 10);

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
        auto labelCurrent = get<Label>("#TGUI_INTERNAL$ColorPickerLabelCurrent#");

        labelLast->setSize(70, 20);
        labelLast->setPosition("#TGUI_INTERNAL$ColorPickerLA#.left",
                               "#TGUI_INTERNAL$ColorPickerLast#.top");

        labelCurrent->setSize(70, 20);
        labelCurrent->setPosition("#TGUI_INTERNAL$ColorPickerLA#.left",
                                  "#TGUI_INTERNAL$ColorPickerCurrent#.top");

        m_last->setPosition("#TGUI_INTERNAL$ColorPickerLabelLast#.right + 10",
                            "#TGUI_INTERNAL$ColorPickerLA#.bottom + 10");
        m_current->setPosition("#TGUI_INTERNAL$ColorPickerLabelCurrent#.right + 10",
                               "#TGUI_INTERNAL$ColorPickerLast#.bottom");

#ifdef TGUI_SYSTEM_WINDOWS
        get<Button>("#TGUI_INTERNAL$ColorPickerBack#")->setPosition("#TGUI_INTERNAL$ColorPickerOK#.left - width - 10",
                                                                    "#TGUI_INTERNAL$ColorPickerCancel#.top");
        get<Button>("#TGUI_INTERNAL$ColorPickerOK#")->setPosition("#TGUI_INTERNAL$ColorPickerCancel#.left - width - 10",
                                                                  "#TGUI_INTERNAL$ColorPickerCancel#.top");
        get<Button>("#TGUI_INTERNAL$ColorPickerCancel#")->setPosition("&.iw - w - 10",
                                                                      "&.ih - h - 10");
#else
        get<Button>("#TGUI_INTERNAL$ColorPickerBack#")->setPosition("#TGUI_INTERNAL$ColorPickerCancel#.left - width - 10",
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
        else if (signalName == onOkPressed.getName())
            return onOkPressed;
        else
            return ChildWindow::getSignal(std::move(signalName));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ColorPicker::rendererChanged(const String& property)
    {
        if (property == "Button")
        {
            const auto &renderer = getSharedRenderer()->getButton();

            get<Button>("#TGUI_INTERNAL$ColorPickerBack#")->setRenderer(renderer);
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
        else
            ChildWindow::rendererChanged(property);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::unique_ptr<DataIO::Node> ColorPicker::save(SavingRenderersMap &renderers) const
    {
        auto node = ChildWindow::save(renderers);
        // Labels, Canvas, Buttons and Sliders are saved indirectly by saving the child window
        return node;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ColorPicker::load(const std::unique_ptr<DataIO::Node> &node, const LoadingRenderersMap &renderers)
    {
        // Remove the label that the MessageBox constructor creates because it will be created when loading the child window
        removeAllWidgets();

        ChildWindow::load(node, renderers);

        identifyButtonsAndConnect();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ColorPicker::identifyButtonsAndConnect()
    {
        auto redBox = get<EditBox>("#TGUI_INTERNAL$ColorPickerRedBox#");
        auto greenBox = get<EditBox>("#TGUI_INTERNAL$ColorPickerGreenBox#");
        auto blueBox = get<EditBox>("#TGUI_INTERNAL$ColorPickerBlueBox#");
        auto alphaBox = get<EditBox>("#TGUI_INTERNAL$ColorPickerAlphaBox#");

        m_canvas = get<Canvas>("#TGUI_INTERNAL$ColorPickerCanvas#");
        m_red = get<Slider>("#TGUI_INTERNAL$ColorPickerRed#");
        m_green = get<Slider>("#TGUI_INTERNAL$ColorPickerGreen#");
        m_blue = get<Slider>("#TGUI_INTERNAL$ColorPickerBlue#");
        m_alpha = get<Slider>("#TGUI_INTERNAL$ColorPickerAlpha#");
        m_value = get<Slider>("#TGUI_INTERNAL$ColorPickerValue#");
        m_last = get<Panel>("#TGUI_INTERNAL$ColorPickerLast#");
        m_current = get<Panel>("#TGUI_INTERNAL$ColorPickerCurrent#");

        auto reCalculateColor = [=]() {
            m_current->getRenderer()->setBackgroundColor(
                    {static_cast<uint8_t>(m_red->getValue()), static_cast<uint8_t>(m_green->getValue()),
                     static_cast<uint8_t>(m_blue->getValue()), static_cast<uint8_t>(m_alpha->getValue())});
            onColorChange.emit(this, getColor());
        };

        redBox->onTextChange.disconnectAll();
        redBox->setText(String(m_red->getValue()));
        redBox->onTextChange([=](const String &s) { m_red->setValue(s.toFloat()); });

        greenBox->onTextChange.disconnectAll();
        greenBox->setText(String(m_green->getValue()));
        greenBox->onTextChange([=](const String &s) { m_green->setValue(s.toFloat()); });

        blueBox->onTextChange.disconnectAll();
        blueBox->setText(String(m_blue->getValue()));
        blueBox->onTextChange([=](const String &s) { m_blue->setValue(s.toFloat()); });

        alphaBox->onTextChange.disconnectAll();
        alphaBox->setText(String(m_alpha->getValue()));
        alphaBox->onTextChange([=](const String &s) { m_alpha->setValue(s.toFloat()); });

        m_red->onValueChange.disconnectAll();
        m_red->onValueChange(
                       [=](float v) {
                           redBox->setText(String(v));
                           reCalculateColor();
                       });

        m_green->onValueChange.disconnectAll();
        m_green->onValueChange(
                         [=](float v) {
                             greenBox->setText(String(v));
                             reCalculateColor();
                         });

        m_blue->onValueChange.disconnectAll();
        m_blue->onValueChange(
                        [=](float v) {
                            blueBox->setText(String(v));
                            reCalculateColor();
                        });

        m_alpha->onValueChange.disconnectAll();
        m_alpha->onValueChange(
                         [=](float v) {
                             alphaBox->setText(String(v));
                             reCalculateColor();
                         });

        auto back = get<Button>("#TGUI_INTERNAL$ColorPickerBack#");
        back->onPress.disconnectAll();
        back->onPress([=]() {
            auto color = m_last->getRenderer()->getBackgroundColor();
            m_red->setValue(color.getRed());
            m_green->setValue(color.getGreen());
            m_blue->setValue(color.getBlue());
            m_alpha->setValue(color.getAlpha());
        });
        auto ok = get<Button>("#TGUI_INTERNAL$ColorPickerOK#");
        ok->onPress.disconnectAll();
        ok->onPress([=]() {
            auto color = m_current->getRenderer()->getBackgroundColor();
            m_last->getRenderer()->setBackgroundColor(color);
            onOkPressed.emit(this, color);

            ChildWindow::close();
        });
        auto close = get<Button>("#TGUI_INTERNAL$ColorPickerCancel#");
        close->onPress.disconnectAll();
        close->onPress([=]() {
            auto color = m_last->getRenderer()->getBackgroundColor();
            setColor(color);

            ChildWindow::close();
        });

        m_value->onValueChange.disconnectAll();
        m_value->onValueChange([=](float v) {
            auto size = m_canvas->getView().getSize();
            sf::Vertex array[4];
            array[0].position = {0, 0};
            array[1].position = {0, size.y};
            array[2].position = {size.x, 0};
            array[3].position = {size.x, size.y};

            m_canvas->clear(Color::Transparent);

#if SFML_VERSION_MAJOR == 2 && SFML_VERSION_MINOR < 4
            m_HSV->setParameter("V", logInvCurve(v / m_value->getMaximum()));
            m_HSV->setParameter("resolution", size);
            m_canvas->draw(array, 4, sf::TrianglesStrip, m_HSV.get());
#else
            m_HSV->setUniform("V", logInvCurve(v / m_value->getMaximum()));
            m_HSV->setUniform("resolution", size);
            m_canvas->draw(array, 4, sf::TriangleStrip, m_HSV.get());
#endif
            m_canvas->display();
        });
        m_value->onValueChange.emit(m_value.get(), m_value->getValue()); //< Emit signal to redraw canvas
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
