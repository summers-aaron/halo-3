#ifndef AI_BEHAVIOR_INFO_H
#define AI_BEHAVIOR_INFO_H

#include <stddef.h>
#include "ai/behavior.h"

typedef short (*behavior_desire_proc)(long, const s_behavior_input *, const s_behavior_arguments *);
typedef short (*behavior_next_proc)(long, c_behavior_state *, c_behavior_state *);
typedef bool (*behavior_action_proc)(long, c_behavior_state *);
typedef void (*behavior_notify_proc)(long, c_behavior_state *);
typedef void (*behavior_object_proc)(long, c_behavior_state *, long);
typedef void (*behavior_firing_position_proc)(long, c_behavior_state *, bool);
typedef void (*behavior_clear_state_proc)(c_behavior_state *);
typedef short (*behavior_decide_proc)(long, short, c_behavior_state *);

class Behavior_header
{
public:
    Behavior_header(short behavior_index, short behavior_type, long condition_flags)
        : index(behavior_index), type(behavior_type), flags(condition_flags), kind(-2), executing_nonce(0) {}

    short index;
    short type;
    long flags;
    long kind;
    long executing_nonce;

protected:
    Behavior_header() {}
};

class Behavior_impulse : public Behavior_header
{
public:
    Behavior_impulse(short index, long flags, short (*impulse)(long, c_behavior_state *))
        : Behavior_header(index, 0, flags), get_impulse(impulse) {}

    short (*get_impulse)(long, c_behavior_state *);
};

class Behavior : public Behavior_header
{
public:
    behavior_desire_proc get_desire;
    behavior_next_proc next;
    behavior_action_proc activate;
    behavior_notify_proc deactivate;
    short layer_behavior_index;
    behavior_object_proc on_object_deleted;
    behavior_object_proc flush_prop;
    behavior_object_proc handle_deleted_object;
    behavior_notify_proc flush_structure;
    behavior_firing_position_proc flush_firing_position;
    behavior_notify_proc debug;
    behavior_clear_state_proc clear_non_deterministic_state;
    bool joint;

protected:
    Behavior() {}

    Behavior(short index, short type, long flags, behavior_desire_proc desire, behavior_next_proc next,
        behavior_action_proc activate, behavior_notify_proc deactivate, short layer,
        behavior_object_proc on_delete, behavior_object_proc flush_prop, behavior_object_proc handle_delete,
        behavior_notify_proc flush_structure, behavior_firing_position_proc flush_positions,
        behavior_notify_proc debug, behavior_clear_state_proc clear_state)
        : Behavior_header(index, type, flags), get_desire(desire), next(next), activate(activate),
          deactivate(deactivate), layer_behavior_index(layer), on_object_deleted(on_delete), flush_prop(flush_prop),
          handle_deleted_object(handle_delete), flush_structure(flush_structure), flush_firing_position(flush_positions),
          debug(debug), clear_non_deterministic_state(clear_state), joint(false) {}
};

class Primitive_behavior : public Behavior
{
public:
    Primitive_behavior(short index, long flags, behavior_desire_proc desire, behavior_next_proc next,
        behavior_action_proc activate, behavior_notify_proc deactivate, short layer, behavior_action_proc perform,
        behavior_notify_proc control, behavior_object_proc on_delete = 0, behavior_object_proc flush_prop = 0,
        behavior_object_proc handle_delete = 0, behavior_notify_proc flush_structure = 0,
        behavior_firing_position_proc flush_positions = 0, behavior_notify_proc debug = 0,
        behavior_clear_state_proc clear_state = 0)
        : Behavior(index, 2, flags, desire, next, activate, deactivate, layer, on_delete, flush_prop,
              handle_delete, flush_structure, flush_positions, debug, clear_state),
          perform(perform), control(control) {}

    behavior_action_proc perform;
    behavior_notify_proc control;
};

