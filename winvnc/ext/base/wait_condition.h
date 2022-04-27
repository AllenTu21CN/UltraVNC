/*
 * wait_condition.h
 *
 *  Created on: 2018/05/09
 *      Author: Tuyj
 */
#pragma once
#include <mutex>
#include <condition_variable>

namespace base {

class WaitCondition
{
public:
    template<class Predicate>
    bool wait(Predicate pred, uint32_t max_time_ms = 0xffffffff) {
        std::chrono::milliseconds dura(max_time_ms);
        auto next_time = std::chrono::steady_clock::now() + dura;
        std::unique_lock<std::mutex> lk(m_mutex);
        return m_cond.wait_until(lk, next_time, pred);
    }

    template<class Predicate>
    void notify_one(Predicate pred) {
        std::unique_lock<std::mutex> lk(m_mutex);
        pred();
        lk.unlock();
        m_cond.notify_one();
    }

    template<class Predicate>
    void notify_all(Predicate pred) {
        std::unique_lock<std::mutex> lk(m_mutex);
        pred();
        lk.unlock();
        m_cond.notify_all();
    }

private:
    std::mutex              m_mutex;
    std::condition_variable m_cond;
};

}  // namespace base
