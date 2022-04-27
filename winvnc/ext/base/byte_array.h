#pragma once

#include <string.h>

namespace base {
/**
 * The ByteArray class provides an array of bytes.
 */
class ByteArray
{
public:
    /**
     * Constructs an empty byte array.
     */
    ByteArray();

    /**
     * Constructs a byte array containing the first len bytes of array data.
     * If data is 0, a null byte array is constructed.
     *
     * ByteArray makes a deep copy of the string data.
     */
    ByteArray(const char *data, size_t len);

    /**
     * Constructs a byte array of size size with every byte set to character ch.
     */
    ByteArray(size_t size, char ch);

    /**
     * Constructs a copy of other.
     * ByteArray makes a deep copy of the other internal data.
     */
    ByteArray(const ByteArray &other);

    /**
     * Move constructor.
     */
    ByteArray(ByteArray &&other);

    /**
     * Destroys the byte array.
     */
    ~ByteArray();

    /**
     * Appends the byte array ba onto the end of this byte array and
     * returns a reference to this byte array.
     */
    ByteArray &append(const ByteArray &ba);

    /**
     * Appends count copies of character ch to this byte array and
     * returns a reference to this byte array.
     */
    ByteArray &append(size_t count, char ch);

    /**
     * Appends the first len characters of the raw data to this byte array and
     * returns a reference to this byte array.
     */
    ByteArray &append(const char *data, size_t len);

    /**
     * Returns the character at index position i in the byte array.
     * i must be a valid index position in the byte array (i.e., 0 <= i < size()).
     */
    char at(size_t i) const;

    /**
     * Clears the contents of the byte array and makes it null.
     */
    void clear();

    /**
     * Returns a pointer to the data stored in the byte array.
     * The pointer can be used to access the bytes that compose the array.
     */
    const char *constData() const;

    /**
     * Returns a pointer to the data stored in the byte array.
     * The pointer can be used to access and modify the bytes that compose the array.
     */
    char *data();

    /**
     * This is an overloaded function.
     */
    const char *data() const;

    /**
     * Sets every byte in the byte array to character ch.
     */
    ByteArray &fill(char ch);

    /**
     * Sets every byte in the byte array to character ch and resized to size.
     */
    ByteArray &fill(char ch, size_t size);

    /**
     * Inserts the byte array ba at index position i and
     * returns a reference to this byte array.
     * If i is greater than size(), the array is first extended.
     */
    ByteArray &insert(size_t i, const ByteArray &ba);

    /**
     * Inserts count copies of character ch at index position i in the byte array.
     * If i is greater than size(), the array is first extended.
     */
    ByteArray &insert(size_t i, size_t count, char ch);

    /**
     * Inserts len bytes of the raw data at position i in the byte array.
     * If i is greater than size(), the array is first extended.
     */
    ByteArray &insert(size_t i, const char *data, size_t len);

    /**
     * Returns a byte array containing all bytes starting at position pos until the end of
     * the byte array.
     */
    ByteArray mid(size_t pos);

    /**
     * Returns a byte array containing len bytes from this byte array, starting at position pos.
     * If pos + len >= size(), returns a byte array containing all bytes starting at position pos
     * until the end of the byte array.
     */
    ByteArray mid(size_t pos, size_t len);

    /**
     * Prepends the byte array ba to this byte array and
     * returns a reference to this byte array.
     */
    ByteArray &prepend(const ByteArray &ba);

    /**
     * Prepends count copies of character ch to this byte array and
     * returns a reference to this byte array.
     */
    ByteArray &prepend(size_t count, char ch);

    /**
     * Prepends len bytes of the raw data to this byte array.
     */
    ByteArray &prepend(const char *data, size_t len);

    /**
     * Removes len bytes from the array, starting at index position pos, and
     * returns a reference to the array.
     * If pos is out of range, nothing happens. If pos is valid, but pos + len
     * is larger than the size of the array, the array is truncated at position pos.
     */
    ByteArray &remove(size_t pos, size_t len);

    /**
     * Returns the number of bytes in this byte array.
     */
    size_t size() const;

    /**
     * Returns true if this byte array starts with byte array ba;
     * otherwise returns false.
     */
    bool startsWith(const ByteArray &ba) const;

    /**
     * Returns true if this byte array starts with len bytes of the raw data;
     * otherwise returns false.
     */
    bool startsWith(const char *data, size_t len) const;

    /**
     * Swaps byte array other with this byte array.
     */
    void swap(ByteArray &other);

    /**
     * Appends the byte array ba onto the end of this byte array and
     * returns a reference to this byte array.
     */
    ByteArray &operator+=(const ByteArray &ba);

    /**
     * Assigns other to this byte array and returns a reference to this byte array.
     * ByteArray makes a deep copy of the other internal data.
     */
    ByteArray &operator=(const ByteArray &other);

    /**
     * Returns the byte at index position i as a modifiable reference.
     * i must be a valid index position in the byte array (i.e., 0 <= i < size()).
     */
    char &operator[](size_t i);

    void dumpHex(int line_size = 16) const;

    /**
     * Set a constant start offset to hide those datas before it forever
     * unless set it to zero again.
     */
    bool setConstStart(size_t pos);

    /**
     * Get the constant start offset.
     */
    size_t getConstStart();

private:
    void *m_priv;

}; // End of class ByteArray

} // End of namespace base
