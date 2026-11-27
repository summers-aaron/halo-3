// hkinternal/hkBoxBoxCollisionDetection.obj: from the records 0x694910 0x694938 0x694a00 0x694aa0 0x694dc0 0x694dd8 0x694f20 0x694fb8 0x695008 0x695730 0x6959f0 0x695e00 0x696038 0x696b80 0x696e00
// @compiler xdk5632
// @flags /TP /O2 /Oc
// Stack arrays use hkVector4 without user constructors.
#define PHYS_DISABLE_VEC4_CONSTRUCTORS
#include "phys/phys_base_types.h"
#include "phys/phys_box_box_detection.h"
#include "phys/phys_box_box_detector.h"
#include "phys/phys_vec4_util.h"
// @address 0x694910 ?normal_sign_from_features@@YAMG@Z
// @address 0x694938 ?set_terminals_from_vertex@@YAXAAHHH@Z
// @address 0x694dc0 ?get_axis_map_from_feature_bits@@YAABVphys_vec4@@G@Z
// @address 0x694fb8 ?max_plane_mask3@@YAHABVphys_vec4@@0AAH@Z
#include <math.h>
#include <ppcintrinsics.h>
#include <vectorintrinsics.h>







#include "phys/phys_scalar_types.h"
#include "phys/phys_collision_proxy.h"
#include "phys/phys_collide.h"
#include "phys/phys_collision_input.h"
#include "phys/phys_constraint_owner.h"
#include "phys/phys_math.h"






float normal_sign_from_features(unsigned short feature_bits)
{
    float normal_sign = 1.0f;
    unsigned int *sign_bits_ptr = (unsigned int *)(&normal_sign);
    *sign_bits_ptr |= (feature_bits << 28) & 0x80000000;
    return normal_sign;
}

void set_terminals_from_vertex(int &terminals, int vertex_idx, int terminal_idx)
{
    int axis = vertex_idx >> 4;
    unsigned int x_terminal = (axis & 8) == 0;
    unsigned int y_terminal = (axis & 4) == 0;
    unsigned int z_terminal = (axis & 2) == 0;
    if (terminal_idx != 0) {
        int mask = 1 << ((z_terminal * 2 + y_terminal) * 2);
        if (((mask << 1) & terminals) == 0)
            terminals += mask;
    }
    if (terminal_idx != 1) {
        int mask = 1 << (((z_terminal + 2) * 2 + x_terminal) * 2);
        if (((mask << 1) & terminals) == 0)
            terminals += mask;
    }
    if (terminal_idx != 2) {
        int mask = 1 << (((y_terminal + 4) * 2 + x_terminal) * 2);
        if (((mask << 1) & terminals) == 0)
            terminals += mask;
    }
}


int max_plane_mask3(const phys_vec4 &v, const phys_vec4 &unused, int &axis)
{
    int index;
    int mask;
    if (v(0) > v(1)) {
        index = 0;
        mask = 128;
    } else {
        index = 1;
        mask = 64;
    }
    if (v(2) > v(index)) {
        index = 2;
        mask = 32;
    }
    axis = index;
    return mask;
}

// @address 0x694a00 ?set_terminals_from_edge@@YAXAAHH@Z
void set_terminals_from_edge(int &edge_terminal_bits, int edge_bits)
{
    int edge_axis = edge_bits & 0x000f;
    edge_bits = edge_bits >> 4;
    int x_factor = (edge_bits & 8) ? 0 : 1;
    int y_factor = (edge_bits & 4) ? 0 : 1;
    int z_factor = (edge_bits & 2) ? 0 : 1;
    if (edge_axis == 0) {
        const int add_bit = 1 << ((y_factor + 2 * z_factor) * 2);
        if (((add_bit << 1) & edge_terminal_bits) == 0)
            edge_terminal_bits += add_bit;
    } else if (edge_axis == 1) {
        const int add_bit = 1 << ((4 + x_factor + 2 * z_factor) * 2);
        if (((add_bit << 1) & edge_terminal_bits) == 0)
            edge_terminal_bits += add_bit;
    } else {
        const int add_bit = 1 << ((8 + x_factor + 2 * y_factor) * 2);
        if (((add_bit << 1) & edge_terminal_bits) == 0)
            edge_terminal_bits += add_bit;
    }
}



static __forceinline void phys_box_box_terminals_from_vertex(int &terminals, int vertex_idx, int terminal_idx)
{
    int axis = vertex_idx >> 4;
    unsigned int x_terminal = (axis & 8) == 0;
    unsigned int y_terminal = (axis & 4) == 0;
    unsigned int z_terminal = (axis & 2) == 0;
    if (terminal_idx != 0) {
        int mask = 1 << ((z_terminal * 2 + y_terminal) * 2);
        if (((mask << 1) & terminals) == 0)
            terminals += mask;
    }
    if (terminal_idx != 1) {
        int mask = 1 << (((z_terminal + 2) * 2 + x_terminal) * 2);
        if (((mask << 1) & terminals) == 0)
            terminals += mask;
    }
    if (terminal_idx != 2) {
        int mask = 1 << (((y_terminal + 4) * 2 + x_terminal) * 2);
        if (((mask << 1) & terminals) == 0)
            terminals += mask;
    }
}

static __forceinline void phys_box_box_terminals_from_edge(int &edge_terminal_bits, int edge_bits)
{
    int edge_axis = edge_bits & 0x000f;
    edge_bits = edge_bits >> 4;
    int x_factor = (edge_bits & 8) ? 0 : 1;
    int y_factor = (edge_bits & 4) ? 0 : 1;
    int z_factor = (edge_bits & 2) ? 0 : 1;
    if (edge_axis == 0) {
        const int add_bit = 1 << ((y_factor + 2 * z_factor) * 2);
        if (((add_bit << 1) & edge_terminal_bits) == 0)
            edge_terminal_bits += add_bit;
    } else if (edge_axis == 1) {
        const int add_bit = 1 << ((4 + x_factor + 2 * z_factor) * 2);
        if (((add_bit << 1) & edge_terminal_bits) == 0)
            edge_terminal_bits += add_bit;
    } else {
        const int add_bit = 1 << ((8 + x_factor + 2 * y_factor) * 2);
        if (((add_bit << 1) & edge_terminal_bits) == 0)
            edge_terminal_bits += add_bit;
    }
}



typedef char phys_box_box_contact_points_offset[((unsigned int)&((phys_process_collision_output *)0)->m_process_contact_points == 0x10) ? 1 : -1];


const float phys_box_box_detector::m_manifold_cos_tolerance = 0.95f;
const float phys_box_box_detector::m_coplanar_ang_cos_tolerance = 0.98f;
const float phys_box_box_detector::m_normal_ang_cos_tolerance = 0.95f;
const float phys_box_box_detector::m_edge_end_tolerance = 1.192092896e-07f;
const int phys_box_box_detector::m_max_rejected_features = 8;
const int phys_box_box_util_mods[9] = { 0, 1, 2, 0, 1, 2, 0, 1, 2 };
static __declspec(align(16)) const float vertex_sign_table[8][4] = {
    { 1.0f, 1.0f, 1.0f, 0.0f },
    { 1.0f, 1.0f, -1.0f, 0.0f },
    { 1.0f, -1.0f, 1.0f, 0.0f },
    { 1.0f, -1.0f, -1.0f, 0.0f },
    { -1.0f, 1.0f, 1.0f, 0.0f },
    { -1.0f, 1.0f, -1.0f, 0.0f },
    { -1.0f, -1.0f, 1.0f, 0.0f },
    { -1.0f, -1.0f, -1.0f, 0.0f }
};
static const unsigned short cycle_mask[7] = { 0xa0, 0xc0, 0x60, 0x80, 0x40, 0x20, 0 };

