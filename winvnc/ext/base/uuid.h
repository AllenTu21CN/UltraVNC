#pragma once

#include "base/byte_array.h"

#include <string>

namespace base {

class Uuid
{
public:
    /**
     * Generate a new Uuid.
     */
    static Uuid generate();

    /**
     * Creates the null UUID.
     * toString() will output the null UUID as "00000000-0000-0000-0000-000000000000".
     */
    Uuid();

    /**
     * Creates an Uuid object from the string text, which must be formatted as
     * five hex fields separated by '-', e.g., "xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx"
     * where 'x' is a hex digit. If the conversion fails, a null UUID is created.
     */
    Uuid(const std::string &str);

    /**
     * Creates an Uuid object from the raw binary bytes.
     * If the conversion fails, a null UUID is created.
     */
    Uuid(const uint8_t bytes[16]);
    Uuid(const ByteArray &bytes);

    /**
     * Copy constructor.
     */
    Uuid(const Uuid &other);

    /**
     * Destroy the Uuid object.
     */
    ~Uuid();

    /**
     * Returns true if this is the null UUID 00000000-0000-0000-0000-000000000000;
     * otherwise returns false.
     */
    bool isNull() const;

    /**
     * Returns the binary representation of this Uuid.
     */
    ByteArray toByteArray() const;

    /**
     * Returns the string representation of this Uuid.
     * The string is formatted as five hex fields separated by '-',
     * i.e., "xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx" where 'x' is a hex digit.
     */
    std::string toString(bool upper = false) const;

    /**
     * Returns uuid string use lowercase.
     */
    const std::string &string() const;

    /**
     *
     */
    Uuid &operator=(const Uuid &other);

    /**
     * Returns true if this Uuid has the same variant field as the other Uuid
     * and is lexicographically before the other Uuid.
     * If the other Uuid has a different variant field, the return value is
     * determined by comparing the two variants.
     */
    bool operator<(const Uuid &other) const;

    /**
     * Returns true if this Uuid and the other Uuid are identical; otherwise returns false.
     */
    bool operator==(const Uuid &other) const;
    bool operator!=(const Uuid &other) const;

    /**
     * Returns 1 if this Uuid is larger than the other;
     * returns 0 if this Uuid is equal with the other;
     * otherwise returns -1.
     */
    int compare(const Uuid &other) const;

private:
    void *m_priv;

}; // End of class Uuid

} // End of namespace base
