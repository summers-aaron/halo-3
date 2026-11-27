#ifndef MEMORY_WRAPPED_FLAGS_H
#define MEMORY_WRAPPED_FLAGS_H

template <typename t_type> class c_basic_buffer;

#include <stddef.h>
#include <string.h>

class c_wrapped_flags
{
public:
    __forceinline c_wrapped_flags() : m_flags(NULL), m_count(0) {}

    __forceinline void set_bits(void *storage, unsigned long bit_count)
    {
        m_flags = static_cast<unsigned long *>(storage);
        m_count = bit_count;
    }
    __forceinline void set_bits(c_basic_buffer<void> buffer, unsigned long bit_count);

    __forceinline bool test(long index) const
    {
        return (m_flags[index >> 5] & (1 << (index & 31))) != 0;
    }
    __forceinline void set(long index, bool value)
    {
        unsigned long mask = 1 << (index & 31);
        unsigned long *words = m_flags;
        value ? (words[index >> 5] |= mask) : (words[index >> 5] &= ~mask);
    }
    __forceinline void clear() { memset(m_flags, 0, (((unsigned long)m_count + 31) >> 3) & 0x1ffffffc); }

    __forceinline void set_bits(long index, unsigned long mask)
    {
        unsigned long *bits = m_flags;
        bits[index >> 5] |= mask;
    }

    union { unsigned long *m_flags; unsigned long *m_storage; };
    long m_count;
};

#endif // MEMORY_WRAPPED_FLAGS_H
