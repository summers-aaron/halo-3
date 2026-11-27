// blamlibxbox/collision_bsp.obj: from the records 0x9e5d0 0x2b24c8 0x2b2578 0x2b29a8 0x2b2a88 0x2b2f50 0x2b3340 0x2b35a0
// @compiler xdk6130
// @flags /TP /O2 /Oc
#include "cseries/cseries_wrapped.h"
#include <math.h>
#include "tag_files/tag_types.h"
#include "physics/collision_bsp.h"
#include "render/render_transparents.h"
#include "math/real_math_types.h"
#include "math/real_math.h"
#include "physics/collision_bsp_types.h"
#include "physics/breakable_surface_types.h"
#include "cseries/cseries.h"
struct bsp2d;
#include "physics/bsp2d.h"
#include "physics/bsp3d.h"
#include "physics/collision_bsp_query_types.h"
#include "math/projection_math.h"

// collision_bsp_test_vector types, shared with bsp3d_test_sphere_recursive
struct test_vector_data
{
    unsigned long flags;
    const collision_bsp *bsp;
    short breakable_surface_set_index;
    const s_breakable_surface_set *breakable_surface_set;
    const real_point3d *point;
    const real_vector3d *vector;
    const s_collision_materials_extant_flags *materials;
    collision_bsp_test_vector_result *result;
    long leaf_index;
    unsigned char leaf_type;
    long plane_index;
};

struct test_sphere_data
{
    c_flags<e_collision_bsp_test_sphere_flag, unsigned long, k_collision_bsp_test_sphere_flag_count> flags;
    const collision_bsp *bsp;
    short breakable_surface_count;
    const s_breakable_surface_set *breakable_surface_set;
    const real_point3d *point;
    float radius;
    collision_bsp_test_sphere_result *result;
    long stack_depth;
    long stack[128];
    short projection_axis;
    bool projection_sign;
    real_point2d projected_point;
};

static __forceinline bool collision_bsp_materials_extant_test(const s_collision_materials_extant_flags *materials, long material)
{
    return material == -1 || (materials->flags[material >> 5] & (1UL << (material & 31))) != 0;
}
static __forceinline bool collision_bsp_surface_is_extant(const collision_bsp *bsp, const s_collision_materials_extant_flags *materials, long surface_index)
{
    const collision_surface *surface = &((const collision_surface *)bsp->surfaces.address)[surface_index];
    if (materials && !collision_bsp_materials_extant_test(materials, surface->material))
        return false;
    return true;
}
static __forceinline long collision_bsp_bsp3d_node_get_child_index(const bsp3d_node *node, long child)
{
    return (long)(child ? node->front_child : node->back_child);
}
static __forceinline long collision_bsp_bsp3d_node_get_plane(const bsp3d_node *node)
{
    return (short)node->plane;
}
static __forceinline void collision_bsp_test_sphere_result_add(long *count, long *indices, long *planes, long index, long plane_index)
{
    for (short i = 0; i < *count; i++)
    {
        if (indices[i] == index)
            return;
    }
    if (*count < 256)
    {
        indices[*count] = index;
        planes[*count] = plane_index;
        (*count)++;
    }
}

#define COLLISION_BSP_TEST_SPHERE_DONE(data) (data)->flags.test(_collision_bsp_test_sphere_stop_at_first_hit_bit) && \
    ((data)->result->surface_count != 0 || (data)->result->edge_count != 0 || (data)->result->vertex_count != 0 || (data)->result->leaf_count != 0)


// @address 0x2b24c8 ?collision_bsp_test_sphere@@YA_NPBUcollision_bsp@@V?$c_flags@W4e_collision_bsp_test_sphere_flag@@K$02@@JPBUs_breakable_surface_set@@PBTreal_point3d@@MPAUcollision_bsp_test_sphere_result@@@Z
bool collision_bsp_test_sphere(const collision_bsp *bsp,
    c_flags<e_collision_bsp_test_sphere_flag, unsigned long, k_collision_bsp_test_sphere_flag_count> flags,
    long breakable_surface_set_count, const s_breakable_surface_set *breakable_surface_set, const real_point3d *point, float radius,
    collision_bsp_test_sphere_result *result)
{
    test_sphere_data data;
    data.flags = flags;
    data.bsp = bsp;
    data.breakable_surface_count = (short)breakable_surface_set_count;
    data.breakable_surface_set = breakable_surface_set;
    data.point = point;
    data.radius = radius;
    data.result = result;
    data.stack_depth = 0;
    result->leaf_count = 0;
    result->surface_count = 0;
    result->edge_count = 0;
    result->vertex_count = 0;
    bsp3d_test_sphere_recursive(&data, 0);
    return result->surface_count > 0 || result->edge_count > 0 || result->vertex_count > 0;
}

