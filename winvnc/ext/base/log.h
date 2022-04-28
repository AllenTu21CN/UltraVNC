#pragma once

#include "base/event_loop.h"
#include "base/io/timer.h"

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <time.h>

#include <string>

#if defined(OS_POSIX)
#include <netinet/in.h>
#include <syslog.h>
#elif defined(OS_WIN)
#include <winsock.h>
#endif

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

#define LOG_BUFFER_STR_MAX_LEN  2048
#define LOG_PREFIX_STR_MAX_LEN  128

/**
 * Log level
 */
extern int _log_level;
void _log_message(char const *src_name, int line, int level, char const *fmt, ...);

/**
 * Log functions
 */
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

#if defined (OS_WIN)
    void _log_message(char const *src_name, int line, int level, char const *fmt, ...);
#elif defined(OS_POSIX)
    void _log_message(char const *src_name, int line, int level, char const *fmt, ...) __attribute__((format(printf, 4, 5)));
#endif

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

/**
 * Set logging level
 * - level: { FATAL, ERROR, WARNING, INFO, DEBUG, VERBOSE }, case insensitive
 */
int _set_log_level(const char *level);

/**
 * Set log output target
 * - destination
 *    - file://<abs_or_rel_path>                                        : BasicFileSink
 *    - udp://<hostname>:<port>                                         : UdpSink
 *    - rot://<abs_or_rel_path>:<max_size>:<max_duration>:<max_files>   : RotatingFileSink
 *    - sys://<ident>:<option>:<facility>                               : SysLogSink
 *    - NULL or empty                                                   : DefaultSink
 */
int _set_log_output(const char *destination);

int _set_log_module_name(const char *name);

namespace log {

/**
 * Log message struct.
 */
typedef struct {
    struct timeval  time;
    time_t          cur_time;
    int             level;
    uint64_t        thread_id;
    char const *    src_name;
    int             line;
    char const *    fmt;
    va_list         va;
    char            formatted_message[LOG_BUFFER_STR_MAX_LEN];
    size_t          formatted_length;
} Message;

/**
 * Abstract logging formatter.
 */
class Formatter
{
public:
    virtual ~Formatter();

    virtual int format(Message &msg) = 0;

}; // End of class Formatter

class DefaultFormatter : public Formatter
{
public:
    DefaultFormatter();
    ~DefaultFormatter();

    int format(Message &msg) override;

}; // End of class DefaultFormatter

/**
 * Abstract logging sink.
 */
class Sink
{
public:
    virtual ~Sink();

protected:
    virtual void log(const Message &msg) = 0;
    virtual void flush();

    friend void base::_log_message(char const *src_name, int line, int level, char const *fmt, ...);
    friend void base::_log_flush();

}; // End of class Sink

/**
 * Default logging sink.
 * - Android: alog
 * - Posix, Windows: stderr
 */
class DefaultSink :  public Sink
{
public:
    DefaultSink(bool enable_color = true);
    ~DefaultSink();

protected:
    void log(const Message &msg) override;
    void flush() override;

private:
    bool m_enable_color;

}; // End of class DefaultSink

class BasicFileSink : public Sink
{
public:
    BasicFileSink(const char *file_name);
    ~BasicFileSink();

protected:
    void log(const Message &msg) override;
    void flush() override;

private:
    FILE *m_fp;

}; // End of class BasicFileSink

class RotatingFileSink : public Sink
{
public:
    static const char *ROTATION_FILE_NAME_SUFFIX;
    static const int RECYCLE_CHECK_INTERVAL = 60; // 1min
    /**
     * Log rotation policy
     * - max_size : maximum file size in bytes, -1 means unlimited
     * - max_duration : maximum logging duration in seconds, -1 means unlimited
     * - max_files : keep maximum files, out of date files will be deleted, -1 means unlimited
     */
    RotatingFileSink(const char *file_name, int64_t max_size, int max_duration, int max_files = -1);
    ~RotatingFileSink();

protected:
    void log(const Message &msg) override;
    void flush() override;

private:
    class RecycleLoop : public base::EventLoop
    {
    public:
        RecycleLoop(const char *file_name, int max_files, int interval);
        ~RecycleLoop();

    protected:
        void recycle(int);
        void onEvent(const Event &event) override;

    private:
        std::string m_dir_path;
        std::string m_file_name;
        int m_max_files;
        int m_interval;

        io::Timer m_timer;
        std::function<void(int)> m_timer_callback;

    }; // End of class RecycleLoop
    RecycleLoop *m_recycle_loop;

    std::string m_file_name;
    int64_t     m_max_size = -1;
    int         m_max_duration = -1;
    int         m_max_files = -1;

    FILE *      m_fp;
    uint64_t    m_cur_size;
    int64_t     m_cur_begin_ts;

}; // End of class RotatingFileSink

class UdpSink : public Sink
{
public:
    UdpSink(const char *hostname, uint16_t port);
    ~UdpSink();

protected:
    void log(const Message &msg) override;

private:
    int                 m_socket;
    struct sockaddr_in  m_serv_addr;

}; // End of class UdpSink

#if defined(OS_POSIX)
class SysLogSink : public Sink
{
public:
    SysLogSink(const char *ident, int option, int facility);
    ~SysLogSink();

protected:
    void log(const Message &msg) override;

}; // End of class SysLogSink
#endif
} // End of namespace log

/**
 * Set log formatter.
 */
int _set_log_formatter(log::Formatter *formatter);

/**
 * Set log sink.
 */
int _set_log_sink(log::Sink *sink);

} // End of namespace base
