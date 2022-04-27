#pragma once

#include <chrono>
#include <functional>

namespace base {
namespace io {

class IOContext;

class Timer
{
public:
    /**
     * Constructs an Timer with given io_context.
     */
    Timer(IOContext &io_context);

    /**
     * Destroy the Timer object.
     */
    ~Timer();

    /**
     * Start an asynchronous wait on the timer.
     * This function may be used to initiate an asynchronous wait against the timer.
     * It always returns immediately.
     * For each call to asyncWait(), the supplied handler will be called exactly once.
     * The handler will be called when:
     *   The timer has expired.
     *   The timer was cancelled, in which case the handler is passed the error operation aborted.
     */
    void asyncWait(const std::chrono::seconds &duration,
                   const std::function<void(int error)> &callback);
    void asyncWait(const std::chrono::milliseconds &duration,
                   const std::function<void(int error)> &callback);
    void asyncWait(const std::chrono::microseconds &duration,
                   const std::function<void(int error)> &callback);

    void asyncWait(const std::chrono::seconds &duration,
                   std::function<void(int error)> &&callback);
    void asyncWait(const std::chrono::milliseconds &duration,
                   std::function<void(int error)> &&callback);
    void asyncWait(const std::chrono::microseconds &duration,
                   std::function<void(int error)> &&callback);

    /**
     * Cancel any asynchronous operations that are waiting on the timer.
     * Returns the number of asynchronous operations that were cancelled.
     */
    size_t cancel();

    /**
     * Perform a blocking wait on the timer.
     */
    void wait(const std::chrono::seconds &duration);
    void wait(const std::chrono::milliseconds &duration);
    void wait(const std::chrono::microseconds &duration);

private:
    void *m_priv;

}; // End of class Timer

} // End of namespace io
} // End of namespace base
