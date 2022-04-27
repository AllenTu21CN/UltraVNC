#include "base/event_loop.h"
#include "base/log.h"
#include "base/io/io_context.h"
#include "base/io/timer.h"

#ifdef ONLY_SUPPORT_SINGLE_THREAD
#include "base/concurrent_queue.h"
#else
#include <mutex>
#include <condition_variable>
#endif

#include <functional>

#include <assert.h>
#include <stddef.h>

namespace base {

#define INIT_PRIV(d) \
    EventLoopPrivate *d = NULL; \
    if (m_priv) { \
        d = static_cast<EventLoopPrivate *>(m_priv); \
    } \
    assert(d); \


class EventLoopPrivate
{
public:
    EventLoopPrivate();
    ~EventLoopPrivate();

    void postEvent(const EventLoop::Event &event);
    int  postEvent(EventLoop::Event &event, bool sync, const std::chrono::milliseconds &timeout);

    int syncPostEvent(EventLoop::Event &event, const std::chrono::milliseconds &timeout);
    int asyncPostEvent(const EventLoop::Event &event);

    io::IOContext &getIOContext();

    void setOnEventCallback(std::function<void(const EventLoop::Event &)> &&callback);

    void setOnSystemSignalCallback(std::function<void(int)> &&callback);

    void run();

    void quit();

protected:
    void idleTimerCallback(int error);

private:
    io::IOContext m_io_ctx;
    io::Timer m_idle_timer;

    std::function<void(const EventLoop::Event &)> m_on_event_callback;
    std::function<void(int)> m_on_system_signal_callback;

    std::function<void(int)> m_idle_timer_callback;

#ifdef ONLY_SUPPORT_SINGLE_THREAD
    ConcurrentQueue<int>     m_sync;

    std::function<void(EventLoop::Event &)> m_on_sync_event_callback;
    void onSyncEvent(EventLoop::Event &event);

#else
    class SyncEvent {
    public:
        SyncEvent(EventLoop::Event *pevt) : m_pevt(pevt), m_valid(true), m_lk(), m_cnd() {}
        ~SyncEvent() {}

        EventLoop::Event *m_pevt;
        bool              m_valid;
        std::mutex        m_lk;
        std::condition_variable m_cnd;
    };

