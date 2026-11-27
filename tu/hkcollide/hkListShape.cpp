// hkcollide/hkListShape.obj: from the records 0x61fc60 0x61fc78 0x61fc80 0x61fe00 0x61ff40 0x61ff48 0x61ff68 0x61ff78 0x61ff90 0x61ffa8 0x620048 0x620138 0x620238 0x620320 0x6203c8 0x620400 0x620508 0x620548 0x620558 0x620560
// @flags /TP /O2 /Oc
#include "phys/phys_base_types.h"
// @address 0x61ff78 ?get_collision_filter_info@phys_list_shape@@UBAII@Z
// @address 0x61ff90 ?set_collision_filter_info@phys_list_shape@@QAAXII@Z
// @address 0x6203c8 ??0phys_list_shape@@QAA@Vphys_fixup_tag@@@Z
// @address 0x620508 ?fixup_loaded_list_shape@@YAXPAX@Z
#include <ppcintrinsics.h>
#include <vectorintrinsics.h>
#include <math.h>
#include <stdlib.h>
#include "physics/havok_halo_external.h"
#include "phys/phys_math.h"
#include "phys/phys_collide.h"
#include "phys/phys_statistics_collector.h"

// @address 0x1264 ??_7phys_list_shape@@6Bphys_shape@@@
// @address 0x12cc ??_7phys_list_shape@@6Bphys_shape_container@@@
// @address 0x150c8 ??_7phys_shape_container@@6B@
#include "phys/phys_thread_memory.h"
// @address 0x620aa0 ?get_child_shape_count@phys_shape_container@@UBAHXZ
// @address 0x620a98 ?get_collision_filter_info@phys_shape_container@@UBAII@Z

// @address 0x61fc78 ?get_first_key@phys_list_shape@@UBAIXZ
unsigned int phys_list_shape::get_first_key() const
{
    return 0;
}

// @address 0x61ff40 ?get_child_shape_count@phys_list_shape@@UBAHXZ
int phys_list_shape::get_child_shape_count() const
{
    return m_child_info.getSize();
}

// @address 0x61ff48 ?get_next_key@phys_list_shape@@UBAII@Z
unsigned int phys_list_shape::get_next_key(unsigned int key) const
{
    if (int(key + 1) < m_child_info.getSize())
        return key + 1;
    else
        return 0xffffffff;
}

// @address 0x61ff68 ?get_child_shape@phys_list_shape@@UBAPBVphys_shape@@IAAY0CAA@D@Z
const phys_shape *phys_list_shape::get_child_shape(unsigned int key, char (&buffer)[512]) const
{
    return m_child_info[key].m_proxy_shape;
}

// @address 0x61fc80 ?trace_ray_impl@phys_list_shape@@UBA?AVphys_bool@@ABUphys_ray_input@@AAUphys_ray_output@@@Z
phys_bool phys_list_shape::trace_ray_impl(const phys_ray_input &input, phys_ray_output &output) const
{
    __declspec(align(16)) char shape_scratch_buffer[512];
    output.change_key_level(1);
    unsigned int list_best_key = 0xffffffffu;
    if (input.m_ray_compound_filter == 0) {
        for (int i = 0; i < m_child_info.getSize(); i++) {
            const phys_shape *bounding_child_shape = get_child_shape(i, shape_scratch_buffer);
            if (bounding_child_shape->trace_ray(input, output))
                list_best_key = i;
        }
    } else {
        for (int i = 0; i < m_child_info.getSize(); i++) {
            if (input.m_ray_compound_filter->allows_collision(input, *this, *this, i)) {
                const phys_shape *bounding_child_shape = m_child_info[i].m_proxy_shape;
                if (bounding_child_shape->trace_ray(input, output))
                    list_best_key = i;
            }
        }
    }
    output.change_key_level(-1);
    if (list_best_key != 0xffffffffu)
        output.set_cached_key(list_best_key);
    return list_best_key != 0xffffffffu;
}

// 0x61ff78
unsigned int phys_list_shape::get_collision_filter_info(unsigned int key) const
{
    return m_child_info[key].m_collision_filter_info;
}
// 0x61ff90
void phys_list_shape::set_collision_filter_info(unsigned int key, unsigned int info)
{
    m_child_info.m_data[key].m_collision_filter_info = info;
}

// @address 0x61ffa8 ?compute_dma_size@phys_list_shape@@UBAHH@Z
int phys_list_shape::compute_dma_size(int solver_buffer_size) const
{
    if (m_child_info.getSize() >= 511) {
        return -1;
    } else {
        for (int i = 0; i < m_child_info.getSize(); ++i) {
            const phys_shape *shape = m_child_info[i].m_proxy_shape;
            m_child_info[i].m_shape_size = shape->compute_dma_size(256);
            m_child_info[i].m_child_shape_count = m_child_info.getSize();
            if (m_child_info[i].m_shape_size == -1)
                return -1;
        }
        return 80;
    }
}

