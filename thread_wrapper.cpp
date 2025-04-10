#include "thread_wrapper.h"
#include "thread_mgr.h"
#include "util_common.h"

// ThreadWrapper 实现
ThreadWrapper::ThreadWrapper(ThreadMgr* threadMgr, thread_type type)
{
    if (type == IO)
        m_pThread = new std::thread(&ThreadWrapper::Run, this);
}

ThreadWrapper::~ThreadWrapper()
{
    running_ = false;
    if (m_pThread->joinable()) m_pThread->join();
}

void ThreadWrapper::thread_run()
{
    ThreadMgr::instance()->m_poll.loop();
}

void ThreadWrapper::Run()
{
    while (true)
    {
        if (!running_) break;

        int id = ThreadMgr::instance()->m_poll.m_pipe[0].get_read_handle();
        char buff[1] = {0};
        read(id, buff, sizeof(buff));

        msg* data = nullptr;
        // handle data
        if (m_ringBuffer.pop(data) == 0)
        {
            data->on_message();
        }
    }
}

// 线程执行函数
// void ThreadWrapper::Run()
// {
//     while (running_)
//     {
//         std::this_thread::sleep_for(std::chrono::milliseconds(1));
//     }
// }

// 发送数据到该线程
// template <typename T>
// void ThreadWrapper::Send(const T& data)
// {
//     m_ringBuffer.push(data);
// }

// 主线程驱动处理数据
void ThreadWrapper::ProcessReceivedData()
{
}