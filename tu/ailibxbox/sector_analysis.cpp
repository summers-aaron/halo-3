// ailibxbox/sector_analysis.obj: from the records 0x56a6b8 0x56a748 0x56aa10 0x56ad78
// @flags /TP /O2 /Oc
#include "math/real_math_types.h"
#include "ai/firing_position_ref.h"
#include "math/real_math.h"
#include "math/matrix_math_types.h"
// @address 0x56a6b8 ?firing_position_get_normal@@YA_NPAUfiring_position_definition@@PATreal_vector3d@@@Z
#include <ppcintrinsics.h>
#include <math.h>
#include <stdlib.h>

#define COPY_PROVENANCE "Typed default-vector copy from the retail listing"
#include "ai/ai_hint_definitions.h"
#include "ai/ai_reference_frame.h"
#include "ai/sector_analysis.h"
#include "math/matrix_math.h"
#include "ai/sector.h"
#include "memory/tls_globals.h"
#include "ai/pathfinding_collision.h"
#include "ai/firing_position_accessors.h"


#include "ai/firing_position_definition.h"
#include "ai/firing_position_ref.h"
#include "ai/sector_ref.h"
#include "ai/actor_datum.h"
#include "ai/pathfinding_geometry.h"
#include "ai/pathfinding_data.h"
#include "memory/data.h"

bool firing_position_get_normal(firing_position_definition *position, real_vector3d *normal)
{
    bool result;
    real_vector3d untransformed_normal;
    if (!((position->flags & 128) == 0)) {
        result = 1;
        *normal = *global_up3d;
    } else {
        vector3d_from_euler_angles2d(&untransformed_normal, &position->normal);
        result = ai_reference_frame_transform_vector(position->point.reference_frame(), &untransformed_normal, normal);
    }
    return result;
}



