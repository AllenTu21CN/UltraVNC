#include "base/log.h"

#include "base/clock.h"
#include "base/thread.h"
#include "base/dirent_r.h"

#include <stdarg.h>
#include <stdlib.h>

#include <algorithm>
#include <iomanip>
#include <list>
#include <mutex>
#include <regex>
#include <sstream>

#if defined(OS_ANDROID)
    #include <android/log.h>
#endif

#if defined(OS_POSIX)
    #include <sys/syscall.h>
    #include <sys/time.h>
    #include <unistd.h>
    #include <netdb.h>
    #include <sys/socket.h>
    #include <arpa/inet.h>
#endif

#if defined(OS_WIN)
    #include <Windows.h>
#ifdef __cplusplus
    extern "C"
    {
#endif
    #include "base/strptime.h"
#ifdef __cplusplus
    }
#endif
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

static log::Sink *log_sink = nullptr;
static log::Formatter *log_formatter = nullptr;
static std::mutex log_mutex;

#if 0
static std::string orig_output_file_name;
static FILE      *log_output_fp = NULL;
#endif

inline static void getTimeVal(struct timeval *tv, time_t &cur_time)
{
#if defined(OS_POSIX)
    gettimeofday(tv, NULL);
    cur_time = tv->tv_sec;
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
    tv->tv_sec = static_cast<long>(cur_time);
    tv->tv_usec = wtm.wMilliseconds * 1000;
#endif
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

    if (nullptr == log_formatter) {
        log_formatter = new log::DefaultFormatter();
    }

    if (nullptr == log_sink) {
        log_sink = new log::DefaultSink();
    }

    // Construct message object
    log::Message msg;
    getTimeVal(&(msg.time), msg.cur_time);
    msg.level = level;
    msg.thread_id = Thread::getNativeThreadId();
    msg.src_name = src_name;
    msg.line = line;
    msg.fmt = fmt;
    va_start(msg.va, fmt);

    std::lock_guard<std::mutex> lock(log_mutex);

    log_formatter->format(msg);

    log_sink->log(msg);

    va_end(msg.va);
}

