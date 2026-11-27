// hkcollide/hkMoppBvTreeStreamAgent.obj: from the records 0x6289c8 0x636f40 0x637028
// @compiler xdk6130
// @flags /TP /O2 /Oc
// @address 0x637028 ?register_agent@phys_bvh_tree_stream_agent@@SAXPAVphys_dispatcher@@@Z
// @address 0x6289c8 ?get_penetrations@?$phys_symmetric_agent_linear_cast@Vphys_bvh_agent@@@@UAAXABVphys_collision_proxy@@0ABUphys_collision_input@@AAVphys_proxy_pair_collector@@@Z
// @address 0x636f40 ?static_sweep_cast@?$phys_symmetric_agent_linear_cast@Vphys_bvh_tree_agent@@@@SAXABVphys_collision_proxy@@0ABUphys_linear_cast_collision_input@@AAVphys_hit_collector@@PAV4@@Z
#include <ppcintrinsics.h>
#include <vectorintrinsics.h>
#include "phys/phys_math.h"
#include "phys/phys_collide.h"
#include "phys/phys_symmetric_agent_linear_cast.h"


#include "phys/phys_dispatcher.h"
#include "phys/phys_bvh_agent.h"
#include "phys/phys_bvh_stream_agent.h"
#include "phys/phys_bvh_tree_agent.h"
#include "phys/phys_symmetric_agent.h"
#include "phys/phys_bvh_tree_stream_agent.h"
extern "C" {
}



#include "phys/phys_collision_input.h"
#include "phys/phys_collision_proxy.h"
#include "phys/phys_collectors.h"
#include "phys/phys_agent.h"
#include "phys/phys_contact_manager_factory.h"
#include "phys/phys_shape_type_enum.h"




namespace {
}

template <>
void phys_symmetric_agent_linear_cast<phys_bvh_tree_agent>::static_sweep_cast(const phys_collision_proxy &agent_body_a,
    const phys_collision_proxy &agent_body_b,
    const phys_linear_cast_collision_input &input, phys_hit_collector &cast_hit_collector, phys_hit_collector *start_collector)
{
    phys_linear_cast_collision_input flipped_input = input;
    flipped_input.m_path.set_neg4(input.m_path);
    phys_symmetric_flip_cast_collector flipped_collector(input.m_path, cast_hit_collector);
    if (start_collector) {
        phys_symmetric_flip_cast_collector flipped_start_collector(input.m_path, *start_collector);
        phys_bvh_tree_agent::static_sweep_cast(agent_body_b, agent_body_a, flipped_input, (phys_hit_collector &)flipped_collector,
            (phys_hit_collector *)&flipped_start_collector);
    } else {
        phys_bvh_tree_agent::static_sweep_cast(agent_body_b, agent_body_a, flipped_input, (phys_hit_collector &)flipped_collector, 0);
    }
}


template <>
void phys_symmetric_agent_linear_cast<phys_bvh_agent>::get_penetrations(
    const phys_collision_proxy &agent_body_a, const phys_collision_proxy &agent_body_b, const phys_collision_input &input, phys_proxy_pair_collector &collector)
{
    phys_symmetric_flip_pair_collector flipped_collector(collector);
    phys_bvh_agent::static_penetrations(agent_body_b, agent_body_a, input, flipped_collector);
}

void phys_bvh_tree_stream_agent::register_agent(phys_dispatcher *dispatcher)
{
    phys_dispatcher::agent_funcs forward;
    forward.m_create_fn = phys_bvh_stream_agent::create_bvh_shape_agent;
    forward.m_penetrations_fn = phys_symmetric_agent_linear_cast<phys_bvh_agent>::static_penetrations;
    forward.m_closest_points_fn = phys_symmetric_agent_linear_cast<phys_bvh_agent>::static_closest_points;
    forward.m_linear_cast_func = phys_symmetric_agent_linear_cast<phys_bvh_tree_agent>::static_sweep_cast;
    forward.m_flipped = true;
    forward.m_predictive = true;
    dispatcher->register_collision_agent(forward, PHYS_SHAPE_BVH_TREE, PHYS_SHAPE_CONVEX);

    phys_dispatcher::agent_funcs backward;
    backward.m_create_fn = phys_bvh_stream_agent::create_shape_bounding_agent;
    backward.m_penetrations_fn = phys_bvh_agent::static_penetrations;
    backward.m_closest_points_fn = phys_bvh_agent::static_closest_points;
    backward.m_linear_cast_func = phys_bvh_tree_agent::static_sweep_cast;
    backward.m_flipped = false;
    backward.m_predictive = true;
    dispatcher->register_collision_agent(backward, PHYS_SHAPE_CONVEX, PHYS_SHAPE_BVH_TREE);

    phys_dispatcher::agent_funcs both_cmp;
    both_cmp.m_create_fn = phys_bvh_tree_agent::create_bvh_bvh_agent;
    both_cmp.m_penetrations_fn = phys_bvh_agent::static_penetrations;
    both_cmp.m_closest_points_fn = phys_bvh_agent::static_closest_points;
    both_cmp.m_linear_cast_func = phys_bvh_tree_agent::static_sweep_cast;
    both_cmp.m_flipped = false;
    both_cmp.m_predictive = true;
    dispatcher->register_collision_agent(both_cmp, PHYS_SHAPE_BVH_TREE, PHYS_SHAPE_BVH_TREE);
}

