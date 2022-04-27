#pragma once

#include <string.h>
#include <stdint.h>

namespace base {

class ByteArray;

class ByteFifo
{
public:
    /**
     * Construct an empty byte fifo.
     */
    ByteFifo();

    /**
     * Construct an empty byte fifo with initial capacity.
     */
    ByteFifo(size_t initial_capacity);

    /**
     * Destroy the byte fifo.
     */
    ~ByteFifo();

    /**
     * Remove all bytes from the fifo.
     */
    void clear();

    /**
     * Copy at most max_length bytes from the front of the fifo to data and returns
     * number of bytes copied without side effects.
     * If max_length > size(), copies all bytes in the fifo.
     */
    size_t peekData(char *data, size_t max_length);

    /**
     * Remove at most max_length bytes from the front of the fifo without side effects.
     * This is an overload function.
     */
    ByteArray peekData(size_t max_length);

    /**
     * Pop at most max_length bytes from the front of the fifo to data,
     * returns number of bytes popped.
     * If max_length > size(), pops all bytes in the fifo.
     */
    size_t popData(char *data, size_t max_length);

    /**
     * Remove at most max_length bytes from the front of the fifo.
     * This is an overload function.
     */
    ByteArray popData(size_t max_length);

    /**
     * Pop first N bytes and return as unsigned integer in big endian.
     * If N <= size(), *ok will be true, otherwise false.
     */
    uint8_t popBEUInt8(bool *ok = NULL);
    uint16_t popBEUInt16(bool *ok = NULL);
    uint32_t popBEUInt24(bool *ok = NULL);
    uint32_t popBEUInt32(bool *ok = NULL);
    uint64_t popBEUInt64(bool *ok = NULL);

    /**
     * Pop first N bytes and return as unsigned integer in little endian.
     * If N <= size(), *ok will be true, otherwise false.
     */
    uint8_t popLEUInt8(bool *ok = NULL);
    uint16_t popLEUInt16(bool *ok = NULL);
    uint32_t popLEUInt24(bool *ok = NULL);
    uint32_t popLEUInt32(bool *ok = NULL);
    uint64_t popLEUInt64(bool *ok = NULL);

    /**
     * Push the length of data to the end of the fifo.
     */
    void pushData(const char *data, size_t length);

    /**
     * Push the byte array to the end of the fifo.
     */
    void pushData(const ByteArray &ba);

    /**
     * Push unsigned integer to the end of fifo in big endian.
     */
    void pushBEUInt8(uint8_t i);
    void pushBEUInt16(uint16_t i);
    void pushBEUInt24(uint32_t i);
    void pushBEUInt32(uint32_t i);
    void pushBEUInt64(uint64_t i);

    /**
     * Push unsigned integer to the end of fifo in little endian.
     */
    void pushLEUInt8(uint8_t i);
    void pushLEUInt16(uint16_t i);
    void pushLEUInt24(uint32_t i);
    void pushLEUInt32(uint32_t i);
    void pushLEUInt64(uint64_t i);

    /**
     * Return the number of bytes in this byte fifo.
     */
    size_t size() const;

private:
    void *m_priv;

}; // End of class ByteFifo

} // End of namespace base
