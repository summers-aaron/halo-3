// hkcollide/hkTriangleShape.obj: from the records 0x6230b8 0x6230d0 0x623130 0x623150 0x623168 0x623178 0x6231f0 0x623238 0x6232a8 0x623388 0x623440 0x623670 0x623698 0x6236a8 0x623778
// @flags /TP /O2 /Oc
#include "phys/phys_base_types.h"
// @address 0x6230d0 ?gather_memory_stats@phys_triangle_shape@@UBAXPAVphys_statistics_collector@@@Z
// @address 0x6231f0 ?get_shape_centre_impl@phys_triangle_shape@@UBAXAAVphys_vec4@@@Z
// @address 0x6236a8 ?get_support_vertex_impl@phys_triangle_shape@@UBAXABVphys_vec4@@AAVphys_hull_vertex@@@Z
// @address 0x623130 ?get_single_edge_bitcode@@YAHGH@Z
// @address 0x623150 ?get_collision_sphere_count_impl@phys_triangle_shape@@UBAHXZ
// @address 0x623670 ?fixup_loaded_triangle_shape@@YAXPAX@Z
// @address 0x6230b8 ?destroy_loaded_triangle_shape@@YAXPAX@Z
// @address 0x623698 ?get_triangle_shape_vtable@@YAPBXXZ
#include <vectorintrinsics.h>
#include <ppcintrinsics.h>
#include <math.h>
#include "phys/phys_math.h"
#include "phys/phys_collide.h"
#include "phys/phys_statistics_collector.h"
#include "phys/phys_triangle_shape.h"
#include "phys/phys_edge_weld_util.h"

// @address 0x724b40 k_phys_triangle_edge_weld_table
s_triangle_vertex_id_table k_phys_triangle_edge_weld_table = {
    { 0, 1, 2, 0, 1, 2 },
    { 0, 0, 0, 1.f, 1.f, 1.f },
    { 1.f, 0, 0, 0, -1.f }
};

namespace phys_math {
    }




void phys_triangle_shape::gather_memory_stats(phys_statistics_collector *collector) const
{
    collector->begin_stats_object("TriangleShp", phys_statistics_collector::stat_category_shared, this);
    collector->end_object();
}

int get_single_edge_bitcode(unsigned short edge_weld_info, int edge)
{
    return (edge_weld_info >> (edge * 5)) & 31;
}

int phys_triangle_shape::get_collision_sphere_count_impl() const
{
    return 3 + 3 * m_extruded;
}

// @address 0x623168 ?get_first_vertex@phys_triangle_shape@@UBAXAAVphys_vec4@@@Z
void phys_triangle_shape::get_first_vertex(phys_vec4 &vertex) const
{
    vertex.m_simd_quad = get_vertex(0).m_simd_quad;
}

// @address 0x623178 ?vertex_ids_to_vertices_impl@phys_triangle_shape@@UBAXPBGHPAVphys_hull_vertex@@@Z
void phys_triangle_shape::vertex_ids_to_vertices_impl(const unsigned short *ids, int count, phys_hull_vertex *vertices) const
{
    const phys_triangle_shape *shape = this;
    for (int i = count - 1; i >= 0; i--) {
        static_cast<phys_vec4 &>(*vertices) = shape->get_vertices()[k_phys_triangle_edge_weld_table.vertex[*ids]];
        vertices->add_scaled4(phys_simd_scalar(k_phys_triangle_edge_weld_table.sign[*ids]), shape->m_extrusion_vec);
        vertices->set_int24_w(*ids);
        ids++;
        vertices++;
    }
}

void phys_triangle_shape::get_shape_centre_impl(phys_vec4 &shape_centre) const
{
    const phys_triangle_shape *shape = this;
    shape_centre.set_add4(shape->get_vertices()[0], shape->get_vertices()[1]);
    shape_centre.add4(shape->get_vertices()[1]);
    shape_centre.mul4(phys_simd_scalar(0.33333334f));
}

