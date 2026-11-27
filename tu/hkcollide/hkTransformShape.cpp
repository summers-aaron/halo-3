// hkcollide/hkTransformShape.obj: from the records 0x625268 0x625280 0x625310 0x625318 0x625350 0x625360 0x6253a0 0x625450 0x6254c0 0x625598 0x6256e8
// @flags /TP /O2 /Oc
// @address 0x6253a0 ??0phys_transform_shape@@QAA@PBVphys_shape@@ABVphys_transform@@@Z
#include "phys/phys_base.h"
// @address 0x625318 ?fixup_loaded_transform_shape@@YAXPAX@Z
// @address 0x625360 ?set_transform@phys_transform_shape@@QAAXABVphys_transform@@@Z
// @address 0x625450 ?get_aabb_impl@phys_transform_shape@@UBAXABVphys_transform@@MAAVphys_aabb@@@Z
// @address 0x625280 ?gather_memory_stats@phys_transform_shape@@UBAXPAVphys_statistics_collector@@@Z
// @address 0x625310 ?get_shape_container@phys_transform_shape@@UBAPBVphys_shape_container@@XZ
// @address 0x625350 ?get_transform_shape_vtable@@YAPBXXZ
#include <vectorintrinsics.h>
#include "phys/phys_collide.h"
#include "phys/phys_statistics_collector.h"

#include "phys/phys_transform_shape.h"
#include "phys/phys_collision_proxy.h"
#include "phys/phys_math.h"

void phys_transform_shape::gather_memory_stats(phys_statistics_collector *collector) const
{
    collector->begin_stats_object("TransformShp", phys_statistics_collector::stat_category_shared, this);
    collector->add_child_object("ChildShape", phys_statistics_collector::stat_category_shared, get_child_shape());
    collector->end_object();
}

const phys_shape_container *phys_transform_shape::get_shape_container() const
{
    return &m_child_container;
}

void fixup_loaded_transform_shape(void *object)
{
    phys_fixup_tag flag;
    flag.m_fixup_finishing = 1;
    new (object) phys_transform_shape(flag);
}

const void *get_transform_shape_vtable()
{
    phys_fixup_tag flag;
    union { __vector4 alignment; char storage[sizeof(phys_transform_shape)]; } buffer;
    new (buffer.storage) phys_transform_shape(flag);
    return *(const void **)buffer.storage;
}

__forceinline void phys_transform_shape::set_transform(const phys_transform &transform)
{
    m_body_transform = transform;
    m_rotation.set(m_body_transform.get_rotation());
}

// @address 0x6254c0 ?get_max_projection@phys_transform_shape@@UBAMABVphys_vec4@@@Z
// @address 0x6256e8 ?trace_ray_with_collector@phys_transform_shape@@UBAXABUphys_ray_input@@ABVphys_collision_proxy@@AAVphys_ray_hit_collector@@@Z

phys_transform_shape::phys_transform_shape(const phys_shape *bounding_child_shape, const phys_transform &transform)
    : phys_shape(PHYS_SHAPE_TRANSFORM), m_child_container(bounding_child_shape)
{
    set_transform(transform);
}

void phys_transform_shape::get_aabb_impl(const phys_transform &transform, float tolerance, phys_aabb &out) const
{
    phys_transform combined;
    combined.set_mul(transform, m_body_transform);
    m_child_container.get_single_child()->get_handle_aabb(combined, tolerance, out);
}

// The inverse rotation of a direction, as hkVector4::setRotatedInverseDir inlines it here.


// A point taken out of the local frame of transform: rotation * point + translation.

// 0x6254c0
float phys_transform_shape::get_max_projection(const phys_vec4 &direction) const
{
    phys_vec4 local_dir;
    local_dir.set_rotated_inverse_dir(m_body_transform.m_rotation, direction);
    float local_projection = get_child_shape()->get_max_projection(local_dir);
    phys_vec4 local_support;
    local_support.set_mul4(local_projection, local_dir);
    phys_vec4 support;
    support.set_transformed_pos(m_body_transform, local_support);
    float projection;
    __stvewx(__vmsum3fp(support.m_simd_quad, direction.m_simd_quad), &projection, 0);
    return projection;
}

// @address 0x625598 ?trace_ray_impl@phys_transform_shape@@UBA?AVphys_bool@@ABUphys_ray_input@@AAUphys_ray_output@@@Z
phys_bool phys_transform_shape::trace_ray_impl(const phys_ray_input &input, phys_ray_output &output) const
{
    phys_ray_input child_space_ray_input = input;
    child_space_ray_input.m_ray_from.set_transformed_inverse_pos(m_body_transform, input.m_ray_from);
    child_space_ray_input.m_ray_to.set_transformed_inverse_pos(m_body_transform, input.m_ray_to);
    output.change_key_level(1);
    phys_bool hit = get_child_shape()->trace_ray(child_space_ray_input, output);
    output.change_key_level(-1);
    if (hit) {
        phys_vec4 normal = output.m_normal;
        output.m_normal.vec_set_rotated_dir(m_body_transform.m_rotation, normal);
        output.set_cached_key(0);
    }
    return hit;
}

// 0x6256e8
void phys_transform_shape::trace_ray_with_collector(const phys_ray_input &input, const phys_collision_proxy &body, phys_ray_hit_collector &collector) const
{
    phys_ray_input child_space_ray_input = input;
    child_space_ray_input.m_ray_from.set_transformed_inverse_pos(m_body_transform, input.m_ray_from);
    child_space_ray_input.m_ray_to.set_transformed_inverse_pos(m_body_transform, input.m_ray_to);
    phys_transform combined;
    combined.set_mul(body.get_transform(), m_body_transform);
    phys_collision_proxy child_proxy(&body);
    child_proxy.m_motion = &combined;
    child_proxy.set_proxy_shape(get_child_shape(), 0);
    get_child_shape()->trace_ray_with_collector(child_space_ray_input, child_proxy, collector);
}

// @address 0x625268 ?destroy_loaded_transform_shape@@YAXPAX@Z
void destroy_loaded_transform_shape(void *p)
{
    static_cast<phys_transform_shape *>(p)->~phys_transform_shape();
}

