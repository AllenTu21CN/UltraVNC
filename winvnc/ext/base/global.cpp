#include "base/global.h"

/**
 * FIX:
 *   VS2017 with VS2015 (v140) toolset link error.
 */
#if _WIN64 && FIX_VS_TOOLSET
template <typename _BidIt>
static inline void _Reverse_tail(_BidIt _First, _BidIt _Last) throw() {
    for (; _First != _Last && _First != --_Last; ++_First) {
        const auto _Temp = *_First;
        *_First = *_Last;
        *_Last = _Temp;
    }
}
extern "C"  __declspec(noalias) void __cdecl __std_reverse_trivially_swappable_8(void * _First, void * _Last) noexcept {
    _Reverse_tail(static_cast<unsigned long long *>(_First), static_cast<unsigned long long *>(_Last));
}
#endif
