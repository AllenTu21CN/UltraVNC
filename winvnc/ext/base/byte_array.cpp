 #include "base/byte_array.h"

#include <memory>

#include <string.h>
#include <assert.h>
#include <stdexcept>
namespace base {

#define INIT_PRIV(d) \
    ByteArrayPrivate *d = NULL; \
    if (m_priv) { \
        d = static_cast<ByteArrayPrivate *>(m_priv); \
    } \
    assert(d); \


class ByteArrayPrivate
{
public:
    ByteArrayPrivate();
    ByteArrayPrivate(const char *data, size_t size);
    ByteArrayPrivate(size_t size, char ch);
    ByteArrayPrivate(const ByteArrayPrivate &other);
    ~ByteArrayPrivate();

    size_t getConstStart();
    bool setConstStart(size_t pos);

    void append(size_t count, char ch);
    void append(const char *data, size_t len);

    char at(size_t i) const;

    void clear();

    const char *constData() const;
    char *data();

    void fill(char ch);
    void fill(char ch, size_t size);

    void insert(size_t i, size_t count, char ch);
    void insert(size_t i, const char *data, size_t len);

    void prepend(size_t count, char ch);
    void prepend(const char *data, size_t len);

    void remove(size_t pos, size_t len);
    size_t size() const;

    ByteArrayPrivate &operator=(const ByteArrayPrivate &other);
    char &operator[](size_t i);

private:
    void ensureCapacity(size_t capacity);

