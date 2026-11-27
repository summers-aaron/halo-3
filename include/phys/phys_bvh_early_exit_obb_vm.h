#ifndef INCLUDE_PHYS_BVH_EARLY_EXIT_OBB_VM_H
#define INCLUDE_PHYS_BVH_EARLY_EXIT_OBB_VM_H

#include "phys/phys_bvh_vm.h"
#include "phys/phys_collide.h"
extern "C" { extern char g_phys_bvh_early_exit_obb_enabled; }

struct phys_bvh_early_exit_obb_vm_query {
    int m_x_hi;
    int m_y_hi;
    int m_z_hi;
    int m_hi_padding;

    int m_x_lo;
    int m_y_lo;
    int m_z_lo;
    int m_lo_padding;

    int m_center_x;
    int m_center_y;
    int m_center_z;
    int m_center_radius;

    int m_obb_offset_x;
    int m_obb_offset_y;
    int m_obb_offset_z;
    unsigned int m_primitive_offset;
    int m_bit_shift;
    unsigned int m_properties[1];
};

// Callers reserve a 16-byte aligned 80-byte machine on the stack.
class __declspec(align(16)) phys_bvh_early_exit_obb_vm : public phys_bvh_vm {
public:
    __forceinline phys_bvh_early_exit_obb_vm() {}
    __forceinline ~phys_bvh_early_exit_obb_vm() {}
    phys_bool query_obb(const phys_bvh_bytecode *code, const phys_transform &bounding_to_world, const phys_vec4 &extent, const float &radius);

protected:
    phys_bvh_fixed_point m_x_hi;
    phys_bvh_fixed_point m_y_hi;
    phys_bvh_fixed_point m_z_hi;
    phys_bvh_fixed_point m_hi_padding;

    phys_bvh_fixed_point m_x_lo;
    phys_bvh_fixed_point m_y_lo;
    phys_bvh_fixed_point m_z_lo;
    phys_bvh_fixed_point m_lo_padding;

    phys_bvh_fixed_point m_center_x;
    phys_bvh_fixed_point m_center_y;
    phys_bvh_fixed_point m_center_z;
    phys_bvh_fixed_point m_center_radius;

    phys_bool m_obb_hit_found;

    const phys_bvh_bytecode *m_bvh_code;

    __forceinline void add_cast_hit(unsigned int id, const unsigned int properties[1]) { m_obb_hit_found = true; }

    void query_obb_on_tree(const phys_bvh_early_exit_obb_vm_query *query, const unsigned char *bvh_commands);
    void generate_query_from_node(const phys_vec4 &extent, const phys_transform &bounding_to_world, const float &radius,
        phys_bvh_early_exit_obb_vm_query &query);
};

#endif // HAVOK_HK_MOPP_EARLY_EXIT_OBB_VIRTUAL_MACHINE_H
