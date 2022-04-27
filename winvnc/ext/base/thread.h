#pragma once

#include <stdint.h>

namespace base {

class Thread
{
public:
    /**
     * This enum type indicates how the operating system should schedule current threads.
     */
    enum Priority {
        PRIORITY_IDLE            = 0,    // Scheduled only when no other threads are running.
        PRIORITY_LOWEST          = 1,    // Scheduled less often than LowPriority.
        PRIORITY_LOW             = 2,    // Scheduled less often than NormalPriority.
        PRIORITY_NORMAL          = 3,    // The default priority of the operating system.
        PRIORITY_HIGH            = 4,    // Scheduled more often than NormalPriority.
        PRIORITY_HIGHEST         = 5,    // Scheduled more often than HighPriority.
        PRIORITY_TIMECRITICAL    = 6,    // Scheduled as often as possible.
#if 0
        PRIORITY_INHERIT         = 7,    // Use the same priority as the creating thread.
#endif
    };

    /**
     * Constructs a new Thread to manage a new thread.
     * The thread does not begin executing until start() is called.
     */
    Thread();

    /**
     * Destroys the Thread.
     */
    virtual ~Thread();

    /**
     * Begins execution of the thread by calling run().
     * If the thread is already running, this function does nothing.
     */
    void start();

    /**
     * Blocks until the thread finishes its execution.
     */
    void wait();

    /**
     * Forces the current thread to sleep for secs seconds.
     */
    static void sleep(unsigned long secs);

    /**
     * Forces the current thread to sleep for msecs milliseconds.
     */
    static void msleep(unsigned long msecs);

    /**
     * Forces the current thread to sleep for usecs microseconds.
     */
    static void usleep(unsigned long usecs);

    /**
     * Returns native thread id.
     * @return
     */
    static uint64_t getNativeThreadId();

    /**
     * Sets the priority for current thread.
     */
    static void setPriority(Priority priority);

    /**
     * Sets thread name for current thread.
     */
    static void setThreadName(const char *name);

protected:
    /**
     * The starting point for the thread.
     * After calling start(), the newly created thread calls this function.
     */
    virtual void run() = 0;
    void innerRun();
    
private:
    void *m_priv;
}; // End of class Thread

#define THREAD_NAME_MAX_LENGTH  16

} // End of namespace base
