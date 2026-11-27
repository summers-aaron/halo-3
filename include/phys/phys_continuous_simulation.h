#ifndef INCLUDE_PHYS_CONTINUOUS_SIMULATION_H
#define INCLUDE_PHYS_CONTINUOUS_SIMULATION_H

#include "phys/phys_simulation.h"
#include "phys/phys_thread_memory.h"
template <typename T> class phys_fixed_array;

struct phys_toi_event;
struct phys_toi_resources;
struct phys_solver_resources;
class phys_toi_resource_manager;
class phys_default_toi_resource_manager;

class phys_continuous_simulation : public phys_simulation {
public:
    phys_continuous_simulation(phys_world *world);
    virtual ~phys_continuous_simulation();
    virtual phys_step_result advance_time();
    void reset_collision_info_for_entities(phys_entity **entities, int count, phys_world *world, phys_bool skip_reinit);
    virtual void assert_no_entity_collision_info(phys_entity **entities, int count, phys_world *world);
    virtual void remove_agent_collision_info(phys_pair_agent_entry *pair_agent_entry);
    virtual void assert_no_agent_collision_info(phys_pair_agent_entry *pair_agent_entry);
    virtual void reintegrate_recollide(phys_entity **entities, int count, phys_world *world);
protected:
    phys_step_result advance_time_internal();
    phys_step_result handle_all_tois_till(float time);
    virtual void collide_internal(const phys_step_info &step_info);
public:
    virtual void warp_time(float time);
protected:
    virtual void simulate_toi(phys_world *world, phys_toi_event &event, float physics_delta_time);
public:
    void collide_entities_broadphase_ccd(phys_entity **entities, int count, phys_world *world, signed char *export_finished);
    void collide_entities_narrowphase_ccd(phys_entity **entities, int count, const phys_process_collision_input &input);
    void add_toi_event(const phys_process_collision_output &output, const phys_pair_agent_entry &pair_agent_entry);
    static __forceinline void operator delete(void *p)
    {
        phys_ref_object *object = static_cast<phys_ref_object *>(p);
        phys_thread_memory *memory = static_cast<phys_thread_memory *>(TlsGetValue(phys_thread_memory::s_threadMemoryInstance.m_slot));
        memory->free_chunk(p, object->m_mem_size_and_flags, PHYS_MEM_BASE_CLASS);
    }
protected:
    void remove_toi_events_of_entities(phys_entity **entities, int count);
    void remove_toi_events_of_entity(phys_entity *entity);
    __forceinline void notify_toi_event_removed(phys_toi_event &event);
    __forceinline void delete_island_toi_events(
        const phys_island *island, const phys_fixed_array<unsigned char> &entity_state, phys_array<phys_toi_event> &toi_events);
    __forceinline void collide_island_entities_narrowphase_ccd(phys_entity **entities, int solver_entity_count, const phys_process_collision_input &input);
    __forceinline void collide_island_entities_ccd(phys_entity **entities, int solver_entity_count, phys_world *world,
        const phys_process_collision_input &input);
    void collide_agent_continuous(phys_pair_agent_entry *pair_agent_entry, const phys_process_collision_input &input, phys_process_collision_output &output);
public:
  void collide_island_narrowphase_ccd(class phys_island *p0, struct phys_process_collision_input const &p1);
    __declspec(align(16)) phys_array<phys_toi_event> m_toi_events;
    phys_default_toi_resource_manager *m_default_toi_resource_manager;
    int m_toi_count;
};

extern "C" { extern char g_phys_toi_simulation_enabled; }
phys_result phys_toi_localized_solve(const phys_toi_resources &toi_resources, phys_solver_resources &solver_resources, phys_toi_event &event,
    phys_toi_resource_manager &toi_resource_manager, phys_world *world, phys_array<phys_entity *> &active_entities,
        phys_fixed_array<unsigned char> &entity_state);

#if defined(_M_PPCBE) || defined(_XBOX)
typedef char phys_continuous_simulation_size[(sizeof(void *) != 4 || (sizeof(phys_continuous_simulation) == 80)) ? 1 : -1];
#endif

#endif
