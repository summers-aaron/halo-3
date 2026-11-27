#ifndef MEMORY_BYTE_SWAPPING_H
#define MEMORY_BYTE_SWAPPING_H

// Functions defined in tu/blamlibxbox/byte_swapping.cpp that other units call.

struct element_type_t;

extern "C" void byte_swap_element(element_type_t *type, char *data, void *definition, int *out_size, int *out_count);
extern "C" void byte_swap_array(element_type_t *type, char *data, int count);

#endif // MEMORY_BYTE_SWAPPING_H
