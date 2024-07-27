#include "test.h"
#include "timer.h"

void Test::timer_work(const timer_it *who_is)
{
    std::cout << "timer_work who_is = " << who_is << std::endl;
}

void Test::func()
{
    m_test_timer.add_timer(this, 2000, -1);
    m_test2_timer.add_timer(this, 3000, -1);
    std::cout << "m_test_timer = " << &m_test_timer << std::endl;
    std::cout << "m_test2_timer = " << &m_test2_timer << std::endl;
}