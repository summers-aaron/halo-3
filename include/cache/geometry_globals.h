#ifndef CACHE_GEOMETRY_GLOBALS_H
#define CACHE_GEOMETRY_GLOBALS_H

#include <stddef.h>

class c_cache_file_tag_resource_runtime_manager;
class c_cache_file_tag_resource_runtime_manager_storage;
class c_cache_file_tag_resource_allocator;

// The allocation the runtime manager is placed in; copied as one 8-byte unit.
struct __declspec(align(8)) s_shell_memory_buffer
{
    void *address;
    unsigned long size;
};

// Geometry runtime state shared by the sound cache and minidump readers.
struct s_geometry_globals
{
    s_shell_memory_buffer allocation;
    union
    {
        c_cache_file_tag_resource_runtime_manager_storage *storage;
        c_cache_file_tag_resource_runtime_manager *manager;
    };
    c_cache_file_tag_resource_allocator *allocator;
};

extern "C" {
extern s_geometry_globals g_geometry_globals;
}

typedef char geometry_globals_size[(sizeof(void *) != 4 || sizeof(s_geometry_globals) == 0x10) ? 1 : -1];
typedef char geometry_globals_storage_offset[(sizeof(void *) != 4 || offsetof(s_geometry_globals, storage) == 8) ? 1 : -1];
typedef char geometry_globals_allocator_offset[(sizeof(void *) != 4 || offsetof(s_geometry_globals, allocator) == 0xc) ? 1 : -1];

#endif // CACHE_GEOMETRY_GLOBALS_H
