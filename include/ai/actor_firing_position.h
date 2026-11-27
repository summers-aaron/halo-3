#ifndef AI_ACTOR_FIRING_POSITION_H
#define AI_ACTOR_FIRING_POSITION_H

#include <stddef.h>

#include "ai/firing_position_ref.h"
#include "ai/actor_firing_position_selection.h"
#include "math/real_math_types.h"
#include "ai/sector_ref.h"
#include "ai/ai_point3d.h"
#include "ai/path_flying.h"
#include "objects/object_reference_types.h"
#include "objects/object_placement.h"

struct firing_position_definition;

// Combat tests and evaluation state for selecting an actor's firing position.
struct actor_firing_position_combat_test {
 short type;
 real_point3d origin;
 real_vector3d aiming;
 long object_index;
};
struct actor_firing_position_influence {
 bool reject;
 bool positive;
 float radius;
 real_point3d position;
};
struct firing_position_evaluation_context {
 short type;
 // The mode-specific evaluation data starts on a word boundary.
 __declspec(align(4)) bool panic;
 union {
  float panic_distance;
  long panic_prop_index;
 };
 float danger_minimum_distance;
 union {
  float danger_maximum_distance;
  struct { bool danger; bool danger_maximum; };
 };
 bool debug;
 bool use_current_position;
 bool ignore_dynamic_firing_positions;
 bool ignore_unassailable_test;
 float panic_maximum_distance;
 float radius;
 bool target_override_valid;
 real_point3d target_override_position;
 c_ai_point3d target_override_point;
 c_sector_ref target_override_sector;
 s_cluster_reference target_override_cluster;
 bool attractor_valid;
 float attractor_weight;
 float attractor_radius;
 bool attractor_blocking;
 bool evaluate_danger_zone;
 signed char compute_path_direction;
 unsigned char flee_search_mode;       // Flee-position search mode.
 signed char compute_target_distance;
 bool has_firing_direction;
 bool has_second_direction;
 signed char ignore_area_path;
 bool allow_unreachable;
 bool prefer_special;
 bool use_weapon_estimate;
 bool require_special;
 bool allow_either;
 bool penalize_unconcealed;
 bool ignore_cover;
 bool prefer_concealed;
 bool combat_disabled;
 bool require_facing;
 float facing_weight;
 char pad68[8];
 long influence_count;
 actor_firing_position_influence influences[32];
 short combat_test_count;
 short combat_position_count;
 short attack_test_count;
 actor_firing_position_combat_test combat_tests[32];
 bool has_estimate_direction;
 real_vector3d estimate_direction;
 real_point3d test_position;
 union {
  bool evaluate_danger;
  bool vehicle_cover;
  bool hide;
  bool uncover;
  bool attack;
 };
 float hide_distance;
 real_point3d target_position;
 real_point3d estimate_target_position;
 real_point3d line_of_sight_target;
 bool special;
 long target_object_index;
 c_sector_ref target_sector;
 c_ai_point3d target_point;
 s_cluster_reference target_cluster;
 long target_prop_index;
 float leader_distance;
 bool hint;
 bool negative;
 union {
  real_vector3d target_hint_vector;      // (March asserts it is zero or unit length)
  real_point3d target_hint_position;     // Target point for stalking and charge recovery.
 };
 bool post_evaluation_result;
 float score;
 long minimum_distance;
 long maximum_distance;
 bool former_enabled;
 bool former_strongly;
 firing_position_ref former_position_index; // (March asserts evaluation_context->former_position_index)
 bool include_current_position;
 char pad793[0x7bc - 0x793];
 bool prefer_flags;
 short preferred_flags;
 bool guard;
 bool guard_negative;
 bool guard_valid;
 real_vector3d guard_plane;
 bool facing_check;
 float facing_threshold;
 bool enabled_areas_only;
 short area_flags;
 short standing;
 flight_evaluation_cache flight_cache;
 float attack_range_minimum;
 float attack_range_maximum;
 float attack_range_distance;
 bool attack_range_full;
 bool evaluate_cornering;
 bool dynamic_firing_positions_valid;
 object_marker cornering_markers[32];
 short cornering_marker_types[32];
 bool allow_exposed;
};
typedef char firing_context_hint_position_offset[(offsetof(firing_position_evaluation_context, target_hint_position) == 0x770) ? 1 : -1];
#include "ai/firing_position_types.h"

