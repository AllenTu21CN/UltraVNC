#pragma once

#include <string.h>
#include <stdint.h>

namespace base {

/**
* Logger macros
*/
#define LOG_LEVEL_FATAL         0
#define LOG_LEVEL_ERROR         1
#define LOG_LEVEL_WARNING       2
#define LOG_LEVEL_INFO          3
#define LOG_LEVEL_DEBUG         4
#define LOG_LEVEL_VERBOSE       5

#define LOG_BUFFER_STR_MAX_LEN  3072
#define LOG_TIME_STR_MAX_LEN    32
#define LOG_PREFIX_STR_MAX_LEN  128

/**
 * Log level
 */
extern int _log_level;

/**
 * Log functions
 */

#if defined(OS_POSIX)
void _log_message(char const *src_name, int line, int level, char const *fmt, ...) __attribute__((format(printf, 4, 5)));
#else
void _log_message(char const *src_name, int line, int level, char const *fmt, ...);
#endif

#define _fatal(fmt, ...) \
    _log_message(__FILE__, __LINE__, LOG_LEVEL_FATAL, fmt, ##__VA_ARGS__)

#define _error(fmt, ...) \
    _log_message(__FILE__, __LINE__, LOG_LEVEL_ERROR, fmt, ##__VA_ARGS__)

#define _warning(fmt, ...) \
    _log_message(__FILE__, __LINE__, LOG_LEVEL_WARNING, fmt, ##__VA_ARGS__)

#define _info(fmt, ...) \
    _log_message(__FILE__, __LINE__, LOG_LEVEL_INFO, fmt, ##__VA_ARGS__)

#define _debug(fmt, ...) \
    _log_message(__FILE__, __LINE__, LOG_LEVEL_DEBUG, fmt, ##__VA_ARGS__)

#define _verbose(fmt, ...) \
    _log_message(__FILE__, __LINE__, LOG_LEVEL_VERBOSE, fmt, ##__VA_ARGS__)

#define _fatal2(fmt, ...) \
    if (LOG_LEVEL_FATAL <= base::_log_level) { \
        base::_log_message(__FILE__, __LINE__, LOG_LEVEL_INFO, fmt, ##__VA_ARGS__); \
    }

#define _error2(fmt, ...) \
    if (LOG_LEVEL_ERROR <= base::_log_level) { \
        base::_log_message(__FILE__, __LINE__, LOG_LEVEL_INFO, fmt, ##__VA_ARGS__); \
    }

#define _warning2(fmt, ...) \
    if (LOG_LEVEL_WARNING <= base::_log_level) { \
        base::_log_message(__FILE__, __LINE__, LOG_LEVEL_INFO, fmt, ##__VA_ARGS__); \
    }

#define _info2(fmt, ...) \
    if (LOG_LEVEL_INFO <= base::_log_level) { \
        base::_log_message(__FILE__, __LINE__, LOG_LEVEL_INFO, fmt, ##__VA_ARGS__); \
    }

#define _debug2(fmt, ...) \
    if (LOG_LEVEL_DEBUG <= base::_log_level) { \
        base::_log_message(__FILE__, __LINE__, LOG_LEVEL_INFO, fmt, ##__VA_ARGS__); \
    }

#define _verbose2(fmt, ...) \
    if (LOG_LEVEL_VERBOSE <= base::_log_level) { \
        base::_log_message(__FILE__, __LINE__, LOG_LEVEL_INFO, fmt, ##__VA_ARGS__); \
    }

#define _enable_debug() _enable_log(LOG_LEVEL_DEBUG)

bool _enable_log(int level);

/**
 * Wide char version for OS_WIN only
 * TODO: Refactor...
 */
#if defined(OS_WIN)
    #define INIT_LOG_WCHAR() \
        namespace base { \
            extern int _get_log_prefix(int level, wchar_t out[], int size); \
            extern uint16_t _change_color(int level); \
            extern void _recover_color(uint16_t old); \
        }

    //wprintf(L"%s[" _FILENAME_ ":%d] " fmt "\n", _log_prefix_, __LINE__, ##__VA_ARGS__);

    int _get_log_prefix(int level, wchar_t out[], int size);
    uint16_t _change_color(int level);
    void _recover_color(uint16_t old);
    #define _log_w(level, fmt, ...) {\
        wchar_t _log_prefix_[LOG_PREFIX_STR_MAX_LEN]; \
        if(base::_get_log_prefix(level, _log_prefix_, LOG_PREFIX_STR_MAX_LEN) > 0){ \
            uint16_t old = base::_change_color(level); \
            wprintf(L"%s[%S:%d] " fmt "\n", _log_prefix_, _FILENAME_, __LINE__, ##__VA_ARGS__); \
            base::_recover_color(old); \
        } \
    }
    #define _verbose_w(fmt, ...) \
        _log_w(LOG_LEVEL_VERBOSE, fmt, ##__VA_ARGS__)
    #define _debug_w(fmt, ...) \
        _log_w(LOG_LEVEL_DEBUG, fmt, ##__VA_ARGS__)
    #define _fatal_w(fmt, ...) \
        _log_w(LOG_LEVEL_FATAL, fmt, ##__VA_ARGS__)
    #define _error_w(fmt, ...) \
        _log_w(LOG_LEVEL_ERROR, fmt, ##__VA_ARGS__)
    #define _warning_w(fmt, ...) \
        _log_w(LOG_LEVEL_WARNING, fmt, ##__VA_ARGS__)
    #define _info_w(fmt, ...) \
        _log_w(LOG_LEVEL_INFO, fmt, ##__VA_ARGS__)
#endif

void _log_flush();

int _set_log_level(const char *level);

int _set_log_output(const char *destination);

int _set_log_module_name(const char *name);

} // End of base
