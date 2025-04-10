#pragma once
#include <condition_variable>
#include <future>
#include <iostream>
#include <thread>
#include <vector>
#include <unordered_map>
#include <memory>
#include <functional>
#include <atomic>
#include <mutex>
#include <queue>
#include "thread_mgr.h"
#include "util.h"
#include "util_common.h"
#include "util_poll.h"

USING_UTIL_NAMEPACE
BEGIN_UTIL_NAMESPACE

class ThreadMgr;
// 线程包装器（自动管理通信）
class ThreadWrapper
{
public:
    ThreadWrapper(ThreadMgr* threadMgr, thread_type type = IO);
    ~ThreadWrapper();

    void thread_run();

    // 发送数据到该线程
    void post_msg(msg* data)
    {
        m_ringBuffer.push(data);
    }

    // 主线程驱动：处理所有收到的数据
    void ProcessReceivedData();

private:
    std::thread* m_pThread;
    std::atomic<bool> running_{true};

    std::atomic<bool> m_awake{false};

    std::condition_variable m_cv;
    std::mutex m_mutex;

    // 接收缓冲区（SPSC 无锁队列）
    RingBuffer<msg*> m_ringBuffer;



    // 线程函数
    void Run();
};

END_UTIL_NAMESPACE