typedef char firing_position_evaluation_context_size_must_be_0x1724[(sizeof(firing_position_evaluation_context) == 0x1724) ? 1 : -1];
typedef char firing_position_size_must_be_0x88[(sizeof(void *) != 4 || ((sizeof(firing_position) == 0x88) )) ? 1 : -1];
typedef char firing_context_flee_mode_offset[((offsetof(firing_position_evaluation_context, flee_search_mode)) == 0x53) ? 1 : -1];
typedef char firing_context_influences_offset[((offsetof(firing_position_evaluation_context, influences)) == 0x74) ? 1 : -1];
typedef char firing_influence_size[(sizeof(actor_firing_position_influence) == 20) ? 1 : -1];


// Functions defined in tu/ailibxbox/actor_firing_position.cpp.

void actor_check_firing_position(long actor_index);
bool firing_position_ref_hash_compare(const void *a, const void *b);
void actor_clear_discarded_firing_positions(long actor_index, bool temporary_only);
bool actor_get_dynamic_firing_positions(long actor_index, firing_position *positions, short *position_count, long max_positions,
    firing_position_evaluation_context *context);
bool firing_position_setup_current_definition(long actor_index);
bool squad_get_available_firing_positions(long actor_index, long squad_index, firing_position *positions, short *position_count, long
    maximum_count, firing_position_evaluation_context *context);
void actor_setup_evaluation_context(long actor_index, firing_position_evaluation_context *context);

struct s_hash_table;
struct actor_datum;

extern "C" bool hash_table_insert(s_hash_table *table, void *key, void *value);
extern "C" void actor_set_current_firing_position(int actor_index, int fpindex);
extern "C" short actor_get_area_flags(long actor_index);
extern "C" short actor_get_area_type(long actor_index);
extern "C" bool actor_bring_forward_to_firing_position(long actor_index);

extern "C" bool consider_firing_position(long actor_index, actor_datum *actor, firing_position_ref reference, firing_position_evaluation_context *context);
void pre_evaluator_panic(long actor_index, firing_position_evaluation_context *context, short count, firing_position *positions);
void pre_evaluator_global(long actor_index, firing_position_evaluation_context *context, short count, firing_position *positions);
void pre_evaluator_obstacles(long actor_index, firing_position_evaluation_context *context, short count, firing_position *positions);
void pre_evaluator_attack(long actor_index, firing_position_evaluation_context *context, short count, firing_position *positions);
void pre_evaluator_combatmove(long actor_index, firing_position_evaluation_context *context, short count, firing_position *positions);
void pre_evaluator_pursuit(long actor_index, firing_position_evaluation_context *context, short count, firing_position *positions);
bool post_evaluator_pursuit(long actor_index, firing_position_evaluation_context *context, firing_position *position);
bool firing_position_forced_evaluation(long actor_index, firing_position_evaluation_context *context, firing_position *position);
extern "C" bool actor_at_firing_position(long actor_index);
short area_get_available_firing_positions(long actor_index, long zone_index, long area_index, firing_position *positions, short
    *position_count, long max_positions, firing_position_evaluation_context *context, short type, short flags);
void firing_position_compute_line_of_sight(long actor_index, firing_position_evaluation_context *context, firing_position *position);
void runtime_firing_position_setup_cornering_info(long actor_index, firing_position *position, firing_position_evaluation_context *context);

// Data defined in tu/ailibxbox/actor_firing_position.cpp that other units use.
extern firing_position_definition global_current_firing_position_definition;
extern "C" bool g_current_firing_position_valid;

struct path_state;

void actor_discard_firing_position(long actor_index, firing_position_ref reference, bool temporary);
struct firing_position_ref actor_select_firing_position(long p0, struct firing_position_evaluation_context *p1, struct firing_position *p2,
    long *p3, struct path_state *p4, bool * p5);
firing_position_ref actor_select_firing_position_internal(long actor_index, firing_position_evaluation_context *context, firing_position
    *firing_positions, short firing_position_count, firing_position *current_position, long *owner_actor_index, path_state *area_path_state,
    bool *area_path_state_valid);
