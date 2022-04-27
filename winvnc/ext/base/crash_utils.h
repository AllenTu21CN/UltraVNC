#pragma once
#include "base/log.h"
namespace base {
#if defined(OS_POSIX)
#define COLOR_LOG_NONE     "\033[0m"
#define COLOR_LOG_ERROR    "\033[33;1m"
#define COLOR_LOG_WARNING  "\033[31;1m"
#define COLOR_LOG_INFO     "\033[37;0m"
#define COLOR_LOG_DEBUG    "\033[37;0m"
#define COLOR_LOG_VERBOSE  "\033[37;0m"


#ifndef SAFE_FREE
# define SAFE_FREE(p) if (p) { free(p); p=NULL; }
#endif

#ifndef SAFE_DELETE
# define SAFE_DELETE(p) if (p) { delete p; p=NULL; }
#endif

#ifndef SAFE_DELETE_A
# define SAFE_DELETE_A(p) if (p) { delete [] p; p =NULL; }
#endif

void sigint_setup(void);
void sigint_setup(int print_num, bool goto_exit);
void sigint_restore(void);

#endif

}
