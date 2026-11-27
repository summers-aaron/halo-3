#ifndef TAG_FILES_TAG_RESOURCE_CACHE_EXTERNAL_ACCESS_H
#define TAG_FILES_TAG_RESOURCE_CACHE_EXTERNAL_ACCESS_H

// Functions defined in tu/blamlibxbox/tag_resource_cache_external_access.cpp.



#include "cache/tag_resource_cache_file_datum_handler.h"
class c_tag_resource_cache_new;
class c_tag_resource_error_receiver;

extern "C" void tag_resource_cache_new_release_page(c_tag_resource_cache_new *self, long page_handle);
extern "C" void tag_resource_cache_new_handle_page_move(c_tag_resource_cache_new *self, long page_handle);
extern "C" void tag_resource_cache_new_commit_dirty(c_tag_resource_cache_new *cache);
extern "C" void tag_resource_cache_new_update_zones(c_tag_resource_cache_new *cache, long unused, bool synchronous,
    c_tag_resource_error_receiver *flag_receiver);
extern "C" void tag_resource_cache_new_update(c_tag_resource_cache_new *cache);

struct s_tag_resource_cache_group_hash_table;
struct s_ranges;
struct s_tag_resource_cache_external_access_bit_iterator;
struct s_tag_resource_cache_external_access_bit_source;
struct s_tag_resource_cache_external_access_state;
class c_tag_resource_cache_controller;
typedef c_tag_resource_cache_new tag_resource_external_access_set_object;
typedef s_tag_resource_cache_external_access_state tag_resource_external_access_table;

extern "C" void tag_resource_cache_track_element(tag_resource_external_access_set_object *o, int idx, int n);
extern "C" unsigned char tag_resource_cache_find_next_mismatch(s_tag_resource_cache_external_access_bit_iterator *iterator,
    s_tag_resource_cache_external_access_bit_source *source,
    long *bit_index);
extern "C" long tag_resource_cache_get_or_add_slot(s_tag_resource_cache_external_access_state *state, long first, long second, long third);
extern "C" bool tag_resource_cache_find_group_handle(s_tag_resource_cache_group_hash_table *table, int key, int *output);
extern "C" void tag_resource_cache_find_or_add_key(tag_resource_external_access_table *table, long index, unsigned short *counter, long value, long flag);
extern "C" long tag_resource_cache_new_group_entry(s_tag_resource_cache_group_hash_table *table, long key, long key1, long key2);
extern "C" long tag_resource_cache_find_or_add_value(s_tag_resource_cache_external_access_state *state, long index, unsigned short *count_ptr, long value);
extern "C" void tag_resource_cache_find_duplicate_slot(s_tag_resource_cache_external_access_state *state, long index);
extern "C" long tag_resource_cache_find_duplicate_group(s_tag_resource_cache_external_access_state *state, long index);
extern "C" void tag_resource_cache_new_update_pages(c_tag_resource_cache_new *cache, bool force);
extern "C" void tag_resource_cache_get_or_add_group(s_tag_resource_cache_external_access_state *state, long slot_index, long key);
struct s_bit_vector;

#ifdef HALO3_64BIT
extern "C" unsigned char tag_resource_cache_set_bits_from_set(s_ranges *ranges, long index, s_bit_vector first, s_bit_vector second);
#else
extern "C" unsigned char tag_resource_cache_set_bits_from_set(s_ranges *ranges, long index, __int64 first, __int64 second);
#endif

extern "C" void tag_resource_cache_set_bits_from_range(s_ranges *object, int index, s_bit_vector storage);
struct s_tag_resource_cache_external_access_hash_table;
class c_tag_resource_runtime_listener;
class c_tag_resource_cache_file_reader;
class c_tag_resource_cache_file_datum_handler;
class c_tag_resource_page_allocator;
class c_tag_resource_page_range_allocator;
class c_allocation_base;
extern "C" inline long tag_resource_cache_hash_find(s_tag_resource_cache_external_access_hash_table *table, long first, long second, long third);
extern "C" inline long tag_resource_cache_hash_insert(s_tag_resource_cache_external_access_hash_table *table, long first, long second, long third);

class c_tag_resource_page_table;
class c_tag_resource_controller_page_table_io_listener;

extern "C" bool tag_resource_page_table_read_required_pages(c_tag_resource_page_table *table,
    c_tag_resource_controller_page_table_io_listener *listener, bool include_reading, bool synchronous,
    c_tag_resource_error_receiver *flag_receiver);

#endif // TAG_FILES_TAG_RESOURCE_CACHE_EXTERNAL_ACCESS_H
