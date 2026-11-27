#pragma once

#include "phys/phys_base_types.h"
#include <ppcintrinsics.h>
#include <vectorintrinsics.h>
#include "phys/phys_error.h"
#include "phys/phys_kdop26.h"
#include "phys/phys_bvh_commands.h"
#include "phys/phys_bvh_kdop_vm.h"
#include "phys/phys_bvh_modify_vm.h"
#include "phys/phys_bvh_early_exit_obb_vm.h"
#include "phys/phys_bvh_find_all_vm.h"
#include "phys/phys_bvh_sphere_vm.h"
#include "phys/phys_err_stream.h"
#include "phys/phys_thread_memory.h"
#include "phys/phys_aabb_types.h"
#include "phys/phys_collide.h"
#include "phys/phys_math.h"
#include "phys/phys_statistics_collector.h"
#include "phys/phys_writer.h"

// match: 0x67a0a8: the sphere query objects take the squared length straight from __vmsum3fp; the
// lengthSquared3 in havok/hk_math.h (with its named result) moves three registers in the sphere tree walk.
static __forceinline phys_simd_scalar phys_bvh_vm_length_squared3(const phys_vec4 &v)
{
    return phys_simd_scalar(__vmsum3fp(v.m_simd_quad, v.m_simd_quad));
}

template <class vm_query, class vm_query_object>
class __declspec(align(16)) phys_bvh_float_aabb_vm : public phys_bvh_vm {
public:
    struct phys_bvh_float_aabb_vm_query_int {
        unsigned int m_primitive_offset;
        unsigned int m_properties[phys_bvh_bytecode::PRIMITIVE_PROP_MAX];
        int m_obb_offset_x;
        int m_obb_offset_y;
        int m_obb_offset_z;
        int m_bit_shift;
    };
    struct phys_bvh_temp_aabb : public phys_aabb {
        __forceinline phys_bvh_temp_aabb() {}
    };
    __forceinline phys_bvh_float_aabb_vm() {}
    __forceinline ~phys_bvh_float_aabb_vm() {}
    __forceinline void query_bvh(const phys_bvh_bytecode *code, const vm_query &query,
                                 phys_array<phys_bvh_primitive_info> *partial_hits_out,
                                 phys_array<phys_bvh_primitive_info> *fully_included_hits_out)
    {
        const unsigned char *PC = &code->m_data[0];
        init_vm_query(partial_hits_out);
        phys_bvh_temp_aabb temp_aabb;
        m_reciprocal_scale = 1.0f / code->m_bvh_info.get_offset_scale();
        phys_bvh_float_aabb_vm_query_int query_internal;
        query_internal.m_obb_offset_x = 0;
        query_internal.m_obb_offset_y = 0;
        query_internal.m_obb_offset_z = 0;
        query_internal.m_bit_shift = 16;
        query_internal.m_primitive_offset = 0;
        for (int i = 0; i < phys_bvh_bytecode::PRIMITIVE_PROP_MAX; i++) {
            query_internal.m_properties[i] = 0;
        }
        temp_aabb.m_min_limit.set_zero4();
        temp_aabb.m_max_limit.set_all3(m_reciprocal_scale * 16777216.0f);
        m_query_object.init_query_object(query, code);
        m_query_object.m_out_primitives = fully_included_hits_out;
        query_bvh_tree(&query_internal, PC, temp_aabb);
    }
protected:
    void query_bvh_tree(const phys_bvh_float_aabb_vm_query_int *query, const unsigned char *PC,
                       phys_bvh_temp_aabb &temp_aabb);
    float m_reciprocal_scale;
    vm_query_object m_query_object;
};

struct phys_bvh_planes_query_input {
    enum { PHYS_MAX_NUM_PLANES = 32 };
    int m_plane_count;
    const phys_vec4 *m_aabb_planes;
};

struct phys_bvh_query_object {
    enum phys_check_collision_result {
        PHYS_BVH_NO_HIT = 0,
        PHYS_BVH_FULL_HIT = 1,
        PHYS_BVH_PARTIAL_HIT = 2
    };
};

