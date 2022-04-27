#pragma once

#include <stdlib.h>

namespace base {
/**
 * Allocate size bytes of uninitialized storage whose alignment is specified
 * by alignment.
 * The size should between [1, INT_MAX - 32].
 * The alignment must be a power of 2 at least as large as sizeof(void *).
 */
void *alignedAlloc(size_t size, size_t alignment);

/**
 * Deallocates the space previously allocated by alignedAlloc().
 * If ptr is a null pointer, the function does nothing.
 */
void alignedFree(void *ptr);

} // End of namespace base
