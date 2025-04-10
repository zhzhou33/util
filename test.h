#pragma once
#include "timer.h"
#include <chrono>
#include <iostream>
#include <sstream>
#include <iomanip>
inline std::string date_time(std::time_t posix)
{
    char buf[20]; // big enough for 2015-07-08 10:06:51\0
    std::tm tp = *std::localtime(&posix);
    return {buf, std::strftime(buf, sizeof(buf), "%F %T", &tp)};
}

inline std::string stamp()
{
    using namespace std;
    using namespace std::chrono;

    // get absolute wall time
    auto now = system_clock::now();

    // find the number of milliseconds
    auto ms = duration_cast<milliseconds>(now.time_since_epoch()) % 1000;

    // build output string
    std::ostringstream oss;
    oss.fill('0');

    // convert absolute time to time_t seconds
    // and convert to "date time"
    oss << date_time(system_clock::to_time_t(now));
    oss << '.' << setw(3) << ms.count();

    return oss.str();
}

class Test : public timer_sink_it
{
public:
    Test() {}
    ~Test() {}
    virtual void timer_work(const timer_it* who_is);

    void func();

private:
    timer_elem_t m_test_timer;
    timer_elem_t m_test2_timer;
};