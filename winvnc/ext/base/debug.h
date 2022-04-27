#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "base/log.h"

#if defined(WINDOWS)
  #define SNPRINTF sprintf_s
  #define CONSOLE_PAUSE() system("pause")
#else
  #include <errno.h>
  #define SNPRINTF snprintf
  #define CONSOLE_PAUSE() { fprintf(stderr, "Put anything to continue:"); getchar(); }
#endif
#define CONSOLE_PAUSE2(msg) { \
    base::_info(msg); \
    CONSOLE_PAUSE(); \
}

inline int F_OPEN(FILE **f, const char *file_name, const char *mode)
{
#if defined(WINDOWS)
    return fopen_s(f, file_name, mode);
#else
    *f = fopen(file_name, mode);
    if(*f)
        return 0;
    return errno;
#endif
}

/* Sample
BASE_DEBUG_INIT("test");

void TEST(){
    BASE_DEBUG_FILE_OPEN(".");

    BASE_DEBUG_2_FILE("123",3);
    BASE_DEBUG_2_LOG("123", 3, 2, "");
    BASE_DEBUG_2_LOG("123", 3, -1, "test:");

    BASE_DEBUG_FILE_CLOSE();
}
*/

#define L_BASE_DEBUG_INIT(name) \
    FILE *_base_local_fp_ = NULL; \
    const char *_base_local_name_ = name;

#define BASE_DEBUG_INIT(name) \
    static FILE *_base_local_fp_ = NULL; \
    static const char *_base_local_name_ = name;

#define BASE_DEBUG_FILE_OPEN(file_path) { \
    if(_base_local_fp_ == NULL){ \
        char _filename[255] = {0};\
        SNPRINTF(_filename, 254, "%s/debug_%s.bin", file_path, _base_local_name_); \
        F_OPEN(&_base_local_fp_, _filename, "wb"); \
        if (_base_local_fp_ == NULL){ \
            base::_error("open file(%s) failed", _filename); \
        } \
    } \
}

#define BASE_DEBUG_FILE_CLOSE() { \
    if(_base_local_fp_){ \
        fclose(_base_local_fp_); \
        _base_local_fp_ = NULL; \
    } \
}

#define BASE_DEBUG_2_FILE(data, size) { \
    if(_base_local_fp_) \
        fwrite(data, 1, size, _base_local_fp_); \
}

#define BASE_DEBUG_2_LOG(data, data_size) BASE_DEBUG_2_LOG_WITH_MSG(data, data_size, -1, "")

#define BASE_DEBUG_2_LOG_LIMITED(data, data_size, limited_size) BASE_DEBUG_2_LOG_WITH_MSG(data, data_size, limited_size, "")

#define BASE_DEBUG_2_LOG_WITH_MSG(data, data_size, limited_size, msg_prefix) BASE_DEBUG_2_LOG_WITH_MSG2(data, data_size, limited_size, msg_prefix, debug)

#define BASE_DEBUG_2_LOG_WITH_MSG2(data, data_size, limited_size, msg_prefix, log_level) { \
    unsigned size = limited_size > 0 && limited_size < data_size ? limited_size : data_size;\
    char *strs = ::_base_local_dumpData2Str16(data, size); \
    base::_##log_level("%s%s", msg_prefix, strs); \
    free(strs); \
}

inline char *_base_local_dumpData2Str16(const uint8_t* data, unsigned len)
{
    int size = 2+len*3+1;
    char *out = (char *)malloc(size);
    memset(out, 0 ,size);
    SNPRINTF(out, 3, "0x");
    int offet = 2;
    for(unsigned i = 0 ; i < len ; ++i)
    {
        SNPRINTF(&out[offet], 4, "%02x ", data[i]&0xff);
        offet += 3;
    }
    return out;
}


