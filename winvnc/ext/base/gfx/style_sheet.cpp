#include "base/gfx/style_sheet.h"
#include "base/log.h"

#include <regex>

#include <assert.h>

namespace base {
namespace gfx {

#define INIT_PRIV(d) \
    StyleSheetPrivate *d = NULL; \
    if (m_priv) { \
        d = static_cast<StyleSheetPrivate *>(m_priv); \
    } \
    assert(d); \


class StyleSheetPrivate
{
public:
    StyleSheetPrivate();
    StyleSheetPrivate(const std::string &text);
    StyleSheetPrivate(const StyleSheetPrivate &style_sheet_priv);
    ~StyleSheetPrivate();

    Color       backgroundColor() const { return m_background_color; }
    std::string backgroundImage() const { return m_background_image; }
    Color       fontColor() const { return m_font_color; }
    bool        fontItalic() const { return m_font_italic; }
    std::string fontFamily() const { return m_font_family; }
    int         fontSize() const { return m_font_size; }
    int         fontWeight() const { return m_font_weight; }

    StyleSheetPrivate &operator=(const StyleSheetPrivate &other);

protected:
    bool        parse(const std::string &text);

private:
    Color       m_background_color;
    std::string m_background_image;
    Color       m_font_color;
    bool        m_font_italic;
    std::string m_font_family;
    int         m_font_size;
    int         m_font_weight;
};

StyleSheetPrivate::StyleSheetPrivate()
{
    m_background_color = Color(0, 0, 0, 0);
    m_background_image = "";
    m_font_color       = Color(255, 255, 255, 255);
    m_font_italic      = false;
    m_font_family      = "";
    m_font_size        = 12;
    m_font_weight      = -1;
}

StyleSheetPrivate::StyleSheetPrivate(const std::string &text)
{
    m_background_color = Color(0, 0, 0, 0);
    m_background_image = "";
    m_font_color       = Color(255, 255, 255, 255);
    m_font_italic      = false;
    m_font_family      = "";
    m_font_size        = 12;
    m_font_weight      = -1;

    parse(text);
}

StyleSheetPrivate::StyleSheetPrivate(const StyleSheetPrivate &style_sheet_priv)
{
    m_background_color = style_sheet_priv.m_background_color;
    m_background_image = style_sheet_priv.m_background_image;
    m_font_color       = style_sheet_priv.m_font_color;
    m_font_italic      = style_sheet_priv.m_font_italic;
    m_font_family      = style_sheet_priv.m_font_family;
    m_font_size        = style_sheet_priv.m_font_size;
    m_font_weight      = style_sheet_priv.m_font_weight;
}

StyleSheetPrivate::~StyleSheetPrivate()
{
}

StyleSheetPrivate &StyleSheetPrivate::operator=(const StyleSheetPrivate &other)
{
    m_background_color = other.m_background_color;
    m_background_image = other.m_background_image;
    m_font_color       = other.m_font_color;
    m_font_italic      = other.m_font_italic;
    m_font_family      = other.m_font_family;
    m_font_size        = other.m_font_size;
    m_font_weight      = other.m_font_weight;

    return *this;
}

bool StyleSheetPrivate::parse(const std::string &text)
{
    const std::string prop_exps[] = {
        R"((background-color)\s*:\s*(#[0-9a-fA-F]{6,8})\s*;)",
        R"((background-image)\s*:\s*\"([^\"]+)\"\s*;)",
        R"((font-color)\s*:\s*(#[0-9a-fA-F]{6,8})\s*;)",
        R"((font-italic)\s*:\s*(true|false)\s*;)",
        R"((font-family)\s*:\s*\"([^\"]+)\"\s*;)",
        R"((font-size)\s*:\s*([0-9]+)px\s*;)",
        R"((font-weight)\s*:\s*([0-9]+)\s*;)",
    };

    for (const auto &exp : prop_exps) {
        //base::_info("Porperty express: %s", exp.c_str());
        std::regex rx(exp);
        std::smatch matches;
        if (std::regex_search(text, matches, rx)) {
            //base::_info("Regex matched!!!");
            //for (const auto &s : matches) {
            //    base::_info("Matches: %s", s.str().c_str());
            //}

            if (matches.size() > 2) {
                std::string name = matches[1].str();
                std::string value = matches[2].str();
                if ("background-color" == name) {
                    m_background_color = Color(value);
                } else if ("background-image" == name) {
                    m_background_image = value;
                } else if ("font-color" == name) {
                    m_font_color = Color(value);
                } else if ("font-italic" == name) {
                    m_font_italic = ("true" == value);
                } else if ("font-family" == name) {
                    m_font_family = value;
                } else if ("font-size" == name) {
                    m_font_size = std::stoi(value);
                } else if ("font-weight" == name) {
                    m_font_weight = std::stoi(value);
                }
            }
        }
    }

    return true;
}

StyleSheet::StyleSheet()
    : m_priv(new StyleSheetPrivate)
{
}

StyleSheet::StyleSheet(const std::string &text)
    : m_priv(new StyleSheetPrivate(text))
{
}

StyleSheet::StyleSheet(const StyleSheet &style_sheet)
    : m_priv(new StyleSheetPrivate(*(static_cast<StyleSheetPrivate *>(style_sheet.m_priv))))
{
}

StyleSheet::~StyleSheet()
{
    if (m_priv) {
        StyleSheetPrivate *d = static_cast<StyleSheetPrivate *>(m_priv);
        if (d) {
            delete d;
            m_priv = NULL;
        }
    }
}

Color StyleSheet::backgroundColor() const
{
    INIT_PRIV(d);

    return d->backgroundColor();
}

std::string StyleSheet::backgroundImage() const
{
    INIT_PRIV(d);

    return d->backgroundImage();
}

Color StyleSheet::fontColor() const
{
    INIT_PRIV(d);

    return d->fontColor();
}

bool StyleSheet::fontItalic() const
{
    INIT_PRIV(d);

    return d->fontItalic();
}

std::string StyleSheet::fontFamily() const
{
    INIT_PRIV(d);

    return d->fontFamily();
}

int StyleSheet::fontSize() const
{
    INIT_PRIV(d);

    return d->fontSize();
}

int StyleSheet::fontWeight() const
{
    INIT_PRIV(d);

    return d->fontWeight();
}

StyleSheet &StyleSheet::operator=(const StyleSheet &other)
{
    // Check for self-assignment!
    if (this == &other)      // Same object?
        return *this;        // Yes, so skip assignment, and just return *this.

    INIT_PRIV(d);

    StyleSheetPrivate *d2 = static_cast<StyleSheetPrivate *>(other.m_priv);
    assert(d2);

    (*d) = (*d2);

    return *this;
}

std::string StyleSheet::toString() const
{
    char buf[1024];
    sprintf(buf,
            R"tpl(
                background-color: %s
                background-image: %s
                font-color      : %s
                font-family     : %s
                font-italic     : %s
                font-size       : %d
                font-weight     : %d)tpl",
            backgroundColor().toString().c_str(),
            backgroundImage().c_str(),
            fontColor().toString().c_str(),
            fontFamily().c_str(),
            fontItalic() ? "true" : "false",
            fontSize(),
            fontWeight());

    return std::string(buf);
}

} // End of namespace gfx
} // End of namespace base
