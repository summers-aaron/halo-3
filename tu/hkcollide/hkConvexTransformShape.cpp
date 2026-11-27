// hkcollide/hkConvexTransformShape.obj: from the records 0x1bff30 0x259cb0 0x620b40 0x620b58 0x620b70 0x620c00 0x620c08 0x620c40 0x620c78 0x620c88 0x620d10 0x620d80 0x620e58 0x620fa8 0x6210c0 0x621180 0x621280 0x621310 0x6213a8
// @compiler xdk6130
// @flags /TP /O2 /Oc
// @address 0x620b70 ?gather_memory_stats@phys_convex_xform_shape@@UBAXPAVphys_statistics_collector@@@Z
// @address 0x620c00 ?get_shape_container@phys_convex_xform_shape@@UBAPBVphys_shape_container@@XZ
// @address 0x620c78 ?get_convex_xform_shape_vtable@@YAPBXXZ
// @address 0x259cb0 ??4phys_rotation@@QAAAAV0@ABV0@@Z
#include "phys/phys_base.h"
// @address 0x1bff30 ?compute_dma_size@phys_convex_xform_shape@@UBAHH@Z
// @address 0x620b58 ?get_collision_sphere_count_impl@phys_convex_xform_shape@@UBAHXZ
// @address 0x620c08 ?set_transform@phys_convex_xform_shape@@QAAXABVphys_transform@@@Z
// @address 0x620c40 ?fixup_loaded_convex_xform_shape@@YAXPAX@Z
// @address 0x620d10 ?get_aabb_impl@phys_convex_xform_shape@@UBAXABVphys_transform@@MAAVphys_aabb@@@Z
#include <vectorintrinsics.h>
#include "phys/phys_collide.h"
#include "phys/phys_statistics_collector.h"
#include "phys/phys_collision_proxy.h"
#include "phys/phys_sphere.h"

// @address 0x61ad28 ?get_shape_centre_impl@phys_convex_shape@@MBAXAAVphys_vec4@@@Z
// @address 0x1c1258 ?compute_dma_size@phys_shape@@UBAHH@Z
// @address 0x42d380 ?get_child_shape_count@phys_single_shape_container@@UBAHXZ
// @address 0x1c1258 ?get_next_key@phys_single_shape_container@@UBAII@Z
// @address 0x620a98 ?get_collision_filter_info@phys_shape_container@@UBAII@Z
// @address 0x86ce0 ?get_child_shape@phys_single_shape_container@@UBAPBVphys_shape@@IAAY0CAA@D@Z
// @address 0x61acb0 ?weld_contact_point_impl@phys_convex_shape@@MBAHPAGAAEAAVphys_vec4@@PBVphys_transform@@PBV1@32@Z
// @address 0x620e58 ?trace_ray_impl@phys_convex_xform_shape@@UBA?AVphys_bool@@ABUphys_ray_input@@AAUphys_ray_output@@@Z
// @address 0x621310 ?get_first_vertex@phys_convex_xform_shape@@UBAXAAVphys_vec4@@@Z
// @address 0x6210c0 ?get_support_vertex_impl@phys_convex_xform_shape@@UBAXABVphys_vec4@@AAVphys_hull_vertex@@@Z
// @address 0x621180 ?vertex_ids_to_vertices_impl@phys_convex_xform_shape@@UBAXPBGHPAVphys_hull_vertex@@@Z
// @address 0x621280 ?get_shape_centre_impl@phys_convex_xform_shape@@UBAXAAVphys_vec4@@@Z

int phys_convex_xform_shape::compute_dma_size(int solver_buffer_size) const
{
    int child_buf_size = solver_buffer_size - sizeof(phys_convex_xform_shape);
    const phys_shape *child = m_single_child_shape.get_single_child();
    int child_dma_size = child->compute_dma_size(child_buf_size);
    if (child_dma_size >= 0 && child_dma_size <= child_buf_size) {
        if (m_single_child_shape.get_single_child() == reinterpret_cast<const phys_shape *>(this + 1)) {
            m_child_shape_size = 0;
            return child_dma_size + sizeof(phys_convex_xform_shape);
        } else {
            m_child_shape_size = child_dma_size;
            return sizeof(phys_convex_xform_shape);
        }
    } else {
        return -1;
    }
}

