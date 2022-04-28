#ifndef STRPTIME_H
#define STRPTIME_H

// from https://github.com/res2001/strptime

#include <time.h>

#ifdef _WIN32
char* strptime(const char *buf, const char *fmt, struct tm *tm);
#endif

#endif // STRPTIME_H