class __declspec(align(16)) phys_bvh_planes_query : public phys_bvh_find_all_vm {
public:
    __forceinline phys_bvh_planes_query() {}
    __forceinline ~phys_bvh_planes_query() {}
    __forceinline void init_query_object(const phys_bvh_planes_query_input &in, const phys_bvh_bytecode *code)
    {
        int plane_quad_count = in.m_plane_count & ~3;
        phys_transform *transform = &m_aabb_planes[0];
        const phys_vec4 *planes = in.m_aabb_planes;
        m_transform_count = 0;
        int i;
        for (i = 0; i < plane_quad_count; i += 4) {
            transform->set_rows4(planes[0], planes[1], planes[2], planes[3]);
            transform++;
            m_transform_count++;
            planes += 4;
        }
        const phys_vec4 *p0 = planes;
        const phys_vec4 *p1 = planes + 1;
        const phys_vec4 *p2 = planes + 2;
        const phys_vec4 *p3 = planes + 3;
        switch (in.m_plane_count - i) {
        case 1:
            p1 = p0;
        case 2:
            p2 = p0;
        case 3:
            p3 = p0;
            transform->set_rows4(*p0, *p1, *p2, *p3);
            transform++;
            m_transform_count++;
            break;
        }
        phys_transform *abs_transform = &m_abs_planes[m_transform_count] - 1;
        for (transform--; transform >= &m_aabb_planes[0]; transform--, abs_transform--) {
            abs_transform->get_column(0).set_abs4(transform->get_column(0));
            abs_transform->get_column(1).set_abs4(transform->get_column(1));
            abs_transform->get_column(2).set_abs4(transform->get_column(2));
            abs_transform->get_column(3).set_zero4();
            phys_vec4 offset;
            offset.set_mul_mat3_vec4(transform->get_rotation(), code->m_bvh_info.m_offset);
            transform->get_column(3).add4(offset);
            transform->get_column(3).add4(transform->get_column(3));
        }
    }
    __forceinline phys_bvh_query_object::phys_check_collision_result check_bounds_collision(const phys_aabb &aabb,
        const phys_bvh_find_all_vm::phys_bvh_find_all_vm_query *query, const unsigned char *PC)
    {
        int mask = 15;
        const phys_transform *plane = &m_aabb_planes[0];
        const phys_transform *abs_plane = &m_abs_planes[0];
        phys_vec4 center;
        center.set_add4(aabb.m_min_limit, aabb.m_max_limit);
        phys_vec4 extent;
        extent.set_sub4(aabb.m_max_limit, aabb.m_min_limit);
        for (int i = m_transform_count - 1; i >= 0; i--)
        {
            phys_vec4 proj_extent;
            proj_extent.set_mul_mat3_vec4(abs_plane->get_rotation(), extent);
            phys_vec4 proj_center;
            proj_center.set_mul_xform_xyz1(*plane, center);
            phys_vec4 min_dist;
            min_dist.set_sub4(proj_center, proj_extent);
            phys_vec4 max_dist_vec;
            max_dist_vec.set_add4(proj_center, proj_extent);
            if (min_dist.compare_less_zero4().get_mask() != 15)
            {
                return phys_bvh_query_object::PHYS_BVH_NO_HIT;
            }
            mask &= max_dist_vec.compare_less_zero4().get_mask();
            plane++;
            abs_plane++;
        }
        if (mask == 15)
        {
            query_on_tree(query, PC);
            return phys_bvh_query_object::PHYS_BVH_FULL_HIT;
        }
        return phys_bvh_query_object::PHYS_BVH_PARTIAL_HIT;
    }
    __declspec(align(16)) int m_transform_count;
    phys_transform m_aabb_planes[8];
    phys_transform m_abs_planes[8];
};

class __declspec(align(16)) phys_bvh_sphere_query : public phys_bvh_find_all_vm {
public:
    __forceinline phys_bvh_sphere_query() {}
    __forceinline ~phys_bvh_sphere_query() {}
    __forceinline void init_query_object(const phys_sphere &query, const phys_bvh_bytecode *code) {
        phys_vec4 position;
        position.set_sub4(query.get_contact_position(), code->m_bvh_info.m_offset);
        position.add4(position);
        m_display_sphere.contact_set_position(position);
        m_display_sphere.set_shape_radius(query.get_convex_radius() * 2.0f);
    }
    __forceinline phys_bvh_query_object::phys_check_collision_result check_bounds_collision(const phys_aabb &aabb,
        const phys_bvh_find_all_vm::phys_bvh_find_all_vm_query *query, const unsigned char *PC)
    {
        phys_vec4 center;
        center.set_add4(aabb.m_min_limit, aabb.m_max_limit);
        phys_vec4 extent;
        extent.set_sub4(aabb.m_max_limit, aabb.m_min_limit);
        center.sub4(m_display_sphere.get_contact_position());
        center.set_abs4(center);
        phys_vec4 closest;
        closest.set_min4(center, extent);
        phys_vec4 diff;
        diff.set_sub4(closest, center);
        const float bvh_dist_sq = phys_bvh_vm_length_squared3(diff);
        const float bvh_radius_squared = m_display_sphere.get_convex_radius() * m_display_sphere.get_convex_radius();
        if (bvh_dist_sq > bvh_radius_squared)
        {
            return phys_bvh_query_object::PHYS_BVH_NO_HIT;
        }
        phys_vec4 farthest_point;
        farthest_point.set_add4(center, extent);
        
        if (phys_bvh_vm_length_squared3(farthest_point) < bvh_radius_squared)
        {
            query_on_tree(query, PC);
            return phys_bvh_query_object::PHYS_BVH_FULL_HIT;
        }
        return phys_bvh_query_object::PHYS_BVH_PARTIAL_HIT;
    }
    phys_sphere m_display_sphere;
};

