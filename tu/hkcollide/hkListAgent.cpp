// hkcollide/hkListAgent.obj: from the records 0x6342e8 0x634340 0x634348 0x634350 0x6343b8 0x634408 0x634410 0x634460 0x6344a8 0x6344c0 0x634508 0x634570 0x634760 0x6347e0 0x634870 0x634940 0x6349a8 0x634a00 0x634a60 0x634b10
// @flags /TP /O2
#include "phys/phys_base_types.h"
#include <vectorintrinsics.h>
#include "phys/phys_symmetric_agent_linear_cast.h"
#include "phys/phys_agent.h"
#include "phys/phys_compound_shape_agent.h"
#include "phys/phys_dispatcher.h"
#include "phys/phys_list_agent.h"
#include "phys/phys_local_buffer.h"
#include "phys/phys_batch_agent_machine.h"
#include "phys/phys_batch_agent_visitor_input.h"
#include "phys/phys_symmetric_agent.h"

#include "phys/phys_thread_memory.h"

namespace {
}


#include "phys/phys_collision_input.h"
#include "phys/phys_collision_proxy.h"
#include "phys/phys_collide.h"
#include "phys/phys_constraint_owner.h"
#include "phys/phys_math.h"
#include "phys/phys_statistics_collector.h"




// The 1n-machine's list of per-child agents (12 bytes).
extern "C" {
}


// Vectors, contact points and the process-collision output (for processCollision).