// 0x2b2578 ?bsp3d_test_sphere_recursive@@YAXPAUtest_sphere_data@@J@Z
void bsp3d_test_sphere_recursive(test_sphere_data *data, long node_index)
{
    const collision_bsp *bsp = data->bsp;
    if (!(node_index & 0x800000))
    {
        const bsp3d_node *node = &((const bsp3d_node *)bsp->bsp3d_nodes.address)[node_index];
        long back_child = collision_bsp_bsp3d_node_get_child_index(node, 0);
        long front_child = collision_bsp_bsp3d_node_get_child_index(node, 1);
        const real_plane3d *plane = &((const real_plane3d *)bsp->planes.address)[collision_bsp_bsp3d_node_get_plane(node)];
        float distance = (dot_product3d(&plane->normal, data->point) - plane->d);
        bool behind = distance < data->radius;
        bool in_front = distance > -data->radius;
        if (in_front && behind)
        {
            data->stack[data->stack_depth] = (short)((collision_bsp_bsp3d_node_get_plane(node) & 0x7fff) | 0x8000);
            data->stack_depth++;
            bsp3d_test_sphere_recursive(data, back_child);
            if (COLLISION_BSP_TEST_SPHERE_DONE(data))
                return;
            data->stack_depth--;
            data->stack[data->stack_depth] = (short)(collision_bsp_bsp3d_node_get_plane(node) & 0x7fff);
            data->stack_depth++;
            bsp3d_test_sphere_recursive(data, front_child);
            if (COLLISION_BSP_TEST_SPHERE_DONE(data))
                return;
            data->stack_depth--;
        }
        else
        {
            bsp3d_test_sphere_recursive(data, collision_bsp_bsp3d_node_get_child_index(node, in_front));
            if (COLLISION_BSP_TEST_SPHERE_DONE(data))
                return;
        }
    }
    else if (node_index != -1)
    {
        long leaf_index = node_index & 0x7fffff;
        const collision_leaf *leaf = &((const collision_leaf *)bsp->leaves.address)[leaf_index];
        if (data->flags.test(_collision_bsp_test_sphere_leaves_bit) && data->result->leaf_count < 256)
        {
            data->result->leaf_indices[data->result->leaf_count] = leaf_index;
            data->result->leaf_count++;
        }
        if (data->flags.test(_collision_bsp_test_sphere_surfaces_bit))
        {
            for (unsigned long reference_index = leaf->first_reference_index;
                reference_index < leaf->first_reference_index + leaf->reference_count; reference_index++)
            {
                const collision_reference *reference = &((const collision_reference *)data->bsp->bsp2d_references.address)[reference_index];
                for (short stack_index = 0; stack_index < data->stack_depth; stack_index++)
                {
                    if (data->stack[stack_index] == reference->plane_index)
                    {
                        const real_plane3d *plane = &((const real_plane3d *)data->bsp->planes.address)[reference->plane_index & 0x7fff];
                        real_point3d point;
                        point_from_line3d(data->point, &plane->normal, -(dot_product3d(&plane->normal, data->point) - plane->d), &point);
                        data->projection_axis = projection_from_vector3d(&plane->normal);
                        data->projection_sign = projection_sign_from_vector3d(&plane->normal, data->projection_axis) !=
                            ((reference->plane_index & 0x8000) != 0);
                        project_point3d(&point, data->projection_axis, data->projection_sign, &data->projected_point);
                        bsp2d_test_sphere_recursive(data, reference->node_index, leaf_index);
                        break;
                    }
                }
            }
        }
    }
}

