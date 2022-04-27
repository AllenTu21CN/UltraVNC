#include "base/aligned_alloc.h"

#include <stdio.h>
#include <limits.h>

namespace base {

void *alignedAlloc(size_t size, size_t alignment)
{
    // Check size range
    if (!size || size > (INT_MAX - 32))
        return NULL;

    // Is alignment power of 2
    if (alignment < sizeof(void *) || (alignment & (alignment - 1)) != 0)
        return NULL;

    void *ptr = NULL;
#if defined(OS_WIN)
    ptr = _aligned_malloc(size, alignment);
#elif defined(OS_POSIX)
    if (posix_memalign(&ptr, alignment, size))
        ptr = NULL;
#else
    // Hacking
    ptr = malloc(size + alignment);
    if (!ptr)
        return ptr;
    long diff = ((~(long)ptr) & (alignment - 1)) + 1;
    ptr = (char *)ptr + diff;
    ((char *)ptr)[-1] = diff;
#endif

    return ptr;
}

void alignedFree(void *ptr)
{
    if (!ptr) return;

#if defined(OS_WIN)
    _aligned_free(ptr);
#elif defined(OS_POSIX)
    //fprintf(stderr, "base::alignedFree ptr = %p\n", ptr);
    free(ptr);
#else
    // Hacking
    if (ptr) {
        int v = ((char *)ptr)[-1];
        free((char *)ptr - v);
    }
#endif
}

} // End of namespace base