bool new_firing_position(long actor_index, firing_position_ref reference, firing_position *position, firing_position_evaluation_context *context);


extern "C" firing_position_definition *firing_position_ref_get_definition(firing_position_ref handle);

bool actor_firing_position_discarded(long actor_index, firing_position_ref reference);
bool actor_nearby_firing_positions(long actor_index, c_ai_point3d *point, c_sector_ref sector, float radius, float path_radius);

extern const firing_position *global_temporary_sort_firing_position_array;

extern short global_temporary_sort_firing_position_count;

// Member boundaries around implicit alignment gaps.
typedef char actor_firing_position_combat_test_origin_layout[
    (offsetof(actor_firing_position_combat_test, origin) == 4) ? 1 : -1];
typedef char actor_firing_position_influence_radius_layout[
    (offsetof(actor_firing_position_influence, radius) == 4) ? 1 : -1];
typedef char firing_position_evaluation_context_mode_data_layout[
    (offsetof(firing_position_evaluation_context, panic) == 4 &&
     offsetof(firing_position_evaluation_context, panic_distance) == 8) ? 1 : -1];
typedef char firing_position_evaluation_context_ignore_dynamic_firing_positions_layout[
    (offsetof(firing_position_evaluation_context, ignore_dynamic_firing_positions) == 0x16) ? 1 : -1];
typedef char firing_position_evaluation_context_target_override_position_layout[
    (offsetof(firing_position_evaluation_context, target_override_position) == 0x24) ? 1 : -1];
typedef char firing_position_evaluation_context_attractor_weight_layout[
    (offsetof(firing_position_evaluation_context, attractor_weight) == 0x48) ? 1 : -1];
typedef char firing_position_evaluation_context_facing_weight_layout[
    (offsetof(firing_position_evaluation_context, facing_weight) == 0x64) ? 1 : -1];
typedef char firing_position_evaluation_context_combat_tests_layout[
    (offsetof(firing_position_evaluation_context, combat_tests) == 0x2fc) ? 1 : -1];
typedef char firing_position_evaluation_context_estimate_direction_layout[
    (offsetof(firing_position_evaluation_context, estimate_direction) == 0x700) ? 1 : -1];
typedef char firing_position_evaluation_context_hide_distance_layout[
    (offsetof(firing_position_evaluation_context, hide_distance) == 0x71c) ? 1 : -1];
typedef char firing_position_evaluation_context_target_object_index_layout[
    (offsetof(firing_position_evaluation_context, target_object_index) == 0x748) ? 1 : -1];
typedef char firing_position_evaluation_context_target_prop_index_layout[
    (offsetof(firing_position_evaluation_context, target_prop_index) == 0x764) ? 1 : -1];
typedef char firing_position_evaluation_context_score_layout[
    (offsetof(firing_position_evaluation_context, score) == 0x780) ? 1 : -1];
typedef char firing_position_evaluation_context_include_current_position_layout[
    (offsetof(firing_position_evaluation_context, include_current_position) == 0x792) ? 1 : -1];
typedef char firing_position_evaluation_context_preferred_flags_layout[
    (offsetof(firing_position_evaluation_context, preferred_flags) == 0x7be) ? 1 : -1];
typedef char firing_position_evaluation_context_guard_plane_layout[
    (offsetof(firing_position_evaluation_context, guard_plane) == 0x7c4) ? 1 : -1];
typedef char firing_position_evaluation_context_facing_threshold_layout[
    (offsetof(firing_position_evaluation_context, facing_threshold) == 0x7d4) ? 1 : -1];
typedef char firing_position_evaluation_context_area_flags_layout[
    (offsetof(firing_position_evaluation_context, area_flags) == 0x7da) ? 1 : -1];
typedef char firing_position_evaluation_context_standing_layout[
    (offsetof(firing_position_evaluation_context, standing) == 0x7dc) ? 1 : -1];
typedef char firing_position_evaluation_context_cornering_markers_layout[
    (offsetof(firing_position_evaluation_context, cornering_markers) == 0x8e0) ? 1 : -1];

#endif
