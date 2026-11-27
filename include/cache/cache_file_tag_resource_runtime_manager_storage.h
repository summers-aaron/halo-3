#ifndef CACHE_CACHE_FILE_TAG_RESOURCE_RUNTIME_MANAGER_STORAGE_H
#define CACHE_CACHE_FILE_TAG_RESOURCE_RUNTIME_MANAGER_STORAGE_H

#include "cache/cache_file_tag_resource_runtime_interface.h"
#include "cache/cache_file_tag_resource_runtime_window_allocator_storage.h"
#include "cache/indirect_cache_file_reader_types.h"
#include "cache/tag_resource_cache_file_datum_handler.h"
#include "tag_files/tag_types.h"

struct cache_file_tag_group;
struct global_tag_index;

// The runtime manager's tag index. The initialize_for_new_map API uses this type.
struct tag_header {
    long group_count;
    TAG_POINTER(cache_file_tag_group) groups;
    long instance_count;
    TAG_POINTER(const cache_file_tag_instance) instances;
    long count;
    TAG_POINTER(global_tag_index) tags;
};

struct s_cache_file_resource_gestalt;
struct s_cache_file_tag_resource_external_access_flush_view;
struct s_cache_file_tag_resource_graph_visitor;
struct s_cache_file_tag_resource_graph_interface
{
    virtual bool visit_tag_resources(long tag_index, void *context, s_cache_file_tag_resource_graph_visitor *visitor) = 0;
};


struct s_cache_file_tag_resource_thread_access_arrays
{
    template<typename t_element>
    struct __declspec(align(8)) s_thread_view
    {
        long count;
        t_element *data;
    };

    struct s_thread_views
    {
        s_thread_view<long> acquired;
        s_thread_view<c_tag_resource_cache_file_access_cache *> caches;

        __forceinline s_thread_views &operator=(const s_thread_views &other)
        {
            acquired = other.acquired;
            caches = other.caches;
            return *this;
        }
    };

    __forceinline s_cache_file_tag_resource_thread_access_arrays()
    {
        m_views.acquired.data = NULL;
        m_views.acquired.count = 0;
        m_views.caches.data = NULL;
        m_views.caches.count = 0;
    }

    // cache_initialize binds the two nine-thread arrays.
    long m_acquired_access_storage[9];
    c_tag_resource_cache_file_access_cache *m_access_cache_storage[9];
    s_thread_views m_views;
};

// The +0x320 placement and 0x80 stride require 32-byte alignment.
class __declspec(align(32)) c_cache_file_tag_resource_runtime_thread_access_storage :
    public s_cache_file_tag_resource_thread_access_arrays
{
public:
    c_read_write_lock m_access_lock;
    c_tag_resource_cache_thread_lock_lock_freeish *m_resource_thread_access_lock;
};
struct s_cache_file_tag_resource_runtime_cache_storage
{
    __forceinline c_tag_resource_cache_new *get_resource_cache() { return &m_cache; }
#if defined(_XBOX) && !defined(__clang__)
    __forceinline void initialize_for_new_map(long resource_count, long maximum_page_count,
        c_tag_resource_runtime_listener *runtime_listener, c_tag_resource_cache_file_reader *file_reader,
        c_tag_resource_cache_file_datum_handler *datum_handler, c_cache_file_tag_resource_runtime_window_allocator_storage *page_allocator,
        c_tag_resource_runtime_active_set *first_interface, c_tag_resource_paging_page_filter *second_interface,
        c_allocation_base *allocator, c_tag_resource_page_range_allocator *range_allocator);
#else
    void initialize_for_new_map(long resource_count, long maximum_page_count,
        c_cache_file_tag_resource_runtime_manager_storage *manager, c_allocation_base *allocator);
#endif
    c_tag_resource_cache_new m_cache;
    c_cache_file_tag_resource_runtime_thread_access_storage m_render_access;
};
struct s_cache_file_tag_resource_runtime_shared_file_arrays
{
    __forceinline void initialize(c_wrapped_array<const s_cache_file_tag_resource_cursor_source_slot> files,
        c_wrapped_array<const long> pages, c_wrapped_array<const long> requests)
    {
        m_files = files;
        m_pages = pages;
        m_requests = requests;
    }

