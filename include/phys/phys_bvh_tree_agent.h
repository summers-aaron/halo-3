#ifndef INCLUDE_PHYS_BVH_TREE_AGENT_H
#define INCLUDE_PHYS_BVH_TREE_AGENT_H


class phys_dispatcher;
#include "phys/phys_bvh_agent.h"

class phys_bvh_tree_agent : public phys_bvh_agent {
public:
  static void register_agent(phys_dispatcher *dispatcher);
  static phys_agent *create_bvh_bvh_agent(const phys_collision_proxy &, const phys_collision_proxy &, const phys_collision_input &, phys_contact_manager *);
  static void static_sweep_cast(const phys_collision_proxy &, const phys_collision_proxy &, const phys_linear_cast_collision_input &,
      phys_hit_collector &, phys_hit_collector *);
protected:
  phys_bvh_tree_agent(phys_contact_manager *agent_contact_manager);
};

typedef char phys_bvh_agent_layout_size[(sizeof(void *) != 4 || ((sizeof(phys_bvh_tree_agent) == 64) )) ? 1 : -1];

#endif
