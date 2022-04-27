/*
 * try to catch the crash signal 
 * and print the crash stack
 */

#if defined(OS_POSIX)

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <time.h>
#include <sys/time.h>
#include <sys/syscall.h>
#include <signal.h>
#include <execinfo.h>
#include <cxxabi.h>

#include "crash_utils.h"
#include "base/log.h"

namespace base {
static inline char *gettime_string(char *sztime, int sztimelen)
{
    unsigned int year, month, day, hour, min, sec, usec;

#ifndef _WIN32
    struct timeval tv;
    struct tm *tm;

    gettimeofday(&tv, NULL);
    
    tm = localtime(&tv.tv_sec);

    year  = tm->tm_year + 1900;
    month = tm->tm_mon + 1;
    day   = tm->tm_mday;
    hour  = tm->tm_hour;
    min   = tm->tm_min;
    sec   = tm->tm_sec;
    usec  = tv.tv_usec;
#else  // windowns
    SYSTEMTIME sm = {0};
    GetLocalTime(&sm);

    year  = sm.wYear;
    month = sm.wMonth;
    day   = sm.wDay;
    hour  = sm.wHour;
    min   = sm.wMinute;
    sec   = sm.wSecond;
    usec  = sm.wMilliseconds * 1000;
#endif

    snprintf(sztime, sztimelen, "%4d-%02d-%02d-%02d:%02d:%02d.%06d", 
             year, month, day, hour, min, sec, usec);

    return sztime;
}

static const char * demangle(const char * symbol, char * buffer)
{
    size_t size;
    int    status;
    char * sbuffer = buffer, * demangled;

    /* first, try to demangle a c++ function name */
    if (1 == sscanf(symbol, "%*[^(]%*[^_]%384[^)+]", sbuffer)) {
        if (NULL != 
                (demangled = 
                 abi::__cxa_demangle(sbuffer, NULL, &size, &status))) {
            snprintf(buffer, 512, "%s", demangled);
            SAFE_FREE(demangled);
            return buffer;
        }
    }

    /* if that didn't work, try to get a regular c symbol */
    if (1 == sscanf(symbol, "%384s", sbuffer)) {
        return buffer;
    }

    /* if all else fails, just return the symbol */
    return symbol;
}

static int addr2line(
        int pid,
        const char *library,
        const char *addr,
        char *buffer)
{
    FILE * filp = NULL;
    char   procfile[128], /* line[256] */ * line = buffer;
    char * rline = NULL, * lib = NULL;

    unsigned long baseaddr, runaddr, vaddr;
    char * cmdline = procfile/*, * buffer = line */;

    /* process maps filepath */
    snprintf(procfile, 128, "/proc/%d/maps", pid);
//  MLOGD("ENTER FUNC-addr2line, library: %s, addr: %s\n", library, addr);

    if ((filp = fopen(procfile, "r")) == NULL) 
        return 1;

    while ((rline = fgets(line, 512, filp)) != NULL) {
        if (strchr(line, '\n'))    *(strchr(line, '\n')) = '\0';

    //  MLOGD("line: %s\n", line);
        if (strstr(line, "r-xp")  == NULL)  continue;
        if (strstr(line, library) != NULL) break;

    }
    fclose(filp);

    if (rline == NULL)
        return 2;

//  MLOGD("++++library: %s\n", line);
    if (strrchr(line, ' '))    lib = strrchr(line, ' ') + 1;
//  MLOGD("++++lib: %s\n", lib);

    if (strchr(line, '-'))     *(strchr(line, '-')) = '\0';
    baseaddr = (unsigned long) strtoll(line, NULL, 16);
    runaddr  = (unsigned long) strtoll(addr, NULL, 16);

    if (strstr(lib, ".so") == NULL)
        vaddr = runaddr;            // programe
    else
        vaddr = runaddr - baseaddr; // library.

//  MLOGD("lib: %s, baseaddr: 0x%08X(%s), runaddr: 0x%08X(%s), "
//           "vaddr: 0x%08X\n\n",
//           lib, baseaddr, line, runaddr, addr, vaddr);
    snprintf(cmdline, 128, "addr2line -C -f -e %s %x",
             lib, (unsigned int)vaddr);

    /* exec 'addr2line' command line. */
    if ((filp = popen(cmdline, "r")) == NULL) 
        return 3;

    memset(buffer, 0, 512);
    /* read addr2line command result. */
    if (fread(buffer, 512, 1, filp) < 0) {
        fclose(filp);
        return 4;
    }
    fclose(filp);

    /* Replace char-'\n' */
    if (strchr(buffer, '\n'))   *(strchr(buffer, '\n')) = '\t';
//  MLOGD("addr2line: %s\n", buffer);

    return 0;
}

static const char *demangle_using_addr2line(
        int pid,
        char *buffer,
        const char *symbol) 
{
    char  library[256], addr[128];
//  char  buffer[512];
    char  * lib = library;

//  MLOGD("demangle_using_addr2line, symbol: %s\n", symbol);

    /* /lib/libc.so.6(__libc_start_main+0xdc) [0x125e9c]
     * ./main [0x8048534] */
    /* try to demangle a c++ name */
    if (2 != sscanf(symbol, "%255[^(]%*[^[][%63[^]]", library, addr) && 
        2 != sscanf(symbol, "%255[^ ] [%63[^]]", library, addr)) {
        /* failure */
        buffer[0] = '\0';
        return buffer;
    }

    if (strrchr(library, '/'))     lib = strrchr(library, '/') + 1;
//  if (strchr(library, '.'))      *(strchr(library, '.')) = '\0';

    if (addr2line(pid, lib, addr, buffer) != 0) {
        /* fail to addr2line. */
        buffer[0] = '\0';
        return buffer;
    }

    return buffer;
}

#define SEGMENT_BUF_SIZE   4096
#define BACKTRACE_SIZE     128
#define SIGNAL_MAX         64      /* SIGRTMAX is 64 */
static const char * _signal[SIGNAL_MAX];       /* SIGNAL_MAX: 64 */
static int g_print_num = -1;
static bool g_goto_exit = false;

static void fault_handler_sigaction(int signum, siginfo_t * si, void *misc)
{
    /* This signal handler is running on an alternative stack,
     * Size of stack is 8192*5 bytes/40kb. */
    int print_num = 0;
    void *  stacks[BACKTRACE_SIZE]; /* 128*4 = 512 bytes on 8192 bytes stack. */
    char    buffer[512];            /* 512bytes */
    char    cmdline[256];           /* 256bytes */
                                    /* Total use. 1024+256bytes */
                                    /* stack buffer - 4096bytes */
    char stackbuffer[SEGMENT_BUF_SIZE];  
    char *sztime = buffer;
    int  offset = 0;   

    /* MLOGD will use 4096 bytes. */

    char **symbols;
    int    i, nptrs;
    FILE  *filp;
    int    pid;

    const char * sbuffer = NULL;
    const char *spline = "\n*****************************************"
                         "*******************************************\n\n";

    /* Next, printf is used instead of g_print(), since it's less likely to
     * deadlock */

    pid = getpid();
    snprintf(buffer, 512, "/proc/%d/cmdline", pid);

    memset(cmdline, 0, sizeof(cmdline));
    if ((filp = fopen(buffer, "r")) == NULL) {
        snprintf(cmdline, 256, "debugme");
    } else {   /* Read command line. */
        fread(cmdline, 256, 1, filp);
        fclose(filp);
    }

    offset += snprintf(stackbuffer+offset, SEGMENT_BUF_SIZE-offset, spline);
    offset += snprintf(stackbuffer+offset, SEGMENT_BUF_SIZE-offset, 
                       COLOR_LOG_WARNING"!PID: %d, cmdline: %s\n"
                       "!Caught signal %s(%d), may crash address is 0x%p\n"
                       "!errno: %d, code: %d\n" COLOR_LOG_NONE,
                       pid, cmdline, _signal[si->si_signo], si->si_signo, 
                       si->si_addr, si->si_errno, si->si_code);
    
    /* HEX backtrace of current stack */
    nptrs = backtrace(stacks, BACKTRACE_SIZE);
    /* translates the addresses into an array of strings 
     * that describe the addresses symbolically */
    symbols = backtrace_symbols(stacks, nptrs);

    if (symbols != NULL) {
        /* print func call back. */
        for (i = 0; i < nptrs; i ++) {
            if (si->si_signo == SIGSEGV) {
                /* only catch SIGSEGV signal, use addr2line. */
                sbuffer = demangle_using_addr2line(pid, buffer, symbols[i]);
            } else {
                buffer[0] = '\0';
                sbuffer   = buffer;
            }

            if (sbuffer[0] == '\0' || strstr(sbuffer, "??") != NULL) {
                offset += snprintf(stackbuffer+offset, SEGMENT_BUF_SIZE-offset,
                                   "%s\n", demangle(symbols[i], buffer));
            } else {
                offset += snprintf(stackbuffer+offset, SEGMENT_BUF_SIZE-offset,
                                   "%s", sbuffer);
            }

            offset  = offset < SEGMENT_BUF_SIZE ? offset : SEGMENT_BUF_SIZE;
        }

        offset += snprintf(stackbuffer+offset, SEGMENT_BUF_SIZE-offset, spline);
        SAFE_FREE(symbols);
    } else {
        offset += snprintf(stackbuffer+offset, SEGMENT_BUF_SIZE-offset, 
                           "!Backtrace stopped: previous frame inner to "
                           "this frame (corrupt stack?)");
        offset += snprintf(stackbuffer+offset, SEGMENT_BUF_SIZE-offset, spline);
    }

    /* output crash stack to vmxlog device. */
    ("%s", stackbuffer);

    gettime_string(sztime, 512);
    snprintf(stackbuffer, SEGMENT_BUF_SIZE, COLOR_LOG_WARNING
             "\nPls run 'gstack %d | grep -B5 -A8 \"<signal handler called>\"' "
             "to get crash stack (crash time: %s)\n\nPlease run 'gdb %s %ld' "
             "to continue debugging or 'kill -9 %d'" COLOR_LOG_NONE"\n",
             pid, sztime, cmdline, (long int)syscall(SYS_gettid), pid);

    while (true) {
        base::_error("%s", stackbuffer);

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
        SIGSEGV, SIGABRT, SIGFPE, SIGILL, SIGBUS, -1
    };

    /* initialize _signal string table. */
    for (i = 0; i < (SIGNAL_MAX); i ++)   _signal[i] = "unknown";
    _signal[SIGSEGV] = "SIGSEGV"; _signal[SIGABRT] = "SIGABRT";
    _signal[SIGFPE]  = "SIGFPE";  _signal[SIGILL]  = "SIGILL";
    _signal[SIGBUS]  = "SIGBUS";  _signal[SIGQUIT] = "SIGQUIT";
    _signal[SIGPIPE] = "SIGPIPE";

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
}
#endif
