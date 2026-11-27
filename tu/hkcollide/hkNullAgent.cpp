// hkcollide/hkNullAgent.obj: from the records 0x6275a8 0x6275b0 0x6275b8 0x6275c0 0x6275d0 0x6275e0
// @compiler xdk6130
// @flags /TP /O2 /Oc /GF /EHs-c- /GR-
// @address 0x6275a8 ?static_closest_points@phys_null_agent@@SAXABVphys_collision_proxy@@0ABUphys_collision_input@@AAVphys_hit_collector@@@Z
// @address 0x6275b0 ?static_penetrations@phys_null_agent@@SAXABVphys_collision_proxy@@0ABUphys_collision_input@@AAVphys_proxy_pair_collector@@@Z
// @address 0x6275b8 ?static_sweep_cast@phys_null_agent@@SAXABVphys_collision_proxy@@0ABUphys_linear_cast_collision_input@@AAVphys_hit_collector@@PAV4@@Z
// @address 0x6275c0 ?create_null_agent@phys_null_agent@@SAPAVphys_agent@@ABVphys_collision_proxy@@0ABUphys_collision_input@@PAVphys_contact_manager@@@Z
// @address 0x6275d0 ?get_null_agent@phys_null_agent@@SAPAV1@XZ
// @address 0x6275e0 ??0phys_null_agent@@QAA@XZ
#define PHYS_CONFIG_SIMD 1
#define PHYS_LEGACY_COMPAT 1
#include "physics/havok_memory.h"
#include "phys/phys_pair_agent_machine.h"
#include "phys/phys_solver.h"

#include "phys/phys_null_agent.h"

 phys_null_agent::phys_null_agent()
 :	phys_agent( 0 )
 {
 }

 phys_null_agent phys_null_agent::s_nullAgent;

 void PHYS_CALL phys_null_agent::static_closest_points(
     const phys_collision_proxy &agent_body_a, const phys_collision_proxy &agent_body_b, const phys_collision_input &input, class phys_hit_collector &collector)
 {
 }

 void PHYS_CALL phys_null_agent::static_penetrations(
     const phys_collision_proxy &agent_body_a, const phys_collision_proxy &agent_body_b, const phys_collision_input &input,
         phys_proxy_pair_collector &collector)
 {
 }

 void PHYS_CALL phys_null_agent::static_sweep_cast(const phys_collision_proxy &agent_body_a, const phys_collision_proxy &agent_body_b,
     const phys_linear_cast_collision_input &input,
     phys_hit_collector &collector, phys_hit_collector *start_collector)
 {
 }

 phys_agent* PHYS_CALL phys_null_agent::create_null_agent(const phys_collision_proxy& agent_body_a, const phys_collision_proxy& agent_body_b, 
                                                const phys_collision_input& input, phys_contact_manager* pair_contact_manager )
 {
    return &s_nullAgent;
 }

 phys_null_agent* PHYS_CALL phys_null_agent::get_null_agent()
 {
    return &s_nullAgent;
 }
