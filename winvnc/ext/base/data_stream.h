#pragma once

namespace base {

/**
 * The DataStream class provides serialization of binary data to a IODevice or ByteBuffer.
 */
class DataStream
{
public:
    DataStream();

private:
    void *m_priv;

}; // End of class DataStream

} // End of namespace base
