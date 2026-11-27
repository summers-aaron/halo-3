#ifndef INCLUDE_PHYS_TYPED_BROADPHASE_DISPATCHER_H
#define INCLUDE_PHYS_TYPED_BROADPHASE_DISPATCHER_H

#include <stddef.h>

#include "phys/phys_base.h"

class phys_broadphase_listener;
class phys_typed_broadphase_pair;
class phys_broadphase_handle_pair;
class phys_collider_pair_filter;

class phys_typed_broadphase_dispatcher {
public:
    PHYS_DECLARE_NONVIRTUAL_ALLOCATOR(PHYS_MEM_COLLISION_INFO, phys_typed_broadphase_dispatcher);
    phys_typed_broadphase_dispatcher();
    ~phys_typed_broadphase_dispatcher();
    void remove_handle_pairs(phys_typed_broadphase_pair *pairs, int count) const;
    static void remove_duplicate_pairs(phys_array<phys_broadphase_handle_pair> &new_pairs, phys_array<phys_broadphase_handle_pair> &sweep_removed_pairs);
    void add_handle_pairs(phys_typed_broadphase_pair *pairs, int count, const phys_collider_pair_filter *filter) const;
    __forceinline void set_broadphase_listener(phys_broadphase_listener *listener, int shape_type_a, int shape_type_b) {
        m_pair_listeners[shape_type_a][shape_type_b] = listener;
    }
    phys_broadphase_listener *m_pair_listeners[8][8];
    phys_broadphase_listener *m_null_listener;
};

typedef char havok_size_hkTypedBroadPhaseDispatcher[(sizeof(void *) != 4 || ((sizeof(phys_typed_broadphase_dispatcher) == 260) )) ? 1 : -1];
typedef char havok_offset_hkTypedBroadPhaseDispatcher_null[(sizeof(void *) != 4 || ((offsetof(phys_typed_broadphase_dispatcher,
    m_null_listener) == 256) )) ? 1 : -1];

#endif