// @address 0x6232a8 ?get_aabb_impl@phys_triangle_shape@@UBAXABVphys_transform@@MAAVphys_aabb@@@Z
void phys_triangle_shape::get_aabb_impl(const phys_transform &transform, float tolerance, class phys_aabb &aabb) const
{
    phys_vec4 vert1_pos;
    phys_vec4 vert0_pos;
    phys_vec4 vert2_pos;
    vert1_pos(0) = m_radius + tolerance;
    __vector4 aabb_expand = __vspltw(vert1_pos.m_simd_quad, 0);
    vert0_pos.vec_set_transformed_pos(transform, get_vertex(0));
    vert1_pos.vec_set_transformed_pos(transform, get_vertex(1));
    vert2_pos.vec_set_transformed_pos(transform, get_vertex(2));
    __vector4 a = vert0_pos.m_simd_quad;
    __vector4 b = vert1_pos.m_simd_quad;
    __vector4 c = vert2_pos.m_simd_quad;
    __vector4 vert_min = __vminfp(__vminfp(a, b), c);
    __vector4 mx = __vmaxfp(__vmaxfp(a, b), c);
    aabb.m_min_limit.m_simd_quad = vert_min;
    aabb.m_max_limit.m_simd_quad = mx;
    aabb.m_min_limit.m_simd_quad = __vminfp(vert_min, __vaddfp(vert_min, m_extrusion_vec.m_simd_quad));
    aabb.m_max_limit.m_simd_quad = __vmaxfp(mx, __vaddfp(mx, m_extrusion_vec.m_simd_quad));
    aabb.m_min_limit.m_simd_quad = __vsubfp(aabb.m_min_limit.m_simd_quad, aabb_expand);
    aabb.m_max_limit.m_simd_quad = __vaddfp(aabb.m_max_limit.m_simd_quad, aabb_expand);
}

void fixup_loaded_triangle_shape(void *object)
{
    phys_fixup_tag flag;
    flag.m_fixup_finishing = 1;
    new (object) phys_triangle_shape(flag);
}

const void *get_triangle_shape_vtable()
{
    phys_fixup_tag flag;
    union { __vector4 alignment; char storage[sizeof(phys_triangle_shape)]; } buffer;
    new (buffer.storage) phys_triangle_shape(flag);
    return *(const void **)buffer.storage;
}

void phys_triangle_shape::get_support_vertex_impl(const phys_vec4 &direction, phys_hull_vertex &supporting_vertex_out) const
{
    const phys_triangle_shape *shape = this;
    const phys_vec4 *vertices = shape->get_vertices();
    phys_vec4 dot_products;
    dot_products.set_rotated_inverse_dir(*reinterpret_cast<const phys_rotation *>(vertices), direction);
    int id;
    float max_dot;
    if (dot_products(2) > dot_products(1)) {
        id = 2;
        max_dot = dot_products(2);
    } else {
        id = 1;
        max_dot = dot_products(1);
    }
    id = dot_products(0) > max_dot ? 0 : id;
    static_cast<phys_vec4 &>(supporting_vertex_out) = shape->get_vertices()[id];
    if (direction.dot3(shape->m_extrusion_vec) > 0.0f) {
        id += 3;
        supporting_vertex_out.add4(shape->m_extrusion_vec);
    }
    supporting_vertex_out.set_int24_w(id);
}

// @address 0x623238 ?compute_ccw_triangle_normal@@YAXABVphys_vec4@@00AAV1@@Z
void compute_ccw_triangle_normal(const phys_vec4 &vertex0, const phys_vec4 &vertex1, const phys_vec4 &vertex2, phys_vec4 &normal)
{
    phys_vec4 edges[2];
    {
        edges[0].set_sub4(vertex1, vertex0);
        edges[1].set_sub4(vertex2, vertex1);
    }
    normal.set_cross(edges[0], edges[1]);
    normal.normalize_xyz();
}