// @address 0x620138 ?get_aabb_impl@phys_list_shape@@UBAXABVphys_transform@@MAAVphys_aabb@@@Z
void phys_list_shape::get_aabb_impl(const phys_transform &tr, float hull_tol, phys_aabb &out) const
{
    __vector4 zero = phys_simd_vec_0000;
    unsigned int cr;
    __vcmpequwR(__vcmpgefp(zero, m_aabb_half_extents.m_simd_quad), __vspltisw(-1), &cr);
    if ((int)(cr & 128) != 0) const_cast<phys_list_shape *>(this)->recompute_bounds_extents();
    m_child_info.m_data[0].m_proxy_shape->get_aabb_impl(tr, hull_tol, out);
    phys_aabb tmp;
    for (int i = 1; i < m_child_info.m_size; ++i) {
        child_shape_entry *ci = m_child_info.m_data;
        ci[i].m_proxy_shape->get_aabb_impl(tr, hull_tol, tmp);
        out.m_min_limit.m_simd_quad = __vminfp(out.m_min_limit.m_simd_quad, tmp.m_min_limit.m_simd_quad);
        out.m_max_limit.m_simd_quad = __vmaxfp(out.m_max_limit.m_simd_quad, tmp.m_max_limit.m_simd_quad);
    }
}
// @address 0x620238 ?gather_memory_stats@phys_list_shape@@UBAXPAVphys_statistics_collector@@@Z
void phys_list_shape::gather_memory_stats(phys_statistics_collector *collector) const
{
    collector->begin_stats_object("ListShape", phys_statistics_collector::stat_category_shared, this);
    collector->add_array_stats("ChildPtrs", phys_statistics_collector::stat_category_shared, m_child_info);
    for (int i = 0; i < m_child_info.getSize(); i++)
        collector->add_child_object("Child", phys_statistics_collector::stat_category_shared, m_child_info[i].m_proxy_shape);
    collector->end_object();
}


#if defined(_XBOX) && !defined(__clang__)
inline phys_list_shape::phys_list_shape(phys_fixup_tag flag)
#else
phys_list_shape::phys_list_shape(phys_fixup_tag flag)
#endif
    : phys_compound_shape(flag), m_child_info(flag)
{
    m_type = PHYS_SHAPE_LIST;
}

// @address 0x620400 ??1phys_list_shape@@UAA@XZ
phys_list_shape::~phys_list_shape()
{
    for (int i = 0; i < m_child_info.getSize(); ++i)
        m_child_info[i].m_proxy_shape->object_remove_ref();
}
void fixup_loaded_list_shape(void *p)
{
    new (p) phys_list_shape(phys_fixup_tag());
}

// @address 0x620548 ?get_list_shape_vtable@@YAPBXXZ
const void *get_list_shape_vtable()
{
    char storage[sizeof(phys_list_shape)];
    phys_fixup_tag flag;
    flag.m_fixup_finishing = 0;
    new (storage) phys_list_shape(flag);
    return *(const void **)storage;
}

// @address 0x620048 ?set_child_shapes@phys_list_shape@@IAAXPBQBVphys_shape@@HPBI@Z
void phys_list_shape::set_child_shapes(const phys_shape *const *child_shape_array, int list_shape_count, const unsigned int *list_filter_info)
{
    m_child_info.set_array_size(list_shape_count);
    for (int i = 0; i < list_shape_count; i++) {
        if (child_shape_array[i] != 0) {
            m_child_info[i].m_proxy_shape = child_shape_array[i];
            m_child_info[i].m_collision_filter_info = list_filter_info ? list_filter_info[i] : 0;
            child_shape_array[i]->add_object_reference();
            m_child_info[i].m_child_shape_count = list_shape_count;
        }
    }
    recompute_bounds_extents();
}

// @address 0x620558 ??_Ephys_list_shape@@WBA@AAPAXI@Z
// @address 0x620560 ??_Gphys_list_shape@@UAAPAXI@Z

// @address 0x61fc60 ?destroy_loaded_list_shape@@YAXPAX@Z
void destroy_loaded_list_shape(void *object)
{
    static_cast<phys_list_shape *>(object)->~phys_list_shape();
}


// @address 0x620320 ??0phys_list_shape@@QAA@PBQBVphys_shape@@H@Z
phys_list_shape::phys_list_shape(const phys_shape *const *child_shape_array, int list_shape_count)
    : phys_compound_shape(PHYS_SHAPE_LIST)
{
    m_child_info.reserve(4);
    set_child_shapes(child_shape_array, list_shape_count, 0);
    recompute_bounds_extents();
}

// @address 0x61fe00 ?recompute_bounds_extents@phys_list_shape@@QAAXXZ
void phys_list_shape::recompute_bounds_extents()
{
    phys_aabb aabb;
    m_child_info[0].m_proxy_shape->get_aabb_impl(phys_transform::get_identity(), 0.0f, aabb);
    phys_aabb t;
    for (int i = 1; i < m_child_info.getSize(); i++) {
        m_child_info[i].m_proxy_shape->get_aabb_impl(phys_transform::get_identity(), 0.0f, t);
        aabb.m_min_limit.set_min4(aabb.m_min_limit, t.m_min_limit);
        aabb.m_max_limit.set_max4(aabb.m_max_limit, t.m_max_limit);
    }
    m_aabb_half_extents.set_sub4(aabb.m_max_limit, aabb.m_min_limit);
    m_aabb_half_extents.mul4(0.5f);
    m_aabb_center.set_add4(aabb.m_min_limit, aabb.m_max_limit);
    m_aabb_center.mul4(0.5f);
}
