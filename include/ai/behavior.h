#ifndef AI_BEHAVIOR_H
#define AI_BEHAVIOR_H

#include <stddef.h>

#include "ai/ai_point3d.h"
#include "ai/sector_ref.h"
#include "math/real_math_types.h"

// The impulse that pushed this behavior (+0x0c). Its object index is read by actor_flush_prop and
// actor_handle_deleted_object; behavior_go_to_perform keeps a pointer to it and reads the point and sector.
struct s_behavior_impulse
{
    unsigned char type;
    signed char mode;
    union {
        struct { short index; short variant; };
        long object_index;
        c_ai_point3d point;
    };
    c_sector_ref sector;
};

// One layer of an actor's behavior stack (ai\behavior.h; the March 2007 debug build asserts from
// ..\..\source\ai\behavior.h). A plain class: the March map owns no method of it. Each behavior keeps its
// own data after this header in a derived class (March names c_flee_state_data, c_patrol_state_data,
// c_kamikaze_state_data, c_cure_isolation_data, c_formation_data, c_joint_behavior_data), so a behavior
// callback that takes a c_behavior_state * converts it to its own derived class.
// Offsets: March behavior_clear_layer_state writes +0 (halfword), +4 (halfword) and +8 (word);
// behavior_state_fail_child writes +2 and +4. The derived data starts at +0x24 in every unit
// (behavior_guard_perform_state_data, swarm_scatter_behavior, flood stalker, scarab).
class c_behavior_state
{
public:
    short behavior_index;
    short child_index;
    short execution_state;
    short pad6;
    long start_time;
    s_behavior_impulse impulse;
};

typedef char behavior_state_size_must_be_0x24[(sizeof(c_behavior_state) == 0x24) ? 1 : -1];
typedef char behavior_state_offset_start_time[((offsetof(c_behavior_state, start_time)) == 0x08) ? 1 : -1];
typedef char behavior_state_offset_impulse_index[((offsetof(c_behavior_state, impulse.index)) == 0x10) ? 1 : -1];
typedef char behavior_state_offset_impulse_sector[((offsetof(c_behavior_state, impulse.sector)) == 0x20) ? 1 : -1];


// Deploy-turret behavior state, written by the deploy atom (ai_atoms.cpp) and read by the
// deploy_turret behavior callbacks (behavior.cpp). March behavior_deploy_turret_setup (82f00620)
// sets keep at +0x39 and point_ref at +0x3c; behavior_deploy_turret_activate (82f00710) writes
// +0x24 (false), +0x28 (NONE) and +0x38 (false).
struct deploy_turret_state_data : public c_behavior_state
{
    bool b24;
    long turret_index;
    real_vector3d facing;
    bool deployed;
    bool keep;
    long point_ref;
};

typedef char deploy_turret_state_b24_offset[(offsetof(deploy_turret_state_data, b24) == 0x24) ? 1 : -1];
typedef char deploy_turret_state_turret_index_offset[(offsetof(deploy_turret_state_data, turret_index) == 0x28) ? 1 : -1];
typedef char deploy_turret_state_facing_offset[(offsetof(deploy_turret_state_data, facing) == 0x2c) ? 1 : -1];
typedef char deploy_turret_state_deployed_offset[(offsetof(deploy_turret_state_data, deployed) == 0x38) ? 1 : -1];
typedef char deploy_turret_state_keep_offset[(offsetof(deploy_turret_state_data, keep) == 0x39) ? 1 : -1];
typedef char deploy_turret_state_point_ref_offset[(offsetof(deploy_turret_state_data, point_ref) == 0x3c) ? 1 : -1];


// Behavior inputs, arguments and child tables shared by the behavior callbacks.
struct s_behavior_input { unsigned char type; unsigned char mode; short pad; short index; };

struct s_behavior_arguments { short script_index; short pad2; float distance; };

struct child_entry
{
    unsigned short behavior_index;
    void (*setup)(c_behavior_state *state, const s_behavior_arguments *arguments);
    bool interruptible;
    short test_type;
    short test_index;
    float test_seconds;
    bool has_arguments;
    unsigned char : 8;
    unsigned char : 8;
    unsigned char : 8;
    s_behavior_arguments arguments;
    unsigned long : 32;
    unsigned long : 32;
};

