#ifndef INCLUDE_PHYS_BVH_RAY_VM_H
#define INCLUDE_PHYS_BVH_RAY_VM_H

#include "phys/phys_bvh_vm.h"
#include "phys/phys_collide.h"
extern "C" { extern char g_phys_bvh_ray_flying_colors_flag; }

struct phys_bvh_ray_vm_query
{
    int m_start_x;
    int m_start_y;
    int m_start_z;
    int m_radius;
    int m_end_x;
    int m_end_y;
    int m_end_z;
    int m_padding;
    int m_obb_offset_x;
    int m_obb_offset_y;
    int m_obb_offset_z;
    int m_bit_shift;
    unsigned int m_primitive_offset;
    unsigned int m_properties[1];
};

class phys_bvh_ray_vm : public phys_bvh_vm
{
public:
    void query_ray(const phys_bvh_bytecode *code, const phys_vec4 &start, const phys_vec4 &end, float radius,
        phys_array<phys_bvh_primitive_info> *found_primitives);

protected:
    int m_start_x;
    int m_start_y;
    int m_start_z;
    int m_radius;
    int m_end_x;
    int m_end_y;
    int m_end_z;
    int m_padding;
    void query_ray_on_tree(const phys_bvh_ray_vm_query *query, const unsigned char *bvh_commands);
};

#endif // HAVOK_HK_MOPP_RAY_VIRTUAL_MACHINE_H
