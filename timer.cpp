#include "timer.h"
#include "thread_wrapper.h"
#include "time_wheel_scheduler.h"

#include <cstddef>
#include <cstdint>

Timer::Timer(int64_t when_ms, int64_t interval_ms,
             timer_it* who_is) :
    interval_ms_(interval_ms), repeated_(interval_ms_ > 0), when_ms_(when_ms),
    who_is_(who_is)
{
}

void Timer::run()
{
    timer_sink_it* sink = who_is_->get_sink();
    if (sink)
    {
        sink->timer_work(who_is_);
    }
}

timer_it::timer_it(uint32_t id, thread_wrapper_t* ownThr) :
    m_id(0 == id ? __COUNTER__ : id), m_times(0)
{
}

int32_t timer_it::add_timer(const timer_sink_it* sink, uint32_t interval,
                            uint32_t timers)
{
    TimeWheelManager* timeWheelScheduler = TimeWheelManager::get_instance();
    timeWheelScheduler->add_timer(sink, interval, timers, this);
    return 0;
}

timer_elem_t::timer_elem_t(uint32_t id, thread_wrapper_t* own_thr) :
    timer_it(id, own_thr),
    m_sink(NULL)
{
}

int32_t timer_elem_t::add_timer(const timer_sink_it* sink, uint32_t interval,
                                uint32_t timers)
{
    int32_t ret = timer_it::add_timer(sink, interval, timers);
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