int phys_convex_xform_shape::get_collision_sphere_count_impl() const
{
    return get_child_shape()->get_collision_sphere_count();
}

void phys_convex_xform_shape::gather_memory_stats(phys_statistics_collector *collector) const
{
    collector->begin_stats_object("CvxTransform", phys_statistics_collector::stat_category_shared, this);
    collector->add_child_object("Child", phys_statistics_collector::stat_category_shared, get_child_shape());
    collector->end_object();
}

const phys_shape_container *phys_convex_xform_shape::get_shape_container() const
{
    return &m_single_child_shape;
}

void phys_convex_xform_shape::set_transform(const phys_transform &transform)
{
    m_body_transform = transform;
}

void fixup_loaded_convex_xform_shape(void *object)
{
    phys_fixup_tag flag;
    flag.m_fixup_finishing = 1;
    new (object) phys_convex_xform_shape(flag);
}

const void *get_convex_xform_shape_vtable()
{
    phys_fixup_tag flag;
    union { __vector4 alignment; char storage[sizeof(phys_convex_xform_shape)]; } buffer;
    new (buffer.storage) phys_convex_xform_shape(flag);
    return *(const void **)buffer.storage;
}

// @address 0x620c88 ??0phys_convex_xform_shape@@QAA@PBVphys_convex_shape@@ABVphys_transform@@@Z
phys_convex_xform_shape::phys_convex_xform_shape(const phys_convex_shape *bounding_child_shape, const phys_transform &transform)
    : phys_convex_shape(PHYS_SHAPE_CONVEX_XFORM, bounding_child_shape->get_convex_radius()), m_single_child_shape(bounding_child_shape)
{
    m_body_transform = transform;
}

void phys_convex_xform_shape::get_aabb_impl(const phys_transform &local_to_world, float tolerance, phys_aabb &out) const
{
    phys_transform combined;
    combined.set_mul(local_to_world, m_body_transform);
    m_single_child_shape.get_single_child()->get_handle_aabb(combined, tolerance, out);
}

// @address 0x620b40 ?destroy_loaded_convex_xform_shape@@YAXPAX@Z
void destroy_loaded_convex_xform_shape(void *object)
{
    static_cast<phys_convex_xform_shape *>(object)->~phys_convex_xform_shape();
}
void phys_convex_xform_shape::get_first_vertex(phys_vec4 &v) const
{
    phys_vec4 local_vertex;
    get_child_shape()->get_first_vertex(local_vertex);
    v.set_transformed_pos(m_body_transform, local_vertex);
}
void phys_convex_xform_shape::get_support_vertex_impl(const phys_vec4 &direction, phys_hull_vertex &supporting_vertex_out) const
{
    phys_vec4 local_dir;
    local_dir.set_rotated_inverse_dir(m_body_transform.m_rotation, direction);
    phys_hull_vertex local_vertex;
    get_child_shape()->get_support_vertex_impl(local_dir, local_vertex);
    supporting_vertex_out.set_transformed_pos(m_body_transform, local_vertex);
    supporting_vertex_out.set_w(local_vertex);
}
void phys_convex_xform_shape::get_shape_centre_impl(phys_vec4 &centre_out) const
{
    get_child_shape()->get_shape_centre(centre_out);
    centre_out.set_transformed_pos(m_body_transform, centre_out);
}
namespace phys_vec4_util {
__forceinline void transform_spheres(const phys_transform &t, const phys_vec4 *vecs_in, int rotate_vec_count, phys_vec4 *vecs_out)
{
    phys_transform unaliased_copy = t;
    int i = rotate_vec_count - 1;
    do {
        phys_vec4 v;
        v.set_transformed_pos(unaliased_copy, vecs_in[i]);
        v.set_w(vecs_in[i]);
        vecs_out[i] = v;
    } while (--i >= 0);
}
}
void phys_convex_xform_shape::vertex_ids_to_vertices_impl(const unsigned short *ids, int id_count, phys_hull_vertex *vertices_out) const
{
    get_child_shape()->vertex_ids_to_vertices_impl(ids, id_count, vertices_out);
    phys_vec4_util::transform_spheres(m_body_transform, vertices_out, id_count, vertices_out);
}

