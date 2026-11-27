#ifndef AI_JOINT_BEHAVIOR_H
#define AI_JOINT_BEHAVIOR_H

#include <stddef.h>

#include "ai/behavior.h"
#include "ai/behavior_info.h"

// Actor state for participation in a joint behavior.
class c_joint_behavior_data : public c_behavior_state
{
public:
    long joint_index;
    short jstatus;
    short slot;
    bool initiator;
};

typedef char joint_behavior_data_offset_joint_index[((offsetof(c_joint_behavior_data, joint_index)) == 0x24) ? 1 : -1];
typedef char joint_behavior_data_offset_initiator[((offsetof(c_joint_behavior_data, initiator)) == 0x2c) ? 1 : -1];

// Formation joint-behavior state, written by the formation atom (ai_atoms.cpp) and read by the
// formation behavior callbacks (behavior.cpp). March behavior_formation_setup (82ee8ce0) takes a
// c_formation_data * and fills formation at +0x32, ai_index at +0x34, formation_requested at
// +0x38, point_ref at +0x3c and seat at +0x40.
class c_formation_data : public c_joint_behavior_data
{
public:
    short position_index;
    unsigned short formation;
    long ai_index;
    unsigned char formation_requested;
    long point_ref;
    long seat;
};

typedef char formation_data_position_index_offset[(offsetof(c_formation_data, position_index) == 0x30) ? 1 : -1];
typedef char formation_data_formation_offset[(offsetof(c_formation_data, formation) == 0x32) ? 1 : -1];
typedef char formation_data_ai_index_offset[(offsetof(c_formation_data, ai_index) == 0x34) ? 1 : -1];
typedef char formation_data_requested_offset[(offsetof(c_formation_data, formation_requested) == 0x38) ? 1 : -1];
typedef char formation_data_point_ref_offset[(offsetof(c_formation_data, point_ref) == 0x3c) ? 1 : -1];
typedef char formation_data_seat_offset[(offsetof(c_formation_data, seat) == 0x40) ? 1 : -1];


#include "ai/joint_behavior_functions.h"

struct s_behavior_input;
struct s_behavior_arguments;
class c_behavior_state;
struct joint_state_datum;
struct child_entry;
enum e_joint_type
{
    _joint_type_invitation = 1,
    _joint_type_blackboard = 2
};

bool joint_primitive_behavior_activate(long actor_index, c_behavior_state *state);
bool joint_primitive_behavior_perform(long actor_index, c_behavior_state *state);
void joint_primitive_behavior_control(long actor_index, c_behavior_state *state);
void joint_primitive_behavior_debug(long actor_index, c_behavior_state *state);

class Joint_primitive_behavior : public Primitive_behavior
{
public:
    typedef long (*activate_proc)(long, c_behavior_state *);
    typedef void (*notify_proc)(long, c_behavior_state *, joint_state_datum *);
    typedef bool (*perform_proc)(long, c_behavior_state *, joint_state_datum *);
    typedef short (*select_proc)(long, long, c_behavior_state *);

    Joint_primitive_behavior(short index, long flags, behavior_desire_proc desire, behavior_next_proc next,
        activate_proc activate, notify_proc deactivate, short layer, perform_proc perform, notify_proc control,
        e_joint_type type, select_proc count, short minimum, short maximum, float weight,
        behavior_clear_state_proc clear_state = 0)
        : Primitive_behavior(index, flags, desire, next, joint_primitive_behavior_activate,
              joint_primitive_behavior_deactivate, layer, joint_primitive_behavior_perform, joint_primitive_behavior_control,
              0, 0, 0, 0, 0, 0, clear_state),
          activate(activate), deactivate(deactivate), debug(0), perform(perform), update(control), count(count),
          joint_type(type), minimum_participants(minimum), maximum_participants(maximum), weight(weight)
    {
        joint = true;
    }

    Joint_primitive_behavior(short index, long flags, behavior_desire_proc desire, behavior_next_proc next,
        activate_proc activate, notify_proc deactivate, short layer, perform_proc perform, notify_proc control,
        e_joint_type type, select_proc count, short minimum, short maximum, float weight,
        behavior_object_proc on_delete, behavior_object_proc flush_prop, behavior_object_proc handle_delete,
        behavior_notify_proc flush_structure, behavior_firing_position_proc flush_positions,
        notify_proc debug, behavior_clear_state_proc clear_state)
        : Primitive_behavior(index, flags, desire, next, joint_primitive_behavior_activate,
              joint_primitive_behavior_deactivate, layer, joint_primitive_behavior_perform, joint_primitive_behavior_control,
              on_delete, flush_prop, handle_delete, flush_structure, flush_positions, joint_primitive_behavior_debug, clear_state),
          activate(activate), deactivate(deactivate), debug(debug), perform(perform), update(control), count(count),
          joint_type(type), minimum_participants(minimum), maximum_participants(maximum), weight(weight)
    {
        joint = true;
    }