void _log_flush()
{
    std::lock_guard<std::mutex> lock(log_mutex);
    if (log_sink) {
        log_sink->flush();
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

#define STR_START_WITH(STR, PREFIX) \
    (0 == strncmp(STR, PREFIX, sizeof(PREFIX) - 1))

int _set_log_output(const char *destination)
{
    if (nullptr == destination || 0 == strlen(destination)) {
        _set_log_sink(new log::DefaultSink());
        return 0;
    }

    // BasicFile
    const char PREFIX_FILE[] = "file://";
    if (STR_START_WITH(destination, PREFIX_FILE)) {
        const char *file_name = destination + sizeof(PREFIX_FILE) - 1;

        _set_log_sink(new log::BasicFileSink(file_name));
        return 0;
    }

    // UdpSink
    const char PREFIX_UDP[] = "udp://";
    if (STR_START_WITH(destination, PREFIX_UDP)) {
        std::string uri = destination + sizeof(PREFIX_UDP) - 1;
        std::smatch matches;
        std::regex rx(R"((.+):([0-9]+))");
        if (std::regex_match(uri, matches, rx)) {
            std::string hostname = matches[1];
            int port = std::stoi(matches[2]);
            //fprintf(stderr, "udp output: %s:%d\n", hostname.c_str(), port);

            _set_log_sink(new log::UdpSink(hostname.c_str(), port));
            return 0;
        }
    }

    // RotatingFileSink
    const char PREFIX_ROT[] = "rot://";
    if (STR_START_WITH(destination, PREFIX_ROT)) {
        std::string uri = destination + sizeof(PREFIX_ROT) - 1;
        std::smatch matches;
        std::regex rx(R"((.+):(-?[0-9]+):(-?[0-9]+):(-?[0-9]+))");
        if (std::regex_match(uri, matches, rx)) {
            std::string file_name = matches[1];
            int max_size = std::stoi(matches[2]);
            int max_duration = std::stoi(matches[3]);
            int max_files = std::stoi(matches[4]);
            //fprintf(stderr, "rotating file_name: %s, max_size: %d, max_duration: %d, max_files: %d\n",
            //        file_name.c_str(), max_size, max_duration, max_files);

            _set_log_sink(new log::RotatingFileSink(file_name.c_str(), max_size, max_duration, max_files));
            return 0;
        }
    }

#if defined(OS_POSIX)
    // SysLogSink
    const char PREFIX_SYS[] = "sys://";
    if (STR_START_WITH(destination, PREFIX_SYS)) {
        std::string uri = destination + sizeof(PREFIX_SYS) - 1;
        std::smatch matches;
        std::regex rx(R"((.+):([0-9]+):([0-9]+))");
        if (std::regex_match(uri, matches, rx)) {
            std::string ident = matches[1];
            int option = std::stoi(matches[2]);
            int facility = std::stoi(matches[3]);
            //fprintf(stderr, "syslog ident: %s, option: %d, facility: %d\n",
            //        ident.c_str(), option, facility);

            _set_log_sink(new log::SysLogSink(ident.c_str(), option, facility));
            return 0;
        }
    }
#endif

    _error("Invalid log output destination path.");
    return -1;
}
int _set_log_formatter(log::Formatter *formatter)
{
    std::lock_guard<std::mutex> lock(log_mutex);
    log_formatter = formatter;

    return 0;
}

int _set_log_sink(log::Sink *sink)
{
    std::lock_guard<std::mutex> lock(log_mutex);
    if (log_sink) {
        delete log_sink;
        log_sink = nullptr;
    }

    log_sink = sink;

    return 0;
}

namespace log {

// Formatter
Formatter::~Formatter() { }

// DefaultFormatter
DefaultFormatter::DefaultFormatter() { }

DefaultFormatter::~DefaultFormatter() { }

int DefaultFormatter::format(Message &msg)
{
    msg.formatted_length = 0;
#if defined(OS_ANDROID)
    msg.formatted_length = snprintf(msg.formatted_message, sizeof(msg.formatted_message), "[%s:%d] ", msg.src_name, msg.line);
#else
    // Generate time string in format yy-mm-dd HH:MM:SS.msec
    const size_t TIME_STR_MAX_LEN = 32;
    char time_buf[TIME_STR_MAX_LEN];
    size_t offset = strftime(time_buf, TIME_STR_MAX_LEN, "%Y-%m-%d %T", localtime(&msg.cur_time));
    snprintf(time_buf + offset, TIME_STR_MAX_LEN - offset, ".%06ld", msg.time.tv_usec);

    // Combine all prefixes: [time][thread_id][level][src_name:line]
    msg.formatted_length = snprintf(msg.formatted_message, sizeof(msg.formatted_message),
                       "[%s][#%llu][%s][%s:%d] ",
                       time_buf, msg.thread_id,
                       log_level_label[msg.level],
                       msg.src_name, msg.line);
#endif

    // Concat user format
    char *cur_pos = msg.formatted_message + msg.formatted_length;
    msg.formatted_length += vsnprintf(cur_pos, sizeof(msg.formatted_message) - msg.formatted_length, msg.fmt, msg.va);

    return msg.formatted_length;
}

// Sink
Sink::~Sink() { }

void Sink::flush() { }

// DefaultSink
DefaultSink::DefaultSink(bool enable_color) : m_enable_color(enable_color) { }

DefaultSink::~DefaultSink() { }

void DefaultSink::log(const Message &msg)
{
#if defined(OS_ANDROID)
    __android_log_print(android_log_level_label[msg.level], LOG_DEFAULT_MODULE_NAME,
                        "%s\n", msg.formatted_message);
#endif
    if (m_enable_color) {
#if defined(OS_POSIX)
        fprintf(stderr, "%s%s\n\x1b[0m", log_level_color[msg.level], msg.formatted_message);
#elif defined(OS_WIN)
        HANDLE h = GetStdHandle(STD_ERROR_HANDLE);
        WORD wOldColorAttrs;
        CONSOLE_SCREEN_BUFFER_INFO csbiInfo;

        // Save the current color
        GetConsoleScreenBufferInfo(h, &csbiInfo);
        wOldColorAttrs = csbiInfo.wAttributes;

        // Set the new color
        SetConsoleTextAttribute(h, log_level_color[msg.level] | FOREGROUND_INTENSITY);
        fprintf(stderr, "%s\n", msg.formatted_message);

        // Restore the original color
        SetConsoleTextAttribute(h, wOldColorAttrs);
#endif
    } else {
        fprintf(stderr, "%s\n", msg.formatted_message);
    }
}

void DefaultSink::flush() { } // stderr is not buffered.

// BasicFileSink
BasicFileSink::BasicFileSink(const char *file_name)
{
    m_fp = fopen(file_name, "a+");
    if (!m_fp) {
        fprintf(stderr, "Open log file %s failed! err: %d(%s)\n",
                file_name, errno, strerror(errno));
        m_fp = stderr;
    }
}

BasicFileSink::~BasicFileSink()
{
    if (m_fp) {
        fclose(m_fp);
        m_fp = nullptr;
    }
}

void BasicFileSink::log(const Message &msg)
{
    fprintf(m_fp, "%s\n", msg.formatted_message);
}

void BasicFileSink::flush()
{
    if (m_fp) {
        fflush(m_fp);
    }
}

// RotatingFileSink
const char *RotatingFileSink::ROTATION_FILE_NAME_SUFFIX = "-%Y%m%d_%H%M%S";

RotatingFileSink::RotatingFileSink(const char *file_name, int64_t max_size, int max_duration, int max_files)
    : m_file_name(file_name), m_max_size(max_size), m_max_duration(max_duration), m_max_files(max_files),
      m_fp(nullptr), m_cur_size(0), m_cur_begin_ts(-1), m_recycle_loop(nullptr)
{
    // Start recycler
    if (max_files > 0) {
        m_recycle_loop = new RecycleLoop(file_name, max_files, RECYCLE_CHECK_INTERVAL);
        m_recycle_loop->start();
    }
}

RotatingFileSink::~RotatingFileSink()
{
    if (m_fp) {
        fclose(m_fp);
        m_fp = nullptr;
    }

    if (m_recycle_loop) {
        m_recycle_loop->quit();
        m_recycle_loop = nullptr;
    }
}

void RotatingFileSink::log(const Message &msg)
{
    if (nullptr == m_fp
        || (m_max_size > 0 && m_cur_size + msg.formatted_length >= m_max_size)
        || (m_max_duration > 0 && m_cur_begin_ts > 0 && msg.time.tv_sec - m_cur_begin_ts >= m_max_duration)) {

        // Close current file
        if (m_fp) {
            fclose(m_fp);
            m_fp = nullptr;
        }
        
        // Open new file
        auto cur_file_name = m_file_name + Clock::getSystemDateTimeString(ROTATION_FILE_NAME_SUFFIX);
        m_fp = fopen(cur_file_name.c_str(), "a+");
        if (!m_fp) {
            fprintf(stderr, "Open log output file '%s' failed! - err: %s(%d)",
                    cur_file_name.c_str(), strerror(errno), errno);
            m_fp = stderr;
            m_max_size = m_max_duration = m_max_files = -1; // Disable rotation
            return;
        }

        // Reset
        m_cur_size = 0;
        m_cur_begin_ts = msg.time.tv_sec;
    }

    fprintf(m_fp, "%s\n", msg.formatted_message);
    m_cur_size += msg.formatted_length;
}

void RotatingFileSink::flush()
{
    if (m_fp) {
        fflush(m_fp);
    }
}

RotatingFileSink::RecycleLoop::RecycleLoop(const char *file_name, int max_files, int interval)
    : m_max_files(max_files), m_interval(interval), m_timer(EventLoop::getIOContext())
{
    std::string orig_path = file_name;
    auto pos = orig_path.rfind("/");

    if (std::string::npos != pos) {
        m_dir_path = orig_path.substr(0, pos);
        m_file_name = orig_path.substr(pos + 1);
    } else {
        m_dir_path = ".";
        m_file_name = orig_path;
    }
    //fprintf(stderr, "dir_path: %s, file_name: %s\n", m_dir_path.c_str(), m_file_name.c_str());

    m_timer_callback = std::bind(&RecycleLoop::recycle, this, std::placeholders::_1);

    Event e(1);
    postEvent(e);
}

RotatingFileSink::RecycleLoop::~RecycleLoop()
{
}

void RotatingFileSink::RecycleLoop::recycle(int)
{
    DIR *d = ::opendir(m_dir_path.c_str());
    if (d) {
        struct dirent *dir;
        std::list<std::time_t> suffixes;
        // Iterate file in dir
        while ((dir = ::readdir(d)) != NULL) {
            if (DT_REG == dir->d_type) {
                // Regular file
                std::string file_name = dir->d_name;
                //fprintf(stderr, "List dir: %s, file: %s\n", m_dir_path.c_str(), file_name.c_str());
                if (0 == file_name.compare(0, m_file_name.size(), m_file_name)) {
                    // File name starts with original file name
                    auto suffix = file_name.substr(m_file_name.size());
                    //fprintf(stderr, "Find log file suffix: %s\n", suffix.c_str());

                    // Parse suffix as time string
                    struct tm ftime;
                    const char *p = ::strptime(suffix.c_str(),
                                        RotatingFileSink::ROTATION_FILE_NAME_SUFFIX, &ftime);
                    if (nullptr != p) {
                        std::time_t tc = std::mktime(&ftime);
                        suffixes.push_back(tc);
                    }
                }
            }
        }
        ::closedir(d);

        // Sort by time
        suffixes.sort(std::less<std::time_t>());

        // Delete oldest files
        auto iter = suffixes.begin();
        while (suffixes.size() > m_max_files && iter != suffixes.end()) {
            std::stringstream time_ss;
            time_ss << std::put_time(std::localtime(&(*iter)), RotatingFileSink::ROTATION_FILE_NAME_SUFFIX);
            std::string fpath = m_dir_path + "/" + m_file_name + time_ss.str();
            //fprintf(stderr, "File to delete: %s\n", fpath.c_str());

            // Delete file
#ifdef OS_WIN
            if (-1 == _unlink(fpath.c_str())) {
#else
			if (-1 == unlink(fpath.c_str())) {
#endif
                fprintf(stderr, "Deleting log file '%s' failed! err: %s(%d)", fpath.c_str(), strerror(errno), errno);
            }

            iter = suffixes.erase(iter);
        }

    } else {
        fprintf(stderr, "Open directory failed! - errno: %s(%d)", strerror(errno), errno);
    }

    m_timer.asyncWait(std::chrono::seconds(m_interval), m_timer_callback);
}

void RotatingFileSink::RecycleLoop::onEvent(const Event &event)
{
    m_timer.asyncWait(std::chrono::seconds(0), m_timer_callback);
}

// UdpSink
UdpSink::UdpSink(const char *hostname, uint16_t port)
{
    // Creating socket
    if ((m_socket = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        fprintf(stderr, "Create udp socket failed! - err: %s(%d)",
                strerror(errno), errno);
    }
  
    memset(&m_serv_addr, 0, sizeof(m_serv_addr));
      
    // Filling server information
    m_serv_addr.sin_family = AF_INET;
    m_serv_addr.sin_port = htons(port);
    m_serv_addr.sin_addr.s_addr = INADDR_ANY;
    struct hostent *hostinfo = gethostbyname (hostname);
    if (hostinfo == NULL) {
        fprintf(stderr, "Unknown host %s.\n", hostname);
    }
    m_serv_addr.sin_addr = *(struct in_addr *)hostinfo->h_addr;
}

UdpSink::~UdpSink()
{
#ifdef OS_WIN
	closesocket(m_socket);
#endif
}

void UdpSink::log(const Message &msg)
{
    if (m_socket > 0) {
        // TODO: Long messages to fragment
        sendto(m_socket,
            msg.formatted_message,
            msg.formatted_length,
            0,
            (struct sockaddr *)&m_serv_addr,
            sizeof(m_serv_addr));
    } else {
        fprintf(stderr, "%s\n", msg.formatted_message);
    }
}

#if defined(OS_POSIX)
// SysLogSink
SysLogSink::SysLogSink(const char *ident, int option, int facility)
{
    ::openlog(ident, option, facility);
}

SysLogSink::~SysLogSink()
{
    ::closelog();
}

void SysLogSink::log(const Message &msg)
{
    const int SYSLOG_LEVELS[] = {
        LOG_CRIT,     // LOG_LEVEL_FATAL      - 0
        LOG_ERR,      // LOG_LEVEL_ERROR      - 1
        LOG_WARNING,  // LOG_LEVEL_WARNING    - 2
        LOG_INFO,     // LOG_LEVEL_INFO       - 3
        LOG_DEBUG,    // LOG_LEVEL_DEBUG      - 4
        LOG_DEBUG,    // LOG_LEVEL_VERBOSE    - 5
    };

    ::syslog(SYSLOG_LEVELS[msg.level], "%.*s", msg.formatted_length, msg.formatted_message);
}
#endif

} // End of namespace log
} // End of namespace base
