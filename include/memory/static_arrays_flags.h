#ifndef MEMORY_STATIC_ARRAYS_FLAGS_H
#define MEMORY_STATIC_ARRAYS_FLAGS_H

// c_static_flags_no_init, static_arrays.h:1486-1785 (memory/static_arrays.h includes this file). It is split out so
// object_datum.h (type_part_flags) can hold one without including static_arrays.h, which collides with the private
// c_static_array of units not yet cleaned (game_engine_king.cpp). Merge it back once those copies are gone.
// XMemSet (clear_range) comes from cseries/crt_xdk.h, as it did inside static_arrays.h.

#include <string.h>

template<long k_maximum_count>
class c_static_flags_no_init {
public:
    static const long MAX_COUNT = k_maximum_count;

    // March [1486]: ASSERT(IN_RANGE_INCLUSIVE(count, 0, k_maximum_count)); csmemset of the words covering count.
    __forceinline void fill(long count, unsigned char value) {
        memset(m_storage, value, ((count + 31) >> 5) * sizeof(unsigned long));
    }
    // March [1497]: ASSERT(VALID_INDEX(index, k_maximum_count)); BIT_VECTOR_SET_FLAG (a ?: expression).
    __forceinline void set(long index, bool enable) {
        enable ? (m_storage[index >> 5] |= (1 << (index & 31))) : (m_storage[index >> 5] &= ~(1 << (index & 31)));
    }
    // March [1507]: ASSERT(VALID_INDEX(index, k_maximum_count)); BIT_VECTOR_TEST_FLAG.
    __forceinline bool test(long index) const {
        return (m_storage[index >> 5] & (1 << (index & 31))) != 0;
    }
    __forceinline void clear() { memset(m_storage, 0, sizeof(m_storage)); }
    __forceinline void set_all() { memset(m_storage, -1, sizeof(m_storage)); }
    // March [1625] clears whole words: csmemset(m_storage, 0, BIT_VECTOR_SIZE_IN_BYTES(count)).
    // Clears the first count bits, masking the unused bits of the partial last word.
    __forceinline void clear_range(long count) {
        if (count != 0) {
            long last_word = ((count + 31) >> 5) - 1;
            if (last_word != 0)
                XMemSet(m_storage, 0, last_word * sizeof(unsigned long));
            m_storage[last_word] &= ~static_cast<unsigned long>((1ui64 << (count - (last_word << 5))) - 1);
        }
    }
    // Sets the first count bits, handling a partial last word bit by bit.
    void set_range(long count);
    __forceinline const unsigned long *get_bits_direct() const { return m_storage; }
    __forceinline unsigned long *get_writeable_bits_direct() { return m_storage; }
    __forceinline bool is_clear() const {
        bool result = true;
        long word_count = (k_maximum_count + 31) >> 5;
        for (long index = 0; index < word_count; index++)
            result &= (m_storage[index] == 0);
        return result;
    }
    // Not used by any unit yet; March bodies (assert lines in brackets):
    //   count_bits_set: bit_vector_count_bits(m_storage, k_maximum_count)
    //   copy: csmemcpy(m_storage, other, sizeof(m_storage))
    //   set_bits_direct_destructive [1531]: csmemcpy(m_storage, bits, BIT_VECTOR_SIZE_IN_BYTES(count))
    //   and_range [1561] / and_not_range [1576]: m_storage[i] = a[i] & b[i] / a[i] & ~b[i] for the words of count
    //   or_bits [1587]: m_storage[i] |= bits[i] for the words of count
    //   and [1644]: ASSERT(vector_a); m_storage[i] &= vector_a->m_storage[i] for every word
    //   keep_range [1710]: clear the words above count, mask the last one
    //   invert_bits: m_storage[i] = ~m_storage[i] for every word, then mask the last one
    //   highest_bit_set_in_range [1785]: bit_vector_highest_bit_set(m_storage, count)
    long count_bits_set() const;
    void copy(const c_static_flags_no_init<k_maximum_count> *other);
    void set_bits_direct_destructive(long count, const unsigned long *bits);
    void and_range(const c_static_flags_no_init<k_maximum_count> *a, const c_static_flags_no_init<k_maximum_count> *b, long count);
    void and_not_range(const c_static_flags_no_init<k_maximum_count> *a, const c_static_flags_no_init<k_maximum_count> *b, long count);
    void or_bits(const unsigned long *bits, long count);
    // void and(const c_static_flags_no_init<k_maximum_count> *vector_a); (`and` is an ISO 646 token to clang)
    void keep_range(long count);
    void invert_bits();
    long highest_bit_set_in_range(long count) const;

    // The member name is not attested (March has no data symbol for it).
    unsigned long m_storage[(k_maximum_count + 31) / 32];
};

#endif // MEMORY_STATIC_ARRAYS_FLAGS_H
