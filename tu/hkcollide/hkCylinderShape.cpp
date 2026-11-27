// hkcollide/hkCylinderShape.obj: from the records 0x626360 0x626378 0x626380 0x6263d8 0x6263e0 0x6263e8 0x626448 0x626490 0x6265b8 0x6265d8 0x626838 0x626978 0x6269b0 0x6269c0 0x626be8 0x6270d0 0x627188 0x6271f8 0x6273b0 0x6273c8
// @compiler xdk6130
// @flags /TP /O2 /Oc
// @address 0x626360 ?destroy_loaded_cylinder_shape@@YAXPAX@Z
// @address 0x626378 ?assert_round_up_threshold@@YAXM@Z
// @address 0x626380 ?set_virtual_side_segments@phys_cylinder_shape@@SAXH@Z
// @address 0x6263d8 ?get_cylinder_radius@phys_cylinder_shape@@QBAMXZ
// @address 0x6263e0 ?get_collision_sphere_count_impl@phys_cylinder_shape@@UBAHXZ
// @address 0x6263e8 ?gather_memory_stats@phys_cylinder_shape@@UBAXPAVphys_statistics_collector@@@Z
// @address 0x6265b8 ?set_cylinder_radius@phys_cylinder_shape@@QAAXM@Z
// @address 0x626978 ?get_shape_centre_impl@phys_cylinder_shape@@UBAXAAVphys_vec4@@@Z
// @address 0x6269b0 ?get_first_vertex@phys_cylinder_shape@@UBAXAAVphys_vec4@@@Z
// @address 0x6273b0 ?fixup_loaded_cylinder_shape@@YAXPAX@Z
// @address 0x6273c8 ?get_cylinder_shape_vtable@@YAPBXXZ
#include "phys/phys_base_types.h"
#include <vectorintrinsics.h>
#include <ppcintrinsics.h>
#include <math.h>
#include "phys/phys_math.h"
#include "phys/phys_collide.h"
#include "phys/phys_statistics_collector.h"

#include "phys/phys_cylinder_shape.h"
#include "phys/phys_vec4_util.h"

namespace {
}


float phys_cylinder_shape::s_virtualTesselationParameter = 2.6874115467071533f;
float phys_cylinder_shape::s_virtualTesselationParameterInv = 0.37210527062416077f;
float phys_cylinder_shape::s_intRoundUpThreshold = -1.0f;

void destroy_loaded_cylinder_shape(void *object)
{
    ((phys_cylinder_shape *)object)->~phys_cylinder_shape();
}

void assert_round_up_threshold(float threshold)
{
}


void phys_cylinder_shape::set_virtual_side_segments(int segment_count)
{
    float parameter = (float(segment_count / 8) - 0.1f) / 0.707f;
    s_virtualTesselationParameter = parameter;
    s_virtualTesselationParameterInv = 1.0f / parameter;
}

float phys_cylinder_shape::get_cylinder_radius() const
{
    return m_cylinder_radius;
}

int phys_cylinder_shape::get_collision_sphere_count_impl() const
{
    return 18;
}