const phys_vec4 &get_axis_map_from_feature_bits(unsigned short feature_bits)
{
    int index = (feature_bits & 0x00e0) >> 1;
    return *phys_add_byte_offset_const<phys_vec4>((const phys_vec4 *)vertex_sign_table, index);
}

__forceinline void phys_box_box_detector::face_a_vertex_b_contact(
    phys_process_hit_point &contact_hit_out, const phys_feature_contact_point &feature_contact, const phys_feature_point_cache &feature_point_cache) const
{
    const int feature_idx = feature_contact.m_feature_a;
    contact_hit_out.m_process_contact_id = feature_contact.m_process_contact_id;
    contact_hit_out.m_hit_contact.get_contact_position().set_transformed_pos(m_world_from_b, feature_point_cache.m_vec_b);
    contact_hit_out.m_hit_contact.get_separating_normal().set_mul4(phys_simd_scalar(-feature_point_cache.m_normal_sign),
        m_world_from_a.get_rotation().get_column(feature_idx));
    contact_hit_out.m_hit_contact.contact_set_distance(feature_point_cache.m_distance);
}

__forceinline void phys_box_box_detector::face_b_vertex_a_contact(
    phys_process_hit_point &contact_hit_out, const phys_feature_contact_point &feature_contact, const phys_feature_point_cache &feature_point_cache) const
{
    const int feature_idx = feature_contact.m_feature_a - PHYS_BOX_BOX_FACE_B_X;
    contact_hit_out.m_process_contact_id = feature_contact.m_process_contact_id;
    contact_hit_out.m_hit_contact.get_contact_position().set_transformed_pos(m_world_from_a, feature_point_cache.m_vec_a);
    contact_hit_out.m_hit_contact.get_separating_normal().set_mul4(phys_simd_scalar(-feature_point_cache.m_normal_sign),
        m_world_from_b.get_rotation().get_column(feature_idx));
    contact_hit_out.m_hit_contact.contact_set_distance(feature_point_cache.m_distance);
}

__forceinline void phys_box_box_detector::edge_edge_contact_from_cache(
    phys_process_hit_point &contact_hit_out, const phys_feature_contact_point &feature_contact, const phys_feature_point_cache &feature_point_cache) const
{
    contact_hit_out.m_process_contact_id = feature_contact.m_process_contact_id;
    contact_hit_out.m_hit_contact.get_contact_position().set_transformed_pos(m_world_from_a, feature_point_cache.m_vec_a);
    contact_hit_out.m_hit_contact.get_separating_normal().set_rotated_dir(m_world_from_a.get_rotation(), feature_point_cache.m_normal_a);
    contact_hit_out.m_hit_contact.contact_set_distance(feature_point_cache.m_distance);
}

__forceinline void phys_box_box_detector::contact_from_feature_cache(
    phys_process_hit_point &contact_hit_out, const phys_feature_contact_point &feature_contact, const phys_feature_point_cache &feature_point_cache) const
{
    const int sep_feature_idx = feature_point_cache.m_feature_idx_a;
    if (sep_feature_idx <= PHYS_BOX_BOX_FACE_A_Z)
    {
        face_a_vertex_b_contact(contact_hit_out, feature_contact, feature_point_cache);
    }
    else if (sep_feature_idx <= PHYS_BOX_BOX_FACE_B_Z)
    {
        face_b_vertex_a_contact(contact_hit_out, feature_contact, feature_point_cache);
    }
    else
    {
        edge_edge_contact_from_cache(contact_hit_out, feature_contact, feature_point_cache);
    }
}

__forceinline phys_result phys_box_box_detector::add_manifold_point(phys_box_box_manifold &box_manifold,
    const phys_feature_point_cache &feature_point_cache, phys_feature_contact_point &feature_contact) const
{
    if (feature_contact.m_feature_a <= PHYS_BOX_BOX_FACE_B_Z)
    {
        phys_vec4 candidate_vec;
        if (feature_contact.m_feature_a <= PHYS_BOX_BOX_FACE_A_Z)
            candidate_vec.set_transformed_pos(m_world_from_b, feature_point_cache.m_vec_b);
        else
            candidate_vec.set_transformed_pos(m_world_from_a, feature_point_cache.m_vec_a);

        int i;
        for (i = 0; i < box_manifold.get_manifold_point_count(); i++)
        {
            if (box_manifold[i].m_feature_a <= PHYS_BOX_BOX_FACE_B_Z)
            {
                const phys_process_hit_point &contact_hit = m_result->m_process_contact_points[i];
                phys_vec4 diff;
                diff.set_sub4(candidate_vec, contact_hit.m_hit_contact.get_contact_position());
                const float delta = diff.dot3(diff);
                if (delta <= m_box_tolerance * m_box_tolerance + 1.1920929e-07f)
                    return PHYS_FAILURE;
            }
        }
    }

    if (box_manifold.has_no_points_left())
        return PHYS_FAILURE;

    int fcp_idx = box_manifold.add_manifold_point(m_body_a, m_body_b, feature_contact);
    if (fcp_idx >= 0)
    {
        phys_process_hit_point &contact_hit = *m_result->reserve_contact_points(1);
        contact_from_feature_cache(contact_hit, feature_contact, feature_point_cache);

        if (box_manifold.get_manifold_point_count() > 1)
        {
            const phys_process_hit_point &existing_hit = m_result->get_track_end()[-1];
            if (existing_hit.m_hit_contact.get_contact_normal().dot3(contact_hit.m_hit_contact.get_contact_normal()) <= 0.0f)
            {
                m_result->abort_contact_points(1);
                box_manifold.bridge_remove_point(fcp_idx);
                return PHYS_FAILURE;
            }
        }

        box_manifold[fcp_idx].m_process_contact_id = m_contact_manager->add_manifold_contact_point(m_body_a, m_body_b, *m_collide_env,
            *m_result, 0, contact_hit.m_hit_contact);

        if (box_manifold[fcp_idx].m_process_contact_id == 0xffff)
        {
            m_result->abort_contact_points(1);
            box_manifold.bridge_remove_point(fcp_idx);
        }
        else
        {
            m_result->commit_contact_points(1);
            feature_contact.m_process_contact_id = box_manifold[fcp_idx].m_process_contact_id;
            contact_hit.m_process_contact_id = box_manifold[fcp_idx].m_process_contact_id;
            if (feature_contact.m_feature_a <= PHYS_BOX_BOX_FACE_B_Z)
                box_manifold.m_face_contact_count++;
        }
    }
    return PHYS_SUCCESS;
}

// @address 0x694aa0 ?check_manifold_completeness@phys_box_box_detector@@IBAXAAVphys_box_box_manifold@@HH@Z
void phys_box_box_detector::check_manifold_completeness(phys_box_box_manifold &box_manifold, int plane_mask_a, int plane_mask_b) const
{
    if (box_manifold.get_manifold_point_count() < 3)
        return;

    if (box_manifold.m_face_contact_count >= 4)
    {
        box_manifold.set_complete(true);
        return;
    }

    int plane_axis_a, plane_axis_b;
    plane_mask_a = plane_mask_a >> 4;
    if (plane_mask_a & 8)
        plane_axis_a = 0;
    else if (plane_mask_a & 4)
        plane_axis_a = 1;
    else
        plane_axis_a = 2;

    plane_mask_b = plane_mask_b >> 4;
    if (plane_mask_b & 8)
        plane_axis_b = 0;
    else if (plane_mask_b & 4)
        plane_axis_b = 1;
    else
        plane_axis_b = 2;

    int edge_terminals_a = 0;
    int edge_terminals_b = 0;
    {
        int i;
        for (i = 0; i < box_manifold.get_manifold_point_count(); i++)
        {
            const phys_feature_contact_point &manifold_fcp = box_manifold[i];

            if (manifold_fcp.m_feature_a <= 2)
            {
                phys_box_box_terminals_from_vertex(edge_terminals_b, manifold_fcp.m_feature_b, plane_axis_b);
            }
            else if (manifold_fcp.m_feature_a <= 6)
            {
                phys_box_box_terminals_from_vertex(edge_terminals_a, manifold_fcp.m_feature_b, plane_axis_a);
            }
            else
            {
                phys_box_box_terminals_from_edge(edge_terminals_a, manifold_fcp.m_feature_a);
                phys_box_box_terminals_from_edge(edge_terminals_b, manifold_fcp.m_feature_b);
            }
        }
    }

    phys_bool has_terminal_bits = ((edge_terminals_a & 0x555555) > 0) || ((edge_terminals_b & 0x555555) > 0);
    box_manifold.set_complete(!has_terminal_bits);
}

