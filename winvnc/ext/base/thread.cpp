#include "base/thread.h"
#include "base/log.h"

#include <algorithm>
#include <functional>
#include <thread>

#include <assert.h>
#include <condition_variable>
#include <mutex>

#if defined(OS_POSIX)
    #include <sys/syscall.h>
    #include <sys/time.h>
    #include <unistd.h>
#endif

#if defined(OS_LINUX) || defined(OS_ANDROID)
    #include <sys/prctl.h>
#endif
#if defined(OS_ANDROID)
    #define HAVE_PRCTL
#endif

#if defined(OS_WIN)
    #include <Windows.h>
#endif

namespace base {

#define INIT_PRIV(d) \
    ThreadPrivate *d = NULL; \
    if (m_priv) { \
        d = static_cast<ThreadPrivate *>(m_priv); \
    } \
    assert(d); \


class ThreadPrivate
{
public:
    ThreadPrivate();
    ~ThreadPrivate();

    void start(std::function<void()> &&entry);
    void wait();
    void innerEntry();

private:
    std::thread *m_thrd;
    std::function<void()> m_inner_run;
    std::function<void()> m_thrd_entry;
    std::mutex m_mt;
    std::condition_variable m_cv;
}; // End of class ThreadPrivate

ThreadPrivate::ThreadPrivate()
    : m_thrd(nullptr),
    m_inner_run(std::bind(&ThreadPrivate::innerEntry, this)),
    m_thrd_entry(nullptr)
{
}

ThreadPrivate::~ThreadPrivate()
{
    wait();
}

void ThreadPrivate::start(std::function<void()> &&entry)
{
    //m_thrd = new std::thread(std::move(entry));
    if (m_thrd_entry) return;
    m_thrd_entry = std::move(entry);
    std::unique_lock<std::mutex> lk(m_mt);
    m_thrd = new std::thread(m_inner_run);
    m_cv.wait(lk);
}

void ThreadPrivate::wait()
{
    if (m_thrd) {
        if (m_thrd->joinable()) {
            m_thrd->join();
        }

        delete m_thrd;
        m_thrd = nullptr;
    }
}

void ThreadPrivate::innerEntry()
{
    std::unique_lock<std::mutex> lk(m_mt);
    m_cv.notify_one();
    lk.unlock();

    if (m_thrd_entry) {
        m_thrd_entry();
    }
}

Thread::Thread()
    : m_priv(new ThreadPrivate)
{
}

Thread::~Thread()
{
    if (m_priv) {
        ThreadPrivate *d = static_cast<ThreadPrivate *>(m_priv);
        if (d) {
            delete d;
            m_priv = NULL;
        }
    }
}

void Thread::start()
{
    INIT_PRIV(d);

    d->start(std::move(std::bind(&Thread::run, this)));
}

void Thread::wait()
{
    INIT_PRIV(d);

    d->wait();
}

void Thread::innerRun()
{
    INIT_PRIV(d);

    d->innerEntry();
}

void Thread::sleep(unsigned long secs)
{
    std::chrono::seconds duration(secs);
    std::this_thread::sleep_for(duration);
}

void Thread::msleep(unsigned long msecs)
{
    std::chrono::milliseconds duration(msecs);
    std::this_thread::sleep_for(duration);
}

void Thread::usleep(unsigned long usecs)
{
    std::chrono::microseconds duration(usecs);
    std::this_thread::sleep_for(duration);
}

uint64_t Thread::getNativeThreadId()
{
#if defined(OS_LINUX) || defined(OS_ANDROID)
    pid_t tid = syscall(__NR_gettid);
#elif defined(OS_MACOS)
    uint64_t tid;
    pthread_threadid_np(NULL, &tid);
#elif defined(OS_WIN)
    DWORD tid = GetCurrentThreadId();
#endif

    return static_cast<uint64_t>(tid);
}

#if defined(OS_POSIX)
// Does some magic and calculate the Unix scheduler priorities
// sched_policy is IN/OUT: it must be set to a valid policy before calling this function
// sched_priority is OUT only
static bool calculateUnixPriority(int priority, int *sched_policy, int *sched_priority)
{
#ifdef SCHED_IDLE
    if (priority == Thread::PRIORITY_IDLE) {
        *sched_policy = SCHED_IDLE;
        *sched_priority = 0;
        return true;
    }
    const int lowestPriority = Thread::PRIORITY_LOWEST;
#else
    const int lowestPriority = Thread::PRIORITY_IDLE;
#endif
    const int highestPriority = Thread::PRIORITY_TIMECRITICAL;

    int prio_min;
    int prio_max;
#if defined(OS_VXWORKS) && defined(VXWORKS_DKM)
    // for other scheduling policies than SCHED_RR or SCHED_FIFO
    prio_min = SCHED_FIFO_LOW_PRI;
    prio_max = SCHED_FIFO_HIGH_PRI;

    if ((*sched_policy == SCHED_RR) || (*sched_policy == SCHED_FIFO))
#endif
    {
    prio_min = sched_get_priority_min(*sched_policy);
    prio_max = sched_get_priority_max(*sched_policy);
    }

    if (prio_min == -1 || prio_max == -1)
        return false;

    int prio;
    // crudely scale our priority enum values to the prio_min/prio_max
    prio = ((priority - lowestPriority) * (prio_max - prio_min) / highestPriority) + prio_min;
    prio = std::max(prio_min, std::min(prio_max, prio));

    *sched_priority = prio;
    return true;
}
#endif // defined(OS_POSIX)

void Thread::setPriority(Priority priority)
{
#if defined(OS_POSIX)
    pthread_t current_thread = pthread_self();
    int sched_policy;
    sched_param param;

    if (pthread_getschedparam(current_thread, &sched_policy, &param) != 0) {
        // Failed to get the scheduling policy
        base::_warning("Cannot get scheduler parameters");
        return;
    }

    int prio;
    if (!calculateUnixPriority(priority, &sched_policy, &prio)) {
        // Failed to get the scheduling parameters
        base::_warning("Cannot determine scheduler priority range");
        return;
    }

    param.sched_priority = prio;
    int status = pthread_setschedparam(current_thread, sched_policy, &param);

#elif defined(OS_WIN)
    // TODO:
    int prio;
    switch (priority) {
    case PRIORITY_IDLE :
        prio = THREAD_PRIORITY_IDLE;
        break;

    case PRIORITY_LOWEST :
        prio = THREAD_PRIORITY_LOWEST;
        break;

    case PRIORITY_LOW :
        prio = THREAD_PRIORITY_BELOW_NORMAL;
        break;

    case PRIORITY_NORMAL :
        prio = THREAD_PRIORITY_NORMAL;
        break;

    case PRIORITY_HIGH :
        prio = THREAD_PRIORITY_ABOVE_NORMAL;
        break;

    case PRIORITY_HIGHEST :
        prio = THREAD_PRIORITY_HIGHEST;
        break;

    case PRIORITY_TIMECRITICAL :
        prio = THREAD_PRIORITY_TIME_CRITICAL;
        break;
#if 0
    case PRIORITY_INHERIT :
#endif
    default:
        base::_warning("Argument cannot be InheritPriority");
        return;
    }

    HANDLE thread = GetCurrentThread();
    if (!SetThreadPriority(thread, prio)) {
        base::_warning("Failed to set thread priority");
    }
#endif
}

void Thread::setThreadName(const char *name)
{
    if (!name) return;

#if defined(OS_LINUX)
    pthread_setname_np(pthread_self(), name);
#elif defined(OS_ANDROID)

  #if defined(HAVE_ANDROID_PTHREAD_SETNAME_NP)
    /* pthread_setname_np fails rather than truncating long strings */
    char buf[THREAD_NAME_MAX_LENGTH];   // MAX_TASK_COMM_LEN=16 is hard-coded into bionic
    strncpy(buf, name, sizeof(buf)-1);
    buf[sizeof(buf)-1] = '\0';
    int err = pthread_setname_np(pthread_self(), buf);
    if (err != 0) {
        base::_warning("Unable to set the name of current thread to '%s': %s",
            buf, strerror(err));
    }
  #elif defined(HAVE_PRCTL)
    prctl(PR_SET_NAME, (unsigned long)name, 0, 0, 0);
  #else
    base::_warning("No way to set current thread's name (%s)", name);
  #endif

#elif defined(OS_MACOS)
#elif defined(OS_WIN)
#endif
}


} // End of namespace base
