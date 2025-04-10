#include "thread_mgr.h"
#include "thread_wrapper.h"
#include "util_common.h"
#include <cstddef>

ThreadWrapper* ThreadMgr::create_thread()
{
    ThreadWrapper* thr = nullptr;

    thr = new ThreadWrapper(this);

    m_threadLists.push_back(thr);

    pipe_t p;
    p.open();
    m_poll.add(p);

    pollfd fd;
    fd.fd = p.get_read_handle();
    fd.events = POLLIN;
    m_poll.add(fd);

    return thr;
}

void ThreadMgr::register_thread(thread_type type)
{
    if (type == MAIN)
    {
        ThreadWrapper* thr = new ThreadWrapper(this, type);
    }
}