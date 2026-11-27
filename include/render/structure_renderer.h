#ifndef RENDER_STRUCTURE_RENDERER_H
#define RENDER_STRUCTURE_RENDERER_H

#include "render_methods/render_method_definitions.h"

class c_structure_renderer
{
public:
    static void initialize();
    static void dispose();
    static void submit_visibility(bool render_transparents);
    static void pop_marker();
    static void initialize_for_new_map();
    static void dispose_from_old_map();
    static void reset();
    static void initialize_for_new_structure_bsp(unsigned long structure_bsp_mask);
    static void dispose_from_old_structure_bsp(unsigned long structure_bsp_mask);
    static void render_transparent_cluster_mesh_part(const void *context, long mesh_part);
    static void update_water_structure_part_list(long *part_indices, long *part_count);
    static void render_albedo();
    static void render_decorators();
    static void render_cluster_parts(e_entry_point entry_point, long user_index);
    static void render_cluster_mesh_part(long mesh_part, e_entry_point entry_point, long count);

private:
    static void reset_bsp_cached_data(unsigned long structure_bsp_mask);
};

#endif // RENDER_STRUCTURE_RENDERER_H
