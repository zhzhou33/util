#ifndef TIMER_H_
#define TIMER_H_

#include <cstddef>
#include <cstdint>
#include <functional>
#include <memory>

#include "thread_wrapper.h"
class timer_sink_it;
class timer_elem_t;
class timer_it;
// typedef std::function<void()> TimerTask;
// class TimeBase;

class Timer
{
public:
    // Timer() {}
    Timer(int64_t when_ms, int64_t interval_ms, timer_it* who_is);

    void run();

    uint32_t id() const { return id_; }

    int64_t when_ms() const { return when_ms_; }

    bool repeated() const { return repeated_; }

    void update_when_time() { when_ms_ += interval_ms_; }

    int32_t get_times() const { return m_times; }

    int32_t decrease_times()
    {
        if (m_times > 0)
            return --m_times;
        return -1;
    }

private:
    uint32_t id_;
    // timer_sink_it *sink_;
    timer_it* who_is_;
    int64_t when_ms_;
    uint32_t interval_ms_;
    bool repeated_;
    int32_t m_times;
};

using TimerPtr = std::shared_ptr<Timer>;

class timer_it;
class thread_wrapper_t;

class timer_sink_it
{
public:
    virtual void timer_work(const timer_it* who_is) = 0;
    virtual ~timer_sink_it() {}
};

class timer_it
{
public:
    timer_it(uint32_t id = 0, thread_wrapper_t* own_thr = NULL);
    virtual ~timer_it() {}
    virtual int32_t add_timer(const timer_sink_it* sink, uint32_t interval,
                              uint32_t timers = 1);
    virtual int32_t remove_timer() { return 0; }

    virtual timer_sink_it* get_sink() const = 0;

    uint32_t get_id() const { return m_id; }

    uint32_t get_times() const { return m_times; }

protected:
    const uint32_t m_id;
    uint32_t m_times;
};

class timer_elem_t : public timer_it
{
public:
    timer_elem_t(uint32_t id = 0, thread_wrapper_t* own_thr = NULL);
    virtual ~timer_elem_t() {}
    virtual int32_t add_timer(const timer_sink_it* sink, uint32_t interval,
                              uint32_t timers = 1);
    virtual int32_t remove_timer();
    timer_sink_it* get_sink() const { return m_sink; }

protected:
    timer_sink_it* m_sink;
};

#endif // TIMER_H_
