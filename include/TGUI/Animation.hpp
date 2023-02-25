/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus' Graphical User Interface
// Copyright (C) 2012-2023 Bruno Van de Velde (vdv_b@tgui.eu)
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


#ifndef TGUI_ANIMATION_HPP
#define TGUI_ANIMATION_HPP

#include <TGUI/Layout.hpp>
#include <TGUI/Duration.hpp>

#if !TGUI_EXPERIMENTAL_USE_STD_MODULE
    #include <functional>
    #include <memory>
#endif

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

TGUI_MODULE_EXPORT namespace tgui
{
    class Widget;

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @brief Type of effect to show/hide widget
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    enum class ShowEffectType
    {
        Fade,          //!< Fade widget in or out
        Scale,         //!< Shrink to the center of the widget to hide or grow from its center to show
        SlideToRight,  //!< Slide to the right to hide or from left to show
        SlideToLeft,   //!< Slide to the left to hide or from right to show
        SlideToBottom, //!< Slide to the bottom to hide or from top to show
        SlideToTop,    //!< Slide to the top to hide or from bottom to show

        SlideFromLeft = SlideToRight, //!< Slide from left to show or to the right to hide
        SlideFromRight = SlideToLeft, //!< Slide from right to show or to the left to hide
        SlideFromTop = SlideToBottom, //!< Slide from top to show or to the bottom to hide
        SlideFromBottom = SlideToTop  //!< Slide from bottom to show or to the top to hide
    };


    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @brief Type of animation
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    enum class AnimationType
    {
        Move,       //!< Position is being changed
        Resize,     //!< Size is being changed
        Opacity,    //!< Opacity is being changed
    };


    namespace priv
    {
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        class TGUI_API Animation
        {
        public:

            // Move constructor has to be explicitly declared since this class has a destructor
            Animation(const Animation&) = default;
            Animation(Animation&&) = default;
            Animation& operator=(const Animation&) = default;
            Animation& operator=(Animation&&) = default;
            virtual ~Animation() = default;

            TGUI_NODISCARD AnimationType getType() const;

            virtual bool update(Duration elapsedTime) = 0;
            virtual void finish();

        protected:
            Animation(AnimationType type, std::shared_ptr<Widget> widget, Duration duration, std::function<void()> finishedCallback);

        protected:
            AnimationType m_type;
            std::shared_ptr<Widget> m_widget;

            Duration m_totalDuration;
            Duration m_elapsedTime;

            std::function<void()> m_finishedCallback;
        };

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        class TGUI_API MoveAnimation : public Animation
        {
        public:
            MoveAnimation(std::shared_ptr<Widget> widget, Vector2f start, Layout2d end, Duration duration, std::function<void()> finishedCallback = nullptr);

            bool update(Duration elapsedTime) override;

            void finish() override;

        private:
            Vector2f m_startPos;
            Layout2d m_endPos;
        };

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        class TGUI_API ResizeAnimation : public Animation
        {
        public:
            ResizeAnimation(std::shared_ptr<Widget> widget, Vector2f start, Layout2d end, Duration duration, std::function<void()> finishedCallback = nullptr);

            bool update(Duration elapsedTime) override;

            void finish() override;

        private:
            Vector2f m_startSize;
            Layout2d m_endSize;
        };

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        class TGUI_API FadeAnimation : public Animation
        {
        public:
            FadeAnimation(std::shared_ptr<Widget> widget, float start, float end, Duration duration, std::function<void()> finishedCallback = nullptr);

            bool update(Duration elapsedTime) override;

            void finish() override;

        private:
            float m_startOpacity;
            float m_endOpacity;
        };

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    } // namespace priv
} // namespace tgui

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif // TGUI_ANIMATION_HPP
