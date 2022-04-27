#pragma once

#include <stdlib.h>
#include <stdint.h>

namespace base {

// Reading and writing of little and big-endian numbers from memory
// TODO: Optimized versions, with direct read/writes of
// integers in host-endian format, when the platform supports it.

class BigEndian
{
public:
    static inline void set8bits(void *memory, size_t offset, uint8_t v)
    {
        static_cast<uint8_t *>(memory)[offset] = v;
    }

    static inline void set16bits(void *memory, uint16_t v)
    {
        set8bits(memory, 0, static_cast<uint8_t>(v >> 8));
        set8bits(memory, 1, static_cast<uint8_t>(v >> 0));
    }

    static inline void set24bits(void *memory, uint32_t v)
    {
        set8bits(memory, 0, static_cast<uint8_t>(v >> 16));
        set8bits(memory, 1, static_cast<uint8_t>(v >> 8));
        set8bits(memory, 2, static_cast<uint8_t>(v >> 0));
    }

    static inline void set32bits(void *memory, uint32_t v)
    {
        set8bits(memory, 0, static_cast<uint8_t>(v >> 24));
        set8bits(memory, 1, static_cast<uint8_t>(v >> 16));
        set8bits(memory, 2, static_cast<uint8_t>(v >> 8));
        set8bits(memory, 3, static_cast<uint8_t>(v >> 0));
    }

    static inline void set64bits(void *memory, uint64_t v)
    {
        set8bits(memory, 0, static_cast<uint8_t>(v >> 56));
        set8bits(memory, 1, static_cast<uint8_t>(v >> 48));
        set8bits(memory, 2, static_cast<uint8_t>(v >> 40));
        set8bits(memory, 3, static_cast<uint8_t>(v >> 32));
        set8bits(memory, 4, static_cast<uint8_t>(v >> 24));
        set8bits(memory, 5, static_cast<uint8_t>(v >> 16));
        set8bits(memory, 6, static_cast<uint8_t>(v >> 8));
        set8bits(memory, 7, static_cast<uint8_t>(v >> 0));
    }

    static inline uint8_t get8bits(const void *memory, size_t offset)
    {
        return static_cast<const uint8_t *>(memory)[offset];
    }

    static inline uint16_t get16bits(const void *memory)
    {
        return static_cast<uint16_t>((get8bits(memory, 0) << 8) |
                                     (get8bits(memory, 1) << 0));
    }

    static inline uint32_t get24bits(const void *memory)
    {
        return (static_cast<uint32_t>(get8bits(memory, 0)) << 16) |
               (static_cast<uint32_t>(get8bits(memory, 1)) << 8) |
               (static_cast<uint32_t>(get8bits(memory, 2)) << 0);
    }

    static inline uint32_t get32bits(const void *memory)
    {
        return (static_cast<uint32_t>(get8bits(memory, 0)) << 24) |
               (static_cast<uint32_t>(get8bits(memory, 1)) << 16) |
               (static_cast<uint32_t>(get8bits(memory, 2)) << 8) |
               (static_cast<uint32_t>(get8bits(memory, 3)) << 0);
    }

    static inline uint64_t get64bits(const void *memory)
    {
        return (static_cast<uint64_t>(get8bits(memory, 0)) << 56) |
               (static_cast<uint64_t>(get8bits(memory, 1)) << 48) |
               (static_cast<uint64_t>(get8bits(memory, 2)) << 40) |
               (static_cast<uint64_t>(get8bits(memory, 3)) << 32) |
               (static_cast<uint64_t>(get8bits(memory, 4)) << 24) |
               (static_cast<uint64_t>(get8bits(memory, 5)) << 16) |
               (static_cast<uint64_t>(get8bits(memory, 6)) << 8) |
               (static_cast<uint64_t>(get8bits(memory, 7)) << 0);
    }

protected:
    BigEndian();

}; // End of class BigEndian

class LittleEndian
{
public:
    static inline void set8bits(void *memory, size_t offset, uint8_t v)
    {
        static_cast<uint8_t *>(memory)[offset] = v;
    }

    static inline void set16bits(void *memory, uint16_t v)
    {
        set8bits(memory, 0, static_cast<uint8_t>(v >> 0));
        set8bits(memory, 1, static_cast<uint8_t>(v >> 8));
    }

