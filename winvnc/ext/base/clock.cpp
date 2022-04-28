#include "base/clock.h"

#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>

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

std::string Clock::getSystemDateTimeString(const char *format)
{
    const std::chrono::time_point<std::chrono::system_clock> now =
        std::chrono::system_clock::now();
    const std::time_t t_c = std::chrono::system_clock::to_time_t(now);

    std::stringstream ss;
    ss << std::put_time(std::localtime(&t_c), format);

    return ss.str();
}

} // End of namespace base
