#include "time_wheel_scheduler.h"
#include "time_wheel.h"
#include "timer.h"
#include <cstdint>

TimeWheelManager::TimeWheelManager() :
    m_timerStepMs(TIMESTEPMS), m_stop(false)
{
}

void TimeWheelManager::init()
{
    TimeWheelManager* timeWhellMgr = TimeWheelManager::get_instance();

    // Minute time wheel. 60 scales, 1 scale = 60 * 1000 ms.
    timeWhellMgr->append_time_wheel(1 << 8, (1 << 8) * (1 << 8) * 10, "Level_2");
    // Second time wheel. 60 scales, 1 scale = 1000 ms.
    timeWhellMgr->append_time_wheel(1 << 8, (1 << 8) * 10, "Level_1");
    // Millisecond time wheel. 1000/timer_step_ms scales, 1 scale = timer_step ms.
    timeWhellMgr->append_time_wheel(1 << 8, 10, "Level_0");
}
bool TimeWheelManager::start()
{
    if (m_timeWheels.empty())
    {
        return false;
    }

    // m_thread = std::thread(std::bind(&TimeWheelManager::run, this));
    // m_preRunTime = get_now_time_stamp();
    m_preRunTime = 0;

    return true;
}

void TimeWheelManager::run_once()
{
    if (m_preRunTime == 0)
        m_preRunTime = get_now_time_stamp();
    // fix time to fit every time_step
    int64_t next_time = m_preRunTime + m_timerStepMs;
    int64_t curTime = get_now_time_stamp();
    std::this_thread::sleep_for(std::chrono::milliseconds(next_time - curTime));
    m_preRunTime = next_time;

    TimeWheelPtr leastTimeWheel = get_least_time_wheel();
    leastTimeWheel->increase();
    std::list<TimerPtr> slot = std::move(leastTimeWheel->get_and_clear_current_slot());
    for (const TimerPtr& timer : slot)
    {
        auto it = m_cancelTimerId.find(timer->id());
        if (it != m_cancelTimerId.end())
        {
            m_cancelTimerId.erase(it);
            continue;
        }

        timer->run();
        if (timer->decrease_times() != 0)
        {
            timer->update_when_time();
            get_greatest_time_wheel()->add_timer(timer);
        }
    }
}

void TimeWheelManager::run()
{
    int64_t cur_time = 0;
    int64_t next_time = 0;
    int64_t pre_time = get_now_time_stamp();
    while (true)
    {
        // fix time to fit every time_step
        next_time = pre_time + m_timerStepMs;
        std::this_thread::sleep_for(std::chrono::milliseconds(next_time - get_now_time_stamp()));
        pre_time = next_time;
        // std::lock_guard<std::mutex> lock(m_mutex);
        if (m_stop)
        {
            break;
        }

        TimeWheelPtr leastTimeWheel = get_least_time_wheel();
        leastTimeWheel->increase();
        std::list<TimerPtr> slot = std::move(leastTimeWheel->get_and_clear_current_slot());
        for (const TimerPtr& timer : slot)
        {
            auto it = m_cancelTimerId.find(timer->id());
            if (it != m_cancelTimerId.end())
            {
                m_cancelTimerId.erase(it);
                continue;
            }

            timer->run();
            if (timer->decrease_times() != 0)
            {
                timer->update_when_time();
                get_greatest_time_wheel()->add_timer(timer);
            }
        }
    }
}

void TimeWheelManager::stop()
{
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_stop = true;
    }

    m_thread.join();
}

TimeWheelPtr TimeWheelManager::get_greatest_time_wheel()
{
    if (m_timeWheels.empty())
    {
        return TimeWheelPtr();
    }

    return m_timeWheels.front();
}

TimeWheelPtr TimeWheelManager::get_least_time_wheel()
{
    if (m_timeWheels.empty())
    {
        return TimeWheelPtr();
    }

    return m_timeWheels.back();
}

void TimeWheelManager::append_time_wheel(uint32_t scales, uint32_t scaleUnitMs, const std::string& strName)
{
    TimeWheelPtr timeWheel = std::make_shared<TimeWheel>(scales, scaleUnitMs, strName);
    if (m_timeWheels.empty())
    {
        m_timeWheels.push_back(timeWheel);
        return;
    }

    TimeWheelPtr greaterTimeWheel = m_timeWheels.back();
    greaterTimeWheel->set_less_level_tw(timeWheel.get());
    timeWheel->set_greater_level_tw(greaterTimeWheel.get());
    m_timeWheels.push_back(timeWheel);
}

void TimeWheelManager::remove_timer(uint32_t timerId)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_cancelTimerId.insert(timerId);
}

uint32_t TimeWheelManager::add_timer(const timer_sink_it* sink, uint32_t interval,
                                     uint32_t times, timer_it* who_is)
{
    if (m_timeWheels.empty())
    {
        return -1;
    }
    std::lock_guard<std::mutex> lock(m_mutex);
    int64_t when = get_now_time_stamp() + interval;
    get_greatest_time_wheel()->add_timer(std::make_shared<Timer>(when, interval, times, who_is));
    return 0;
}
