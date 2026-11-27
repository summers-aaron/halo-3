#ifndef INCLUDE_PHYS_SCALAR_TYPES_H
#define INCLUDE_PHYS_SCALAR_TYPES_H

typedef float phys_real;
typedef int phys_bool32;

#if defined(PHYS_ARCH_X64) || defined(_WIN64)
typedef unsigned __int64 phys_ulong;
#else
typedef unsigned long phys_ulong;
#endif

enum phys_result {
    PHYS_SUCCESS = 0,
    PHYS_FAILURE = 1
};

#endif // INCLUDE_HAVOK_HK_SCALAR_TYPES_H