// 0x2b29a8 ?bsp2d_test_sphere_recursive@@YAXPAUtest_sphere_data@@JJ@Z
void bsp2d_test_sphere_recursive(test_sphere_data *data, long bsp2d_node_index, long leaf_index)
{
    if (!(bsp2d_node_index & 0x8000))
    {
        const real_point2d *point = &data->projected_point;
        do
        {
            const bsp2d_node *node = &((const bsp2d_node *)data->bsp->bsp2d_nodes.address)[bsp2d_node_index & 0x7fff];
            float distance = node->plane.i * point->x + node->plane.j * point->y - node->plane.d;
            bool left = distance <= data->radius;
            bool right = distance >= -data->radius;
            if (left)
                bsp2d_test_sphere_recursive(data, node->children[0], leaf_index);
            if (!right)
                return;
            bsp2d_node_index = node->children[1];
        } while (!(bsp2d_node_index & 0x8000));
    }
    collision_surface_test_sphere(data, bsp2d_node_index & 0x7fff, leaf_index);
}

// @address 0x2b2a88 ?collision_surface_test_sphere@@YAXPAUtest_sphere_data@@JJ@Z
void collision_surface_test_sphere(test_sphere_data *data, long surface_index, long plane_index)
{
    const collision_surface *surface = &((const collision_surface *)data->bsp->surfaces.address)[surface_index];
    if (!(surface->flags & 8) ||
        surface->breakable_surface < 0 ||
        surface->breakable_surface >= data->breakable_surface_count ||
        !data->breakable_surface_set ||
        data->breakable_surface_set[surface->breakable_surface].surface_flags.test(surface->breakable_surface_bit))
    {
        bool hit = false;
        long edge_index = surface->first_edge;
        do
        {
            const collision_edge *edge = &((const collision_edge *)data->bsp->edges.address)[edge_index];
            bool right = edge->surfaces[1] == surface_index;
            long vertex_index = edge->vertices[right];
            const collision_vertex *vertex = &((const collision_vertex *)data->bsp->vertices.address)[vertex_index];
            if (point_in_sphere(&vertex->point, data->point, data->radius))
            {
                collision_bsp_test_sphere_result_add(
                    &data->result->vertex_count, data->result->vertex_indices, data->result->vertex_planes, vertex_index, plane_index);
                hit = true;
            }
            edge_index = edge->edges[right];
        } while (edge_index != surface->first_edge);

        edge_index = surface->first_edge;
        do
        {
            const collision_edge *edge = &((const collision_edge *)data->bsp->edges.address)[edge_index];
            bool right = edge->surfaces[1] == surface_index;
            const collision_vertex *vertex0 = &((const collision_vertex *)data->bsp->vertices.address)[edge->vertices[right]];
            const collision_vertex *vertex1 = &((const collision_vertex *)data->bsp->vertices.address)[edge->vertices[!right]];
            real_vector3d vector;
            vector_from_points3d(&vertex0->point, &vertex1->point, &vector);
            if (fast_vector_intersects_sphere(&vertex0->point, &vector, data->point, data->radius))
            {
                collision_bsp_test_sphere_result_add(&data->result->edge_count, data->result->edge_indices, data->result->edge_planes, edge_index, plane_index);
                hit = true;
            }
            edge_index = edge->edges[right];
        } while (edge_index != surface->first_edge);

        bool inside = true;
        if (!hit)
        {
            edge_index = surface->first_edge;
            do
            {
                const collision_edge *edge = &((const collision_edge *)data->bsp->edges.address)[edge_index];
                bool right = edge->surfaces[1] == surface_index;
                const collision_vertex *vertex0 = &((const collision_vertex *)data->bsp->vertices.address)[edge->vertices[right]];
                const collision_vertex *vertex1 = &((const collision_vertex *)data->bsp->vertices.address)[edge->vertices[!right]];
                real_point2d point0, point1;
                real_vector2d vector0, vector1;
                project_point3d(&vertex0->point, data->projection_axis, data->projection_sign, &point0);
                project_point3d(&vertex1->point, data->projection_axis, data->projection_sign, &point1);
                vector_from_points2d(&data->projected_point, &point0, &vector0);
                vector_from_points2d(&data->projected_point, &point1, &vector1);
                if (cross_product2d(&vector0, &vector1) < 0.0f)
                {
                    inside = false;
                    break;
                }
                edge_index = edge->edges[right];
            } while (edge_index != surface->first_edge);
        }

        if (hit || inside)
            collision_bsp_test_sphere_result_add(
                &data->result->surface_count, data->result->surface_indices, data->result->surface_planes, surface_index, plane_index);
    }
}

