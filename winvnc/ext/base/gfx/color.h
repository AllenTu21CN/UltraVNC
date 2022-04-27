#pragma once

#include "base/log.h"

#include <stdint.h>
#include <string>

namespace base {
namespace gfx {
/**
 * The Color class provides colors based on RGBA values.
 */
class Color
{
public:
    Color();
    Color(int r, int g, int b, int a = 255);
    /**
     * @param name - predefined name or #RRGGBB/#RRGGBBAA
     *
     * Predefined color names.
     * Name         Description (value)
     * white        White (#ffffff)
     * black        Black (#000000)
     * red          Red (#ff0000)
     * darkRed      Dark red (#800000)
     * green        Green (#00ff00)
     * darkGreen    Dark green (#008000)
     * blue         Blue (#0000ff)
     * darkBlue     Dark blue (#000080)
     * cyan         Cyan (#00ffff)
     * darkCyan     Dark cyan (#008080)
     * magenta      Magenta (#ff00ff)
     * darkMagenta  Dark magenta (#800080)
     * yellow       Yellow (#ffff00)
     * darkYellow   Dark yellow (#808000)
     * gray         Gray (#a0a0a4)
     * darkGray     Dark gray (#808080)
     * lightGray    Light gray (#c0c0c0)
     * transparent  a transparent black value (i.e., Color(0, 0, 0, 0))
     */
    Color(const std::string &name);
    Color(const Color &other);

    uint8_t red() const { return m_rgba.red; }
    uint8_t green() const { return m_rgba.green; }
    uint8_t blue() const { return m_rgba.blue; }
    uint8_t alpha() const { return m_rgba.alpha; }

    int argb() const;
    int rgba() const;

    void setRed(uint8_t red) { m_rgba.red = red; }
    void setGreen(uint8_t green) { m_rgba.green = green; }
    void setBlue(uint8_t blue) { m_rgba.blue = blue; }
    void setAlpha(uint8_t alpha) { m_rgba.alpha = alpha; }

    Color & operator=(const Color &other);
    bool operator==(const Color &other) const;
    bool operator!=(const Color &other) const;

    std::string toString() const;

private:
    struct rgba {
        uint8_t red;
        uint8_t green;
        uint8_t blue;
        uint8_t alpha;
    } m_rgba;

    struct predefined_color {
        const char *name;
        struct rgba color;
    } m_predefined_colors[18] = {                       // #RRGGBBAA
        { "white",       { 0xff, 0xff, 0xff, 0xff } },  // #ffffffff
        { "black",       { 0x00, 0x00, 0x00, 0xff } },  // #000000ff
        { "red",         { 0xff, 0x00, 0x00, 0xff } },  // #ff0000ff
        { "darkRed",     { 0x80, 0x00, 0x00, 0xff } },  // #800000ff
        { "green",       { 0x00, 0xff, 0x00, 0xff } },  // #00ff00ff
        { "darkGreen",   { 0x00, 0x80, 0x00, 0xff } },  // #008000ff
        { "blue",        { 0x00, 0x00, 0xff, 0xff } },  // #0000ffff
        { "darkBlue",    { 0x00, 0x00, 0x80, 0xff } },  // #000080ff
        { "cyan",        { 0x00, 0xff, 0xff, 0xff } },  // #00ffffff
        { "darkCyan",    { 0x00, 0x80, 0x80, 0xff } },  // #008080ff
        { "magenta",     { 0xff, 0x00, 0xff, 0xff } },  // #ff00ffff
        { "darkMagenta", { 0x80, 0x00, 0x80, 0xff } },  // #800080ff
        { "yellow",      { 0xff, 0xff, 0x00, 0xff } },  // #ffff00ff
        { "darkYellow",  { 0x80, 0x80, 0x00, 0xff } },  // #808000ff
        { "gray",        { 0xa0, 0xa0, 0xa4, 0xff } },  // #a0a0a4ff
        { "darkGray",    { 0x80, 0x80, 0x80, 0xff } },  // #808080ff
        { "lightGray",   { 0xc0, 0xc0, 0xc0, 0xff } },  // #c0c0c0ff
        { "transparent", { 0x00, 0x00, 0x00, 0x00 } },  // #00000000
    };

}; // End of class Color

inline Color::Color()
{
    m_rgba.red      = 0;
    m_rgba.green    = 0;
    m_rgba.blue     = 0;
    m_rgba.alpha    = 255;
}

inline Color::Color(int r, int g, int b, int a)
{
    m_rgba.red      = r;
    m_rgba.green    = g;
    m_rgba.blue     = b;
    m_rgba.alpha    = a;
}

inline Color::Color(const std::string &name)
{
    // #RRGGBBAA or #RRGGBB
    std::string str = "000000ff";
    if ('#' == name[0] && (7 == name.size() || 9 == name.size())) {
        std::copy(name.begin() + 1, name.end(), str.begin());
        //base::_info("Color string: #%s", str.c_str());
    } else {
        for (auto &c : m_predefined_colors) {
            if (0 == strcmp(c.name, name.c_str())) {
                m_rgba.red      = c.color.red;
                m_rgba.green    = c.color.green;
                m_rgba.blue     = c.color.blue;
                m_rgba.alpha    = c.color.alpha;
                return;
            }
        }
    }

    unsigned long d = std::stoul(str, 0, 16);
    m_rgba.red      = (d >> 24) & 0xFF;
    m_rgba.green    = (d >> 16) & 0xFF;
    m_rgba.blue     = (d >> 8) & 0xFF;
    m_rgba.alpha    = (d) & 0xFF;
}

inline Color::Color(const Color &other)
{
    m_rgba.red      = other.m_rgba.red;
    m_rgba.green    = other.m_rgba.green;
    m_rgba.blue     = other.m_rgba.blue;
    m_rgba.alpha    = other.m_rgba.alpha;
}

inline int Color::argb() const
{
    return ((m_rgba.alpha & 0xff) << 24) |
           ((m_rgba.red   & 0xff) << 16) |
           ((m_rgba.green & 0xff) <<  8) |
            (m_rgba.blue  & 0xff);
}

inline int Color::rgba() const
{
    return ((m_rgba.red   & 0xff) << 24) |
           ((m_rgba.green & 0xff) << 16) |
           ((m_rgba.blue  & 0xff) <<  8) |
            (m_rgba.alpha & 0xff);
}

inline Color & Color::operator=(const Color &other)
{
    m_rgba.red = other.m_rgba.red;
    m_rgba.green = other.m_rgba.green;
    m_rgba.blue = other.m_rgba.blue;
    m_rgba.alpha = other.m_rgba.alpha;

    return *this;
}

inline bool Color::operator ==(const Color &other) const
{
    return m_rgba.red == other.m_rgba.red
            && m_rgba.green == other.m_rgba.green
            && m_rgba.blue == other.m_rgba.blue
            && m_rgba.alpha == other.m_rgba.alpha;
}

inline bool Color::operator !=(const Color &other) const
{
    return !((*this) == other);
}

inline std::string Color::toString() const
{
    char buf[10];
    sprintf(buf, "#%02x%02x%02x%02x",
            m_rgba.alpha, m_rgba.red, m_rgba.green, m_rgba.blue);

    return std::string(buf);
}

} // End of namespace gfx
} // End of namespace base
