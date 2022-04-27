#include "base/byte_fifo.h"
#include "base/byte_array.h"
#include "base/endian.h"
#include "base/log.h"

#include <assert.h>
#include <stdio.h>

#include <vector>

namespace base {

#define INIT_PRIV(d) \
    ByteFifoPrivate *d = NULL; \
    if (m_priv) { \
        d = static_cast<ByteFifoPrivate *>(m_priv); \
    } \
    assert(d); \


class ByteFifoPrivate
{
public:
    static const size_t DEFAULT_CAPACITY = 1023;

    ByteFifoPrivate(size_t initial_capacity = DEFAULT_CAPACITY);
    ~ByteFifoPrivate();

    void clear();

    size_t peekData(char *data, size_t max_length) const;

    size_t popData(char *data, size_t max_length);

    void pushData(const char *data, size_t length);

    size_t size() const;

protected:
    void resize(size_t new_size);

private:
    ByteArray   m_buf;
    /**
     * Assume that the fifo is empty when:
     *   m_start == m_end
     *
     * Assume that the fifo is full when there is one empty slot left in the fifo:
     *   m_start == (m_end + 1) % m_buf.size()
     */
    size_t      m_start;    // Point to next byte to read
    size_t      m_end;      // Point to next byte to write
};

ByteFifoPrivate::ByteFifoPrivate(size_t initial_capacity)
    : m_buf(initial_capacity + 1, 0), m_start(0), m_end(0)
{
}

ByteFifoPrivate::~ByteFifoPrivate()
{
}

void ByteFifoPrivate::clear()
{
    m_start = m_end = 0;
}

size_t ByteFifoPrivate::peekData(char *data, size_t max_length) const
{
    if (0 == max_length) {
        return 0;
    }

    const size_t len = max_length <= size() ? max_length : size();
    const char *start = m_buf.data() + m_start;
    
    if (m_end >= m_start) {
        std::copy(start, start + len, data);
    } else {
        const char *buf_start = m_buf.data();     
        const char *buf_end = m_buf.data() + m_buf.size();
        if (m_start + len <= m_buf.size()) {
            std::copy(start, start + len, data);
        } else {
#if 0
            printf("buffer size = %d, m_start = %d, m_end = %d\tstart: %p buffer_end: %p \n",
                size(), m_start, m_end, start, buf_end);
#endif
            std::copy(start, buf_end, data);
            std::copy(buf_start, buf_start + (len - (buf_end - start)), data + (buf_end - start));
        }
    }

    return len;
}

size_t ByteFifoPrivate::popData(char *data, size_t max_length)
{
    size_t len = peekData(data, max_length);

    // Move start flag
    if (m_start + len < m_buf.size()) {
        m_start += len;
    } else {
        m_start = len - (m_buf.size() - m_start);
    }

    return len;
}

void ByteFifoPrivate::pushData(const char *data, size_t length)
{
    // Resize if necessary
    if (length > m_buf.size() - 1 - size()) {
        resize(m_buf.size() + 1 + length);
    }

    char *end = m_buf.data() + m_end;
    if (m_end >= m_start) {
        if (m_buf.size() - m_end >= length) {
            std::copy(data, data + length, end);
        } else {
            std::copy(data, data + (m_buf.size() - m_end), end);
            std::copy(data + (m_buf.size() - m_end), data + length, m_buf.data());
        }
    } else {
        std::copy(data, data + length, end);
    }

    m_end = (m_end + length) % m_buf.size();
}

size_t ByteFifoPrivate::size() const
{
    return (m_end + m_buf.size() - m_start) % m_buf.size();
}

void ByteFifoPrivate::resize(size_t new_size)
{
#if 0
    printf("Before resize...\n");
    m_buf.dumpHex();
#endif
    ByteArray new_buf(new_size, 0);

    size_t len = size();
    if (len > 0) {
        // Copy orignal data
        if (m_end > m_start) {
            std::copy(m_buf.data() + m_start, m_buf.data() + m_end, new_buf.data());
        } else {
#if 0
            printf("resize new buffer size = %d, m_start = %d, m_end = %d\n",
                new_buf.size(), m_start, m_end);
#endif
            std::copy(m_buf.data() + m_start, m_buf.data() + m_buf.size(), new_buf.data());
            std::copy(m_buf.data(), m_buf.data() + m_end, new_buf.data() + m_buf.size() - m_start);
        }
    }

    // Swap buffers
    m_buf.swap(new_buf);

    // Reset indices
    m_end = len;
    m_start = 0;
#if 0
    printf("After resize...\n");
    m_buf.dumpHex();
#endif
}

ByteFifo::ByteFifo()
    : m_priv(new ByteFifoPrivate)
{
}

ByteFifo::ByteFifo(size_t initial_capacity)
{
    if (initial_capacity > INT32_MAX - 1) {
        fprintf(stderr, "Initial capacity exceed INT32_MAX.\n");
        exit(-1);
    }

    m_priv = new ByteFifoPrivate(initial_capacity);
}

ByteFifo::~ByteFifo()
{
    if (m_priv) {
        ByteFifoPrivate *d = static_cast<ByteFifoPrivate *>(m_priv);
        if (d) {
            delete d;
            m_priv = NULL;
        }
    }
}

void ByteFifo::clear()
{
    INIT_PRIV(d);

    d->clear();
}

size_t ByteFifo::peekData(char *data, size_t max_length)
{
    INIT_PRIV(d);

    return d->peekData(data, max_length);
}

ByteArray ByteFifo::peekData(size_t max_length)
{
    ByteArray buf(max_length, 0);

    size_t n = peekData(buf.data(), max_length);
    return buf.mid(0, n);
}

size_t ByteFifo::popData(char *data, size_t max_length)
{
    INIT_PRIV(d);

    return d->popData(data, max_length);
}

ByteArray ByteFifo::popData(size_t max_length)
{
    ByteArray buf(max_length, 0);

    size_t n = popData(buf.data(), max_length);
    return buf.mid(0, n);
}

uint8_t ByteFifo::popBEUInt8(bool *ok)
{
    const size_t len = sizeof(uint8_t);
    if (len > size()) {
        if (ok) *ok = false;
        return 0;
    }

    char buf[len];
    popData(buf, len);

    if (ok) *ok = true;
    return BigEndian::get8bits(buf, 0);
}

uint16_t ByteFifo::popBEUInt16(bool *ok)
{
    const size_t len = sizeof(uint16_t);
    if (len > size()) {
        if (ok) *ok = false;
        return 0;
    }

    char buf[len];
    popData(buf, len);

    if (ok) *ok = true;
    return BigEndian::get16bits(buf);
}

uint32_t ByteFifo::popBEUInt24(bool *ok)
{
    const size_t len = sizeof(uint8_t) * 3;
    if (len > size()) {
        if (ok) *ok = false;
        return 0;
    }

    char buf[len];
    popData(buf, len);

    if (ok) *ok = true;
    return BigEndian::get24bits(buf);
}

uint32_t ByteFifo::popBEUInt32(bool *ok)
{
    const size_t len = sizeof(uint32_t);
    if (len > size()) {
        if (ok) *ok = false;
        return 0;
    }

    char buf[len];
    popData(buf, len);

    if (ok) *ok = true;
    return BigEndian::get32bits(buf);
}

uint64_t ByteFifo::popBEUInt64(bool *ok)
{
    const size_t len = sizeof(uint64_t);
    if (len > size()) {
        if (ok) *ok = false;
        return 0;
    }

    char buf[len];
    popData(buf, len);

    if (ok) *ok = true;
    return BigEndian::get64bits(buf);
}

uint8_t ByteFifo::popLEUInt8(bool *ok)
{
    const size_t len = sizeof(uint8_t);
    if (len > size()) {
        if (ok) *ok = false;
        return 0;
    }

    char buf[len];
    popData(buf, len);

    if (ok) *ok = true;
    return LittleEndian::get8bits(buf, 0);
}

uint16_t ByteFifo::popLEUInt16(bool *ok)
{
    const size_t len = sizeof(uint16_t);
    if (len > size()) {
        if (ok) *ok = false;
        return 0;
    }

    char buf[len];
    popData(buf, len);

    if (ok) *ok = true;
    return LittleEndian::get16bits(buf);
}

uint32_t ByteFifo::popLEUInt24(bool *ok)
{
    const size_t len = sizeof(uint8_t) * 3;
    if (len > size()) {
        if (ok) *ok = false;
        return 0;
    }

    char buf[len];
    popData(buf, len);

    if (ok) *ok = true;
    return LittleEndian::get24bits(buf);
}

uint32_t ByteFifo::popLEUInt32(bool *ok)
{
    const size_t len = sizeof(uint32_t);
    if (len > size()) {
        if (ok) *ok = false;
        return 0;
    }

    char buf[len];
    popData(buf, len);

    if (ok) *ok = true;
    return LittleEndian::get32bits(buf);
}

uint64_t ByteFifo::popLEUInt64(bool *ok)
{
    const size_t len = sizeof(uint64_t);
    if (len > size()) {
        if (ok) *ok = false;
        return 0;
    }

    char buf[len];
    popData(buf, len);

    if (ok) *ok = true;
    return LittleEndian::get64bits(buf);
}

void ByteFifo::pushData(const char *data, size_t length)
{
    INIT_PRIV(d);

    d->pushData(data, length);
}

void ByteFifo::pushData(const ByteArray &ba)
{
    pushData(ba.constData(), ba.size());
}

void ByteFifo::pushBEUInt8(uint8_t i)
{
    const size_t len = sizeof(uint8_t);

    char buf[len];
    BigEndian::set8bits(buf, 0, i);

    pushData(buf, len);
}

void ByteFifo::pushBEUInt16(uint16_t i)
{
    const size_t len = sizeof(uint16_t);

    char buf[len];
    BigEndian::set16bits(buf, i);

    pushData(buf, len);
}

void ByteFifo::pushBEUInt24(uint32_t i)
{
    const size_t len = sizeof(uint8_t) * 3;

    char buf[len];
    BigEndian::set24bits(buf, i);

    pushData(buf, len);
}

void ByteFifo::pushBEUInt32(uint32_t i)
{
    const size_t len = sizeof(uint32_t);

    char buf[len];
    BigEndian::set32bits(buf, i);

    pushData(buf, len);
}

void ByteFifo::pushBEUInt64(uint64_t i)
{
    const size_t len = sizeof(uint64_t);

    char buf[len];
    BigEndian::set64bits(buf, i);

    pushData(buf, len);
}

void ByteFifo::pushLEUInt8(uint8_t i)
{
    const size_t len = sizeof(uint8_t);

    char buf[len];
    LittleEndian::set8bits(buf, 0, i);

    pushData(buf, len);
}

void ByteFifo::pushLEUInt16(uint16_t i)
{
    const size_t len = sizeof(uint16_t);

    char buf[len];
    LittleEndian::set16bits(buf, i);

    pushData(buf, len);
}

void ByteFifo::pushLEUInt24(uint32_t i)
{
    const size_t len = sizeof(uint8_t) * 3;

    char buf[len];
    LittleEndian::set24bits(buf, i);

    pushData(buf, len);
}

void ByteFifo::pushLEUInt32(uint32_t i)
{
    const size_t len = sizeof(uint32_t);

    char buf[len];
    LittleEndian::set32bits(buf, i);

    pushData(buf, len);
}

void ByteFifo::pushLEUInt64(uint64_t i)

{
    const size_t len = sizeof(uint64_t);

    char buf[len];
    LittleEndian::set64bits(buf, i);

    pushData(buf, len);
}

size_t ByteFifo::size() const
{
    INIT_PRIV(d);

    return d->size();
}

} // End of namespace base
