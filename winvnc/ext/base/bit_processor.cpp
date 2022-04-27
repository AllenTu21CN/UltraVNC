#include "base/bit_processor.h"     
#include "base/log.h"
#include "base/endian.h"
#include "media/base/mediadata_util.h"
#include <limits.h>
#include <assert.h>
#include <algorithm>
#include <string.h>
#include <functional>
#include <map>

namespace media {

#define INIT_EXTRACTER_PRIV(d) \
    BitExtractorPrivate *d = NULL; \
    if (m_priv) { \
        d = static_cast<BitExtractorPrivate *>(m_priv); \
    } \
    assert(d); \

class BitExtractorPrivate 
{
public:
    BitExtractorPrivate() :
        m_buffer(NULL), m_buffer_end(NULL), m_index(0), m_size_in_bits(0) {
        m_handler_be[1]  = std::bind(base::BigEndian::get8bits, std::placeholders::_1, 0);
        m_handler_be[2] = base::BigEndian::get16bits;
        m_handler_be[3] = base::BigEndian::get24bits;
        m_handler_be[4] = base::BigEndian::get32bits;

        m_handler_le[1] = std::bind(base::LittleEndian::get8bits, std::placeholders::_1, 0);
        m_handler_le[2] = base::LittleEndian::get16bits;
        m_handler_le[3] = base::LittleEndian::get24bits;
        m_handler_le[4] = base::LittleEndian::get32bits;
    }
    
    int initExtractor(const uint8_t *buffer, int bit_size);

    void skipBits(int n);

    int bitsReaded();
    int bitsRemain();

    int readBitsBE(int n, uint32_t &v);
    int readBitsLE(int n, uint32_t &v);
    int peekBitsBE(int n, uint32_t &v);
    int peekBitsLE(int n, uint32_t &v);

private:
    int clip(int a, int amin, int amax);
    int readBitsbe(int n, uint32_t &v);
    int readBitsle(int n, uint32_t &v);
    int peekBitsbe(int n, uint32_t &v);
    int peekBitsle(int n, uint32_t &v);
    int convert(int n);

private:
    typedef std::function<uint32_t(const void*)> ReadHandler;