// @address 0x6213a8 ?get_collision_spheres_impl@phys_convex_xform_shape@@UBAPBVphys_sphere@@PAV2@@Z
const phys_sphere *phys_convex_xform_shape::get_collision_spheres_impl(phys_sphere *p0) const
{
    const phys_sphere *spheres = get_child_shape()->get_collision_spheres_impl(p0);
    int child_sphere_count = get_child_shape()->get_collision_sphere_count();
    phys_vec4_util::transform_spheres(m_body_transform, (const phys_vec4 *)spheres, child_sphere_count, (phys_vec4 *)p0);
    return p0;
}

// @address 0x620d80 ?get_max_projection@phys_convex_xform_shape@@UBAMABVphys_vec4@@@Z
float phys_convex_xform_shape::get_max_projection(class phys_vec4 const &p0) const
{
    phys_vec4 local_dir;
    local_dir.set_rotated_inverse_dir(m_body_transform.m_rotation, p0);
    float child_max_proj = get_child_shape()->get_max_projection(local_dir);
    __vector4 scaled = __vmulfp(local_dir.m_simd_quad, __vspltw(__lvlx(&child_max_proj, 0), 0));
    phys_vec4 tmp;
    tmp.m_simd_quad = scaled;
    phys_vec4 world;
    world.set_transformed_pos(m_body_transform, tmp);
    float result;
    __stvewx(__vmsum3fp(world.m_simd_quad, p0.m_simd_quad), &result, 0);
    return result;
}

phys_bool phys_convex_xform_shape::trace_ray_impl(const phys_ray_input &input, phys_ray_output &results) const
{
    phys_ray_input sub_input = input;
    sub_input.m_ray_from.set_transformed_inverse_pos(m_body_transform, input.m_ray_from);
    sub_input.m_ray_to.set_transformed_inverse_pos(m_body_transform, input.m_ray_to);
    results.change_key_level(1);
    phys_bool hit = get_child_shape()->trace_ray(sub_input, results);
    results.change_key_level(-1);
    if (hit) {
        phys_vec4 n = results.m_normal;
        results.m_normal.vec_set_rotated_dir(m_body_transform.get_rotation(), n);
        results.set_cached_key(0);
    }
    return hit;
}
// @address 0x620fa8 ?trace_ray_with_collector@phys_convex_xform_shape@@UBAXABUphys_ray_input@@ABVphys_collision_proxy@@AAVphys_ray_hit_collector@@@Z
void phys_convex_xform_shape::trace_ray_with_collector(struct phys_ray_input const &p0, class phys_collision_proxy const &p1,
    class phys_ray_hit_collector &p2) const
{
    phys_ray_input child_space_ray_input(p0);
    child_space_ray_input.m_ray_from.set_transformed_inverse_pos(m_body_transform, p0.m_ray_from);
    child_space_ray_input.m_ray_to.set_transformed_inverse_pos(m_body_transform, p0.m_ray_to);

    phys_transform child_xform;
    child_xform.set_mul(p1.get_transform(), m_body_transform);

    phys_collision_proxy child_proxy(&p1, &child_xform);
    child_proxy.set_proxy_shape(m_single_child_shape.get_single_child(), 0);

    m_single_child_shape.get_single_child()->trace_ray_with_collector(child_space_ray_input, child_proxy, p2);
}

