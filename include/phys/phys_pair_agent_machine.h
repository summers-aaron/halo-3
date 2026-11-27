#ifndef INCLUDE_PHYS_PAIR_AGENT_MACHINE_H
#define INCLUDE_PHYS_PAIR_AGENT_MACHINE_H

// Functions defined in tu/hkinternal/hkAgentNnMachine.cpp.

struct phys_pair_agent_entry;
struct phys_pair_agent_track;
struct phys_collision_input;
class phys_collider;
class phys_constraint_owner;
class phys_island;
class phys_contact_manager;
class phys_dispatcher;
class phys_linked_collider;
struct phys_process_collision_input;
struct phys_process_collision_output;

extern "C" void phys_pair_agent_update_compound_filter(phys_pair_agent_entry *entry, phys_collision_input *input, phys_constraint_owner *owner);

extern "C" phys_pair_agent_entry *phys_pair_agent_find(phys_linked_collider *a, phys_linked_collider *b);
extern "C" void phys_pair_agent_free_entry_internal(phys_pair_agent_track &track, phys_pair_agent_entry *entry);
extern "C" void phys_pair_agent_append_track(phys_pair_agent_track &destination, phys_pair_agent_track &source);
extern "C" void phys_pair_agent_assert_track_valid(phys_pair_agent_track &track);
extern "C" void phys_pair_agent_get_type(phys_collider *collision_proxy_a, phys_collider *collision_proxy_b, const phys_process_collision_input &input,
    int &agent_type_out, int &out_is_flipped);
extern "C" phys_pair_agent_entry *phys_pair_agent_create(phys_pair_agent_track &track, phys_linked_collider *collider_a, phys_linked_collider *collider_b,
    int agent_type_id, const phys_process_collision_input &input, phys_contact_manager *pair_contact_manager);
extern "C" void phys_pair_agent_process(phys_pair_agent_entry *entry, const phys_process_collision_input &input,
    phys_process_collision_output &output, phys_contact_manager *pair_contact_manager);
extern "C" void phys_pair_agent_process_track(phys_constraint_owner *owner, phys_pair_agent_track &pair_agent_track, const phys_process_collision_input &input);
extern "C" void phys_pair_agent_destroy(phys_pair_agent_track &track, phys_pair_agent_entry *entry,
    phys_dispatcher *dispatcher, phys_constraint_owner &owner);
extern "C" void phys_pair_agent_invalidate_sweep(phys_pair_agent_entry *entry, phys_collision_input *input);
extern "C" void phys_pair_agent_warp_time(phys_pair_agent_entry *entry, float agent_old_time, float agent_new_time, phys_collision_input *input);
phys_pair_agent_entry *phys_pair_agent_alloc_entry(phys_pair_agent_track &track, int agent_type_id);
extern "C" void phys_pair_agent_pop_last_entry(phys_pair_agent_track &track);
extern "C" phys_pair_agent_entry *phys_pair_agent_copy_relink_entry(phys_pair_agent_track *track, phys_pair_agent_entry *source);

class phys_swept_transform;
class phys_transform;
namespace phys_swept_transform_util {
void lerp_swept(const phys_swept_transform &swept_transform, float time, phys_transform &xform_out);
}
#endif // HAVOK_HK_AGENT_NN_MACHINE_H
