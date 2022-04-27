#include "base/uuid.h"

#include "base/endian.h"

#if defined(OS_WIN)
#include <objbase.h> // For CoCreateGuid
#else
#include <uuid/uuid.h>
#endif

#include <assert.h>
#include <ctype.h>

namespace base {

/*
 * Code from Qt/qtbase
 */
inline char toHexLower(uint32_t value)
{
    return "0123456789abcdef"[value & 0xF];
}

inline int fromHex(uint32_t c)
{
    return ((c >= '0') && (c <= '9')) ? int(c - '0') :
           ((c >= 'A') && (c <= 'F')) ? int(c - 'A' + 10) :
           ((c >= 'a') && (c <= 'f')) ? int(c - 'a' + 10) :
           -1;  // otherwise
}

template <class Char, class Integral>
void _q_toHex(Char *&dst, Integral value)
{
    switch (sizeof(Integral)) {
    case 1 : break;
    case 2 : value = hostToNetwork16(value); break;
    case 4 : value = hostToNetwork32(value); break;
    case 8 : value = hostToNetwork64(value); break;
    }

    const char *p = reinterpret_cast<const char *>(&value);

    for (uint32_t i = 0; i < sizeof(Integral); ++i, dst += 2) {
        dst[0] = Char(toHexLower((p[i] >> 4) & 0xf));
        dst[1] = Char(toHexLower(p[i] & 0xf));
    }
}

template <class Char, class Integral>
bool _q_fromHex(const Char *&src, Integral &value)
{
    value = 0;

    for (uint32_t i = 0; i < sizeof(Integral) * 2; ++i) {
        uint32_t ch = *src++;
        int tmp = fromHex(ch);
        if (tmp == -1)
            return false;

        value = value * 16 + tmp;
    }

    return true;
}

template <class Char>
void _q_uuidToHex(Char *&dst,
                  const uint32_t &d1, const uint16_t &d2, const uint16_t &d3,
                  const uint8_t (&d4)[8])
{
    _q_toHex(dst, d1);
    *dst++ = Char('-');
    _q_toHex(dst, d2);
    *dst++ = Char('-');
    _q_toHex(dst, d3);
    *dst++ = Char('-');
    for (int i = 0; i < 2; i++)
        _q_toHex(dst, d4[i]);
    *dst++ = Char('-');
    for (int i = 2; i < 8; i++)
        _q_toHex(dst, d4[i]);
}

template <class Char>
bool _q_uuidFromHex(const Char *src, uint32_t &d1, uint16_t &d2, uint16_t &d3, uint8_t (&d4)[8])
{
    if (!_q_fromHex(src, d1)
            || *src++ != Char('-')
            || !_q_fromHex(src, d2)
            || *src++ != Char('-')
            || !_q_fromHex(src, d3)
            || *src++ != Char('-')
            || !_q_fromHex(src, d4[0])
            || !_q_fromHex(src, d4[1])
            || *src++ != Char('-')
            || !_q_fromHex(src, d4[2])
            || !_q_fromHex(src, d4[3])
            || !_q_fromHex(src, d4[4])
            || !_q_fromHex(src, d4[5])
            || !_q_fromHex(src, d4[6])
            || !_q_fromHex(src, d4[7])) {
        return false;
    }

    return true;
}

#define INIT_PRIV(d) \
    UuidPrivate *d = NULL; \
    if (m_priv) { \
        d = static_cast<UuidPrivate *>(m_priv); \
    } \
    assert(d); \


class UuidPrivate
{
public:
    UuidPrivate();
    UuidPrivate(const std::string &str);
    UuidPrivate(const uint8_t bytes[16]);
    UuidPrivate(const ByteArray &bytes);
    UuidPrivate(const UuidPrivate &other);

    void generateInPlace();

    bool isNull() const;
    ByteArray toByteArray() const;
    std::string toString(bool upper = false) const;
    const std::string &string() const;

    UuidPrivate &operator=(const UuidPrivate &other);
    bool operator<(const UuidPrivate &other) const;
    bool operator==(const UuidPrivate &other) const;
    int compare(const UuidPrivate &other) const;

private:
    uint32_t  m_data1;
    uint16_t  m_data2;
    uint16_t  m_data3;
    uint8_t   m_data4[8];