// @address 0x694dd8 ?set_edge_feature_bits_from_axis_map@@YAXABVphys_vec4@@0HHAAVphys_feature_contact_point@@@Z
void set_edge_feature_bits_from_axis_map(const phys_vec4 &axis_map_a_in, const phys_vec4 &axis_map_b_in, int feature_index_a,
    int feature_index_b, phys_feature_contact_point &feature_contact)
{
    phys_vec4 axis_map_b = axis_map_b_in;
    phys_vec4 axis_map_a = axis_map_a_in;
    axis_map_b(feature_index_b) = 1.0f;
    axis_map_a(feature_index_a) = 1.0f;

    int sign_mask_b = axis_map_b.compare_less_zero4().get_mask(phys_vec4_cmp::CMP_MASK_XYZ) ^ phys_vec4_cmp::CMP_MASK_XYZ;
    int sign_mask_a = axis_map_a.compare_less_zero4().get_mask(phys_vec4_cmp::CMP_MASK_XYZ);

    feature_contact.m_feature_a = (unsigned char)((phys_vec4_cmp::CMP_MASK_W << 4) | feature_index_a | (sign_mask_a << 4));
    feature_contact.m_feature_b = (unsigned char)(feature_index_b + (sign_mask_b << 4));
}


// @address 0x695730 ?is_valid_edge_edge@phys_box_box_detector@@IBA?AVphys_bool@@AAVphys_feature_point_cache@1@@Z
phys_bool phys_box_box_detector::is_valid_edge_edge(phys_feature_point_cache &feature_point_cache) const
{
    const int axis_dim_a = feature_point_cache.m_feature_idx_a & 0xf;
    const phys_vec4 &axis_signs_a = get_axis_map_from_feature_bits(feature_point_cache.m_feature_idx_a);

    phys_vec4 vertex_a_in_a;
    vertex_a_in_a.set_mul4(axis_signs_a, m_radius_a);

    const int axis_dim_b = feature_point_cache.m_feature_idx_b & 0xf;
    const phys_vec4 &axis_signs_b = get_axis_map_from_feature_bits(feature_point_cache.m_feature_idx_b);

    phys_vec4 vertex_b_in_a;
    vertex_b_in_a.set_mul4(axis_signs_b, m_radius_b);

    phys_vec4 edge_b_local;
    edge_b_local.set_mul4(phys_simd_scalar(m_radius_b(axis_dim_b) * 2.0f), m_a_to_b.get_column(axis_dim_b));

    vertex_b_in_a.set_transformed_pos(m_a_to_b, vertex_b_in_a);
    phys_vec4 d12;
    d12.set_sub4(vertex_b_in_a, vertex_a_in_a);

    const float d1_at_dim_a = m_radius_a(axis_dim_a) * -2.0f;

    const float R = d1_at_dim_a * edge_b_local(axis_dim_a);
    const float D1 = d1_at_dim_a * d1_at_dim_a;
    float D2 = m_radius_b(axis_dim_b) * m_radius_b(axis_dim_b) * 4.0f;

    const float S1 = d1_at_dim_a * d12(axis_dim_a);
    const float S2 = edge_b_local.dot3(d12);

    float box_denominator = (float)fabs(D1 * D2 - R * R);

    float t = S1 * D2 - S2 * R;
    if (t <= m_edge_end_tolerance * box_denominator || t >= (1.0f - m_edge_end_tolerance) * box_denominator)
    {
        return false;
    }

    t /= box_denominator;

    const float u = t * R - S2;
    if (u <= m_edge_end_tolerance * D2 || u >= (1.0f - m_edge_end_tolerance) * D2)
    {
        return false;
    }

    vertex_a_in_a(axis_dim_a) += d1_at_dim_a * t;

    phys_vec4 edge_a_local;
    edge_a_local.set_zero4();
    edge_a_local(axis_dim_a) = d1_at_dim_a * 0.5f;

    phys_vec4 unit_normal_in_a;
    unit_normal_in_a.set_cross(edge_a_local, edge_b_local);

    const float normal_length = unit_normal_in_a.normalize_xyz_with_length();
    if (normal_length < m_boundary_tolerance)
    {
        return false;
    }

    const float check = unit_normal_in_a.dot3(axis_signs_a);
    if (check < 0.0f)
    {
        unit_normal_in_a.set_neg4(unit_normal_in_a);
    }

    const float out_depth = unit_normal_in_a.dot3(d12);
    if (out_depth > m_box_tolerance)
    {
        return false;
    }

    feature_point_cache.m_vec_a = vertex_a_in_a;
    feature_point_cache.m_normal_a.set_neg4(unit_normal_in_a);
    feature_point_cache.m_distance = out_depth;
    return true;
}

// @address 0x6959f0 ?add_extra_edge_helper@phys_box_box_detector@@IBAXAAVphys_box_box_manifold@@AAVphys_feature_contact_point@@M@Z
void phys_box_box_detector::add_extra_edge_helper(phys_box_box_manifold &box_manifold, phys_feature_contact_point &feature_contact,
    float closest_point_dist) const
{
    if (!box_manifold.find_in_manifold(feature_contact))
    {
        phys_feature_point_cache feature_point_cache;

        edge_edge_validate_from_id(feature_point_cache, feature_contact);
        if (is_valid_edge_edge(feature_point_cache))
        {
            if (feature_point_cache.m_distance >= closest_point_dist * 0.999f)
            {
                add_manifold_point(box_manifold, feature_point_cache, feature_contact);
            }
        }
    }
}

static __forceinline void phys_box_box_set_edge_feature_bits(
    const phys_vec4 &axis_map_a_in, const phys_vec4 &axis_map_b_in, int feature_index_a, int feature_index_b, phys_feature_contact_point &feature_contact)
{
    phys_vec4 axis_map_b = axis_map_b_in;
    phys_vec4 axis_map_a = axis_map_a_in;
    axis_map_b(feature_index_b) = 1.0f;
    axis_map_a(feature_index_a) = 1.0f;

    int sign_mask_b = axis_map_b.compare_less_zero4().get_mask(phys_vec4_cmp::CMP_MASK_XYZ) ^ phys_vec4_cmp::CMP_MASK_XYZ;
    int sign_mask_a = axis_map_a.compare_less_zero4().get_mask(phys_vec4_cmp::CMP_MASK_XYZ);

    feature_contact.m_feature_a = (unsigned char)((phys_vec4_cmp::CMP_MASK_W << 4) | feature_index_a | (sign_mask_a << 4));
    feature_contact.m_feature_b = (unsigned char)(feature_index_b + (sign_mask_b << 4));
}