// @address 0x6342e8 ?cleanup@phys_list_agent@@UAAXAAVphys_constraint_owner@@@Z
void phys_list_agent::cleanup(phys_constraint_owner &owner)
{
    phys_batch_agent_destroy(m_agent_track, m_agent_dispatcher, m_contact_manager, owner);
    delete this;
}
// @address 0x634340 ?invalidate_sweep_info@phys_list_agent@@UAAXAAUphys_collision_input@@@Z
void phys_list_agent::invalidate_sweep_info(phys_collision_input &input)
{
    phys_batch_agent_invalidate_sweep(&m_agent_track, &input);
}
// @address 0x634348 ?warp_time@phys_list_agent@@UAAXMMAAUphys_collision_input@@@Z
void phys_list_agent::warp_time(float agent_old_time, float agent_new_time, phys_collision_input &input)
{
    phys_batch_agent_warp_time(&m_agent_track, agent_old_time, agent_new_time, &input);
}
// @address 0x634350 ?update_compound_filter@phys_list_agent@@UAAXABVphys_collision_proxy@@0ABUphys_collision_input@@AAVphys_constraint_owner@@@Z
void phys_list_agent::update_compound_filter(const phys_collision_proxy &agent_body_a, const phys_collision_proxy &agent_body_b,
    const phys_collision_input &input, phys_constraint_owner &owner)
{
    phys_batch_agent_visitor_input list_visitor_input;
    list_visitor_input.m_body_a = &agent_body_a;
    list_visitor_input.m_compound_proxy_b = &agent_body_b;
    list_visitor_input.m_input = &input;
    list_visitor_input.m_contact_manager = m_contact_manager;
    list_visitor_input.m_constraint_owner = &owner;
    list_visitor_input.m_container_shape_b = agent_body_b.get_shape()->get_shape_container();
    phys_batch_agent_update_compound_filter(m_agent_track, list_visitor_input);
}
namespace {
static __forceinline void closest_points_flipped(
    const phys_collision_proxy &agent_body_a, const phys_collision_proxy &agent_body_b, const phys_collision_input &input, phys_hit_collector &collector)
{
    phys_symmetric_flip_collector flipped_collector(collector);
    phys_compound_shape_agent::static_closest_points(agent_body_b, agent_body_a, input, flipped_collector);
}
}
// @address 0x6343b8 ?static_closest_points@phys_list_agent@@SAXABVphys_collision_proxy@@0ABUphys_collision_input@@AAVphys_hit_collector@@@Z
void phys_list_agent::static_closest_points(const phys_collision_proxy &agent_body_a, const phys_collision_proxy &agent_body_b,
    const phys_collision_input &input, phys_hit_collector &collector)
{
    closest_points_flipped(agent_body_a, agent_body_b, input, collector);
}
// @address 0x634408 ?static_sweep_cast@phys_list_agent@@SAXABVphys_collision_proxy@@0ABUphys_linear_cast_collision_input@@AAVphys_hit_collector@@PAV4@@Z
void phys_list_agent::static_sweep_cast(const phys_collision_proxy &agent_body_a, const phys_collision_proxy &agent_body_b,
    const phys_linear_cast_collision_input &input,
    phys_hit_collector &collector, phys_hit_collector *start_collector)
{
    phys_symmetric_agent_linear_cast<phys_compound_shape_agent>::static_sweep_cast(agent_body_a, agent_body_b, input, collector, start_collector);
}
// @address 0x634410 ?static_penetrations@phys_list_agent@@SAXABVphys_collision_proxy@@0ABUphys_collision_input@@AAVphys_proxy_pair_collector@@@Z
void phys_list_agent::static_penetrations(const phys_collision_proxy &agent_body_a, const phys_collision_proxy &agent_body_b,
    const phys_collision_input &input, phys_proxy_pair_collector &collector)
{
    phys_symmetric_flip_pair_collector flipped_collector(collector);
    phys_compound_shape_agent::static_penetrations(agent_body_b, agent_body_a, input, flipped_collector);
}
// @address 0x634460 ?get_closest_points@phys_list_agent@@UAAXABVphys_collision_proxy@@0ABUphys_collision_input@@AAVphys_hit_collector@@@Z
void phys_list_agent::get_closest_points(const phys_collision_proxy &agent_body_a, const phys_collision_proxy &agent_body_b,
    const phys_collision_input &input, phys_hit_collector &collector)
{
    static_closest_points(agent_body_a, agent_body_b, input, collector);
}
// @address 0x6344a8 ?sweep_cast@phys_list_agent@@UAAXABVphys_collision_proxy@@0ABUphys_linear_cast_collision_input@@AAVphys_hit_collector@@PAV4@@Z
void phys_list_agent::sweep_cast(const phys_collision_proxy &agent_body_a, const phys_collision_proxy &agent_body_b,
    const phys_linear_cast_collision_input &input,
    phys_hit_collector &collector, phys_hit_collector *start_collector)
{
    static_sweep_cast(agent_body_a, agent_body_b, input, collector, start_collector);
}
// @address 0x6344c0 ?get_penetrations@phys_list_agent@@UAAXABVphys_collision_proxy@@0ABUphys_collision_input@@AAVphys_proxy_pair_collector@@@Z
void phys_list_agent::get_penetrations(const phys_collision_proxy &agent_body_a, const phys_collision_proxy &agent_body_b,
    const phys_collision_input &input, phys_proxy_pair_collector &collector)
{
    static_penetrations(agent_body_a, agent_body_b, input, collector);
}
// @address 0x634508 ??0phys_list_agent@@QAA@ABVphys_collision_proxy@@0ABUphys_collision_input@@PAVphys_contact_manager@@@Z
phys_list_agent::phys_list_agent(const phys_collision_proxy &agent_body_a, const phys_collision_proxy &agent_body_b,
    const phys_collision_input &input, phys_contact_manager *agent_contact_manager)
    : phys_agent(agent_contact_manager)
{
    m_agent_dispatcher = input.m_agent_dispatcher;
    phys_batch_agent_create(&m_agent_track);
}
// @address 0x634760 ?create_list_b_agent@phys_list_agent@@SAPAVphys_agent@@ABVphys_collision_proxy@@0ABUphys_collision_input@@PAVphys_contact_manager@@@Z
phys_agent *phys_list_agent::create_list_b_agent(const phys_collision_proxy &agent_body_a, const phys_collision_proxy &agent_body_b,
    const phys_collision_input &input, phys_contact_manager *agent_contact_manager)
{
    return new phys_list_agent(agent_body_a, agent_body_b, input, agent_contact_manager);
}
// @address 0x6347e0 ?create_list_a_agent@phys_list_agent@@SAPAVphys_agent@@ABVphys_collision_proxy@@0ABUphys_collision_input@@PAVphys_contact_manager@@@Z
phys_agent *phys_list_agent::create_list_a_agent(const phys_collision_proxy &agent_body_a, const phys_collision_proxy &agent_body_b,
    const phys_collision_input &input, phys_contact_manager *agent_contact_manager)
{
    return new phys_symmetric_agent<phys_list_agent>(agent_body_a, agent_body_b, input, agent_contact_manager);
}

