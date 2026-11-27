#ifndef CACHE_CACHE_FILE_RESOURCE_ELEMENT_SET_H
#define CACHE_CACHE_FILE_RESOURCE_ELEMENT_SET_H

#include "memory/data_types.h"
#include "cache/cache_file_resource_span_table.h"

struct s_cache_file_resource_element_set
{
    s_cache_file_resource_fixed_array *first;
    s_data_array **second;
    s_data_array **third;

    __forceinline bool valid_first(long index) const { return index >= 0 && index < first->count; }
    __forceinline s_cache_file_tag_resource_element *get_first_element(long index) const { return first->elements + index; }
    __forceinline bool valid_second(long index) const { return index >= 0 && index < (*second)->actual_count; }
    __forceinline bool valid_third(long index) const { return index >= 0 && index < (*third)->actual_count; }
    __forceinline long get_maximum_count() const { return first->count + (*second)->maximum_count + (*third)->maximum_count; }
    __forceinline void initialize(s_cache_file_resource_fixed_array *fixed_array, s_data_array **first_data_array,
        s_data_array **second_data_array)
    {
        first = fixed_array;
        second = first_data_array;
        third = second_data_array;
    }
};

#endif // CACHE_CACHE_FILE_RESOURCE_ELEMENT_SET_H