// 0x9e5d0 collision_bsp_test_vector (unplaced)
bool collision_bsp_test_vector(unsigned long flags, const collision_bsp *bsp, const s_collision_materials_extant_flags *materials,
    short breakable_surface_set_index, const s_breakable_surface_set *breakable_surface_set, const real_point3d *point,
    const real_vector3d *vector, float max_t, collision_bsp_test_vector_result *result)
{
    test_vector_data data;
    data.flags = flags;
    data.bsp = bsp;
    data.breakable_surface_set_index = breakable_surface_set_index;
    data.breakable_surface_set = breakable_surface_set;
    data.point = point;
    data.vector = vector;
    data.result = result;
    data.materials = materials;
    result->fraction = MAX(max_t, 0.0f);
    result->leaf_count = 0;
    data.leaf_index = NONE;
    data.leaf_type = 0;
    data.plane_index = NONE;
    bool hit = collision_bsp_test_vector_recursive(&data, 0, 0.0f, PIN(max_t, 0.0f, 1.0f));
    return hit;
}

// @address 0x2b2f50 ?collision_bsp_test_vector_recursive@@YA_NPAUtest_vector_data@@JMM@Z
bool collision_bsp_test_vector_recursive(test_vector_data *data, long node_index, float t0, float t1)
{
    while (!(node_index & (1 << 23)))
    {
        const bsp3d_node *node = &((const bsp3d_node *)data->bsp->bsp3d_nodes.address)[node_index & 0x7fffff];
        const real_plane3d *plane = &((const real_plane3d *)data->bsp->planes.address)[(short)node->plane];
// match: 0x2b2f50: keeps the plane-distance call at 0x2b2fac before the two segment distances.
#pragma inline_depth(0)
        float distance = plane3d_distance_to_point(plane, data->point);
        float dot = dot_product3d(data->vector, &plane->normal);
        float distance0 = distance + dot * t0;
        float distance1 = distance + dot * t1;
        bool front0 = distance0 >= 0.0f;
        bool front1 = distance1 >= 0.0f;

        if (front0 != front1)
        {
            bool front = dot > 0.0f;
            float t = -distance / dot;

            if (collision_bsp_test_vector_recursive(data, collision_bsp_bsp3d_node_get_child_index(node, !front), t0, t))
                return true;

            if (data->result->fraction <= t)
                return false;

            data->plane_index = (short)node->plane;
            node_index = collision_bsp_bsp3d_node_get_child_index(node, front);
            t0 = t;
        }
        else
        {
            node_index = collision_bsp_bsp3d_node_get_child_index(node, front0);
        }
    }

    long leaf_index = -1;
    unsigned char leaf_type = 3;
    long test_leaf_index = -1;
    bool test_all_surfaces = false;

    if (node_index != -1)
    {
        leaf_index = node_index & 0x7fffff;
        leaf_type = (((const collision_leaf *)data->bsp->leaves.address)[leaf_index].flags & 1) ? 2 : 1;
    }

    if ((data->flags & 1) && (data->leaf_type == 1 || data->leaf_type == 2) && leaf_type == 3)
    {
        test_leaf_index = data->leaf_index;
    }
    else if ((data->flags & 2) && data->leaf_type == 3 && (leaf_type == 1 || leaf_type == 2))
    {
        test_leaf_index = leaf_index;
    }
    else if (!(data->flags & 4) && data->leaf_type == 2 && leaf_type == 2)
    {
        test_leaf_index = (data->flags & 1) ? data->leaf_index : leaf_index;
        test_all_surfaces = true;
    }

    if (test_leaf_index != -1)
    {
        long surface_index = collision_leaf_test_vector(data->bsp, data->materials, data->breakable_surface_set_index, data->breakable_surface_set,
            data->point, data->vector, test_leaf_index, data->plane_index, t0, test_all_surfaces);
        if (surface_index != -1)
        {
            const collision_surface *surface = &((const collision_surface *)data->bsp->surfaces.address)[surface_index];
            if ((!(surface->flags & 2) || !(data->flags & 8)) && (!(surface->flags & 8) || !(data->flags & 16)))
            {
                data->result->fraction = t0;
                data->result->plane = &((const real_plane3d *)data->bsp->planes.address)[data->plane_index];
                data->result->leaf_index = test_leaf_index;
                data->result->surface_index = surface_index;
                data->result->plane_index = surface->plane;
                data->result->flags = surface->flags;
                data->result->breakable_surface_bit = (char)surface->breakable_surface_bit;
                data->result->breakable_surface = (char)surface->breakable_surface;
                data->result->material = surface->material;
                return true;
            }
        }
    }

    if (leaf_index != -1)
    {
        if (data->result->leaf_count >= 0 && data->result->leaf_count < 256)
        {
            data->result->leaf_indices[data->result->leaf_count] = leaf_index;
            data->result->leaf_count++;
        }
        else
        {
            data->result->leaf_indices[255] = leaf_index;
        }
    }

    data->leaf_index = leaf_index;
    data->leaf_type = leaf_type;
    return false;
}

