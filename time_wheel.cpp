#include "time_wheel.h"
#include <iostream>

TimeWheel::TimeWheel(uint32_t scales, uint32_t scale_unit_ms,
                     const std::string& name) :
    m_strName(name),
    m_curIndex(0),
    m_scales(scales),
    m_scaleUnitMs(scale_unit_ms),
    m_slots(scales),
    m_pGreaterLevelTW(nullptr),
    m_pLessLevelTW(nullptr)
{
}

int64_t TimeWheel::get_current_time() const
{
    int64_t time = m_curIndex * m_scaleUnitMs;
    if (m_pLessLevelTW != nullptr)
    {
        time += m_pLessLevelTW->get_current_time();
    }

    return time;
}

void TimeWheel::add_timer(TimerPtr timer)
{
    int64_t less_tw_time = 0;
    if (m_pLessLevelTW != nullptr)
    {
        less_tw_time = m_pLessLevelTW->get_current_time();
    }

    // for cur time wheel, time will change diff(ms)
    // we need check whether cur time wheel scale slot change
    int64_t diff = timer->when_ms() + less_tw_time - get_now_time_stamp();

    // If the difference is greater than scale unit, the timer can be added into
    // the current time wheel.
    if (diff >= m_scaleUnitMs)
    {
        // check remaining time whether greater than 5 ms
        size_t n = (m_curIndex + (diff + 5) / m_scaleUnitMs) % m_scales;
        m_slots[n].push_back(timer);
        return;
    }

    // If the difference is less than scale uint, the timer should be added into
    // less level time wheel.
    if (m_pLessLevelTW != nullptr)
    {
        m_pLessLevelTW->add_timer(timer);
        return;
    }

    // If the current time wheel is the least level, the timer can be added into
    // the current time wheel.
    m_slots[m_curIndex].push_back(timer);
}

void TimeWheel::increase()
{
    // Increase the time wheel.
    ++m_curIndex;
    if (m_curIndex < m_scales)
    {
        return;
    }

    // If the time wheel is full, the greater level time wheel should be
    // increased. The timers in the current slot of the greater level time wheel
    // should be moved into the less level time wheel.
    m_curIndex = m_curIndex % m_scales;
    if (m_pGreaterLevelTW != nullptr)
    {
        m_pGreaterLevelTW->increase();
        std::list<TimerPtr> slot = std::move(m_pGreaterLevelTW->get_and_clear_current_slot());
        for (TimerPtr timer : slot)
        {
            add_timer(timer);
        }
    }
}

std::list<TimerPtr> TimeWheel::get_and_clear_current_slot()
{
    std::list<TimerPtr> slot;
    slot = std::move(m_slots[m_curIndex]);
    return slot;
}