    size_t m_const_start;
    size_t m_size;
    size_t m_capacity;
    std::unique_ptr<char[]> m_data;
};

ByteArrayPrivate::ByteArrayPrivate() :
    m_const_start(0),
    m_size(0),
    m_capacity(0),
    m_data(nullptr)
{}

ByteArrayPrivate::ByteArrayPrivate(const char *data, size_t size) : 
    m_const_start(0),
    m_size(size),
    m_capacity((size+1023)/1024*1024),
    m_data(new char[m_capacity])
{
    memcpy(m_data.get(), data, m_size);
}

ByteArrayPrivate::ByteArrayPrivate(size_t len, char ch) : 
    m_const_start(0),
    m_size(len),
    m_capacity((len+1023)/1024*1024),
    m_data(new char[m_capacity])
{
    memset(m_data.get(), ch, m_size);
}

ByteArrayPrivate::ByteArrayPrivate(const ByteArrayPrivate &other) : 
    m_const_start(other.m_const_start),
    m_size(other.m_size),
    m_capacity(other.m_capacity),
    m_data(new char[m_capacity])
{
    memcpy(m_data.get(), other.m_data.get(), m_size);
}

ByteArrayPrivate::~ByteArrayPrivate()
{}

void ByteArrayPrivate::ensureCapacity(size_t capacity)
{
    // buffer is enough
    if (capacity <= m_capacity) return;

    // not enough, re-new buffer
    const size_t new_capacity = (capacity + 1023) / 1024 * 1024;

    // todo, check @new is suc, will, 2017/12/20
    std::unique_ptr<char[]> new_data(new char[new_capacity]);
    memcpy(new_data.get(), m_data.get(), m_size);

    // update new buffer
    m_data     = std::move(new_data);
    m_capacity = new_capacity;
}

size_t ByteArrayPrivate::getConstStart()
{
    return m_const_start;
}

bool ByteArrayPrivate::setConstStart(size_t pos)
{
    if (pos > m_size)
        return false;
    m_const_start = pos;
    return true;
}

void ByteArrayPrivate::append(size_t count, char ch)
{
    size_t new_size = m_size + count;
    // check buffer is enough
    ensureCapacity(new_size);

    memset(m_data.get() + m_size, ch, count);
    m_size = new_size;
}

void ByteArrayPrivate::append(const char *data, size_t len)
{
    size_t new_size = m_size + len;
    // check buffer is enough
    ensureCapacity(new_size);

    memcpy(m_data.get() + m_size, data, len);
    m_size = new_size;
}

char ByteArrayPrivate::at(size_t i) const
{
    i += m_const_start;
    if (i >= m_size) {
        throw std::out_of_range("ByteArray index out of range.");
    }

    return m_data[i];
}

void ByteArrayPrivate::clear()
{
    m_size = m_const_start;
}

const char *ByteArrayPrivate::constData() const
{
    return m_data.get() + m_const_start;
}

char *ByteArrayPrivate::data()
{
    return m_data.get() + m_const_start;
}

void ByteArrayPrivate::fill(char ch)
{
    if (m_size == m_const_start) return;

    memset(m_data.get() + m_const_start, ch, m_size - m_const_start);
}

void ByteArrayPrivate::fill(char ch, size_t size)
{
    if (size == 0) return;

    // check the buffer is enough
    ensureCapacity(size + m_const_start);

    memset(m_data.get() + m_const_start, ch, size);
    m_size = size + m_const_start;
}

void ByteArrayPrivate::insert(size_t i, size_t count, char ch)
{
    i += m_const_start;
#if 0
    if (i < 0) {
        throw std::out_of_range("ByteArray insert index out of range.");
    }
#endif
    // calc new data size: + insert size
    size_t new_size = m_size + count;
    if (new_size < i + count) new_size = i + count;

    ensureCapacity(new_size);

    // buffer
    //               i       
    //               |<- count ->|
    // +-------------+-----------+------+
    // |             |                  |
    // +-------------+-----------+------+
    // 
    // |<----- m_size ---->|
    //
    // |<---------- new_size ---------->|
    //
    // move
    if (i < m_size) {
        memmove(m_data.get() + i + count, m_data.get() + i, m_size - i);
    } 

    // set 
    memset(m_data.get() + i, ch, count);

    // update data size
    m_size = new_size;
}

void ByteArrayPrivate::insert(size_t i, const char *data, size_t len)
{
    i += m_const_start;
#if 0
    if (i < 0) {
        throw std::out_of_range("ByteArray insert index out of range.");
    }
#endif
    // calc new data size: + insert size
    size_t new_size = m_size + len;
    if (new_size < i + len) new_size = i + len;

    ensureCapacity(new_size);

    // buffer
    //               i       
    //               |<-  len  ->|
    // +-------------+-----------+------+
    // |             |                  |
    // +-------------+-----------+------+
    // 
    // |<----- m_size ---->|
    //
    // |<---------- new_size ---------->|
    //
    // move
    if (i < m_size) {
        memmove(m_data.get() + i + len, m_data.get() + i, m_size - i);
    }

    // copy
    memcpy(m_data.get() + i, data, len);

    // update data size
    m_size = new_size;
}

void ByteArrayPrivate::prepend(size_t count, char ch)
{
    // calc new data size
    size_t new_size = m_size + count;
    ensureCapacity(new_size);

    // move
    memmove(m_data.get() + m_const_start + count, m_data.get() + m_const_start, m_size - m_const_start);

    // set
    memset(m_data.get() + m_const_start, ch, count);

    // update data size
    m_size = new_size;
}

void ByteArrayPrivate::prepend(const char *data, size_t len)
{
    // calc new data size
    size_t new_size = m_size + len;
    ensureCapacity(new_size);

    // move
    memmove(m_data.get() + m_const_start + len, m_data.get() + m_const_start, m_size - m_const_start);

    // copy
    memcpy(m_data.get() + m_const_start, data, len);

    // update data size
    m_size = new_size;
}

void ByteArrayPrivate::remove(size_t pos, size_t len)
{
    pos += m_const_start;

    // start offset is out of range and whether len is valid
    if (pos >= m_size || len <= 0) return;

    // check len is more than m_size
    if (pos + len >= m_size) {
        len = m_size - pos;
    }

    // buffer
    //              pos        
    //               |<-- len -->|
    // +-------------+-----------+------+
    // |             |   remove  |      |
    // +-------------+-----------+------+
    //
    // |<----------- m_size ----------->|
    //

    // left size of data
    size_t left = (m_size - pos) - len;

    // move data and update data size
    memmove(m_data.get() + pos, m_data.get() + pos + len, left);
    
    m_size -= len;
}

size_t ByteArrayPrivate::size() const
{
    return m_size - m_const_start;
}

ByteArrayPrivate &ByteArrayPrivate::operator=(const ByteArrayPrivate &other)
{
    m_const_start = other.m_const_start;
    m_size     = other.m_size;
    m_capacity = other.m_capacity;

    m_data.reset(new char[m_capacity]);

    memcpy(m_data.get(), other.m_data.get(), m_size);

    return *this;
}

char &ByteArrayPrivate::operator[](size_t i)
{
    i += m_const_start;
    if (i >= m_size) {
        throw std::out_of_range("ByteArray index out of range.");
    }

    return m_data[i];
}

ByteArray::ByteArray()
    : m_priv(new ByteArrayPrivate)
{
}

ByteArray::ByteArray(const char *data, size_t size)
    : m_priv(new ByteArrayPrivate(data, size))
{
}

ByteArray::ByteArray(size_t size, char ch)
    : m_priv(new ByteArrayPrivate(size, ch))
{
}

ByteArray::ByteArray(const ByteArray &other)
    : m_priv(new ByteArrayPrivate(
                 *static_cast<ByteArrayPrivate *>(other.m_priv)))
{
    //printf("Invoke ByteArray Copy-Constructor.\n");
}

ByteArray::ByteArray(ByteArray &&other)
{
    m_priv = other.m_priv;
    other.m_priv = new ByteArrayPrivate;
}

ByteArray::~ByteArray()
{
    if (m_priv) {
        ByteArrayPrivate *d = static_cast<ByteArrayPrivate *>(m_priv);
        if (d) {
            delete d;
            m_priv = NULL;
        }
    }
}

size_t ByteArray::getConstStart()
{
    INIT_PRIV(d);

    return d->getConstStart();
}

bool ByteArray::setConstStart(size_t pos)
{
    INIT_PRIV(d);

    return d->setConstStart(pos);
}

ByteArray &ByteArray::append(const ByteArray &ba)
{
    INIT_PRIV(d);

    d->append(ba.constData(), ba.size());
    return *this;
}

ByteArray &ByteArray::append(size_t count, char ch)
{
    INIT_PRIV(d);

    d->append(count, ch);
    return *this;
}

ByteArray &ByteArray::append(const char *data, size_t len)
{
    INIT_PRIV(d);

    d->append(data, len);
    return *this;
}

char ByteArray::at(size_t i) const
{
    INIT_PRIV(d);

    return d->at(i);
}

void ByteArray::clear()
{
    INIT_PRIV(d);

    d->clear();
}

const char *ByteArray::constData() const
{
    INIT_PRIV(d);
    return d->constData();
}

char *ByteArray::data()
{
    INIT_PRIV(d);
    return d->data();
}

const char *ByteArray::data() const
{
    return constData();
}

ByteArray &ByteArray::fill(char ch)
{
    INIT_PRIV(d);

    d->fill(ch);
    return *this;
}

ByteArray &ByteArray::fill(char ch, size_t size)
{
    INIT_PRIV(d);

    d->fill(ch, size);
    return *this;
}

ByteArray &ByteArray::insert(size_t i, const ByteArray &ba)
{
    INIT_PRIV(d);

    d->insert(i, ba.constData(), ba.size());
    return *this;
}

ByteArray &ByteArray::insert(size_t i, size_t count, char ch)
{
    INIT_PRIV(d);

    d->insert(i, count, ch);
    return *this;
}

ByteArray &ByteArray::insert(size_t i, const char *data, size_t len)
{
    INIT_PRIV(d);

    d->insert(i, data, len);
    return *this;
}

ByteArray ByteArray::mid(size_t pos)
{
    if (pos >= size()) {
        return ByteArray();
    }

    char *p = data() + pos;
    return ByteArray(p, size() - pos);
}

ByteArray ByteArray::mid(size_t pos, size_t len)
{
    if (pos >= size()) {
        return ByteArray();
    }

    if (pos + len >= size()) {
        return mid(pos);
    }

    char *p = data() + pos;
    return ByteArray(p, len);
}

ByteArray &ByteArray::prepend(const ByteArray &ba)
{
    INIT_PRIV(d);

    d->prepend(ba.constData(), ba.size());
    return *this;
}

ByteArray &ByteArray::prepend(size_t count, char ch)
{
    INIT_PRIV(d);

    d->prepend(count, ch);
    return *this;
}

ByteArray &ByteArray::prepend(const char *data, size_t len)
{
    INIT_PRIV(d);

    d->prepend(data, len);
    return *this;
}

ByteArray &ByteArray::remove(size_t pos, size_t len)
{
    INIT_PRIV(d);

    d->remove(pos, len);
    return *this;
}

size_t ByteArray::size() const
{
    INIT_PRIV(d);

    return d->size();
}

bool ByteArray::startsWith(const ByteArray &ba) const
{
    return startsWith(ba.constData(), ba.size());
}

bool ByteArray::startsWith(const char *data, size_t len) const
{
    if (len <= size()) {
        return (0 == memcmp(data, constData(), len));
    } else {
        return false;
    }
}

void ByteArray::swap(ByteArray &other)
{
    std::swap(m_priv, other.m_priv);
}

ByteArray &ByteArray::operator+=(const ByteArray &ba)
{
    append(ba);
    return *this;
}

ByteArray &ByteArray::operator=(const ByteArray &other)
{
    // Check for self-assignment!
    if (this == &other)      // Same object?
        return *this;        // Yes, so skip assignment, and just return *this.

    INIT_PRIV(d);

    ByteArrayPrivate *d2 = static_cast<ByteArrayPrivate *>(other.m_priv);
    assert(d2);

    (*d) = (*d2);

    return *this;
}

char &ByteArray::operator[](size_t i)
{
    INIT_PRIV(d);

    return (*d)[i];
}

void ByteArray::dumpHex(int line_size) const
{
    for (size_t i = 0; i < this->size(); ++i) {
        if (0 == i % line_size) {
            printf("    \n");
        }
        printf("%02x ", (uint8_t)(this->at(i)));
    }
    printf("\n");
}

} // End of namespace base
