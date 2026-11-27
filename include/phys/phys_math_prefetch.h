#ifndef INCLUDE_PHYS_MATH_PREFETCH_H
#define INCLUDE_PHYS_MATH_PREFETCH_H

#include <ppcintrinsics.h>

namespace phys_math {
    template <int SIZE> static __forceinline void force_prefetch(const void *p)
    {
        __dcbt(0, p);
        if (SIZE > 128) { __dcbt(128, p); }
        if (SIZE > 256) { __dcbt(256, p); }
        if (SIZE > 384) { __dcbt(384, p); }
    }
}

#endif