struct custom_child_entry { short type; short behavior_index; child_entry entry; };

struct custom_child_list { short behavior_index; custom_child_entry *entries; short count; };

typedef char child_entry_size[(sizeof(void *) != 4 || sizeof(child_entry) == 0x28) ? 1 : -1];
typedef char custom_child_list_size[(sizeof(void *) != 4 || sizeof(custom_child_list) == 0xc) ? 1 : -1];

// Functions defined in tu/ailibxbox/behavior.cpp.

struct joint_state_datum;
union real_point3d;
struct s_behavior_arguments;
struct s_behavior_input;

short actor_executing_layer(long datum_index, short value);

// Whether the actor runs a behavior on the layer.
static __forceinline bool actor_executing(long actor_index, short layer)
{
    return actor_executing_layer(actor_index, layer) != -1;
}

long behavior_build_condition_flags(long actor_index);
bool behavior_change(long actor_index, short layer, short behavior_index, c_behavior_state *new_state);
short behavior_presearch_get_desire(long actor_index, const s_behavior_input *input, const s_behavior_arguments *arguments);
void behaviors_initialize();
short behavior_decide_sequential_looping(long actor_index, short layer, c_behavior_state *parent_state);
void actor_get_desired_follow_range(long actor_index, long leader_unit_index, float *minimum_range, float *maximum_range);
long behavior_presearch_activate(long actor_index, c_behavior_state *state);
short behavior_presearch_next(long actor_index, c_behavior_state *state, c_behavior_state *next_state);
void behavior_presearch_deactivate(long actor_index, c_behavior_state *state, joint_state_datum *joint_state);
void behavior_presearch_flush_prop(long actor_index, c_behavior_state *state, long prop_index);
void ai_handle_deployed_turret(long actor_index, long turret_index);
bool behavior_allowed_to_investigate(long actor_index, long prop_ref_index);
void mark_executing_behaviors(long actor_index);
short behavior_melee_charge_get_desire(long actor_index, const s_behavior_input *input, const s_behavior_arguments *arguments);
void action_selection(long actor_index);
bool actor_assess_danger(long actor_index, float radius, bool *danger, bool *vehicle, bool *projectile, bool *p5, float *p6);
short behavior_presearch_decide(long actor_index, short layer, class c_behavior_state *behavior_state, struct joint_state_datum *joint_state);
bool vehicle_find_destination(long actor_index, long vehicle_index, real_point3d *point, c_ai_point3d *ai_point, c_sector_ref *sector_ref,
    bool *destination_found, bool use_actor_position);
short equipment_activate_impulse_get_impulse(long actor_index, c_behavior_state *state);
short behavior_search_get_desire(long actor_index, const s_behavior_input *input, const s_behavior_arguments *arguments);
extern "C" long unit_get_child_actor(long unit_index, long *child_unit_index, bool driver_only);
extern "C" bool actor_has_special_equipment(long actor_index);

short behavior_surprise_get_desire(long actor_index, const s_behavior_input *input, const s_behavior_arguments *arguments);
short behavior_search_next(long actor_index, c_behavior_state *state, c_behavior_state *next_state);
short berserk_get_impulse(long actor_index, c_behavior_state *state);
short behavior_decide_prioritized(long actor_index, short layer, c_behavior_state *state);
short behavior_decide_sequential(long actor_index, short layer, c_behavior_state *state);

// Data defined in tu/ailibxbox/behavior.cpp that other units use.

struct child_entry;
struct precomputed_child_list
{
    short offset;
    signed char count;
    char pad3;
};

typedef char precomputed_child_list_size_must_be_4[(sizeof(precomputed_child_list) == 4) ? 1 : -1];
typedef char precomputed_child_list_offset_count[((offsetof(precomputed_child_list, count)) == 2) ? 1 : -1];

extern child_entry *g_precomputed_child_entries[5120];
extern precomputed_child_list g_precomputed_child_list_indices[197][25];

