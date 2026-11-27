#ifndef INCLUDE_PHYS_SIMULATION_H
#define INCLUDE_PHYS_SIMULATION_H
#include "phys/phys_base.h"
class phys_world;
class phys_entity;
class phys_island;
class phys_step_info;
class phys_constraint_query_in;
struct phys_world_dynamics_step_info;
struct phys_pair_agent_entry;
struct phys_process_collision_input;
struct phys_process_collision_output;
enum phys_step_result {
    PHYS_STEP_RESULT_SUCCESS = 0,
    PHYS_STEP_RESULT_MEMORY_FAILURE_BEFORE_INTEGRATION = 1,
    PHYS_STEP_RESULT_MEMORY_FAILURE_DURING_COLLIDE = 2,
    PHYS_STEP_RESULT_MEMORY_FAILURE_DURING_TOI_SOLVE = 3
};
class phys_simulation : public phys_ref_object {
    friend class phys_world;
public:
    PHYS_DECLARE_ALLOCATOR(PHYS_MEM_BASE_CLASS);
    enum find_contacts_mode { CONTACT_SEARCH_DEFAULT = 0, CONTACT_SEARCH_EXTRA = 1 };
    phys_simulation(phys_world *world);
    virtual ~phys_simulation();
    virtual phys_step_result step_delta_time(float body_delta_time);
    virtual phys_step_result integrate_world(float body_delta_time);
    virtual phys_step_result collide();
    virtual phys_step_result advance_time();
    virtual void collide_entities_discrete(phys_entity **entities, int count, phys_world *world,
        const phys_step_info &step_info, find_contacts_mode find_contacts_fn);
    virtual void reset_collision_info_for_entities(phys_entity **entities, int count, phys_world *world, phys_bool skip_reinit);
    virtual void assert_no_entity_collision_info(phys_entity **entities, int count, phys_world *world) {}
    virtual void remove_agent_collision_info(phys_pair_agent_entry *pair_agent_entry) {}
    virtual void assert_no_agent_collision_info(phys_pair_agent_entry *pair_agent_entry) {}
    virtual void reintegrate_recollide(phys_entity **entities, int count, phys_world *world);
    __forceinline float get_current_time() { return m_sim_cur_time; }
    __forceinline float get_current_psi_time() { return m_current_psi_time; }
    __forceinline float get_simulate_until_time() { return m_simulate_until_time; }
    __forceinline void set_current_time(float time) { m_sim_cur_time = time; }
    __forceinline void set_current_psi_time(float time) { m_current_psi_time = time; }
    __forceinline void set_simulate_until_time(float time) { m_simulate_until_time = time; }
    __forceinline phys_world *get_action_world() { return m_world; }
    bool sim_at_marker();
    bool sim_at_psi() const;
    void set_frame_time_marker(float frame_delta_time);
    static void collide_entities_broadphase_discrete(phys_entity **entities, int count, phys_world *world);
    void collide_entities_narrowphase_discrete(phys_entity **entities, int count, const phys_process_collision_input &input,
        find_contacts_mode find_contacts_fn);
protected:
    virtual void collide_internal(const phys_step_info &step_info);
public:
    virtual void warp_time(float body_delta_time);
protected:
    void validate_delta_time(float body_delta_time);
    phys_step_result recollide_after_step_fail();
    void integrate_internal(const phys_step_info &step_info);
    void apply_actions();
    static void integrate_island(phys_island *island, const phys_world_dynamics_step_info &step_info, phys_constraint_query_in &solver_query_in);
    static void collide_island_narrowphase_discrete(phys_island *island, const phys_process_collision_input &input);
    float snap_sim_time_to_advance();
    void collide_agent_discrete(phys_pair_agent_entry *entry, const phys_process_collision_input &input, phys_process_collision_output &output);
    void reset_agent_collision_info(phys_pair_agent_entry *entry, const phys_process_collision_input &input, phys_process_collision_output &output);
    void process_entity_agents(phys_entity **entities, int count, const phys_process_collision_input &input,
        void (phys_simulation::*process)(phys_pair_agent_entry *, const phys_process_collision_input &,
            phys_process_collision_output &), find_contacts_mode find_contacts_fn);
    phys_world *m_world;
    int m_frame_counter;
    float m_sim_cur_time;
    float m_current_psi_time;
    float m_physics_dt;
    float m_simulate_until_time;
    float m_frame_marker_psi_snap;
    phys_step_result m_prev_step_result;
};
typedef char phys_simulation_size[(sizeof(void *) != 4 || ((sizeof(phys_simulation) == 40) )) ? 1 : -1];
#endif