    static inline void set24bits(void *memory, uint32_t v)
    {
        set8bits(memory, 0, static_cast<uint8_t>(v >> 0));
        set8bits(memory, 1, static_cast<uint8_t>(v >> 8));
        set8bits(memory, 2, static_cast<uint8_t>(v >> 16));
    }

    static inline void set32bits(void *memory, uint32_t v)
    {
        set8bits(memory, 0, static_cast<uint8_t>(v >> 0));
        set8bits(memory, 1, static_cast<uint8_t>(v >> 8));
        set8bits(memory, 2, static_cast<uint8_t>(v >> 16));
        set8bits(memory, 3, static_cast<uint8_t>(v >> 24));
    }

    static inline void set64bits(void *memory, uint64_t v)
    {
        set8bits(memory, 0, static_cast<uint8_t>(v >> 0));
        set8bits(memory, 1, static_cast<uint8_t>(v >> 8));
        set8bits(memory, 2, static_cast<uint8_t>(v >> 16));
        set8bits(memory, 3, static_cast<uint8_t>(v >> 24));
        set8bits(memory, 4, static_cast<uint8_t>(v >> 32));
        set8bits(memory, 5, static_cast<uint8_t>(v >> 40));
        set8bits(memory, 6, static_cast<uint8_t>(v >> 48));
        set8bits(memory, 7, static_cast<uint8_t>(v >> 56));
    }

    static inline uint8_t get8bits(const void *memory, size_t offset)
    {
        return static_cast<const uint8_t *>(memory)[offset];
    }

    static inline uint16_t get16bits(const void *memory)
    {
        return static_cast<uint16_t>((get8bits(memory, 0) << 0) |
                                     (get8bits(memory, 1) << 8));
    }

    static inline uint32_t get24bits(const void *memory)
    {
        return (static_cast<uint32_t>(get8bits(memory, 0)) << 0) |
               (static_cast<uint32_t>(get8bits(memory, 1)) << 8) |
               (static_cast<uint32_t>(get8bits(memory, 2)) << 16);
    }

    static inline uint32_t get32bits(const void *memory)
    {
        return (static_cast<uint32_t>(get8bits(memory, 0)) << 0) |
               (static_cast<uint32_t>(get8bits(memory, 1)) << 8) |
               (static_cast<uint32_t>(get8bits(memory, 2)) << 16) |
               (static_cast<uint32_t>(get8bits(memory, 3)) << 24);
    }

    static inline uint64_t get64bits(const void *memory)
    {
        return (static_cast<uint64_t>(get8bits(memory, 0)) << 0) |
               (static_cast<uint64_t>(get8bits(memory, 1)) << 8) |
               (static_cast<uint64_t>(get8bits(memory, 2)) << 16) |
               (static_cast<uint64_t>(get8bits(memory, 3)) << 24) |
               (static_cast<uint64_t>(get8bits(memory, 4)) << 32) |
               (static_cast<uint64_t>(get8bits(memory, 5)) << 40) |
               (static_cast<uint64_t>(get8bits(memory, 6)) << 48) |
               (static_cast<uint64_t>(get8bits(memory, 7)) << 56);
    }

protected:
    LittleEndian();

}; // End of class LittleEndian

// Check if the current host is big endian.
inline bool isHostBigEndian()
{
    static const int number = 1;
    return 0 == *reinterpret_cast<const char*>(&number);
}

inline uint16_t hostToNetwork16(uint16_t n)
{
    uint16_t result = n;
    if (!isHostBigEndian()) {
        BigEndian::set16bits(&result, n);
    }
    return result;
}

inline uint32_t hostToNetwork32(uint32_t n)
{
    uint32_t result = n;
    if (!isHostBigEndian()) {
        BigEndian::set32bits(&result, n);
    }
    return result;
}

inline uint64_t hostToNetwork64(uint64_t n)
{
    uint64_t result = n;
    if (!isHostBigEndian()) {
        BigEndian::set64bits(&result, n);
    }
    return result;
}

inline uint16_t networkToHost16(uint16_t n)
{
    if (isHostBigEndian()) { 
        return n;
    } else { 
        return BigEndian::get16bits(&n);
    }
}

inline uint32_t networkToHost32(uint32_t n)
{
    if (isHostBigEndian()) {
        return n;
    } else {
        return BigEndian::get32bits(&n);
    }
}

inline uint64_t networkToHost64(uint64_t n)
{
    if (isHostBigEndian()) {
        return n;
    } else {
        return BigEndian::get64bits(&n);
    }
}

}  // End of namespace base