    const uint8_t*    m_buffer;
    const uint8_t*    m_buffer_end;
    int               m_index;
    int               m_size_in_bits;
    std::map<int, ReadHandler> m_handler_be;
    std::map<int, ReadHandler> m_handler_le;
};

int BitExtractorPrivate::initExtractor(const uint8_t *buffer, int bit_size)
{
    if (bit_size >= INT_MAX - 7 || bit_size < 0 || !buffer) {
        return -1;
    }

    int buffer_size      = (bit_size + 7) >> 3;
    m_buffer             = buffer;
    m_size_in_bits       = bit_size;
    m_buffer_end         = buffer + buffer_size;
    m_index              = 0;
    return 0;
}

int BitExtractorPrivate::clip(int a, int amin, int amax)
{
    if (a < amin) {
        return amin;
    } else if (a > amax) {
        return amax;
    } else {
        return a;
    }
}

int BitExtractorPrivate::bitsRemain()
{
    return m_size_in_bits - bitsReaded();
}

int BitExtractorPrivate::bitsReaded()
{
    return m_index;
}

void BitExtractorPrivate::skipBits(int n)
{
    m_index += clip(n, -m_index, m_size_in_bits - m_index);
}

int BitExtractorPrivate::readBitsBE(int n, uint32_t &value)
{
    int ret = -1;
    if (n < 0 || n > 32) {
        return ret;
    }

    if (!n) {
        value = 0;
        return 0;
    } else if (n <= 16) {
        return readBitsbe(n, value);
    } else {
        int ret = readBitsbe(16, value);
        if (ret < 0) {
            return -1;
        }
        value <<= (n - 16);

        uint32_t tmp = 0;
        ret = readBitsbe(n - 16, tmp);
        if (ret < 0) {
            return -1;
        }

        value = value | tmp;
        return 0;
    }
}

int BitExtractorPrivate::readBitsLE(int n, uint32_t &value)
{
    int ret = -1;
    if (n < 0 || n > 32) {
        return ret;
    }

    if (!n) {
        value = 0;
        return 0;
    } else if (n <= 16) {
        return readBitsle(n, value);
    } else {
        int ret = readBitsle(16, value);
        if (ret < 0) {
            return -1;
        }

        uint32_t tmp = 0;
        ret = readBitsle(n - 16, tmp);
        if (ret < 0) {
            return -1;
        }

        value = value | tmp << 16;
        return 0;
    }
}

int BitExtractorPrivate::peekBitsBE(int n, uint32_t &value)
{
    int ret = -1;
    if (n < 0 || n > 32) {
        return ret;
    }

    if (!n) {
        value = 0;
        return 0;
    } else if (n <= 16) {
        return peekBitsbe(n, value);
    } else {
        uint32_t tmp = 0;

        int ret = peekBitsbe(16, value) << (n - 16);
        if (ret < 0) {
            return -1;
        }

        ret = peekBitsbe(n - 16, tmp);
        if (ret < 0) {
            return -1;
        }

        value = value | tmp;
        return 0;
    }
}

int BitExtractorPrivate::peekBitsLE(int n, uint32_t &value)
{
    int ret = -1;
    if (n < 0 || n > 32) {
        return ret;
    }

    if (!n) {
        value = 0;
        return 0;
    } else if (n <= 16) {
        return peekBitsle(n, value);
    } else {
        uint32_t tmp = 0;

        int ret = peekBitsle(16, value) << (n - 16);
        if (ret < 0) {
            return -1;
        }

        ret = peekBitsle(n - 16, tmp);
        if (ret < 0) {
            return -1;
        }

        value = value | tmp;
        return 0;
    }
}

int BitExtractorPrivate::readBitsbe(int n, uint32_t &v)
{
    if (n <= 0 || n > 25) {
        return -1;
    }

    int32_t  bytes_remain = bitsRemain();
    if (bytes_remain <= 0 || bytes_remain < n) {
        return -1;
    }

    uint32_t tmp;
    uint32_t re_index = m_index;
    uint32_t re_size_plus8 = m_size_in_bits;

    int index = convert(n);
    if (index < 0) {
        return -1;
    }

    ReadHandler handle = m_handler_be[index];
    switch (index)
    {
    case 1: {
        uint8_t re_cache = (uint8_t)handle(m_buffer + (re_index >> 3)) << (re_index & 7);
        tmp = ((uint8_t)(re_cache)) >> (8 - n);
    }
        break;
    case 2: {
        uint16_t re_cache = (uint16_t)handle(m_buffer + (re_index >> 3)) << (re_index & 7);
        tmp = ((uint16_t)(re_cache)) >> (16 - n);
    }
        break;
    case 3: {
        uint32_t re_cache = (uint32_t)handle(m_buffer + (re_index >> 3)) << (8 + (re_index & 7));
        tmp = ((uint32_t)(re_cache)) >> (24 + 8 - n);
    }
        break;
    case 4: {
        uint32_t re_cache = (uint32_t)handle(m_buffer + (re_index >> 3)) << (re_index & 7);
        tmp = ((uint32_t)(re_cache)) >> (32 - n);
    }
        break;
    default:
        break;
    }

    re_index = std::min(re_size_plus8, re_index + n);
    m_index = re_index;

    v = tmp;
    return 0;
}

int BitExtractorPrivate::readBitsle(int n, uint32_t &v)
{
    if (n <= 0 || n > 25) {
        return -1;
    }

    int32_t  bytes_remain = bitsRemain();
    if (bytes_remain <= 0 || bytes_remain < n) {
        return -1;
    }

    uint32_t tmp;
    uint32_t re_index = m_index;
    uint32_t re_size_plus8 = m_size_in_bits;

    int index = convert(n);
    if (index < 0) {
        return -1;
    }

    ReadHandler handle = m_handler_le[index];
    switch (index)
    {
    case 1: {
        uint8_t re_cache = (uint8_t)handle(m_buffer + (re_index >> 3)) << (re_index & 7);
        tmp = ((uint8_t)(re_cache)) >> (8 - n);
    }
            break;
    case 2: {
        uint16_t re_cache = (uint16_t)handle(m_buffer + (re_index >> 3)) << (re_index & 7);
        tmp = ((uint16_t)(re_cache)) >> (16 - n);
    }
            break;
    case 3: {
        uint32_t re_cache = (uint32_t)handle(m_buffer + (re_index >> 3)) << (8 + (re_index & 7));
        tmp = ((uint32_t)(re_cache)) >> (24 + 8 - n);
    }
            break;
    case 4: {
        uint32_t re_cache = (uint32_t)handle(m_buffer + (re_index >> 3)) << (re_index & 7);
        tmp = ((uint32_t)(re_cache)) >> (32 - n);
    }
            break;
    default:
        break;
    }

    re_index = std::min(re_size_plus8, re_index + n);
    m_index = re_index;

    v = tmp;
    return 0;
}

int BitExtractorPrivate::peekBitsbe(int n, uint32_t &v)
{
    if (n <= 0 || n > 25) {
        return -1;
    }

    int32_t  bytes_remain = bitsRemain();
    if (bytes_remain <= 0 || bytes_remain < n) {
        return -1;
    }

    uint32_t tmp;
    uint32_t re_index = m_index;
    uint32_t re_size_plus8 = m_size_in_bits;

    int index = convert(n);
    if (index < 0) {
        return -1;
    }

    ReadHandler handle = m_handler_be[index];
    switch (index)
    {
    case 1: {
        uint8_t re_cache = (uint8_t)handle(m_buffer + (re_index >> 3)) << (re_index & 7);
        tmp = ((uint8_t)(re_cache)) >> (8 - n);
    }
            break;
    case 2: {
        uint16_t re_cache = (uint16_t)handle(m_buffer + (re_index >> 3)) << (re_index & 7);
        tmp = ((uint16_t)(re_cache)) >> (16 - n);
    }
            break;
    case 3: {
        uint32_t re_cache = (uint32_t)handle(m_buffer + (re_index >> 3)) << (8 + (re_index & 7));
        tmp = ((uint32_t)(re_cache)) >> (24 + 8 - n);
    }
            break;
    case 4: {
        uint32_t re_cache = (uint32_t)handle(m_buffer + (re_index >> 3)) << (re_index & 7);
        tmp = ((uint32_t)(re_cache)) >> (32 - n);
    }
            break;
    default:
        break;
    }

    re_index = std::min(re_size_plus8, re_index + n);

    v = tmp;
    return 0;
}

int BitExtractorPrivate::peekBitsle(int n, uint32_t &v)
{
    if (n <= 0 || n > 25) {
        return -1;
    }

    int32_t  bytes_remain = bitsRemain();
    if (bytes_remain <= 0 || bytes_remain < n) {
        return -1;
    }

    uint32_t tmp;
    uint32_t re_index = m_index;
    uint32_t re_size_plus8 = m_size_in_bits;

    int index = convert(n);
    if (index < 0) {
        return -1;
    }

    ReadHandler handle = m_handler_be[index];
    switch (index)
    {
    case 1: {
        uint8_t re_cache = (uint8_t)handle(m_buffer + (re_index >> 3)) << (re_index & 7);
        tmp = ((uint8_t)(re_cache)) >> (8 - n);
    }
            break;
    case 2: {
        uint16_t re_cache = (uint16_t)handle(m_buffer + (re_index >> 3)) << (re_index & 7);
        tmp = ((uint16_t)(re_cache)) >> (16 - n);
    }
            break;
    case 3: {
        uint32_t re_cache = (uint32_t)handle(m_buffer + (re_index >> 3)) << (8 + (re_index & 7));
        tmp = ((uint32_t)(re_cache)) >> (24 + 8 - n);
    }
            break;
    case 4: {
        uint32_t re_cache = (uint32_t)handle(m_buffer + (re_index >> 3)) << (re_index & 7);
        tmp = ((uint32_t)(re_cache)) >> (32 - n);
    }
            break;
    default:
        break;
    }

    re_index = std::min(re_size_plus8, re_index + n);

    v = tmp;
    return 0;
}

int BitExtractorPrivate::convert(int n)
{
    uint32_t re_index = m_index;
    int32_t left_bit = (8 - (m_index & 7)) % 8;

    int ret = (n - left_bit + 7) >> 3;
    ret = ret + (!left_bit ? 0 : 1);
    if (ret < 1 || ret > 4) {
        return -1;
    }
    return ret;
}

BitExtractor::BitExtractor()
    : m_priv(new BitExtractorPrivate())
{

}

BitExtractor::~BitExtractor()
{
    if (m_priv) {
        BitExtractorPrivate *d = static_cast<BitExtractorPrivate *>(m_priv);
        if (d) {
            delete d;
            m_priv = NULL;
        }
    }
}

int BitExtractor::initExtractor(const uint8_t *buffer, int bit_size)
{
    INIT_EXTRACTER_PRIV(d);

    return d->initExtractor(buffer, bit_size);
}

void BitExtractor::skipBits(int n)
{
    INIT_EXTRACTER_PRIV(d);

    return d->skipBits(n);
}
int BitExtractor::bitsReaded()
{
    INIT_EXTRACTER_PRIV(d);

    return d->bitsReaded();
}

int BitExtractor::bitsRemain()
{
    INIT_EXTRACTER_PRIV(d);

    return d->bitsRemain();
}

int BitExtractor::readBitsBE(int n, uint32_t &v)
{
    INIT_EXTRACTER_PRIV(d);

    return d->readBitsBE(n, v);
}

int BitExtractor::readBitsLE(int n, uint32_t &v)
{
    INIT_EXTRACTER_PRIV(d);

    return d->readBitsLE(n, v);
}

int BitExtractor::peekBitsBE(int n, uint32_t &v)
{
    INIT_EXTRACTER_PRIV(d);

    return d->peekBitsBE(n, v);
}

int BitExtractor::peekBitsLE(int n, uint32_t &v)
{
    INIT_EXTRACTER_PRIV(d);

    return d->peekBitsLE(n, v);
}

#define INIT_ASSEMBLER_PRIV(d) \
    BitAssemblerPrivate *d = NULL; \
    if (m_priv) { \
        d = static_cast<BitAssemblerPrivate *>(m_priv); \
    } \
    assert(d); \


class BitAssemblerPrivate
{
public:
    BitAssemblerPrivate();
    ~BitAssemblerPrivate();