// @address 0x695e00 ?try_add_edge_point@phys_box_box_detector@@IBAXAAVphys_box_box_manifold@@HHHHABVphys_vec4@@1M@Z
void phys_box_box_detector::try_add_edge_point(phys_box_box_manifold &box_manifold, int edge_a, int edge_b, int next_vert_a, int next_vert_b,
    const phys_vec4 &box_normal_a, const phys_vec4 &box_normal_b, float closest_point_dist) const
{
    phys_feature_contact_point feature_contact;
    phys_box_box_set_edge_feature_bits(box_normal_a, box_normal_b, edge_a, edge_b, feature_contact);

    add_extra_edge_helper(box_manifold, feature_contact, closest_point_dist);

    int shift_width;
    if (next_vert_a == 0)
        shift_width = 3;
    else if (next_vert_a == 1)
        shift_width = 2;
    else
        shift_width = 1;
    int sign_flip_a = 1 << (4 + shift_width);

    if (next_vert_b == 0)
        shift_width = 3;
    else if (next_vert_b == 1)
        shift_width = 2;
    else
        shift_width = 1;
    int sign_flip_b = 1 << (4 + shift_width);

    feature_contact.m_feature_a ^= sign_flip_a;
    add_extra_edge_helper(box_manifold, feature_contact, closest_point_dist);
    feature_contact.m_feature_b ^= sign_flip_b;
    add_extra_edge_helper(box_manifold, feature_contact, closest_point_dist);
    feature_contact.m_feature_a ^= sign_flip_a;
    add_extra_edge_helper(box_manifold, feature_contact, closest_point_dist);
}

static __forceinline unsigned short phys_box_box_face_feature_bits(const phys_vec4 &axis_map, float normal_sign)
{
    const int sign_mask_a = axis_map.compare_less_zero4().get_mask(phys_vec4_cmp::CMP_MASK_XYZ);
    int face_bits = sign_mask_a << 4;
    const unsigned int *sign_bits_ptr = (unsigned int *)(&normal_sign);
    face_bits |= (int)(((0x80000000 & sign_bits_ptr[0]) >> 28));
    return (unsigned short)face_bits;
}

static __forceinline void phys_box_box_get_row(const phys_transform &t, int row, phys_vec4 &r)
{
    r.set(t.get_column(0)(row), t.get_column(1)(row), t.get_column(2)(row));
}
static __forceinline void phys_box_box_select_if_gt3(phys_vec4 &v, const phys_simd_scalar &a, const phys_vec4 &c, const phys_simd_scalar &b)
{
    v(0) = (c(0) > b) ? (float)a : v(0);
    v(1) = (c(1) > b) ? (float)a : v(1);
    v(2) = (c(2) > b) ? (float)a : v(2);
}
static __forceinline phys_bool phys_box_box_find_in_manifold(phys_box_box_manifold &box_manifold, const phys_feature_contact_point &feature_contact)
{
    for (int i = box_manifold.m_contact_point_count - 1; i >= 0; i--)
    {
        if (box_manifold.m_process_contact_points[i].m_feature_a == feature_contact.m_feature_a &&
            box_manifold.m_process_contact_points[i].m_feature_b == feature_contact.m_feature_b)
        {
            return true;
        }
    }
    return false;
}
static __forceinline void phys_box_box_set_edge_feature_bits_a(
    const phys_vec4 &axis_map_a_in, const phys_vec4 &axis_map_b_in, int feature_index_a, int feature_index_b, phys_feature_contact_point &feature_contact)
{
    phys_vec4 axis_map_a = axis_map_a_in;
    phys_vec4 axis_map_b = axis_map_b_in;
    axis_map_a(feature_index_a) = 1.0f;
    axis_map_b(feature_index_b) = 1.0f;

    int sign_mask_a = axis_map_a.compare_less_zero4().get_mask(phys_vec4_cmp::CMP_MASK_XYZ);
    int sign_mask_b = axis_map_b.compare_less_zero4().get_mask(phys_vec4_cmp::CMP_MASK_XYZ) ^ phys_vec4_cmp::CMP_MASK_XYZ;

    feature_contact.m_feature_a = (unsigned char)((phys_vec4_cmp::CMP_MASK_W << 4) | feature_index_a | (sign_mask_a << 4));
    feature_contact.m_feature_b = (unsigned char)(feature_index_b + (sign_mask_b << 4));
}

__forceinline void phys_box_box_detector::face_a_vertex_b_validate_idx(phys_feature_point_cache &feature_point_cache, int feature_idx) const
{
    feature_point_cache.m_feature_idx_a = feature_idx;
    phys_vec4 dim_a;
    phys_box_box_get_row(m_a_to_b, feature_idx, dim_a);

    if (m_din_a(feature_idx) < 0.0f)
    {
        feature_point_cache.m_normal_sign = -1.0f;
        feature_point_cache.m_vec_b = m_radius_b;
    }
    else
    {
        feature_point_cache.m_normal_sign = 1.0f;
        feature_point_cache.m_vec_b.set_neg4(m_radius_b);
    }

    phys_vec4 normal_tweak;
    normal_tweak.set_all3(-0.00001f);
    phys_simd_scalar zero = 0.0f;
    phys_simd_scalar hull_tol = 0.00001f;
    phys_box_box_select_if_gt3(normal_tweak, hull_tol, m_din_b, zero);
    normal_tweak.mul4(phys_simd_scalar(feature_point_cache.m_normal_sign));
    dim_a.add4(normal_tweak);

    phys_vec4_util::mul_signs4(feature_point_cache.m_vec_b, dim_a);
    feature_point_cache.m_vec_a.set_transformed_pos(m_a_to_b, feature_point_cache.m_vec_b);
}

__forceinline void phys_box_box_detector::face_b_vertex_a_validate_idx(phys_feature_point_cache &feature_point_cache, int feature_idx) const
{
    feature_point_cache.m_feature_idx_a = feature_idx + 4;
    phys_vec4 dim_b = m_a_to_b.get_column(feature_idx);

    if (m_din_b(feature_idx) < 0.0f)
    {
        feature_point_cache.m_normal_sign = -1.0f;
        feature_point_cache.m_vec_a.set_neg4(m_radius_a);
    }
    else
    {
        feature_point_cache.m_normal_sign = 1.0f;
        feature_point_cache.m_vec_a = m_radius_a;
    }

    phys_vec4 normal_tweak;
    normal_tweak.set_all3(0.00001f);
    phys_simd_scalar zero = 0.0f;
    phys_simd_scalar hull_tol = -0.00001f;
    phys_box_box_select_if_gt3(normal_tweak, hull_tol, m_din_a, zero);
    normal_tweak.mul4(phys_simd_scalar(-feature_point_cache.m_normal_sign));
    dim_b.add4(normal_tweak);

    phys_vec4_util::mul_signs4(feature_point_cache.m_vec_a, dim_b);
    feature_point_cache.m_vec_b.set_transformed_inverse_pos(m_a_to_b, feature_point_cache.m_vec_a);
}

__forceinline void phys_box_box_detector::edge_edge_validate_from_idx(phys_feature_point_cache &feature_point_cache) const
{
    const int fpp_feature_a = feature_point_cache.m_feature_idx_a;
    const int fpp_feature_b = feature_point_cache.m_feature_idx_b;

    phys_vec4 edge_a_in_a;
    edge_a_in_a.set_zero4();
    edge_a_in_a(fpp_feature_a) = 1.0f;
    phys_vec4 edge_b_in_a = m_a_to_b.get_column(fpp_feature_b);
    phys_vec4 normal_in_a;
    normal_in_a.set_cross(edge_a_in_a, edge_b_in_a);
    if (normal_in_a.dot3(m_din_a) < 0.0f)
    {
        normal_in_a.set_neg4(normal_in_a);
    }
    phys_vec4 normal_in_b;
    normal_in_b.set_rotated_inverse_dir(m_a_to_b.get_rotation(), normal_in_a);

    feature_point_cache.m_vec_a = normal_in_a;
    feature_point_cache.m_vec_b = normal_in_b;
}