// @address 0x623388 ?get_collision_spheres_impl@phys_triangle_shape@@UBAPBVphys_sphere@@PAV2@@Z
const phys_sphere *phys_triangle_shape::get_collision_spheres_impl(phys_sphere *sphere_buffer) const
{
    const phys_triangle_shape *shape = this;
    phys_sphere *s = sphere_buffer;
    {
        s->m_sphere_center = shape->get_vertices()[0];
        s->m_sphere_center(3) = m_radius;
        s++;
    }
    {
        s->m_sphere_center = shape->get_vertices()[1];
        s->m_sphere_center(3) = m_radius;
        s++;
    }
    {
        s->m_sphere_center = shape->get_vertices()[2];
        s->m_sphere_center(3) = m_radius;
        s++;
    }
    if (m_extruded)
    {
        {
            phys_vec4 &pos = s->get_position_and_radius();
            pos.set_add4(shape->get_vertices()[0], shape->m_extrusion_vec);
            pos(3) = m_radius;
            s++;
        }
        {
            phys_vec4 &pos = s->get_position_and_radius();
            pos.set_add4(shape->get_vertices()[1], shape->m_extrusion_vec);
            pos(3) = m_radius;
            s++;
        }
        {
            phys_vec4 &pos = s->get_position_and_radius();
            pos.set_add4(shape->get_vertices()[2], shape->m_extrusion_vec);
            pos(3) = m_radius;
            s++;
        }
    }
    return sphere_buffer;
}

void destroy_loaded_triangle_shape(void *p)
{
    static_cast<phys_triangle_shape *>(p)->~phys_triangle_shape();
}



static __forceinline void phys_vec4_set_interpolate4(phys_vec4 &out, const phys_vec4 &a, const phys_vec4 &b, const phys_simd_scalar &t)
{
    __vector4 x = __vspltw(t.m_simd_scalar, 0);
    __vector4 s = __vsubfp(phys_simd_vec_1111, x);
    __vector4 temp = __vmulfp(s, a.m_simd_quad);
    out.m_simd_quad = __vmaddfp(x, b.m_simd_quad, temp);
}

// @address 0x623440 ?trace_ray_impl@phys_triangle_shape@@UBA?AVphys_bool@@ABUphys_ray_input@@AAUphys_ray_output@@@Z
phys_bool phys_triangle_shape::trace_ray_impl(const phys_ray_input &input, phys_ray_output &results) const
{
    const phys_triangle_shape *shape = this;
    const phys_vec4 &a = shape->get_vertices()[0];
    const phys_vec4 &b = shape->get_vertices()[1];
    const phys_vec4 &c = shape->get_vertices()[2];
    phys_vec4 ab;
    ab.set_sub4(b, a);
    phys_vec4 ac;
    ac.set_sub4(c, a);
    phys_vec4 n;
    n.set_cross(ab, ac);
    const float d = a.dot3(n);
    const float dist_from = (float)n.dot3(input.m_ray_from) - d;
    const float dist_to = (float)n.dot3(input.m_ray_to) - d;
    if ((dist_from * dist_to < 0.0f) || (dist_from == 0.0f && dist_to != 0.0f)) {
        float t = dist_from / (dist_from - dist_to);
        if (t < results.m_hit_frac) {
            const float float_epsilon = (float)n.dot3(n) * -0.0001f;
            phys_vec4 p;
            phys_vec4_set_interpolate4(p, input.m_ray_from, input.m_ray_to, t);
            phys_vec4 ap;
            ap.set_sub4(a, p);
            phys_vec4 bp;
            bp.set_sub4(b, p);
            phys_vec4 cross;
            cross.set_cross(ap, bp);
            if ((float)cross.dot3(n) < float_epsilon)
                return false;
            phys_vec4 cp;
            cp.set_sub4(c, p);
            cross.set_cross(bp, cp);
            if ((float)cross.dot3(n) < float_epsilon)
                return false;
            cross.set_cross(cp, ap);
            if ((float)cross.dot3(n) < float_epsilon)
                return false;
            results.m_hit_frac = t;
            if (dist_from > 0.0f)
                results.m_normal = n;
            else
                results.m_normal.set_neg4(n);
            results.m_normal.normalize_xyz();
            results.set_cached_key(-1);
            return true;
        }
    }
    return false;
}

namespace phys_math {
    }
