#pragma once

#include <mutex>
#include <vector>

namespace base {

/**
 * DoubleBuffer is a thread-safe double-buffering container.
 */
template<typename T>
class DoubleBuffer {
public:
    /**
     * Constructor a DoubleBuffer object.
     */
    DoubleBuffer();

    /**
     * Destroy the DoubleBuffer and buffered objects.
     */
    ~DoubleBuffer();

    /**
     * Returns a reference to the front element in the container.
     */
    T &front();

    /**
     * Returns a reference to the back element in the container.
     */
    T &back();

    /**
     * Locks the DoubleBuffer to disable swap.
     * If another thread has already locked the DoubleBuffer,
     * a call to lock will block execution until the lock is acquired.
     */
    void lock();

    /**
     * Tries to lock the DoubleBuffer. Returns immediately.
     * On successful lock acquisition returns true, otherwise returns false.
     */
    bool tryLock();

    /**
     * Unlocks the DoubleBuffer.
     * The DoubleBuffer must be locked by the current thread of execution,
     * otherwise, the behavior is undefined.
     */
    void unlock();

    /**
     * Flip front/back buffers.
     * The DoubleBuffer must be locked first.
     */
    void flip();

private:
    std::vector<T>  m_buffer;
    std::mutex      m_mutex;
    bool            m_swap;

}; // End of class DoubleBuffer

template<typename T>
DoubleBuffer<T>::DoubleBuffer()
    : m_buffer(2), m_swap(false)
{
}

template<typename T>
DoubleBuffer<T>::~DoubleBuffer()
{
}

template<typename T>
T &DoubleBuffer<T>::front()
{
    return m_swap ? m_buffer[0] : m_buffer[1];
}

template<typename T>
T &DoubleBuffer<T>::back()
{
    return m_swap ? m_buffer[1] : m_buffer[0];
}

template<typename T>
void DoubleBuffer<T>::lock()
{
    m_mutex.lock();
}

template<typename T>
bool DoubleBuffer<T>::tryLock()
{
    return m_mutex.try_lock();
}

template<typename T>
void DoubleBuffer<T>::unlock()
{
    m_mutex.unlock();
}

template<typename T>
void DoubleBuffer<T>::flip()
{
    m_swap = !m_swap;
}

} // End of namespace base