class __declspec(align(16)) phys_bvh_planes_query_fast : public phys_bvh_planes_query {
public:
    __forceinline phys_bvh_planes_query_fast() {}
    __forceinline ~phys_bvh_planes_query_fast() {}
    __forceinline phys_bvh_query_object::phys_check_collision_result check_bounds_collision(const phys_aabb &aabb,
        const phys_bvh_find_all_vm::phys_bvh_find_all_vm_query *query, const unsigned char *PC)
    {
        int mask = 15;
        const phys_transform *plane = &m_aabb_planes[0];
        const phys_transform *abs_plane = &m_abs_planes[0];
        phys_vec4 center;
        center.set_add4(aabb.m_min_limit, aabb.m_max_limit);
        phys_vec4 extent;
        extent.set_sub4(aabb.m_max_limit, aabb.m_min_limit);
        for (int i = m_transform_count - 1; i >= 0; i--)
        {
            phys_vec4 proj_extent;
            proj_extent.set_mul_xform_xyz1(*abs_plane, extent);
            phys_vec4 proj_center;
            proj_center.set_mul_xform_xyz1(*plane, center);
            phys_vec4 min_dist;
            min_dist.set_sub4(proj_center, proj_extent);
            phys_vec4 max_dist_vec;
            max_dist_vec.set_add4(proj_center, proj_extent);
            const int min_mask = min_dist.compare_less_zero4().get_mask();
            if (min_mask != 15)
            {
                return phys_bvh_query_object::PHYS_BVH_NO_HIT;
            }
            mask &= max_dist_vec.compare_less_zero4().get_mask();
            plane++;
            abs_plane++;
        }
        if (mask == 15)
        {
            query_on_tree_left(query, PC);
            query_on_tree_right(query, PC);
            return phys_bvh_query_object::PHYS_BVH_FULL_HIT;
        }
        return phys_bvh_query_object::PHYS_BVH_PARTIAL_HIT;
    }
};

class __declspec(align(16)) phys_bvh_sphere_query_fast : public phys_bvh_sphere_query {
public:
    __forceinline phys_bvh_sphere_query_fast() {}
    __forceinline ~phys_bvh_sphere_query_fast() {}
    __forceinline phys_bvh_query_object::phys_check_collision_result check_bounds_collision(const phys_aabb &aabb,
        const phys_bvh_find_all_vm::phys_bvh_find_all_vm_query *query, const unsigned char *PC)
    {
        phys_vec4 center;
        center.set_add4(aabb.m_min_limit, aabb.m_max_limit);
        phys_vec4 extent;
        extent.set_sub4(aabb.m_max_limit, aabb.m_min_limit);
        center.sub4(m_display_sphere.get_contact_position());
        center.set_abs4(center);
        phys_vec4 closest;
        closest.set_min4(center, extent);
        phys_vec4 diff;
        diff.set_sub4(closest, center);
        const float bvh_dist_sq = phys_bvh_vm_length_squared3(diff);
        const float bvh_radius_squared = m_display_sphere.get_convex_radius() * m_display_sphere.get_convex_radius();
        if (bvh_dist_sq > bvh_radius_squared)
        {
            return phys_bvh_query_object::PHYS_BVH_NO_HIT;
        }
        phys_vec4 farthest_point;
        farthest_point.set_add4(center, extent);
        
        if (phys_bvh_vm_length_squared3(farthest_point) < bvh_radius_squared)
        {
            query_on_tree_left(query, PC);
            query_on_tree_right(query, PC);
            return phys_bvh_query_object::PHYS_BVH_FULL_HIT;
        }
        return phys_bvh_query_object::PHYS_BVH_PARTIAL_HIT;
    }
};
