// hkcollide/hkBoxShape.obj: from the records 0x61f588 0x61f5a0 0x61f600 0x61f608 0x61f618 0x61f658 0x61f668 0x61f708 0x61f7d8 0x61f7e8 0x61f840 0x61f8b8 0x61fbb0 0x61fbd8 0x61fbe8
// @flags /TP /O2 /Oc
// @address 0x61f618 ?set_half_extents@phys_box_shape@@QAAXABVphys_vec4@@@Z
// @address 0x61fbe8 ??0phys_box_shape@@QAA@ABVphys_vec4@@M@Z
#include "phys/phys_base.h"
// @address 0x61f588 ?destroy_loaded_box_shape@@YAXPAX@Z
// @address 0x61f5a0 ?gather_memory_stats@phys_box_shape@@UBAXPAVphys_statistics_collector@@@Z
// @address 0x61f600 ?get_collision_sphere_count_impl@phys_box_shape@@UBAHXZ
// @address 0x61f708 ?get_support_vertex_impl@phys_box_shape@@UBAXABVphys_vec4@@AAVphys_hull_vertex@@@Z
// @address 0x61f7e8 ?vertex_ids_to_vertices_impl@phys_box_shape@@UBAXPBGHPAVphys_hull_vertex@@@Z
// @address 0x61fbb0 ?fixup_loaded_box_shape@@YAXPAX@Z
// @address 0x61fbd8 ?get_box_shape_vtable@@YAPBXXZ
#include <ppcintrinsics.h>
#include <vectorintrinsics.h>
#include <math.h>
#include "phys/phys_collide.h"
#include "phys/phys_statistics_collector.h"
#include "phys/phys_aabb_util.h"
#include "phys/phys_vec4_util.h"
#include "phys/phys_aabb_types.h"
#include "phys/phys_math.h"

extern "C" {
}
typedef float phys_real;

template<typename T> inline T phys_math_min2(T a, T b)
{
    return a < b ? a : b;
}
// @rdata 0x82015160..0x820151e0
__declspec(align(16)) static const float box_vertex_directions[8][4] = {
    {1, 1, 1, 0}, {1, 1, -1, 0}, {1, -1, 1, 0}, {1, -1, -1, 0},
    {-1, 1, 1, 0}, {-1, 1, -1, 0}, {-1, -1, 1, 0}, {-1, -1, -1, 0}
};
// @rdata-end
// @rdata 0x820151e0..0x82015260
__declspec(align(16)) static const float box_sphere_directions[8][4] = {
    {1, 1, 1, 1}, {-1, 1, 1, 1}, {1, -1, 1, 1}, {-1, -1, 1, 1},
    {1, 1, -1, 1}, {-1, 1, -1, 1}, {1, -1, -1, 1}, {-1, -1, -1, 1}
};
// @rdata-end
static __forceinline phys_vec4 box_vertex_dir(unsigned long box_vertex_byte_offset)
{
    phys_vec4 direction;
    direction.m_simd_quad = __lvx(box_vertex_directions, box_vertex_byte_offset);
    return direction;
}

void destroy_loaded_box_shape(void *object)
{
    static_cast<phys_box_shape *>(object)->~phys_box_shape();
}

void phys_box_shape::gather_memory_stats(phys_statistics_collector *collector) const
{
    collector->begin_stats_object("BoxShape", phys_statistics_collector::stat_category_shared, this);
    collector->end_object();
}

int phys_box_shape::get_collision_sphere_count_impl() const
{
    return 8;
}

inline int phys_box_shape::compute_dma_size(int buffer_size_left) const
{
    return sizeof(phys_box_shape);
}

// @address 0x61f608 ??1phys_box_shape@@UAA@XZ
phys_box_shape::~phys_box_shape()
{
}

void phys_box_shape::set_half_extents(const phys_vec4 &box_half_extents)
{
    m_half_extents = box_half_extents;
    float m = m_half_extents(0) < m_half_extents(1) ? m_half_extents(0) : m_half_extents(1);
    m_half_extents(3) = m;
    m_half_extents(3) = m_half_extents(2) < m ? m_half_extents(2) : m;
}

// @address 0x61f658 ?get_first_vertex@phys_box_shape@@UBAXAAVphys_vec4@@@Z
void phys_box_shape::get_first_vertex(phys_vec4 &value) const
{
    value = m_half_extents;
}

