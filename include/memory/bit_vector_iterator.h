#ifndef MEMORY_BIT_VECTOR_ITERATOR_H
#define MEMORY_BIT_VECTOR_ITERATOR_H

class c_bit_vector_iterator {
public:
    short m_count;
    short m_index;
    long m_word;
    long *m_bits;

    bool next_index(long *out_index);
};

#endif
