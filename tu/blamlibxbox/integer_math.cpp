// blamlibxbox/integer_math.obj: from the records 0x1099a8 0x119de8 0x1b6fa8 0x200cf8 0x21a520
// @compiler xdk6130
// @flags /TP /O2 /Oc
#include <ppcintrinsics.h>
#define INTEGER_MATH_DEFINING_TU
#include "math/integer_math.h"
#include "cseries/crt_xdk.h"
template unsigned long count_bits<long>(long);
// @address 0x1099a8 bit_vector_find_highest_bit
extern "C" long bit_vector_find_highest_bit(const unsigned long *vector, long bit_count) {
    long word_count = (bit_count + 31) >> 5;
    long remainder = bit_count & 31;
    unsigned long mask = ~0UL;
    if (remainder > 0)
        mask >>= 32 - remainder;
    long bit_index = highest_bit_set(vector[word_count - 1] & mask);
    if (bit_index >= 0)
        bit_index += (word_count - 1) * 32;
    if (bit_index != -1)
        return bit_index;
    long word_index = word_count - 2;
    long bit_base = word_index * 32;
    const unsigned long *word = &vector[word_index];
    while (bit_index == -1) {
        if (word_index < 0)
            return bit_index;
        bit_index = highest_bit_set(*word);
        if (bit_index >= 0)
            bit_index += bit_base;
        --word_index;
        --word;
        bit_base -= 32;
    }
    return bit_index;
}

// @address 0x119de8 ?bit_vector_lowest_bit_set@@YAJPBKJ@Z
long bit_vector_lowest_bit_set(const unsigned long *vector, long bit_count)
{
    long word_count = (bit_count + 31) >> 5;
    long bit_index = -1;
    for (long word_index = 0; bit_index == -1 && word_index < word_count - 1; ++word_index) {
        bit_index = lowest_bit_set(vector[word_index]);
        if (bit_index >= 0)
            bit_index += word_index * 32;
    }
    if (bit_index == -1) {
        long remainder = bit_count & 31;
        unsigned long mask = ~0UL;
        if (remainder > 0)
            mask >>= 32 - remainder;
        unsigned long last_word = vector[word_count - 1] & mask;
        bit_index = lowest_bit_set(last_word);
        if (bit_index >= 0)
            bit_index += (word_count - 1) * 32;
    }
    return bit_index;
}

// @address 0x200cf8 bit_vector_clear_range
extern "C" void bit_vector_clear_range(unsigned long *vector, long bit_count)
{
    if (bit_count != 0) {
        long last = ((bit_count + 31) >> 5) - 1;
        if (last != 0)
            XMemSet(vector, 0, last * sizeof(unsigned long));
        vector[last] &= ~((unsigned long)((1i64 << (bit_count - last * 32)) - 1));
    }
}
long bit_vector_count_bits(const unsigned long *vector, long bit_count) {
    long word_count = (bit_count + 31) >> 5;
    long result = 0;
    for (long word_index = 0; word_index < word_count - 1; ++word_index)
        result += count_bits(vector[word_index]);
    long remainder = bit_count & 31;
    unsigned long mask = ~0UL;
    if (remainder > 0)
        mask >>= 32 - remainder;
    long last_word = vector[word_count - 1] & mask;
    result += count_bits(last_word);
    return result;
}
