#ifndef INCLUDE_PHYS_BVH_FIND_ALL_VM_H
#define INCLUDE_PHYS_BVH_FIND_ALL_VM_H

#include "phys/phys_bvh_vm.h"
#include "phys/phys_collide.h"

// March: constructor 8353c5c8, destructor 8353c5f8 and queryAll 8353c620 are inline.
class phys_bvh_find_all_vm : public phys_bvh_vm {
public:
    __forceinline phys_bvh_find_all_vm() {}
    __forceinline ~phys_bvh_find_all_vm() {}
    __forceinline void query_all_leaves(const phys_bvh_bytecode *code, phys_array<phys_bvh_primitive_info> *found_primitives);

    struct phys_bvh_find_all_vm_query {
        unsigned int m_primitive_offset;
        unsigned int m_properties[phys_bvh_bytecode::PRIMITIVE_PROP_MAX];
    };

    void query_on_tree(const phys_bvh_find_all_vm_query *query, const unsigned char *bvh_commands);
    void query_on_tree_left(const phys_bvh_find_all_vm_query *query, const unsigned char *bvh_commands);
    void query_on_tree_right(const phys_bvh_find_all_vm_query *query, const unsigned char *bvh_commands);
};

__forceinline void phys_bvh_find_all_vm::query_all_leaves(const phys_bvh_bytecode *code, phys_array<phys_bvh_primitive_info> *found_primitives)
{
    m_out_primitives = found_primitives;
    phys_bvh_find_all_vm_query query;
    query.m_primitive_offset = 0;
    query.m_properties[0] = 0;
    query_on_tree(&query, &code->m_data[0]);
}

#endif // HAVOK_HK_MOPP_FIND_ALL_VIRTUAL_MACHINE_H
