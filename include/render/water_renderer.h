#ifndef RENDER_WATER_RENDERER_H
#define RENDER_WATER_RENDERER_H

#include "math/real_math_types.h"

enum e_entry_point;
struct s_render_geometry;
struct geometry_material;
void rasterizer_render_water(bool enabled);

class c_water_renderer
{
    friend void ::rasterizer_render_water(bool enabled);

public:
    static bool update_water_part_list();
    static void initialize();
    static void dispose();
    static void initialize_for_new_map();
    static void dispose_from_old_map();
    static void game_interation_event_add(long type, const real_point3d *position,
        const real_vector3d *direction, const real_vector3d *velocity);
    static void ripple_update();
    static void ripple_apply();
    static void ripple_slope();
    static void render_shading();

private:
    static void render_cluster_parts(e_entry_point entry_point, long part_type);
    static void render_water_part(const s_render_geometry *render_geometry, long mesh_index, long part_index,
        e_entry_point entry_point, const geometry_material *materials);
};

#endif
