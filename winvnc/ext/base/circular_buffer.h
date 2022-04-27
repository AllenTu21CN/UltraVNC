#pragma once

namespace base {

template<typename T>
class CircularBuffer
{
public:
    CircularBuffer();
    ~CircularBuffer();

    void clear();

private:
    void *m_priv;

}; // End of class CircularBuffer

} // End of namespace base