// @address 0x6269c0 ?get_collision_spheres_impl@phys_cylinder_shape@@UBAPBVphys_sphere@@PAV2@@Z
const phys_sphere *phys_cylinder_shape::get_collision_spheres_impl(phys_sphere *sphere_buffer) const
{
    long stack_pad_0 = 0;
    long stack_pad_1 = 0;
    long stack_pad_2 = 0;
    long stack_pad_3 = 0;
    long stack_pad_4 = 0;
    long stack_pad_5 = 0;
    long stack_pad_6 = 0;
    long stack_pad_7 = 0;
    long stack_pad_8 = 0;
    long stack_pad_9 = 0;
    long stack_pad_10 = 0;
    long stack_pad_11 = 0;
    long stack_pad_12 = 0;
    long stack_pad_13 = 0;
    long stack_pad_14 = 0;
    long stack_pad_15 = 0;
    long stack_pad_16 = 0;
    long stack_pad_17 = 0;
    long stack_pad_18 = 0;
    long stack_pad_19 = 0;
    long stack_pad_20 = 0;
    long stack_pad_21 = 0;
    long stack_pad_22 = 0;
    long stack_pad_23 = 0;
    long stack_pad_24 = 0;
    long stack_pad_25 = 0;
    long stack_pad_26 = 0;
    long stack_pad_27 = 0;
    long stack_pad_28 = 0;
    long stack_pad_29 = 0;
    long stack_pad_30 = 0;
    float base_sphere_radius = m_cylinder_radius;
    float big_sphere_shift = 0.0f;
    {
        phys_vec4 symmetry_axis;
        symmetry_axis.set_sub4(get_vertex(1), get_vertex(0));
        float height_sq = symmetry_axis.length_sq_3d();
        if (height_sq >= 2.0f * 2.0f * m_cylinder_radius * m_cylinder_radius) {
            big_sphere_shift = m_cylinder_radius;
            base_sphere_radius = m_cylinder_radius * m_cyl_heightfield_radius_factor;
        }
    }

    phys_vec4 diag_axis1;
    diag_axis1.set_add4(m_perp_axis1, m_perp_axis2);
    diag_axis1.mul4(base_sphere_radius * 0.70710677f);
    phys_vec4 diag_sq;
    diag_sq.set_sub4(m_perp_axis1, m_perp_axis2);
    diag_sq.mul4(base_sphere_radius * 0.70710677f);
    phys_vec4 perp_axis_1;
    perp_axis_1.set_mul4(base_sphere_radius, m_perp_axis1);
    phys_vec4 perp_axis_2;
    perp_axis_2.set_mul4(base_sphere_radius, m_perp_axis2);

    perp_axis_1.zero_element(3);
    perp_axis_2.zero_element(3);
    diag_axis1.zero_element(3);
    diag_sq.zero_element(3);

    const float &shell_radius = m_radius;
    for (int cap = 0; cap < 2; ++cap) {
        phys_vec4 *s = reinterpret_cast<phys_vec4 *>(sphere_buffer + 8 * cap);
        phys_vec4 base_center = get_vertex(cap);
        base_center(3) = shell_radius;

        s[0].set_add4(base_center, perp_axis_1);
        s[1].set_add4(base_center, diag_axis1);
        s[2].set_add4(base_center, perp_axis_2);
        s[3].set_sub4(base_center, diag_sq);
        s[4].set_sub4(base_center, perp_axis_1);
        s[5].set_sub4(base_center, diag_axis1);
        s[6].set_sub4(base_center, perp_axis_2);
        s[7].set_add4(base_center, diag_sq);
    }

    {
        phys_vec4 &s0 = *reinterpret_cast<phys_vec4 *>(sphere_buffer + 16);
        phys_vec4 &s1 = *reinterpret_cast<phys_vec4 *>(sphere_buffer + 17);

        phys_vec4 symmetry_axis_unit;
        symmetry_axis_unit.set_cross(m_perp_axis1, m_perp_axis2);

        s0.set_add_mul4(get_vertex(0), symmetry_axis_unit, big_sphere_shift);
        s1.set_add_mul4(get_vertex(1), symmetry_axis_unit, -big_sphere_shift);
        s0(3) = big_sphere_shift + shell_radius;
        s1(3) = big_sphere_shift + shell_radius;
    }

    return sphere_buffer;
}

void phys_cylinder_shape::gather_memory_stats(phys_statistics_collector *collector) const
{
    collector->begin_stats_object("Cylinder", phys_statistics_collector::stat_category_shared, (const phys_ref_object *)this);
    collector->end_object();
}

namespace phys_math {  }

// @address 0x626448 ?min_value_rounded_up@@YAMXZ
float min_value_rounded_up()
{
    for (float f = 0.0f; f < 1.1f; f += 0.01f) {
        int int_value = phys_math::phys_to_int_fast(f);
        if (int_value)
            return f;
    }
    return 1.0f;
}


// @address 0x626490 ?preset_perpendicular_vec@phys_cylinder_shape@@QAAXXZ
void phys_cylinder_shape::preset_perpendicular_vec()
{
    phys_vec4 unit_axis;
    unit_axis.set_sub4(m_vertices[1], m_vertices[0]);
    unit_axis.normalize_xyz();
    phys_vec4_util::compute_perpendicular_vec(unit_axis, m_perp_axis1);
    m_perp_axis1.normalize_xyz();
    m_perp_axis2.set_cross(unit_axis, m_perp_axis1);
}

