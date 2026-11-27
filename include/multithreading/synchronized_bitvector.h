#ifndef MULTITHREADING_SYNCHRONIZED_BITVECTOR_H
#define MULTITHREADING_SYNCHRONIZED_BITVECTOR_H

#include "multithreading/synchronized_value.h"

template<unsigned int count> class c_synchronized_array {
    c_synchronized_long values[count];
public:
    __forceinline c_synchronized_long *get_member(long index) { return &values[index]; }
    __forceinline void set_array(long value) { for (unsigned int index = 0; index < count; index++) values[index].set(value); }
};
template<long k_bit_count> class c_synchronized_bitvector {
public:
    void set_bit(long index, bool set);
    __forceinline void set_array(long value) { m_bits.set_array(value); }
    bool test_bit(long index)
    {
        long word_index = index >> 5;
        long word = m_bits.get_member(word_index)->peek();
        long bit = 1 << (index & 31);
        return (word & bit) != 0;
    }
private:
    c_synchronized_array<(k_bit_count + 31) / 32> m_bits;
};
#endif // MULTITHREADING_SYNCHRONIZED_BITVECTOR_H
