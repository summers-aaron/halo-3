// hkcollide/hkMoppAgent.obj: from the records 0x637128 0x637168 0x637288 0x637348 0x637458 0x6374a8 0x6374f8 0x6375e8
// @compiler xdk6130
// @flags /TP /O2 /Oc
// @address 0x637128 ??0phys_bvh_tree_agent@@IAA@PAVphys_contact_manager@@@Z
// @address 0x637348 ?register_agent@phys_bvh_tree_agent@@SAXPAVphys_dispatcher@@@Z
// @address 0x637458 ?get_penetrations@?$phys_symmetric_agent_linear_cast@Vphys_bvh_tree_agent@@@@UAAXABVphys_collision_proxy@@0ABUphys_collision_input@@AAVphys_proxy_pair_collector@@@Z
// @address 0x6374a8 ?get_closest_points@?$phys_symmetric_agent_linear_cast@Vphys_bvh_tree_agent@@@@UAAXABVphys_collision_proxy@@0ABUphys_collision_input@@AAVphys_hit_collector@@@Z
// @address 0x6374f8 ?sweep_cast@?$phys_symmetric_agent@Vphys_bvh_tree_agent@@@@UAAXABVphys_collision_proxy@@0ABUphys_linear_cast_collision_input@@AAVphys_hit_collector@@PAV4@@Z
// @address 0x6375e8 ??_Gphys_bvh_tree_agent@@UAAPAXI@Z
#include "phys/phys_base.h"
#include "phys/phys_collectors.h"
#include <vectorintrinsics.h>
#include "phys/phys_collide.h"
#include "phys/phys_agent.h"
#include "phys/phys_symmetric_agent_linear_cast.h"

class phys_collision_constraint_owner;
#include "phys/phys_bvh_agent.h"
#include "phys/phys_dispatcher.h"
#include "phys/phys_bvh_tree_agent.h"
#include "phys/phys_symmetric_agent.h"
#include "phys/phys_bvh_aabb_cast_vm.h"
#include "phys/phys_collision_proxy.h"
#include "phys/phys_constraint_owner.h"
#include "phys/phys_math.h"
#include "phys/phys_statistics_collector.h"
#include "phys/phys_collision_input.h"
phys_bvh_tree_agent::phys_bvh_tree_agent(phys_contact_manager *agent_contact_manager) : phys_bvh_agent(agent_contact_manager) {}
void phys_bvh_tree_agent::register_agent(phys_dispatcher *dispatcher)
{
  phys_dispatcher::agent_funcs forward;
  forward.m_create_fn = phys_bvh_agent::create_bvh_shape_agent;
  forward.m_penetrations_fn = phys_symmetric_agent_linear_cast<phys_bvh_agent>::static_penetrations;
  forward.m_closest_points_fn = phys_symmetric_agent_linear_cast<phys_bvh_agent>::static_closest_points;
  forward.m_linear_cast_func = phys_symmetric_agent_linear_cast<phys_bvh_tree_agent>::static_sweep_cast;
  forward.m_flipped = true;
  forward.m_predictive = false;
  dispatcher->register_collision_agent(forward, (phys_shape_type)27, (phys_shape_type)-1);

  phys_dispatcher::agent_funcs backward;
  backward.m_create_fn = phys_bvh_agent::create_shape_bounding_agent;
  backward.m_penetrations_fn = phys_bvh_agent::static_penetrations;
  backward.m_closest_points_fn = phys_bvh_agent::static_closest_points;
  backward.m_linear_cast_func = phys_bvh_tree_agent::static_sweep_cast;
  backward.m_flipped = false;
  backward.m_predictive = false;
  dispatcher->register_collision_agent(backward, (phys_shape_type)-1, (phys_shape_type)27);

  phys_dispatcher::agent_funcs both_cmp;
  both_cmp.m_create_fn = phys_bvh_tree_agent::create_bvh_bvh_agent;
  both_cmp.m_penetrations_fn = phys_bvh_agent::static_penetrations;
  both_cmp.m_closest_points_fn = phys_bvh_agent::static_closest_points;
  both_cmp.m_linear_cast_func = phys_bvh_tree_agent::static_sweep_cast;
  both_cmp.m_flipped = false;
  both_cmp.m_predictive = true;
  dispatcher->register_collision_agent(both_cmp, (phys_shape_type)27, (phys_shape_type)27);
}
template <>
void phys_symmetric_agent_linear_cast<phys_bvh_tree_agent>::get_penetrations(
    const phys_collision_proxy &agent_body_a, const phys_collision_proxy &agent_body_b, const phys_collision_input &input, phys_proxy_pair_collector &collector)
{
  phys_symmetric_flip_pair_collector flipped_collector(collector);
  phys_bvh_agent::get_penetrations(agent_body_b, agent_body_a, input, flipped_collector);
}