extern long g_action_selection_nonce;
extern long global_condition_flags;

struct behavior_info;
struct custom_child_list;

extern behavior_info global_behavior_info[197];
extern short behavior_input_type_compatibility[6];
extern custom_child_list *global_custom_behavior_list[25];

extern "C" {
extern const float k_behavior_follow_ranges[2];
extern const float k_behavior_surprise_movement_values[];
extern long k_behavior_posture_type_names[];
extern long k_posture_variant_indices[35][10];
extern short g_behavior_posture_variant_counts[35];
extern short k_clump_state_combat_status[];
extern short k_movement_combat_status_index[];
extern short k_movement_combat_status_table[];
}

class Behavior_header;
struct actor_datum;
class c_cure_isolation_data;
class c_fight_state_data;
class c_flee_state_data;
class c_patrol_state_data;
class c_vehicle_board_data;
struct path_state;
union real_point2d;
union real_vector2d;
union real_vector3d;
struct seat_entry_info;

Behavior_header *actor_behavior_get(actor_datum *actor, short layer);
extern "C" float actor_grenade_get_throw_delay(long actor_index);
bool actor_search_allowed(long actor_index);
c_behavior_state *actor_behavior_state_allocate(actor_datum *actor, short layer);
extern "C" bool behavior_vehicle_charge_activate_with_duration(long actor_index, c_behavior_state *state);
void fight_reset_firing_position_timer(long actor_index, c_fight_state_data *state);
short behavior_flee_get_desire(long actor_index, const s_behavior_input *input, const s_behavior_arguments *arguments);
bool flee_at_flee_position(long actor_index, c_flee_state_data *state);
bool formation_get_position(long actor_index, short formation, short member_index, const real_point2d *origin,
    const real_vector2d *forward, const real_vector2d *right, real_point2d *position);
void patrol_choose_random_point(short point_count, c_patrol_state_data *state, bool retry);
void actor_request_search(long actor_index);
short behavior_self_preserve_time_left(long datum_index);
short behavior_wander_get_desire(long actor_index, const s_behavior_input *input, const s_behavior_arguments *arguments);
bool emerge_wait_for_actor(long actor_index, long other_actor_index, float *out_distance, short *out_time);
bool cover_friend_continue(long actor_index, long other_actor_index, short behavior);
extern "C" short construct_child_list_internal(child_entry **output_child_list, custom_child_list *custom_children, child_entry *default_children,
    short number_of_default_children);
short behavior_update_running_child(long actor_index, short layer, c_behavior_state *state, short *running_child);
void test_child_entry(long actor_index, child_entry *entry, short *behavior_index, short *desire, c_behavior_state *state, c_behavior_state *child_state);
long pickup_find_passenger(long actor_index);
void formation_get_facing(joint_state_datum *joint_state, real_vector2d *facing);
bool leap_on_cover_object_get_target_point(long actor_index, long object_index, union real_point3d * point);
bool board_activate_helper(long actor_index, c_vehicle_board_data *data);
bool fight_choose_fight_point(long actor_index, c_fight_state_data *state);
bool actor_action_find_escape_from_danger(long actor_index, short *escape_direction_reference, float *escape_distance_reference,
    real_vector2d *alignment_vector_reference, bool *escape_blocked_reference);
bool action_flee_current_position_exposed(long actor_index, c_flee_state_data *state);
void action_flee_find_flee_position(long actor_index, bool unused, c_flee_state_data *state);
bool find_pursuit_position(long actor_index, long prop_ref_index);
bool cure_isolation_visible_enemy(long actor_index, path_state *path, float distance);
bool cure_isolation_attached_vehicle(long actor_index, seat_entry_info *seat, c_cure_isolation_data *data, path_state *path);
bool cure_isolation_undirected(long actor_index, path_state *path, float distance);
extern "C" bool behavior_compute_avoid_vector(long actor_index, float minimum_magnitude, real_vector3d *avoid_vector);

extern long g_current_actions_nonce;

extern child_entry behavior_presearch_children[];

#endif
