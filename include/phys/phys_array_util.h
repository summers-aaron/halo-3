#ifndef INCLUDE_PHYS_ARRAY_UTIL_H
#define INCLUDE_PHYS_ARRAY_UTIL_H

namespace phys_array_util {
    void reserve_array_storage(void *array, int array_elem_count, int elem_size);
    void grow_array_storage(void *array, int elem_size);
    void shrink_array_storage(void *array, int elem_size, char *inline_mem, int array_requested_capacity);
}

#endif