    int initBitAssembler(uint8_t *buffer, int buffer_size);
    void paddingBits();

    int bitsWrited();
    int bitsRemain();

    int writeBitBE(int n, uint32_t value);
    void flushBE();

    int writeBitLE(int n, uint32_t value);
    void flushLE();

private:
    uint32_t    m_bit_buf;
    int32_t     m_bit_left;
    uint8_t*    m_buf;
    uint8_t*    m_buf_ptr;
    uint8_t*    m_buf_end;
    int32_t     m_size_in_bits;

};

BitAssemblerPrivate::BitAssemblerPrivate()
    : m_buf(NULL), m_buf_ptr(NULL), m_buf_end(NULL), m_bit_buf(0), m_bit_left(32)
{

}

BitAssemblerPrivate::~BitAssemblerPrivate()
{

}

void BitAssemblerPrivate::paddingBits()
{
    writeBitBE(m_bit_left & 7, 0);
}

int BitAssemblerPrivate::initBitAssembler(uint8_t *buffer, int buffer_size)
{
    if (buffer_size < 0 || !buffer) {
        return -1;
    }

    m_size_in_bits = 8 * buffer_size;
    m_buf          = buffer;
    m_buf_end      = m_buf + buffer_size;
    m_buf_ptr      = m_buf;
    m_bit_left     = 32;
    m_bit_buf      = 0;
    return 0;
}

int BitAssemblerPrivate::bitsWrited()
{
    return (m_buf_ptr - m_buf) * 8 + 32 - m_bit_left;
}

int BitAssemblerPrivate::bitsRemain()
{
    return (m_buf_end - m_buf_ptr) * 8 - 32 + m_bit_left;
}

int BitAssemblerPrivate::writeBitBE(int n, uint32_t value)
{
    if (n > bitsRemain() || n > 32 || n < 0 || value >= (1U << n)) {
        return -1;
    }

    uint32_t bit_buf = m_bit_buf;
    int bit_left     = m_bit_left;
    if (n < bit_left) {
        bit_buf   = (bit_buf << n) | value;
        bit_left -= n;
    } else {
        bit_buf = (uint64_t)bit_buf << bit_left;
        bit_buf |= value >> (n - bit_left);
        if (m_buf_end - m_buf_ptr > 3) {
            base::BigEndian::set32bits(m_buf_ptr, bit_buf);
            m_buf_ptr += 4;
        } else {
            base::_error("Internal error, bit assembler buffer too small.");
            return -1;
        }
        bit_left += 32 - n;
        bit_buf   = value;
    }

    m_bit_buf  = bit_buf;
    m_bit_left = bit_left;
    return 0;
}

void BitAssemblerPrivate::flushBE()
{
    if (m_bit_left < 32) {
        m_bit_buf <<= m_bit_left;
    }

    while (m_bit_left < 32) {
        *m_buf_ptr++ = m_bit_buf >> 24;
        m_bit_buf <<= 8;
        m_bit_left += 8;
    }

    m_bit_left = 32;
    m_bit_buf  =  0;
}

int BitAssemblerPrivate::writeBitLE(int n, uint32_t value)
{
    if (n > bitsRemain() || n > 32 || n < 0 || value >= (1U << n)) {
        return -1;
    }

    uint32_t bit_buf  = m_bit_buf;
    int32_t  bit_left = m_bit_left;
    bit_buf |= value << (32 - bit_left);
    if (n >= bit_left) {
        if (3 < m_buf_end - m_buf_ptr) {
            base::LittleEndian::set32bits(m_buf_ptr, bit_buf);
            m_buf_ptr += 4;
        } else {
            base::_error("Internal error, buffer too small.");
            return -1;
        }
        bit_buf = (uint64_t)value >> bit_left;
        bit_left += 32;
    }

    bit_left -= n;

    m_bit_buf = bit_buf;
    m_bit_left = bit_left;
    return 0;
}

void BitAssemblerPrivate::flushLE()
{
    while (m_bit_left < 32) {
        *m_buf_ptr++ = m_bit_buf;
        m_bit_buf >>= 8;
        m_bit_left += 8;
    }
    m_bit_left = 32;
    m_bit_buf = 0;
}

BitAssembler::BitAssembler()
    :m_priv(new BitAssemblerPrivate())
{

}

BitAssembler::~BitAssembler()
{
    if (m_priv) {
        BitAssemblerPrivate *d = static_cast<BitAssemblerPrivate *>(m_priv);
        if (d) {
            delete d;
            m_priv = NULL;
        }
    }
}

int BitAssembler::initBitAssembler(uint8_t *buffer, int buffer_size)
{
    INIT_ASSEMBLER_PRIV(d);

    return d->initBitAssembler(buffer, buffer_size);
}

int BitAssembler::bitsWrited()
{
    INIT_ASSEMBLER_PRIV(d);

    return d->bitsWrited();
}

int BitAssembler::bitsRemain()
{
    INIT_ASSEMBLER_PRIV(d);

    return d->bitsRemain();
}

int BitAssembler::writeBitBE(int n, uint32_t value)
{
    INIT_ASSEMBLER_PRIV(d);

    return d->writeBitBE(n, value);
}

void BitAssembler::flushBE()
{
    INIT_ASSEMBLER_PRIV(d);

    d->flushBE();
}

int BitAssembler::writeBitLE(int n, uint32_t value)
{
    INIT_ASSEMBLER_PRIV(d);

    return d->writeBitLE(n, value);
}

void BitAssembler::flushLE()
{
    INIT_ASSEMBLER_PRIV(d);

    d->flushLE();
}

void BitAssembler::paddingBits()
{
    INIT_ASSEMBLER_PRIV(d);

    d->paddingBits();
}

} //End of namespace base
