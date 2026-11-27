#ifndef CACHE_CACHE_FILE_TAG_RESOURCE_RUNTIME_INTERFACE_H
#define CACHE_CACHE_FILE_TAG_RESOURCE_RUNTIME_INTERFACE_H

#include <stddef.h>
#include "cseries/cseries.h"
#include "memory/wrapped_array.h"
#include "memory/basic_buffer.h"
#include "memory/data.h"
#include "memory/static_arrays.h"
#include "cache/tag_resource_external_access_state.h"
#include "tag_files/tag_resource_cache_external_access.h"
#include "tag_files/tag_resource_cache_paging.h"
#include "cache/tag_resource_page_table.h"
#include "cache/cache_file_tag_resource_runtime_window_allocator_storage.h"
#include "cache/tag_resource_cache_new.h"
#include "cache/geometry_storage.h"
#include "cache/tag_resource_runtime_interfaces.h"
#include "cache/tag_resource_cache_file_datum_handler.h"
#include "cache/optional_cache.h"
#include "saved_games/game_state_procs_types.h"
#include "multithreading/synchronized_value.h"
#include "cache/asynchronous_io_marker_types.h"

class c_cache_file_tag_resource_runtime_listener_storage : public c_allocation_base
{
public:
    __forceinline c_cache_file_tag_resource_runtime_listener_storage() {}

    // 0x1aefe0: takes an aligned block off the top of the descending m_second cursor
    virtual void *allocate(unsigned int size, const char *name);
    virtual void deallocate(void *allocation) {}

    __forceinline void reset();
    __forceinline void initialize(c_basic_buffer<void> buffer);

    // m_first keeps the original allocation; m_second is the descending cursor and its used size.
    c_cache_file_tag_resource_runtime_buffer m_first;
    c_cache_file_tag_resource_runtime_buffer m_second;
};

struct tag_header;
struct s_cache_file_tag_resource_data;
struct s_tag_resource_definition;
struct s_cache_file_tag_resource_codec_resolver;
struct s_cache_file_resource_runtime_data_new;

// Shared runtime interface and resource request storage.
class c_cache_file_tag_resource_window_allocator;
struct s_cache_file_tag_resource_request
{
    int timestamp;
    signed char external;
    signed char valid;
    void *resource;
    int token;
    union
    {
        c_game_state_proc_listener *listener;
        c_optional_cache_user_callback *owner;
    };
};
struct s_cache_file_tag_resource_requests
{
    int current_timestamp;
    s_cache_file_tag_resource_request requests[6];
};

struct s_cache_file_tag_resource_slots : public c_game_state_proc_handler
{
    __forceinline s_cache_file_tag_resource_slots() {}
    virtual void set_mode(long mode, s_game_state_buffer_description *descriptions);
    virtual void reset();
    virtual void *game_state_proc_handler_allocate(long slot_index, long load_mode, long token, c_game_state_proc_listener *listener);
    void *try_and_get(int slot_index, int load_mode, int token, c_game_state_proc_listener *listener);
    void try_and_load(int slot_index);
    virtual void game_state_proc_handler_release(long slot_index, void *resource);
    bool m_reclaim_expired;
    s_cache_file_tag_resource_requests request_data;
    unsigned int bitmap[1];
    c_cache_file_tag_resource_window_allocator *loader;
};
STATIC_ASSERT_ABI32(sizeof(s_cache_file_tag_resource_request) == 0x14, resource_request_size);
STATIC_ASSERT(offsetof(s_cache_file_tag_resource_request, resource) == 8, resource_request_address_offset);
STATIC_ASSERT_ABI32(offsetof(s_cache_file_tag_resource_request, owner) == 0x10, resource_request_owner_offset);
STATIC_ASSERT_ABI32(sizeof(s_cache_file_tag_resource_slots) == 0x8c, resource_slots_size);
STATIC_ASSERT_ABI32(offsetof(s_cache_file_tag_resource_slots, m_reclaim_expired) == 4, resource_slots_reclaim_offset);
STATIC_ASSERT_ABI32(offsetof(s_cache_file_tag_resource_slots, request_data) == 8, resource_slots_requests_offset);
STATIC_ASSERT_ABI32(offsetof(s_cache_file_tag_resource_slots, bitmap) == 0x84, resource_slots_bitmap_offset);
STATIC_ASSERT_ABI32(offsetof(s_cache_file_tag_resource_slots, loader) == 0x88, resource_slots_loader_offset);