inline void phys_aabb_util::compute_bounds(const phys_transform &local_to_world, const phys_vec4 &box_half_extents, float extra_radius, phys_aabb &aabb_out)
{
    phys_vec4 half_x; half_x.set_broadcast3_clobber_w(box_half_extents, 0);
    phys_vec4 half_y; half_y.set_broadcast3_clobber_w(box_half_extents, 1);
    phys_vec4 half_z; half_z.set_broadcast3_clobber_w(box_half_extents, 2);
    phys_vec4 world_axis_x; world_axis_x.set_mul4(half_x, local_to_world.get_rotation().get_column(0));
    phys_vec4 world_axis_y; world_axis_y.set_mul4(half_y, local_to_world.get_rotation().get_column(1));
    phys_vec4 world_axis_z; world_axis_z.set_mul4(half_z, local_to_world.get_rotation().get_column(2));
    world_axis_x.set_abs4(world_axis_x);
    world_axis_y.set_abs4(world_axis_y);
    world_axis_z.set_abs4(world_axis_z);
    phys_vec4 max = aabb_out.m_max_limit;
    phys_vec4 min = aabb_out.m_min_limit;
    phys_vec4 extra; extra.set_all3(extra_radius);
    world_axis_z.add4(extra);
    max.set_add4(world_axis_x, world_axis_y);
    max.add4(world_axis_z);
    min.set_neg4(max);
    max.add4(local_to_world.get_translation());
    min.add4(local_to_world.get_translation());
    aabb_out.m_max_limit = max;
    aabb_out.m_min_limit = min;
}
// @address 0x61f668 ?get_aabb_impl@phys_box_shape@@UBAXABVphys_transform@@MAAVphys_aabb@@@Z
void phys_box_shape::get_aabb_impl(const phys_transform &local_to_world, float tolerance, phys_aabb &out) const
{
    phys_aabb_util::compute_bounds(local_to_world, m_half_extents, m_radius + tolerance, out);
}

// @address 0x61f7d8 ?get_shape_centre_impl@phys_box_shape@@UBAXAAVphys_vec4@@@Z
void phys_box_shape::get_shape_centre_impl(phys_vec4 &value) const
{
    __vector4 v = value.m_simd_quad;
#if defined(_XBOX) && !defined(__clang__)
    v = __vxor(v, v);
#else
    v = __vzero();
#endif
    value.m_simd_quad = v;
}

void phys_box_shape::vertex_ids_to_vertices_impl(const unsigned short *ids, int count, phys_hull_vertex *vertices) const
{
    const phys_box_shape *box = this;
    for (int i = count - 1; i >= 0; --i) {
        unsigned long box_vertex_byte_offset = *ids << 3;
        vertices->set_mul4(box->m_half_extents, box_vertex_dir(box_vertex_byte_offset));
        vertices->set_int24_w(*ids);
        ++vertices;
        ++ids;
    }
}

void fixup_loaded_box_shape(void *object)
{
    if (object) {
        new (object) phys_box_shape(phys_fixup_tag());
    }
}

const void *get_box_shape_vtable()
{
    __declspec(align(16)) char storage[sizeof(phys_box_shape)];
    phys_fixup_tag flag;
    flag.m_fixup_finishing = 0;
    new (storage) phys_box_shape(flag);
    return *(const void **)storage;
}

phys_box_shape::phys_box_shape(const phys_vec4 &box_half_extents, phys_real radius) : phys_convex_shape(PHYS_SHAPE_BOX, radius)
{
    m_radius = radius;
    m_object_ref_count = 1;
    m_user_data = 0;
    m_type = PHYS_SHAPE_BOX;
    memcpy(&m_half_extents(0), &box_half_extents(0), sizeof(float));
    memcpy(&m_half_extents(1), &box_half_extents(1), sizeof(float));
    memcpy(&m_half_extents(2), &box_half_extents(2), sizeof(float));
    memcpy(&m_half_extents(3), &box_half_extents(3), sizeof(float));
    m_half_extents(3) = m_half_extents(0) < m_half_extents(1) ? m_half_extents(0) : m_half_extents(1);
    m_half_extents(3) = phys_math_min2(m_half_extents(2), m_half_extents(3));
}