    std::string m_uuid_string; // lowercase string
}; // End of class UuidPrivate

UuidPrivate::UuidPrivate()
{
    m_data1 = 0;
    m_data2 = 0;
    m_data3 = 0;
    for (int i = 0; i < 8; ++i) {
        m_data4[i] = 0;
    }
}

UuidPrivate::UuidPrivate(const std::string &str)
{
    if (!_q_uuidFromHex(str.c_str(), m_data1, m_data2, m_data3, m_data4)) {
        m_data1 = 0;
        m_data2 = 0;
        m_data3 = 0;
        for (int i = 0; i < 8; ++i) {
            m_data4[i] = 0;
        }
    }

    m_uuid_string = std::move(toString());
}

UuidPrivate::UuidPrivate(const uint8_t bytes[16])
{
    const uint8_t *p = &(bytes[0]);
    m_data1 = BigEndian::get32bits(p);
    p += sizeof(m_data1);
    m_data2 = BigEndian::get16bits(p);
    p += sizeof(m_data2);
    m_data3 = BigEndian::get16bits(p);
    p += sizeof(m_data3);

    for (int i = 0; i < 8; ++i) {
        m_data4[i] = *(p + i);
    }

    m_uuid_string = std::move(toString());
}

UuidPrivate::UuidPrivate(const ByteArray &bytes)
{
    uint8_t buf[16];
    if (bytes.size() < sizeof(buf)) {
        m_data1 = 0;
        m_data2 = 0;
        m_data3 = 0;
        for (int i = 0; i < 8; ++i) {
            m_data4[i] = 0;
        }

        return;
    }

    memcpy(buf, bytes.constData(), sizeof(buf));

    *this = UuidPrivate(buf);
}

UuidPrivate::UuidPrivate(const UuidPrivate &other)
{
    m_data1 = other.m_data1;
    m_data2 = other.m_data2;
    m_data3 = other.m_data3;
    for (int i = 0; i < 8; ++i) {
        m_data4[i] = other.m_data4[i];
    }

    m_uuid_string = other.m_uuid_string;
}

void UuidPrivate::generateInPlace()
{
#if defined(OS_WIN)
    GUID guid;
    CoCreateGuid(&guid);

    m_data1 = guid.Data1;
    m_data2 = guid.Data2;
    m_data3 = guid.Data3;
    for (int i = 0; i < 8; i++) {
        m_data4[i] = guid.Data4[i];
    }

    m_uuid_string = std::move(toString());
#else
    uuid_t uuid;
    uuid_generate(uuid);

    char buf[40];
    uuid_unparse_lower(uuid, buf);
    *this = UuidPrivate(buf);
#endif
}

bool UuidPrivate::isNull() const
{
    return m_data4[0] == 0 && m_data4[1] == 0 && m_data4[2] == 0 && m_data4[3] == 0 &&
           m_data4[4] == 0 && m_data4[5] == 0 && m_data4[6] == 0 && m_data4[7] == 0 &&
           m_data1 == 0 && m_data2 == 0 && m_data3 == 0;
}

ByteArray UuidPrivate::toByteArray() const
{
    char buf[16];
    char *p = (char *)buf;

    BigEndian::set32bits(p, m_data1);
    p += sizeof(m_data1);
    BigEndian::set16bits(p, m_data2);
    p += sizeof(m_data2);
    BigEndian::set16bits(p, m_data3);
    p += sizeof(m_data3);

    memcpy(p, m_data4, sizeof(m_data4));

    ByteArray ba((const char *)buf, sizeof(buf));

    return ba;
}

std::string UuidPrivate::toString(bool upper) const
{
    char buf[40];
    memset(buf, 0, sizeof(buf));

    char *p = (char *)buf;
    _q_uuidToHex(p, m_data1, m_data2, m_data3, m_data4);

    if (upper) {
        for (int i = 0; i < sizeof(buf); ++i) {
            buf[i] = toupper(buf[i]);
        }
    }

    return buf;
}

const std::string &UuidPrivate::string() const
{
    return m_uuid_string;
}

UuidPrivate &UuidPrivate::operator=(const UuidPrivate &other)
{
    m_data1 = other.m_data1;
    m_data2 = other.m_data2;
    m_data3 = other.m_data3;
    for (int i = 0; i < 8; ++i) {
        m_data4[i] = other.m_data4[i];
    }

    m_uuid_string = other.m_uuid_string;

    return *this;
}

bool UuidPrivate::operator<(const UuidPrivate &other) const
{
    if (m_data1 != other.m_data1) {
        return m_data1 < other.m_data1;
    }

    if (m_data2 != other.m_data2) {
        return m_data2 < other.m_data2;
    }

    if (m_data3 != other.m_data3) {
        return m_data3 < other.m_data3;
    }

    for (int i = 0; i < 8; ++i) {
        if (m_data4[i] != other.m_data4[i]) {
            return m_data4[i] < other.m_data4[i];
        }
    }

    return false;
}

bool UuidPrivate::operator==(const UuidPrivate &other) const
{
    if (m_data1 != other.m_data1
            || m_data2 != other.m_data2
            || m_data3 != other.m_data3) {
        return false;
    }

    for (int i = 0; i < 8; ++i) {
        if (m_data4[i] != other.m_data4[i]) {
            return false;
        }
    }

    return true;
}

int UuidPrivate::compare(const UuidPrivate &other) const
{
    if (m_data1 > other.m_data1) {
        return 1;
    } else if (m_data1 < other.m_data1) {
        return -1;
    }

    if (m_data2 > other.m_data2) {
        return 1;
    } else if (m_data2 < other.m_data2) {
        return -1;
    }

    if (m_data3 > other.m_data3) {
        return 1;
    } else if (m_data3 < other.m_data3) {
        return -1;
    }

    for (int i = 0; i < 8; ++i) {
        if (m_data4[i] > other.m_data4[i]) {
            return 1;
        } else if (m_data4[i] < other.m_data4[i]) {
            return -1;
        }
    }

    return 0;
}

Uuid Uuid::generate()
{
    Uuid uuid;

    UuidPrivate *d = static_cast<UuidPrivate *>(uuid.m_priv);
    d->generateInPlace();

    return uuid;
}

Uuid::Uuid()
    : m_priv(new UuidPrivate)
{
}

Uuid::Uuid(const std::string &str)
    : m_priv(new UuidPrivate(str))
{
}

Uuid::Uuid(const uint8_t bytes[16])
    : m_priv(new UuidPrivate(bytes))
{
}

Uuid::Uuid(const ByteArray &bytes)
    : m_priv(new UuidPrivate(bytes))
{
}

Uuid::Uuid(const Uuid &other)
    : m_priv(new UuidPrivate(
                 *static_cast<UuidPrivate *>(other.m_priv)))
{
}

Uuid::~Uuid()
{
    if (m_priv) {
        UuidPrivate *d = static_cast<UuidPrivate *>(m_priv);
        if (d) {
            delete d;
            m_priv = NULL;
        }
    }
}

bool Uuid::isNull() const
{
    INIT_PRIV(d);

    return d->isNull();
}

ByteArray Uuid::toByteArray() const
{
    INIT_PRIV(d);

    return d->toByteArray();
}

std::string Uuid::toString(bool upper) const
{
    INIT_PRIV(d);

    return d->toString(upper);
}

const std::string &Uuid::string() const
{
    INIT_PRIV(d);

    return d->string();
}

Uuid &Uuid::operator=(const Uuid &other)
{
    // Check for self-assignment!
    if (this == &other)      // Same object?
        return *this;        // Yes, so skip assignment, and just return *this.

    INIT_PRIV(d);

    UuidPrivate *d2 = static_cast<UuidPrivate *>(other.m_priv);
    assert(d2);

    (*d) = (*d2);

    return *this;
}

bool Uuid::operator<(const Uuid &other) const
{
    INIT_PRIV(d);

    return (*d) < (*static_cast<UuidPrivate *>(other.m_priv));
}

bool Uuid::operator==(const Uuid &other) const
{
    INIT_PRIV(d);

    return (*d) == (*static_cast<UuidPrivate *>(other.m_priv));
}

bool Uuid::operator!=(const Uuid &other) const
{
    return !(*this == other);
}

int Uuid::compare(const Uuid &other) const
{
    INIT_PRIV(d);

    return (*d).compare(*static_cast<UuidPrivate *>(other.m_priv));
}

} // End of namespace base
