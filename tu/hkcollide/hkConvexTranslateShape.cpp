// hkcollide/hkConvexTranslateShape.obj: from the records 0x6214c0 0x6214d8 0x621568 0x621580 0x621610 0x621618 0x621680 0x621760 0x621808 0x621868 0x6218d0 0x621930 0x621990 0x621a18 0x621a50 0x621a60 0x621ad8
// @compiler xdk6130
// @flags /TP /O2 /Oc /GF /EHs-c- /GR-
// @address 0x6214c0 ?destroy_loaded_convex_offset_shape@@YAXPAX@Z
// @address 0x621a18 ?fixup_loaded_convex_offset_shape@@YAXPAX@Z
// @address 0x621a50 ?get_convex_offset_shape_vtable@@YAPBXXZ
// @address 0x6214d8 ?compute_dma_size@phys_convex_offset_shape@@UBAHH@Z
// @address 0x621568 ?get_collision_sphere_count_impl@phys_convex_offset_shape@@UBAHXZ
// @address 0x621580 ?gather_memory_stats@phys_convex_offset_shape@@UBAXPAVphys_statistics_collector@@@Z
// @address 0x621610 ?get_shape_container@phys_convex_offset_shape@@UBAPBVphys_shape_container@@XZ
// @address 0x621618 ?get_max_projection@phys_convex_offset_shape@@UBAMABVphys_vec4@@@Z
// @address 0x621808 ?get_support_vertex_impl@phys_convex_offset_shape@@UBAXABVphys_vec4@@AAVphys_hull_vertex@@@Z
// @address 0x621868 ?vertex_ids_to_vertices_impl@phys_convex_offset_shape@@UBAXPBGHPAVphys_hull_vertex@@@Z
// @address 0x6218d0 ?get_shape_centre_impl@phys_convex_offset_shape@@UBAXAAVphys_vec4@@@Z
// @address 0x621930 ?get_first_vertex@phys_convex_offset_shape@@UBAXAAVphys_vec4@@@Z
// @address 0x621990 ?get_collision_spheres_impl@phys_convex_offset_shape@@UBAPBVphys_sphere@@PAV2@@Z
// @address 0x621680 ?trace_ray_impl@phys_convex_offset_shape@@UBA?AVphys_bool@@ABUphys_ray_input@@AAUphys_ray_output@@@Z
// @address 0x621760 ?trace_ray_with_collector@phys_convex_offset_shape@@UBAXABUphys_ray_input@@ABVphys_collision_proxy@@AAVphys_ray_hit_collector@@@Z
// @address 0x621a60 ??0phys_convex_offset_shape@@QAA@PBVphys_convex_shape@@ABVphys_vec4@@@Z
// @address 0x621ad8 ?get_aabb_impl@phys_convex_offset_shape@@UBAXABVphys_transform@@MAAVphys_aabb@@@Z
#include "phys/phys_collide.h"
#include "phys/phys_statistics_collector.h"

void fixup_loaded_convex_offset_shape(void *p)
{
    phys_fixup_tag flag;
    flag.m_fixup_finishing = 1;
    new (p) phys_convex_offset_shape(flag);
}

void destroy_loaded_convex_offset_shape(void *p)
{
    static_cast<phys_convex_offset_shape *>(p)->~phys_convex_offset_shape();
}

const void *get_convex_offset_shape_vtable()
{
    union {
        __declspec(align(16)) void *ptr;
        char buf[sizeof(phys_convex_offset_shape)];
    } u;
    phys_fixup_tag flag;
    new (u.buf) phys_convex_offset_shape(flag);
    return u.ptr;
}

int phys_convex_offset_shape::compute_dma_size(int buffer_size_left) const
{
    int child_buf_bytes = buffer_size_left - sizeof(*this);
    int child_byte_size = m_single_child_shape.get_single_child()->compute_dma_size(child_buf_bytes);
    if (child_byte_size < 0 || child_byte_size > child_buf_bytes) {
        return -1;
    }
    if (static_cast<const void *>(m_single_child_shape.get_single_child()) == static_cast<const void *>(this + 1)) {
        m_child_shape_size = 0;
        return sizeof(*this) + child_byte_size;
    }
    m_child_shape_size = child_byte_size;
    return sizeof(*this);
}

int phys_convex_offset_shape::get_collision_sphere_count_impl() const
{
    return get_child_shape()->get_collision_sphere_count();
}

