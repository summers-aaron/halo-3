#ifndef CSERIES_BIT_OPERATIONS_H
#define CSERIES_BIT_OPERATIONS_H

#include "cseries/cseries.h"

#ifndef SET_BIT
#define SET_BIT(flags, bit, value) ((value) ? ((flags) |= FLAG(bit)) : ((flags) &= ~FLAG(bit)))
#endif

static __forceinline bool bit_test(unsigned long flags, long bit)
{
    return (flags >> bit) & 1;
}

static __forceinline void session_flag_set(unsigned long &flags, long bit, bool value)
{
    if (value)
        flags |= 1 << bit;
    else
        flags &= ~(1 << bit);
}

static __forceinline void operation_flag_set(unsigned short &flags, long bit, bool value)
{
    if (value)
        flags |= 1 << bit;
    else
        flags &= ~(1 << bit);
}

// Branchless set-or-clear; the read-modify-write shape keeps adjacent bit updates from merging.
static __forceinline void set_bit_value(unsigned long *flags, long bit, bool value)
{
    *flags = (*flags & ~(1 << bit)) | ((value ? 1 : 0) << bit);
}

#endif // CSERIES_BIT_OPERATIONS_H
