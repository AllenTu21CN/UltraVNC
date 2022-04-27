#include "base/io/io_context.h"

#include <asio.hpp>

namespace base {
namespace io {

bool errorIsCancelled(int error)
{
    return asio::error::operation_aborted == error;
}

#define INIT_PRIV(d) \
    IOContextPrivate *d = NULL; \
    if (m_priv) { \
        d = static_cast<IOContextPrivate *>(m_priv); \
    } \
    assert(d); \


class IOContextPrivate
{
public:
    IOContextPrivate();
    ~IOContextPrivate();

    void dispatch(const std::function<void()> &callback);
    void dispatch(std::function<void()> &&callback);

    void post(const std::function<void()> &callback);
    void post(std::function<void()> &&callback);

    void run();
    void runOnce();

    void stop();
    bool stopped() const;

    void *getImpl();

private:
    asio::io_service m_io_srv;

}; // End of class IOContextPrivate

IOContextPrivate::IOContextPrivate()
{
}

IOContextPrivate::~IOContextPrivate()
{
}

void IOContextPrivate::dispatch(const std::function<void()> &callback)
{
    m_io_srv.dispatch(callback);
}

void IOContextPrivate::dispatch(std::function<void()> &&callback)
{
    m_io_srv.dispatch(std::move(callback));
}

void IOContextPrivate::post(const std::function<void()> &callback)
{
    m_io_srv.post(callback);
}

void IOContextPrivate::post(std::function<void()> &&callback)
{
    m_io_srv.post(std::move(callback));
}

void IOContextPrivate::run()
{
    m_io_srv.run();
}

void IOContextPrivate::runOnce()
{
    m_io_srv.run_one();
}

void IOContextPrivate::stop()
{
    m_io_srv.stop();
}

bool IOContextPrivate::stopped() const
{
    return m_io_srv.stopped();
}

void *IOContextPrivate::getImpl()
{
    return &m_io_srv;
}

IOContext::IOContext()
    : m_priv(new IOContextPrivate)
{
}

IOContext::~IOContext()
{
    if (m_priv) {
        IOContextPrivate *d = static_cast<IOContextPrivate *>(m_priv);
        if (d) {
            delete d;
            m_priv = NULL;
        }
    }
}

void IOContext::dispatch(const std::function<void()> &callback)
{
    INIT_PRIV(d);

    d->dispatch(callback);
}

void IOContext::dispatch(std::function<void()> &&callback)
{
    INIT_PRIV(d);

    d->dispatch(std::move(callback));
}

void IOContext::post(const std::function<void()> &callback)
{
    INIT_PRIV(d);

    d->post(callback);
}

void IOContext::post(std::function<void()> &&callback)
{
    INIT_PRIV(d);

    d->post(std::move(callback));
}

void IOContext::run()
{
    INIT_PRIV(d);

    d->run();
}

void IOContext::runOnce()
{
    INIT_PRIV(d);

    d->runOnce();
}

void IOContext::stop()
{
    INIT_PRIV(d);

    d->stop();
}

bool IOContext::stopped() const
{
    INIT_PRIV(d);

    return d->stopped();
}

void *IOContext::getPriv()
{
    INIT_PRIV(d);

    return d->getImpl();
}

} // End of namespace io
} // End of namespace base