void phys_cylinder_shape::set_cylinder_radius(float radius)
{
    m_cylinder_radius = radius;
    m_vertices[0](3) = radius + m_radius;
    m_vertices[1](3) = radius + m_radius;
}

__forceinline void phys_cylinder_shape::decode_vertex_id(unsigned short code, phys_vec4 &result) const
{
    phys_bool base_a = (code >> 7) & 1;
    phys_bool sin_negative = (code >> 6) & 1;
    phys_bool cos_negative = (code >> 5) & 1;
    phys_bool sin_lesser = (code >> 4) & 1;
    float value = (float(code & 0x0f) + 0.5f) * s_virtualTesselationParameterInv;
    float sin, cos;
    if (sin_lesser) {
        sin = value;
        cos = sqrtf(1.0f - value * value);
    } else {
        cos = value;
        sin = sqrtf(1.0f - value * value);
    }
    if (!sin_negative)
        sin = -sin;
    if (!cos_negative)
        cos = -cos;
    phys_vec4 radius;
    radius.set_mul4(sin, m_perp_axis2);
    radius.add_scaled4(cos, m_perp_axis1);
    result.set_add_mul4(get_vertex(1 - base_a), radius, m_cylinder_radius);
}

// @address 0x6265d8 ?get_support_vertex_impl@phys_cylinder_shape@@UBAXABVphys_vec4@@AAVphys_hull_vertex@@@Z
void phys_cylinder_shape::get_support_vertex_impl(const phys_vec4 &direction, phys_hull_vertex &supporting_vertex_out) const
{
    phys_vec4 axis;
    axis.set_sub4(m_vertices[1], m_vertices[0]);
    float cos = m_perp_axis1.dot3(direction);
    float sin = m_perp_axis2.dot3(direction);
    const float length_sq_quad = sin * sin + cos * cos;
    if (length_sq_quad >= 1.192092896e-07f * 1.192092896e-07f) {
        const float inv_len = 1.0f / sqrtf(length_sq_quad);
        sin *= inv_len;
        cos *= inv_len;
    } else {
        sin = 0.0f;
        cos = 1.0f;
    }
    const int sin_negative = sin >= 0.0f ? 1 : 0;
    const int cos_negative = cos >= 0.0f ? 1 : 0;
    int sin_lesser;
    float value;
    {
        float abs_sin = fabsf(sin);
        float abs_cos = fabsf(cos);
        sin_lesser = abs_sin <= abs_cos;
        if (sin_lesser)
            value = abs_sin;
        else
            value = abs_cos;
    }
    const int base_a = float(axis.dot3(direction)) <= 0.0f ? 1 : 0;
    int code = 0;
    code += base_a << 7;
    code += sin_negative << 6;
    code += cos_negative << 5;
    code += sin_lesser << 4;
    assert_round_up_threshold(s_intRoundUpThreshold);
    int int_value = phys_math::phys_to_int_fast(value * s_virtualTesselationParameter - s_intRoundUpThreshold + 0.05f);
    code += int_value;
    decode_vertex_id((unsigned short)code, supporting_vertex_out);
    supporting_vertex_out.set_int24_w(code);
}

// @address 0x626838 ?vertex_ids_to_vertices_impl@phys_cylinder_shape@@UBAXPBGHPAVphys_hull_vertex@@@Z
void phys_cylinder_shape::vertex_ids_to_vertices_impl(const unsigned short *ids, int id_count, phys_hull_vertex *vertices_out) const
{
    for (int i = id_count - 1; i >= 0; --i, ++vertices_out, ++ids) {
        decode_vertex_id(ids[0], *vertices_out);
        vertices_out->set_int24_w(ids[0]);
    }
}

