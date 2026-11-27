#ifndef INCLUDE_PHYS_NULL_AGENT_H
#define INCLUDE_PHYS_NULL_AGENT_H
#include "phys/phys_agent.h"
class phys_null_agent : public phys_agent
{
	public:

		phys_null_agent();

		virtual void get_penetrations( const phys_collision_proxy& agent_body_a, const phys_collision_proxy& agent_body_b,
			const phys_collision_input& input, phys_proxy_pair_collector& collector ) { }

		virtual void get_closest_points( const phys_collision_proxy& agent_body_a, const phys_collision_proxy& agent_body_b,
			const phys_collision_input& input, phys_hit_collector& point_details) { }

		static void PHYS_CALL static_closest_points( const phys_collision_proxy& agent_body_a, const phys_collision_proxy& agent_body_b,
			const phys_collision_input& input, class phys_hit_collector& collector  );

		static void PHYS_CALL static_penetrations(const  phys_collision_proxy& agent_body_a, const phys_collision_proxy& agent_body_b,
			const phys_collision_input& input, phys_proxy_pair_collector& collector );

		virtual void sweep_cast( const phys_collision_proxy& agent_body_a, const phys_collision_proxy& agent_body_b,
			const phys_linear_cast_collision_input& input, phys_hit_collector& collector, phys_hit_collector* start_collector ){ }

		static void PHYS_CALL static_sweep_cast( const phys_collision_proxy& agent_body_a, const phys_collision_proxy& agent_body_b,
			const phys_linear_cast_collision_input& input, phys_hit_collector& collector, phys_hit_collector* start_collector );

		virtual void process_collision(const phys_collision_proxy& agent_body_a, const phys_collision_proxy& agent_body_b,
			const phys_process_collision_input& input, phys_process_collision_output& result){}
	
		virtual void cleanup(phys_constraint_owner& info){}

		static phys_agent* PHYS_CALL create_null_agent( const phys_collision_proxy& agent_body_a,
			const phys_collision_proxy& agent_body_b, const phys_collision_input& input, phys_contact_manager* pair_contact_manager );

		static phys_null_agent* PHYS_CALL get_null_agent();

	private:

		static phys_null_agent s_nullAgent;

};

#endif // INCLUDE_HAVOK_HK_NULL_AGENT_H
