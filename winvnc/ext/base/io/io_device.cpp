#include "base/io/io_device.h"
#include "base/io/io_context.h"

#include <assert.h>

namespace base {
namespace io {

#define INIT_PRIV(d) \
    IODevicePrivate *d = NULL; \
    if (m_priv) { \
        d = static_cast<IODevicePrivate *>(m_priv); \
    } \
    assert(d); \


class IODevicePrivate
{
public:
    IODevicePrivate(IOContext &io_context);
    ~IODevicePrivate();

    IOContext &getIOContext();

    bool open(IODevice::OpenMode mode);
    void close();

    IODevice::OpenMode openMode() const;

private:
    IOContext &m_io_context;
    IODevice::OpenMode m_open_mode;

}; // End of class IOContextPrivate

IODevicePrivate::IODevicePrivate(IOContext &io_context)
    : m_io_context(io_context), m_open_mode(IODevice::NotOpen)
{
}

IODevicePrivate::~IODevicePrivate()
{
}

IOContext &IODevicePrivate::getIOContext()
{
    return m_io_context;
}

bool IODevicePrivate::open(IODevice::OpenMode mode)
{
    // Validate mode set
    if (mode == IODevice::NotOpen || m_open_mode != IODevice::NotOpen) {
        return false;
    }

    m_open_mode = mode;
    return true;
}

void IODevicePrivate::close()
{
    m_open_mode = IODevice::NotOpen;
}

IODevice::OpenMode IODevicePrivate::openMode() const
{
    return m_open_mode;
}

IODevice::IODevice(IOContext &io_context)
    : m_priv(new IODevicePrivate(io_context))
{
}

IODevice::IODevice(IODevice &&other)
{
    //fprintf(stderr, "IODevice move-constructor called!\n");
    m_priv = other.m_priv;

    // Following the move, the moved-from object is in the same state
    // as if constructed using the IODevice(io_context &) constructor.
    other.m_priv = new IODevicePrivate(getIOContext());
}

IODevice::~IODevice()
{
    if (m_priv) {
        IODevicePrivate *d = static_cast<IODevicePrivate *>(m_priv);
        if (d) {
            delete d;
            m_priv = NULL;
        }
    }
}

IOContext &IODevice::getIOContext() const
{
    INIT_PRIV(d);

    return d->getIOContext();
}

bool IODevice::open(OpenMode mode)
{
    INIT_PRIV(d);

    return d->open(mode);
}

void IODevice::close()
{
    INIT_PRIV(d);

    return d->close();
}

bool IODevice::seek(int64_t pos)
{
    return false;
}

IODevice::OpenMode IODevice::openMode() const
{
    INIT_PRIV(d);

    return d->openMode();
}

bool IODevice::isOpen() const
{
    INIT_PRIV(d);

    return NotOpen != d->openMode();
}
bool IODevice::isReadable() const
{
    INIT_PRIV(d);

    return ReadOnly & d->openMode();
}

bool IODevice::isTextModeEnabled() const
{
    INIT_PRIV(d);

    return (Text & d->openMode()) != 0;
}

bool IODevice::isWritable() const
{
    INIT_PRIV(d);

    return (WriteOnly & d->openMode()) != 0;
}

void *IODevice::getIOContextPriv()
{
    return getIOContext().getPriv();
}

} // End of namespace io
} // End of namespace base
