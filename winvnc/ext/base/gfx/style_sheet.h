#pragma once

#include "color.h"

#include <string>

namespace base {
namespace gfx {

class StyleSheet
{
public:
    StyleSheet();
    StyleSheet(const std::string &text);
    StyleSheet(const StyleSheet &style_sheet);
    ~StyleSheet();

    Color       backgroundColor() const;
    std::string backgroundImage() const;

    Color       fontColor() const;
    std::string fontFamily() const;
    bool        fontItalic() const;
    int         fontSize() const;
    int         fontWeight() const;

    StyleSheet &operator=(const StyleSheet &other);

    std::string toString() const;

private:
    void *m_priv;

}; // End of class StyleSheet

} // End of namespace gfx
} // End of namespace base
