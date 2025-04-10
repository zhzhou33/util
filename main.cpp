#include <cstddef>
#include <string>
#include <thread>

#include "spdlog/sinks/rotating_file_sink.h"
#include "test.h"
#include "thread_mgr.h"
#include "time_wheel.h"
#include "time_wheel_scheduler.h"
#include <chrono>
// #include "time_common.h"
#include "util.h"
#include "util_common.h"

USING_UTIL_NAMEPACE

void handle_user_event()
{
    Test* test = new Test();
    test->func();
}

void worker1_func(util::RingBuffer<int>& rb)
{
    int counter = 0;
    while (true)
    {
        if (rb.push(counter) == 0)
        {
            counter++;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(1000)); // 模拟工作负载
    }
}

// 工作线程2：向另一个 RingBuffer 写入数据
void worker2_func(util::RingBuffer<int>& rb)
{
    int counter = 1000;
    while (true)
    {
        if (rb.push(counter) == 0)
        {
            counter++;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(1000)); // 模拟工作负载
    }
}

// 主定时器线程：每隔10ms读取两个 RingBuffer 的数据
void timer_func(util::RingBuffer<int>& rb1, util::RingBuffer<int>& rb2)
{
    while (true)
    {
        // 读取 rb1 的数据
        int data;
        while (rb1.pop(data) == 0)
        {
            std::cout << "[Worker1] Data: " << data << std::endl;
        }

        // 读取 rb2 的数据
        while (rb2.pop(data) == 0)
        {
            std::cout << "[Worker2] Data: " << data << std::endl;
        }

        // 每隔10ms轮询一次
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}

class customer_msg : public msg
{
public:
    virtual void on_message()
    {
        std::cout << data << std::endl;
    }

public:
    std::string data;
};

int main()
{
    auto my_logger = spdlog::rotating_logger_mt("UTIL", "logs/log.log", 1024 * 1024 * 10, 10);
    my_logger->set_level(spdlog::level::trace);
    // spdlog::set_pattern("[%H:%M:%S %z] %v");
    spdlog::set_default_logger(my_logger);

    // spdlog::cfg::load_env_levels();

    UTIL_LOG_INFO("hello " << "world");

    TimeWheelManager* timeWhellMgr = TimeWheelManager::get_instance();

    timeWhellMgr->init();

    timeWhellMgr->start();
    handle_user_event();

    // util::RingBuffer<int> buufer(4);

    // util::RingBuffer<int> rb1(4);
    // util::RingBuffer<int> rb2(4);

    // // 启动工作线程
    // std::thread worker1(worker1_func, std::ref(rb1));
    // std::thread worker2(worker2_func, std::ref(rb2));

    ThreadWrapper* thr = ThreadMgr::instance()->create_thread();
    // timer_func(rb1, rb2);
    customer_msg* data = new customer_msg();
    data->data = "hello";
    thr->post_msg(data);

    // worker1.join();
    // worker2.join();
    while (true)
    {
        timeWhellMgr->run_once();
        thr->thread_run();
    }

    return 0;
}