phys_edge_weld_util::weld_sector_kind phys_edge_weld_util::get_sector(const phys_vec4 &edge_weld_triangle_normal,
    const phys_vec4 &collision_normal, int weld_edge_bitcode)
{
    float weld_cos_angle = edge_weld_triangle_normal.dot3(collision_normal);
    if (weld_cos_angle >= m_sin_cos_table[weld_edge_bitcode].m_cos_accept0)
        return WELD_ACCEPT_A;
    else if (weld_cos_angle >= m_sin_cos_table[weld_edge_bitcode].m_cos_snap0)
        return WELD_SNAP_A;
    else if (weld_cos_angle >= m_sin_cos_table[weld_edge_bitcode].m_cos_snap1)
        return WELD_REJECT;
    else if (weld_cos_angle >= m_sin_cos_table[weld_edge_bitcode].m_cos_accept1)
        return WELD_SNAP_B;
    return WELD_ACCEPT_B;
}
phys_bool phys_edge_weld_util::should_snap_one_sided(edge_weld_kind edge_weld_type, const phys_vec4 &edge_weld_triangle_normal,
    const phys_vec4 &collision_normal, int weld_edge_bitcode)
{
    float weld_cos_angle = edge_weld_triangle_normal.dot3(collision_normal);
    float *sin_cos_entry = &m_sin_cos_table[weld_edge_bitcode].m_cos_accept0;
    return (edge_weld_type == EDGE_WELD_ANTICLOCKWISE) ? weld_cos_angle < sin_cos_entry[edge_weld_type] : weld_cos_angle > sin_cos_entry[edge_weld_type];
}
void phys_edge_weld_util::compute_snap_vector(const phys_vec4 &edge_weld_triangle_normal, const phys_vec4 &edge, int weld_edge_bitcode,
    weld_sector_kind sector, phys_vec4 &snap_vec_out)
{
    phys_vec4 orth_normal_edge;
    orth_normal_edge.set_cross(edge, edge_weld_triangle_normal);
    float *sin_cos_entry = &m_sin_cos_table[weld_edge_bitcode].m_cos_accept0;
    phys_vec4 half_vec0;
    half_vec0.set_mul4(sin_cos_entry[sector + 0], edge_weld_triangle_normal);
    phys_vec4 half_vec1;
    half_vec1.set_mul4(sin_cos_entry[sector + 1], orth_normal_edge);
    snap_vec_out.set_add4(half_vec0, half_vec1);
    snap_vec_out.normalize_xyz();
}
void phys_edge_weld_util::compute_one_sided_snap_vector(
    const phys_vec4 &edge_weld_triangle_normal, const phys_vec4 &edge, int weld_edge_bitcode, edge_weld_kind edge_weld_type, phys_vec4 &snap_vec_out)
{
    compute_snap_vector(edge_weld_triangle_normal, edge, weld_edge_bitcode, (weld_sector_kind)edge_weld_type, snap_vec_out);
}
void phys_edge_weld_util::snap_collision_normal(
    const phys_vec4 &edge_weld_triangle_normal, const phys_vec4 &edge, int weld_edge_bitcode, weld_sector_kind sector, phys_vec4 &collision_normal_in_out)
{
    phys_vec4 snap_vec;
    compute_snap_vector(edge_weld_triangle_normal, edge, weld_edge_bitcode, sector, snap_vec);
    float snap_dot = snap_vec.dot3(collision_normal_in_out);
    float edge_dot = edge.dot3(collision_normal_in_out);
    {
        phys_vec4 half_vec0;
        half_vec0.set_mul4(snap_dot, snap_vec);
        phys_vec4 half_vec1;
        half_vec1.set_mul4(edge_dot, edge);
        collision_normal_in_out.set_add4(half_vec0, half_vec1);
        collision_normal_in_out.normalize_xyzw();
    }
}

