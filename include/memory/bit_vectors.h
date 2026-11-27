#ifndef MEMORY_BIT_VECTORS_H
#define MEMORY_BIT_VECTORS_H

static __forceinline bool bit_vector_test(const unsigned long *vector, long bit)
{
    return (vector[bit >> 5] & (1 << (bit & 31))) != 0;
}

static __forceinline bool bit_vector_test_unsigned(const unsigned long *vector, unsigned long bit)
{
    return (vector[bit >> 5] & (1 << (bit & 31))) != 0;
}

static __forceinline void bit_vector_set(unsigned long *vector, long bit)
{
    vector[bit >> 5] |= 1 << (bit & 31);
}

static inline void bit_vector_or(long count, const unsigned long *a, const unsigned long *b, unsigned long *out)
{
    long n = count >> 5;
    for (long i = n - 1; i >= 0; i--)
        out[i] = a[i] | b[i];
}

#endif // MEMORY_BIT_VECTORS_H