// @address 0x634870 ?register_agent@phys_list_agent@@SAXPAVphys_dispatcher@@@Z
void phys_list_agent::register_agent(phys_dispatcher *dispatcher)
{
  phys_dispatcher::agent_funcs forward;

  forward.m_create_fn = phys_list_agent::create_list_a_agent;
  forward.m_penetrations_fn = phys_compound_shape_agent::static_penetrations;
  forward.m_closest_points_fn = phys_compound_shape_agent::static_closest_points;
  forward.m_linear_cast_func = phys_compound_shape_agent::static_sweep_cast;
  forward.m_flipped = true;
  forward.m_predictive = true;
  dispatcher->register_collision_agent(forward, PHYS_SHAPE_LIST, PHYS_SHAPE_ALL);

  phys_dispatcher::agent_funcs backward;
  backward.m_create_fn = phys_list_agent::create_list_b_agent;
  backward.m_penetrations_fn = phys_symmetric_agent_linear_cast<phys_compound_shape_agent>::static_penetrations;
  backward.m_closest_points_fn = phys_symmetric_agent_linear_cast<phys_compound_shape_agent>::static_closest_points;
  backward.m_linear_cast_func = phys_symmetric_agent_linear_cast<phys_compound_shape_agent>::static_sweep_cast;
  backward.m_flipped = false;
  backward.m_predictive = true;
  dispatcher->register_collision_agent(backward, PHYS_SHAPE_ALL, PHYS_SHAPE_LIST);
}
// @address 0x634940 ?update_compound_filter@?$phys_symmetric_agent_linear_cast@Vphys_list_agent@@@@UAAXABVphys_collision_proxy@@0ABUphys_collision_input@@AAVphys_constraint_owner@@@Z
template<>
void phys_symmetric_agent_linear_cast<phys_list_agent>::update_compound_filter(
    const phys_collision_proxy &agent_body_a, const phys_collision_proxy &agent_body_b, const phys_collision_input &input, phys_constraint_owner &owner)
{
    phys_list_agent::update_compound_filter(agent_body_b, agent_body_a, input, owner);
}
// @address 0x6349a8 ?get_penetrations@?$phys_symmetric_agent_linear_cast@Vphys_list_agent@@@@UAAXABVphys_collision_proxy@@0ABUphys_collision_input@@AAVphys_proxy_pair_collector@@@Z
template<>
void phys_symmetric_agent_linear_cast<phys_list_agent>::get_penetrations(
    const phys_collision_proxy &agent_body_a, const phys_collision_proxy &agent_body_b, const phys_collision_input &input, phys_proxy_pair_collector &collector)
{
    phys_symmetric_flip_pair_collector flipped_collector(collector);
    phys_list_agent::get_penetrations(agent_body_b, agent_body_a, input, flipped_collector);
}
// @address 0x634a00 ?get_closest_points@?$phys_symmetric_agent_linear_cast@Vphys_list_agent@@@@UAAXABVphys_collision_proxy@@0ABUphys_collision_input@@AAVphys_hit_collector@@@Z
template<>
void phys_symmetric_agent_linear_cast<phys_list_agent>::get_closest_points(
    const phys_collision_proxy &agent_body_a, const phys_collision_proxy &agent_body_b, const phys_collision_input &input, phys_hit_collector &collector)
{
    phys_symmetric_flip_collector flipped_collector(collector);
    phys_list_agent::get_closest_points(agent_body_b, agent_body_a, input, flipped_collector);
}
// @address 0x634a60 ?process_collision@?$phys_symmetric_agent_linear_cast@Vphys_list_agent@@@@UAAXABVphys_collision_proxy@@0ABUphys_process_collision_input@@AAUphys_process_collision_output@@@Z
template<>
void phys_symmetric_agent_linear_cast<phys_list_agent>::process_collision(
    const phys_collision_proxy &agent_body_a, const phys_collision_proxy &agent_body_b, const phys_process_collision_input &input,
        phys_process_collision_output &output)
{
    phys_process_hit_point *point = output.m_next_free_contact;
    float old_toi = output.m_collision_toi.m_time;
    phys_list_agent::process_collision(agent_body_b, agent_body_a, input, output);
    for (; point < output.m_next_free_contact; ++point) {
        point->m_hit_contact.set_flipped_contact(point->m_hit_contact);
    }
    if (old_toi != output.m_collision_toi.m_time) {
        output.m_collision_toi.flip();
    }
}
// @address 0x634b10 ?sweep_cast@?$phys_symmetric_agent@Vphys_list_agent@@@@UAAXABVphys_collision_proxy@@0ABUphys_linear_cast_collision_input@@AAVphys_hit_collector@@PAV4@@Z
template<>
void phys_symmetric_agent<phys_list_agent>::sweep_cast(const phys_collision_proxy &agent_body_a,
    const phys_collision_proxy &agent_body_b, const phys_linear_cast_collision_input &input,
    phys_hit_collector &collector, phys_hit_collector *start_collector)
{
    phys_linear_cast_collision_input flipped_input = input;
    flipped_input.m_path.set_neg4(input.m_path);
    phys_symmetric_flip_cast_collector flip(input.m_path, collector);
    if (start_collector) {
        phys_symmetric_flip_cast_collector start_flip_collector(input.m_path, *start_collector);
        phys_symmetric_agent_linear_cast<phys_compound_shape_agent>::static_sweep_cast(agent_body_b, agent_body_a, flipped_input, flip, &start_flip_collector);
    }
    else {
        phys_linear_cast_collision_input flipped_input2 = flipped_input;
        flipped_input2.m_path.set_neg4(flipped_input.m_path);
        phys_symmetric_flip_cast_collector flip_collector2(flipped_input.m_path, flip);
        phys_compound_shape_agent::static_sweep_cast(agent_body_a, agent_body_b, flipped_input2, flip_collector2, 0);
    }
}

