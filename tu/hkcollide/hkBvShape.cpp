// hkcollide/hkBvShape.obj: from the records 0x61f288 0x61f2a0 0x61f330 0x61f338 0x61f3e8 0x61f400 0x61f480 0x61f4d0 0x61f508 0x61f518
// @flags /TP /O2 /Oc
// @address 0x61f518 ??0phys_bounding_shape@@QAA@PBVphys_shape@@0@Z
#include "phys/phys_base_types.h"
#include "physics/havok_halo_external.h"
#include "phys/phys_collide.h"
#include "phys/phys_statistics_collector.h"
// @address 0x61f4d0 ?fixup_loaded_bounding_shape@@YAXPAX@Z

// @address 0x61f2a0 ?gather_memory_stats@phys_bounding_shape@@UBAXPAVphys_statistics_collector@@@Z
// @address 0x61f330 ?get_shape_container@phys_bounding_shape@@UBAPBVphys_shape_container@@XZ
// @address 0x61f3e8 ?get_aabb_impl@phys_bounding_shape@@UBAXABVphys_transform@@MAAVphys_aabb@@@Z
// @address 0x61f400 ?trace_ray_impl@phys_bounding_shape@@UBA?AVphys_bool@@ABUphys_ray_input@@AAUphys_ray_output@@@Z
// @address 0x61f480 ?trace_ray_with_collector@phys_bounding_shape@@UBAXABUphys_ray_input@@ABVphys_collision_proxy@@AAVphys_ray_hit_collector@@@Z
// @address 0x61f508 ?get_bounding_shape_vtable@@YAPBXXZ

#include "phys/phys_bounding_shape.h"
#include "phys/phys_aabb_types.h"
#include "phys/phys_object.h"
#include "phys/phys_collision_proxy.h"
#include "phys/phys_math.h"

// @address 0x61f338 ??1phys_bounding_shape@@UAA@XZ
phys_bounding_shape::~phys_bounding_shape()
{
    m_bounding_shape->object_remove_ref();
}

void phys_bounding_shape::gather_memory_stats(phys_statistics_collector *collector) const
{
    collector->begin_stats_object("BvShape", phys_statistics_collector::stat_category_shared, this);
    collector->add_child_object("Child", phys_statistics_collector::stat_category_shared, get_child_shape());
    collector->end_object();
}

const phys_shape_container *phys_bounding_shape::get_shape_container() const
{
    return &m_child_container;
}

void phys_bounding_shape::get_aabb_impl(const phys_transform &transform, float tolerance, phys_aabb &out) const
{
    get_bounding_volume_shape()->get_aabb_impl(transform, tolerance, out);
}

phys_bool phys_bounding_shape::trace_ray_impl(const phys_ray_input &input, phys_ray_output &output) const
{
    output.change_key_level(1);
    phys_bool hit = m_child_container->trace_ray(input, output);
    output.change_key_level(-1);
    if (hit) {
        output.set_cached_key(0);
    }
    return hit;
}

void phys_bounding_shape::trace_ray_with_collector(const phys_ray_input &input, const phys_collision_proxy &body, phys_ray_hit_collector &collector) const
{
    phys_collision_proxy child_proxy(&body);
    const phys_shape *bounding_child_shape = get_child_shape();
    child_proxy.set_proxy_shape(bounding_child_shape, 0);
    bounding_child_shape->trace_ray_with_collector(input, child_proxy, collector);
}

void fixup_loaded_bounding_shape(void *object)
{
    phys_fixup_tag flag;
    flag.m_fixup_finishing = 1;
    new (object) phys_bounding_shape(flag);
}

const void *get_bounding_shape_vtable()
{
    phys_fixup_tag flag;
    union { __vector4 alignment; char storage[sizeof(phys_bounding_shape)]; } buffer;
    new (buffer.storage) phys_bounding_shape(flag);
    return *(const void **)buffer.storage;
}

// @address 0x61f288 ?destroy_loaded_bounding_shape@@YAXPAX@Z
void destroy_loaded_bounding_shape(void *p)
{
    static_cast<phys_bounding_shape *>(p)->~phys_bounding_shape();
}

phys_bounding_shape::phys_bounding_shape(const phys_shape *bounding_volume_shape, const phys_shape *bounding_child_shape)
    : phys_shape(PHYS_SHAPE_BOUNDING)
    , m_bounding_shape(bounding_volume_shape)
    , m_child_container(bounding_child_shape)
{
    m_bounding_shape->add_object_reference();
}

