#ifndef INCLUDE_PHYS_BVH_VM_H
#define INCLUDE_PHYS_BVH_VM_H

#include "phys/phys_base.h"

class phys_bvh_primitive_info {
public:
    PHYS_DECLARE_NONVIRTUAL_ALLOCATOR(PHYS_MEM_BVH, phys_bvh_primitive_info);
    unsigned int ID;
};

// March: 82a9b190/82a9b1f0, 8353c898, 8353e7a0, 8353e888, 835441c0/835441f0.
class phys_bvh_vm {
public:
    PHYS_DECLARE_NONVIRTUAL_ALLOCATOR(PHYS_MEM_BVH, phys_bvh_vm);
    typedef int phys_bvh_fixed_point;
    __forceinline phys_bvh_vm() {}
    __forceinline ~phys_bvh_vm() {}
    __forceinline int get_hit_count() const { return m_out_primitives->getSize(); }
protected:
    __forceinline void init_vm_query(phys_array<phys_bvh_primitive_info> *found_primitives) { m_out_primitives = found_primitives; }
    __forceinline void add_cast_hit(unsigned int id, const unsigned int properties[1]) {
        phys_bvh_primitive_info info;
        info.ID = id;
        m_out_primitives->array_push_back(info);
    }
    static __forceinline int to_int_min(float x) { return phys_math::phys_to_int_fast(x) - 1; }
    static __forceinline int to_int_max(float x) { return phys_math::phys_to_int_fast(x) + 1; }
    static __forceinline int read_u24(const unsigned char *PC) { return (PC[0] << 16) + (PC[1] << 8) + PC[2]; }
public:
    phys_array<phys_bvh_primitive_info> *m_out_primitives;
    int : 32;
    int : 32;
    int : 32;
};

typedef char havok_size_hkMoppPrimitiveInfo[(sizeof(phys_bvh_primitive_info) == 4) ? 1 : -1];
typedef char havok_size_hkMoppVirtualMachine[(sizeof(void *) != 4 || ((sizeof(phys_bvh_vm) == 16) )) ? 1 : -1];

#endif // HAVOK_HK_MOPP_VIRTUAL_MACHINE_H