    c_wrapped_array<const s_cache_file_tag_resource_cursor_source_slot> m_files;
    c_wrapped_array<const long> m_pages;
    c_wrapped_array<const long> m_requests;
};
class c_cache_file_tag_resource_runtime_manager_storage :
    public c_tag_resource_runtime_listener,
    public c_tag_resource_runtime_active_set,
    public c_tag_resource_paging_page_filter,
    public c_tag_resource_page_range_allocator,
    public c_tag_resource_cache_file_reader,
    public c_indirect_cache_file_location_atlas,
    public c_physical_memory_contiguous_region_listener,
    public s_cache_file_tag_resource_graph_interface,
    public c_cache_file_tag_resource_window_allocator
{
public:
    c_cache_file_tag_resource_runtime_manager_storage();

    __forceinline void initialize_for_new_map(tag_header *header, long game_mode, long cache_file_resource_gestalt_index,
        c_wrapped_array<const s_cache_file_tag_resource_vtable *> resource_vtables, s_cache_file_tag_resource_codec_resolver *codec_resolver);
    template <typename t_layout_table_owner>
    __forceinline void initialize_layout_table(const t_layout_table_owner *owner, s_cache_file_tag_resource_codec_resolver *codec_resolver);
    __forceinline void initialize_external_access();
    __forceinline void bind_file_slots();
    __forceinline c_indirect_cache_file_location_atlas *get_location_atlas()
    {
        return this;
    }
    __forceinline const s_cache_file_resource_gestalt *get_gestalt() const
    {
        return static_cast<const s_cache_file_resource_gestalt *>(m_header->instances[(short)m_cache_file_resource_gestalt_index].definition);
    }
    void update_bulk_reader();
    unsigned char submit_resource_read(long index, unsigned char high_priority, long offset, long size,
        c_basic_buffer<void> buffer, c_asynchronous_io_marker *marker);

    bool register_resource(long owner, long resource_type, void *resource) override;
    void unregister_resource(long owner, long resource_type, void *resource) override;
#if defined(_XBOX) && !defined(__clang__)
    bool any_resources_active() const override { return true; }
#else
    bool is_loaded() override { return true; }
#endif
    bool is_resource_required(long owner, long resource_handle) const override;
    bool is_resource_deferred(long owner, long resource_handle) const override { return false; }
    bool is_resource_unneeded(long owner, long resource_handle) const override;

    bool page_is_necessary(long handle) override;

    bool set_window(c_basic_buffer<void> window, c_basic_buffer<void> *out_buffer) override;
    bool resize(c_basic_buffer<void> *buffer, unsigned long minimum_size, unsigned long requested_size, long flags) override;
    void allocate(c_basic_buffer<void> *buffer, unsigned long size) override;
    void get_window(s_cache_file_tag_resource_window *out) override;
    void reset_window(s_cache_file_tag_resource_window *out) override;
    void set_locked_window(c_basic_buffer<void> window) override;
    void lock_window() override;

    void update_io_state() override;
    void wait_for_io() override;
    bool request_data(c_indirect_cache_file_bulk_read_iterator *iterator) override;
    bool request_data_async(const s_indirect_cache_file_read_request *request, c_synchronized_long *bytes_read, c_asynchronous_io_marker *marker) override;

    bool get_location(unsigned __int64 position, s_indirect_cache_file_location *location) override;

    void initialize_resize_buffer(c_basic_buffer<void>) override {}
    void resize_no_fail(c_basic_buffer<void> old_buffer, c_basic_buffer<void> new_buffer) override;
    void dispose_resize_buffer(c_basic_buffer<void>) override {}

    bool visit_tag_resources(long tag_index, void *context, s_cache_file_tag_resource_graph_visitor *visitor) override;

    void *allocate_lruv(unsigned long size) override;
    void *allocate(unsigned long size) override;
    void advance_window(int unused, long size) override;

    tag_header *m_header;
    long m_cache_file_resource_gestalt_index;
    s_cache_file_tag_resource_zone_state m_zone_state;
    s_cache_file_tag_resource_zone_names m_zone_names;
    bool m_state_valid;
    bool m_required_resource_state_changed;
    bool m_active_resource_state_changed;
    bool m_active_zone_changed;
    struct s_listener_link
    {
        __forceinline s_listener_link(c_cache_file_tag_resource_runtime_listener_storage *listener) :
            m_address(NULL), m_size(0), m_flush_view(NULL), m_listener(listener)
        {
        }

        void *m_address;
        long m_size;
        s_cache_file_tag_resource_external_access_flush_view *m_flush_view;
        c_cache_file_tag_resource_runtime_listener_storage *m_listener;
    };
    s_listener_link m_listener_link;
    s_tag_resource_cache_external_access_hash_table m_external_access;
    s_cache_file_tag_resource_runtime_resource_arrays m_resources;
    unsigned long m_required_resource_flags[1024];
    unsigned long m_active_resource_flags[1024];
    unsigned long m_active_tag_flags[625];
    const s_cache_file_tag_resource_vtable *m_resource_vtables[16];
    c_wrapped_array<s_cache_file_resource_runtime_data_new> m_resource_runtime_data;
    c_cache_file_tag_resource_runtime_buffer m_interop_buffer;
    c_cache_file_tag_resource_runtime_manager::c_cache_file_tag_resource_datum_handler m_datum_handler;
    c_cache_file_tag_resource_runtime_window_allocator_storage m_window_allocator;
    c_cache_file_tag_resource_runtime_buffer m_buffer;
    union {
        struct {
            c_basic_buffer<void> m_requested_buffer;
            c_basic_buffer<void> m_result_buffer;
        };
        struct {
            s_cache_file_tag_resource_window m_requested_window;
            s_cache_file_tag_resource_window m_result_window;
        };
    };
    c_cache_file_tag_resource_runtime_buffer m_window;
    bool m_map_initialized;
    c_cache_file_tag_resource_runtime_buffer m_locked_window;
    s_cache_file_tag_resource_runtime_cache_storage m_resource_cache;
    c_cache_file_tag_resource_runtime_listener_storage m_listener_storage;
    c_cache_file_tag_resource_runtime_buffer m_region;
    s_cache_file_runtime_handle_stack m_handles;
    s_data_array *m_shared_file_handles;
    s_cache_file_tag_resource_runtime_shared_file_arrays m_shared_files;
    s_data_array *m_runtime_locations;
    s_data_array *m_pending_locations;
    s_cache_file_resource_element_set m_element_set;
    long m_previous_game_mode;
    s_cache_file_tag_resource_named_value m_first_cursor;
    s_cache_file_tag_resource_named_value m_second_cursor;
    s_cache_file_tag_resource_relocation_list m_relocations;
    s_cache_file_tag_resource_page_state m_page_storage;
    s_cache_file_tag_resource_page_request m_page_cursor;
    s_cache_file_tag_resource_slots m_file_slots;
    long m_game_mode;
    bool m_streaming_enabled;
    bool m_locked;
};

// The geometry runtime initializes the manager with the storage class.
__forceinline c_cache_file_tag_resource_runtime_manager_storage *get_geometry_runtime_manager()
{
    return g_geometry_globals.storage;
}

#endif // CACHE_CACHE_FILE_TAG_RESOURCE_RUNTIME_MANAGER_STORAGE_H
