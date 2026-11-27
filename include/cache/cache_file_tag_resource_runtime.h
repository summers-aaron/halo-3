#ifndef CACHE_CACHE_FILE_TAG_RESOURCE_RUNTIME_H
#define CACHE_CACHE_FILE_TAG_RESOURCE_RUNTIME_H

// Functions defined in tu/blamlibxbox/cache_file_tag_resource_runtime.cpp.

struct s_cache_file_resource_gestalt;
class c_tag_resource_cache_file_datum_setup_iterator
{
public:
    virtual bool next_tracked_resource_handle(long *out_resource_handle) = 0;
};

class c_cache_file_tag_resource_setup_iterator : public c_tag_resource_cache_file_datum_setup_iterator
{
public:
    __forceinline c_cache_file_tag_resource_setup_iterator(s_cache_file_resource_gestalt *owner, bool all) :
        m_owner(owner), m_index(0), m_all(all) {}
    virtual bool next_tracked_resource_handle(long *out_resource_handle);

    s_cache_file_resource_gestalt *m_owner;
    unsigned long m_index;
    bool m_all;
};

struct s_cache_file_resource_element_set;
struct s_cache_file_tag_resource_set_iterator
{
    __forceinline s_cache_file_tag_resource_set_iterator(s_cache_file_resource_element_set *set, bool streaming) :
        m_set(set), m_streaming(streaming), m_index(0) {}
    virtual bool next_tracked_page_handle(long *out_handle, long *out_first_index);

    s_cache_file_resource_element_set *m_set;
    bool m_streaming;
    long m_index;
};

struct s_tag_resource;

bool scenario_active_zone_is_tag_available(long index);
void *tag_resource_available(const s_tag_resource *resource);
long tag_resources_lock_game(void);
void tag_resources_unlock_game(long &locked);

class c_asynchronous_io_arena;
class c_cache_file_tag_resource_runtime_manager;
class c_cache_file_tag_resource_runtime_manager_storage;
class c_allocation_base;
class c_tag_resource_cache_file_access_cache;
class c_tag_resource_cache_thread_lock_lock_freeish;
struct s_cache_file_resource_element_set;

extern "C" void cache_file_tag_resource_compute_required_flags(
    c_cache_file_tag_resource_runtime_manager *runtime, void *resource_bits, void *tag_bits);
extern "C" void cache_file_tag_resource_compute_zone_flags(
    c_cache_file_tag_resource_runtime_manager *runtime, void *resource_bits);
extern "C" void cache_file_tag_resource_compute_scenario_flags(
    c_cache_file_tag_resource_runtime_manager_storage *runtime, unsigned long mask0, unsigned long mask1,
    unsigned long mask2, unsigned long mask3, void *buffer2);
extern "C" void tag_resources_lock_render_access(void);
extern "C" void cache_file_tag_resource_runtime_initialize(c_cache_file_tag_resource_runtime_manager_storage *storage, c_allocation_base *allocator);
struct s_geometry_globals;
extern "C" int cache_file_tag_resource_runtime_dispose(s_geometry_globals *globals);
extern "C" long cache_file_resource_get_first_handle_datum(const s_cache_file_resource_element_set *set, long index);
extern "C" c_tag_resource_cache_file_access_cache *tag_resource_cache_thread_lock_lock_freeish_acquire(c_tag_resource_cache_thread_lock_lock_freeish *lock);
extern "C" long tag_resource_fixed_array_get_second_handle(const s_cache_file_resource_element_set *set, long index);
extern "C" bool asynchronous_io_arena_open_file(c_asynchronous_io_arena *arena, long file_index, long flags, const char *file_name);

struct s_cache_file_bit_vector_target;
struct s_cache_file_tag_resource_slots;

void scenario_handle_tag_resource_lock(long thread_index);
extern "C" void cache_file_tag_resource_requests_try_and_load(s_cache_file_tag_resource_slots *slots, int request_index);
extern "C" s_cache_file_bit_vector_target *cache_file_tag_resource_bit_vector_target_initialize(
    s_cache_file_bit_vector_target *scratch, long resource_count, void *resource_bits,
    long tag_count, void *tag_bits, unsigned char include_streamed_resources);

struct s_cache_file_runtime_handle_set;
struct s_cache_file_resource_shared_file;
struct s_tag_block;

extern "C" void cache_file_tag_resource_definition_initialize(s_cache_file_runtime_handle_set *set, long index,
    const char *path, const s_cache_file_resource_shared_file *shared_file, bool pending);
extern "C" void cache_file_tag_resource_update_masked_bits(s_cache_file_bit_vector_target *target, const s_tag_block *list, unsigned long mask);
extern "C" void cache_file_tag_resource_update_all_bits(s_cache_file_bit_vector_target *target, const s_tag_block *list);

