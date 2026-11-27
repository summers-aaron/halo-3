#ifndef VISIBILITY_VISIBLE_ITEMS_H
#define VISIBILITY_VISIBLE_ITEMS_H

#include "visibility/visible_items_types.h"

// The marker stack and interface owned by visibility_collection.cpp.
// March visibility_collection records attest these shared method names.
class c_visible_items
{
public:
    static s_visible_items m_items;
    static __forceinline s_visible_items &get_global_items() { return m_items; }
    static __forceinline long get_camera_root_objects_count()
    {
        if (m_marker_count == 0)
            return m_items.hierarchies.get_count();
        return m_markers[1].root_objects_starting_index;
    }
    static __forceinline long get_camera_instances_count()
    {
        if (m_marker_count == 0)
            return m_items.instances.get_count();
        return m_markers[1].instances;
    }
    static void clear_all();
    static void push_marker();
    static void pop_marker();
    static __forceinline void clear();
    static __forceinline unsigned short get_root_objects_starting_index() { return m_markers[m_marker_count].root_objects_starting_index; }
    static __forceinline unsigned short get_objects_starting_index() { return m_markers[m_marker_count].objects; }
    static __forceinline unsigned short get_cluster_starting_index() { return m_markers[m_marker_count].clusters; }
    static __forceinline unsigned short get_instance_list_starting_index() { return m_markers[m_marker_count].instance_lists; }
    static __forceinline unsigned short get_instances_starting_index() { return m_markers[m_marker_count].instances; }
    static __forceinline unsigned short get_light_starting_index() { return m_markers[m_marker_count].objects_to_render; }
    static __forceinline long get_marker_index() { return m_marker_count; }
private:
    struct s_marker_indices
    {
        unsigned short root_objects_starting_index;
        unsigned short objects, instance_lists, instances, clusters, objects_to_render;
        unsigned short instance_visibility_count, instance_visibility_bit_count, skies;
    };
    // Six collection markers; the marker count indexes this stack.
    static s_marker_indices m_markers[6];
    static long m_marker_count;
};

#endif // VISIBILITY_VISIBLE_ITEMS_H
