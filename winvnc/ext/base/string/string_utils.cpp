#include "base/string/string_utils.h"

#include <stdlib.h>
#include <string.h>
#include <cctype>
#include <algorithm>
#include <sstream>
#include <stdarg.h>
#include <assert.h>

#if defined(WINDOWS)
#include <Windows.h>
#include <stdint.h>
#endif

namespace base {

namespace string {

std::vector<std::string> Utils::split(const std::string &s, char delim)
{
    std::stringstream ss(s);
    std::string item;
    std::vector<std::string> elems;

    while (std::getline(ss, item, delim)) {
        elems.push_back(std::move(item));
    }

    return elems;
}

} // End of namespace string

std::string to_lower(const std::string &s)
{
    std::string t = s;
    std::transform(s.begin(), s.end(), t.begin(),
        [](unsigned char c) { return std::tolower(c); });
    return t;
}

std::string to_upper(const std::string &s)
{
    std::string t = s;
    std::transform(s.begin(), s.end(), t.begin(),
        [](unsigned char c) { return std::toupper(c); });
    return t;
}


#if defined(OS_ANDROID)
    #define TO_STRINGs(in, size, format) { \
        char tmp[size] = {0}; \
        sprintf(tmp, format, in); \
        return tmp; \
    }
#else
    #define TO_STRINGs(in, size, format) {return std::to_string(in);}
#endif

std::string to_string(int8_t in)
{
    TO_STRINGs(in, 5, "%d");
}

std::string to_string(uint8_t in)
{
    TO_STRINGs(in, 5, "%u");
}

std::string to_string(int32_t in)
{
    TO_STRINGs(in, 15, "%d");
}

std::string to_string(uint32_t in)
{
    TO_STRINGs(in, 15, "%u");
}

std::string to_string(int64_t in)
{
    TO_STRINGs(in, 25, "%lld");
}

std::string to_string(uint64_t in)
{
    TO_STRINGs(in, 25, "%llu");
}

int stoi(const std::string& str, size_t* pos, int base)
{
#if defined(OS_ANDROID)
    return (int)::base::stol(str, pos, base);
#else
    return std::stoi(str, pos, base);
#endif
}

long stol(const std::string& str, size_t* pos, int base)
{
#if defined(OS_ANDROID)
    const char *ptr = str.c_str();
    char *eptr;
    long ans = ::strtol(ptr, &eptr, base);

    assert(ptr != eptr);
    if (pos != 0)
        *pos = (size_t)(eptr - ptr);
    return (ans);
#else
    return std::stol(str, pos, base);
#endif
}

long long stoll(const std::string& str, size_t* pos, int base)
{
#if defined(OS_ANDROID)
    const char *ptr = str.c_str();
    char *eptr;
    long long ans = ::strtoll(ptr, &eptr, base);

    assert(ptr != eptr);
    if (pos != 0)
        *pos = (size_t)(eptr - ptr);
    return (ans);
#else
    return std::stoll(str, pos, base);
#endif
}

float stof(const std::string& str, size_t* pos)
{
#if defined(OS_ANDROID)
    const char *ptr = str.c_str();
    char *eptr;
    float ans = ::strtof(ptr, &eptr);

    assert(ptr != eptr);
    if (pos != 0)
        *pos = (size_t)(eptr - ptr);
    return (ans);
#else
    return std::stof(str, pos);
#endif
}

bool starts_with(const std::string &s, const std::string &t, bool case_sensitive)
{
    if (!case_sensitive) {
        std::string _s = to_lower(s);
        std::string _t = to_lower(t);
        return (_s.compare(0, _t.length(), _t) == 0);
    }

    return (s.compare(0, t.length(), t) == 0);
}

void string_split(const char *separator, const std::string target, std::vector<std::string> &lines, int max_cnt)
{
    lines.clear();
    size_t pos = 0;
    size_t index = target.find(separator, pos);
    int cnt = 1;
    while(index != -1) {
        lines.push_back(target.substr(pos, index-pos));
        pos = index + strlen(separator);
        if(max_cnt > 0 && cnt >= max_cnt)
            break;
        index = target.find(separator, pos);
        ++cnt;
    }
    if(pos < target.size())
        lines.push_back(target.substr(pos));
}

