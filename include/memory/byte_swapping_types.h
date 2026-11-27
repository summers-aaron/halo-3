#ifndef MEMORY_BYTE_SWAPPING_TYPES_H
#define MEMORY_BYTE_SWAPPING_TYPES_H

enum e_byte_swap_code
{
    _byte_swap_2byte = -2,
    _byte_swap_4byte = -4,
    _byte_swap_8byte = -8,
    _byte_swap_array_start = -100,
    _byte_swap_array_end = -101,
    _byte_swap_external = -102
};

struct element_type_t
{
    void *name;
    int size;
    void *definition;
};

typedef char byte_swapping_element_type_size[(sizeof(void *) != 4 || ((sizeof(element_type_t) == 12) )) ? 1 : -1];

#endif // MEMORY_BYTE_SWAPPING_TYPES_H
