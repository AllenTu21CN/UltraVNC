#pragma once

#include <algorithm>
#include <cmath>
#include <stddef.h>  // For size_t.

namespace base {

// The arraysize(arr) macro returns the # of elements in an array arr.  The
// expression is a compile-time constant, and therefore can be used in defining
// new arrays, for example.  If you use arraysize on a pointer by mistake, you
// will get a compile-time error.  For the technical details, refer to
// http://blogs.msdn.com/b/the1/archive/2004/05/07/128242.aspx.

// This template function declaration is used in defining arraysize.
// Note that the function doesn't need an implementation, as we only
// use its type.
template <typename T, size_t N> char(&ArraySizeHelper(T(&array)[N]))[N];
#define arraysize(array) (sizeof(ArraySizeHelper(array)))

static inline bool fuzzyCompare(double p1, double p2)
{
    using namespace std;
    return (fabs(p1 - p2) * 1000000000000. <= min(fabs(p1), fabs(p2)));
}

static inline bool fuzzyCompare(float p1, float p2)
{
    using namespace std;
    return (fabs(p1 - p2) * 100000.f <= min(fabs(p1), fabs(p2)));
}

static inline size_t roundUp(size_t value, size_t alignment)
{
    return ((value + (alignment - 1)) & ~(alignment - 1));
}

} // End of namespace base
