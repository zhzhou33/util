#pragma once
#include "spdlog/spdlog.h"
#include <sstream>

#define BEGIN_UTIL_NAMESPACE \
    namespace util           \
    {

#define END_UTIL_NAMESPACE }

#define USING_UTIL_NAMEPACE using namespace util;

BEGIN_UTIL_NAMESPACE

#define UTIL_LOG_INFO(x)               \
    do {                               \
        std::ostringstream oss;        \
        oss << x;                      \
        spdlog::info("{}", oss.str()); \
    } while (0);

class NoCopy
{
public:
    NoCopy() {}

protected:
    ~NoCopy() {}

private:
    NoCopy(const NoCopy&);
    NoCopy& operator=(const NoCopy&);
};

enum thread_type
{
    MAIN = 0,
    IO,
};

END_UTIL_NAMESPACE