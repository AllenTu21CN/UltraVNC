#pragma once

#include <queue>
#include <thread>
#include <condition_variable>
#include "clock.h"

namespace base {

/**
 * ConcurrentQueue is a thread-safe first in-first out (FIFO) container.
 */
template<typename T>
class ConcurrentQueue
{
public:
    /**
     * Construct an empty ConcurrentQueue object.
     */
    ConcurrentQueue();

    /**
     * Destroy the ConcurrentQueue object.
     */
    ~ConcurrentQueue();

    /**
     * Tries to return an element from the beginning of the queue without removing it.
     */
    bool tryPeak(T &element);

    /**
     * Tries to remove and return the element at the beginning of the queue.
     */
    bool tryPop(T &element);

    /**
     * Returns an element from the beginning of the queue without removing it.
     */
    template<class Rep, class Period>
    bool peak(T &element, const std::chrono::duration<Rep, Period> &wait_time);

    /**
     * Removes and returns the element at the beginning of the queue.
     */
    template<class Rep, class Period>
    bool pop(T &element, const std::chrono::duration<Rep, Period> &wait_time);

    /**
     * Adds the elementto the end of the queue.
     */
    void push(const T &element);

    /**
     * Pops(moves) the element which is in the beginning of the queue.
     * 2019/3/2, fangzh@3bu.cn
     */
    template<class Rep, class Period>
    T pop(const std::chrono::duration<Rep, Period> &wait_time);

    /**
     * Moves the element into the end of the queue.
     * 2019/3/2, fangzh@3bu.cn
     */
    void push(T &&element);

    /**
     * Returns the number of elements in the queue.
     */
    size_t size();

private:
    std::queue<T>           m_queue;
    std::mutex              m_mutex;
    std::condition_variable m_cond_var;
}; // End of class ConcurrentQueue

template<typename T>
ConcurrentQueue<T>::ConcurrentQueue()
{
}

template<typename T>
ConcurrentQueue<T>::~ConcurrentQueue()
{
}

template<typename T>
bool ConcurrentQueue<T>::tryPeak(T &element)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    if (!m_queue.empty()) {
        element = m_queue.front();

        return true;
    }

    return false;
}

template<typename T>
bool ConcurrentQueue<T>::tryPop(T &element)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    if (!m_queue.empty()) {
        element = m_queue.front();
        m_queue.pop();

        return true;
    }

    return false;
}

template<typename T>
template<class Rep, class Period>
bool ConcurrentQueue<T>::peak(T &element, const std::chrono::duration<Rep, Period> &wait_time)
{
    std::unique_lock<std::mutex> lock(m_mutex);
    if (m_cond_var.wait_for(lock, wait_time, [&] { return !m_queue.empty(); })) {
        element = m_queue.front();

        return true;
    }

    return false;
}

template<typename T>
template<class Rep, class Period>
bool ConcurrentQueue<T>::pop(T &element, const std::chrono::duration<Rep, Period> &wait_time)
{
    std::unique_lock<std::mutex> lock(m_mutex);
    if (m_cond_var.wait_for(lock, wait_time, [&] { return !m_queue.empty(); })) {
        element = m_queue.front();
        m_queue.pop();

        return true;
    }

    return false;
}

template<typename T>
void ConcurrentQueue<T>::push(const T &element)
{
    std::unique_lock<std::mutex> lock(m_mutex);

    m_queue.push(element);

    lock.unlock();
    m_cond_var.notify_one();
}

template<typename T>
template<class Rep, class Period>
T ConcurrentQueue<T>::pop(const std::chrono::duration<Rep, Period> &wait_time)
{
    T element;

    std::unique_lock<std::mutex> lock(m_mutex);
    if (m_cond_var.wait_for(lock, wait_time, [&] { return !m_queue.empty(); })) {
        element = std::move(m_queue.front());
        m_queue.pop();

        return element;
    }

    return element;
}

template<typename T>
void ConcurrentQueue<T>::push(T &&element)
{
    std::unique_lock<std::mutex> lock(m_mutex);

    m_queue.push(std::move(element));

    lock.unlock();
    m_cond_var.notify_one();
}

template<typename T>
size_t ConcurrentQueue<T>::size()
{
    std::lock_guard<std::mutex> lock(m_mutex);

    return m_queue.size();
}

} // namespace base