void phys_convex_offset_shape::gather_memory_stats(phys_statistics_collector *collector) const
{
    collector->begin_stats_object("CvxTranslate", phys_statistics_collector::stat_category_shared, this);
    collector->add_child_object("Child", phys_statistics_collector::stat_category_shared, m_single_child_shape.get_single_child());
    collector->end_object();
}

const phys_shape_container *phys_convex_offset_shape::get_shape_container() const
{
    return &m_single_child_shape;
}

float phys_convex_offset_shape::get_max_projection(const phys_vec4 &direction) const
{
    float child_max_proj = get_child_shape()->get_max_projection(direction);
    float offset = direction.dot3(m_translation);
    return child_max_proj + offset;
}

void phys_convex_offset_shape::get_support_vertex_impl(const phys_vec4 &direction, phys_hull_vertex &supporting_vertex_out) const
{
    get_child_shape()->get_support_vertex(direction, supporting_vertex_out);
    supporting_vertex_out.add4(m_translation);
}

void phys_convex_offset_shape::vertex_ids_to_vertices_impl(const phys_vertex_id *ids, int id_count, phys_hull_vertex *vertices_out) const
{
    get_child_shape()->vertex_ids_to_vertices(ids, id_count, vertices_out);
    for (int i = 0; i < id_count; i++) {
        vertices_out[i].add4(m_translation);
    }
}

void phys_convex_offset_shape::get_shape_centre_impl(phys_vec4 &centre_out) const
{
    get_child_shape()->get_shape_centre(centre_out);
    centre_out.add4(m_translation);
}

void phys_convex_offset_shape::get_first_vertex(phys_vec4 &v) const
{
    get_child_shape()->get_first_vertex(v);
    v.add4(m_translation);
}

const phys_sphere *phys_convex_offset_shape::get_collision_spheres_impl(phys_sphere *sphere_buffer) const
{
    const phys_sphere *child_sphere_list = get_child_shape()->get_collision_spheres(sphere_buffer);
    phys_sphere *out_spheres = sphere_buffer;
    int sphere_count = get_child_shape()->get_collision_sphere_count();
    for (int i = 0; i < sphere_count; i++) {
        out_spheres->get_position_and_radius().set_add4(child_sphere_list->get_position_and_radius(), m_translation);
        child_sphere_list++;
        out_spheres++;
    }
    return sphere_buffer;
}

phys_convex_offset_shape::phys_convex_offset_shape(const phys_convex_shape *bounding_child_shape, const phys_vec4 &translation)
    : phys_convex_shape(PHYS_SHAPE_CONVEX_OFFSET, bounding_child_shape->get_convex_radius()), m_single_child_shape(bounding_child_shape)
{
    m_translation = translation;
    m_translation(3) = 0.0f;
    m_child_shape_size = 0;
}

void phys_convex_offset_shape::get_aabb_impl(const phys_transform &local_to_world, float tolerance, phys_aabb &out) const
{
    get_child_shape()->get_handle_aabb(local_to_world, tolerance, out);
    phys_vec4 translation;
    translation.set_rotated_dir(local_to_world.get_rotation(), m_translation);
    out.m_min_limit.add4(translation);
    out.m_max_limit.add4(translation);
}

phys_bool phys_convex_offset_shape::trace_ray_impl(const phys_ray_input &input, phys_ray_output &results) const
{
    phys_ray_input child_ray_input = input;
    child_ray_input.m_ray_from.set_sub4(input.m_ray_from, m_translation);
    child_ray_input.m_ray_to.set_sub4(input.m_ray_to, m_translation);
    results.change_key_level(1);
    const phys_bool hit = get_child_shape()->trace_ray(child_ray_input, results);
    results.change_key_level(-1);
    if (hit) {
        results.set_cached_key(0);
    }
    return hit;
}

void phys_convex_offset_shape::trace_ray_with_collector(const phys_ray_input &input, const phys_collision_proxy &collision_proxy,
    phys_ray_hit_collector &collector) const
{
    phys_ray_input child_ray_input = input;
    child_ray_input.m_ray_from.set_sub4(input.m_ray_from, m_translation);
    child_ray_input.m_ray_to.set_sub4(input.m_ray_to, m_translation);
    phys_collision_proxy offset_child_proxy(&collision_proxy);
    const phys_shape *child_shape = get_child_shape();
    offset_child_proxy.set_proxy_shape(child_shape, 0);
    child_shape->trace_ray_with_collector(child_ray_input, offset_child_proxy, collector);
}
