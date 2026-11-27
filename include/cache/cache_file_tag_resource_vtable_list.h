#ifndef CACHE_CACHE_FILE_TAG_RESOURCE_VTABLE_LIST_H
#define CACHE_CACHE_FILE_TAG_RESOURCE_VTABLE_LIST_H

#include "cache/cache_file_resource_gestalt.h"

// Per resource type callbacks handed to c_cache_file_tag_resource_runtime_manager::initialize_for_new_map.
struct s_cache_file_tag_resource_vtable
{
    s_tag_persistent_identifier identifier;
    bool (*register_resource)(long tag_index, void *resource);
    void (*unregister_resource)(long tag_index, void *resource);
};

// The structure_bsp entry and the null-terminated list of all resource types.
extern s_cache_file_tag_resource_vtable scenario_structure_bsp_cache_file_tag_resource_vtable;
extern const s_cache_file_tag_resource_vtable *g_cache_file_tag_resource_vtable_list[];
// Entries in g_cache_file_tag_resource_vtable_list, null terminator included.
const long g_cache_file_tag_resource_vtable_count = 2;

#endif // CACHE_CACHE_FILE_TAG_RESOURCE_VTABLE_LIST_H
