// hkcollide/hkMoppBvTreeShape.obj: from the records 0x6205c8 0x6205e0 0x620618 0x6206c8 0x6206e0 0x620718 0x620810 0x6208a0 0x620908 0x6209c8 0x620a00 0x620a10
// @flags /TP /O2 /Oc
// @address 0x6205e0 ?shape_query_sphere@phys_bvh_tree_shape@@UBAXABVphys_sphere@@AAV?$phys_array@I@@@Z
// @address 0x6206c8 ?get_aabb_impl@phys_bvh_tree_shape@@UBAXABVphys_transform@@MAAVphys_aabb@@@Z
// @address 0x6206e0 ?query_obb@phys_bvh_tree_shape@@UBAXABVphys_transform@@ABVphys_vec4@@MAAV?$phys_array@I@@@Z
// @address 0x620810 ?trace_ray_impl@phys_bvh_tree_shape@@UBA?AVphys_bool@@ABUphys_ray_input@@AAUphys_ray_output@@@Z
// @address 0x6208a0 ?trace_ray_with_collector@phys_bvh_tree_shape@@UBAXABUphys_ray_input@@ABVphys_collision_proxy@@AAVphys_ray_hit_collector@@@Z
// @address 0x620908 ?gather_memory_stats@phys_bvh_tree_shape@@UBAXPAVphys_statistics_collector@@@Z
// @address 0x620a00 ?get_bvh_tree_shape_vtable@@YAPBXXZ
// @address 0x6209c8 ?fixup_loaded_bvh_tree_shape@@YAXPAX@Z
#include "phys/phys_base.h"
#include "phys/phys_aabb.h"
#include "physics/havok_halo_external.h"
#include "phys/phys_collide.h"
#include "phys/phys_statistics_collector.h"


#include "phys/phys_bvh_sphere_vm.h"
#include "phys/phys_bvh_obb_vm.h"
#include "phys/phys_bvh_long_ray_vm.h"
#include "phys/phys_object.h"
#include "phys/phys_bvh_vm.h"
#include "phys/phys_sphere.h"
extern "C" {
}


// @address 0x620a10 ??0phys_bvh_tree_shape@@QAA@PBVphys_compound_shape@@PBVphys_bvh_bytecode@@@Z
phys_bvh_tree_shape::phys_bvh_tree_shape(const phys_compound_shape *collection, const phys_bvh_bytecode *code)
  : phys_bvh_shape(PHYS_SHAPE_BVH_TREE), m_child_shape(collection), m_bvh_code(code)
{
    code->add_object_reference();
}

void phys_bvh_tree_shape::shape_query_sphere(const phys_sphere &sphere, phys_array<unsigned> &hits) const
{
    phys_bvh_sphere_vm vm;
    vm.shape_query_sphere(m_bvh_code, sphere, (phys_array<phys_bvh_primitive_info> *)&hits);
}

void phys_bvh_tree_shape::get_aabb_impl(const phys_transform &transform, float tolerance, phys_aabb &aabb) const
{
    m_child_shape->get_handle_aabb(transform, tolerance, aabb);
}

void phys_bvh_tree_shape::query_obb(const phys_transform &transform, const phys_vec4 &box_half_extents, float tolerance, phys_array<unsigned> &hits) const
{
    phys_bvh_obb_vm vm;
    vm.query_obb(m_bvh_code, transform, box_half_extents, tolerance, (phys_array<phys_bvh_primitive_info> *)&hits);
}

phys_bool phys_bvh_tree_shape::trace_ray_impl(const phys_ray_input &input, phys_ray_output &output) const
{
    phys_bvh_long_ray_vm vm;
    output.change_key_level(1);
    const phys_compound_shape *collection = get_compound_shape();
    phys_bool hit = vm.query_long_ray(collection ? static_cast<const phys_shape_container *>(collection) : 0, m_bvh_code, input, output);
    output.change_key_level(-1);
    if (hit) output.set_cached_key(0);
    return hit;
}

void phys_bvh_tree_shape::trace_ray_with_collector(const phys_ray_input &input, const phys_collision_proxy &body, phys_ray_hit_collector &collector) const
{
    phys_bvh_long_ray_vm vm;
    phys_collision_proxy child_proxy(&body);
    child_proxy.set_proxy_shape((const phys_shape *)get_compound_shape(), 0);
    const phys_compound_shape *collection = get_compound_shape();
    vm.query_long_ray(collection ? static_cast<const phys_shape_container *>(collection) : 0, m_bvh_code, input, child_proxy, collector);
}

void phys_bvh_tree_shape::gather_memory_stats(phys_statistics_collector *collector) const
{
    collector->begin_stats_object("MoppShape", phys_statistics_collector::stat_category_shared, this);
    collector->add_child_object("Mesh", phys_statistics_collector::stat_category_shared, m_child_shape.get_single_child());
    collector->add_chunk_stats("Mopp", phys_statistics_collector::stat_category_shared, m_bvh_code, m_bvh_code->get_code_size(), 0);
    collector->end_object();
}

void fixup_loaded_bvh_tree_shape(void *object)
{
    phys_fixup_tag flag;
    flag.m_fixup_finishing = 1;
    new (object) phys_bvh_tree_shape(flag);
}

const void *get_bvh_tree_shape_vtable()
{
    phys_fixup_tag flag;
    union { __vector4 alignment; char storage[sizeof(phys_bvh_tree_shape)]; } buffer;
    new (buffer.storage) phys_bvh_tree_shape(flag);
    return *(const void **)buffer.storage;
}

// @address 0x6205c8 ?destroy_loaded_bvh_tree_shape@@YAXPAX@Z
void destroy_loaded_bvh_tree_shape(void *object)
{
    static_cast<phys_bvh_tree_shape *>(object)->~phys_bvh_tree_shape();
}


// @address 0x620618 ??1phys_bvh_tree_shape@@UAA@XZ
phys_bvh_tree_shape::~phys_bvh_tree_shape()
{
    m_bvh_code->object_remove_ref();
}

// @address 0x620718 ?query_aabb@phys_bvh_tree_shape@@UBAXABVphys_aabb@@AAV?$phys_array@I@@@Z
void phys_bvh_tree_shape::query_aabb(class phys_aabb const &aabb, class phys_array<unsigned int> &hits) const
{
    phys_vec4 offset = m_bvh_code->m_bvh_info.m_offset;
    const phys_bvh_bytecode::bytecode_info &bvh_code_info = m_bvh_code->m_bvh_info;
    float width = 16777216.0f / bvh_code_info.get_offset_scale();
    phys_vec4 w;
    w.set(width, width, width, 0.0f);

    phys_aabb scratch_aabb;
    scratch_aabb.m_min_limit = offset;
    scratch_aabb.m_max_limit = offset;
    scratch_aabb.m_max_limit.add4(w);
    phys_aabb new_aabb = scratch_aabb;
    new_aabb.m_min_limit.set_max4(new_aabb.m_min_limit, aabb.m_min_limit);
    new_aabb.m_max_limit.set_min4(new_aabb.m_max_limit, aabb.m_max_limit);

    phys_bvh_obb_vm obb_vm;
    obb_vm.query_aabb(m_bvh_code, new_aabb, (phys_array<phys_bvh_primitive_info> *)&hits);
}

