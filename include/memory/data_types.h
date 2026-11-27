#ifndef MEMORY_DATA_TYPES_H
#define MEMORY_DATA_TYPES_H

class c_allocation_base;

struct s_datum_header {
    short identifier;
};

struct s_data_array {
    char name[32];
    long maximum_count;
    union {
        long size;
        long element_size;
    };
    unsigned char alignment_bits;
    unsigned char valid;
    unsigned short flags;
    unsigned long signature;
    c_allocation_base *allocator;
    long next_index;
    long first_unallocated;
    long actual_count;
    short next_identifier;
    short isolated_next_identifier;
    char *data;
    char *in_use_bit_vector;
    unsigned int data_offset;
    unsigned int in_use_bit_vector_offset;
};

struct s_data_iterator {
    s_data_array *data;
    long index;
    long absolute_index;
};

// Canonical datum-handle decode: the low 16 bits select the absolute index.
#define DATUM_INDEX_TO_ABSOLUTE_INDEX(index) ((index) & 0xffff)

typedef char data_types_size_datum_header[(sizeof(s_datum_header) == 2) ? 1 : -1];
typedef char data_types_size_data_array[(sizeof(void *) != 4 || ((sizeof(s_data_array) == 84) )) ? 1 : -1];
typedef char data_types_size_data_iterator[(sizeof(void *) != 4 || ((sizeof(s_data_iterator) == 12) )) ? 1 : -1];

#endif // MEMORY_DATA_TYPES_H