// @address 0x626be8 ?trace_ray_impl@phys_cylinder_shape@@UBA?AVphys_bool@@ABUphys_ray_input@@AAUphys_ray_output@@@Z
phys_bool phys_cylinder_shape::trace_ray_impl(const phys_ray_input &input, phys_ray_output &results) const
{
    do
    {
        float radius = m_cylinder_radius + m_radius;
        phys_vec4 ray_start;
        phys_vec4 cyl_ray_end;
        {
            phys_vec4 offset;
            offset.set_cross(m_perp_axis2, m_perp_axis1);
            phys_simd_scalar radius_simd(m_radius);
            offset.mul4(radius_simd);
            ray_start.set_add4(get_vertex(0), offset);
            cyl_ray_end.set_sub4(get_vertex(1), offset);
        }

        phys_vec4 closest;
        phys_capsule_shape::closest_point_on_segment(input.m_ray_from, ray_start, cyl_ray_end, closest);

        phys_vec4 join;
        join.set_sub4(input.m_ray_from, closest);
        float segment_to_cylinder_dist = join.length_3d();

        phys_vec4 simplex_point_a;
        phys_vec4 capsule_point_b;
        phys_vec4 dA;
        phys_vec4 dB;

        if (segment_to_cylinder_dist < radius) {
            phys_vec4 segment;
            segment.set_sub4(cyl_ray_end, ray_start);
            phys_vec4 cyl_to_start;
            cyl_to_start.set_sub4(input.m_ray_from, ray_start);
            float projection = segment.dot3(cyl_to_start);
            if (projection > 0.0f) {
                cyl_to_start.set_sub4(input.m_ray_from, cyl_ray_end);
                projection = segment.dot3(cyl_to_start);
                if (projection < 0.0f)
                    break;
            }
        }

        float inf_inf_dist_sq = 3.40282e+38f;
        float fraction_a;
        float fraction_b;
        dA.set_sub4(input.m_ray_to, input.m_ray_from);
        dB.set_sub4(cyl_ray_end, ray_start);
        phys_capsule_shape::closest_inf_segment_pair(input.m_ray_from, dA, ray_start, dB, inf_inf_dist_sq, fraction_a, fraction_b,
            simplex_point_a, capsule_point_b);

        if (inf_inf_dist_sq > radius * radius)
            break;

        {
            phys_vec4 axis;
            axis = dB;
            if (axis.length_sq_3d() <= 1.1920929e-07f)
                break;
            float capsule_axis_length = axis.normalize_xyz_with_length();

            float component = dA.dot3(axis);
            phys_vec4 capsule_flat_direction;
            capsule_flat_direction.set_add_mul4(dA, axis, phys_simd_scalar(-component));

            float distance = sqrtf(radius * radius - inf_inf_dist_sq);
            float flat_dir_len_sq = capsule_flat_direction.length_sq_3d();
            float intersect_param;
            if (flat_dir_len_sq != 0.0f)
                intersect_param = fraction_a - distance * static_cast<float>(capsule_flat_direction.inv_length_3d());
            else
                intersect_param = -1.0f;

            if (intersect_param >= results.m_hit_frac)
                break;

            float point_a_proj = ray_start.dot3(axis);
            phys_vec4 intersect_point;
            intersect_point.set_lerp4(input.m_ray_from, input.m_ray_to, phys_simd_scalar(intersect_param));
            float point_proj = intersect_point.dot3(axis);
            float capsule_point_height = point_proj - point_a_proj;

            if (intersect_param >= 0.0f && capsule_point_height > 0.0f && capsule_point_height < capsule_axis_length) {
                phys_vec4 axis_proj_point;
                axis_proj_point.set_lerp4(ray_start, cyl_ray_end, phys_simd_scalar(capsule_point_height / capsule_axis_length));
                phys_vec4 normal;
                normal.set_sub4(intersect_point, axis_proj_point);
                normal.normalize_xyz();
                results.m_normal = normal;
                results.m_hit_frac = intersect_param;
                results.m_shape_keys[results.m_shape_key_idx] = -1;
                return true;
            }

            float from_local_proj = input.m_ray_from.dot3(axis);
            float sign;
            phys_vec4 extra_vertex;
            if (from_local_proj < point_a_proj) {
                sign = -1.0f;
                extra_vertex = ray_start;
            } else {
                if (from_local_proj <= point_a_proj + capsule_axis_length)
                    break;
                sign = 1.0f;
                extra_vertex = cyl_ray_end;
            }

            phys_vec4 axis_to_ray_start;
            axis_to_ray_start.set_sub4(extra_vertex, input.m_ray_from);
            float projection = axis_to_ray_start.dot3(axis);
            float numerator = projection * sign * -1.0f;
            if (numerator < 0.0f)
                break;

            float da_along_axis = dA.dot3(axis);
            float denominator = da_along_axis * sign * -1.0f;
            if (numerator >= results.m_hit_frac * denominator)
                break;

            float fraction = numerator / denominator;
            phys_vec4 hit;
            hit.set_lerp4(input.m_ray_from, input.m_ray_to, phys_simd_scalar(fraction));
            phys_vec4 hit_offset;
            hit_offset.set_sub4(hit, extra_vertex);
            if (hit_offset.length_sq_3d() <= radius * radius) {
                results.m_normal.set_mul4(phys_simd_scalar(sign), axis);
                results.m_hit_frac = fraction;
                results.m_shape_keys[results.m_shape_key_idx] = -1;
                return true;
            }
        }
    } while (false);
    return false;
}