// @address 0x623778 ?weld_contact_point_impl@phys_triangle_shape@@UBAHPAGAAEAAVphys_vec4@@PBVphys_transform@@PBVphys_convex_shape@@32@Z
int phys_triangle_shape::weld_contact_point_impl(unsigned short *feature_points, unsigned char &feature_point_count, phys_vec4 &contact_point_world,
    const phys_transform *this_obj_transform, const phys_convex_shape *colliding_shape, const phys_transform *colliding_transform,
    phys_vec4 &separating_normal_in_out) const
{
    const phys_triangle_shape *this_obj = this;
    const int *mod3_table = k_phys_triangle_edge_weld_table.vertex;
    if (this_obj->m_edge_weld_type == phys_edge_weld_util::EDGE_WELD_NONE)
        return 2;
    int vert_id0 = mod3_table[feature_points[0]];
    int vert_id1;
    if (feature_point_count > 1) {
        vert_id1 = mod3_table[feature_points[1]];
        if (feature_point_count == 3) {
            if (feature_points[0] + feature_points[1] + feature_points[2] == 3)
                return 2;
            if (vert_id1 == vert_id0)
                vert_id1 = mod3_table[feature_points[2]];
            feature_point_count = 2;
            feature_points[0] = (unsigned short)vert_id0;
            feature_points[1] = (unsigned short)vert_id1;
        }
        if (mod3_table[vert_id0 + 1] != vert_id1)
            vert_id0 = vert_id1;
    }
    vert_id1 = mod3_table[vert_id0 + 1];
    float dist = separating_normal_in_out(3);
    phys_vec4 local_sep_normal;
    local_sep_normal.vec_set_rotated_inverse_dir(this_obj_transform->get_rotation(), separating_normal_in_out);
    phys_vec4 edge_weld_triangle_normal;
    compute_ccw_triangle_normal(get_vertex(0), get_vertex(1), get_vertex(2), edge_weld_triangle_normal);
    phys_vec4 edge;
    edge.set_sub4(get_vertex(vert_id1), get_vertex(vert_id0));
    edge.normalize_xyz();
    int weld_edge_bitcode = get_single_edge_bitcode(m_edge_weld_info, vert_id0);
    if (this_obj->m_edge_weld_type != phys_edge_weld_util::EDGE_WELD_TWO_SIDED) {
        if (!phys_edge_weld_util::should_snap_one_sided((phys_edge_weld_util::edge_weld_kind)this_obj->m_edge_weld_type,
            edge_weld_triangle_normal, local_sep_normal, weld_edge_bitcode))
            return 2;
        phys_edge_weld_util::compute_one_sided_snap_vector(
            edge_weld_triangle_normal, edge, weld_edge_bitcode, (phys_edge_weld_util::edge_weld_kind)this_obj->m_edge_weld_type, separating_normal_in_out);
        separating_normal_in_out.vec_set_rotated_dir(this_obj_transform->get_rotation(), separating_normal_in_out);
        if (m_extruded) {
            edge_weld_triangle_normal.mul4(k_phys_triangle_edge_weld_table.weld_type_normal_flip[this_obj->m_edge_weld_type]);
            phys_vec4 local_contact_pos;
            local_contact_pos.vec_set_transformed_inverse_pos(*this_obj_transform, contact_point_world);
            phys_vec4 depth_vec;
            depth_vec.set_sub4(get_vertex(0), local_contact_pos);
            float depth = -depth_vec.dot3(edge_weld_triangle_normal);
            separating_normal_in_out(3) = phys_math::min_of_two(depth, dist);
            return 1;
        } else {
            separating_normal_in_out(3) = dist;
            phys_vec4 center_a;
            colliding_shape->get_shape_centre(center_a);
            center_a.vec_set_transformed_pos(*colliding_transform, center_a);
            phys_vec4 center_to_plane;
            center_to_plane.set_sub4(contact_point_world, center_a);
            if ((float)center_to_plane.dot3(separating_normal_in_out) > 0.0f)
                return 0;
            return 1;
        }
    } else {
        phys_edge_weld_util::weld_sector_kind sector = phys_edge_weld_util::get_sector(edge_weld_triangle_normal, separating_normal_in_out, weld_edge_bitcode);
        switch (sector) {
        case phys_edge_weld_util::WELD_REJECT:
            return 0;
        case phys_edge_weld_util::WELD_SNAP_A:
        case phys_edge_weld_util::WELD_SNAP_B:
            phys_edge_weld_util::snap_collision_normal(edge_weld_triangle_normal, edge, weld_edge_bitcode, sector, separating_normal_in_out);
            return 1;
        default:
            break;
        }
    }
    return 2;
}