static __forceinline float sector_analysis_alignment_dot(const real_vector3d *a, const real_vector3d *b)
{
    return (a->k * b->k + a->i * b->i) + a->j * b->j;
}
static __forceinline float sector_analysis_side_dot(const real_vector3d *a, const real_vector3d *b)
{
    return (a->k * b->k + a->j * b->j) + a->i * b->i;
}
static __forceinline float sector_analysis_accumulate_product(const float &sum, float value, float scale)
{
    return value * scale + sum;
}
// @address 0x56aa10 ?runtime_cornering_get_info@@YA_NPAUpathfinding_data@@JPBVc_ai_point3d@@PATreal_vector3d@@PATreal_point3d@@2PAD@Z
bool runtime_cornering_get_info(pathfinding_data *data, long link_index, const c_ai_point3d *ai_point, real_vector3d *direction,
    real_point3d *point, real_vector3d *out_normal, char *out_corner)
{
    bool result = false;
    if (link_index < data->links.count)
    {
        sector_link *link = &((sector_link *)data->links.address)[link_index];
        real_point3d *end_vertex = &data->vertices[link->end_vertex];
        real_point3d *start_vertex = &data->vertices[link->start_vertex];
        real_vector3d facing;
        real_vector3d normal;
        bool valid = false;
        float delta_x = end_vertex->x - start_vertex->x;
        float delta_y = end_vertex->y - start_vertex->y;
        normal.i = delta_y;
        normal.j = -delta_x;
        normal.k = 0.0f;
        if (normalize3d(&normal) > 0.0f)
            valid = true;
        if (valid)
        {
            bool facing_valid = true;
            if (direction)
            {
                ai_reference_frame_inverse_transform_vector(ai_point->reference_frame(), direction, &facing);
                float facing_dot = sector_analysis_alignment_dot(&facing, &normal);
                if (facing_dot < 0.0f)
                    facing_valid = false;
            }
            if (facing_valid)
            {
                if (out_normal)
                    *out_normal = normal;
                link = &((sector_link *)data->links.address)[link_index];
                start_vertex = &data->vertices[link->start_vertex];
                end_vertex = &data->vertices[link->end_vertex];
                float start_x = start_vertex->x;
                float start_y = start_vertex->y;
                float start_z = start_vertex->z;
                float distance = 0.0f;
                real_vector3d edge;
                edge.i = end_vertex->x - start_x;
                edge.j = end_vertex->y - start_y;
                edge.k = end_vertex->z - start_z;
                float length = normalize3d(&edge);
                if (length > 0.0f)
                {
                    real_vector3d relative;
                    relative.i = ai_point->point.x - start_x;
                    relative.j = ai_point->point.y - start_y;
                    relative.k = ai_point->point.z - start_z;
                    distance = dot_product3d(&relative, &edge);
                    float pinned_distance;
                    if (distance > 0.0f)
                        pinned_distance = distance;
                    else
                        pinned_distance = 0.0f;
                    if (pinned_distance < length)
                    {
                        if (distance <= 0.0f)
                            distance = 0.0f;
                    }
                    else
                    {
                        distance = length;
                    }
                    if (out_corner)
                        *out_corner = 0;
                    real_vector3d side_vector;
                    side_vector.i = normal.j;
                    side_vector.j = -normal.i;
                    side_vector.k = normal.k;
                    bool check_start;
                    bool check_end;
                    if (direction)
                    {
                        float side = sector_analysis_accumulate_product(facing.k * side_vector.k, facing.j, side_vector.j)
                            + facing.i * side_vector.i;
                        if (side > 0.0f)
                        {
                            check_end = false;
                            check_start = true;
                        }
                        else
                        {
                            check_start = false;
                            check_end = true;
                        }
                    }
                    else
                    {
                        check_end = true;
                        check_start = true;
                    }
                    if (check_start)
                    {
                        if (distance < 0.4f && (link->flags & 0x400) != 0)
                        {
                            result = true;
                            distance = 0.0f;
                        }
                        if (result && out_corner)
                            *out_corner = 1;
                    }
                    if (check_end)
                    {
                        if (length - distance < 0.4f && (link->flags & 0x800) != 0)
                        {
                            result = true;
                            distance = length;
                        }
                        if (result && out_corner)
                            *out_corner = 2;
                    }
                    real_point3d closest;
                    closest.x = edge.i * distance + start_x;
                    closest.y = edge.j * distance + start_y;
                    closest.z = edge.k * distance + start_z;
                    if (point)
                        *point = closest;
                }
            }
        }
    }
    return result;
}
static __forceinline bool sector_analysis_root_entry_point(
    long actor_index, long entry, const real_point3d *point, const real_vector3d *direction, real_point3d *out)
{
    bool result = false;
    if (actor_index != -1)
    {
        actor_datum *actor = datum_get<actor_datum>(tls_actor_data(), actor_index);
        real_matrix4x3 root;
        if (ai_hint_find_root_entry_matrix(actor->unit_index, entry, 84, point, direction, &root))
        {
            if (out)
                *out = root.position;
            result = true;
        }
    }
    else
    {
        if (out)
            *out = *point;
        result = true;
    }
    return result;
}
// @rdata 0x820271bc
static const float sector_analysis_ray_length = 0.35f;
static __forceinline bool sector_analysis_get_forward(firing_position_definition *position, real_vector3d *forward)
{
    bool result = false;
    real_vector3d normal;
    if (firing_position_get_normal(position, &normal))
    {
        real_matrix4x3 rotation;
        real_matrix4x3 facing;
        matrix4x3_rotation_between_vectors(&rotation, global_up3d, &normal);
        float cosine = (float)cos(position->facing);
        float sine = (float)sin(position->facing);
        matrix4x3_rotation_from_axis_and_angle(&facing, global_up3d, sine, cosine);
        matrix4x3_compute_product(&rotation, &facing, &rotation);
        *forward = rotation.forward;
        result = true;
    }
    return result;
}
// @address 0x56a748 ?runtime_manual_cornering_get_info@@YA_NJUfiring_position_ref@@PAVc_ai_point3d@@PATreal_vector3d@@PAD@Z
bool runtime_manual_cornering_get_info(long actor_index, firing_position_ref reference, c_ai_point3d *point, real_vector3d *direction, char *cornering_type)
{
    bool result = false;
    firing_position_definition *position = firing_position_ref_get_definition(reference);
    real_vector3d forward;
    if (actor_index != -1 && position->cornering_flags != 0 && sector_analysis_get_forward(position, &forward))
    {
        actor_datum *actor = datum_get<actor_datum>(tls_actor_data(), actor_index);
        short type = -1;
        forward.k = 0.0f;
        if (actor->unit_index != -1 && normalize3d(&forward) > 0.0f)
        {
            ai_reference_frame_inverse_transform_vector(position->point.reference_frame(), &forward, &forward);
            path_collision_result collision;
            pathfinding_test_vector(position->point.get_point(), position->sector, g_sector_none, &forward, sector_analysis_ray_length, 0, &collision);
            real_point3d corner;
            if (position->cornering_flags & 4)
            {
                corner = collision.point;
                type = 3;
                result = true;
            }
            else
            {
                float offset = 0.0f;
                if (position->cornering_flags & 1)
                {
                    type = 1;
                    offset = -0.2f;
                }
                else if (position->cornering_flags & 2)
                {
                    type = 2;
                    offset = 0.2f;
                }
                real_vector3d up;
                if (ai_reference_frame_inverse_transform_vector(position->point.reference_frame(), global_up3d, &up))
                {
                    real_vector3d lateral;
                    cross_product3d(&up, &forward, &lateral);
                    point_from_line3d(&collision.point, &lateral, offset, &corner);
                    result = true;
                }
            }
            if (result)
            {
                if (point)
                    point->set(&corner, position->point.reference_frame(), position->point.bsp_index());
                if (direction)
                    *direction = forward;
                if (cornering_type)
                    *cornering_type = (char)type;
            }
        }
    }
    return result;
}
// @address 0x56ad78 ?runtime_cornering_get_entry@@YA_NJPBVc_ai_point3d@@Vc_sector_ref@@0PBTreal_vector3d@@F_NPAV1@PAV2@@Z
bool runtime_cornering_get_entry(long actor_index, const c_ai_point3d *search, c_sector_ref search_sector, const c_ai_point3d *marker,
    const real_vector3d *direction, short type, bool alternate, c_ai_point3d *out, c_sector_ref *out_sector)
{
    long entry = -1;
    bool result = false;
    if (out_sector)
        *out_sector = g_sector_none;
    switch (type)
    {
    case 1: entry = 260 + (alternate ? 12 : 0); break;
    case 2: entry = 261 + (alternate ? 12 : 0); break;
    case 3: entry = 726 + (alternate ? 1 : 0); break;
    }
    if (entry != -1)
    {
        real_point3d point;
        if (sector_analysis_root_entry_point(actor_index, entry, marker->get_point(), direction, &point))
        {
            point.z = marker->get_point()->z;
            c_sector_ref found;
            if (sector_from_point_and_sector(search->get_point(), search_sector, &point, &found))
            {
                if (out)
                    out->set(&point, search->reference_frame());
                if (out_sector)
                    *out_sector = found;
                result = true;
            }
        }
    }
    return result;
}
