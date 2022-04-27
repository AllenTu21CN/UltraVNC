#pragma once

#include <string>

namespace base {
namespace gfx {

class Size
{
public:
    Size() : m_width(0), m_height(0) {}
    Size(int width, int height) : m_width(width), m_height(height) {}
    Size(const Size &other) : m_width(other.m_width), m_height(other.m_height) {}

    bool operator==(const Size &other) const
    { return m_width == other.m_width && m_height == other.m_height; }

    bool operator!=(const Size &other) const { return !(*this == other); }

    Size &operator=(const Size &other)
    {
        m_width = other.m_width;
        m_height = other.m_height;
        return *this;
    }

    inline int width() const { return m_width; }
    inline int height() const { return m_height; }

    inline void setWidth(int width) { m_width = width; }
    inline void setHeight(int height) { m_height = height; }
    inline void setSize(int width, int height) { m_width = width; m_height = height; }

    inline bool isValid() const { return m_width > 0 && m_height > 0; }

    /**
     * Format: <width><unit><connector><height><unit>
     */
    std::string toString(const char *unit = "", const char *connector = "x") const
    {
        char buf[1024];
        snprintf(buf, sizeof(buf),
                 "%d%s%s%d%s",
                 m_width, unit, connector, m_height, unit);

        return buf;
    }

private:
    int m_width;
    int m_height;
}; // End of class Size

} // End of namespace gfx
} // End of namespace base
