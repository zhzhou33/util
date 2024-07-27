#pragma once
#include "timer.h"
#include <iostream>
class Test : public timer_sink_it
{
  public:
    Test() {}
    ~Test() {}
    virtual void timer_work(const timer_it *who_is);

    void func();

  private:
    timer_elem_t m_test_timer;
    timer_elem_t m_test2_timer;
};