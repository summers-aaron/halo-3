#ifndef AI_SECTOR_SURFACE_BSP_H
#define AI_SECTOR_SURFACE_BSP_H

// Functions defined in tu/ailibxbox/sector_surface_bsp.cpp that other units call.

struct collision_bsp;
struct pathfinding_data;
struct real_matrix4x3;
union real_point2d;
union real_point3d;
struct s_tag_block;

long sector_bsp2d_traverse(const pathfinding_data *data, const collision_bsp *bsp, long index,
    const s_tag_block *block, const real_point3d *point, const real_matrix4x3 *matrix);
long recursive_traverse_sector_bsp2d_node(const pathfinding_data *data, long node_index, const real_point2d *point);

#endif // AI_SECTOR_SURFACE_BSP_H
