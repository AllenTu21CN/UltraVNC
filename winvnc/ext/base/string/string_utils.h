#pragma once

#include <string>
#include <string.h>
#include <vector>
#include <stdint.h>

namespace base {

namespace string {

class Utils
{
public:
    static std::vector<std::string> split(const std::string &s, char delim);


}; // End of class Utils

} // End of namespace string

// Returns a lowercase copy of the string.
std::string to_lower(const std::string &s);

// Returns a uppercase copy of the string.
std::string to_upper(const std::string &s);

std::string to_string(int8_t in);
std::string to_string(uint8_t in);
std::string to_string(int32_t in);
std::string to_string(uint32_t in);
std::string to_string(int64_t in);
std::string to_string(uint64_t in);

int stoi(const std::string& str, size_t* pos = 0, int base = 10);
long stol(const std::string& str, size_t* pos = 0, int base = 10);
long long stoll(const std::string& str, size_t* pos = 0, int base = 10);
float stof(const std::string& str, size_t* pos = 0);

// Returns true if the string s starts with t; otherwise returns false.
// If case_sensitive is true (default), the search is case sensitive; otherwise the search is case insensitive.
bool starts_with(const std::string &s, const std::string &t, bool case_sensitive = true);

void string_split(const char *separator, const std::string target, std::vector<std::string> &lines, int max_cnt=-1);
void string_trim(std::string &target, const char *chars = " ");
void string_ltrim(std::string &target, const char *chars = " ");
void string_rtrim(std::string &target, const char *chars = " ");
std::string &string_replace(std::string &target, const std::string &old_fragment, const std::string &new_fragment);

void string_format(std::string &target, char *tmp_buff, const uint32_t buf_size, bool append, const char *fmt, ...);
#define string_format512(target, fmt, ...) { \
    char tmp[512 + 1] = { 0 }; \
    ::base::string_format(target, tmp, 512, false, fmt, ##__VA_ARGS__); \
}
#define string_format2K(target, fmt, ...) { \
    char tmp[2048 + 1] = { 0 }; \
    ::base::string_format(target, tmp, 2048, false, fmt, ##__VA_ARGS__); \
}
#define string_format2M(target, fmt, ...) { \
    char tmp[2048000 + 1] = { 0 }; \
    ::base::string_format(target, tmp, 2048000, false, fmt, ##__VA_ARGS__); \
}
#define string_append_format512(target, fmt, ...) { \
    char tmp[512 + 1] = { 0 }; \
    ::base::string_format(target, tmp, 512, true, fmt, ##__VA_ARGS__); \
}
#define string_append_format2K(target, fmt, ...) { \
    char tmp[2048 + 1] = { 0 }; \
    ::base::string_format(target, tmp, 2048, true, fmt, ##__VA_ARGS__); \
}
#define string_append_format2M(target, fmt, ...) { \
    char tmp[2048000 + 1] = { 0 }; \
    ::base::string_format(target, tmp, 2048000, true, fmt, ##__VA_ARGS__); \
}

void string_from_data(std::string &target, const uint8_t *data, uint32_t len);

bool string_startwith(const std::string &str, const std::string &sub);
bool string_startwith(const std::string &str, const char *sub);
bool string_startwith(const char *str, const char *sub);

// Windows-specific ------------------------------------------------------------
#if defined(WINDOWS)

// Converts between wide and UTF-8 representations of a string. On error, the
// result is system-dependent.
std::string sys_wide_to_utf8(const std::wstring &wide);
std::wstring sys_utf8_to_wide(const std::string &utf8);

// Converts between wide and the system multi-byte representations of a string.
// DANGER: This will lose information and can change (on Windows, this can
// change between reboots).
std::string sys_wide_to_native_mb(const std::wstring &wide);
std::wstring sys_native_mb_to_wide(const std::string &native_mb);

// Converts between 8-bit and wide strings, using the given code page. The
// code page identifier is one accepted by the Windows function
// MultiByteToWideChar().
std::wstring sys_multi_byte_to_wide(const std::string &mb, uint32_t code_page);
std::string sys_wide_to_multi_byte(const std::wstring &wide, uint32_t code_page);

#endif  // defined(WINDOWS)

}
