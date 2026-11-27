// OBJ: from the records 0xb2258 0xb2420
// @compiler xdk6130
// @flags /TP /O2 /Oc
#include "math/matrix_math.h"
#include "math/projection_math.h"
#include "tag_files/tag_types.h"
#include "tag_files/tag_groups.h"
#include "ai/sector_surface_bsp.h"
#include "ai/pathfinding_data.h"
#include "physics/collision_bsp_types.h"

struct sector_bsp2d_node
{
    real_plane2d plane;
    long front_child;
    long back_child;
};

static __forceinline real_plane3d *sector_surface_bsp_get_plane_from_designator(const collision_bsp *bsp, long designator, real_plane3d *plane)
{
    const real_plane3d *source = &((const real_plane3d *)bsp->planes.address)[designator & 0x7fff];
    if (designator & 0x8000)
    {
        plane->normal.i = -source->normal.i;
        plane->normal.j = -source->normal.j;
        plane->normal.k = -source->normal.k;
        plane->distance = -source->distance;
    }
    else
        *plane = *source;
    return plane;
}

// @address 0xb2258 ?sector_bsp2d_traverse@@YAJPBUpathfinding_data@@PBUcollision_bsp@@JPBUs_tag_block@@PBTreal_point3d@@PBUreal_matrix4x3@@@Z
long sector_bsp2d_traverse(const pathfinding_data *data, const collision_bsp *bsp, long index, const s_tag_block *block,
    const real_point3d *point, const real_matrix4x3 *matrix)
{
    long result = -1;
    if (index >= 0 && index < block->count)
    {
        long *root = TAG_BLOCK_GET_ELEMENT(block, index, long);
        collision_surface *surface = TAG_BLOCK_GET_ELEMENT(&bsp->surfaces, index, collision_surface);
        real_plane3d plane;
        real_point3d local_point;
        real_point2d projected;
        sector_surface_bsp_get_plane_from_designator(bsp, surface->plane, &plane);
        short projection = projection_from_vector3d(&plane.normal);
        bool sign = projection_sign_from_vector3d(&plane.normal, projection);
        if (matrix)
            matrix4x3_inverse_transform_point(matrix, point, &local_point);
        else
            local_point = *point;
        project_point3d(&local_point, projection, sign, &projected);
        result = recursive_traverse_sector_bsp2d_node(data, *root, &projected);
    }
    return result;
}

// @address 0xb2420 ?recursive_traverse_sector_bsp2d_node@@YAJPBUpathfinding_data@@JPBTreal_point2d@@@Z
long recursive_traverse_sector_bsp2d_node(const pathfinding_data *data, long node_index, const real_point2d *point)
{
    if (node_index != -1 && (node_index & FLAG(31)) != 0)
    {
        long bsp2d_node_index = node_index & ~FLAG(31);
        if (bsp2d_node_index >= 0 && bsp2d_node_index < data->bsp2d_nodes.count)
        {
            const sector_bsp2d_node *node = TAG_BLOCK_GET_ELEMENT(&data->bsp2d_nodes, bsp2d_node_index, sector_bsp2d_node);
            if (plane2d_distance_to_point(&node->plane, point) > 0.0f)
            {
                return recursive_traverse_sector_bsp2d_node(data, node->front_child, point);
            }
            else
            {
                return recursive_traverse_sector_bsp2d_node(data, node->back_child, point);
            }
        }
        else
        {
            return -1;
        }
    }
    else
    {
        return node_index;
    }
}