__forceinline phys_bool phys_box_box_detector::is_valid_face_a_vertex_b(phys_feature_point_cache &feature_point_cache) const
{
    phys_vec4 vertex_b_in_a = feature_point_cache.m_vec_a;
    vertex_b_in_a.set_abs4(vertex_b_in_a);

    if (vertex_b_in_a.all_less_than3(m_keep_radius_a))
    {
        const int feature_idx = feature_point_cache.m_feature_idx_a;
        feature_point_cache.m_distance = feature_point_cache.m_normal_sign * feature_point_cache.m_vec_a(feature_idx) - m_radius_a(feature_idx);
        return true;
    }
    return false;
}

__forceinline phys_bool phys_box_box_detector::is_valid_face_b_vertex_a(phys_feature_point_cache &feature_point_cache) const
{
    phys_vec4 vertex_a_in_b = feature_point_cache.m_vec_b;
    vertex_a_in_b.set_abs4(vertex_a_in_b);

    if (vertex_a_in_b.all_less_than3(m_keep_radius_b))
    {
        const int feature_idx = feature_point_cache.m_feature_idx_a - 4;
        feature_point_cache.m_distance = -feature_point_cache.m_normal_sign * feature_point_cache.m_vec_b(feature_idx) - m_radius_b(feature_idx);
        return true;
    }
    return false;
}

// @address 0x696038 ?find_closest_point@phys_box_box_detector@@IBAHAAVphys_box_box_manifold@@AAVphys_feature_contact_point@@AAVphys_feature_point_cache@1@@Z
int phys_box_box_detector::find_closest_point(phys_box_box_manifold &box_manifold, phys_feature_contact_point &feature_contact,
    phys_feature_point_cache &feature_point_cache) const
{
    for (int features_left = 8; features_left > 0; features_left--)
    {
        unsigned int sep_feature_idx = 0;
        float best_dist = m_sep_dist[0](0);

        for (int i = 0; i < 5; i++)
        {
            if (m_sep_dist[i](0) > best_dist)
            {
                sep_feature_idx = i * 4;
                best_dist = m_sep_dist[i](0);
            }
            if (m_sep_dist[i](1) > best_dist)
            {
                sep_feature_idx = i * 4 + 1;
                best_dist = m_sep_dist[i](1);
            }
            if (m_sep_dist[i](2) > best_dist)
            {
                sep_feature_idx = i * 4 + 2;
                best_dist = m_sep_dist[i](2);
            }
        }

        if (sep_feature_idx <= PHYS_BOX_BOX_FACE_A_Z)
        {
            const int feature_idx = sep_feature_idx;
            feature_contact.m_feature_a = (unsigned char)sep_feature_idx;
            face_a_vertex_b_validate_idx(feature_point_cache, feature_idx);
            feature_contact.m_feature_b = (unsigned char)phys_box_box_face_feature_bits(feature_point_cache.m_vec_b, feature_point_cache.m_normal_sign);

            if (phys_box_box_find_in_manifold(box_manifold, feature_contact))
            {
                return 1;
            }
            if (is_valid_face_a_vertex_b(feature_point_cache))
            {
                return 2;
            }
        }
        else if (sep_feature_idx <= PHYS_BOX_BOX_FACE_B_Z)
        {
            const int feature_idx = sep_feature_idx - PHYS_BOX_BOX_FACE_B_X;
            feature_contact.m_feature_a = (unsigned char)sep_feature_idx;
            face_b_vertex_a_validate_idx(feature_point_cache, feature_idx);
            feature_contact.m_feature_b = (unsigned char)phys_box_box_face_feature_bits(feature_point_cache.m_vec_a, feature_point_cache.m_normal_sign);

            if (phys_box_box_find_in_manifold(box_manifold, feature_contact))
            {
                return 1;
            }
            if (is_valid_face_b_vertex_a(feature_point_cache))
            {
                return 2;
            }
        }
        else
        {
            const unsigned int feature_i = sep_feature_idx - 8;
            feature_point_cache.m_feature_idx_a = feature_i / 4;
            feature_point_cache.m_feature_idx_b = feature_i & 3;

            edge_edge_validate_from_idx(feature_point_cache);
            phys_box_box_set_edge_feature_bits_a(feature_point_cache.m_vec_a, feature_point_cache.m_vec_b,
                feature_point_cache.m_feature_idx_a, feature_point_cache.m_feature_idx_b, feature_contact);
            feature_point_cache.m_feature_idx_a = feature_contact.m_feature_a;
            feature_point_cache.m_feature_idx_b = feature_contact.m_feature_b;

            if (phys_box_box_find_in_manifold(box_manifold, feature_contact))
            {
                return 1;
            }
            phys_bool result = is_valid_edge_edge(feature_point_cache);
            if (result)
            {
                return 2;
            }
        }

        m_sep_dist[sep_feature_idx >> 2](sep_feature_idx & 3) = -3.40282e38f;
    }
    return 0;
}

// @address 0x696b80 ?compute_closest_point@phys_box_box_detector@@QBA?AVphys_bool@@AAVphys_contact_point@@@Z
phys_bool phys_box_box_detector::compute_closest_point(phys_contact_point &contact) const
{
    phys_bool success = false;

    phys_box_box_manifold box_manifold;
    phys_feature_contact_point feature_contact;
    phys_feature_point_cache feature_point_cache;

    init_work_vars();
    {
        if (PHYS_SUCCESS == check_box_intersection(m_box_tolerance4))
        {
            int valid_collision = find_closest_point(box_manifold, feature_contact, feature_point_cache);

            if (valid_collision == 2)
            {
                success = true;
                phys_process_hit_point contact_hit;

                contact_from_feature_cache(contact_hit, feature_contact, feature_point_cache);

                if (feature_contact.m_feature_a > PHYS_BOX_BOX_FACE_A_Z)
                {
                    contact_hit.m_hit_contact.get_contact_position().add_scaled4(
                        phys_simd_scalar(-contact_hit.m_hit_contact.get_distance()),
                        contact_hit.m_hit_contact.get_contact_normal());
                }

                contact.get_separating_normal() = contact_hit.m_hit_contact.get_contact_normal();
                contact.get_contact_position() = contact_hit.m_hit_contact.get_contact_position();
                contact.contact_set_distance(contact_hit.m_hit_contact.get_distance());
            }
            else
            {
                success = false;
            }
        }
    }

    return success;
}


__forceinline void phys_box_box_detector::bridge_remove_point(phys_box_box_manifold &box_manifold, int index) const
{
    if (box_manifold[index].m_feature_a <= PHYS_BOX_BOX_FACE_B_Z)
    {
        box_manifold.m_face_contact_count--;
    }
    m_contact_manager->contact_remove_point(box_manifold.m_process_contact_points[index].m_process_contact_id, *m_result->m_constraint_owner);
    box_manifold.bridge_remove_point(index);
}

__forceinline void phys_box_box_detector::face_a_vertex_b_validate_id(phys_feature_point_cache &feature_point_cache,
    const phys_feature_contact_point &feature_contact) const
{
    feature_point_cache.m_feature_idx_a = feature_contact.m_feature_a;
    feature_point_cache.m_normal_sign = normal_sign_from_features(feature_contact.m_feature_b);
    const phys_vec4 &axis_map = get_axis_map_from_feature_bits(feature_contact.m_feature_b);
    feature_point_cache.m_vec_b.set_mul4(m_radius_b, axis_map);
    feature_point_cache.m_vec_a.set_transformed_pos(m_a_to_b, feature_point_cache.m_vec_b);
}

