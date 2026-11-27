#ifndef MATH_INTEGER_MATH_H
#define MATH_INTEGER_MATH_H

#include <ppcintrinsics.h>

// Functions defined in tu/blamlibxbox/integer_math.cpp that other units call.

#include "math/integer_math_count_bits.h"

#ifdef INTEGER_MATH_DEFINING_TU
template unsigned long count_bits<unsigned long>(unsigned long);
#endif

// Bit-scan helpers: index of the highest set bit, and index of the lowest set bit.
__forceinline long highest_bit_set(unsigned long value)
{
    return 31 - _CountLeadingZeros(value);
}

__forceinline long lowest_bit_set(unsigned long value)
{
    return highest_bit_set(value & -(long)value);
}

long bit_vector_lowest_bit_set(const unsigned long *vector, long bit_count);
long bit_vector_count_bits(const unsigned long *vector, long bit_count);
extern "C" void bit_vector_clear_range(unsigned long *vector, long bit_count);

#endif // MATH_INTEGER_MATH_H
