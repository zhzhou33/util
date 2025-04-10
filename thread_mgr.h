#pragma once

#include "util_common.h"
#include "util_poll.h"
#include <list>
#include <sys/poll.h>
#include "util_poll.h"

USING_UTIL_NAMEPACE
BEGIN_UTIL_NAMESPACE

class ThreadWrapper;

class ThreadMgr : public NoCopy
{
public:
    static ThreadMgr* instance()
    {
        static ThreadMgr instance;
        return &instance;
    }

    static void destory();

    ThreadWrapper* create_thread();

    void register_thread(thread_type type);

protected:
private:
    ThreadMgr()
    {
        pipe_t p;
        p.open();
        m_poll.add(p);

        pollfd fd;
        fd.fd = p.get_read_handle();
        fd.events = POLLIN;
        m_poll.add(fd);
    }

public:
    std::list<ThreadWrapper*> m_threadLists;

    poll_t m_poll;
};

END_UTIL_NAMESPACE