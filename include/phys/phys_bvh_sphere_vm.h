#ifndef INCLUDE_PHYS_BVH_SPHERE_VM_H
#define INCLUDE_PHYS_BVH_SPHERE_VM_H

#include "phys/phys_bvh_vm.h"
#include "phys/phys_collide.h"

extern "C" { extern char g_phys_bvh_sphere_flying_colors_flag; }

class phys_bvh_sphere_vm : public phys_bvh_vm {
public:
    struct phys_bvh_sphere_vm_query {
        int m_x;
        int m_y;
        int m_soa_z;
        int m_radius;
        int m_obb_offset_x;
        int m_obb_offset_y;
        int m_obb_offset_z;
        unsigned int m_primitive_offset;
        int m_bit_shift;
        unsigned int m_properties[1];
    };
    void shape_query_sphere(const phys_bvh_bytecode *code, const phys_sphere &sphere, phys_array<phys_bvh_primitive_info> *found_primitives);

protected:
    phys_bvh_fixed_point m_x;
    phys_bvh_fixed_point m_y;
    phys_bvh_fixed_point m_soa_z;
    phys_bvh_fixed_point m_radius;

    void query_sphere_on_tree(const phys_bvh_sphere_vm_query *query, const unsigned char *bvh_commands);
};

#endif // HAVOK_HK_MOPP_SPHERE_VIRTUAL_MACHINE_H