// @address 0x61f840 ?get_collision_spheres_impl@phys_box_shape@@UBAPBVphys_sphere@@PAV2@@Z
const phys_sphere *phys_box_shape::get_collision_spheres_impl(phys_sphere *sphere_buffer) const
{
    phys_sphere *s = sphere_buffer;
    phys_vec4 radius = m_half_extents;
    radius(3) = m_radius;
    const phys_vec4 *vertex_dirs = reinterpret_cast<const phys_vec4 *>(box_sphere_directions);
    for (int i = 3; i >= 0; i--) {
        phys_vec4 v0;
        v0.set_mul4(radius, vertex_dirs[0]);
        phys_vec4 v1;
        v1.set_mul4(radius, vertex_dirs[1]);
        s[0].m_sphere_center = v0;
        s[1].m_sphere_center = v1;
        s += 2;
        vertex_dirs += 2;
    }
    return sphere_buffer;
}

void phys_box_shape::get_support_vertex_impl(const phys_vec4 &direction, phys_hull_vertex &supporting_vertex_out) const
{
    phys_vec4 support = m_half_extents;
    phys_vec4_util::mul_signs4(support, direction);
    int vertex_id = support.compare_less_zero4().get_mask(phys_vec4_cmp::CMP_MASK_XYZ);
    support.set_int24_w(vertex_id);
    supporting_vertex_out.m_simd_quad = support.m_simd_quad;
}

// @address 0x61f8b8 ?trace_ray_impl@phys_box_shape@@UBA?AVphys_bool@@ABUphys_ray_input@@AAUphys_ray_output@@@Z
phys_bool phys_box_shape::trace_ray_impl(const phys_ray_input &input, phys_ray_output &results) const
{
    phys_vec4 radius;
    radius.flags_set_all(m_radius);
    phys_vec4 d;
    d.set_add4(m_half_extents, radius);
    phys_vec4 neg_extents;
    neg_extents.set_neg4(d);

    int from_out_pos = d.compare_less_equal4(input.m_ray_from).get_mask(phys_vec4_cmp::CMP_MASK_XYZ);
    int from_out_neg = input.m_ray_from.compare_less_equal4(neg_extents).get_mask(phys_vec4_cmp::CMP_MASK_XYZ);
    int to_out_pos = d.compare_less_equal4(input.m_ray_to).get_mask(phys_vec4_cmp::CMP_MASK_XYZ);
    int to_out_neg = input.m_ray_to.compare_less_equal4(neg_extents).get_mask(phys_vec4_cmp::CMP_MASK_XYZ);

    if (((from_out_pos & to_out_pos) == 0) && ((from_out_neg & to_out_neg) == 0) && ((from_out_pos | from_out_neg) != 0))
    {
        int mask[3] = { phys_vec4_cmp::CMP_MASK_X, phys_vec4_cmp::CMP_MASK_Y, phys_vec4_cmp::CMP_MASK_Z };
        int out = from_out_pos | to_out_pos;
        int from_outside = from_out_pos;
        phys_real near_min_frac = 0.0f;
        phys_real far_max_frac = results.m_hit_frac;
        phys_real sign = 1.0f;
        phys_vec4 box_hit_normal;

        for (int j = 1; j >= 0; sign = -1.0f, j--)
        {
            for (int i = 2; i >= 0; i--)
            {
                int m = mask[i];
                if (out & m)
                {
                    phys_real d0 = neg_extents(i);
                    phys_real a = sign * input.m_ray_from(i) + d0;
                    phys_real b = sign * input.m_ray_to(i) + d0;
                    phys_real t = a / (a - b);
                    if (from_outside & m)
                    {
                        if (t >= near_min_frac)
                        {
                            near_min_frac = t;
                            box_hit_normal.set_zero4();
                            box_hit_normal(i) = sign;
                        }
                    }
                    else
                    {
                        far_max_frac = phys_math::min_of_two(far_max_frac, t);
                    }
                    if (far_max_frac < near_min_frac)
                    {
                        return false;
                    }
                }
            }
            out = from_out_neg | to_out_neg;
            from_outside = from_out_neg;
        }

        results.m_normal = box_hit_normal;
        results.m_hit_frac = near_min_frac;
        results.set_cached_key(0xffffffff);
        return true;
    }
    return false;
}

