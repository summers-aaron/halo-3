#ifndef INCLUDE_PHYS_BVH_OBB_VM_H
#define INCLUDE_PHYS_BVH_OBB_VM_H

#include "phys/phys_bvh_vm.h"
#include "phys/phys_collide.h"
extern "C" { extern char g_phys_bvh_obb_flying_colors_flag; }

struct phys_bvh_obb_vm_query {
    __declspec(align(16)) int m_x_hi;
    int m_y_hi;
    int m_z_hi;
    int m_hi_padding;
    int m_x_lo;
    int m_y_lo;
    int m_z_lo;
    int m_lo_padding;
    int m_obb_offset_x;
    int m_obb_offset_y;
    int m_obb_offset_z;
    unsigned int m_primitive_offset;
    int m_bit_shift;
    unsigned int m_properties[1];
};

class phys_bvh_obb_vm : public phys_bvh_vm {
public:
    void query_obb(const phys_bvh_bytecode *code, const phys_transform &bounding_to_world, const phys_vec4 &extent,
        const float radius, phys_array<phys_bvh_primitive_info> *found_primitives);
    void query_aabb(const phys_bvh_bytecode *code, const phys_aabb &aabb, phys_array<phys_bvh_primitive_info> *found_primitives);

protected:
    __declspec(align(16)) phys_bvh_fixed_point m_x_hi;
    phys_bvh_fixed_point m_y_hi;
    phys_bvh_fixed_point m_z_hi;
    phys_bvh_fixed_point m_hi_padding;
    phys_bvh_fixed_point m_x_lo;
    phys_bvh_fixed_point m_y_lo;
    phys_bvh_fixed_point m_z_lo;
    phys_bvh_fixed_point m_lo_padding;
    phys_padded<const phys_bvh_bytecode *> m_bvh_code;
    phys_padded<int> m_reindex_mask;

    void query_aabb_on_tree(const phys_bvh_obb_vm_query *query, const unsigned char *bvh_commands, int bvh_chunk_offset);
    __forceinline void generate_query_from_node(const phys_vec4 &extent, const phys_transform &bounding_to_world, const float radius,
        phys_bvh_obb_vm_query &query);
    __forceinline void generate_query_from_aabb(const phys_vec4 &aabb_min, const phys_vec4 &aabb_max, phys_bvh_obb_vm_query &query);
};

__forceinline void phys_bvh_obb_vm::generate_query_from_aabb(const phys_vec4 &aabb_min, const phys_vec4 &aabb_max, phys_bvh_obb_vm_query &query)
{
    const phys_vec4 &max_corner = aabb_max;
    const phys_vec4 &min_vertex = aabb_min;

    m_x_lo = to_int_min((min_vertex(0) - m_bvh_code->m_bvh_info.m_offset(0)) * m_bvh_code->m_bvh_info.get_offset_scale());
    m_x_hi = to_int_max((max_corner(0) - m_bvh_code->m_bvh_info.m_offset(0)) * m_bvh_code->m_bvh_info.get_offset_scale());
    m_y_lo = to_int_min((min_vertex(1) - m_bvh_code->m_bvh_info.m_offset(1)) * m_bvh_code->m_bvh_info.get_offset_scale());
    m_y_hi = to_int_max((max_corner(1) - m_bvh_code->m_bvh_info.m_offset(1)) * m_bvh_code->m_bvh_info.get_offset_scale());
    m_z_lo = to_int_min((min_vertex(2) - m_bvh_code->m_bvh_info.m_offset(2)) * m_bvh_code->m_bvh_info.get_offset_scale());
    m_z_hi = to_int_max((max_corner(2) - m_bvh_code->m_bvh_info.m_offset(2)) * m_bvh_code->m_bvh_info.get_offset_scale());

    query.m_x_lo = (m_x_lo >> 16);
    query.m_x_hi = (m_x_hi >> 16) + 1;
    query.m_y_lo = (m_y_lo >> 16);
    query.m_y_hi = (m_y_hi >> 16) + 1;
    query.m_z_lo = (m_z_lo >> 16);
    query.m_z_hi = (m_z_hi >> 16) + 1;

    query.m_obb_offset_x = 0;
    query.m_obb_offset_y = 0;
    query.m_obb_offset_z = 0;
    query.m_primitive_offset = 0;
    query.m_bit_shift = 0;
    query.m_properties[0] = 0;
}

#endif // HAVOK_HK_MOPP_OBB_VIRTUAL_MACHINE_H
