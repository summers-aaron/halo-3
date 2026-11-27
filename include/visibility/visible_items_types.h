#ifndef VISIBILITY_VISIBLE_ITEMS_TYPES_H
#define VISIBILITY_VISIBLE_ITEMS_TYPES_H

#include <stddef.h>
#include "render/render_objects_static_lighting_hierarchy_levels_types.h"
#include "objects/cluster_reference.h"
#include "render/render_objects_static_lighting_setup_types.h"

#pragma pack(push, 2)
struct s_object_hierarchy
{
    short flags;
    unsigned short region_cluster_bitvector_index;
    long first_object;
    unsigned short object_count;
    union
    {
        struct { render_objects_static_lighting_hierarchy_levels levels; short hierarchy_padding; };
        struct { long levels_word; short hierarchy_word_padding; };
    };
};
#pragma pack(pop)
struct s_object_visibility
{
    s_render_objects_static_lighting_setup render_setup;
};
template<class T, long capacity> struct simple_list
{
    unsigned short unknown, count;
    T entries[capacity];
    __forceinline long get_count() { return count; }
    __forceinline void set_count(unsigned short value) { count = value; }
    __forceinline void clear() { count = 0; }
    __forceinline T *list_iterator_next(long *iterator)
    {
        if (*iterator >= count - 1)
            return 0;
        return &entries[++*iterator];
    }
    __forceinline T *list_iterator_new(long *iterator, long start)
    {
        T *result;
        *iterator = start - 1;
        if (*iterator < count - 1)
        {
            ++*iterator;
            result = &entries[*iterator];
        }
        else
        {
            *iterator = -1;
            result = 0;
        }
        return result;
    }
    __forceinline long add()
    {
        long result = -1;
        if (count < unknown) result = count++;
        return result;
    }
    __forceinline T *list_iterator_next_reset(long *iterator)
    {
        if (*iterator < count - 1) return &entries[++*iterator];
        *iterator = -1;
        return 0;
    }
    __forceinline T *list_iterator_new_reset(long *iterator, long start)
    {
        *iterator = start - 1;
        return list_iterator_next_reset(iterator);
    }
    __forceinline T &operator[](long index) { return entries[index]; }
};

struct s_visible_instance_list
{
    union
    {
        s_cluster_reference cluster_reference;
        struct { signed char bsp_index; unsigned char cluster_index; };
    };
    unsigned short unknown;
    short subpart_index;
    unsigned short first_instance;
    unsigned short instance_count;
    unsigned short mesh_index;
};
struct s_visible_instances
{
    short flags;
    unsigned short instance_index;
    short bsp_index;
    short region_cluster_bitvector_start_index;
    // Collection writes fade alpha; render_structure consumes this byte as lod.
    union { unsigned char alpha; unsigned char lod; };
    unsigned long *visible_parts;
};
struct s_visible_clusters
{
    short flags;
    s_cluster_reference cluster_reference;
    short pad4;
    unsigned short region_cluster;
    unsigned short mesh_index;
    unsigned long *visible_parts;
};

struct s_visible_items
{
    // The list headers below start with their capacities, so hierarchies and objects
    // have no pad members.
    simple_list<s_object_hierarchy, 768> hierarchies;
    simple_list<s_object_visibility, 896> objects;
    simple_list<s_visible_instance_list, 1024> instance_lists;
    simple_list<s_visible_instances, 1152> instances;
    simple_list<s_visible_clusters, 348> clusters;
    simple_list<long, 50> objects_to_render;
    simple_list<long, 6> skies;
    long instance_visibility[512];
    unsigned short instance_visibility_count;
    unsigned short instance_visibility_bit_count;
    unsigned long region_cluster_bitvectors[512][4];
};

typedef char visible_hierarchy_size[(sizeof(s_object_hierarchy) == 0x10) ? 1 : -1];
typedef char visible_hierarchy_levels_offset[(offsetof(s_object_hierarchy, levels) == 0xa) ? 1 : -1];
typedef char visible_object_size[(sizeof(void *) != 4 || ((sizeof(s_object_visibility) == 0x58))) ? 1 : -1];
typedef char visible_object_flags_offset[(sizeof(void *) != 4 || offsetof(s_object_visibility, render_setup) +
    offsetof(s_render_objects_static_lighting_setup, flags) == 0x54) ? 1 : -1];
typedef char visible_objects_offset[(sizeof(void *) != 4 || offsetof(s_visible_items, objects) == 12292) ? 1 : -1];
typedef char visible_lights_offset[(sizeof(void *) != 4 || offsetof(s_visible_items, objects_to_render) == 0x1f1d4) ? 1 : -1];
typedef char visible_skies_offset[(sizeof(void *) != 4 || offsetof(s_visible_items, skies) == 0x1f2a0) ? 1 : -1];
typedef char visible_instances_offset[(sizeof(void *) != 4 || offsetof(s_visible_items, instances) == 103436) ? 1 : -1];
typedef char visible_instance_size[(sizeof(void *) != 4 || sizeof(s_visible_instances) == 0x10) ? 1 : -1];
typedef char visible_instance_parts_offset[(sizeof(void *) != 4 || offsetof(s_visible_instances, visible_parts) == 0xc) ? 1 : -1];
typedef char visible_cluster_size[(sizeof(void *) != 4 || sizeof(s_visible_clusters) == 0x10) ? 1 : -1];
typedef char visible_cluster_parts_offset[(sizeof(void *) != 4 || offsetof(s_visible_clusters, visible_parts) == 0xc) ? 1 : -1];
typedef char visible_region_bitvectors_offset[(sizeof(void *) != 4 || offsetof(s_visible_items, region_cluster_bitvectors) == 0x1fac0) ? 1 : -1];

#endif // VISIBILITY_VISIBLE_ITEMS_TYPES_H
