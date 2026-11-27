#ifndef INCLUDE_PHYS_BVH_MODIFY_VM_H
#define INCLUDE_PHYS_BVH_MODIFY_VM_H

#include "phys/phys_bvh_obb_vm.h"

extern "C" {
 // @address 0x7ceb80 g_phys_bvh_modify_flying_colors
 extern char g_phys_bvh_modify_flying_colors;
}
class phys_bvh_modifier {
public:
    virtual ~phys_bvh_modifier();
    virtual phys_bool should_remove_terminal(unsigned int id, const unsigned int *properties);
    virtual void add_terminal_remove_signal(int x);
};
// March 0x8353c688 constructs the OBB machine before the modification state.
class phys_bvh_modify_vm : public phys_bvh_obb_vm {
public:
    void query_aabb(const phys_bvh_bytecode *code, const phys_aabb &aabb, phys_bvh_modifier *modifier_out);
protected:
    __forceinline void add_cast_hit(unsigned int id, const unsigned int properties[1], unsigned long chunk_id = 0)
    {
        m_last_terminal_removed = m_modifier->should_remove_terminal(id, properties);
    }
    phys_bool query_modify_points_rec(const phys_bvh_obb_vm_query *query, const unsigned char *bvh_commands, int chunk_id);
    phys_bool m_last_terminal_removed;
    phys_bvh_modifier *m_modifier;
    phys_padded<int> m_reindex_mask2;
};

#if defined(_M_PPCBE) || defined(_XBOX)
typedef char phys_bvh_modify_vm_size[(sizeof(void *) != 4 || sizeof(phys_bvh_modify_vm) == 0x50) ? 1 : -1];
#endif

#endif
