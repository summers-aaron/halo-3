// hkcollide/hkShapeCollection.obj: from the records 0x624c90 0x624ca8 0x624d08 0x624ed0 0x624ff8 0x6250a8 0x625228
// @flags /TP /O2 /Oc
#include "phys/phys_base_types.h"
// @address 0x624c90 ?get_shape_container@phys_compound_shape@@UBAPBVphys_shape_container@@XZ
// @address 0x624ca8 ?gather_memory_stats@phys_compound_shape@@UBAXPAVphys_statistics_collector@@@Z
// @address 0x624ed0 ?get_aabb_impl@phys_compound_shape@@UBAXABVphys_transform@@MAAVphys_aabb@@@Z
// @address 0x624ff8 ?get_max_projection@phys_compound_shape@@UBAMABVphys_vec4@@@Z
// @address 0x625228 ??0phys_compound_shape@@QAA@W4phys_shape_type@@@Z
#include <vectorintrinsics.h>
#include "phys/phys_aabb.h"
#include "phys/phys_collide.h"
#include "phys/phys_statistics_collector.h"
#include "phys/phys_collision_proxy.h"
namespace {
__forceinline float max_projection(float a, float b) { return (a - b >= 0.0f) ? a : b; }
}
const phys_shape_container *phys_compound_shape::get_shape_container() const { return this; }
void phys_compound_shape::gather_memory_stats(phys_statistics_collector *collector) const {
    collector->begin_stats_object("Collection", phys_statistics_collector::stat_category_shared, this);
    collector->end_object();
}
void phys_compound_shape::get_aabb_impl(const phys_transform &transform, float tolerance, phys_aabb &out) const {
    out.m_min_limit.set_zero4();
    out.m_max_limit.set_zero4();
    int key = get_first_key();
    __declspec(align(16)) char buffer[512];
    if (key != -1) {
        const phys_shape *shape = get_child_shape(key, buffer);
        shape->get_handle_aabb(transform, tolerance, out);
    }
    for (; key != -1; key = get_next_key(key)) {
        const phys_shape *shape = get_child_shape(key, buffer);
        phys_aabb child;
        shape->get_handle_aabb(transform, tolerance, child);
        out.m_min_limit.set_min4(out.m_min_limit, child.m_min_limit);
        out.m_max_limit.set_max4(out.m_max_limit, child.m_max_limit);
    }
}
float phys_compound_shape::get_max_projection(const phys_vec4 &direction) const {
    float result = -3.40282e38f;
    __declspec(align(16)) char buffer[512];
    for (int key = get_first_key(); key != -1; key = get_next_key(key)) {
        const phys_shape *shape = get_child_shape(key, buffer);
        float projection = shape->get_max_projection(direction);
        result = max_projection(result, projection);
    }
    return result;
}
phys_compound_shape::phys_compound_shape(phys_shape_type compound_type) : phys_shape(compound_type) { m_edge_weld_disabled = false; }

// @address 0x6250a8 ?trace_ray_with_collector@phys_compound_shape@@UBAXABUphys_ray_input@@ABVphys_collision_proxy@@AAVphys_ray_hit_collector@@@Z
void phys_compound_shape::trace_ray_with_collector(const phys_ray_input &input, const phys_collision_proxy &collision_proxy,
    phys_ray_hit_collector &collector) const
{
    __declspec(align(16)) char buffer[512];
    if (input.m_ray_compound_filter == 0) {
        for (int key = get_first_key(); key != -1; key = get_next_key(key)) {
            const phys_shape *bounding_child_shape = get_child_shape(key, buffer);
            phys_collision_proxy child_proxy(&collision_proxy);
            child_proxy.set_proxy_shape(bounding_child_shape, key);
            bounding_child_shape->trace_ray_with_collector(input, child_proxy, collector);
        }
    } else {
        for (int key = get_first_key(); key != -1; key = get_next_key(key)) {
            if (input.m_ray_compound_filter->allows_collision(input, *this, *this, key)) {
                const phys_shape *bounding_child_shape = get_child_shape(key, buffer);
                phys_collision_proxy child_proxy(&collision_proxy);
                child_proxy.set_proxy_shape(bounding_child_shape, key);
                bounding_child_shape->trace_ray_with_collector(input, child_proxy, collector);
            }
        }
    }
}
// @address 0x624d08 ?trace_ray_impl@phys_compound_shape@@UBA?AVphys_bool@@ABUphys_ray_input@@AAUphys_ray_output@@@Z
phys_bool phys_compound_shape::trace_ray_impl(const phys_ray_input &input, phys_ray_output &results) const
{
    __declspec(align(16)) char shape_scratch_buffer[512];
    results.change_key_level(1);
    unsigned int list_best_key = ~0u;
    if (!input.m_ray_compound_filter) {
        for (unsigned int key = get_first_key(); key != ~0u; key = get_next_key(key)) {
            const phys_shape *bounding_child_shape = get_child_shape(key, shape_scratch_buffer);
            phys_bool hit = bounding_child_shape->trace_ray_impl(input, results);
            if (hit) {
                list_best_key = key;
            }
        }
    } else {
        for (unsigned int key = get_first_key(); key != ~0u; key = get_next_key(key)) {
            if (input.m_ray_compound_filter->allows_collision(input, *this, *this, key)) {
                const phys_shape *bounding_child_shape = get_child_shape(key, shape_scratch_buffer);
                phys_bool hit = bounding_child_shape->trace_ray_impl(input, results);
                if (hit) {
                    list_best_key = key;
                }
            }
        }
    }
    results.change_key_level(-1);
    if (list_best_key != ~0u) {
        results.set_cached_key(list_best_key);
    }
    return list_best_key != ~0u;
}
