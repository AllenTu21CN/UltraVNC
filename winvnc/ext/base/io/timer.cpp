#include "base/io/timer.h"
#include "base/io/io_context.h"

#include <asio.hpp>

namespace base {
namespace io {

#define INIT_PRIV(d) \
    TimerPrivate *d = NULL; \
    if (m_priv) { \
        d = static_cast<TimerPrivate *>(m_priv); \
    } \
    assert(d); \


class TimerPrivate
{
public:
    TimerPrivate(void *io_ctx_priv);
    ~TimerPrivate();

    template<class Rep, class Period>
    void asyncWait(const std::chrono::duration<Rep, Period> &duration,
                   const std::function<void(int error)> &callback);

    template<class Rep, class Period>
    void asyncWait(const std::chrono::duration<Rep, Period> &duration,
                   std::function<void(int error)> &&callback);

    size_t cancel();

    template<class Rep, class Period>
    void wait(const std::chrono::duration<Rep, Period> &duration);

private:
    asio::steady_timer m_asio_timer;
}; // End of class TimerPrivate

TimerPrivate::TimerPrivate(void *io_ctx_priv)
    : m_asio_timer(*static_cast<asio::io_service *>(io_ctx_priv))
{
}

TimerPrivate::~TimerPrivate()
{
}

template<class Rep, class Period>
void TimerPrivate::asyncWait(const std::chrono::duration<Rep, Period> &duration,
                             const std::function<void(int error)> &callback)
{
    m_asio_timer.expires_from_now(duration);
    m_asio_timer.async_wait([&](asio::error_code ec) {
        //fprintf(stderr, "TimerPrivate::asyncWait with const & callback.\n");
        if (asio::error::operation_aborted != ec) {
            callback(ec.value());
        }
    });
}

template<class Rep, class Period>
void TimerPrivate::asyncWait(const std::chrono::duration<Rep, Period> &duration,
                             std::function<void(int error)> &&callback)
{
    m_asio_timer.expires_from_now(duration);
    m_asio_timer.async_wait([=](asio::error_code ec) {
        //fprintf(stderr, "TimerPrivate::asyncWait with && callback.\n");
        if (asio::error::operation_aborted != ec) {
            callback(ec.value());
        }
    });
}

size_t TimerPrivate::cancel()
{
    return m_asio_timer.cancel();
}

template<class Rep, class Period>
void TimerPrivate::wait(const std::chrono::duration<Rep, Period> &duration)
{
    m_asio_timer.expires_from_now(duration);
    m_asio_timer.wait();
}

Timer::Timer(IOContext &io_context)
{
    void *io_ctx_priv = io_context.getPriv();
    m_priv = new TimerPrivate(io_ctx_priv);
}

Timer::~Timer()
{
    if (m_priv) {
        TimerPrivate *d = static_cast<TimerPrivate *>(m_priv);
        if (d) {
            delete d;
            m_priv = NULL;
        }
    }
}

void Timer::asyncWait(const std::chrono::seconds &duration,
                      const std::function<void(int error)> &callback)
{
    INIT_PRIV(d);

    d->asyncWait(duration, callback);
}

void Timer::asyncWait(const std::chrono::milliseconds &duration,
                      const std::function<void(int error)> &callback)
{
    INIT_PRIV(d);

    d->asyncWait(duration, callback);
}

void Timer::asyncWait(const std::chrono::microseconds &duration,
                      const std::function<void(int error)> &callback)
{
    INIT_PRIV(d);

    d->asyncWait(duration, callback);
}

void Timer::asyncWait(const std::chrono::seconds &duration,
                      std::function<void(int error)> &&callback)
{
    INIT_PRIV(d);

    d->asyncWait(duration, std::move(callback));
}

void Timer::asyncWait(const std::chrono::milliseconds &duration,
                      std::function<void(int error)> &&callback)
{
    INIT_PRIV(d);

    d->asyncWait(duration, std::move(callback));
}

void Timer::asyncWait(const std::chrono::microseconds &duration,
                      std::function<void(int error)> &&callback)
{
    INIT_PRIV(d);

    d->asyncWait(duration, std::move(callback));
}

size_t Timer::cancel()
{
    INIT_PRIV(d);

    return d->cancel();
}

void Timer::wait(const std::chrono::seconds &duration)
{
    INIT_PRIV(d);

    d->wait(duration);
}

void Timer::wait(const std::chrono::milliseconds &duration)
{
    INIT_PRIV(d);

    d->wait(duration);
}

void Timer::wait(const std::chrono::microseconds &duration)
{
    INIT_PRIV(d);

    d->wait(duration);
}

} // End of namespace io
} // End of namespace base