// Runtime resource allocation, readers, sorting, and shared storage.
class c_allocation_base;
template <typename t_type> class c_basic_buffer;
class c_cache_file_tag_resource_page_demand;
class c_tag_resource_cache_file_datum_handler;
class c_cache_file_tag_resource_runtime_window_allocator_storage;
class c_indirect_cache_file_bulk_read_iterator;
class c_indirect_cache_file_location_atlas;
class c_physical_memory_allocation;
class c_sound_cache_hash_source;
class c_tag_resource_cache_new;
class c_tag_resource_controller_page_table_io_listener;
class c_tag_resource_page_table;
template <typename t_type> class c_wrapped_array;
struct s_tag_resource_interop_type;
struct s_cache_file_tag_resource_page_request;
struct s_cache_file_tag_resource_page_state;
struct s_cache_file_tag_resource_cursor;
struct s_cache_file_tag_resource_cursor_source;
struct s_cache_file_tag_resource_external_access_flush_view;
struct s_tag_resource_cache_external_access_hash_table;
struct s_cache_file_tag_resource_file_globals;
struct s_cache_file_tag_resource_page_reader;
struct s_cache_file_tag_resource_physical_globals;
struct s_cache_file_tag_resource_relocation_list;
struct s_cache_file_tag_resource_sort_key_predicate;
struct s_cache_file_tag_resource_sort_predicate;
struct s_geometry_globals;
struct s_sound_cache_hash_state;
struct s_tag_persistent_identifier;

// Maps each codec identifier of a resource layout table to its handler result.
struct s_cache_file_tag_resource_codec_resolver
{
    virtual void *resolve(const s_tag_persistent_identifier *codec) = 0;
};

// The resolver main_game_initialize_map_resources hands to initialize_for_new_map:
// runs every k_tag_resource_handler_table entry whose key equals the codec.
struct s_cache_file_tag_resource_handler_table_resolver : s_cache_file_tag_resource_codec_resolver
{
    virtual void *resolve(const s_tag_persistent_identifier *codec);
};
struct s_xenon_game_state_globals;

#include "cache/cache_file_tag_resource_sort.h"
extern "C" void cache_file_tag_resource_sort_push_heap_by_key(s_cache_file_tag_resource_relocation *first, long hole, long top,
    s_cache_file_tag_resource_relocation value, s_cache_file_tag_resource_sort_key_predicate predicate);
extern "C" void cache_file_tag_resource_sort_rotate(s_cache_file_tag_resource_relocation *first, s_cache_file_tag_resource_relocation *mid,
    s_cache_file_tag_resource_relocation *last, long *distance_type, s_cache_file_tag_resource_relocation *value_type);
extern "C" void cache_file_tag_resource_sort_heap_by_key(s_cache_file_tag_resource_relocation *first,
    s_cache_file_tag_resource_relocation *last, s_cache_file_tag_resource_sort_key_predicate predicate);
extern "C" void cache_file_tag_resource_sort_adjust_heap_by_key(s_cache_file_tag_resource_relocation *first, long hole, long bottom,
    s_cache_file_tag_resource_relocation value, s_cache_file_tag_resource_sort_key_predicate predicate);
extern "C" void cache_file_tag_resource_update_relocation_window(
    s_cache_file_tag_resource_relocation_list *list, c_basic_buffer<void> old_buffer, c_basic_buffer<void> buffer);
extern "C" void tag_resource_page_table_reset_page(c_tag_resource_page_table *table, long handle);
extern "C" bool cached_map_file_exists(const char *path);
extern "C" bool tag_resource_page_table_is_idle(c_tag_resource_page_table *page_table);
extern "C" void cache_file_bulk_reader_initialize_hash_state(s_sound_cache_hash_state *state, unsigned long read_size,
    c_basic_buffer<void> buffer, c_sound_cache_hash_source *source, bool hash_enabled);
extern "C" long indirect_cache_file_read_blocking(c_asynchronous_io_arena *arena, c_indirect_cache_file_location_atlas *atlas,
    c_indirect_cache_file_bulk_read_iterator *iterator, const void *request, unsigned long offset,
    unsigned long limit, const c_basic_buffer<void> *override_buffer, unsigned long *out_size, unsigned long *total_bytes);
extern "C" void tag_resource_cache_update_resources_from_active_set(void *state, void *filter, void *state2, unsigned char *changed);
extern "C" void main_update_idle_tasks(void);
extern "C" void tag_resource_cache_external_access_track_resource(c_tag_resource_cache_new *cache, long resource, long unknown);
extern "C" bool tag_resource_page_table_read_pages(c_tag_resource_page_table *table, s_cache_file_tag_resource_page_reader *reader,
    c_tag_resource_controller_page_table_io_listener *listener);
extern "C" { extern const s_tag_resource_interop_type *k_tag_resource_interop_types[]; }
struct s_cache_file_tag_resource_cursor_storage;
extern "C" s_cache_file_tag_resource_cursor *cache_file_tag_resource_cursor_new(
    s_cache_file_tag_resource_cursor_storage *storage, const s_cache_file_tag_resource_cursor_source *source);
#if defined(_XBOX) && !defined(__clang__)
#endif
#include "memory/shared_resource_allocator.h"
struct s_tag_resource_handler_entry
{
    const s_tag_persistent_identifier *key;
    void *(*get_context)(void);
};
extern "C" { extern const s_tag_resource_handler_entry k_tag_resource_handler_table[2]; }
extern "C" { extern c_asynchronous_io_arena g_asynchronous_io_arena; }

// Runtime cache construction and shared state.
struct s_cache_file_tag_resource_runtime_cache_storage;
extern "C" void cache_file_tag_resource_cache_initialize(s_cache_file_tag_resource_runtime_cache_storage *storage);

struct s_resource_access_cookie;
s_resource_access_cookie *tag_resources_unlock_render_access(void);

#endif // CACHE_CACHE_FILE_TAG_RESOURCE_RUNTIME_H
