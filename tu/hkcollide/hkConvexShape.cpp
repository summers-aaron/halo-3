// hkcollide/hkConvexShape.obj: from the records 0x61acb0 0x61acb8 0x61ad28 0x61ada8
// @compiler xdk6130
// @flags /TP /O2 /Oc
#include "phys/phys_base.h"
// @address 0x61acb0 ?weld_contact_point_impl@phys_convex_shape@@MBAHPAGAAEAAVphys_vec4@@PBVphys_transform@@PBV1@32@Z
// @address 0x61acb8 ?get_max_projection@phys_convex_shape@@UBAMABVphys_vec4@@@Z
// @address 0x61ad28 ?get_shape_centre_impl@phys_convex_shape@@MBAXAAVphys_vec4@@@Z
// @address 0x61ada8 ?trace_ray_with_collector@phys_convex_shape@@UBAXABUphys_ray_input@@ABVphys_collision_proxy@@AAVphys_ray_hit_collector@@@Z
#include <vectorintrinsics.h>
#include "phys/phys_collide.h"
#include "phys/phys_sphere.h"
#include "phys/phys_statistics_collector.h"

// @address 0x7246fc ?phys_convex_shape_default_radius@@3MA
float phys_convex_shape_default_radius = 0.05f;

float phys_convex_shape::get_max_projection(const phys_vec4 &direction) const
{
    phys_hull_vertex supporting_vertex;
    get_support_vertex_impl(direction, supporting_vertex);
    __vector4 projection = __vmsum3fp(supporting_vertex.m_simd_quad, direction.m_simd_quad);
    float distance;
    __stvewx(projection, (char *)&distance, 0);
    return m_radius + distance;
}

void phys_convex_shape::trace_ray_with_collector(const phys_ray_input &local_ray_input, const phys_collision_proxy &collision_proxy,
    phys_ray_hit_collector &collector) const
{
    phys_ray_output results;
    results.m_hit_frac = collector.m_early_out_hit_frac;

    if (trace_ray(local_ray_input, results))
    {
        results.m_normal.set_rotated_dir(collision_proxy.get_transform().get_rotation(), results.m_normal);
        collector.add_ray_hit(collision_proxy, results);
    }
}

int phys_convex_shape::weld_contact_point_impl(unsigned short *feature_points, unsigned char &feature_point_count, phys_vec4 &contact_point_world,
    const phys_transform *this_obj_transform, const phys_convex_shape *colliding_shape, const phys_transform *colliding_transform,
    phys_vec4 &separating_normal_in_out) const
{
    return 2;
}

void phys_convex_shape::get_shape_centre_impl(phys_vec4 &centre_out) const
{
    phys_aabb aabb;
    get_handle_aabb(phys_transform::get_identity(), 0.0f, aabb);
    centre_out.set_add4(aabb.m_max_limit, aabb.m_min_limit);
    centre_out.mul4(0.5f);
}