__forceinline void phys_box_box_detector::face_b_vertex_a_validate_id(phys_feature_point_cache &feature_point_cache,
    const phys_feature_contact_point &feature_contact) const
{
    feature_point_cache.m_feature_idx_a = feature_contact.m_feature_a;
    feature_point_cache.m_normal_sign = normal_sign_from_features(feature_contact.m_feature_b);
    const phys_vec4 &axis_map = get_axis_map_from_feature_bits(feature_contact.m_feature_b);
    feature_point_cache.m_vec_a.set_mul4(m_radius_a, axis_map);
    feature_point_cache.m_vec_b.set_transformed_inverse_pos(m_a_to_b, feature_point_cache.m_vec_a);
}

__forceinline void phys_box_box_detector::refresh_manifold(phys_box_box_manifold &box_manifold) const
{
    int i = 0;
    while (i < box_manifold.get_manifold_point_count())
    {
        const phys_feature_contact_point &feature_contact = box_manifold[i];
        const int sep_feature_idx = feature_contact.m_feature_a;

        if (sep_feature_idx <= PHYS_BOX_BOX_FACE_A_Z)
        {
            phys_feature_point_cache feature_point_cache;
            face_a_vertex_b_validate_id(feature_point_cache, feature_contact);
            if (is_valid_face_a_vertex_b(feature_point_cache))
            {
                phys_process_hit_point &contact_hit = *m_result->reserve_contact_points(1);
                m_result->commit_contact_points(1);
                face_a_vertex_b_contact(contact_hit, feature_contact, feature_point_cache);
                i++;
                continue;
            }
        }
        else if (sep_feature_idx <= PHYS_BOX_BOX_FACE_B_Z)
        {
            phys_feature_point_cache feature_point_cache;
            face_b_vertex_a_validate_id(feature_point_cache, feature_contact);
            if (is_valid_face_b_vertex_a(feature_point_cache))
            {
                phys_process_hit_point &contact_hit = *m_result->reserve_contact_points(1);
                m_result->commit_contact_points(1);
                face_b_vertex_a_contact(contact_hit, feature_contact, feature_point_cache);
                i++;
                continue;
            }
        }
        else
        {
            phys_feature_point_cache feature_point_cache;
            edge_edge_validate_from_id(feature_point_cache, feature_contact);
            if (is_valid_edge_edge(feature_point_cache))
            {
                phys_process_hit_point &contact_hit = *m_result->reserve_contact_points(1);
                m_result->commit_contact_points(1);
                edge_edge_contact_from_cache(contact_hit, feature_contact, feature_point_cache);
                i++;
                continue;
            }
        }

        bridge_remove_point(box_manifold, i);
    }
}

__forceinline phys_bool phys_box_box_detector::check_manifold_normals(phys_box_box_manifold &box_manifold) const
{
    if (box_manifold.get_manifold_point_count() < 2)
    {
        box_manifold.m_manifold_normal_ready = false;
        return true;
    }

    if (!box_manifold.m_manifold_normal_ready)
    {
        phys_vec4 manifold_normal_b;
        manifold_normal_b.vec_set_rotated_inverse_dir(m_a_to_b.get_rotation(), box_manifold.m_manifold_normal_a);
        box_manifold.m_manifold_normal_ready = true;
        box_manifold.m_manifold_normal_b = phys_vec4_util::pack_quat_to_u32(manifold_normal_b);
        return true;
    }

    phys_vec4 manifold_normal_b;
    phys_vec4_util::unpack_i32_to_quat(box_manifold.m_manifold_normal_b, manifold_normal_b);

    phys_vec4 manifold_normal_b_in_a;
    manifold_normal_b_in_a.set_rotated_dir(m_a_to_b.get_rotation(), manifold_normal_b);

    const float cos_delta = box_manifold.m_manifold_normal_a.dot3(manifold_normal_b_in_a);
    return cos_delta < m_manifold_cos_tolerance;
}

__forceinline void phys_box_box_detector::check_manifold_normal_consistency(phys_box_box_manifold &box_manifold) const
{
    phys_vec4 manifold_normal_ws;
    manifold_normal_ws.vec_set_rotated_dir(m_world_from_a.get_rotation(), box_manifold.m_manifold_normal_a);

    int result_start = m_result->get_process_contact_count() - box_manifold.get_manifold_point_count();
    int i = 0;
    while (i < box_manifold.get_manifold_point_count())
    {
        const phys_process_hit_point &contact_hit = m_result->m_process_contact_points[result_start];
        const float cos_check = contact_hit.m_hit_contact.get_contact_normal().dot3(manifold_normal_ws);

        if (cos_check < m_manifold_cos_tolerance)
        {
            m_result->m_process_contact_points[result_start] = m_result->get_track_end()[-1];
            bridge_remove_point(box_manifold, i);
            m_result->commit_contact_points(-1);
        }
        else
        {
            i++;
            result_start++;
        }
    }

    if (box_manifold.get_manifold_point_count() < 2)
    {
        box_manifold.m_manifold_normal_ready = false;
    }
    else
    {
        phys_vec4 manifold_normal_b;
        manifold_normal_b.set_rotated_inverse_dir(m_a_to_b.get_rotation(), box_manifold.m_manifold_normal_a);
        box_manifold.m_manifold_normal_b = phys_vec4_util::pack_quat_to_u32(manifold_normal_b);
        box_manifold.m_manifold_normal_ready = true;
    }
}

__forceinline void phys_box_box_detector::compute_manifold_normal(
    phys_vec4 &manifold_normal, const phys_feature_contact_point &feature_contact, phys_feature_point_cache &feature_point_cache,
        phys_bool is_point_validated) const
{
    const int sep_feature_idx = feature_point_cache.m_feature_idx_a;

    if (sep_feature_idx <= PHYS_BOX_BOX_FACE_A_Z)
    {
        const int feature_idx = feature_contact.m_feature_a;
        manifold_normal.set_zero4();
        manifold_normal(feature_idx) = -feature_point_cache.m_normal_sign;
    }
    else if (sep_feature_idx <= PHYS_BOX_BOX_FACE_B_Z)
    {
        const int feature_idx = feature_contact.m_feature_a - PHYS_BOX_BOX_FACE_B_X;
        manifold_normal.set_mul4(phys_simd_scalar(-feature_point_cache.m_normal_sign), m_a_to_b.get_rotation().get_column(feature_idx));
    }
    else
    {
        if (is_point_validated)
        {
            manifold_normal = feature_point_cache.m_normal_a;
        }
    }
}

__forceinline void phys_box_box_detector::try_add_point_face_a(
    phys_box_box_manifold &box_manifold, const phys_feature_contact_point feature_contact_template, unsigned short plane_mask, float closest_point_dist) const
{
    int axis_a_i;
    for (axis_a_i = 0; axis_a_i < 7; axis_a_i++)
    {
        const unsigned short cycle_bits = cycle_mask[axis_a_i];
        if (!(cycle_bits & plane_mask))
        {
            unsigned short new_feature_id = cycle_bits ^ feature_contact_template.m_feature_b;
            phys_feature_contact_point fcp_test = feature_contact_template;
            fcp_test.m_feature_b = (unsigned char)(new_feature_id | (fcp_test.m_feature_b & 0xff0f));
            if (!box_manifold.find_in_manifold(fcp_test))
            {
                phys_feature_point_cache feature_point_cache;
                face_a_vertex_b_validate_id(feature_point_cache, fcp_test);

                if (is_valid_face_a_vertex_b(feature_point_cache))
                {
                    if (feature_point_cache.m_distance >= (closest_point_dist - 1.1920929e-07f))
                    {
                        add_manifold_point(box_manifold, feature_point_cache, fcp_test);
                    }
                }
            }
        }
    }
}