class Mutex_group : public Behavior
{
public:
    Mutex_group(short index, long flags, behavior_desire_proc desire, behavior_next_proc next,
        behavior_action_proc activate, behavior_notify_proc deactivate, short layer, behavior_decide_proc decide,
        short count, child_entry *children, behavior_object_proc on_delete = 0, behavior_object_proc flush_prop = 0,
        behavior_object_proc handle_delete = 0, behavior_notify_proc flush_structure = 0,
        behavior_firing_position_proc flush_positions = 0, behavior_notify_proc debug = 0,
        behavior_clear_state_proc clear_state = 0)
        : Behavior(index, 1, flags, desire, next, activate, deactivate, layer, on_delete, flush_prop,
              handle_delete, flush_structure, flush_positions, debug, clear_state),
          decide(decide), child_count(count), children(children) {}

    behavior_decide_proc decide;
    short child_count;
    child_entry *children;

protected:
    Mutex_group() {}
};

struct behavior_info
{
    Behavior_header *header;
    short state_size;
    short inhibit_flags;
    bool flag;
};

typedef char behavior_impulse_size[(sizeof(void *) != 4 || sizeof(Behavior_impulse) == 0x14) ? 1 : -1];
typedef char behavior_header_size[(sizeof(Behavior_header) == 0x10) ? 1 : -1];
typedef char behavior_joint_offset[(sizeof(void *) != 4 || offsetof(Behavior, joint) == 0x40) ? 1 : -1];
typedef char primitive_behavior_size[(sizeof(void *) != 4 || sizeof(Primitive_behavior) == 0x4c) ? 1 : -1];
typedef char mutex_group_children_offset[(sizeof(void *) != 4 || offsetof(Mutex_group, children) == 0x4c) ? 1 : -1];
typedef char behavior_info_size[(sizeof(void *) != 4 || sizeof(behavior_info) == 12) ? 1 : -1];
typedef char behavior_info_flag_offset[(sizeof(void *) != 4 || offsetof(behavior_info, flag) == 8) ? 1 : -1];

class Joint_primitive_behavior;

extern Behavior_impulse smash_obstacle_impulse;
extern Primitive_behavior blind_panic_behavior;
extern Behavior_impulse charge_when_cornered_impulse;
extern Primitive_behavior bugger_group_uncover_behavior;
extern Mutex_group swarm_root_behavior;
extern Primitive_behavior swarm_attack_behavior;
extern Primitive_behavior support_attack_behavior;
extern Behavior_impulse infect_impulse;
extern Primitive_behavior swarm_scatter_behavior;
extern Behavior_impulse combatform_berserk_control_impulse;
extern Primitive_behavior flood_eject_parasite_behavior;
extern Behavior_impulse sentinel_enforcer_weapon_control_impulse;
extern Primitive_behavior surge_behavior;
extern Behavior_impulse guardian_proximity_surge_impulse;
extern Behavior_impulse guardian_danger_surge_impulse;
extern Behavior_impulse guardian_isolation_surge_impulse;
extern Behavior_impulse pureform_group_morph_impulse;
extern Behavior_impulse pureform_arrival_morph_impulse;
extern Behavior_impulse pureform_default_form_impulse;
extern Behavior_impulse pureform_search_morph_impulse;
extern Behavior_impulse pureform_stealth_active_camo_control_impulse;
extern Behavior_impulse pureform_stealth_damage_morph_impulse;
extern Primitive_behavior pureform_stealth_stalk_behavior;
extern Mutex_group pureform_stealth_stalk_group_behavior;
extern Primitive_behavior pureform_stealth_charge_recover_behavior;
extern Behavior_impulse pureform_ranged_proximity_morph_impulse;
extern Behavior_impulse pureform_tank_distance_damage_morph_impulse;
extern Behavior_impulse pureform_tank_throttle_control_impulse;
extern Primitive_behavior pureform_stealth_morph_behavior;
extern Primitive_behavior pureform_tank_morph_behavior;
extern Primitive_behavior pureform_ranged_morph_behavior;
extern Primitive_behavior pureform_ranged_turtle_behavior;
extern Primitive_behavior pureform_ranged_uncover_behavior;
extern Mutex_group scarab_root_behavior;
extern Primitive_behavior scarab_cure_isolation_behavior;
extern Mutex_group scarab_combat_behavior;
extern Primitive_behavior scarab_fight_behavior;
extern Behavior_impulse scarab_target_lock_impulse;
extern Mutex_group scarab_search_behavior;
extern Primitive_behavior scarab_search_pause_behavior;
extern Joint_primitive_behavior scared_by_leader_behavior;

#endif // AI_BEHAVIOR_INFO_H
