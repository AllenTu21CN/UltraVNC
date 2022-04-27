#include "base/log.h"
#include "base/thread.h"

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <mutex>

#if defined(OS_ANDROID)
    #include <android/log.h>
#endif

#if defined(OS_POSIX)
    #include <sys/syscall.h>
    #include <sys/time.h>
    #include <unistd.h>
#endif

#if defined(OS_WIN)
    #include <Windows.h>
#endif

#if defined(OS_ANDROID)
    #define LOG_DEFAULT_MODULE_NAME "avalon"
#endif

namespace base {

static const char *log_level_label[] = { "FATAL", "ERROR", "WARNING", "INFO", "DEBUG", "VERBOSE" };
static const wchar_t *log_level_label_w[] = { L"FATAL", L"ERROR", L"WARNING", L"INFO", L"DEBUG", L"VERBOSE" };

#if defined(OS_ANDROID)
    static int android_log_level_label[] = {
            ANDROID_LOG_FATAL,
            ANDROID_LOG_ERROR,
            ANDROID_LOG_WARN,
            ANDROID_LOG_INFO,
            ANDROID_LOG_DEBUG,
            ANDROID_LOG_VERBOSE,
    };
#elif defined(OS_POSIX)
    #define ANSI_COLOR_RED     "\x1b[31m"
    #define ANSI_COLOR_GREEN   "\x1b[32m"
    #define ANSI_COLOR_YELLOW  "\x1b[33m"
    #define ANSI_COLOR_BLUE    "\x1b[34m"
    #define ANSI_COLOR_MAGENTA "\x1b[35m"
    #define ANSI_COLOR_CYAN    "\x1b[36m"
    #define ANSI_COLOR_RESET   "\x1b[0m"

