#include "timer.h"
#include "thread_wrapper.h"
#include "time_wheel_scheduler.h"

#include <cstddef>
#include <cstdint>

Timer::Timer(int64_t when_ms, int64_t interval_ms,
             int32_t times, timer_it* who_is) :
    m_intervalMs(interval_ms),
    m_repeated(times > 0),
    m_whenMs(when_ms),
    m_times(times),
    m_who(who_is)
{
}

void Timer::run()
{
    timer_sink_it* sink = m_who->get_sink();
    if (sink)
    {
        sink->timer_work(m_who);
    }
}

timer_it::timer_it(uint32_t id, thread_wrapper_t* ownThr) :
    m_id(0 == id ? __COUNTER__ : id), m_times(0)
{
}

int32_t timer_it::add_timer(const timer_sink_it* sink, uint32_t interval,
                            uint32_t times)
{
    TimeWheelManager* timeWheelScheduler = TimeWheelManager::get_instance();
    timeWheelScheduler->add_timer(sink, interval, times, this);
    return 0;
}

timer_elem_t::timer_elem_t(uint32_t id, thread_wrapper_t* own_thr) :
    timer_it(id, own_thr),
    m_sink(NULL)
{
}

int32_t timer_elem_t::add_timer(const timer_sink_it* sink, uint32_t interval,
                                uint32_t times)
{
    int32_t ret = timer_it::add_timer(sink, interval, times);
    if (ret == 0)
        m_sink = const_cast<timer_sink_it*>(sink);
    return ret;
}

int32_t timer_elem_t::remove_timer()
{
    TimeWheelManager* timeWheelScheduler = TimeWheelManager::get_instance();
    timeWheelScheduler->remove_timer(this->m_id);
    return 0;
}