class c_cache_file_tag_resource_runtime_manager_storage;
class c_cache_file_tag_resource_window_allocator
{
public:
    virtual void *allocate_lruv(unsigned long size) = 0;
    virtual void *allocate(unsigned long size) = 0;
    virtual void advance_window(int unused, long size) = 0;
};

class c_cache_file_tag_resource_allocator
{
public:
    virtual void *allocate(unsigned int size, const char *name) = 0;
    virtual int deallocate(void *address) = 0;
};

class c_sound_cache_hash_source;
struct s_cache_file_tag_resource_reader;
struct s_cache_file_tag_resource_slot_source
{
    virtual bool is_empty() = 0;
    virtual c_sound_cache_hash_source *get_hash_source() = 0;
    virtual s_cache_file_tag_resource_reader *open(c_basic_buffer<void> buffer) = 0;
#if defined(_XBOX) && !defined(__clang__)
    // match: 0x1af3b0: the slot 3 call sets only r4 (source); two more int parameters add li r5,0 / li r6,0
    virtual void handle(c_sound_cache_hash_source *source) = 0;
#else
    // x86 thiscall: slot 3 is c_tag_resource_paging_task::reset_receiver(int, int, int), which pops three words
    virtual void handle(c_sound_cache_hash_source *source, int slot = 0, int value = 0) = 0;
#endif
};
struct s_cache_file_tag_resource_cursor;
struct s_cache_file_tag_resource_memory_stream;
struct s_tag_persistent_identifier;
struct s_cache_file_tag_resource_cursor_source_slot;
struct s_cache_file_tag_resource_cursor_storage
{
    __forceinline s_cache_file_tag_resource_cursor_storage() : object(0) {}
#ifdef HALO3_64BIT
    char memory[48];
#else
    char memory[24];
#endif
    s_cache_file_tag_resource_cursor *object;
};
struct s_cache_file_tag_resource_stream_storage
{
    __forceinline s_cache_file_tag_resource_stream_storage() : object(0) {}
#ifdef HALO3_64BIT
    char memory[32];
#else
    char memory[16];
#endif
    s_cache_file_tag_resource_memory_stream *object;
};
class c_cache_file_tag_resource_runtime_listener_storage;
// vtable 0x3bbd4: create (0x1af2b8) opens a reader over a buffer, release (0x1af3b0) drops the reader a slot handed out.
struct s_cache_file_tag_resource_page_state
{
    __forceinline s_cache_file_tag_resource_page_state() : source_count(0), buffer(), busy(false) {}

    void initialize(c_allocation_base *allocator, c_wrapped_array<const s_tag_persistent_identifier> codecs,
        s_cache_file_tag_resource_codec_resolver *resolver, c_wrapped_array<const s_cache_file_tag_resource_cursor_source_slot> sources);
    virtual s_cache_file_tag_resource_reader *create(unsigned long long position, long index, c_basic_buffer<void> buffer);
    virtual void release(unsigned long long position, long index, c_sound_cache_hash_source *source);
    __forceinline void dispose_from_old_map(c_cache_file_tag_resource_runtime_listener_storage *allocator);

    s_cache_file_tag_resource_slot_source *sources[127];
    long source_count;
    c_wrapped_array<const s_cache_file_tag_resource_cursor_source_slot> cursor_sources;
    s_cache_file_tag_resource_cursor_storage cursor;
    s_cache_file_tag_resource_stream_storage stream;
    c_basic_buffer<void> buffer;
    bool busy;
};
STATIC_ASSERT_ABI32(offsetof(s_cache_file_tag_resource_page_state, source_count) == 0x200, resource_state_source_count_offset);
STATIC_ASSERT_ABI32(offsetof(s_cache_file_tag_resource_page_state, cursor_sources) == 0x204, resource_state_cursor_sources_offset);
STATIC_ASSERT_ABI32(offsetof(s_cache_file_tag_resource_page_state, cursor) == 0x20c, resource_state_cursor_offset);
STATIC_ASSERT_ABI32(offsetof(s_cache_file_tag_resource_page_state, stream) == 0x228, resource_state_stream_offset);
STATIC_ASSERT_ABI32(offsetof(s_cache_file_tag_resource_page_state, buffer) == 0x23c, resource_state_buffer_offset);
struct s_cache_file_tag_resource_page_request
{
    __forceinline s_cache_file_tag_resource_page_request() : id(-1) {}

