#include "base/clock.h"

#include <chrono>

namespace base {

class ClockPrivate
{
public:
    ClockPrivate()
    {
        m_base_steady_msec = std::chrono::steady_clock::now();
    }

    ~ClockPrivate()
    {
    }

    std::chrono::steady_clock::time_point m_base_steady_msec;
};

void * Clock::m_priv = new ClockPrivate;

int64_t Clock::getSteadyTimeStamp()
{
    ClockPrivate *d = static_cast<ClockPrivate *>(m_priv);
    return std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::steady_clock::now() - d->m_base_steady_msec).count();
}

int64_t Clock::getMicroSecondsSinceEpoch()
{
    return std::chrono::duration_cast<std::chrono::microseconds>(
        std::chrono::steady_clock::now().time_since_epoch()).count();
}

int64_t Clock::getMilliSecondsSinceEpoch()
{
    return std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::steady_clock::now().time_since_epoch()).count();
}

} // End of namespace base
