#ifndef CACHE_GEOMETRY_STORAGE_H
#define CACHE_GEOMETRY_STORAGE_H

#include <stddef.h>
#include "memory/static_arrays.h"
#include "memory/physical_memory_map_types.h"
#include "cache/tag_resource_external_access_state.h"
#include "cache/tag_resource_page_table.h"
#include "cache/tag_resource_cache_new.h"
#include "multithreading/read_write_lock.h"
#include "cache/cache_file_resource_element_set.h"
#include "cache/cache_file_tag_resource_relocation.h"

class c_tag_resource_cache_file_access_cache;
struct s_data_array;
class c_tag_resource_cache_thread_lock_lock_freeish;
struct s_resource_access_cookie;
class c_tag_resource_thread_access {
public:
    long m_acquired_access_storage[9];
    c_tag_resource_cache_file_access_cache *m_access_cache_storage[9];
    long m_acquired_access_count;
    long *m_per_thread_acquired_access_cache;
    long m_access_cache_count;
    c_tag_resource_cache_file_access_cache **m_per_thread_access_cache;
    c_read_write_lock m_access_lock;
    c_tag_resource_cache_thread_lock_lock_freeish *m_resource_thread_access_lock;
private:
    friend class c_cache_file_tag_resource_runtime_manager;
    friend void tag_resources_unlock_game(long &);
    s_resource_access_cookie *unlock_access_for_current_thread_internal(bool allocate_event);
};

struct s_cache_file_tag_resource_vtable;
struct s_cache_file_resource_runtime_data_new;

struct s_geometry_first_base : c_tag_resource_cache_new
{
};

struct s_geometry_second_base
{
    long m_acquired_access_storage[9];
    c_tag_resource_cache_file_access_cache *m_access_cache_storage[9];
    long m_acquired_access_count;
    long *m_per_thread_acquired_access_cache;
    long m_access_cache_count;
    c_tag_resource_cache_file_access_cache **m_per_thread_access_cache;
    c_read_write_lock m_access_lock;
};

struct s_geometry_storage_member : s_geometry_first_base, s_geometry_second_base
{
};

struct xenon_minidump_geometry_buffer
{
    void *address;
    long size;
};

struct s_geometry_path_string
{
    long file_index;
    char file_name[256];
};

struct s_cache_file_runtime_handle_stack
{
    s_cache_file_runtime_handle_stack() {}
    explicit s_cache_file_runtime_handle_stack(long count) : m_count(count) {}

    __forceinline unsigned long *begin() { return m_elements; }
    __forceinline unsigned long *end() { return &m_elements[m_count]; }
    __forceinline unsigned long &operator[](long index) { return m_elements[index]; }

    unsigned long m_elements[4];
    long m_count;
};

class c_cache_file_tag_resource_allocator;
struct s_cache_file_tag_resource_buffer
{
    void dispose();
    void *m_address;
    long m_size;
#ifdef HALO3_64BIT
    size_t m_allocated;
#else
    unsigned long m_allocated;
#endif
    c_cache_file_tag_resource_allocator *m_allocator;
};

struct s_cache_file_tag_resource_named_value
{
    long m_value;
    c_static_string<256> m_name;
};
struct s_cache_file_tag_resource_active_state
{
    unsigned char flags;
    unsigned long mask4;
    unsigned long mask8;
    unsigned long maskc;
    unsigned long mask10;
    unsigned long mask14;
    unsigned long mask18;
    unsigned long mask1c;
};
struct s_cache_file_tag_resource_zone_state
{
        __forceinline void reset()
        {
            for (long index = 0; index < NUMBEROF(m_state_words); index++)
                m_state_words[index] = 0;
            m_active_zone_handle = NONE;
        }

        union { long m_state_words[8]; s_cache_file_tag_resource_active_state m_active_state; };
        long m_active_zone_handle;
};
struct s_cache_file_tag_resource_zone_names
{
        __forceinline void reset()
        {
            m_zone_set_index = NONE;
            reset_name(&m_first);
            reset_name(&m_second);
        }
        static __forceinline void reset_name(s_cache_file_tag_resource_named_value *name)
        {
            name->m_value = NONE;
            name->m_name.clear();
        }

        long m_zone_set_index;
        s_cache_file_tag_resource_named_value m_first;
        s_cache_file_tag_resource_named_value m_second;
};


struct s_cache_file_tag_resource_runtime_resource_arrays
{
    c_wrapped_array<long> m_resource_flags;
    c_wrapped_array<long> m_resource_values;
    c_wrapped_array<long> m_resource_handles;
    c_wrapped_array<long> m_resource_owners;
};

#if defined(_M_PPC) || defined(_M_PPCBE) || defined(_XBOX)
typedef char geometry_storage_path_size[(sizeof(s_geometry_path_string) == 0x104) ? 1 : -1];
typedef char geometry_storage_path_name_offset[(offsetof(s_geometry_path_string, file_name) == 4) ? 1 : -1];
typedef char geometry_storage_cache_base_size[(sizeof(void *) != 4 || ((sizeof(s_geometry_first_base) == 0x320))) ? 1 : -1];
typedef char geometry_storage_secondary_lock_offset[(sizeof(void *) != 4 || ((offsetof(s_geometry_second_base, m_access_lock) == 0x58))) ? 1 : -1];
typedef char geometry_storage_thread_access_size[(sizeof(void *) != 4 || ((sizeof(c_tag_resource_thread_access) == 0x68))) ? 1 : -1];
typedef char geometry_storage_acquired_access_offset[
    (sizeof(void *) != 4 || ((offsetof(c_tag_resource_thread_access, m_per_thread_acquired_access_cache) == 0x4c))) ? 1 : -1];
#endif

#include "cache/geometry_globals.h"
extern "C" s_geometry_globals g_geometry_globals;

#endif