    static const char *log_level_color[] = {
        ANSI_COLOR_RED,       // fatal
        ANSI_COLOR_MAGENTA,   // error
        ANSI_COLOR_YELLOW,    // warning
        ANSI_COLOR_RESET,     // info
        ANSI_COLOR_CYAN,      // debug
        ANSI_COLOR_CYAN,      // verbose
    };
#elif defined(OS_WIN)
    static const int log_level_color[] = {
        FOREGROUND_RED,                     // fatal
        FOREGROUND_RED | FOREGROUND_BLUE,   // error
        FOREGROUND_RED | FOREGROUND_GREEN,  // warning
        FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE, // info
        FOREGROUND_GREEN | FOREGROUND_BLUE, // debug
        FOREGROUND_GREEN | FOREGROUND_BLUE  // verbose
    };
#endif

int _log_level = LOG_LEVEL_INFO;
static FILE      *log_output_fp = NULL;
static std::mutex log_mutex;

/**
 * Get now time in format yy-mm-dd HH:MM:SS.msec
 * @param buf
 * @param buf_len
 * @return
 */
inline static size_t getTimeString(char *buf, size_t buf_len)
{
    // TODO: Refactor using std::chrono

    struct timeval tv;
    time_t cur_time;

#if defined(OS_POSIX)
    gettimeofday(&tv, NULL);
    cur_time = tv.tv_sec;
#elif defined(OS_WIN)
    struct tm tm;
    SYSTEMTIME wtm;

    GetLocalTime(&wtm);
    tm.tm_year = wtm.wYear - 1900;
    tm.tm_mon = wtm.wMonth - 1;
    tm.tm_mday = wtm.wDay;
    tm.tm_hour = wtm.wHour;
    tm.tm_min = wtm.wMinute;
    tm.tm_sec = wtm.wSecond;
    tm.tm_isdst = -1;

    cur_time = mktime(&tm);
    tv.tv_sec = static_cast<long>(cur_time);
    tv.tv_usec = wtm.wMilliseconds * 1000;
#endif

    size_t offset = strftime(buf, buf_len, "%Y-%m-%d %T", localtime(&cur_time));
    offset += snprintf(buf + offset, buf_len - offset, ".%06ld", tv.tv_usec);

    return offset;
}

#if defined(OS_ANDROID)
    #define FORMAT_FILENAME(src_name) src_name
#elif defined(OS_POSIX)
    #define FORMAT_FILENAME(src_name) (strrchr(src_name, '/') ? strrchr(src_name, '/') + 1 : src_name)
#elif defined(OS_WIN)
    #define FORMAT_FILENAME(src_name) (strrchr(src_name, '\\') ? strrchr(src_name, '\\') + 1 : src_name)
#endif

inline const char *formatFilename(char const *src_name) {
#if defined(OS_ANDROID)
    const char * out = strrchr(src_name, '/');
#elif defined(OS_POSIX)
    const char * out = strrchr(src_name, '/');
#elif defined(OS_WIN)
    const char * out = strrchr(src_name, '\\');
#endif

    if (out == NULL)
        return src_name;
    else
        return out + 1;
}

void _log_message(char const *src_name, int line, int level, char const *fmt, ...)
{
    // Check current log level
    if (level > _log_level) return;

    if (level <= LOG_LEVEL_DEBUG) {
        src_name = formatFilename(src_name);
    }

    char buffer[LOG_BUFFER_STR_MAX_LEN];

#if defined(OS_ANDROID)
    int prefix_len = snprintf(buffer, LOG_BUFFER_STR_MAX_LEN, "[%s:%d] ", src_name, line);
#else
    char time_buf[LOG_TIME_STR_MAX_LEN];
    getTimeString(time_buf, sizeof(time_buf));

    int prefix_len = snprintf(buffer, LOG_BUFFER_STR_MAX_LEN, "[%s][#%llu][%s][%s:%d] ",
                              time_buf, Thread::getNativeThreadId(),
                              log_level_label[level],
                              src_name, line);
#endif
    char *n_buf = buffer + prefix_len;

    va_list va;
    va_start(va, fmt);
    vsnprintf(n_buf, LOG_BUFFER_STR_MAX_LEN - prefix_len, fmt, va);
    va_end(va);

    std::lock_guard<std::mutex> lock(log_mutex);
    if (log_output_fp) {
        fprintf(log_output_fp, "%s\n", buffer);
    } else {
#if defined(OS_ANDROID)
        __android_log_print(android_log_level_label[level], LOG_DEFAULT_MODULE_NAME,
                            "%s\n", buffer);
        //fprintf(stderr, "%s\n", buffer);
#elif defined(OS_POSIX)
        fprintf(stderr, "%s%s\n\x1b[0m", log_output_fp ? "" : log_level_color[level], buffer);
#elif defined(OS_WIN)
        HANDLE h = GetStdHandle(STD_ERROR_HANDLE);
        WORD wOldColorAttrs;
        CONSOLE_SCREEN_BUFFER_INFO csbiInfo;

        // Save the current color
        GetConsoleScreenBufferInfo(h, &csbiInfo);
        wOldColorAttrs = csbiInfo.wAttributes;

        // Set the new color
        SetConsoleTextAttribute(h, log_level_color[level] | FOREGROUND_INTENSITY);
        fprintf(stderr, "%s\n", buffer);

        // Restore the original color
        SetConsoleTextAttribute(h, wOldColorAttrs);
#endif
    }

    if (level == LOG_LEVEL_FATAL) {
        if (log_output_fp) {
            fflush(log_output_fp);
        }
    }
}

void _log_flush()
{
    std::lock_guard<std::mutex> lock(log_mutex);
    if (log_output_fp) {
        fflush(log_output_fp);
    }
}

int _set_log_level(const char *level)
{
    for (int i = 0; i < sizeof(log_level_label) / sizeof(log_level_label[0]); ++i) {
#if defined(OS_WIN)
        if (0 == _stricmp(level, log_level_label[i])) {
#else
        if (0 == strcasecmp(level, log_level_label[i])) {
#endif
            _log_level = i;
            return 0;
        }
    }

    _warning("Invalid log level: %s, using default level: %s", level, log_level_label[_log_level]);
    return -1;
}

bool _enable_log(int level)
{
    return level <= _log_level;
}

int _set_log_output(const char *destination)
{
    if (log_output_fp) {
        _error("Log had been output to file, can't be called again");
        return -1;
    }

    if (0 == strncmp("file://", destination, sizeof("file://") - 1)) {
        const char *file_name = destination + sizeof("file://") - 1;
        log_output_fp = fopen(file_name, "a+");
        if (!log_output_fp) {
            _error("Open log output file '%s' failed! - err: %s(%d)",
                   file_name, strerror(errno), errno);
            return -2;
        }
    } else if (0 == strncmp("udp://", destination, sizeof("udp://"))) {
        // TODO:
        return -3;
    }

    return 0;
}

#if defined(OS_WIN)
int _get_log_prefix(int level, wchar_t out[], int size)
{
    if (level > _log_level) return 0;

    static wchar_t timebuf[LOG_TIME_STR_MAX_LEN];
    struct timeval tv;
    time_t curtime;

#if defined(WINDOWS)
    struct tm tm;
    SYSTEMTIME wtm;

    GetLocalTime(&wtm);
    tm.tm_year = wtm.wYear - 1900;
    tm.tm_mon = wtm.wMonth - 1;
    tm.tm_mday = wtm.wDay;
    tm.tm_hour = wtm.wHour;
    tm.tm_min = wtm.wMinute;
    tm.tm_sec = wtm.wSecond;
    tm.tm_isdst = -1;

    curtime = mktime(&tm);
    tv.tv_sec = static_cast<long>(curtime);
    tv.tv_usec = wtm.wMilliseconds * 1000;

    DWORD tid = GetCurrentThreadId();
#elif defined(LINUX)
    gettimeofday(&tv, NULL);
    curtime = tv.tv_sec;
    pid_t tid = GET_LINUX_THREAD_ID();
#endif
    wcsftime(timebuf, LOG_TIME_STR_MAX_LEN, L"%Y-%m-%d %T", localtime(&curtime));
    swprintf_s(out, size-1, L"[%s.%06ld][#%d][%s]",
             timebuf, tv.tv_usec,
             tid, log_level_label_w[level]);
   return 1;
}
static HANDLE _STD_ERROR_HANDLE_ = NULL;
uint16_t _change_color(int level)
{
    if(_STD_ERROR_HANDLE_ == NULL)
        _STD_ERROR_HANDLE_ = GetStdHandle(STD_ERROR_HANDLE);
    WORD wOldColorAttrs;
    CONSOLE_SCREEN_BUFFER_INFO csbiInfo;

    // Save the current color
    GetConsoleScreenBufferInfo(_STD_ERROR_HANDLE_, &csbiInfo);
    wOldColorAttrs = csbiInfo.wAttributes;

    // Set the new color
    SetConsoleTextAttribute(_STD_ERROR_HANDLE_, log_level_color[level] | FOREGROUND_INTENSITY);
    return (uint16_t)wOldColorAttrs;
}
void _recover_color(uint16_t old)
{
    // Restore the original color
    SetConsoleTextAttribute(_STD_ERROR_HANDLE_, (WORD)old);
}
#endif

} // End of namespace base
