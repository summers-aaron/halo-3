// hkcollide/hkPhantomCallbackShape.obj: from the records 0x621b60 0x621b68 0x621bc8 0x621be8
// @flags /TP /O2 /Oc
#include "phys/phys_base.h"
#include "phys/phys_base_types.h"
#include "phys/phys_collide.h"
#include <vectorintrinsics.h>
#include "phys/phys_statistics_collector.h"

#include "phys/phys_volume_callback_shape.h"

// @address 0x621b60 ?trace_ray_with_collector@phys_volume_callback_shape@@UBAXABUphys_ray_input@@ABVphys_collision_proxy@@AAVphys_ray_hit_collector@@@Z
void phys_volume_callback_shape::trace_ray_with_collector(const phys_ray_input &input, const phys_collision_proxy &body,
    phys_ray_hit_collector &collector) const
{
}

// @address 0x621b68 ?gather_memory_stats@phys_volume_callback_shape@@UBAXPAVphys_statistics_collector@@@Z
void phys_volume_callback_shape::gather_memory_stats(phys_statistics_collector *collector) const
{
    collector->begin_stats_object("PhantomCb", phys_statistics_collector::stat_category_shared, this);
    collector->end_object();
}

// @address 0x621bc8 ?get_aabb_impl@phys_volume_callback_shape@@UBAXABVphys_transform@@MAAVphys_aabb@@@Z
void phys_volume_callback_shape::get_aabb_impl(const phys_transform &transform, float tolerance, phys_aabb &aabb) const
{
    aabb.m_min_limit.set_zero4();
    aabb.m_max_limit.set_zero4();
}

// @address 0x621be8 ?trace_ray_impl@phys_volume_callback_shape@@UBA?AVphys_bool@@ABUphys_ray_input@@AAUphys_ray_output@@@Z
phys_bool phys_volume_callback_shape::trace_ray_impl(const phys_ray_input &input, phys_ray_output &output) const
{
    return false;
}
