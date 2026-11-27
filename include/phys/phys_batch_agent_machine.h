#ifndef INCLUDE_PHYS_BATCH_AGENT_MACHINE_H
#define INCLUDE_PHYS_BATCH_AGENT_MACHINE_H

#include "phys/phys_agent_entry.h"
#include "phys/phys_math.h"
#include "phys/phys_shape_key.h"

class phys_agent;

// The 1n agent stream entry: the common agent header, the agent object of an old-style stream agent, and the
// shape key of the collection child this agent collides with (hkAgent1nMachine_FindAgent compares +8).
#include "tag_files/tag_types.h"

struct phys_batch_agent_entry : phys_agent_entry {
    TAG_POINTER(phys_agent) m_stream_agent;
    phys_shape_key m_shape_key;
};

struct phys_batch_agent_padded_entry : phys_batch_agent_entry {
    unsigned int m_pad_to_16;
};

struct phys_batch_agent_sweep_entry : phys_batch_agent_entry {
    float m_separating_normal_time;
    phys_vec4 m_separating_normal;
};

#if defined(_M_PPCBE) || defined(_XBOX)
typedef char phys_batch_agent_entry_size[(sizeof(phys_batch_agent_entry) == 12) ? 1 : -1];
typedef char phys_batch_agent_pad_entry_size[(sizeof(void *) != 4 || (sizeof(phys_batch_agent_padded_entry) == 16)) ? 1 : -1];
typedef char phys_batch_agent_sweep_entry_size[(sizeof(void *) != 4 || (sizeof(phys_batch_agent_sweep_entry) == 32)) ? 1 : -1];
#endif

// Functions defined in tu/hkinternal/hkAgent1nMachine.cpp.

struct phys_batch_agent_visitor_input;
struct phys_batch_agent_track;
struct phys_narrow_agent_input;
struct phys_narrow_agent_process_input;
struct phys_process_collision_output;
struct phys_collision_input;
class phys_shape_container;
class phys_dispatcher;
class phys_contact_manager;
class phys_constraint_owner;

extern "C" void phys_batch_agent_destroy(phys_batch_agent_track &track, phys_dispatcher *dispatcher,
    phys_contact_manager *agent_contact_manager, phys_constraint_owner &owner);
extern "C" void phys_batch_agent_process(phys_batch_agent_track &track, phys_batch_agent_track *track_on_ppu, phys_narrow_agent_process_input &input,
    const phys_shape_container *container, const unsigned int *hit_key_list, int agent_key_count, phys_process_collision_output &output);

extern "C" void phys_batch_agent_update_compound_filter(phys_batch_agent_track &track, phys_batch_agent_visitor_input &input);
extern "C" void phys_batch_agent_create(void *data);
extern "C" void phys_batch_agent_warp_time(phys_batch_agent_track *track, float agent_old_time, float agent_new_time, phys_collision_input *input);
extern "C" void phys_batch_agent_invalidate_sweep(phys_batch_agent_track *track, phys_collision_input *input);
extern "C" void phys_batch_agent_weld(phys_narrow_agent_input &input, const phys_shape_container *compound_shape, phys_process_collision_output &output);


extern "C" phys_batch_agent_entry *phys_batch_agent_find(phys_batch_agent_track &track, phys_shape_key collision_partner, void **agent_data_out);
void *phys_batch_agent_compound_filter_visitor(phys_batch_agent_visitor_input &visitor_input, phys_batch_agent_entry *entry, void *agent_data);
void phys_batch_agent_visit_all(phys_batch_agent_track &track, phys_batch_agent_visitor_input &input,
    void *(*visitor)(phys_batch_agent_visitor_input &, phys_batch_agent_entry *, void *));

#endif // HAVOK_HK_AGENT1N_MACHINE_H