    std::function<void(std::shared_ptr<SyncEvent> evt)> m_on_sync_event_callback;
    void onSyncEvent(std::shared_ptr<SyncEvent> evt);
#endif
}; // End of class EventLoopPrivate

EventLoopPrivate::EventLoopPrivate()
    : m_idle_timer(m_io_ctx)
{
    m_idle_timer_callback = std::bind(&EventLoopPrivate::idleTimerCallback,
                                      this, std::placeholders::_1);

    m_on_sync_event_callback = std::bind(&EventLoopPrivate::onSyncEvent,
                                      this, std::placeholders::_1);
}

EventLoopPrivate::~EventLoopPrivate()
{
    // TODO:
}

void EventLoopPrivate::postEvent(const EventLoop::Event &event)
{
    m_io_ctx.dispatch(std::move([event, this]() { m_on_event_callback(event); }));
    //m_io_ctx.post(std::move([event, this]() { m_on_event_callback(event); }));
}

int EventLoopPrivate::postEvent(EventLoop::Event &event, bool sync,
        const std::chrono::milliseconds &timeout)
{
    if (sync) return syncPostEvent(event, timeout);
    else return asyncPostEvent(event);
}

#ifdef ONLY_SUPPORT_SINGLE_THREAD
// @brief Require caller (call @post) must be in the same thread.
int EventLoopPrivate::syncPostEvent(EventLoop::Event &event,
        const std::chrono::milliseconds &timeout)
{
    if (m_io_ctx.stopped()) {
        base::_error("Event loop is stopped, Not running!");
        return -1; // event loop has beed stopped
    }

    int what = event.what;
    m_io_ctx.dispatch(std::move([&]() { m_on_sync_event_callback(event); }));
    //m_io_ctx.post(std::move([&]() { m_on_sync_event_callback(event); }));

    // wait for on sync event over
    int evt;
    if (!m_sync.pop(evt, timeout) || evt != what) {
        //abort(); // what should i do
        base::_error("PostEvent(sync: true) pop failed: evt(%d), what: (%d)",
            evt, what);
        return -1; // failed sync post event
    }

    return 0;
}

void EventLoopPrivate::onSyncEvent(EventLoop::Event &event)
{
    int what = event.what;
    // on event callback
    m_on_event_callback(event);

    // sync
    m_sync.push(what);
}

#else // NOT ONLY_SUPPORT_SINGLE_THREAD
int EventLoopPrivate::syncPostEvent(EventLoop::Event &event,
        const std::chrono::milliseconds &timeout)
{
    if (m_io_ctx.stopped()) {
        base::_error("Sync post event, loop is stopped, Not running!");
        return -1; // event loop has beed stopped
    }

    std::shared_ptr<SyncEvent> evt(new SyncEvent(&event));

    m_io_ctx.dispatch(std::move([&, evt]() { m_on_sync_event_callback(evt); } ));
    //NOTE: onSyncEvent would be deadlock when post in the @onEvent(same) thread.
    //m_io_ctx.post(std::move([&]() { m_on_sync_event_callback(evt);} ));

    std::unique_lock<std::mutex> lk(evt->m_lk);
    // wait for callback onEvent
    if (evt->m_valid && evt->m_cnd.wait_for(lk, timeout) == std::cv_status::timeout) {
        // re-acquire lk again even if timeout, Timeout time maybe greater than 300s.
        base::_error("Sync post event failed: timeout(%lld)ms!", timeout.count());
        // @m_pevt will be released, the caller MUST be careful.
        evt->m_valid = false;

        return -1;
    }

    return 0;
}

void EventLoopPrivate::onSyncEvent(std::shared_ptr<SyncEvent> evt)
{
    std::unique_lock<std::mutex> lk(evt->m_lk);
    if (!evt->m_valid) return;
    lk.unlock();

    // on event callback
    m_on_event_callback(*evt->m_pevt);

    lk.lock(); // lock again
    evt->m_valid = false;
    lk.unlock(); 

    // sync 
    evt->m_cnd.notify_one();
}
#endif // End of NOT ONLY_SUPPORT_SINGLE_THREAD

int EventLoopPrivate::asyncPostEvent(const EventLoop::Event &event)
{
    m_io_ctx.dispatch(std::move([=]() { m_on_event_callback(event); }));
    //m_io_ctx.post(std::move([=]() { m_on_event_callback(event); }));

    return 0;
}

io::IOContext &EventLoopPrivate::getIOContext()
{
    return m_io_ctx;
}

void EventLoopPrivate::setOnEventCallback(std::function<void(const EventLoop::Event &)> &&callback)
{
    m_on_event_callback = std::move(callback);
}

void EventLoopPrivate::setOnSystemSignalCallback(std::function<void(int)> &&callback)
{
    m_on_system_signal_callback = callback;
}

void EventLoopPrivate::run()
{
    //base::_info("EventLoopPrivate started!");

    // Start idle timer
    m_idle_timer.asyncWait(std::chrono::seconds(1), m_idle_timer_callback);

    m_io_ctx.run();

    //base::_info("EventLoopPrivate quitted!");
}

void EventLoopPrivate::quit()
{
    m_io_ctx.stop();

    // Block until stopped
    while (!m_io_ctx.stopped()) {
        Thread::usleep(1);
    }
}

void EventLoopPrivate::idleTimerCallback(int error)
{
    if (0 == error) {
        // Do nothing
        m_idle_timer.asyncWait(std::chrono::seconds(1), m_idle_timer_callback);
    }
}

EventLoop::EventLoop()
    : Thread(), m_priv(new EventLoopPrivate)
{
    // Set on event callback
    INIT_PRIV(d);

    d->setOnEventCallback(std::move(std::bind(&EventLoop::onEvent,
                                              this, std::placeholders::_1)));
    //d->setOnSystemSignalCallback(std::move(std::bind(&EventLoop::onSystemSignal,
    //                                                 this, std::placeholders::_1)));
}

EventLoop::~EventLoop()
{
    // Quit first
    quit();

    if (m_priv) {
        EventLoopPrivate *d = static_cast<EventLoopPrivate *>(m_priv);
        if (d) {
            delete d;
            m_priv = NULL;
        }
    }
}

void EventLoop::postEvent(const Event &event)
{
    INIT_PRIV(d);

    return d->postEvent(event);
}

int EventLoop::postEvent(Event &event, bool sync, const std::chrono::milliseconds &timeout)
{
    INIT_PRIV(d);

    return d->postEvent(event, sync, timeout);
}

io::IOContext &EventLoop::getIOContext()
{
    INIT_PRIV(d);

    return d->getIOContext();
}

void EventLoop::quit()
{
    INIT_PRIV(d);

    d->quit();

    // Wait thread exited.
    Thread::wait();
}

void EventLoop::run()
{
    //base::_info("EventLoop started!");

    INIT_PRIV(d);

    d->run();
}

} // End of namespace base
