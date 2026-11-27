#ifndef CACHE_CACHE_FILE_TAG_RESOURCE_RUNTIME_TYPES_H
#define CACHE_CACHE_FILE_TAG_RESOURCE_RUNTIME_TYPES_H

#include "cache/cache_file_tag_resource_runtime.h"
#include "cseries/cseries.h"
#include "multithreading/synchronized_value.h"

#include "memory/wrapped_flags.h"
#include "cache/tag_resource_cache_address_cache.h"

#include "cache/tag_resource_cache_thread_lock.h"





extern "C" void tag_resource_cache_initialize_hash_table(s_tag_resource_cache_external_access_hash_table *state,
    c_wrapped_array<const long> list, c_wrapped_array<const long> a,
    c_wrapped_array<const long> b, c_wrapped_array<const long> c, c_wrapped_array<const long> d);
#if defined(_XBOX) && !defined(__clang__)
class c_tag_resource_runtime_listener;
class c_tag_resource_runtime_active_set;
class c_tag_resource_paging_page_filter;
class c_tag_resource_page_range_allocator;
class c_tag_resource_cache_file_reader;
extern "C" long tag_resource_cache_new_initialize(c_tag_resource_cache_new *cache, long resource_count, long maximum_page_count, long unknown,
    c_tag_resource_runtime_listener *runtime_listener, c_tag_resource_cache_file_reader *file_reader,
    c_tag_resource_cache_file_datum_handler *datum_handler, c_cache_file_tag_resource_runtime_window_allocator_storage *page_allocator,
    c_tag_resource_runtime_active_set *first_interface, c_tag_resource_paging_page_filter *second_interface,
    c_allocation_base *allocator, c_tag_resource_page_range_allocator *range_allocator);
#else
class c_tag_resource_runtime_listener;
class c_tag_resource_cache_file_reader;
class c_tag_resource_cache_file_datum_handler;
class c_tag_resource_page_allocator;
class c_tag_resource_cache_new_loaded_query;
class c_tag_resource_paging_page_filter;
class c_tag_resource_page_range_allocator;

extern "C" long tag_resource_cache_new_initialize(c_tag_resource_cache_new *cache, unsigned long resource_count,
    unsigned long maximum_page_count, long unknown, c_tag_resource_runtime_listener *runtime_listener,
    c_tag_resource_cache_file_reader *file_reader, c_tag_resource_cache_file_datum_handler *datum_handler,
    c_tag_resource_page_allocator *page_allocator, c_tag_resource_cache_new_loaded_query *loaded_query,
    c_tag_resource_paging_page_filter *page_filter, c_allocation_base *allocator, c_tag_resource_page_range_allocator *range_allocator);
#endif

#endif // CACHE_CACHE_FILE_TAG_RESOURCE_RUNTIME_TYPES_H
