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


#include <TGUI/Timer.hpp>

#if !TGUI_EXPERIMENTAL_USE_STD_MODULE
    #include <algorithm>
#endif

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    namespace
    {
        // This struct is used to be able to use make_shared while still keeping the constructor private.
        struct MakeSharedTimer : public Timer {};

        // Helper function to reduce duplicate code in create function
        template <typename Func>
        std::shared_ptr<Timer> createTimer(const Func& callback, Duration interval, bool enable)
        {
            auto timer = std::make_shared<MakeSharedTimer>();
            timer->setInterval(interval);
            timer->setCallback(callback);

            if (enable)
                timer->setEnabled(true);

            return timer;
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::vector<std::shared_ptr<Timer>> Timer::m_activeTimers;

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::shared_ptr<Timer> Timer::create(const std::function<void()>& callback, Duration interval, bool enable)
    {
        auto timer = createTimer(callback, interval, enable);
        timer->m_repeats = true;
        return timer;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::shared_ptr<Timer> Timer::create(const std::function<void(std::shared_ptr<Timer>)>& callback, Duration interval, bool enable)
    {
        auto timer = createTimer(callback, interval, enable);
        timer->m_repeats = true;
        return timer;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Timer::scheduleCallback(const std::function<void()>& callback, Duration interval)
    {
        auto timer = createTimer(callback, interval, true);
        timer->m_repeats = false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Timer::setInterval(Duration interval)
    {
        if (m_interval == interval)
            return;

        m_interval = interval;
        if (m_enabled)
            restart();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Duration Timer::getInterval() const
    {
        return m_interval;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Timer::setEnabled(bool enabled)
    {
        if (m_enabled != enabled)
        {
            if (enabled)
                m_activeTimers.push_back(shared_from_this());
            else
            {
                const auto it = std::find(m_activeTimers.begin(), m_activeTimers.end(), shared_from_this());
                if (it != m_activeTimers.end())
                    m_activeTimers.erase(it);
            }
        }

        m_enabled = enabled;
        if (enabled)
            restart();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Timer::isEnabled() const
    {
        return m_enabled;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Timer::setCallback(const std::function<void()>& callback)
    {
        m_callback = callback;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Timer::setCallback(const std::function<void(std::shared_ptr<Timer>)>& callback)
    {
        m_callback = TGUI_LAMBDA_CAPTURE_EQ_THIS{ callback(shared_from_this()); };
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Timer::restart()
    {
        m_remainingDuration = m_interval;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Timer::updateTime(Duration elapsedTime)
    {
        if (m_activeTimers.empty())
            return false;

        bool timerTriggered = false;
        auto activeTimers = m_activeTimers; // The callback could start and stop timers, so we need to operate on a copy
        for (auto& timer : activeTimers)
        {
            if (timer->m_remainingDuration > elapsedTime)
                timer->m_remainingDuration -= elapsedTime;
            else
            {
                timerTriggered = true;
                timer->m_callback();
                timer->m_remainingDuration = timer->m_interval;

                if (!timer->m_repeats)
                    timer->setEnabled(false);
            }
        }

        return timerTriggered;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Optional<Duration> Timer::getNextScheduledTime()
    {
        // If there are no active timers then return an empty object
        if (m_activeTimers.empty())
            return {};

        // Find the timer with the smallest remaining duration
        Duration minDuration = m_activeTimers[0]->m_remainingDuration;
        for (std::size_t i = 1; i < m_activeTimers.size(); ++i)
        {
            if (m_activeTimers[i]->m_remainingDuration < minDuration)
                minDuration = m_activeTimers[i]->m_remainingDuration;
        }

        return minDuration;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Timer::clearTimers()
    {
        m_activeTimers.clear();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