    activate_proc activate;
    notify_proc deactivate;
    notify_proc debug;
    perform_proc perform;
    notify_proc update;
    select_proc count;
    e_joint_type joint_type;
    short minimum_participants;
    short maximum_participants;
    float weight;
};

typedef char joint_primitive_behavior_size[(sizeof(void *) != 4 || sizeof(Joint_primitive_behavior) == 0x70) ? 1 : -1];

struct Joint_mutex_group : public Mutex_group
{
    long (*activate)(long, c_behavior_state *);
    void (*deactivate)(long, c_behavior_state *, joint_state_datum *);
    short (*decide)(long, short, c_behavior_state *, joint_state_datum *);
    long update;
    short (*count)(long, long, c_behavior_state *);
    long joint_type;
    short minimum_participants;
    short maximum_participants;
    float weight;

    Joint_mutex_group(
        short behavior_index,
        long flags,
        short (*get_desire_fn)(long, const s_behavior_input *, const s_behavior_arguments *),
        short (*next_fn)(long, c_behavior_state *, c_behavior_state *),
        long (*activate_fn)(long, c_behavior_state *),
        void (*deactivate_fn)(long, c_behavior_state *, joint_state_datum *),
        short layer_behavior_index_arg,
        short (*decide_fn)(long, short, c_behavior_state *, joint_state_datum *),
        short child_count,
        child_entry *children_arg,
        e_joint_type joint_type,
        short (*count_fn)(long, long, c_behavior_state *),
        short min_participants,
        short max_participants,
        float weight_arg,
        void (*flush_object_fn)(long, c_behavior_state *, long),
        void (*flush_prop_fn)(long, c_behavior_state *, long),
        void (*flush_firing_pos_fn)(long, c_behavior_state *, long),
        void (*flush_struct_fn)(long, c_behavior_state *),
        void (*flush_firing_position_fn)(long, c_behavior_state *, bool),
        void (*update_fn)(long, c_behavior_state *, joint_state_datum *),
        void (*debug_fn)(c_behavior_state *));
};

typedef char joint_mutex_group_size_check[(sizeof(void *) != 4 || sizeof(Joint_mutex_group) == 0x70) ? 1 : -1];

typedef char joint_mutex_group_flags_offset[(offsetof(Joint_mutex_group, flags) == 4) ? 1 : -1];
typedef char joint_mutex_group_executing_nonce_offset[(offsetof(Joint_mutex_group, executing_nonce) == 0xc) ? 1 : -1];
typedef char joint_mutex_group_children_offset[(sizeof(void *) != 4 || ((offsetof(Joint_mutex_group, children) == 0x4c))) ? 1 : -1];
typedef char joint_mutex_group_kind_offset[(offsetof(Joint_mutex_group, kind) == 8) ? 1 : -1];
typedef char joint_mutex_group_layer_behavior_index_offset[(sizeof(void *) != 4 || ((offsetof(Joint_mutex_group, layer_behavior_index) == 32))) ? 1 : -1];
typedef char joint_mutex_group_on_object_deleted_offset[(sizeof(void *) != 4 || ((offsetof(Joint_mutex_group, on_object_deleted) == 36))) ? 1 : -1];
typedef char joint_mutex_group_handle_deleted_object_offset[(sizeof(void *) != 4 || ((offsetof(Joint_mutex_group, handle_deleted_object) == 44))) ? 1 : -1];
typedef char joint_mutex_group_flush_structure_offset[(sizeof(void *) != 4 || ((offsetof(Joint_mutex_group, flush_structure) == 48))) ? 1 : -1];
typedef char joint_mutex_group_flush_firing_position_offset[(sizeof(void *) != 4 || ((offsetof(Joint_mutex_group, flush_firing_position) == 52))) ? 1 : -1];
typedef char joint_mutex_group_child_count_offset[(sizeof(void *) != 4 || ((offsetof(Joint_mutex_group, child_count) == 72))) ? 1 : -1];
typedef char joint_mutex_group_update_offset[(sizeof(void *) != 4 || ((offsetof(Joint_mutex_group, update) == 92))) ? 1 : -1];

extern Joint_mutex_group presearch_behavior;

#endif
