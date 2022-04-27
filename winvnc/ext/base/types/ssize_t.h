#pragma once

#if defined(OS_WIN)
    #if defined(_WIN64)
        typedef __int64 ssize_t;
    #else
        typedef long ssize_t;
    #endif

#include <BaseTsd.h>
    typedef SSIZE_T ssize_t;
#endif