    __forceinline bool is_ready() const
    {
        const volatile long &request_id = id;
        return request_id != -1 && ready;
    }

    long id;
    unsigned char ready;
    c_synchronized_long done;
    c_synchronized_long progress;
    c_asynchronous_io_marker *marker;
    bool success;
    long slot;
    c_sound_cache_hash_source *source;
    c_basic_buffer<void> buffer;
};

extern "C" void cache_file_bulk_reader_complete_request(
    s_cache_file_tag_resource_page_request *request, s_cache_file_tag_resource_page_state *state);

// The legacy interface view has a four-byte vptr and four-byte member packing.
#ifndef HALO3_64BIT
#pragma pack(push, 4)
#endif
class c_cache_file_tag_resource_runtime_manager :
    public c_tag_resource_runtime_listener,
    public c_tag_resource_runtime_active_set,
    public c_tag_resource_paging_page_filter,
    public c_tag_resource_page_range_allocator
{
public:
    class c_cache_file_tag_resource_datum_handler : public c_tag_resource_cache_file_datum_handler
    {
    public:
        c_basic_buffer<void> get_control_data(long resource_handle, bool page) override;
        c_tag_resource_fixup get_root_address_fixup(long resource_handle) override;
        bool is_streamed(long resource_handle) override;
        long get_resource_definition(long resource_handle) override;
        long get_resource_owner(long resource_handle) override;
        c_wrapped_array<const s_tag_resource_fixup_location> get_control_fixups(long resource_handle) override;
        c_basic_buffer<void> get_interop_buffer(long resource_handle, bool optional) override;
        c_wrapped_array<const s_tag_resource_interop_location> get_interop_locations(long resource_handle) override;
        bool any_pageable_data(long resource_handle) override;
        bool any_optional_data(long resource_handle) override;
        long get_required_page_index(long resource_handle) override;
        long get_optional_page_index(long resource_handle) override;
        unsigned long get_pageable_offset(long resource_handle) override;
        unsigned long get_optional_offset(long resource_handle) override;
#if !defined(_XBOX) || defined(__clang__)
        bool is_optional(long resource_handle) override { return false; }
#endif

        __forceinline void initialize_for_new_map(tag_header *header, long zone_tag_index, c_wrapped_array<s_cache_file_resource_runtime_data_new> runtime_data,
            bool streaming, s_cache_file_resource_element_set *element_set)
        {
            m_header = header;
            m_resource_gestalt_index = zone_tag_index;
            m_data = runtime_data;
            m_streaming = streaming;
            m_element_set = element_set;
        }
        __forceinline void dispose_from_old_map()
        {
            m_streaming = false;
            m_data.set_elements(NULL, 0);
            m_resource_gestalt_index = -1;
        }

    private:
        __forceinline const s_cache_file_tag_resource_data *get_entry(long resource_handle) const;
        tag_header *m_header;
        long m_resource_gestalt_index;
        c_wrapped_array<s_cache_file_resource_runtime_data_new> m_data;
        bool m_streaming;
        s_cache_file_resource_element_set *m_element_set;
    };

    void initialize_for_new_map(tag_header *header, long game_mode, long gestalt_index,
        c_wrapped_array<const s_cache_file_tag_resource_vtable *> vtables, s_cache_file_tag_resource_codec_resolver *resolver);
    __forceinline s_resource_access_cookie *unlock_for_render_access();
    void commit_active_zones();
    __forceinline c_tag_resource_cache_new *get_resource_cache() { return &m_resource_cache.m_geometry_storage; }
    void *m_runtime_interfaces[5];
    tag_header *m_header;
    long m_cache_file_resource_gestalt_index;
    s_cache_file_tag_resource_zone_state m_zone_state;
    s_cache_file_tag_resource_zone_names m_zone_names;
    bool m_state_valid;
    bool m_required_resource_state_changed;
    bool m_active_resource_state_changed;
    bool m_active_zone_changed;
    s_cache_file_tag_resource_buffer m_listener_buffer;
    s_tag_resource_cache_external_access_published_table m_external_access;
    unsigned long m_required_resource_flags[1024];
    unsigned char m_active_resource_flags[4096];
    unsigned long m_active_tag_flags[625];
    const s_cache_file_tag_resource_vtable *m_resource_vtables[16];
    c_wrapped_array<s_cache_file_resource_runtime_data_new> m_resource_runtime_data;
    c_basic_buffer<void> m_interop_buffer;
    c_cache_file_tag_resource_datum_handler m_datum_handler;
    c_cache_file_tag_resource_runtime_window_allocator_storage m_window_allocator;
    c_basic_buffer<void> m_buffer;
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
    c_basic_buffer<void> m_window;
    bool m_map_initialized;
    c_cache_file_tag_resource_runtime_buffer m_locked_window;
    struct __declspec(align(32)) {
        union {
            struct {
                c_tag_resource_cache_new m_cache;
                __declspec(align(32)) c_tag_resource_thread_access m_render_access;
            };
            struct { s_geometry_storage_member m_geometry_storage; };
        };
    } m_resource_cache;
    c_cache_file_tag_resource_runtime_listener_storage m_listener_storage;
    c_basic_buffer<void> m_region;
    s_cache_file_runtime_handle_stack m_handles;
    s_data_array *m_shared_file_handles;
    c_wrapped_array<const s_cache_file_tag_resource_cursor_source_slot> m_shared_files;
    c_wrapped_array<const long> m_shared_pages;
    c_wrapped_array<const long> m_shared_requests;
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

#ifndef HALO3_64BIT
#pragma pack(pop)
#endif
STATIC_ASSERT_ABI32(offsetof(c_cache_file_tag_resource_runtime_manager, m_external_access) == 0x270, runtime_external_access_offset);
STATIC_ASSERT_ABI32(sizeof(s_tag_resource_cache_external_access_published_table) == 0x28024, runtime_external_access_size);
STATIC_ASSERT_ABI32(offsetof(c_cache_file_tag_resource_runtime_manager, m_required_resource_flags) == 0x28294, runtime_required_flags_offset);
STATIC_ASSERT_ABI32(offsetof(c_cache_file_tag_resource_runtime_manager, m_cache_file_resource_gestalt_index) == 0x28, runtime_manager_gestalt_offset);
STATIC_ASSERT_ABI32(offsetof(c_cache_file_tag_resource_runtime_manager, m_resource_cache.m_cache) == 0x2ad00, runtime_manager_cache_offset);
STATIC_ASSERT_ABI32(offsetof(c_cache_file_tag_resource_runtime_manager, m_resource_runtime_data) == 0x2ac98, runtime_manager_runtime_data_offset);
STATIC_ASSERT_ABI32(offsetof(c_cache_file_tag_resource_runtime_manager, m_interop_buffer) == 0x2aca0, runtime_manager_interop_buffer_offset);
STATIC_ASSERT_ABI32(offsetof(c_cache_file_tag_resource_runtime_manager, m_datum_handler) == 0x2aca8, runtime_manager_datum_handler_offset);
STATIC_ASSERT_ABI32(offsetof(c_cache_file_tag_resource_runtime_manager, m_window_allocator) == 0x2acc4,
    runtime_manager_window_allocator_offset);
STATIC_ASSERT_ABI32(offsetof(c_cache_file_tag_resource_runtime_manager, m_buffer) == 0x2accc, runtime_manager_buffer_offset);
STATIC_ASSERT_ABI32(offsetof(c_cache_file_tag_resource_runtime_manager, m_resource_cache.m_render_access) == 0x2b020, runtime_manager_render_access_offset);
STATIC_ASSERT_ABI32(offsetof(c_cache_file_tag_resource_runtime_manager, m_listener_storage) == 0x2b0a0, runtime_manager_listener_offset);
STATIC_ASSERT_ABI32(offsetof(c_cache_file_tag_resource_runtime_manager, m_region) == 0x2b0b4, runtime_manager_region_offset);
STATIC_ASSERT_ABI32(offsetof(c_cache_file_tag_resource_runtime_manager, m_relocations) == 0x2b30c, runtime_manager_relocations_offset);
STATIC_ASSERT_ABI32(offsetof(c_cache_file_tag_resource_runtime_manager, m_page_storage) == 0x8b31c, runtime_manager_page_storage_offset);
STATIC_ASSERT_ABI32(offsetof(c_cache_file_tag_resource_runtime_manager, m_map_initialized) == 0x2acec, runtime_map_initialized_offset);
STATIC_ASSERT_ABI32(offsetof(c_cache_file_tag_resource_runtime_manager, m_locked_window) == 0x2acf0, runtime_locked_window_offset);
STATIC_ASSERT(sizeof(s_cache_file_tag_resource_zone_state) == 0x24, runtime_zone_state_size);
STATIC_ASSERT(sizeof(s_cache_file_tag_resource_zone_names) == 0x20c, runtime_zone_names_size);
STATIC_ASSERT_ABI32(offsetof(c_cache_file_tag_resource_runtime_manager, m_zone_state) == 0x2c, runtime_zone_state_offset);
STATIC_ASSERT_ABI32(offsetof(c_cache_file_tag_resource_runtime_manager, m_zone_names) == 0x50, runtime_zone_names_offset);
STATIC_ASSERT_ABI32(offsetof(c_cache_file_tag_resource_runtime_manager, m_state_valid) == 0x25c, runtime_zone_valid_offset);
STATIC_ASSERT_ABI32(offsetof(c_cache_file_tag_resource_runtime_manager, m_required_resource_state_changed) == 0x25d,
    runtime_required_resource_state_changed_offset);
STATIC_ASSERT_ABI32(offsetof(c_cache_file_tag_resource_runtime_manager, m_active_resource_state_changed) == 0x25e,
    runtime_active_resource_state_changed_offset);
STATIC_ASSERT_ABI32(offsetof(c_cache_file_tag_resource_runtime_manager, m_active_zone_changed) == 0x25f,
    runtime_active_zone_changed_offset);
STATIC_ASSERT_ABI32(
    offsetof(c_cache_file_tag_resource_runtime_manager, m_resource_cache.m_geometry_storage) == 0x2ad00, runtime_manager_geometry_storage_offset);
STATIC_ASSERT_ABI32(offsetof(c_cache_file_tag_resource_runtime_manager, m_listener_storage.m_second) == 0x2b0ac,
    runtime_manager_remaining_geometry_buffer_offset);
STATIC_ASSERT(offsetof(s_cache_file_tag_resource_zone_names, m_first) == 4, runtime_zone_names_first_offset);
STATIC_ASSERT_ABI32(offsetof(c_cache_file_tag_resource_runtime_manager, m_first_cursor) == 0x2b104, runtime_manager_first_cursor_offset);
STATIC_ASSERT_ABI32(offsetof(c_cache_file_tag_resource_runtime_manager, m_second_cursor) == 0x2b208, runtime_manager_second_cursor_offset);
STATIC_ASSERT_ABI32(offsetof(c_cache_file_tag_resource_runtime_manager, m_result_buffer) == 0x2acdc, runtime_manager_result_buffer_offset);
STATIC_ASSERT_ABI32(offsetof(c_cache_file_tag_resource_runtime_manager, m_file_slots) == 0x8b58c, runtime_manager_file_slots_offset);
STATIC_ASSERT_ABI32(offsetof(c_cache_file_tag_resource_runtime_manager, m_game_mode) == 0x8b618, runtime_manager_game_mode_offset);
STATIC_ASSERT_ABI32(offsetof(c_cache_file_tag_resource_runtime_manager, m_locked) == 0x8b61d, runtime_manager_locked_offset);



#endif
