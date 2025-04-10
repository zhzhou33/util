#ifndef TIME_WHEEL_SCHEDULER_H_
#define TIME_WHEEL_SCHEDULER_H_

#include <cstdint>
#include <mutex>
#include <thread>
#include <unordered_set>
#include <vector>

#include "time_wheel.h"
#include "timer.h"

#define TIMESTEPMS 10

class TimeWheelManager
{
public:
    static TimeWheelManager* get_instance()
    {
        static TimeWheelManager instance;
        return &instance;
    }

    void init();

    uint32_t add_timer(const timer_sink_it* sink, uint32_t interval,
                       uint32_t timers, timer_it* who_is);

    void remove_timer(uint32_t timer_id);

    bool start();
    void stop();

    void append_time_wheel(uint32_t scales, uint32_t scaleUnitMs, const std::string& strName = "");

    void run_once();

private:
    TimeWheelManager();

private:
    void run();

    TimeWheelPtr get_greatest_time_wheel();
    TimeWheelPtr get_least_time_wheel();

private:
    std::mutex m_mutex;
    std::thread m_thread;

    bool m_stop;

    std::unordered_set<uint32_t> m_cancelTimerId;

    uint32_t m_timerStepMs;
    std::vector<TimeWheelPtr> m_timeWheels;

    int64_t m_preRunTime;
};

#endif // TIME_WHEEL_SCHEDULER_H_
