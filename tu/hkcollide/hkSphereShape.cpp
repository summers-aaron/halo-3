// hkcollide/hkSphereShape.obj: from the records 0x622ec8 0x622ee0 0x622f40 0x622f48 0x622f58 0x622f88 0x622f98 0x622fa8 0x622fe8 0x623018 0x623050 0x623078 0x623088
// @flags /TP /O2 /Oc
#include "phys/phys_base.h"
// @address 0x622f48 ?get_support_vertex_impl@phys_sphere_shape@@UBAXABVphys_vec4@@AAVphys_hull_vertex@@@Z
// @address 0x622f58 ?vertex_ids_to_vertices_impl@phys_sphere_shape@@UBAXPBGHPAVphys_hull_vertex@@@Z
// @address 0x622f88 ?get_shape_centre_impl@phys_sphere_shape@@UBAXAAVphys_vec4@@@Z
// @address 0x622f98 ?get_first_vertex@phys_sphere_shape@@UBAXAAVphys_vec4@@@Z
// @address 0x622fa8 ?get_aabb_impl@phys_sphere_shape@@UBAXABVphys_transform@@MAAVphys_aabb@@@Z
// @address 0x622fe8 ?get_collision_spheres_impl@phys_sphere_shape@@UBAPBVphys_sphere@@PAV2@@Z
#include <vectorintrinsics.h>
#include <math.h>
#include "phys/phys_statistics_collector.h"
#include "phys/phys_aabb.h"
#include "phys/phys_collide.h"
// @address 0x622ec8 ?destroy_loaded_sphere_shape@@YAXPAX@Z
// @address 0x622ee0 ?gather_memory_stats@phys_sphere_shape@@UBAXPAVphys_statistics_collector@@@Z
// @address 0x622f40 ?get_collision_sphere_count_impl@phys_sphere_shape@@UBAHXZ
// @address 0x623018 ?trace_ray_impl@phys_sphere_shape@@UBA?AVphys_bool@@ABUphys_ray_input@@AAUphys_ray_output@@@Z

#include "phys/phys_sphere_util.h"
#include "phys/phys_collision_proxy.h"



// @address 0x61ada8 ?trace_ray_with_collector@phys_convex_shape@@UBAXABUphys_ray_input@@ABVphys_collision_proxy@@AAVphys_ray_hit_collector@@@Z
// @address 0x42d298 ?get_shape_container@phys_shape@@UBAPBVphys_shape_container@@XZ
// @address 0x61acb0 ?weld_contact_point_impl@phys_convex_shape@@MBAHPAGAAEAAVphys_vec4@@PBVphys_transform@@PBV1@32@Z
// @address 0x623088 ??0phys_sphere_shape@@QAA@M@Z


void destroy_loaded_sphere_shape(void *object)
{
    static_cast<phys_sphere_shape *>(object)->~phys_sphere_shape();
}

void phys_sphere_shape::gather_memory_stats(phys_statistics_collector *collector) const
{
    collector->begin_stats_object("SphereShape", phys_statistics_collector::stat_category_shared, this);
    collector->end_object();
}

int phys_sphere_shape::get_collision_sphere_count_impl() const
{
    return 1;
}

void phys_sphere_shape::get_support_vertex_impl(const phys_vec4 &direction, phys_hull_vertex &vertex) const
{
    __vector4 v = vertex.m_simd_quad;
#if defined(_XBOX) && !defined(__clang__)
    vertex.m_simd_quad = __vxor(v, v);
#else
    vertex.m_simd_quad = __vzero();
#endif
}

void phys_sphere_shape::vertex_ids_to_vertices_impl(const unsigned short *ids, int count, phys_hull_vertex *vertices) const
{
    for (int i = count - 1; i >= 0; i--) {
        vertices->set_zero4();
        vertices++;
    }
}

void phys_sphere_shape::get_shape_centre_impl(phys_vec4 &shape_centre) const
{
    __vector4 v = shape_centre.m_simd_quad;
#if defined(_XBOX) && !defined(__clang__)
    v = __vxor(v, v);
#else
    v = __vzero();
#endif
    shape_centre.m_simd_quad = v;
}

void phys_sphere_shape::get_first_vertex(phys_vec4 &vertex) const
{
    __vector4 v = vertex.m_simd_quad;
#if defined(_XBOX) && !defined(__clang__)
    v = __vxor(v, v);
#else
    v = __vzero();
#endif
    vertex.m_simd_quad = v;
}

void phys_sphere_shape::get_aabb_impl(const phys_transform &transform, float tolerance, phys_aabb &out) const
{
    union { float f[4]; __vector4 q; } u;
    u.f[0] = m_radius + tolerance;
    __vector4 r = __vspltw(u.q, 0);
    out.m_min_limit.m_simd_quad = __vsubfp(transform.get_translation().m_simd_quad, r);
    out.m_max_limit.m_simd_quad = __vaddfp(transform.get_translation().m_simd_quad, r);
}

const phys_sphere *phys_sphere_shape::get_collision_spheres_impl(phys_sphere *spheres) const
{
    phys_vec4 center;
    center.set(0.0f, 0.0f, 0.0f, m_radius);
    spheres->get_position_and_radius() = center;
    return spheres;
}

phys_bool phys_sphere_shape::trace_ray_impl(const phys_ray_input &input, phys_ray_output &output) const
{
    return phys_sphere_util::trace_sphere_ray(m_radius, input, output);
}



// @address 0x623050 ?fixup_loaded_sphere_shape@@YAXPAX@Z
void fixup_loaded_sphere_shape(void *object)
{
    phys_fixup_tag flag;
    flag.m_fixup_finishing = 1;
    if (object) {
        new (object) phys_sphere_shape(flag);
    }
}

// @address 0x623078 ?get_sphere_shape_vtable@@YAPBXXZ
const void *get_sphere_shape_vtable()
{
    phys_fixup_tag flag;
    phys_sphere_shape sphere(flag);
    return *reinterpret_cast<const void *const *>(&sphere);
}

phys_sphere_shape::phys_sphere_shape(float radius) : phys_convex_shape(PHYS_SHAPE_SPHERE, radius)
{
}
