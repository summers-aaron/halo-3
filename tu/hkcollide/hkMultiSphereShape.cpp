// hkcollide/hkMultiSphereShape.obj: from the records 0x621d08 0x621d20 0x621d28 0x621d88 0x621ec8 0x622050 0x6220d0 0x6220f8 0x622108 0x622168 0x6222e0 0x622328
// @flags /TP /O2 /Oc
#include "phys/phys_base_types.h"
// @address 0x622108 ??0phys_multi_sphere_shape@@QAA@PBVphys_vec4@@H@Z
// @address 0x6222e0 ?get_collision_spheres_impl@phys_multi_sphere_shape@@UBAPBVphys_sphere@@PAV2@@Z
// @address 0x621d08 ?destroy_loaded_multi_sphere_shape@@YAXPAX@Z
// @address 0x621d20 ?get_collision_sphere_count_impl@phys_multi_sphere_shape@@UBAHXZ
// @address 0x621d28 ?gather_memory_stats@phys_multi_sphere_shape@@UBAXPAVphys_statistics_collector@@@Z
// @address 0x622050 ?trace_ray_impl@phys_multi_sphere_shape@@UBA?AVphys_bool@@ABUphys_ray_input@@AAUphys_ray_output@@@Z
// @address 0x6220d0 ?fixup_loaded_multi_sphere_shape@@YAXPAX@Z
// @address 0x6220f8 ?get_multi_sphere_shape_vtable@@YAPBXXZ
// @address 0x621d88 ?trace_ray_internal@@YAHABUphys_ray_input@@PBVphys_vec4@@HPAMPAH@Z
// @address 0x621ec8 ?get_best_hit@@YAHPBVphys_vec4@@HPAMPAHHABUphys_ray_input@@AAUphys_ray_output@@@Z
// @address 0x622168 ?get_aabb_impl@phys_multi_sphere_shape@@UBAXABVphys_transform@@MAAVphys_aabb@@@Z
// @address 0x622328 ?trace_ray_with_collector@phys_multi_sphere_shape@@UBAXABUphys_ray_input@@ABVphys_collision_proxy@@AAVphys_ray_hit_collector@@@Z
#include <vectorintrinsics.h>
#include <ppcintrinsics.h>
#include <math.h>
#include "phys/phys_multi_sphere_shape.h"
#include "phys/phys_math.h"
#include "phys/phys_collide.h"
#include "phys/phys_aabb.h"
#include "phys/phys_statistics_collector.h"


#include "phys/phys_vec4_util.h"
extern "C" {
}

namespace {
}

void destroy_loaded_multi_sphere_shape(void *object)
{
    static_cast<phys_multi_sphere_shape *>(object)->~phys_multi_sphere_shape();
}

int phys_multi_sphere_shape::get_collision_sphere_count_impl() const
{
    return get_sphere_count();
}

void phys_multi_sphere_shape::gather_memory_stats(phys_statistics_collector *collector) const
{
    collector->begin_stats_object("MultiSphere", phys_statistics_collector::stat_category_shared, this);
    collector->end_object();
}

int trace_ray_internal(const phys_ray_input &input, const phys_vec4 *spheres, int count, float *hit_fractions, int *indices)
{
    int hit_count = 0;
    for (int i = 0; i < count; i++) {
        phys_vec4 local_from = input.m_ray_from;
        phys_vec4 local_to = input.m_ray_to;
        local_from.sub4(spheres[i]);
        local_to.sub4(spheres[i]);
        phys_vec4 dir;
        dir.set_sub4(local_to, local_from);
        float a = local_from.length_sq_3d();
        float b = dir.dot3(local_from);
        float b2 = b * 2.0f;
        if (b2 < 0.0f) {
            float c = dir.length_sq_3d();
            float radius = spheres[i](3);
            float capsule_determinant = b2 * b2 - (a - radius * radius) * c * 4.0f;
            if (capsule_determinant > 0.0f) {
                float t = (-b2 - sqrtf(capsule_determinant)) * 0.5f;
                if (t < c && t >= 0.0f) {
                    hit_fractions[hit_count] = t / c;
                    indices[hit_count] = i;
                    hit_count++;
                }
            }
        }
    }
    return hit_count;
}