void phys_list_agent::process_collision(class phys_collision_proxy const &agent_body_a, class phys_collision_proxy const &agent_body_b,
    struct phys_process_collision_input const &input,
    struct phys_process_collision_output &output)
{
    const phys_motion_state *motion_a_state = (const phys_motion_state *)agent_body_a.m_motion;
    const phys_motion_state *motion_b_state = (const phys_motion_state *)agent_body_b.m_motion;
    const phys_list_shape *penetration_shape_b = (const phys_list_shape *)agent_body_b.m_proxy_shape;
    phys_narrow_agent_process_input narrow_input;
    narrow_input.m_body_a = &agent_body_a;
    narrow_input.m_body_b = &agent_body_b;
    narrow_input.m_input = &input;
    narrow_input.m_contact_manager = m_contact_manager;

    const phys_swept_transform &swept_a = motion_a_state->m_swept_transform;
    const phys_swept_transform &swept_b = motion_b_state->m_swept_transform;
    float dt = input.m_step_info.m_delta_time;
    float swept_frac_a = dt * swept_a.m_center_of_mass1.m_simd_quad.w;
    float swept_frac_b = dt * swept_b.m_center_of_mass1.m_simd_quad.w;
    narrow_input.m_lin_sweep_info.m_simd_quad = __vmaddfp(
        __vsubfp(swept_b.m_center_of_mass1.m_simd_quad, swept_b.m_center_of_mass0.m_simd_quad),
        __vspltw(__lvlx(&swept_frac_b, 0), 0),
        __vmulfp(__vsubfp(swept_a.m_center_of_mass0.m_simd_quad, swept_a.m_center_of_mass1.m_simd_quad), __vspltw(__lvlx(&swept_frac_a, 0), 0)));
    narrow_input.m_lin_sweep_info.m_simd_quad.w =
        swept_frac_a * (motion_a_state->m_delta_angle.m_simd_quad.w * motion_a_state->m_object_radius) +
        swept_frac_b * (motion_b_state->m_delta_angle.m_simd_quad.w * motion_b_state->m_object_radius);
    narrow_input.m_a_to_b.set_mul_inverse_mul(motion_a_state->get_transform(), motion_b_state->get_transform());

    int size = penetration_shape_b->m_child_info.getSize();

    phys_local_buffer<unsigned int> hit_key_list(size + 1);
    for (int i = 0; i < size; i++) hit_key_list[i] = i;
    hit_key_list[size] = 0xffffffffu;

    phys_batch_agent_process(m_agent_track, 0, narrow_input, static_cast<const phys_shape_container *>(penetration_shape_b),
        hit_key_list.begin(), size, output);

}
// @address 0x634570 ?process_collision@phys_list_agent@@UAAXABVphys_collision_proxy@@0ABUphys_process_collision_input@@AAUphys_process_collision_output@@@Z
