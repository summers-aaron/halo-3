#ifndef INCLUDE_PHYS_BVH_TREE_STREAM_AGENT_H
#define INCLUDE_PHYS_BVH_TREE_STREAM_AGENT_H
class phys_dispatcher;
class phys_bvh_tree_stream_agent {
public:
    static void register_agent(phys_dispatcher *dispatcher);
};
#endif