template <>
void phys_symmetric_agent_linear_cast<phys_bvh_tree_agent>::get_closest_points(
    const phys_collision_proxy &agent_body_a, const phys_collision_proxy &agent_body_b, const phys_collision_input &input, phys_hit_collector &collector)
{
  phys_symmetric_flip_collector flipped_collector(collector);
  phys_bvh_agent::get_closest_points(agent_body_b, agent_body_a, input, flipped_collector);
}

template <>
void phys_symmetric_agent<phys_bvh_tree_agent>::sweep_cast(const phys_collision_proxy &agent_body_a,
    const phys_collision_proxy &agent_body_b, const phys_linear_cast_collision_input &input,
    phys_hit_collector &collector, phys_hit_collector *start_collector)
{
  phys_linear_cast_collision_input flipped_input = input;
  flipped_input.m_path.set_neg4(input.m_path);
  phys_symmetric_flip_cast_collector flipped_collector(input.m_path, collector);
  if (start_collector) {
    phys_symmetric_flip_cast_collector flipped_start_collector(input.m_path, *start_collector);
    phys_bvh_agent::sweep_cast(agent_body_b, agent_body_a, flipped_input, flipped_collector, &flipped_start_collector);
  } else {
    phys_bvh_agent::sweep_cast(agent_body_b, agent_body_a, flipped_input, flipped_collector, 0);
  }
}
// @address 0x1624c k_hk_mopp_agent_vtable
// @address 0x16284 k_hk_symmetric_mopp_agent_vtable
// @address 0x637288 ?create_bvh_bvh_agent@phys_bvh_tree_agent@@SAPAVphys_agent@@ABVphys_collision_proxy@@0ABUphys_collision_input@@PAVphys_contact_manager@@@Z
phys_agent *phys_bvh_tree_agent::create_bvh_bvh_agent(const phys_collision_proxy &agent_body_a,
    const phys_collision_proxy &agent_body_b, const phys_collision_input &input, phys_contact_manager *pair_contact_manager)
{
  int code_size_a = static_cast<const phys_bvh_tree_shape *>(agent_body_a.get_shape())->get_bvh_bytecode()->get_code_size();
  int code_size_b = static_cast<const phys_bvh_tree_shape *>(agent_body_b.get_shape())->get_bvh_bytecode()->get_code_size();
  if (code_size_a < code_size_b)
  {
    return new phys_bvh_tree_agent(pair_contact_manager);
  }
  else
  {
    return new phys_symmetric_agent<phys_bvh_tree_agent>(pair_contact_manager);
  }
}
// @address 0x637168 ?static_sweep_cast@phys_bvh_tree_agent@@SAXABVphys_collision_proxy@@0ABUphys_linear_cast_collision_input@@AAVphys_hit_collector@@PAV4@@Z
void phys_bvh_tree_agent::static_sweep_cast(const phys_collision_proxy &agent_body_a, const phys_collision_proxy &agent_body_b,
    const phys_linear_cast_collision_input &input,
    phys_hit_collector &collector, phys_hit_collector *start_collector)
{
  phys_aabb aabb;
  {
    phys_transform b_from_a;
    b_from_a.set_mul_inverse_mul(agent_body_b.get_transform(), agent_body_a.get_transform());
    agent_body_a.get_shape()->get_handle_aabb(b_from_a, input.m_box_tolerance, aabb);
  }
  phys_vec4 bvh_path_b;
  bvh_path_b.vec_set_rotated_inverse_dir(agent_body_b.get_transform().get_rotation(), input.m_path);
  phys_bvh_aabb_cast_vm::phys_bvh_aabb_cast_input ai;
  ai.m_cast_proxy = &agent_body_a;
  ai.m_bvh_proxy = &agent_body_b;
  ai.m_ray_from.set_lerp4(aabb.m_min_limit, aabb.m_max_limit, 0.5f);
  ai.m_ray_to.set_add4(ai.m_ray_from, bvh_path_b);
  ai.m_cast_extents.set_sub4(aabb.m_max_limit, aabb.m_min_limit);
  ai.m_cast_extents.mul4(0.5f);
  phys_vec4 tol_vec4;
  tol_vec4.flags_set_all(input.m_box_tolerance);
  ai.m_cast_extents.add4(tol_vec4);
  ai.m_collision_input = &input;
  phys_bvh_aabb_cast_vm machine;
  machine.trace_aabb_cast(ai, collector, start_collector);
}

