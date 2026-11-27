#ifndef CACHE_CACHE_FILE_RESOURCE_SPAN_TABLE_H
#define CACHE_CACHE_FILE_RESOURCE_SPAN_TABLE_H

#include "cseries/cseries.h"
#include "memory/wrapped_array.h"
#include "tag_files/tag_types.h"
#include "cache/cache_file_tag_resource_element_types.h"

struct s_cache_file_resource_handle_pair
{
    short first;
    short second;
    long first_fixup_value;
    long second_fixup_value;
    short first_shared_page;
    short second_shared_page;
};
struct s_cache_file_span
{
    unsigned long start;
    unsigned long size;
};
// A shared page: its total size (the runtime address once the page is resident) and its 16-byte subpage table.
struct s_cache_file_resource_shared_page
{
    long size;
    c_typed_tag_block<s_cache_file_span> subpages;
};
struct s_cache_file_resource_fixed_array
{
    c_wrapped_array<s_cache_file_resource_shared_page> shared_pages;
    long count;
    s_cache_file_tag_resource_element *elements;
    long m_handle_count;
    s_cache_file_resource_handle_pair *handles;

    unsigned char find_direct(long index, unsigned long offset, long unused, unsigned char *alternate, unsigned long *result);
    unsigned char find_with_offset(long index, unsigned long offset, long unused, unsigned char *alternate, unsigned long *result);
    bool find_span(long list_index, unsigned long offset, long unused, unsigned long *result);
};
STATIC_ASSERT_ABI32(sizeof(s_cache_file_resource_fixed_array) == 0x18, cache_file_resource_fixed_array_size);
STATIC_ASSERT_ABI32(offsetof(s_cache_file_resource_fixed_array, elements) == 0xc, cache_file_resource_elements_offset);

#endif