void phys_cylinder_shape::get_shape_centre_impl(phys_vec4 &shape_centre) const
{
    const phys_cylinder_shape *shape = this;
    shape_centre.set_add4(shape->m_vertices[0], shape->m_vertices[1]);
    float half = 0.5f;
    shape_centre.mul4(phys_simd_scalar(half));
}

void phys_cylinder_shape::get_first_vertex(phys_vec4 &vertex) const
{
    vertex = get_vertex(1);
}

void fixup_loaded_cylinder_shape(void *object)
{
    phys_fixup_tag flag;
    flag.m_fixup_finishing = 1;
    new (object) phys_cylinder_shape(flag);
}

const void *get_cylinder_shape_vtable()
{
    __declspec(align(16)) char storage[sizeof(phys_cylinder_shape)];
    phys_fixup_tag flag;
    flag.m_fixup_finishing = 0;
    new (storage) phys_cylinder_shape(flag);
    return *(const void **)storage;
}

// @address 0x6270d0 ??0phys_cylinder_shape@@QAA@ABVphys_vec4@@0MM@Z
phys_cylinder_shape::phys_cylinder_shape(const phys_vec4 &capsule_vertex_a, const phys_vec4 &capsule_vertex_b, float cylinder_radius, float padding_radius)
    : phys_convex_shape(PHYS_SHAPE_CYLINDER, padding_radius)
{
    if (s_intRoundUpThreshold < 0.0f)
        s_intRoundUpThreshold = 1.0f - min_value_rounded_up();
    m_vertices[0] = capsule_vertex_a;
    m_vertices[1] = capsule_vertex_b;
    set_cylinder_radius(cylinder_radius);
    preset_perpendicular_vec();
    m_cyl_heightfield_radius_factor = 0.8f;
}

// @address 0x627188 ??0phys_cylinder_shape@@QAA@Vphys_fixup_tag@@@Z
phys_cylinder_shape::phys_cylinder_shape(phys_fixup_tag flag)
    : phys_convex_shape(flag)
{
    if (flag.m_fixup_finishing != 0 && !(s_intRoundUpThreshold >= 0.0f))
        s_intRoundUpThreshold = 1.0f - min_value_rounded_up();
    m_type = PHYS_SHAPE_CYLINDER;
}


// @address 0x6271f8 ?get_aabb_impl@phys_cylinder_shape@@UBAXABVphys_transform@@MAAVphys_aabb@@@Z
void phys_cylinder_shape::get_aabb_impl(const phys_transform &local_to_world, float tolerance, phys_aabb &out) const
{
    phys_vec4 vertices[2];
    phys_vec4_util::transform_points(local_to_world, &get_vertex(0), 2, vertices);
    phys_vec4 tolerance_vec;
    tolerance_vec.set_all3(tolerance + m_radius);
    phys_vec4 axis;
    axis.set_sub4(vertices[1], vertices[0]);
    axis.normalize_xyz();
    phys_vec4 offsets;
    offsets(0) = sqrtf(1.01f - axis(0) * axis(0));
    offsets(1) = sqrtf(1.01f - axis(1) * axis(1));
    offsets(2) = sqrtf(1.01f - axis(2) * axis(2));
    offsets.mul4(m_cylinder_radius);
    tolerance_vec.add4(offsets);
    out.m_min_limit.set_min4(vertices[0], vertices[1]);
    out.m_min_limit.sub4(tolerance_vec);
    out.m_max_limit.set_max4(vertices[0], vertices[1]);
    out.m_max_limit.add4(tolerance_vec);
}