// @address 0x2b3340 ?collision_leaf_test_vector@@YAJPBUcollision_bsp@@PBUs_collision_materials_extant_flags@@FPBUs_breakable_surface_set@@PBTreal_point3d@@PBTreal_vector3d@@JJM_N@Z
long collision_leaf_test_vector(const collision_bsp *bsp, const s_collision_materials_extant_flags *materials,
    short breakable_surface_set_count, const s_breakable_surface_set *breakable_surface_sets, const real_point3d *point,
    const real_vector3d *vector, long leaf_index, long plane_index, float t, bool test_surface)
{
    const collision_leaf *leaf = &((const collision_leaf *)bsp->leaves.address)[leaf_index];
    for (unsigned long i = leaf->first_reference_index; i < leaf->first_reference_index + leaf->reference_count; i++)
    {
        const collision_reference *reference = &((const collision_reference *)bsp->bsp2d_references.address)[i];
        if ((reference->plane_index & 0x7fff) == plane_index)
        {
            const real_plane3d *plane = &((const real_plane3d *)bsp->planes.address)[plane_index];
            short projection = projection_from_vector3d(&plane->normal);
            bool sign = projection_sign_from_vector3d(&plane->normal, projection) != ((reference->plane_index & 0x8000) != 0);
            real_point3d intersection;
            real_point2d projected;
            point_from_line3d(point, vector, t, &intersection);
            project_point3d(&intersection, projection, sign, &projected);
            long surface_index = bsp2d_test_point((const bsp2d *)&bsp->bsp2d_nodes, &projected, reference->node_index);
            if (collision_bsp_surface_is_extant(bsp, materials, surface_index))
            {
                if (!test_surface ||
                    collision_surface_test_point(
                        bsp, breakable_surface_set_count, breakable_surface_sets, surface_index, projection, sign, &projected))
                    return surface_index;
            }
        }
    }
    return -1;
}
// @address 0x2b35a0 ?collision_surface_test_point@@YA_NPBUcollision_bsp@@FPBUs_breakable_surface_set@@JF_NPBTreal_point2d@@@Z
bool collision_surface_test_point(const collision_bsp *bsp, short breakable_surface_set_count,
    const s_breakable_surface_set *breakable_surface_sets, long surface_index, short projection, bool sign, const real_point2d *point)
{
    const collision_surface *surface = &((const collision_surface *)bsp->surfaces.address)[surface_index];
    if (breakable_surface_sets && (surface->flags & 8) &&
        surface->breakable_surface >= 0 && surface->breakable_surface < breakable_surface_set_count &&
        !breakable_surface_sets[surface->breakable_surface].surface_flags.test(surface->breakable_surface_bit))
        return false;
    long first_edge_index = surface->first_edge;
    long edge_index = first_edge_index;
    do {
        const collision_edge *edge = &((const collision_edge *)bsp->edges.address)[edge_index];
        bool side = edge->surfaces[1] == surface_index;
        const collision_vertex *vertex0 = &((const collision_vertex *)bsp->vertices.address)[edge->vertices[side]];
        const collision_vertex *vertex1 = &((const collision_vertex *)bsp->vertices.address)[edge->vertices[!side]];
        real_point2d point0, point1;
        project_point3d(&vertex0->point, projection, sign, &point0);
        project_point3d(&vertex1->point, projection, sign, &point1);
        real_vector2d point_vector, edge_vector;
        vector_from_points2d(&point0, point, &point_vector);
        vector_from_points2d(&point0, &point1, &edge_vector);
        if (cross_product2d(&point_vector, &edge_vector) > 0.0f)
            return false;
        edge_index = edge->edges[side];
    } while (edge_index != first_edge_index);
    return true;
}
