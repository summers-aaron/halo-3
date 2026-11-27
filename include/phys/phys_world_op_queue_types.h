#ifndef INCLUDE_PHYS_WORLD_OP_QUEUE_TYPES_H
#define INCLUDE_PHYS_WORLD_OP_QUEUE_TYPES_H

#include "phys/phys_base.h"
#include "phys/phys_world_op_queue.h"
#include "phys/phys_critical_section.h"

class phys_world;

class phys_world_op_queue {
public:
    PHYS_DECLARE_NONVIRTUAL_ALLOCATOR(PHYS_MEM_WORLD, phys_world_op_queue);
    phys_world_op_queue(phys_world *world);
    ~phys_world_op_queue();
    void queue_world_op(const phys_world_op::base_op &operation);
    void execute_all_pending();

    phys_array<phys_world_op::biggest_op> m_pending_ops;
    phys_world *m_world;
    phys_array<phys_world_op::biggest_op> m_pending_island_merges;
    phys_bool m_defer_island_merges;
    phys_critical_section m_display_lock;
};

#if defined(_M_PPCBE) || defined(_XBOX)
typedef char phys_world_op_queue_size[(sizeof(void *) != 4 || (sizeof(phys_world_op_queue) == 80)) ? 1 : -1];
typedef char phys_world_op_queue_merge_flag[(sizeof(void *) != 4 || (offsetof(phys_world_op_queue, m_defer_island_merges) == 28)) ? 1 : -1];
#endif

#endif