int get_best_hit(const phys_vec4 *spheres, int count, float *hit_fractions, int *indices, int hit_count, const phys_ray_input &input,
    phys_ray_output &output)
{
    float best_hit_frac = output.m_hit_frac;
    int best_sphere_idx = -1;
    for (int i = 0; i < hit_count; i++) {
        if (hit_fractions[i] < best_hit_frac) {
            best_hit_frac = hit_fractions[i];
            best_sphere_idx = i;
        }
    }
    if (best_sphere_idx != -1) {
        output.set_cached_key(0xffffffff);
        int i = indices[best_sphere_idx];
        output.m_hit_frac = best_hit_frac;
        phys_vec4 local_to;
        local_to.set_sub4(input.m_ray_to, spheres[i]);
        phys_vec4 local_from;
        local_from.set_sub4(input.m_ray_from, spheres[i]);
        output.m_normal.set_lerp4(local_from, local_to, phys_simd_scalar(best_hit_frac));
        phys_simd_scalar inv_radius(1.0f / spheres[i](3));
        output.m_normal.mul4(inv_radius);
        output.m_extra_info = i;
        return best_sphere_idx;
    }
    return -1;
}


phys_bool phys_multi_sphere_shape::trace_ray_impl(const phys_ray_input &input, phys_ray_output &output) const
{
    int indices[8];
    float hit_fractions[8];
    int hit_count;
    hit_count = trace_ray_internal(input, m_sphere_array, m_sphere_count, hit_fractions, indices);
    return phys_bool(get_best_hit(m_sphere_array, m_sphere_count, hit_fractions, indices, hit_count, input, output) != -1);
}

void fixup_loaded_multi_sphere_shape(void *object)
{
    phys_fixup_tag flag;
    flag.m_fixup_finishing = 1;
    new (object) phys_multi_sphere_shape(flag);
}

const void *get_multi_sphere_shape_vtable()
{
    phys_fixup_tag flag;
    union { __vector4 alignment; char storage[sizeof(phys_multi_sphere_shape)]; } buffer;
    new (buffer.storage) phys_multi_sphere_shape(flag);
    return *(const void **)buffer.storage;
}

phys_multi_sphere_shape::phys_multi_sphere_shape(const phys_vec4 *spheres, int count)
    : phys_sphere_rep_shape(PHYS_SHAPE_MULTI_SPHERE)
{
    for (int index = 0; index < count; index++)
        m_sphere_array[index] = spheres[index];
    m_sphere_count = count;
}

const phys_sphere *phys_multi_sphere_shape::get_collision_spheres_impl(phys_sphere *spheres) const
{
    for (int index = 0; index < m_sphere_count; index++)
        *(phys_vec4 *)&spheres[index] = m_sphere_array[index];
    return spheres;
}

void phys_multi_sphere_shape::get_aabb_impl(const phys_transform &local_to_world, float tolerance, phys_aabb &out) const
{
    phys_vec4 world_spheres[8];
    phys_vec4_util::transform_points(local_to_world, &m_sphere_array[0], get_sphere_count(), &world_spheres[0]);
    phys_vec4 abs_bounds_min;
    abs_bounds_min.set_all3(3.40282e+38f);
    phys_vec4 abs_bounds_max;
    abs_bounds_max.set_all3(-3.40282e+38f);
    for (int i = 0; i < m_sphere_count; i++) {
        phys_vec4 r;
        r.set_broadcast3_clobber_w(m_sphere_array[i], 3);
        phys_vec4 min;
        min.set_sub4(world_spheres[i], r);
        phys_vec4 max;
        max.set_add4(world_spheres[i], r);
        abs_bounds_min.set_min4(abs_bounds_min, min);
        abs_bounds_max.set_max4(abs_bounds_max, max);
    }
    phys_vec4 tol_vec4;
    tol_vec4.set_all3(tolerance);
    out.m_min_limit.set_sub4(abs_bounds_min, tol_vec4);
    out.m_max_limit.set_add4(abs_bounds_max, tol_vec4);
}

void phys_multi_sphere_shape::trace_ray_with_collector(const phys_ray_input &local_ray_input,
    const phys_collision_proxy &collision_proxy, phys_ray_hit_collector &collector) const
{
    float dist[8];
    int idx[8];
    int hit_total = trace_ray_internal(local_ray_input, m_sphere_array, m_sphere_count, dist, idx);
    phys_ray_output results;
    while (hit_total) {
        results.reset();
        int best_slot = get_best_hit(m_sphere_array, m_sphere_count, dist, idx, hit_total, local_ray_input, results);
        results.m_normal.set_rotated_dir(collision_proxy.get_transform().get_rotation(), results.m_normal);
        collector.add_ray_hit(collision_proxy, results);
        --hit_total;
        dist[best_slot] = dist[hit_total];
        idx[best_slot] = idx[hit_total];
    }
}
