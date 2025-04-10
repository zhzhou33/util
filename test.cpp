#include "test.h"
// #include "time_common.h"
#include "time_wheel.h"
#include "timer.h"

void Test::timer_work(const timer_it* who_is)
{
    auto time = get_now_time_stamp();

    UTIL_LOG_INFO("timer_work who_is = " << who_is << ", time=" << time);

    // std::cout << "timer_work who_is = " << who_is << ", time=" << time << std::endl;
}

void Test::func()
{
    auto time = get_now_time_stamp();
    // m_test_timer.add_timer(this, 2000, 1);
    m_test2_timer.add_timer(this, 3000, -1);

    // std::cout << "m_test_timer = " << &m_test_timer << ", time=" << time << std::endl;
    time = get_now_time_stamp();
    std::cout << "m_test2_timer = " << &m_test2_timer << ", time=" << time << std::endl;
}