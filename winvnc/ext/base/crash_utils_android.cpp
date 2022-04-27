/* @file   crash_utils_android.cpp
 * @brief  catch the crash stack for debug
 * @author fangzh
 *
 * @date   2018/8
 */

#ifdef OS_ANDROID

#include <iostream>
#include <iomanip>

#include <unistd.h>

#include <unwind.h>
#include <dlfcn.h>

#include <sstream>
#include <android/log.h>

#include "crash_utils.h"

namespace {

struct BacktraceState {
    void** current;
    void** end;
};

static _Unwind_Reason_Code unwind_callback(struct _Unwind_Context* context, void* arg)
{
    BacktraceState* state = static_cast<BacktraceState*>(arg);
    uintptr_t pc = _Unwind_GetIP(context);
    if (pc) {
        if (state->current == state->end) {
            return _URC_END_OF_STACK;
        } else {
            *state->current++ = reinterpret_cast<void*>(pc);
        }
    }
    return _URC_NO_REASON;
}

} // end of namespace

namespace base {

static size_t capture_backtrace(void** buffer, size_t max)
{
    BacktraceState state = {buffer, buffer + max};
    _Unwind_Backtrace(unwind_callback, &state);

    return state.current - buffer;
}

static void dump_backtrace(std::ostream& os, void** buffer, size_t count)
{
    for (size_t idx = 0; idx < count; ++idx) {
        const void* addr = buffer[idx];
        const char* symbol = "";

        Dl_info info;
        if (dladdr(addr, &info) && info.dli_sname) {
            symbol = info.dli_sname;
        }

        os << "  #" << std::setw(2) << idx << ": " << addr << "  " << symbol << "\n";
    }
}

#define SEGMENT_BUF_SIZE   4096
#define BACKTRACE_SIZE     128
#define SIGNAL_MAX         64      /* SIGRTMAX is 64 */
static const char * _signal[SIGNAL_MAX];       /* SIGNAL_MAX: 64 */
static int g_print_num = 10;
static bool g_goto_exit = false;

static void fault_handler_sigaction(int signum, siginfo_t * si, void *misc)
{
    /* This signal handler is running on an alternative stack,
     * Size of stack is 8192*5 bytes/40kb. */
    const size_t max = BACKTRACE_SIZE;
    int print_num = 0;
    void *stacks[max];  /* 128*4 = 512 bytes on 8192 bytes stacks. */
    std::ostringstream oss;

    dump_backtrace(oss, stacks, capture_backtrace(stacks, max));

    while (true) {
        _error("!Catch signal %s(%d), may crash address is 0x%p\n"
            "!errno: %d, code: %d\n\n%s", _signal[si->si_signo],
            si->si_signo, si->si_addr, si->si_errno, si->si_code,
            oss.str().c_str());

        print_num ++;
        if (g_print_num >= 0 && print_num >= g_print_num)
            break;

        // output tips warn to debug it.
        //if (si->si_signo == SIGABRT) break;

        sleep(10);
    }

    /* restore signals' handle, and raise the signal. */
    sigint_restore();

    if (g_goto_exit)
        exit(-1);
    else
        raise(signum);
}

static void *signal_stack = NULL;

void sigint_setup(int print_num, bool goto_exit)
{
    g_print_num = print_num;
    g_goto_exit = goto_exit;

    if (print_num > 0) {
        sigint_setup();
    } else {
        sigint_restore();
    }
}

void sigint_setup(void)
{
    unsigned int i ;

    /* We run the signal handler on an alternative stack
     * because we might have crashed because of a stack overflow */

    /* We use this value rather than SIGSTKSZ because 
     * we would end up overruning such a small stack. */
    const unsigned int kSigStackSize = 40960;   /* 40kb */

    stack_t stack;

    struct sigaction action;

    /* The list of signal which we consider to be crashes.
     * The default action for all these signals must be Core 
     * (see man 7 signal) because we rethrow the signal 
     * after handling it and expect that it'll be fatal */
    const int kExceptionSignals[] = {
        SIGSEGV, SIGABRT, SIGFPE, SIGILL, SIGBUS, SIGTRAP, -1
    };

    /* initialize _signal string table. */
    for (i = 0; i < (SIGNAL_MAX); i ++)   _signal[i] = "unknown";
    _signal[SIGSEGV] = "SIGSEGV"; _signal[SIGABRT] = "SIGABRT";
    _signal[SIGFPE]  = "SIGFPE";  _signal[SIGILL]  = "SIGILL";
    _signal[SIGBUS]  = "SIGBUS";  _signal[SIGQUIT] = "SIGQUIT";
    _signal[SIGPIPE] = "SIGPIPE"; _signal[SIGTRAP] = "SIGTRAP";

    /* Only set an alternative stack if there isn't already one, or if the current
     * one is too small.  */
    if (sigaltstack (NULL,  &stack) == -1 || !stack.ss_sp ||
            stack.ss_size < kSigStackSize)
    {
        signal_stack = malloc(kSigStackSize);
        memset(&stack, 0, sizeof (stack));
        stack.ss_sp = signal_stack;
        stack.ss_size = kSigStackSize;

        if (sigaltstack (&stack, NULL) == -1)
            return ;
    }


    memset (&action, 0, sizeof (action));

    /* Block most signal while SIGSEGV is being handled.  */
    /* Signals SIGKILL, SIGSTOP cannot be blocked.  */
    /* Signals SIGCONT, SIGTSTP, SIGTTIN, SIGTTOU are not blocked because
     * dealing with these signals seems dangerous.  */
    /* Signals SIGILL, SIGABRT, SIGFPE, SIGSEGV, SIGTRAP, SIGIOT, SIGEMT,
     * SIGBUS, SIGSYS, SIGSTKFLE are not blocked 
     * because these are synchronous signal which may require immediate 
     * intervention, otherwise the process may starve.  */
    sigemptyset (&action.sa_mask);
#ifdef SIGHUP
    sigaddset (&action.sa_mask,SIGHUP);
#endif
#ifdef SIGINT
    sigaddset (&action.sa_mask,SIGINT);
#endif
#ifdef SIGQUIT
    sigaddset (&action.sa_mask,SIGQUIT);
#endif
#ifdef SIGPIPE
    sigaddset (&action.sa_mask,SIGPIPE);
#endif
#ifdef SIGALRM
    sigaddset (&action.sa_mask,SIGALRM);
#endif
#ifdef SIGTERM
    sigaddset (&action.sa_mask,SIGTERM);
#endif
#ifdef SIGUSR1
    sigaddset (&action.sa_mask,SIGUSR1);
#endif
#ifdef SIGUSR2
    sigaddset (&action.sa_mask,SIGUSR2);
#endif
#ifdef SIGCHLD
    sigaddset (&action.sa_mask,SIGCHLD);
#endif
#ifdef SIGCLD
    sigaddset (&action.sa_mask,SIGCLD);
#endif
#ifdef SIGURG
    sigaddset (&action.sa_mask,SIGURG);
#endif
#ifdef SIGIO
    sigaddset (&action.sa_mask,SIGIO);
#endif
#ifdef SIGPOLL
    sigaddset (&action.sa_mask,SIGPOLL);
#endif
#ifdef SIGXCPU
    sigaddset (&action.sa_mask,SIGXCPU);
#endif
#ifdef SIGXFSZ
    sigaddset (&action.sa_mask,SIGXFSZ);
#endif
#ifdef SIGVTALRM
    sigaddset (&action.sa_mask,SIGVTALRM);
#endif
#ifdef SIGPROF
    sigaddset (&action.sa_mask,SIGPROF);
#endif
#ifdef SIGPWR
    sigaddset (&action.sa_mask,SIGPWR);
#endif
#ifdef SIGLOST
    sigaddset (&action.sa_mask,SIGLOST);
#endif
#ifdef SIGWINCH
    sigaddset (&action.sa_mask,SIGWINCH);
#endif

    /* mask all exception signals when we're handling one of them */
    for (i = 0; kExceptionSignals[i] != -1; ++ i)
        sigaddset (&action.sa_mask, kExceptionSignals[i]);

    /* SA_ONSTACK: if a process has a limit on the size of the stack.
     *  If a process exceeds that limit it will get a signal, often SIGSEGV,
     *  but it can vary from os to os.
     *
     *  Now, how can you catch that signal?
     *  Without an alternate stack, the signal handler could never run.
     *  
     *  e.g., a process was dying from a blown stack 
     *  but its current directory was in a filesystem 
     *  that was too small to hold a core dump.
     *  My handler needed to catch the signal, cd to a large filesystem, 
     *  set the signal action back to default, and resend the signal to itself.
     *  I could not have done that without an alternate stack.
     *
     *  That was the only time that I needed an alternate stack. */
    action.sa_flags = SA_ONSTACK | SA_SIGINFO;
    
    action.sa_sigaction = fault_handler_sigaction;

    for (i = 0; kExceptionSignals[i] != -1; ++ i) {
        sigaction (kExceptionSignals[i], &action, (struct sigaction *)NULL);
    }

    /* SIGQUIT: catch */
    sigaction (SIGQUIT, &action, (struct sigaction *)NULL);

    /* Suppress SIGPIPE. Without this, attempting to send on a socket whose
     * other end is closed will result in a SIGPIPE signal being raised to
     * our process, which by default will terminate the process, which we
     * don't want. By specifying this flag, we'll just get the error EPIPE
     * instead and can handle the error gracefully. */
    memset (&action, 0, sizeof (action));
    action.sa_handler = SIG_IGN;
    sigemptyset (&action.sa_mask);
    action.sa_flags = 0;
    sigaction (SIGPIPE, &action, NULL);
}

void sigint_restore (void)
{
    struct sigaction action;
    unsigned int i ;

    /* The list of signal which we consider to be crashes.
     * The default action for all these signals must be Core
     * (see man 7 signal) because we rethrow the signal
     * after handling it and expect that it'll be fatal */
    const int kExceptionSignals[] = {
        SIGSEGV, SIGABRT, SIGFPE, SIGILL, SIGBUS, -1
    };

    memset (&action, 0, sizeof (action));
    action.sa_handler = SIG_DFL;
    
    for (i = 0; kExceptionSignals[i] != -1; ++ i) {
        sigaction (kExceptionSignals[i], &action, NULL);
    }

    sigaction (SIGQUIT, &action, NULL);

    SAFE_FREE(signal_stack);
}

} // End of namespace base

#endif // OS_ANDROID
