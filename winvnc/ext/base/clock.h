#pragma once

#include <string>

#include <stdint.h>

namespace base {

class Clock
{
public:
    /**
     * Return the steady timestamp in milliseconds.
     */
    static int64_t getSteadyTimeStamp();

    /**
     * Returns the number of microseconds since 1970-01-01T00:00:00 Universal Coordinated Time.
     */
    static int64_t getMicroSecondsSinceEpoch();

    /**
     * Returns the number of milliseconds since 1970-01-01T00:00:00 Universal Coordinated Time.
     */
    static int64_t getMilliSecondsSinceEpoch();

    /**
     * Return system clock date time string with format.
     * format string ref: https://en.cppreference.com/w/cpp/io/manip/put_time
     */
    static std::string getSystemDateTimeString(const char *format);

protected:
    Clock();

private:
    static void *m_priv;

}; // End of class Clock

} // namespace base
