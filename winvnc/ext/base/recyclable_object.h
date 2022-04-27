#pragma once

#include <functional>
#include <list>
#include <memory>
#include <mutex>

namespace base {

class RecyclableObject
{
protected:
    RecyclableObject();
    virtual ~RecyclableObject();

private:
    template <typename T>
    friend class RecyclableObjectPool;

}; // End of class RecyclableObject

template <typename T>
class RecyclableObjectPool
{
public:
    RecyclableObjectPool();

    ~RecyclableObjectPool();

    std::unique_ptr<T, std::function<void(T *)>> allocate2();
    template <typename ...Args>
    std::unique_ptr<T, std::function<void(T *)>> allocate(Args&&... args);

protected:
    void recycle(T *t);

private:
    std::list<T *> m_objects;
    std::mutex m_mutex;

}; // End of class RecyclableObjectPool

template <typename T>
RecyclableObjectPool<T>::RecyclableObjectPool()
{
}

template <typename T>
RecyclableObjectPool<T>::~RecyclableObjectPool()
{
    while (!m_objects.empty()) {
        T *t = m_objects.front();
        m_objects.pop_front();
        delete t;
    }
}

template <typename T>
template <typename ...Args>
std::unique_ptr<T, std::function<void(T *)>> RecyclableObjectPool<T>::allocate(Args&&... args)
{
    T *t = nullptr;
    if (m_objects.empty()) {
        t = new T(std::forward<Args>(args)...);
    } else {
        std::lock_guard<std::mutex> lock(m_mutex);
        t = m_objects.front();
        m_objects.pop_front();
    }

    std::unique_ptr<T, std::function<void(T *)>> obj(t, [this](T *t) {
        recycle(t);
    });

    return std::move(obj);
}

template <typename T>
std::unique_ptr<T, std::function<void(T *)>> RecyclableObjectPool<T>::allocate2()
{
    T *t = nullptr;
    if (m_objects.empty()) {
        t = new T();
    } else {
        std::lock_guard<std::mutex> lock(m_mutex);
        t = m_objects.front();
        m_objects.pop_front();
    }

    std::unique_ptr<T, std::function<void(T *)>> obj(t, [this](T *t) { recycle(t); });

    return std::move(obj);
}


template <typename T>
void RecyclableObjectPool<T>::recycle(T *t)
{
    if (t) {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_objects.push_back(t);
    }
}

} // End of namespace base