void string_trim(std::string &target, const char *chars)
{
    string_ltrim(target, chars);
    string_rtrim(target, chars);
}

void string_ltrim(std::string &target, const char *chars)
{
    if (target.empty())
        return;
    target.erase(0, target.find_first_not_of(chars));
}

void string_rtrim(std::string &target, const char *chars)
{
    if (target.empty())
        return;
    target.erase(target.find_last_not_of(chars) + 1);
}

std::string &string_replace(std::string &target, const std::string &old_fragment, const std::string &new_fragment)
{
    std::string::size_type pos = 0;
    std::string::size_type old_fgt_len = old_fragment.size();
    std::string::size_type new_fgt_len = new_fragment.size();
    while((pos = target.find(old_fragment, pos)) != std::string::npos ) {
        target.replace(pos, old_fgt_len, new_fragment);
        pos += new_fgt_len;
    }
    return target;
}

void string_format(std::string &target, char *tmp_buff, const uint32_t buf_size, bool append, const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    vsnprintf(tmp_buff, buf_size, fmt, args);
    va_end(args);
    if(append)
        target += tmp_buff;
    else
        target = tmp_buff;
}

void string_from_data(std::string &target, const uint8_t *data, uint32_t len)
{
    if(len >= 512) {
        char *buf = new char[len+1];
        memcpy(buf, data, len);
        buf[len] = 0;
        target = buf;
        delete []buf;
    } else {
        char buf[512] = {0};
        memcpy(buf, data, len);
        buf[len] = 0;
        target = buf;
    }
}

bool string_startwith(const std::string &str, const std::string &sub)
{
    return strncmp(str.c_str(), sub.c_str(), sub.length()) == 0;
}

bool string_startwith(const std::string &str, const char *sub)
{
    return strncmp(str.c_str(), sub, strlen(sub)) == 0;
}

bool string_startwith(const char *str, const char *sub)
{
    return strncmp(str, sub, strlen(sub)) == 0;
}

#if defined(WINDOWS)
std::string sys_wide_to_utf8(const std::wstring &wide)
{
    return sys_wide_to_multi_byte(wide, CP_UTF8);
}

std::wstring sys_utf8_to_wide(const std::string &utf8)
{
    return sys_multi_byte_to_wide(utf8, CP_UTF8);
}

std::string sys_wide_to_native_mb(const std::wstring &wide)
{
    return sys_wide_to_multi_byte(wide, CP_ACP);
}

std::wstring sys_native_mb_to_wide(const std::string &native_mb)
{
    return sys_multi_byte_to_wide(native_mb, CP_ACP);
}

std::wstring sys_multi_byte_to_wide(const std::string &mb, uint32_t code_page)
{
    if (mb.empty())
        return std::wstring();

    int mb_length = static_cast<int>(mb.length());
    // Compute the length of the buffer.
    int charcount = MultiByteToWideChar(code_page, 0, mb.data(), mb_length, NULL, 0);
    if (charcount == 0)
        return std::wstring();

    std::wstring wide;
    wide.resize(charcount);
    MultiByteToWideChar(code_page, 0, mb.data(), mb_length, &wide[0], charcount);

    return wide;
}

std::string sys_wide_to_multi_byte(const std::wstring &wide, uint32_t code_page)
{
    int wide_length = static_cast<int>(wide.length());
    if (wide_length == 0)
        return std::string();

    // Compute the length of the buffer we'll need.
    int charcount = WideCharToMultiByte(code_page, 0, wide.data(), wide_length, NULL, 0, NULL, NULL);
    if (charcount == 0)
        return std::string();

    std::string mb;
    mb.resize(charcount);
    WideCharToMultiByte(code_page, 0, wide.data(), wide_length, &mb[0], charcount, NULL, NULL);

    return mb;
}
#endif // WINDOWS
}