__forceinline void phys_box_box_detector::try_add_point_face_b(
    phys_box_box_manifold &box_manifold, const phys_feature_contact_point feature_contact_template, unsigned short plane_mask, float closest_point_dist) const
{
    int axis_a_i;
    for (axis_a_i = 0; axis_a_i < 7; axis_a_i++)
    {
        const unsigned short cycle_bits = cycle_mask[axis_a_i];
        if (!(cycle_bits & plane_mask))
        {
            unsigned short new_feature_id = cycle_bits ^ feature_contact_template.m_feature_b;
            phys_feature_contact_point fcp_test = feature_contact_template;
            fcp_test.m_feature_b = (unsigned char)(new_feature_id | (fcp_test.m_feature_b & 0xff0f));
            if (!box_manifold.find_in_manifold(fcp_test))
            {
                phys_feature_point_cache feature_point_cache;
                face_b_vertex_a_validate_id(feature_point_cache, fcp_test);

                if (is_valid_face_b_vertex_a(feature_point_cache))
                {
                    if (feature_point_cache.m_distance >= (closest_point_dist - 1.1920929e-07f))
                    {
                        add_manifold_point(box_manifold, feature_point_cache, fcp_test);
                    }
                }
            }
        }
    }
}

__forceinline void phys_box_box_detector::find_extra_manifold_points(phys_box_box_manifold &box_manifold,
    phys_feature_contact_point feature_contact_a_template) const
{
    if (box_manifold.get_manifold_point_count() < 1)
    {
        return;
    }

    int plane_mask_a, plane_mask_b;
    phys_vec4 axis_map_a, axis_map_b;
    int axis_a, axis_b;

    const int result_point_idx = m_result->get_process_contact_count() - box_manifold.get_manifold_point_count();

    {
        phys_vec4 axis_map_a_tmp, axis_map_b_tmp;
        const phys_process_hit_point &contact_hit = m_result->m_process_contact_points[result_point_idx];
        const phys_vec4 &normal = contact_hit.m_hit_contact.get_contact_normal();

        axis_map_a.set_rotated_inverse_dir(m_world_from_a.get_rotation(), normal);
        axis_map_b.set_rotated_inverse_dir(m_world_from_b.get_rotation(), normal);
        axis_map_a.set_neg4(axis_map_a);
        axis_map_a_tmp.set_abs4(axis_map_a);
        axis_map_b_tmp.set_abs4(axis_map_b);

        plane_mask_a = max_plane_mask3(axis_map_a_tmp, axis_map_a, axis_a);
        plane_mask_b = max_plane_mask3(axis_map_b_tmp, axis_map_b, axis_b);

        if (axis_map_a_tmp(axis_a) < m_coplanar_ang_cos_tolerance || axis_map_b_tmp(axis_b) < m_coplanar_ang_cos_tolerance)
        {
            return;
        }
    }

    phys_feature_contact_point fcp_b_template;
    {
        const float sign_a = m_din_a(axis_a);
        const float sign_b = m_din_b(axis_b);

        feature_contact_a_template.m_feature_b = (unsigned char)phys_box_box_face_feature_bits(axis_map_b, sign_a);
        fcp_b_template.m_feature_b = (unsigned char)phys_box_box_face_feature_bits(axis_map_a, sign_b);
    }

    feature_contact_a_template.m_feature_a = (unsigned char)axis_a;
    fcp_b_template.m_feature_a = (unsigned char)(axis_b + PHYS_BOX_BOX_FACE_B_X);

    int i;
    float closest_point_dist = 1.0f;
    for (i = 0; i < box_manifold.get_manifold_point_count(); i++)
    {
        const float candidate = m_result->m_process_contact_points[result_point_idx + i].m_hit_contact.get_distance();
        if (candidate < closest_point_dist)
            closest_point_dist = candidate;
    }

    try_add_point_face_a(box_manifold, feature_contact_a_template, plane_mask_b, closest_point_dist);
    try_add_point_face_b(box_manifold, fcp_b_template, plane_mask_a, closest_point_dist);

    if (box_manifold.m_face_contact_count >= 4)
    {
        box_manifold.set_complete(true);
        return;
    }

    check_manifold_completeness(box_manifold, plane_mask_a, plane_mask_b);
}

// @address 0x696e00 ?compute_box_manifold@phys_box_box_detector@@QBAXAAVphys_box_box_manifold@@@Z
void phys_box_box_detector::compute_box_manifold(phys_box_box_manifold &box_manifold) const
{
    init_work_vars();

    phys_process_hit_point *contact_hit = m_result->m_next_free_contact;

    refresh_manifold(box_manifold);

    phys_bool want_manifold_normal = check_manifold_normals(box_manifold);

    if (!want_manifold_normal && box_manifold.is_complete())
    {
        return;
    }

    {
        phys_result points_available;
        if (box_manifold.m_contact_point_count)
        {
            float min_dist = contact_hit->m_hit_contact.get_distance();
            for (int i = box_manifold.m_contact_point_count - 1; i > 0; i--)
            {
                min_dist = phys_math::min_of_two(min_dist, contact_hit[i].m_hit_contact.get_distance());
            }
            min_dist -= m_box_tolerance4(0) * 0.05f;
            phys_vec4 min_tol_vec;
            min_tol_vec.set_all3(min_dist);
            points_available = check_box_intersection(min_tol_vec);
        }
        else
        {
            points_available = check_box_intersection(m_box_tolerance4);
        }

        if (phys_bool(points_available == PHYS_SUCCESS))
        {
            phys_feature_contact_point feature_contact;
            phys_feature_point_cache feature_point_cache;
            feature_point_cache.m_normal_a.set_zero4();

            int valid_collision = find_closest_point(box_manifold, feature_contact, feature_point_cache);

            if (want_manifold_normal && valid_collision)
            {
                compute_manifold_normal(box_manifold.m_manifold_normal_a, feature_contact, feature_point_cache, valid_collision == 2);
                if (box_manifold.m_manifold_normal_ready)
                {
                    check_manifold_normal_consistency(box_manifold);
                }
            }

            if (valid_collision == 2)
            {
                phys_result result = add_manifold_point(box_manifold, feature_point_cache, feature_contact);
                if (result == PHYS_SUCCESS)
                {
                    find_extra_manifold_points(box_manifold, feature_contact);
                }
            }
        }
    }
}

// @address 0x694f20 ?compute_face_feature_bits@@YAGABVphys_vec4@@M@Z
unsigned short compute_face_feature_bits(const phys_vec4 &axis_map, float normal_sign)
{
    const int sign_mask_a = axis_map.compare_less_zero4().get_mask(phys_vec4_cmp::CMP_MASK_XYZ);
    int face_bits = sign_mask_a << 4;
    const unsigned int *sign_bits_ptr = (unsigned int *)(&normal_sign);
    face_bits |= (int)(((0x80000000 & sign_bits_ptr[0]) >> 28));
    return (unsigned short)face_bits;
}


static __forceinline void phys_box_box_rsqrt_all3(phys_vec4 &v)
{
    const float float_epsilon = 1.192092896e-07f;
    v(0) = phys_math::inv_sqrt(v(0) + float_epsilon);
    v(1) = phys_math::inv_sqrt(v(1) + float_epsilon);
    v(2) = phys_math::inv_sqrt(v(2) + float_epsilon);
    v(3) = 0.0f;
}

__forceinline void phys_box_box_detector::set_vd_proj(const phys_rotation &b_rot_a, int i, int j, phys_vec4 &vd_proj) const
{
    phys_vec4 t0;
    t0.set_mul4(m_din_a.get_simd_at(j), b_rot_a.get_column(i));
    phys_vec4 t1;
    t1.set_mul4(m_din_a.get_simd_at(i), b_rot_a.get_column(j));
    vd_proj.set_sub4(t0, t1);
    vd_proj.set_abs4(vd_proj);
}

// @address 0x695008 ?check_box_intersection@phys_box_box_detector@@QBA?AW4phys_result@@ABVphys_vec4@@@Z
phys_result phys_box_box_detector::check_box_intersection(const phys_vec4 &tolerance) const
{
    const phys_simd_scalar PHYS_EDGE_IGNORE_TOLERANCE = phys_simd_scalar::create(1.0f / 0.001f);
    const phys_simd_scalar neg_huge_scalar = phys_simd_scalar::create(-1e17f);

    phys_rotation a_rot_b;
    a_rot_b.get_column(0).set_abs4(m_a_to_b.get_rotation().get_column(0));
    a_rot_b.get_column(1).set_abs4(m_a_to_b.get_rotation().get_column(1));
    a_rot_b.get_column(2).set_abs4(m_a_to_b.get_rotation().get_column(2));

    {
        phys_vec4 width_vec;
        width_vec.set_rotated_dir(a_rot_b, m_radius_b);
        phys_vec4 vd_proj;
        vd_proj.set_abs4(m_din_a);
        width_vec.add4(m_radius_a);
        vd_proj.sub4(width_vec);

        if (vd_proj.compare_greater4(tolerance).any_is_set(phys_vec4_cmp::CMP_MASK_XYZ))
        {
            return PHYS_FAILURE;
        }
        m_sep_dist[0] = vd_proj;
    }

    {
        phys_vec4 width_vec;
        width_vec.set_rotated_inverse_dir(a_rot_b, m_radius_a);
        phys_vec4 vd_proj;
        vd_proj.set_abs4(m_din_b);
        width_vec.add4(m_radius_b);
        vd_proj.sub4(width_vec);

        if (vd_proj.compare_greater4(tolerance).any_is_set(phys_vec4_cmp::CMP_MASK_XYZ))
        {
            return PHYS_FAILURE;
        }
        m_sep_dist[1] = vd_proj;
    }

    {
        phys_rotation b_rot_a;
        b_rot_a.set_transpose(m_a_to_b.get_rotation());

        phys_vec4 rot_sq_col0 = b_rot_a.get_column(0);
        phys_vec4 rot_sq_col1 = b_rot_a.get_column(1);
        phys_vec4 rot_sq_col2 = b_rot_a.get_column(2);
        {
            rot_sq_col0.mul4(rot_sq_col0);
            rot_sq_col1.mul4(rot_sq_col1);
            rot_sq_col2.mul4(rot_sq_col2);
        }

        {
            phys_vec4 vd_proj;
            set_vd_proj(b_rot_a, 1, 2, vd_proj);

            phys_vec4 width_vec;
            width_vec.set(
                a_rot_b(2, 0) * m_radius_a(1) + a_rot_b(1, 0) * m_radius_a(2) + a_rot_b(0, 2) * m_radius_b(1) + a_rot_b(0, 1) * m_radius_b(2),
                a_rot_b(2, 1) * m_radius_a(1) + a_rot_b(1, 1) * m_radius_a(2) + a_rot_b(0, 2) * m_radius_b(0) + a_rot_b(0, 0) * m_radius_b(2),
                a_rot_b(2, 2) * m_radius_a(1) + a_rot_b(1, 2) * m_radius_a(2) + a_rot_b(0, 1) * m_radius_b(0) + a_rot_b(0, 0) * m_radius_b(1));

            phys_vec4 inv_cross_lengths;
            inv_cross_lengths.set_add4(rot_sq_col2, rot_sq_col1);

            vd_proj.sub4(width_vec);
            phys_box_box_rsqrt_all3(inv_cross_lengths);
            vd_proj.set_mul4(inv_cross_lengths, vd_proj);

            if (vd_proj.compare_greater4(m_box_tolerance4).any_is_set(phys_vec4_cmp::CMP_MASK_XYZ))
            {
                return PHYS_FAILURE;
            }

            phys_box_box_select_if_gt3(vd_proj, neg_huge_scalar, inv_cross_lengths, PHYS_EDGE_IGNORE_TOLERANCE);

            m_sep_dist[2] = vd_proj;
        }

        {
            phys_vec4 vd_proj;
            set_vd_proj(b_rot_a, 2, 0, vd_proj);

            phys_vec4 width_vec;
            width_vec.set(
                a_rot_b(2, 0) * m_radius_a(0) + a_rot_b(0, 0) * m_radius_a(2) + a_rot_b(1, 2) * m_radius_b(1) + a_rot_b(1, 1) * m_radius_b(2),
                a_rot_b(2, 1) * m_radius_a(0) + a_rot_b(0, 1) * m_radius_a(2) + a_rot_b(1, 2) * m_radius_b(0) + a_rot_b(1, 0) * m_radius_b(2),
                a_rot_b(2, 2) * m_radius_a(0) + a_rot_b(0, 2) * m_radius_a(2) + a_rot_b(1, 1) * m_radius_b(0) + a_rot_b(1, 0) * m_radius_b(1));

            phys_vec4 inv_cross_lengths;
            inv_cross_lengths.set_add4(rot_sq_col2, rot_sq_col0);

            vd_proj.sub4(width_vec);
            phys_box_box_rsqrt_all3(inv_cross_lengths);
            vd_proj.set_mul4(inv_cross_lengths, vd_proj);

            if (vd_proj.compare_greater4(m_box_tolerance4).any_is_set(phys_vec4_cmp::CMP_MASK_XYZ))
            {
                return PHYS_FAILURE;
            }

            phys_box_box_select_if_gt3(vd_proj, neg_huge_scalar, inv_cross_lengths, PHYS_EDGE_IGNORE_TOLERANCE);

            m_sep_dist[3] = vd_proj;
        }

        {
            phys_vec4 vd_proj;
            set_vd_proj(b_rot_a, 0, 1, vd_proj);

            phys_vec4 width_vec;
            width_vec.set(
                a_rot_b(1, 0) * m_radius_a(0) + a_rot_b(0, 0) * m_radius_a(1) + a_rot_b(2, 2) * m_radius_b(1) + a_rot_b(2, 1) * m_radius_b(2),
                a_rot_b(1, 1) * m_radius_a(0) + a_rot_b(0, 1) * m_radius_a(1) + a_rot_b(2, 0) * m_radius_b(2) + a_rot_b(2, 2) * m_radius_b(0),
                a_rot_b(1, 2) * m_radius_a(0) + a_rot_b(0, 2) * m_radius_a(1) + a_rot_b(2, 1) * m_radius_b(0) + a_rot_b(2, 0) * m_radius_b(1));

            phys_vec4 inv_cross_lengths;
            inv_cross_lengths.set_add4(rot_sq_col1, rot_sq_col0);

            vd_proj.sub4(width_vec);
            phys_box_box_rsqrt_all3(inv_cross_lengths);
            vd_proj.set_mul4(inv_cross_lengths, vd_proj);

            if (vd_proj.compare_greater4(m_box_tolerance4).any_is_set(phys_vec4_cmp::CMP_MASK_XYZ))
            {
                return PHYS_FAILURE;
            }

            phys_box_box_select_if_gt3(vd_proj, neg_huge_scalar, inv_cross_lengths, PHYS_EDGE_IGNORE_TOLERANCE);

            m_sep_dist[4] = vd_proj;
        }
    }

    return PHYS_SUCCESS;
}
