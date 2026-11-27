// ailibxbox/actor_firing_position.obj: from the records 0xb8238 0xc5078 0xcb690 0xcb988 0xcc190 0x11ac08 0x11ad78 0x11ae50 0x11bbc8 0x11bde8 0x11bf10 0x11c130 0x11c330 0x11c3c0 0x11c3d0 0x11c3e0 0x11d998 0x11e740 0x5160a8 0x54fe00 0x55e3b8 0x55ecb8 0x55f030 0x55f160 0x55f1b8 0x55f288 0x55f5f8 0x55f7e0 0x55f920 0x55fc38 0x55fcb0 0x55fe30 0x5603f0 0x560940 0x560ea0 0x560fd0 0x5611c0 0x561330 0x5613e8 0x561490 0x561548 0x561630 0x5616c0 0x561748 0x561800 0x5618e8 0x561b00 0x561bb8 0x561cd0 0x5621a0 0x5622d8 0x562330 0x562410 0x562508 0x562630 0x5626b0
// @compiler xdk6130
// @flags /TP /O2 /Oc
// @address 0xb8238 ?actor_check_firing_position@@YAXJ@Z
// @address 0x5611c0 ?post_evaluator_hide@@YA_NJPAUfiring_position_evaluation_context@@PAUfiring_position@@@Z
// @address 0xc5078 firing_position_ref_get_definition
// @address 0x11c3c0 firing_position_ref_hash_function
// @address 0x562630 firing_position_get_owner


#include "ai/squad_data.h"
#include "cseries/long_sort.h"
#include "ai/path_state_types.h"
#include "ai/character_behavior_properties.h"
#include "ai/character_weapon_properties.h"
#include "ai/actor_type_flood_types.h"
#include "objects/object_reference_types.h"
#include "ai/actor_firing_position.h"
#include "memory/data_types.h"
#include "objects/object_placement.h"
#include "memory/data.h"
#include "memory/datum_index.h"
#include "math/real_math_types.h"
#include "math/real_math.h"
#include "math/matrix_math_types.h"
#include "math/random_math_range.h"
#include "ai/sector_ref.h"
#include "objects/object_datum.h"
#include "ai/ai_point3d.h"
#include "game/game_time_types.h"
#include <float.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include "ai/actor_datum.h"
#include "ai/actors.h"
#include "motor/motor_animation.h"
#include "objects/objects.h"
#include "ai/actor_dynamic_firing_position.h"
#include "ai/actor_perception.h"
#include "ai/ai.h"
#include "ai/behavior.h"
#include "ai/objectives.h"
#include "ai/path.h"
#include "ai/path_obstacles.h"
#include "ai/props.h"
#include "ai/sector.h"
#include "tag_files/tag_types.h"
#include "ai/firing_position_definition.h"
#include "ai/sector_analysis.h"
#include "ai/squads.h"
#include "memory/hashtable.h"
#include "memory/hashtable_types.h"
#include "memory/tls_data_arrays.h"
#include "memory/tls_globals.h"
#include "ai/actor_globals.h"
#include "ai/Zone.h"
#include "ai/squad_leader.h"
#include "ai/path_state.h"
#include "ai/path_obstacle_types.h"
#include "ai/pathfinding_collision.h"
#include "ai/prop_datum.h"
#include "ai/clump_datum.h"
#include "game/player_types.h"
#include "ai/actor_type_pureform.h"
#include "ai/character_definitions.h"
#include "ai/pathfinding_utilities.h"
#include "ai/combat_range.h"
#include "ai/pathfinding_geometry.h"
#include "ai/firing_position_ref.h"
#include "ai/squad_types.h"
#include "ai/ai_player.h"
#include "ai/task_actor_iterator_types.h"
#include "ai/pathfinding_mode_types.h"
#include "ai/pathfinding_sector_types.h"
#include "game/game_types.h"
#include "ai/ai_scenario_definitions.h"
#include "cache/cache_files.h"
#include "game/game_allegiance.h"
#include "ai/special_movement_types.h"
#include "ai/ai_reference_frame.h"
#include "scenario/scenario_tag.h"
#include "scenario/scenario.h"
#include "ai/pureform_types.h"
#include "ai/prop_ref_datum.h"
#include "ai/prop_state.h"
#include "ai/pathfinding_data.h"
#include "ai/objective_definitions.h"
#include "scenario/scenario_structure_bsp_reference.h"
#include "ai/actor_firing_position_hash.h"
#include "ai/actor_firing_position_queries.h"
#include "ai/firing_position_ref_inline.h"
#include "ai/character_properties_types.h"
#include "ai/path_result.h"
#include "ai/actor_destination.h"
#include "math/real_math_constants.h"
#include "math/real_math_inline_normalize.h"
#include "ai/actor_firing_position_evaluation_inline.h"
#include "ai/squad_area_iterator.h"
#include "units/unit_types.h"
#include "structures/structure_bsp_tag.h"
#include "ai/path_types.h"
#include "objects/object_velocity_queries.h"
#include "tag_files/string_ids.h"

struct s_firing_position_pre_evaluator {
 short mask;
 void (*function)(long actor_index, firing_position_evaluation_context *context, short count, firing_position *positions);
};
struct s_firing_position_post_evaluator {
 short mask;
 bool (*function)(long actor_index, firing_position_evaluation_context *context, firing_position *position);
};
struct relocate_unit_index { unsigned long value; };

// Complete 0x584-byte dynamic firing set, shared by both lookup paths.
typedef char dynamic_firing_set_size[(sizeof(dynamic_firing_set_datum) == 0x584) ? 1 : -1];
typedef char dynamic_firing_set_points_offset[(offsetof(dynamic_firing_set_datum, points) == 0x44) ? 1 : -1];

firing_position_ref FP_REF_NONE = { -1, -1 };
firing_position_ref FP_REF_CURRENT = { -2, -1 };
// @address 0x8c4956 g_current_firing_position_valid
bool g_current_firing_position_valid;
// @address 0x8a1e84 ?global_current_firing_position_definition@@3Ufiring_position_definition@@A
firing_position_definition global_current_firing_position_definition;
// @address 0x8c4958 ?global_temporary_sort_firing_position_count@@3FA
short global_temporary_sort_firing_position_count;
// @address 0x8c495c ?global_temporary_sort_firing_position_array@@3PBUfiring_position@@B
const firing_position *global_temporary_sort_firing_position_array;
/* Takes a two-short handle by value and returns its first short's low byte scaled by 4. */
struct s_firing_position_hash_search_result { int *ptr; };

static __forceinline actor_datum *actor_firing_position_check_get(long actor_index)
{
 return &((actor_datum *)actor_data->data)[datum_index_to_absolute_index(actor_index)];
}
static __forceinline bool actor_firing_position_path_executing(long actor_index)
{
 actor_datum *actor = actor_firing_position_check_get(actor_index);
 bool result = false;
 if (actor->path.valid && actor->path_state == 1) result = true;
 return result;
}
static __forceinline bool actor_firing_position_path_is_current(long actor_index)
{
 actor_datum *actor = actor_firing_position_check_get(actor_index);
 bool result = false;
 if (actor->destination_orders.type == 3)
  result = actor->destination_orders.firing_position_index == actor->firing_position;
 return result;
}
void actor_check_firing_position(long actor_index)
{
 actor_datum *actor = actor_firing_position_check_get(actor_index);
 if (!fp_ref_equals(actor->firing_position_reference, FP_REF_NONE) && !actor_at_firing_position(actor_index) &&
     (!actor_firing_position_path_executing(actor_index) || !actor_firing_position_path_is_current(actor_index))) {
  actor_discard_firing_position(actor_index, actor->firing_position_reference, false);
  actor_set_current_firing_position(actor_index, FP_REF_NONE.value);
 }
}

firing_position_definition *firing_position_ref_get_definition(firing_position_ref handle)
{
  firing_position_definition *result = 0;
  if (!fp_ref_equals(handle, FP_REF_NONE)) {
    if ((handle.block & 0x8000) && !fp_ref_equals(handle, FP_REF_CURRENT)) {
      s_data_array *data = dynamic_firing_set_data;
      long block_index = handle.block & 0x7fff;
      long index = handle.index;
      if (block_index != -1 && block_index >= 0 && block_index < data->first_unallocated) {
        dynamic_firing_set_datum *block = (dynamic_firing_set_datum *)(data->data + data->size * block_index);
        if (block->identifier != 0 && index >= 0 && index < block->position_count &&
            ((1 << (index & 31)) & block->initialized_points.get_bits_direct()[index >> 5]))
          result = &block->points[index];
      }
    } else if (fp_ref_equals(handle, FP_REF_CURRENT)) {
      if (g_current_firing_position_valid)
        result = &global_current_firing_position_definition;
    } else {
      long index = handle.index;
      zone_definition *block = &((zone_definition *)global_scenario->zones.address)[(unsigned short)handle.block];
      if (index >= 0 && index < block->firing_positions.count)
        result = &((firing_position_definition *)block->firing_positions.address)[index];
    }
  }
  return result;
}

bool area_valid_firing_positions(short zone_index, short area_index, firing_position_ref reference) {
 bool result = false;
 if ((reference.block & 0x8000) == 0 && !fp_ref_equals(reference, FP_REF_CURRENT) &&
     zone_index != -1 && area_index != -1 && zone_index == reference.block) {
  zone_definition *zone = &((zone_definition *)global_scenario->zones.address)[(unsigned short)zone_index];
  area_definition *area = &((area_definition *)zone->areas.address)[area_index];
  result = reference.index >= area->first_firing_position &&
           reference.index < area->first_firing_position + area->firing_position_count;
 }
 return result;
}
// @address 0x11c330 hash_table_insert
// takes a node off the free list, stores its key, hash and value, and links it at the head of its bucket


// @address 0x11c130 consider_firing_position
extern "C" bool consider_firing_position(long actor_index, actor_datum *actor, firing_position_ref reference, firing_position_evaluation_context
    *context)
{
    bool tr30;
    tr30 = 1;
    firing_position_definition *definition;
    definition = firing_position_ref_get_definition(reference);
    if (definition != 0 && (short)definition->cluster != -1) {
        if (context != 0) {
            if (context->allow_unreachable == 0 && context->prefer_special == 0 &&
                (definition->sector == NONE ||
                 ((definition->flags & 64) == 0 && (definition->flags & 32) == 0))) {
                tr30 = 0;
            } else {
                tr30 = _rotl((unsigned short)definition->flags, 27) & 1;
                if (context->require_special == 0) {
                    if (context->allow_either != 0) {
                        tr30 = 1;
                    } else {
                        tr30 = tr30 == 0;
                    }
                }
            }
            if ((unsigned int)tr30 != 0 && actor->sector_valid != 0) {
                tr30 = (unsigned char)ai_reference_frames_are_incompatible(actor->pathfinding_point.location[0], (unsigned
                    short)definition->point.location[0]) == 0;
            }
            if ((unsigned int)tr30 != 0) {
                int tr3;
                tr3 = firing_position_get_owner(reference.value);
                if (tr3 != actor_index && tr3 != -1) {
                    actor_datum *owner;
                    owner = &((actor_datum *)actor_data->data)[datum_index_to_absolute_index(tr3)];
                    if (owner->team == actor->team) {
                        real_point3d point;
                        ::ai_point_get_position(&definition->point, &point);
                        float tf0;
                        tf0 = point.x - owner->position.body.x;
                        float tf13;
                        tf13 = point.z - owner->position.body.z;
                        float tf12;
                        tf12 = point.y - owner->position.body.y;
                        float tf11;
                        tf11 = point.x - actor->position.body.x;
                        float tf9;
                        tf9 = point.y - actor->position.body.y;
                        float tf10;
                        tf10 = point.z - actor->position.body.z;
                        tf0 = sqrt(tf12 * tf12 + (tf13 * tf13 + tf0 * tf0));
                        if (tf0 < 1.0f) {
                            tr30 = 0;
                        } else if (tf0 < sqrt(tf9 * tf9 + (tf10 * tf10 + tf11 * tf11)) * 2.0f) {
                            tr30 = 0;
                        }
                    } else {
                        tr30 = 0;
                    }
                }
            }
        }
    } else {
        tr30 = 0;
    }
    return tr30;
}
extern "C" bool hash_table_insert(s_hash_table *table, void *key, void *value)
{
  bool result = false;
  if (table->last != 0) {
    unsigned long hash = table->hash(key);
    s_hash_table_bucket *node = table->last;
    unsigned long index = hash % table->count;
    table->last = node->next;
    node->key = key;
    node->hash = hash;
    memcpy(node + 1, value, table->entry_size);
    node->next = table->buckets[index];
    table->buckets[index] = node;
    result = true;
  }
  return result;
}
// @address 0x11c3d0 ?firing_position_ref_hash_compare@@YA_NPBX0@Z
bool firing_position_ref_hash_compare(const void *a, const void *b)
{
  return a == b;
}
extern "C" int firing_position_ref_hash_function(firing_position_ref ref)
{
  return ((unsigned short)ref.index << 2) & 0x3fc;
}

bool actor_firing_position_discarded(long actor_index, firing_position_ref reference) {
 actor_datum *actor = actor_firing_position_check_get(actor_index);
 bool result = false;
 if (!fp_ref_equals(reference, FP_REF_NONE) && !fp_ref_equals(reference, FP_REF_CURRENT)) {
  for (short index = 0; index < 4; index++) {
   if (fp_ref_equals(reference, actor->firing_positions[index].reference)) { result = true; break; }
  }
 }
 return result;
}
void actor_clear_discarded_firing_positions(long actor_index, bool temporary_only) {
 actor_datum *actor = actor_firing_position_check_get(actor_index);
 actor->last_discarded_firing_position = 3;
 for (short index = 0; index < 4; index++) {
  actor->firing_positions[index].reference = FP_REF_NONE;
 }
}
void actor_discard_firing_position(long actor_index, firing_position_ref reference, bool temporary) {
 if (!fp_ref_equals(reference, FP_REF_NONE)) {
  actor_datum *actor = actor_firing_position_check_get(actor_index);
  actor_firing_position_entry *discarded = actor->firing_positions;
  bool add = true;
  for (short index = 0; index < 4; index++) {
   if (fp_ref_equals(reference, discarded[index].reference)) {
    add = false;
    if (!temporary) discarded[index].temporary = false;
   }
  }
  if (add) {
   actor->last_discarded_firing_position = (short)((actor->last_discarded_firing_position + 1) % 4);
   discarded[actor->last_discarded_firing_position].temporary = temporary;
   discarded[actor->last_discarded_firing_position].reference = reference;
  }
 }
}
static __forceinline actor_datum *firing_position_current_actor_get(long actor_index)
{
    return (actor_datum *)(actor_data->data + (datum_index_to_absolute_index(actor_index)) * sizeof(actor_datum));
}
// @address 0x562508 actor_set_current_firing_position
extern "C" void actor_set_current_firing_position(int actor_index, int fpindex)
{
    actor_datum *actor = firing_position_current_actor_get(actor_index);
    if (!fp_ref_equals(actor->firing_position_reference, FP_REF_NONE))
        ::hash_table_remove((s_hash_table *)g_actor_firing_position_hash, (const void *)(uintptr_t)actor->firing_position);
    if (fpindex != FP_REF_NONE.value) {
        int prev_owner_index[2];
        prev_owner_index[0] = -1;
        s_firing_position_hash_search_result out;
        out.ptr = &prev_owner_index[1];
        s_hash_table *table = g_actor_firing_position_hash;
        char *entry = (char *)((s_hash_table_bucket *)::hash_table_find_internal((s_hash_table *)table, (const void *)(uintptr_t)fpindex));
        if (entry && out.ptr - 1 != 0)
            memcpy(out.ptr - 1, entry + 12, table->entry_size);
        if (prev_owner_index[0] != -1) {
            actor_datum *prev_owner = firing_position_current_actor_get(prev_owner_index[0]);
            prev_owner->firing_position_reference = FP_REF_NONE;
            table = g_actor_firing_position_hash;
            entry = (char *)((s_hash_table_bucket *)::hash_table_find_internal((s_hash_table *)table, (const void *)(uintptr_t)fpindex));
            if (entry)
                memcpy(entry + 12, &actor_index, table->entry_size);
        } else {
            hash_table_insert(g_actor_firing_position_hash, (void *)(uintptr_t)fpindex, (void *)&actor_index);
        }
    }
    actor->firing_position = fpindex;
    actor->discarded_position_valid = 0;
}
int firing_position_get_owner(int fp_ref)
{
  s_hash_table *state = g_actor_firing_position_hash;
  int local[2];
  local[0] = -1;
  int result = -1;
  s_firing_position_hash_search_result out;
  out.ptr = &local[1];
  char *entry = (char *)((s_hash_table_bucket *)::hash_table_find_internal((s_hash_table *)state, (const void *)(uintptr_t)fp_ref));
  if (entry != 0 && out.ptr - 1 != 0) {
    memcpy(out.ptr - 1, entry + 12, state->entry_size);
    result = local[0];
  }
  return result;
}


struct path_state;
static __forceinline void actor_firing_position_direction_construct(c_ai_direction *direction)
{
 memset(direction, 0, sizeof(*direction));
}
static __forceinline void actor_firing_position_action_construct(c_ai_action *action)
{
 actor_firing_position_direction_construct(&action->direction);
 memset(action, 0, sizeof(*action));
}
static __forceinline void actor_firing_position_orders_construct(c_destination_orders *orders)
{
 actor_firing_position_action_construct(&orders->action);
 memset(orders, 0, sizeof(*orders));
}
static __forceinline bool actor_firing_position_perch(c_destination_orders *orders, long actor_index, real_vector3d *normal)
{
 bool result = false;
 if (orders->type != 0) {
  orders->action.type = 0;
  c_path_destination destination;
  if (orders->to_raw(actor_index, &destination, 0)) {
   orders->action.type = 3;
   orders->action.point_valid = true;
   orders->action.point = destination.point;
   orders->action.direction.type = 5;
   orders->action.direction.vector = *normal;
   orders->flags.m_storage |= 8;
  }
 }
 return result;
}
// @address 0x11e740 ?actor_change_firing_position@@YA?AUfiring_position_ref@@JU1@PAUfiring_position@@JPAUpath_state@@_N@Z
firing_position_ref actor_change_firing_position(
    long actor_index, firing_position_ref reference, firing_position *position, long previous_owner, path_state *path, bool move)
{
 actor_datum *actor = &((actor_datum *)actor_data->data)[datum_index_to_absolute_index(actor_index)];
 if (fp_ref_equals(reference, FP_REF_NONE)) {
  actor_move_halt(actor_index);
  actor_set_current_firing_position(actor_index, FP_REF_NONE.value);
 } else {
  if (!fp_ref_equals(actor->firing_position_reference, FP_REF_NONE) && !fp_ref_equals(actor->firing_position_reference, reference))
      actor_discard_firing_position(actor_index, actor->firing_position_reference, true);
  if (previous_owner != -1 && datum_try_and_get(actor_data, previous_owner)) {
   actor_move_halt(previous_owner);
   actor_set_current_firing_position(previous_owner, FP_REF_NONE.value);
  }
  if (!fp_ref_equals(actor->firing_position_reference, reference)) {
   actor_set_current_firing_position(actor_index, reference.value);
   actor->firing_position_locked = path == 0;
  }
  if (move) {
   firing_position_definition *definition = firing_position_ref_get_definition(reference);
   bool result = false;
   if (definition) {
    c_destination_orders orders;
    actor_firing_position_orders_construct(&orders);
    result = orders.to_firing_position(reference);
    firing_position *corner = position;
    if (result && position && corner->cornering != 0) {
     if (orders.corner(actor_index, &corner->cornering_point, &corner->cornering_normal, corner->cornering, corner->cornering_left)) {
      actor->current_firing_position_point = corner->cornering_point;
      actor->discarded_position_valid = true;
     }
    } else if (definition->flags & 32) {
     character_movement_properties *movement = character_definition_movement_properties_get(actor->character_index);
     if (movement && movement->flags & 512) {
      real_vector3d normal;
      if (firing_position_get_normal(definition, &normal)) actor_firing_position_perch(&orders, actor_index, &normal);
     }
    }
    if (result) result = actor_move(actor_index, &orders, path);
   }
   if (!result) actor_set_current_firing_position(actor_index, FP_REF_NONE.value);
  }
 }
 return actor->firing_position_reference;
}

// @rdata 0x82032900
static const float actor_firing_position_leader_score = 20.0f;
static __forceinline actor_datum *actor_firing_position_leader_datum_get(s_data_array *array, long index)
{
    return (actor_datum *)(array->data + (datum_index_to_absolute_index(index)) * sizeof(actor_datum));
}
static __forceinline real_vector2d *actor_firing_position_leader_vector_from_points(
    const real_point3d *a, const real_point3d *b, real_vector2d *v)
{
    v->i = b->x - a->x;
    v->j = b->y - a->y;
    return v;
}
static __forceinline float actor_firing_position_leader_magnitude_squared(const real_vector2d *v) { return v->i * v->i + v->j * v->j; }
static __forceinline float actor_firing_position_leader_distance(const real_point3d *a, const real_point3d *b)
{
    real_vector2d v;
    actor_firing_position_leader_vector_from_points(a, b, &v);
    return sqrtf(actor_firing_position_leader_magnitude_squared(&v));
}
static __forceinline void actor_firing_position_leader_evaluate(firing_position *position, float value, short type)
{
    if (type == 0)
        position->penalty += value;
    else if (type == 1)
        position->secondary_score -= value;
}
// @address 0x55f030 ?pre_evaluator_leader_dist@@YAXJPAUfiring_position_evaluation_context@@FPAUfiring_position@@@Z
void pre_evaluator_leader_dist(long actor_index, firing_position_evaluation_context *context, short count, firing_position *positions)
{
 actor_datum *actor = actor_firing_position_leader_datum_get(actor_data, actor_index);
 character_general_properties *properties =
     (character_general_properties *)character_definition_general_properties_get(actor->character_index);
 if (actor->root_vehicle_index == -1 && (!properties || !(properties->flags & 256)) && (context->standing == 1 || context->standing == 2)) {
#if defined(_XBOX) && !defined(__clang__)
  // match: 0x55f030: the 8.0f default and the k_real_zero_storage compare share one r31 base (+0x3f64 and +0); a literal takes its own lis and frees r31
  float leader_distance = (&k_real_zero_storage)[4057];
#else
  float leader_distance = 8.0f;
#endif
  long leader_index = squad_get_leader((short)actor->squad_index);
  if (properties && properties->leader_distance_override > k_real_zero_storage) leader_distance = properties->leader_distance_override;
  real_point3d destination;
  actor_get_destination(leader_index, &destination);
  short index = 0;
  firing_position *position;
  float distance;
  for (position = positions; index < count; index++, position++) {
   if (position->valid) {
    distance = actor_firing_position_leader_distance(&position->position, &destination);
    if (distance > leader_distance) actor_firing_position_leader_evaluate(position, actor_firing_position_leader_score, 1);
   }
  }
 }
}
// @address 0x55f160 ?pre_evaluator_cornering@@YAXJPAUfiring_position_evaluation_context@@FPAUfiring_position@@@Z
void pre_evaluator_cornering(long actor_index, firing_position_evaluation_context *context, short count, firing_position *positions)
{
  if (context->evaluate_cornering == 0) return;
  if (count <= 0) return;
  for (short index = 0; index < count; index++, positions++) {
    if (positions->cornering != 0) positions->score += 2.5f;
  }
}
static __forceinline void actor_firing_position_add_evaluation(
    firing_position_evaluation_context *context, firing_position *position, float evaluation, short type, bool negative)
{
 if(!negative) position->score+=evaluation;
}
static __forceinline void actor_firing_position_uncover_add_evaluation(
    firing_position_evaluation_context *context, firing_position *position, float evaluation, short type, bool negative)
{
 if (!negative) {
  float &score = position->score;
  score = evaluation + score;
 }
}
// @address 0x55f1b8 ?pre_evaluator_uncover@@YAXJPAUfiring_position_evaluation_context@@FPAUfiring_position@@@Z
void pre_evaluator_uncover(long actor_index, firing_position_evaluation_context *context, short count, firing_position *positions)
{
  firing_position *position = positions;
  for (short index = 0; index < count; index++, position++) {
    if (position->valid) {
      if (position->evaluation_distance < (float)context->minimum_distance || position->distance < (float)context->minimum_distance) continue;
      float evaluation = 0.0f;
      if ((float)context->maximum_distance <= 0.0f || position->evaluation_distance < (float)context->maximum_distance) {
        evaluation = 20.0f;
      } else {
        float distance = (float)context->maximum_distance + 20.0f - position->evaluation_distance;
        if (distance > 0.0f) evaluation = distance;
      }
      actor_firing_position_uncover_add_evaluation(context, position, evaluation, 4, false);
    }
  }
}
// @address 0x55f5f8 ?pre_evaluator_hint_plane@@YAXJPAUfiring_position_evaluation_context@@FPAUfiring_position@@@Z
// match: noalias declaration preserves caller live registers
static __forceinline float actor_firing_position_hint_dot(const real_vector3d *a, const real_vector3d *b) {
 return a->i * b->i + a->j * b->j + a->k * b->k;
}
void pre_evaluator_hint_plane(long actor_index, firing_position_evaluation_context *context, short count, firing_position *positions)
{
 firing_position_evaluation_context *hint_context = context;
 if (hint_context->evaluate_danger && hint_context->hint) {
  firing_position *position = positions;
  for (short index = 0; index < count; index++, position++) {
   if (position->valid) {
    bool negative = hint_context->negative;
    real_vector3d direction;
    if (!negative && hint_context->has_firing_direction) direction = position->firing_direction;
    else direction = position->second;
    direction.k = 0.0f;
    normalize3d(&direction);
    float alignment = actor_firing_position_hint_dot(&hint_context->target_hint_vector, &direction);
    alignment = MIN(MAX(alignment, 0.0f), 1.0f);
    float threshold = negative ? 0.5f : 0.8660254f;
    float fraction = alignment / threshold;
    fraction = MIN(MAX(fraction, 0.0f), 1.0f);
    float score = fraction * 2.0f;
    if (alignment > threshold) score += MIN(MAX((alignment - threshold) / (1.0f - threshold), 0.0f), 1.0f) * 8.0f;
    if (negative) position->secondary_score -= score * 2.0f;
    else position->score += score;
   }
  }
 }
}
// Each iteration evaluates one complete firing_position candidate.
// @address 0x55f7e0 ?pre_evaluator_panic@@YAXJPAUfiring_position_evaluation_context@@FPAUfiring_position@@@Z
void pre_evaluator_panic(long actor_index, firing_position_evaluation_context *context, short count, firing_position *positions)
{
    firing_position *position = positions;
    for (short i = 0; i < count; i++, position++) {
        if (position->valid) {
            float panic_score = 0.0f;
            // match: 0x55f7e0: the zero branch keeps the bge-then-b pair and loads target_distance before the fmr of 0.0f
            if (position->target_distance < context->panic_distance) {
                panic_score = 0.0f;
            } else {
                float double_panic_distance = context->panic_distance * 2.0f;
                if (position->target_distance < double_panic_distance) {
                    panic_score = (position->target_distance - context->panic_distance) / context->panic_distance * 8.0f;
                } else if (position->target_distance < context->panic_maximum_distance) {
                    panic_score = (context->panic_maximum_distance - position->target_distance) * 8.0f /
                        (context->panic_maximum_distance - double_panic_distance);
                }
            }
            position->score += panic_score;
            if (position->special && context->standing == 1) {
                position->score += 8.0f;
            }
            if (context->evaluate_danger) {
                float danger_distance_squared = position->danger_distance_squared;
                float danger_score;
                if (danger_distance_squared < context->danger_minimum_distance * context->danger_minimum_distance) {
                    danger_score = 0.0f;
                } else if (danger_distance_squared < context->danger_maximum_distance * context->danger_maximum_distance) {
                    danger_score = (sqrt(danger_distance_squared) - context->danger_minimum_distance) * 10.0f /
                        (context->danger_maximum_distance - context->danger_minimum_distance);
                } else {
                    danger_score = 10.0f;
                }
                position->score += danger_score;
            }
        }
    }
}

// @rdata 0x82026480
static const float actor_firing_position_guard_near_score = 8.0f;
// @rdata 0x8202b100
static const float actor_firing_position_guard_plane_score = 12.0f;
// @rdata 0x82024264
static const float actor_firing_position_guard_concealment_score = 5.0f;
// @rdata 0x82021694
static const float actor_firing_position_guard_maximum = 3.402823466e+38F;
// @rdata 0x82022024
static const float actor_firing_position_guard_minimum = -3.402823466e+38F;
static __forceinline float actor_firing_position_guard_sine(float angle)
{
 return (float)sin((double)angle);
}
static __forceinline float actor_firing_position_guard_cosine(float angle)
{
 return (float)cos((double)angle);
}
static __forceinline float actor_firing_position_guard_dot(const real_point3d *a, const real_vector3d *b)
{
 return (a->k * b->k + a->i * b->i) + a->j * b->j;
}
static __forceinline real_vector3d *actor_firing_position_guard_direction(real_vector3d *vector, float angle)
{
 float j = actor_firing_position_guard_sine(angle);
 float i = actor_firing_position_guard_cosine(angle);
 vector->j = j;
 vector->k = 0.0f;
 vector->i = i;
 return vector;
}
// @address 0x55f920 ?pre_evaluator_guard@@YAXJPAUfiring_position_evaluation_context@@FPAUfiring_position@@@Z
void pre_evaluator_guard(long actor_index, firing_position_evaluation_context *context, short count, firing_position *positions)
{
 short index;
 firing_position *position;
 if (context->panic_maximum_distance > 0.0f) {
  for (index = 0, position = positions; index < count; index++, position++) {
   if (position->valid) {
    float evaluation = actor_firing_position_guard_near_score * (1.0f - position->target_distance / context->panic_maximum_distance);
    evaluation = MAX(evaluation, 0.0f);
    actor_firing_position_add_evaluation(context, position, evaluation, 12, false);
   }
  }
 }
 if (context->facing_check) {
  for (index = 0, position = positions; index < count; index++, position++) {
   if (position->valid) {
    real_vector3d direction;
    actor_firing_position_guard_direction(&direction, position->definition->facing);
    float facing_x = position->second.i;
    float facing_y = position->second.j;
    float facing_z = position->second.k;
    if (facing_x * direction.i + facing_y * direction.j + facing_z * direction.k > context->facing_threshold) {
     position->rejected = true;
     if (!context->debug) position->valid = false;
    }
   }
  }
 }
 if (context->guard) {
  bool found = false;
  float minimum = actor_firing_position_guard_maximum;
  float maximum = actor_firing_position_guard_minimum;
  for (index = 0, position = positions; index < count; index++, position++) {
   if (position->valid) {
    float distance = actor_firing_position_guard_dot(&position->position, &context->guard_plane);
    if (distance < minimum) minimum = distance;
    if (distance > maximum) maximum = distance;
    found = true;
   }
  }
  if (found) {
   float range = maximum - minimum;
   if (range > 0.2f && range < actor_firing_position_guard_maximum) {
    for (index = 0, position = positions; index < count; index++, position++) {
     if (position->valid) {
      float distance = actor_firing_position_guard_dot(&position->position, &context->guard_plane);
      float evaluation = (distance - minimum) / range;
      evaluation = MIN(MAX(evaluation, 0.0f), 1.0f);
      if (!context->guard_negative) evaluation = 1.0f - evaluation;
      evaluation *= actor_firing_position_guard_plane_score;
      actor_firing_position_add_evaluation(context, position, evaluation, 50, false);
     }
    }
   }
  }
 }
 if (context->penalize_unconcealed) {
  for (index = 0, position = positions; index < count; index++, position++) {
   if (position->valid && !position->concealed) position->hide_score -= actor_firing_position_guard_concealment_score;
  }
 }
}

// @address 0x55fc38 ?pre_evaluator_postsearch@@YAXJPAUfiring_position_evaluation_context@@FPAUfiring_position@@@Z
void pre_evaluator_postsearch(long actor_index,firing_position_evaluation_context *context,short count,firing_position *positions) {
 firing_position *position=positions;
 for(short i=0;i<count;i++,position++) {
  if(position->valid) {
   float evaluation=8.0f * (1.0f-position->evaluation_distance/20.0f);
   evaluation=evaluation>0.0f ? evaluation:0.0f;
   actor_firing_position_add_evaluation(context,position,evaluation,12,false);
  }
 }
}
static __forceinline float actor_firing_position_hide_evaluate(const firing_position *position,float hide_distance) {
 float score=0.0f;
 if(position->distance<3.402823466e+38F) {
  float fraction=position->distance/(0.8f*hide_distance);
  score=-8.0f*MIN(MAX(1.0f-fraction,0.0f),1.0f);
 }
 return score;
}
// @address 0x55fcb0 ?pre_evaluator_hide@@YAXJPAUfiring_position_evaluation_context@@FPAUfiring_position@@@Z
void pre_evaluator_hide(long actor_index,firing_position_evaluation_context *context,short count,firing_position *positions) {
 actor_datum *actor=&((actor_datum *)actor_data->data)[datum_index_to_absolute_index(actor_index)];
 if(context->hide && context->hide_distance>0.0f) {
  firing_position *position=positions;
  for(short i=0;i<count;i++,position++) {
   if(position->valid) {
    float score=actor_firing_position_hide_evaluate(position,context->hide_distance);
    position->hide_score=score+position->hide_score;
   }
  }
 }
 if(actor->support_object_index!=-1 && ((s_object_header
     *)object_header_data->data)[datum_index_to_absolute_index(actor->support_object_index)].object_type==3) {
  firing_position *position=positions;
  for(short i=0;i<count;i++,position++) {
   if(position->valid && (position->flags&0x8000) && !fp_ref_equals(position->original_index, FP_REF_CURRENT)) position->penalty+=10.0f;
  }
 }
}
// @address 0x560ea0 ?post_evaluator_flying@@YA_NJPAUfiring_position_evaluation_context@@PAUfiring_position@@@Z
bool post_evaluator_flying(long actor_index, firing_position_evaluation_context *context, firing_position *position)
{
 actor_datum *actor = &((actor_datum *)actor_data->data)[datum_index_to_absolute_index(actor_index)];
 if (context->allow_unreachable ||
     (context->prefer_concealed && (!position || position->concealed)) ||
     (context->prefer_special && (!position || position->reachable))) {
  if (!position) {
   context->score += 15.0f;
  } else if (position->valid) {
   if (path_3d_available(&context->test_position, (const c_ai_point3d *)position->definition,
                         actor->root_vehicle_index, position->original_index,
                         &context->flight_cache)) {
    float evaluation = 15.0f;
    actor_firing_position_add_evaluation(context, position, evaluation, 27, false);
   } else if (actor_firing_position_reject(context, position)) {
    position->valid = false;
   }
  }
 }
 if (!position) return true;
 return position->valid;
}

static __forceinline long actor_firing_position_seconds_to_ticks_round(float seconds)
{
 return (long)(seconds * (float)game_time_globals->ticks_per_second + 0.5f);
}
// @address 0x560fd0
bool post_evaluator_pursuit(long actor_index, firing_position_evaluation_context *context, firing_position *position)
{
 if (position) {
  if (context->danger_maximum) {
   float evaluation;
   switch (position->type) {
   case 0: evaluation = 10.0f; break;
   case 1: evaluation = 5.0f; break;
   default: evaluation = 0.0f; break;
   }
   actor_firing_position_add_evaluation(context, position, evaluation, 8, false);
  } else {
   long time = game_time_globals->time;
   long examined_time = -1;
   short examined_count = 0;
   bool unexamined = true;
   if (position->type == 0 && position->target_distance < 6.0f) {
    examined_time = time;
    examined_count = 7;
    unexamined = false;
   }
   if (context->danger) {
    if (unexamined) {
     float evaluation = 15.0f;
     actor_firing_position_add_evaluation(context, position, evaluation, 8, false);
    }
   } else if (!unexamined) {
    if (actor_firing_position_reject(context, position)) position->valid = false;
   }
   if (position->valid) {
    float evaluation = 0.0f;
    if (examined_time == -1 || examined_time + actor_firing_position_seconds_to_ticks_round(10.0f) < time) {
     evaluation = 10.0f;
    } else if (examined_time < time) {
     evaluation = 10.0f * ((float)(time - examined_time) * game_time_globals->tick_length) / 10.0f;
    }
    actor_firing_position_add_evaluation(context, position, evaluation, 6, false);
    float count_evaluation = 0.0f;
    if (examined_count < 4) count_evaluation = (float)(4 - examined_count) * 5.0f;
    actor_firing_position_add_evaluation(context, position, count_evaluation, 7, false);
   }
  }
 }
 if (!position) return false;
 return position->valid;
}
bool post_evaluator_hide(long actor_index, firing_position_evaluation_context *context, firing_position *position)
{
 bool result = true;
 if (context->hide) {
  if (!position) {
   context->score += 15.0f;
  } else if (position->concealed && context->prefer_special) {
   float evaluation = 0.0f;
   switch (position->type) {
   case 2: evaluation = 15.0f; break;
   case 4: evaluation = 12.0f; break;
   case 3: evaluation = 8.0f; break;
   case 0: case 1: evaluation = 6.0f; break;
   }
   actor_firing_position_add_evaluation(context, position, evaluation, 19, false);
  } else {
   float evaluation = 0.0f;
   switch (position->type) {
   case 2: evaluation = 12.0f; break;
   case 4: evaluation = 10.0f; break;
   case 3: evaluation = 4.0f; break;
   case 1:
    if (position->definition->flags & 16) break;
    // fall through
   case 0:
    if (context->allow_exposed) evaluation = -10.0f;
    else if (actor_firing_position_reject(context, position)) position->valid = false;
    break;
   }
   actor_firing_position_add_evaluation(context, position, evaluation, 19, false);
  }
 }
 if (position) result = position->valid;
 return result;
}

// @address 0x561330 ?post_evaluator_guard@@YA_NJPAUfiring_position_evaluation_context@@PAUfiring_position@@@Z
bool post_evaluator_guard(long actor_index, firing_position_evaluation_context *context, firing_position *position)
{
  if (context->guard && context->guard_valid) {
    if (!position) {
      context->score += 6.0f;
    } else {
      float evaluation = 0.0f;
      switch (position->type) {
      case 0: case 1: evaluation = 0.0f; break;
      case 2: case 4: evaluation = 6.0f; break;
      case 3: evaluation = 3.0f; break;
      default: break;
      }
      actor_firing_position_add_evaluation(context, position, evaluation, 6, false);
    }
  }
  if (!position) return true;
  return position->valid;
}
// @address 0x5613e8 ?post_evaluator_vehicle_cover@@YA_NJPAUfiring_position_evaluation_context@@PAUfiring_position@@@Z
bool post_evaluator_vehicle_cover(long actor_index,firing_position_evaluation_context *context,firing_position *position) {
 if (context->vehicle_cover) {
  if (!position) {
   context->score+=10.0f;
  } else {
   float evaluation=0.0f;
   long type=position->type;
   switch(type) {
   case 1: case 4: evaluation=2.0f; break;
   case 0: evaluation=10.0f; break;
   case 2: case 3: { float unused=0.0f; break; }
   default: break;
   }
   actor_firing_position_add_evaluation(context,position,evaluation,19,false);
  }
 }
 if (!position) return true;
 return position->valid;
}

// @address 0x561490 ?post_evaluator_uncover@@YA_NJPAUfiring_position_evaluation_context@@PAUfiring_position@@@Z
bool post_evaluator_uncover(long actor_index, firing_position_evaluation_context *context, firing_position *position)
{
  bool result = true;
  if (context->uncover) {
    if (!position) {
      context->score += 20.0f;
    } else {
      float evaluation = 0.0f;
      switch (position->type) {
      case 0: evaluation = 20.0f; break;
      case 1: evaluation = 10.0f; break;
      default: {
        float distance = context->hide_distance - 7.5f;
        if (distance < 0.0f || position->danger_distance_squared > distance * distance) {
          if (actor_firing_position_reject(context, position)) position->valid = false;
        }
        break;
      }
      }
      actor_firing_position_add_evaluation(context, position, evaluation, 20, false);
    }
  }
  if (position) result = position->valid;
  return result;
}
// @address 0x561548 ?post_evaluator_attack@@YA_NJPAUfiring_position_evaluation_context@@PAUfiring_position@@@Z
bool post_evaluator_attack(long actor_index, firing_position_evaluation_context *context, firing_position *position)
{
  bool result = true;
  if (context->attack) {
    if (!position) {
      context->score += context->special ? 6.0f : 15.0f;
    } else {
      float evaluation = 0.0f;
      switch (position->type) {
      case 0: evaluation = context->special ? 6.0f : 15.0f; break;
      case 1: evaluation = context->special ? 2.5f : 5.0f; break;
      default:
        if (!context->special && actor_firing_position_reject(context, position)) position->valid = false;
        break;
      }
      actor_firing_position_add_evaluation(context, position, evaluation, 15, false);
    }
  }
  if (position) result = position->valid;
  return result;
}
// @rdata 0x820225f0..0x820226a0
static const s_firing_position_pre_evaluator g_firing_position_pre_evaluators[] = {
 { -1, pre_evaluator_global }, { -1, pre_evaluator_obstacles },
 { 0x01, pre_evaluator_attack }, { 0x4d, pre_evaluator_combatmove },
 { 0x10, pre_evaluator_guard }, { 0x100, pre_evaluator_postsearch },
 { 0x82, pre_evaluator_panic }, { 0x20, pre_evaluator_pursuit },
 { 0x08, pre_evaluator_uncover }, { 0x06, pre_evaluator_hide },
 { 0x1d, pre_evaluator_cornering }, { 0x01, pre_evaluator_hint_plane },
 { 0x119, pre_evaluator_leader_dist }, { 0, 0 }
};
static const s_firing_position_post_evaluator g_firing_position_post_evaluators[] = {
 { 0x41, post_evaluator_attack }, { 0x08, post_evaluator_uncover },
 { 0x06, post_evaluator_hide }, { 0x10, post_evaluator_guard },
 { 0x20, post_evaluator_pursuit }, { 0x80, post_evaluator_vehicle_cover },
 { -1, post_evaluator_flying }, { 0, 0 }
};
// @rdata-end
// @address 0x561630 ?firing_position_pre_evaluate@@YAXJPAUfiring_position_evaluation_context@@FPAUfiring_position@@@Z
void firing_position_pre_evaluate(long actor_index, firing_position_evaluation_context *context, short count, firing_position *positions)
{
  for (const s_firing_position_pre_evaluator *evaluator = g_firing_position_pre_evaluators; evaluator->function; evaluator++) {
    if ((1 << context->type) & evaluator->mask) evaluator->function(actor_index, context, count, positions);
  }
}
// @address 0x5616c0 ?firing_position_post_evaluate@@YA_NJPAUfiring_position_evaluation_context@@PAUfiring_position@@@Z
bool firing_position_post_evaluate(long actor_index, firing_position_evaluation_context *context, firing_position *position)
{
  bool result = true;
  const s_firing_position_post_evaluator *evaluator = g_firing_position_post_evaluators;
  do {
    if (!evaluator->function) break;
    if ((1 << context->type) & evaluator->mask) result = evaluator->function(actor_index, context, position);
    evaluator++;
  } while (result);
  return result;
}

bool firing_position_forced_evaluation(long actor_index, firing_position_evaluation_context *context, firing_position *position)
{
 position->penalty = 0.0f;
 position->preferred = false;
 position->hide_score = 0.0f;
 position->valid = true;
 position->evaluated_penalty = 0.0f;
 position->forced = false;
 position->evaluated_hide_score = 0.0f;
 if (!context->allow_unreachable && !position->definition->sector_ref.valid()) {
  position->preferred = true;
  position->valid = false;
 }
 if (position->valid) {
  firing_position_pre_evaluate(actor_index, context, 1, position);
  if (position->valid) {
   if (context->evaluate_danger)
    firing_position_compute_line_of_sight(actor_index, context, position);
   position->evaluated_penalty = position->penalty;
   position->evaluated_hide_score = position->hide_score;
   position->valid = firing_position_post_evaluate(actor_index, context, position);
  }
 }
 return position->valid;
}
// @address 0x561748 ?firing_position_compare@@YA_NJJPAX@Z
bool firing_position_compare(long index1,long index2,void *context) {
 const firing_position *position1=&global_temporary_sort_firing_position_array[index1];
 const firing_position *position2=&global_temporary_sort_firing_position_array[index2];
 if(position1->valid != position2->valid)
  return (position1->valid ? -1 : 1)>0;
 if(position1->preferred != position2->preferred)
  return (position1->preferred ? 1 : -1)>0;
 float score1=position1->base_score+position1->secondary_score;
 float score2=position2->base_score+position2->secondary_score;
 if(score1>score2) return false;
 else if(score1<score2) return true;
 return false;
}

static __forceinline real_vector3d *actor_firing_position_vector_from_points(const real_point3d *from, const real_point3d *to, real_vector3d
    *vector) {
 vector->i = to->x - from->x;
 vector->j = to->y - from->y;
 vector->k = to->z - from->z;
 return vector;
}
static __forceinline float actor_firing_position_magnitude_squared(const real_vector3d *vector) {
 return vector->i * vector->i + vector->j * vector->j + vector->k * vector->k;
}
// @address 0x5618e8 ?actor_nearby_firing_positions@@YA_NJPAVc_ai_point3d@@Vc_sector_ref@@MM@Z
static __forceinline bool firing_flight_path_3d_available(const real_point3d *start, const c_ai_point3d *end, long unit)
{
 real_point3d position;
 bool result = false;
 ::ai_point_get_position(end, &position);
 if (flight_test_points(start, &position, unit, -1)) result = true;
 return result;
}
bool actor_nearby_firing_positions(long actor_index, c_ai_point3d *point, c_sector_ref sector, float radius, float path_radius)
{
 c_sector_ref *home_sector = &sector;
 actor_datum *actor = datum_get<actor_datum>(actor_data, actor_index);
 bool found = false;
 if (actor->squad_index != NONE) {
  c_path_state path;
  squad_area_iterator iterator;
  long form = actor_pureform_get_firing_point_selection_form((int)actor_index);
  bool mode = actor->goal_status == 3;
  short area_type = actor_get_area_type(actor_index);
  squad_area_iterator_new(&iterator, (short)actor->squad_index, actor_get_area_flags(actor_index), area_type, mode, (e_pureform)form);
  const float lift = 0.1f;
  while (!found && squad_area_iterator_next(&iterator) != -1) {
   zone_definition *zone = &((zone_definition *)global_scenario->zones.address)[(unsigned short)iterator.zone_index];
   area_definition *area = &((area_definition *)zone->areas.address)[iterator.area_index];
   for (short index = area->first_firing_position; index < area->first_firing_position + area->firing_position_count; index++) {
    if (index >= 0 && index < zone->firing_positions.count) {
     firing_position_definition *position = &((firing_position_definition *)zone->firing_positions.address)[index];
     if (::ai_distance_squared3d(point, &position->point) < radius * radius) {
      if (actor->use_object) {
       real_point3d start;
       ::ai_point_get_position(point, &start);
       start.z += lift;
       if (firing_flight_path_3d_available(&start, &position->point, actor->unit_index)) {
        found = true;
        break;
       }
      } else {
       found = true;
       break;
      }
     }
    }
   }
  }

 }
 return found;
}
// @address 0x561b00 ?path_distance_to_target_flying_point@@YAXPAUfiring_position@@PAUfiring_position_evaluation_context@@@Z
void path_distance_to_target_flying_point(firing_position *position, firing_position_evaluation_context *context)
{
 real_vector3d vector;
 actor_firing_position_vector_from_points(&context->target_position, &position->position, &vector);
 if (!(actor_firing_position_magnitude_squared(&vector) >= 400.0f)) {
  position->evaluation_distance = normalize3d(&vector);
  if (context->has_firing_direction != 0) position->firing_direction = vector;
 }
}
// @address 0xcb690 actor_get_area_flags
extern "C" short actor_get_area_flags(long actor_index)
{
 actor_datum *actor=&((actor_datum *)actor_data->data)[datum_index_to_absolute_index(actor_index)];
 short standing=::actor_get_standing(actor_index);
 short flags=0;
 flags|=4;
 switch(standing) {
 case 0: case 1: {
   unsigned char flying=actor->flying;
   if(!flying) {
     character_movement_properties *movement=character_definition_movement_properties_get(actor->character_index);
     if(movement && (movement->flags & 0x1000)) flying=1;
   }
   flags|=1;
   if(flying) flags|=2; else flags&=~2;
   break;
 }
 }
 return flags;
}


// @address 0xcb988 actor_get_area_type
extern "C" short actor_get_area_type(long actor_index)
{
 actor_datum *actor = &((actor_datum *)actor_data->data)[datum_index_to_absolute_index(actor_index)];
 if (actor->root_vehicle_index != -1) return 1;
 if (actor->deployed_turret != 0 && actor_executing_layer(actor_index, 196) != -1) return 2;
 return 0;
}
// @address 0x562410 actor_at_firing_position
extern "C" bool actor_at_firing_position(long actor_index)
{
 bool at_position = false;
 actor_datum *actor = &((actor_datum *)actor_data->data)[datum_index_to_absolute_index(actor_index)];
 if (!fp_ref_equals(actor->firing_position_reference, FP_REF_NONE)) {
  firing_position_definition *definition = firing_position_ref_get_definition(actor->firing_position_reference);
  if (definition) {
   float tolerance = ::actor_destination_tolerance(actor_index) + 0.25f;
   const c_ai_point3d *point = actor->discarded_position_valid ? &actor->current_firing_position_point : (c_ai_point3d *)definition;
   real_point3d origin;
   real_point3d position;
   object_get_origin(actor->unit_index, &origin);
   ::ai_point_get_position(point, &position);
   at_position = distance_squared2d(&origin.xy, &position.xy) < tolerance * tolerance;
  }
 }
 return at_position;
}


// @address 0x8a1ecc ?g_active_structure_bsp_mask@@3KA
// @address 0x11bf10 ?area_get_available_firing_positions@@YAFJJJPAUfiring_position@@PAFJPAUfiring_position_evaluation_context@@FF@Z
short area_get_available_firing_positions(long actor_index, long zone_index, long area_index, firing_position *positions,
    short *position_count, long max_positions, firing_position_evaluation_context *context, short type, short flags)
{
 actor_datum *actor = &((actor_datum *)actor_data->data)[datum_index_to_absolute_index(actor_index)];
 if (zone_index != -1 && area_index != -1) {
  zone_definition *zone = &((zone_definition *)global_scenario->zones.address)[datum_index_to_absolute_index(zone_index)];
  if (((short)g_active_structure_bsp_mask & (1 << zone->structure_bsp_index)) != 0) {
   area_definition *area = &((area_definition *)zone->areas.address)[area_index];
   if ((actor->root_vehicle_index != -1) == ((area->flags & 1) != 0)) {
    // every caller passes max_positions = 512
    for (short index = area->first_firing_position; *position_count < 512 && index < area->first_firing_position + area->firing_position_count;
        index++) {
     firing_position_ref reference;
     reference.index = index;
     reference.block = (short)zone_index;
     if (consider_firing_position(actor_index, actor, reference, context) &&
         new_firing_position(actor_index, reference, &positions[*position_count], context))
     {
      switch (type) {
      case 1:
       positions[*position_count].primary = true;
       break;
      case 2:
       positions[*position_count].special = true;
       break;
      }
      short index = (*position_count)++;
      firing_position *position = &positions[index];
      positions[index].enabled = (flags & 1) != 0;
      positions[index].concealed = (position->definition->flags & 0x20) != 0;
      positions[index].reachable = (position->definition->flags & 0x40) == 0;
     }
    }
    if (context && context->prefer_special)
     flight_evaluation_cache_add_area(&context->flight_cache, (short)zone_index, (short)area_index);
   }
  }
 }
 return *position_count;
}
// @address 0x561bb8 ?actor_get_dynamic_firing_positions@@YA_NJPAUfiring_position@@PAFJPAUfiring_position_evaluation_context@@@Z
bool actor_get_dynamic_firing_positions(
    long actor_index, firing_position *positions, short *position_count, long max_positions, firing_position_evaluation_context *context)
{
 bool result = false;
 actor_datum *actor = &((actor_datum *)actor_data->data)[datum_index_to_absolute_index(actor_index)];
 long dynamic_set_index = actor->dynamic_firing_set_index;
 dynamic_firing_set_datum *set = &((dynamic_firing_set_datum *)dynamic_firing_set_data->data)[datum_index_to_absolute_index(dynamic_set_index)];
 for (short index = 0; index < set->position_count && *position_count < 512; index++) {
  firing_position_ref reference;
  reference.index = index;
  reference.block = (short)(dynamic_set_index | 0x8000);
  if (consider_firing_position(actor_index, actor, reference, context) &&
      new_firing_position(actor_index, reference, &positions[*position_count], context))
  {
   *position_count = (short)(*position_count + 1);
   result = true;
  }
 }
 return result;
}

static __forceinline bool actor_firing_position_location_valid(const s_cluster_reference *location)
{
 return (unsigned char)location->bsp_index != 255;
}
// @address 0x5626b0 ?firing_position_setup_current_definition@@YA_NJ@Z
bool firing_position_setup_current_definition(long actor_index)
{
 bool result = false;
 actor_datum *actor = &((actor_datum *)actor_data->data)[datum_index_to_absolute_index(actor_index)];
 s_cluster_reference *location = &actor->position.location.cluster_reference;
 firing_position_definition *definition = &global_current_firing_position_definition;
 if (actor_firing_position_location_valid(&actor->position.location.cluster_reference)) {
  actor_find_pathfinding_location(actor_index);
  if (actor->pathfinding_sector.valid()) {
   definition->area = -1;
   definition->sector = actor->pathfinding_sector;
   definition->point = actor->pathfinding_point;
   definition->normal.yaw = 0.0f;
   definition->normal.pitch = 0.0f;
   definition->cluster = location->bsp_index == definition->sector.structure_bsp_index ? location->cluster_index : -1;
   definition->facing = 0.0f;
   definition->flags = 0;
   definition->unknown = 0;
   definition->flags |= 64;
   g_current_firing_position_valid = true;
   result = true;
  }
 }
 return result;
}

static __forceinline actor_datum *firing_position_valid_actor_get(long actor_index)
{
    return (actor_datum *)(actor_data->data + (datum_index_to_absolute_index(actor_index)) * 2700);
}
// @address 0x5160a8 firing_position_valid
extern "C" void firing_position_valid(long actor_index, firing_position_ref reference)
{
 actor_datum *actor = firing_position_valid_actor_get(actor_index);
 long squad = actor->squad_index;
 if (squad != -1) {
  bool pending = actor->flying && !actor->searching;
  bool special = actor->goal_status == 2;
  if (special || pending) {
   bool valid = false;
   bool flag = false;
   short mode = actor_get_area_type(actor_index);
   squad_area_iterator iterator;
   squad_area_iterator_new(&iterator, (short)squad, actor_get_area_flags(actor_index), mode, false, _pureform_none);
   while (squad_area_iterator_next(&iterator) != -1) {
    if (area_valid_firing_positions(iterator.zone_index, iterator.area_index, reference)) {
     if (iterator.iterator_type == 2) {
      squads_area_zone_set_entry *entry = &((squads_area_zone_set_entry *)iterator.zone_set->address)[iterator.zone_set_index];
      flag = (entry->flags & 1) != 0;
      valid = entry->type == 1;
     }
     break;
    }
   }
   if (pending && !valid) actor->flying = false;
   if (special) actor->goal_area_valid = flag;
  }
 }
}

// ---- 0x11bde8 squad_get_available_firing_positions ----

static __forceinline real_vector3d *actor_firing_position_combat_vector_from_points3d(const real_point3d *a, const real_point3d *b,
    real_vector3d *result)
{
 result->i = b->x - a->x;
 result->j = b->y - a->y;
 result->k = b->z - a->z;
 return result;
}
static __forceinline float actor_firing_position_combat_magnitude_squared3d(const real_vector3d *vector)
{
 return vector->i * vector->i + vector->j * vector->j + vector->k * vector->k;
}
static __forceinline float actor_firing_position_combat_distance_squared3d(const real_point3d *a, const real_point3d *b)
{
 real_vector3d vector;
 actor_firing_position_combat_vector_from_points3d(a, b, &vector);
 return actor_firing_position_combat_magnitude_squared3d(&vector);
}
// @address 0x5603f0 ?pre_evaluator_combatmove@@YAXJPAUfiring_position_evaluation_context@@FPAUfiring_position@@@Z
void pre_evaluator_combatmove(long actor_index, firing_position_evaluation_context *context, short count, firing_position *positions)
{
 actor_datum *actor = datum_get<actor_datum>(actor_data, actor_index);
 bool following = false;
 if (context->combat_disabled) return;
 float proximity_weight = 12.0f;
 if (context->evaluate_danger) {
  character_weapon_properties *weapon = actor_weapon_properties_get(actor_index);
  if (weapon) {
   if (context->hide_distance > 1.5f * weapon->maximum_range) proximity_weight = 0.0f;
   else if (context->hide_distance > weapon->maximum_range)
    proximity_weight *= 1.0f - (context->hide_distance - weapon->maximum_range) / 0.5f * weapon->maximum_range;
  }
  proximity_weight = MAX(proximity_weight, 4.0f);
 }
 if (proximity_weight > 0.0f) {
  firing_position *position = positions;
  for (short index = 0; index < count; index++, position++) {
   if (position->valid && position->target_distance < context->panic_maximum_distance) {
    float fraction = position->target_distance / context->panic_maximum_distance;
    float normalized = 1.0f - fraction;
    if (normalized < 0.0f) normalized = 0.0f;
    float evaluation = proximity_weight * normalized;
    actor_firing_position_add_evaluation(context, position, evaluation, 17, false);
   }
  }
 }
 if (actor->squad_index != -1) {
  squad_datum *squad = datum_get<squad_datum>(squad_data, actor->squad_index);
  if (::squad_valid_task(squad) &&
      ((s_task *)((s_objective *)global_scenario->objectives.address)[squad->objective_index].tasks.address)[squad->task_index].follow_mode !=
          0 &&
      squad->follow_object_index != -1)
  {
   unit_datum *object = (unit_datum *)datum_get<s_object_header>(object_header_data, squad->follow_object_index)->datum;
   if (!game_team_is_enemy((e_game_team)actor->team, (e_game_team)object->unit.team)) {
    float minimum_squared, maximum_squared;
    actor_get_desired_follow_range(actor_index, squad->follow_object_index, &minimum_squared, &maximum_squared);
    float minimum = square_root(minimum_squared);
    float maximum = square_root(maximum_squared);
    real_point3d origin;
    object_get_origin(squad->follow_object_index, &origin);
    firing_position *position = positions;
    for (short index = 0; index < count; index++, position++) {
     if (position->valid) {
      float evaluation = 0.0f;
      float distance = distance3d(&origin, &position->position);
      if (distance < maximum) {
       evaluation = 12.0f;
       if (distance > minimum) evaluation *= 1.0f - (distance - minimum) / (maximum - minimum);
      }
      actor_firing_position_add_evaluation(context, position, evaluation, 41, false);
     }
    }
    following = true;
   }
  }
 }
 if (context->combat_position_count > 0) {
  firing_position *position = positions;
  for (short index = 0; index < count; index++, position++) {
   short primary_blockage = 0;
   short secondary_blockage = 0;
   short reverse_blockage = 0;
   if (position->valid) {
    float minimum_distance = 3.402823466e+38f;
    for (short test_index = 0; test_index < context->combat_test_count; test_index++) {
     float distance = 0.0f;
     if (context->combat_tests[test_index].type == 0 || context->combat_tests[test_index].type == 1) {
      short blockage = actor_perception_compute_target_blockage(&context->combat_tests[test_index].origin,
          &context->combat_tests[test_index].aiming, &position->position, actor->unit_index, 0, &distance);
      if (distance < minimum_distance) minimum_distance = distance;
      if (context->combat_tests[test_index].type == 0) secondary_blockage = MAX(secondary_blockage, blockage);
      else if (context->combat_tests[test_index].type == 1) primary_blockage = MAX(primary_blockage, blockage);
      if (blockage == 0 && context->has_second_direction) {
       short reverse = actor_perception_compute_target_blockage(&context->combat_tests[test_index].origin, &position->second,
           &position->position, context->combat_tests[test_index].object_index, 0, 0);
       reverse_blockage = MAX(reverse_blockage, reverse);
      }
     }
    }
    float evaluation;
    if (primary_blockage >= 2) evaluation = 0.0f;
    else if (primary_blockage >= 1) evaluation = 1.5f;
    else if (secondary_blockage >= 2) evaluation = 4.0f;
    else if (secondary_blockage >= 1) evaluation = 6.5f;
    else evaluation = 10.0f;
    actor_firing_position_add_evaluation(context, position, evaluation, 18, false);
    float reverse_evaluation;
    if (reverse_blockage >= 2) reverse_evaluation = 0.0f;
    else if (reverse_blockage >= 1) reverse_evaluation = 3.0f;
    else reverse_evaluation = 6.0f;
    actor_firing_position_add_evaluation(context, position, reverse_evaluation, 37, false);
    if (!following) {
     character_movement_properties *movement = character_definition_movement_properties_get(actor->character_index);
     if (movement && (movement->flags & 4) && minimum_distance < 15.0f) {
      float proximity = 10.0f;
      if (minimum_distance > 3.0f) proximity *= 1.0f - (minimum_distance - 3.0f) / 12.0f;
      actor_firing_position_add_evaluation(context, position, proximity, 40, false);
     }
    }
   }
  }
 }
}

// @address 0x11bde8 ?squad_get_available_firing_positions@@YA_NJJPAUfiring_position@@PAFJPAUfiring_position_evaluation_context@@@Z
bool squad_get_available_firing_positions(long actor_index, long squad_index, firing_position *positions, short *position_count,
    long maximum_count, firing_position_evaluation_context *context)
{
 squad_area_iterator iterator;
 short flags = actor_get_area_flags(actor_index);
 bool enabled_only = context ? context->enabled_areas_only : false;
 bool result = squad_area_iterator_new(&iterator, (short)squad_index, flags, actor_get_area_type(actor_index), enabled_only,
     (e_pureform)actor_pureform_get_firing_point_selection_form((int)actor_index));
 if (iterator.iterator_type == 2 && context)
  context->area_flags = iterator.type_flags;
 while (squad_area_iterator_next(&iterator) != -1) {
  short area_type = 0;
  short area_flags = 0;
  if (iterator.iterator_type == 2) {
   squads_area_zone_set_entry *area = &((squads_area_zone_set_entry *)iterator.zone_set->address)[iterator.zone_set_index];
   area_type = area->type;
   area_flags = area->flags;
  }
  area_get_available_firing_positions(actor_index, iterator.zone_index, iterator.area_index, positions, position_count, 512, context, area_type,
      area_flags);
 }
 return result;
}


static __forceinline bool test_flag_inline(unsigned long flags, long bit)
{
 return (flags & (1UL << bit)) != 0;
}
static __forceinline real_point3d *actor_firing_position_sight_point_from_line(
    const real_point3d *point, const real_vector3d *direction, float scale, real_point3d *result)
{
 result->x = point->x + direction->i * scale;
 result->y = point->y + direction->j * scale;
 result->z = point->z + direction->k * scale;
 return result;
}
static __forceinline long actor_firing_position_root(long index)
{
 if (index != -1) {
  s_object_header *headers = ((s_object_header *)object_header_data->data);
  do {
   object_datum *object = (object_datum *)headers[datum_index_to_absolute_index(index)].datum;
   if (!test_flag_inline(object->flags, 26)) break;
   index = object->parent_object_index;
  } while (index != -1);
 }
 return index;
}
static __forceinline s_cluster_reference actor_firing_position_invalid_cluster()
{
 s_cluster_reference cluster;
 cluster.value() = -1;
 return cluster;
}
static __forceinline void actor_firing_position_set_cluster(s_cluster_reference *cluster, long bsp, long index)
{
 cluster->bsp_index = (signed char)bsp;
 cluster->cluster_index = (unsigned char)index;
}
static __forceinline bool actor_firing_position_bsp_active(long bsp)
{
 return (unsigned long)bsp <= 15 && (g_active_structure_bsp_mask & (1 << bsp)) != 0;
}
// @address 0x560940 ?firing_position_compute_line_of_sight@@YAXJPAUfiring_position_evaluation_context@@PAUfiring_position@@@Z
void firing_position_compute_line_of_sight(long actor_index, firing_position_evaluation_context *context, firing_position *position)
{
 actor_datum *actor = &((actor_datum *)actor_data->data)[datum_index_to_absolute_index(actor_index)];
 real_vector3d normal;
 if (position->concealed)
  firing_position_get_normal(firing_position_ref_get_definition(position->original_index), &normal);
 short type = context->type;
 if (type == 5 && !context->danger_maximum) {
  if (position->target_distance < 6.0f) {
   real_point3d estimated_position;
   actor_estimate_position(actor->unit_index, 1, &position->position, position->concealed ? &normal : 0, 0, 0, &estimated_position);
   s_cluster_reference cluster;
   if (position->definition->flags & 8)
    cluster = ai_scenario_find_cluster_index(&position->position, global_zero_vector3d);
   else if (position->definition->cluster_index != -1)
    actor_firing_position_set_cluster(&cluster, position->definition->sector.structure_bsp_index, position->definition->cluster_index);
   else
    cluster = actor_firing_position_invalid_cluster();
   position->type = ai_test_line_of_sight(&actor->position.head,
       (unsigned char)actor->position.location.cluster_reference.bsp_index != 255 ? actor->position.location.cluster_reference :
           actor_firing_position_invalid_cluster(), &estimated_position,
       cluster, 0, true, -1, actor_firing_position_root(actor->unit_index), actor->root_vehicle_index != -1, true, 0, 0);
  } else {
   position->type = 4;
  }
 } else {
  real_point3d estimated_position;
  short estimate_type;
  short mode;
  {
  character_weapon_properties *weapon = actor_weapon_properties_get(actor_index);
  const real_vector3d *estimate_direction = 0;
  const real_vector3d *forward = 0;
  estimate_type = 4;
  mode = 0;
  real_vector3d direction;
  if (weapon && context->use_weapon_estimate && (weapon->flags & 2)) {
   estimate_type = 5;
  } else if (type == 1 || type == 2) {
   estimate_type = context->use_weapon_estimate ? 2 : 1;
  } else if (context->has_estimate_direction) {
   estimate_type = 3;
   estimate_direction = &context->estimate_direction;
   actor_firing_position_vector_from_points(&position->position, &context->estimate_target_position, &direction);
   if (normalize2d(&direction.xy) > 0.0f) {
    direction.k = 0.0f;
    forward = &direction;
   } else {
    forward = &actor->facing_vector;
   }
  }
  actor_estimate_position(actor->unit_index, estimate_type, &position->position, position->concealed ? &normal : 0, forward,
      estimate_direction, &estimated_position);
  }
  {
  if (position->cornering == 3) {
   switch (estimate_type) {
   case 2: case 5: estimated_position.z -= 0.1f; break;
   }
  }
  switch (context->type) {
  case 1: mode = 2; break;
  case 2: mode = (short)(((unsigned long)~position->definition->flags >> 3) & 2); break;
  case 3: mode = 1; break;
  }
  bool alternate;
  if (context->type == 2 || context->type == 1) alternate = false;
  else alternate = true;
  s_cluster_reference cluster;
  if (position->definition->flags & 8) {
   cluster = ai_scenario_find_cluster_index(&estimated_position, global_zero_vector3d);
  } else {
   short cluster_index = position->definition->cluster_index;
   if (cluster_index != -1) {
    actor_firing_position_set_cluster(&cluster, position->definition->sector.structure_bsp_index, cluster_index);
    if (!(position->definition->sector.structure_bsp_index >= 0 &&
            position->definition->sector.structure_bsp_index < global_scenario->structure_bsps.count &&
            actor_firing_position_bsp_active(position->definition->sector.structure_bsp_index) && cluster_index >= 0 &&
            cluster_index < ((structure_bsp *)global_tag_instances[datum_index_to_absolute_index(
                ((scenario_structure_bsp_reference *)global_scenario->structure_bsps.address)
                    [position->definition->sector.structure_bsp_index].structure_bsp.index)].definition)->clusters.count))
     cluster = actor_firing_position_invalid_cluster();
   } else {
    cluster = actor_firing_position_invalid_cluster();
   }
  }
  real_point3d target;
  s_cluster_reference target_cluster;
  long target_object;
  if (context->evaluate_danger) {
   target = context->line_of_sight_target;
   target_cluster = context->target_cluster;
   target_object = context->target_object_index;
  } else if (context->guard && context->guard_valid) {
   actor_firing_position_sight_point_from_line(&estimated_position, &context->guard_plane, 1.5f, &target);
   target_cluster = actor_firing_position_invalid_cluster();
   target_object = -1;
  }
  position->type = ai_test_line_of_sight(&estimated_position, cluster, &target, target_cluster, mode, true, target_object,
      actor_firing_position_root(actor->unit_index), actor->root_vehicle_index != -1, alternate, 0, &position->transparent_line_of_sight);
  }
 }
}

// @rdata 0x82021774
static const float actor_firing_position_relocate_maximum_distance_squared = 3.402823466e+38F;
// @rdata 0x8202490c
static const float actor_firing_position_relocate_epsilon = 0.0001f;
// @rdata 0x8202a1a4 pooled
static const float actor_firing_position_relocate_camo_transition_time = 4.0f;
static __forceinline float actor_firing_position_relocate_normalize3d(real_vector3d *vector)
{
 float length = sqrtf(vector->i * vector->i + vector->j * vector->j + vector->k * vector->k);
 if (fabsf(length) >= actor_firing_position_relocate_epsilon) {
  float inverse = 1.0f / length;
  vector->i *= inverse;
  vector->j *= inverse;
  vector->k *= inverse;
  return length;
 }
 return 0.0f;
}
static __forceinline relocate_unit_index actor_firing_position_relocate_player_unit_index_get(
    player_datum *player)
{
 return *(relocate_unit_index *)&player->unit_index;
}
static __forceinline actor_datum *actor_firing_position_relocate_actor_get(long actor_index)
{
 return &((actor_datum *)actor_data->data)[datum_index_to_absolute_index(actor_index)];
}
static __forceinline unit_datum *actor_firing_position_relocate_object_get(long object_index)
{
 return (unit_datum *)((s_object_header *)object_header_data->data)[datum_index_to_absolute_index(object_index)].datum;
}
static __forceinline bool actor_firing_position_relocate_camo_active(long actor_index)
{
 actor_datum *actor = actor_firing_position_relocate_actor_get(actor_index);
 bool result = false;
 if (actor->unit_index != -1)
  result = (_rotl(actor_firing_position_relocate_object_get(actor->unit_index)->unit.unit_flags, 29) & 1) != 0;
 return result;
}
static __forceinline bool actor_firing_position_relocate_set_active_camo(long actor_index, bool camo, float transition_time)
{
 return ((bool (*)(long, bool, float))actor_set_active_camo)(actor_index, camo, transition_time);
}

// @address 0x54fe00 actor_bring_forward_to_firing_position
extern "C" bool actor_bring_forward_to_firing_position(long actor_index)
{
 actor_datum *actor = actor_firing_position_relocate_actor_get(actor_index);
 bool result = false;
 float best_distance_squared = actor_firing_position_relocate_maximum_distance_squared;
 short best_index = -1;
 unit_datum *unit = actor_firing_position_relocate_object_get(actor->unit_index);
 float radius = unit->object.bounding_radius;
 actor_dynamic_firing_set_support_object(actor_index, -1);
 firing_position positions[512];
 short position_count = 0;
 actor_datum *positions_actor = actor_firing_position_relocate_actor_get(actor_index);
 bool get_dynamic = false;
 do
 {
  if (positions_actor->dynamic_firing_set_index != NONE)
  {
   if (positions_actor->attached_vehicle_index != NONE)
   {
    actor_get_dynamic_firing_positions(actor_index, positions, &position_count, 512, 0);
    break;
   }
   get_dynamic = true;
  }
  if (positions_actor->squad_index != NONE)
   squad_get_available_firing_positions(actor_index, positions_actor->squad_index, positions, &position_count, 512, 0);
  if (get_dynamic)
   actor_get_dynamic_firing_positions(actor_index, positions, &position_count, 512, 0);
 } while (false);
 firing_position *position = positions;
 for (short position_index = 0; position_index < position_count; position_index++, position++)
 {
  bool visible = false;
  struct { void *datum; s_data_iterator iterator; } player_iterator;
  player_iterator.iterator.absolute_index = -1;
  player_iterator.iterator.index = -1;
  player_iterator.iterator.data = player_data;
  while ((player_iterator.datum = data_iterator_next(&player_iterator.iterator)) != 0)
  {
   player_datum *player = (player_datum *)player_iterator.datum;
   if (player->unit_index != -1)
   {
    long unit_index = actor_firing_position_relocate_player_unit_index_get(player).value;
    unit_datum *player_unit = actor_firing_position_relocate_object_get(unit_index);
    object_marker marker;
    internal_object_get_markers_by_string_id(unit_index, _string_id_head, &marker, 1, false);
    real_vector3d direction;
    actor_firing_position_vector_from_points(&marker.matrix.position, &position->position, &direction);
    float distance = actor_firing_position_relocate_normalize3d(&direction);
    if (distance < 2.0f)
    {
     visible = true;
     break;
    }
    const real_vector3d *aiming_vector = &player_unit->unit.desired_aiming_vector;
    float dot = actor_firing_position_hint_dot(&direction, aiming_vector);
    if (dot > (float)cos((float)atan2(radius, distance) + 0.7853982f))
    {
     visible = true;
     break;
    }
   }
  }
  if (!visible)
  {
   real_vector3d offset;
   float distance_squared = actor_firing_position_magnitude_squared(actor_firing_position_vector_from_points(&position->position,
       &actor->position.body, &offset));
   if (distance_squared < best_distance_squared)
   {
    best_distance_squared = distance_squared;
    best_index = position_index;
   }
  }
 }

 if (best_index != -1)
 {
  firing_position *position = &positions[best_index];
  long object_index = -1;
  for (long parent_index = actor->unit_index; parent_index != -1; parent_index =
      actor_firing_position_relocate_object_get(parent_index)->object.parent_object_index)
   object_index = parent_index;
  object_set_position_direct(object_index, &position->position, 0, 0, 0, false);
  object_reset_after_teleport(object_index);
  actor_datum *relocated_actor = actor_firing_position_relocate_actor_get(actor_index);
  relocated_actor->path.valid = false;
  relocated_actor->path.obstacle_object_index = -1;
  relocated_actor->path.result_value = 0;
  relocated_actor->path_result_time = 0;
  relocated_actor->path_result_count = 0;
  relocated_actor->path_state = 0;
  relocated_actor->destination_orders.clear();
  character_general_properties *properties =
      (character_general_properties *)character_definition_general_properties_get(actor->character_index);
  if (properties->flags & 0x200)
  {
   if (!actor_firing_position_relocate_camo_active(actor_index))
   {
    unit->unit.active_camouflage = 1.0f;
    actor_firing_position_relocate_set_active_camo(actor_index, false, actor_firing_position_relocate_camo_transition_time);
   }
  }
  result = true;
 }
 return result;
}
// @address 0x11ac08 ?actor_active_select_firing_position@@YA?AUfiring_position_ref@@JPAUfiring_position_evaluation_context@@PAUfiring_position@@PAJPAUpath_state@@PA_N@Z
struct firing_position_ref actor_active_select_firing_position(
    long actor_index, struct firing_position_evaluation_context *context, struct firing_position *position, long *selected_index, struct
        path_state *area_path_state, bool *area_path_state_valid)
{
    actor_datum *actor = &((actor_datum *)actor_data->data)[datum_index_to_absolute_index(actor_index)];
    firing_position_ref result;
    firing_position_ref current_reference;
    if (!fp_ref_equals(actor->firing_position_reference, FP_REF_NONE) && actor->firing_position_locked)
        context->use_current_position = 0;
    else
        context->use_current_position = 1;
    result = actor_select_firing_position(actor_index, context, position, selected_index, area_path_state, area_path_state_valid);
    if (fp_ref_equals(result, FP_REF_NONE))
    {
        if (!fp_ref_equals(actor->firing_position_reference, FP_REF_NONE) && actor->firing_position_locked)
        {
            current_reference = actor->firing_position_reference;
            if (position != 0 && firing_position_ref_get_definition(actor->firing_position_reference) != 0 && new_firing_position(actor_index,
                actor->firing_position_reference, position, context))
            {
                if (context->evaluate_danger)
                {
                    position->danger_distance_squared = actor_firing_position_combat_distance_squared3d(&context->target_position,
                        &position->position);
                }
                else
                {
                    position->danger_distance_squared = 0.0f;
                }
                if (!firing_position_forced_evaluation(actor_index, context, position))
                    current_reference = FP_REF_NONE;
            }
            *area_path_state_valid = 0;
            result = current_reference;
            *selected_index = -1;
        }
    }
    return result;
}

// @address 0x11bbc8 actor_get_available_firing_positions
extern "C" short actor_get_available_firing_positions(
    long actor_index, firing_position *positions, long maximum_count, firing_position_evaluation_context *context)
{
 short count = 0;
 bool check_dynamic = false;
 actor_datum *actor = &((actor_datum *)actor_data->data)[datum_index_to_absolute_index(actor_index)];
 if (context != 0 && context->include_current_position && new_firing_position(actor_index, FP_REF_CURRENT, positions, context)) {
  positions->enabled = (actor->goal_status == 3);
  positions->primary = actor->flying;
  bool special = (context->standing == 2 || context->standing == 3);
  count = 1;
  positions->special = special;
 }
 if (actor->dynamic_firing_set_index != -1) {
  if (actor->attached_vehicle_index != -1) {
   actor_get_dynamic_firing_positions(actor_index, positions, &count, 512, context);
   return count;
  }
  check_dynamic = (context == 0 || !context->ignore_dynamic_firing_positions);
 }
 if (actor->squad_index != -1) {
  squad_get_available_firing_positions(actor_index, actor->squad_index, positions, &count, 512, context);
 }
 if (check_dynamic) {
  if (context != 0 && context->type == 4 && actor->squad_index != -1 && count > 0) {
   squad_datum *squad = &((squad_datum *)squad_data->data)[datum_index_to_absolute_index(actor->squad_index)];
   if (::squad_valid_task((::squad_datum *)squad) &&
       ((s_task *)((s_objective *)global_scenario->objectives.address)[squad->objective_index].tasks.address)[squad->task_index].follow_mode !=
           0 &&
       squad->follow_object_index != -1 && !actor->seeking_squad)
    check_dynamic = false;
  }
  if (check_dynamic) {
   actor_get_dynamic_firing_positions(actor_index, positions, &count, 512, context);
   return count;
  }
 }
 return count;
}
// ---- 0x11ae50 actor_setup_evaluation_context support ----
// match: noalias declaration preserves caller live registers

static __forceinline long setup_object_get_ultimate_parent(long object_index)
{
 long ultimate_index = -1;
 long parent_index = object_index;
 while (parent_index != -1) {
  ultimate_index = parent_index;
  parent_index = ((object_datum *)((s_object_header
      *)object_header_data->data)[datum_index_to_absolute_index(parent_index)].datum)->parent_object_index;
 }
 return ultimate_index;
}
static __forceinline float actor_setup_distance3d(const real_point3d *a, const real_point3d *b)
{
 float dy = b->y - a->y;
 float dz = b->z - a->z;
 float dx = b->x - a->x;
 return sqrtf(dx * dx + (dz * dz + dy * dy));
}
static __forceinline float actor_setup_magnitude_squared3d(const real_vector3d *vector)
{
 float j = vector->j;
 float i = vector->i;
 float k = vector->k;
 return k * k + (i * i + j * j);
}
// @address 0x11ae50 ?actor_setup_evaluation_context@@YAXJPAUfiring_position_evaluation_context@@@Z
void actor_setup_evaluation_context(long actor_index, firing_position_evaluation_context *context)
{
 actor_datum *actor = &((actor_datum *)actor_data->data)[datum_index_to_absolute_index(actor_index)];
 if (actor_is_giant(actor))
  context->panic_maximum_distance = 200.0f;
 else if (actor->vehicle_driver_type > 0)
  context->panic_maximum_distance = 40.0f;
 else if (actor->use_object)
  context->panic_maximum_distance = 40.0f;
 else
  context->panic_maximum_distance = 20.0f;
 if (context->radius == 0.0f)
  context->radius = context->panic_maximum_distance;
 if (actor->root_vehicle_index != -1)
  ai_get_center_of_mass(actor->root_vehicle_index, &context->test_position);
 else
  ai_get_center_of_mass(actor->unit_index, &context->test_position);
 short saved_type = context->type;
 context->compute_target_distance = (saved_type == 5 || saved_type == 8 || saved_type == 3);
 context->standing = actor_get_standing(actor_index);
 context->evaluate_danger = false;
 if (saved_type == 4 && context->has_second_direction == 0) {
  context->evaluate_danger = false;
 } else if (context->target_override_valid) {
  context->evaluate_danger = true;
  context->target_position = context->target_override_position;
  context->target_point = context->target_override_point;
  context->target_sector = context->target_override_sector;
  context->target_cluster = context->target_override_cluster;
  context->hide_distance = actor_setup_distance3d(&actor->position.body, &context->target_position);
  context->target_prop_index = -1;
  context->target_object_index = -1;
  context->leader_distance = 0.0f;
  actor_estimate_position(actor->unit_index, 1, &context->target_position, 0, 0, 0, &context->estimate_target_position);
  context->line_of_sight_target = context->estimate_target_position;
 } else {
  long prop_choice;
  if (saved_type == 1)
   prop_choice = actor->handle;
  else
   prop_choice = actor->target_prop_index;
  if (prop_choice != -1) {
   prop_ref_datum *ref = &((prop_ref_datum *)tls_prop_ref_data()->data)[datum_index_to_absolute_index(prop_choice)];
   prop_state *state = prop_state_get(ref);
   long owner = ref->owner_index;
   prop_view *view2 = 0;
   if (owner != -1) {
    tracking_datum *addr = &((tracking_datum *)tls_prop_data()->data)[datum_index_to_absolute_index(owner)];
    if (addr != 0)
     view2 = &addr->tracking_view;
   }
   object_datum *obj = (object_datum *)((s_object_header *)object_header_data->data)[datum_index_to_absolute_index(ref->object_index)].datum;
   prop_find_pathfinding_location(actor_index, prop_choice);
   context->evaluate_danger = true;
   context->target_position = state->position;
   context->target_point = state->pathfinding_point;
   context->target_sector = state->pathfinding_sector;
   context->hide_distance = ref->distance;
   context->target_prop_index = prop_choice;
   context->estimate_target_position = state->head_position;
   context->target_object_index = -1;
   if ((unsigned char)state->location.cluster_reference.bsp_index != 255)
    context->target_cluster = state->location.cluster_reference;
   else
    context->target_cluster.value() = -1;
   if (obj->object_identifier.get_type() == _object_type_biped && obj->parent_object_index != -1)
    context->target_object_index = setup_object_get_ultimate_parent(obj->parent_object_index);
   else
    context->target_object_index = setup_object_get_ultimate_parent(ref->object_index);
   if (context->type == 2 && ((_object_mask_unit & (1 << obj->object_identifier.get_type())) != 0) && ref->awareness < 1 &&
       (view2 != 0 && view2->tracking_index >= 0))
   {
    real_point3d tmp;
    ai_point_get_position(&view2->pathfinding_point, &tmp);
    actor_estimate_position(ref->object_index, 1, &tmp, 0, 0, 0, &context->line_of_sight_target);
    {
     s_cluster_reference cluster = scenario_cluster_reference_from_point(&context->line_of_sight_target);
     context->target_cluster = cluster;
    }
   } else {
    context->line_of_sight_target = state->center;
   }
   {
    character_vitality_properties *vit = character_definition_vitality_properties_get(actor->character_index);
    context->leader_distance = 0.0f;
    if (vit != 0)
     context->leader_distance = vit->suicide_radius;
   }
   if (view2 != 0 && ref->knowledge_state >= 3 && context->hint == 0) {
    context->hint = true;
    context->target_hint_vector = view2->sighting_direction;
   }
   {
    short t = context->type;
    context->special = context->special | (t == 4 || t == 6);
   }
  }
 }
 if (context->evaluate_danger != 0) {
  short t = context->type;
  if (t == 0 || t == 3 || t == 2 || t == 6)
   context->has_second_direction = true;
 }
 if (context->hint) {
  context->target_hint_vector.k = 0.0f;
  context->hint = normalize3d(&context->target_hint_vector) > 0.0f;
 }
 {
  character_weapon_properties *weapon = actor_weapon_properties_get(actor_index);
  if (weapon != 0 && actor_setup_magnitude_squared3d(&weapon->estimate_direction) > 0.0001f) {
   context->has_estimate_direction = true;
   context->estimate_direction = weapon->estimate_direction;
  } else {
   context->has_estimate_direction = false;
  }
 }
 if (actor->unit_index != -1)
  context->use_weapon_estimate = motor_animation_test_mode(actor->unit_index, 141);
 if (actor->danger_zone.danger_type > 0) {
  long pref = actor->danger_zone.pref_index;
  if ((pref == -1 || ((prop_ref_datum *)tls_prop_ref_data()->data)[datum_index_to_absolute_index(pref)].knowledge_state >= 1) &&
      !(actor->danger_zone.distance >= actor->danger_zone.extent + 3.0f)) {
   context->compute_path_direction = 1;
   context->evaluate_danger_zone = 1;
  }
 }
 if (actor->vehicle_driver_type == 4) {
  context->combat_disabled = true;
  context->require_facing = true;
 }
 // Unreachable positions remain eligible when combat permits them.
 context->allow_unreachable |= actor->combat_flag339;
 {
  character_movement_properties *move = character_definition_movement_properties_get(actor->character_index);
  if (move != 0) {
   if (context->ignore_cover) {
    context->penalize_unconcealed = false;
    context->allow_either = false;
   } else {
    context->penalize_unconcealed |= ((move->flags & 0x100) != 0);
    context->allow_either |= context->penalize_unconcealed;
    context->allow_either &= ((move->flags & 0x40) || (move->flags & 0x80));
   }
   if (move->flags & 0x200) {
    if (context->allow_either)
     context->prefer_concealed = true;
    context->prefer_special = true;
   }
   object_datum *unit = (object_datum *)((s_object_header *)object_header_data->data)[datum_index_to_absolute_index(actor->unit_index)].datum;
   if (*(long *)((char *)unit + ((unit_datum *)unit)->unit.motor_program_offset) == 5) {
    character_general_properties *gen = character_definition_general_properties_get(actor->character_index);
    if (gen != 0 && !(gen->flags & 2) && (move->flags & 0x200))
     context->allow_unreachable = true;
   }
  } else {
   context->allow_either = false;
   context->prefer_special = false;
   context->prefer_concealed = false;
  }
 }
 context->combat_test_count = 0;
 context->combat_position_count = 0;
 context->attack_test_count = 0;
 context->prefer_special |= context->allow_unreachable;
 bool docombat = false;
 if (actor->combat_status >= 5) {
  long clump = actor->clump_index;
  if (clump != -1 && ((clump_datum *)tls_clump_data()->data)[datum_index_to_absolute_index(clump)].actor_count > 1)
   docombat = true;
 }
 switch (context->type) {
 case 0:
 case 2:
 case 3:
 case 6:
  break;
 default:
  docombat = false;
  break;
 }
 if (docombat) {
  long prop_ref_index = ((actor_datum *)actor_data->data)[datum_index_to_absolute_index(actor_index)].first_prop_ref_index;
  while (context->combat_test_count < 32 && prop_ref_index != -1) {
   prop_ref_datum *pr = &((prop_ref_datum *)tls_prop_ref_data()->data)[datum_index_to_absolute_index(prop_ref_index)];
   long own = pr->owner_index;
   prop_ref_index = pr->next_prop_ref_index;
   prop_state *sv;
   if (own != -1)
    sv = &((tracking_datum *)tls_prop_data()->data)[datum_index_to_absolute_index(own)].state;
   else
    sv = &((prop_datum *)tls_ai_prop_data()->data)[datum_index_to_absolute_index(pr->prop_index)].state;
   prop_datum *pd = &((prop_datum *)tls_ai_prop_data()->data)[datum_index_to_absolute_index(pr->prop_index)];
   real_vector3d aiming;
   if (pr->knowledge_state >= 1 && sv->object_index == -1 && sv->inactive == 0 && pd->dead == 0 && pd->player != 0 &&
       ai_player_try_and_get_combat_position(pd->object_index, &aiming) != 0)
   {
    context->combat_tests[context->combat_test_count].type = 1;
    context->combat_tests[context->combat_test_count].origin = sv->position;
    context->combat_tests[context->combat_test_count].aiming = aiming;
    context->combat_tests[context->combat_test_count].object_index = pd->object_index;
    context->combat_test_count++;
    context->combat_position_count++;
   }
  }
  long clump = actor->clump_index;
  if (clump != -1) {
   long cur = ((clump_datum *)tls_clump_data()->data)[datum_index_to_absolute_index(clump)].first_actor_index;
   while (context->combat_test_count < 32 && cur != -1) {
    long idx = cur;
    actor_datum *other = &((actor_datum *)actor_data->data)[datum_index_to_absolute_index(cur)];
    cur = other->next_clump_actor_index;
    real_vector3d aiming;
    if (other != actor && other->root_vehicle_index == -1 && actor_attacking_target(idx, &aiming)) {
     context->combat_tests[context->combat_test_count].type = 0;
     context->combat_tests[context->combat_test_count].origin = other->position.body;
     context->combat_tests[context->combat_test_count].aiming = aiming;
     context->combat_tests[context->combat_test_count].object_index = other->unit_index;
     context->combat_test_count++;
     context->combat_position_count++;
    }
   }
  }
 }
 if (actor->search_active == 0 || (actor->attached_vehicle_index != -1 && actor->vehicle_desire >= 2))
  context->ignore_dynamic_firing_positions = true;
 if (context->evaluate_cornering) {
  if (actor->seeking_squad) {
   context->evaluate_cornering = false;
  } else {
   long unit = actor->unit_index;
   if (unit == -1 || !motor_animation_test_mode(unit, 260) || !motor_animation_test_mode(unit, 261))
    context->evaluate_cornering = false;
  }
 }
 if (!context->ignore_dynamic_firing_positions && (context->evaluate_cornering || context->type == 2))
  context->dynamic_firing_positions_valid = dynamic_firing_set_find_and_attach(actor_index, context, context->evaluate_cornering, context->type == 2);
 context->flight_cache.count = 0;
}
// @address 0x11ad78 ?actor_select_firing_position@@YA?AUfiring_position_ref@@JPAUfiring_position_evaluation_context@@PAUfiring_position@@PAJPAUpath_state@@PA_N@Z
struct firing_position_ref actor_select_firing_position(long p0, struct firing_position_evaluation_context *p1, struct firing_position *p2,
    // match: 0x11ad78: pointer __restrict qualifier prevents alias reload
    long *p3, struct path_state *p4, bool *__restrict p5)
{
    actor_datum *actor;
    firing_position positions[0x200];
    short count;
    *p5 = 0;
    actor = &((actor_datum *)actor_data->data)[datum_index_to_absolute_index(p0)];
    if (actor->swarm != 0)
        return FP_REF_NONE;
    actor_find_pathfinding_location(p0);
    actor_setup_evaluation_context(p0, p1);
    if (actor->searching)
        actor->flying = 1;
    count = actor_get_available_firing_positions(p0, positions, 0x200, p1);
    return actor_select_firing_position_internal(p0, p1, positions, count, p2, p3, p4, p5);
}

struct path_error_info;
struct path_debug_storage;
struct path_input;


static __forceinline actor_datum *actor_firing_position_select_actor_get(long actor_index)
{
    return &((actor_datum *)actor_data->data)[datum_index_to_absolute_index(actor_index)];
}
static __forceinline prop_ref_datum *actor_firing_position_select_prop_ref_get(long prop_ref_index)
{
    return &((prop_ref_datum *)tls_prop_ref_data()->data)[datum_index_to_absolute_index(prop_ref_index)];
}

static __forceinline void actor_firing_position_select_path_input_new(
    path_input *input, float radius, bool pathfinding_type, long unit_index, float danger_cost)
{
    memset(input, 0, sizeof(*input));
    input->radius = radius;
    input->allow_broken_sectors = pathfinding_type;
    input->unit_index = unit_index;
    input->destination_object_index = -1;
    input->hint_cost = danger_cost;
}
static __forceinline void actor_firing_position_select_path_input_set_start(path_input *input, const c_ai_point3d *point, c_sector_ref sector)
{
    input->start_set = true;
    input->start = *point;
    input->start_sector_ref = sector;
}
static __forceinline void actor_firing_position_select_path_input_set_search_bounds(path_input *input, float radius)
{
    input->bounded_search = true;
    input->maximum_distance = radius;
}
// @rdata 0x82022dc0
static const float k_actor_select_attractor_inner_radius_scale = 0.5f;

static __forceinline void actor_firing_position_select_path_input_set_attractor(
    path_input *input, const real_point3d *point, float radius, float inner_radius, long object_index, float weight, bool blocking)
{
    input->use_attractor = true;
    input->attractor = *point;
    input->attractor_radius = radius;
    input->attractor_strength = inner_radius;
    input->attractor_object_index = object_index;
    input->attractor_weight = weight;
    input->pad47 = blocking;
}
// @rdata 0x8202490c
static const float k_actor_select_epsilon = 0.0001f;

static __forceinline float actor_firing_position_select_distance_limit(const float *limit)
{
    return *limit;
}

static __forceinline void actor_firing_position_select_set_reference(firing_position_ref *result, const firing_position *position)
{
    *result = position->original_index;
}

static __forceinline float actor_firing_position_select_normalize3d(real_vector3d *vector)
{
    float length = sqrtf(magnitude_squared3d(vector));
    if (fabsf(length) >= k_actor_select_epsilon) {
        float inverse = 1.0f / length;
        vector->i *= inverse;
        vector->j *= inverse;
        vector->k *= inverse;
        return length;
    }
    return 0.0f;
}

static __forceinline bool actor_firing_position_select_post_evaluate(long actor_index, firing_position_evaluation_context *context)
{
    const s_firing_position_post_evaluator *evaluator = g_firing_position_post_evaluators;
    bool result = true;
    context->score = 0.0f;
    while (result && evaluator->function) {
        if (evaluator->mask & (long)(1ULL << context->type))
            result = evaluator->function(actor_index, context, 0);
        evaluator++;
    }
    return result;
}
static const void *actor_firing_position_select_hash_table_find(s_hash_table *table, const void *key, s_firing_position_hash_search_result output)
{
    const void *result = 0;
    s_hash_table_bucket *node = (s_hash_table_bucket *)hash_table_find_internal((s_hash_table *)table, key);
    if (node) {
        if (output.ptr - 1)
            memcpy(output.ptr - 1, node + 1, table->entry_size);
        result = node->key;
    }
    return result;
}
static long actor_firing_position_select_firing_position_owner(firing_position_ref reference)
{
    long owner = -1;
    s_firing_position_hash_search_result output;
    output.ptr = (int *)&owner + 1;
    actor_firing_position_select_hash_table_find(g_actor_firing_position_hash, (const void *)(uintptr_t)reference.value, output);
    return owner;
}

static __forceinline void actor_firing_position_select_copy_selected_position(
    long actor_index, const firing_position *best, firing_position *current_position, firing_position_ref *result, long *owner_actor_index)
{
    if (current_position)
        *current_position = *best;
    *(long *)result = best->reference;
    if (owner_actor_index) {
        if (!fp_ref_equals(*result, FP_REF_CURRENT)) {
            long owner = actor_firing_position_select_firing_position_owner(*result);
            if (owner != actor_index)
                *owner_actor_index = owner;
            else
                *owner_actor_index = -1;
        }
        else {
            *owner_actor_index = -1;
        }
    }
}

// @address 0x11c3e0 ?actor_select_firing_position_internal@@YA?AUfiring_position_ref@@JPAUfiring_position_evaluation_context@@PAUfiring_position@@F1PAJPAUpath_state@@PA_N@Z
firing_position_ref actor_select_firing_position_internal(long actor_index, firing_position_evaluation_context *context,
    firing_position *firing_positions, short firing_position_count, firing_position *current_position, long *owner_actor_index,
    // match: 0x11c3e0: pointer __restrict qualifier prevents alias reload
    path_state *area_path_state, bool *__restrict area_path_state_valid)
{
    actor_datum *actor = actor_firing_position_select_actor_get(actor_index);
    float best_score = 0.0f;
    float maximum_distance = actor_firing_position_select_distance_limit(&actor_firing_position_guard_maximum);
    firing_position_ref result = FP_REF_NONE;
    long count = firing_position_count;
    bool any_within_radius = false;
    bool any_outside_radius = false;
    bool danger_close = false;
    short best_index = -1;
    *area_path_state_valid = false;
    tls_actor_globals()->firing_positions_evaluated = true;
    if (count == 0) {
        actor_clear_discarded_firing_positions(actor_index, false);
        result = FP_REF_NONE;
    }
    else {
        bool path_distances_computed = false;
        if (context->evaluate_danger && context->compute_target_distance) {
            if (context->allow_unreachable) {
                for (short index = 0; index < count; index++)
                    path_distance_to_target_flying_point(&firing_positions[index], context);
            }
            else if (context->target_sector.valid()) {
                character_movement_properties *movement = character_definition_movement_properties_get(actor->character_index);
                c_path_state path;
                path_input input;
                special_movement special;
                actor_firing_position_select_path_input_new(&input, movement->pathfinding_radius, actor->pathfinding_type, -1, 0.0f);
                actor_firing_position_select_path_input_set_start(&input, &context->target_point, context->target_sector);
                actor_firing_position_select_path_input_set_search_bounds(&input, 20.0f);
                actor_special_movement_new(actor_index, &special);
                path_state_new(&input, (path_state *)path.m_data, 0, 0, &special, 0);
                path_state_find((path_state *)path.m_data);
                for (short index = 0; index < count; index++) {
                    firing_position *position = &firing_positions[index];
                    if ((position->concealed && context->prefer_concealed) || (position->reachable && context->prefer_special)) {
                        path_distance_to_target_flying_point(position, context);
                    }
                    else {
                        path_state_estimated_distance((path_state *)path.m_data, (const c_ai_point3d *)position->definition,
                            position->definition->sector_ref,
                            &position->evaluation_distance, 0, context->has_firing_direction ? &position->firing_direction : 0);
                        path_distances_computed = true;
                    }
                }
            }
            else if (actor_is_giant(actor_firing_position_select_actor_get(actor_index))) {
                if (count > 0) {
                    const real_point3d *target = &context->target_position;
                    short index = 0;
                    do {
                        firing_position *position = &firing_positions[index];
                        float x = position->position.x - target->x;
                        float y = position->position.y - target->y;
                        float z = position->position.z - target->z;
                        position->evaluation_distance = sqrtf(x * x + (z * z + y * y));
                        index++;
                    } while (index < count);
                }
            }
        }
        if (!path_distances_computed && context->has_firing_direction) {
            for (short index = 0; index < count; index++) {
                firing_position *position = &firing_positions[index];
                vector_from_points3d(&context->target_position, &position->position, &position->firing_direction);
                normalize3d(&position->firing_direction);
            }
        }
        if (context->evaluate_danger && context->has_second_direction) {
            for (short index = 0; index < count; index++) {
                firing_position *position = &firing_positions[index];
                vector_from_points3d(&context->target_position, &position->position, &position->second);
                actor_firing_position_select_normalize3d(&position->second);
            }
        }
        bool use_area_path = true;
        if (context->allow_unreachable || context->ignore_area_path)
            use_area_path = false;
        if (use_area_path) {
            path_input input;
            special_movement special;
            actor_moving_reserved_hints hints;
            actor_path_input_new(actor_index, &input);
            actor_firing_position_select_path_input_set_search_bounds(&input, context->radius);
            if (context->attractor_valid && context->evaluate_danger) {
                long object_index = -1;
                if (context->target_prop_index != -1)
                    object_index = actor_firing_position_select_prop_ref_get(context->target_prop_index)->object_index;
                actor_firing_position_select_path_input_set_attractor(&input, &context->target_position, context->attractor_radius,
                    context->attractor_radius *k_actor_select_attractor_inner_radius_scale, object_index, context->attractor_weight,
                    !context->attractor_blocking);
            }
            else if (actor->danger_zone.danger_type > 0) {
                actor_firing_position_select_path_input_set_attractor(&input, &actor->danger_zone.position, actor->danger_zone.radius,
                    actor->danger_zone.radius, actor->danger_zone.object_index, 10.0f, true);
            }
            actor_special_movement_new(actor_index, &special);
            actor_moving_compute_reserved_hints(actor_index, &hints);
            path_state_new(&input, area_path_state, 0, 0, &special, &hints);
            if (path_state_find(area_path_state))
                *area_path_state_valid = true;
        }
        float danger_distance = 0.0f;
        actor_get_weapon_range(actor_index, 0, 0, &danger_distance);
        for (short index = 0; index < count; index++) {
            firing_position *position = &firing_positions[index];
            if (context->evaluate_danger) {
                float distance_squared = distance_squared3d(&context->target_position, &position->position);
                position->danger_distance_squared = distance_squared;
                if (!danger_close && distance_squared < danger_distance * danger_distance)
                    danger_close = true;
            }
            real_vector3d direction;
            vector_from_points3d(&actor->position.body, &position->position, &direction);
            float distance_squared = magnitude_squared3d(&direction);
            if (distance_squared < context->radius * context->radius) {
                if (use_area_path && !(position->concealed && context->prefer_concealed) && !(position->reachable && context->prefer_special)) {
                    path_state_estimated_distance(area_path_state, (const c_ai_point3d *)position->definition, position->definition->sector_ref,
                        &position->target_distance, &position->distance, context->compute_path_direction ? &position->target_direction : 0);
                }
                else {
                    position->distance = sqrtf(point_to_line_distance_squared3d(&context->target_position, &actor->position.body, &direction));
                    position->target_distance = actor_firing_position_select_normalize3d(&direction);
                    if (context->compute_path_direction)
                        position->target_direction = direction;
                }
            }
            if (position->target_distance < context->radius) {
                any_within_radius = true;
            }
            else {
                position->preferred = true;
                position->valid = false;
                if (position->target_distance < maximum_distance)
                    any_outside_radius = true;
            }
        }
        if (!any_within_radius && !any_outside_radius && !context->allow_unreachable && !actor->airborne &&
            (!actor->pathfinding_sector.valid() || (*area_path_state_valid && count > 0 && area_path_state->node_count < 30)))
            actor->isolation_level++;
        else
            actor->isolation_level = 0;
        if (context->use_current_position && !any_within_radius) {
            *area_path_state_valid = false;
            unsigned long *seed = tls_random_seed();
            random_seed_next(seed);
            best_index = (short)(((*seed >> 16) * count) >> 16);
            actor_clear_discarded_firing_positions(actor_index, false);
            if (!firing_position_forced_evaluation(actor_index, context, &firing_positions[best_index]))
                best_index = -1;
        }
        else {
            short line_of_sight_count = 0;
            firing_position_pre_evaluate(actor_index, context, firing_position_count, firing_positions);
            long sorted_indices[2048];
            for (short index = 0; index < count; index++)
                sorted_indices[index] = index;
            global_temporary_sort_firing_position_count = firing_position_count;
            global_temporary_sort_firing_position_array = firing_positions;
            if ((unsigned long)count >= 2)
                templated_sort<long>::qsort(sorted_indices, count, (bool (*)(long, long, const void *))firing_position_compare, 0);
            context->post_evaluation_result = actor_firing_position_select_post_evaluate(actor_index, context);
            bool out_of_evaluations = false;
            bool done = false;
            while (!done && !out_of_evaluations) {
                float best_hide_score = actor_firing_position_guard_minimum;
                short best_hide_count = 0;
                firing_position *position = firing_positions;
                for (short index = 0; index < count; index++, position++) {
                    if (position->valid) {
                        if (position->hide_score > best_hide_score) {
                            best_hide_score = position->hide_score;
                            best_hide_count = 1;
                        }
                        else if (position->hide_score == best_hide_score) {
                            best_hide_count++;
                        }
                    }
                }
                short remaining = best_hide_count;
                if (remaining == 0)
                    break;
                if (best_index == -1)
                    best_score = best_hide_score;
                done = false;
                bool cannot_improve = false;
                for (short sorted_index = 0; sorted_index < count; sorted_index++) {
                    if (cannot_improve && done && best_index != -1)
                        break;
                    short index = (short)sorted_indices[sorted_index];
                    firing_position *position = &firing_positions[index];
                    bool valid = position->valid;
                    if (!valid && !position->evaluated) {
                        out_of_evaluations = true;
                        break;
                    }
                    float hide_score = position->hide_score;
                    bool forced = hide_score < best_hide_score - actor_firing_position_guard_concealment_score;
                    position->forced = forced;
                    if (context->post_evaluation_result && !cannot_improve) {
                        float evaluation = position->penalty + position->hide_score;
                        float upper_bound = evaluation + context->score;
                        cannot_improve = upper_bound <= best_score;
                    }
                    if (!valid || forced)
                        continue;
                    bool evaluated = position->evaluated;
                    if (cannot_improve && hide_score != best_hide_score)
                        continue;
                    if (!position->evaluated) {
                        position->evaluated_penalty = position->penalty;
                        position->evaluated_hide_score = hide_score;
                        position->evaluated = true;
                        if (context->evaluate_danger || (context->guard && context->guard_valid)) {
                            short maximum_line_of_sight_count = 40;
                            if (tls_actor_globals()->throttled)
                                maximum_line_of_sight_count = 10;
                            if (line_of_sight_count >= maximum_line_of_sight_count) {
                                out_of_evaluations = true;
                                break;
                            }
                            firing_position_compute_line_of_sight(actor_index, context, position);
                            line_of_sight_count++;
                        }
                        evaluated = firing_position_post_evaluate(actor_index, context, position);
                    }
                    if (position->hide_score == best_hide_score) {
                        done = done || evaluated;
                        if (--remaining == 0 && !done)
                            break;
                    }
                    if (evaluated) {
                        float score = position->penalty + position->hide_score;
                        if (best_index == -1 || score > best_score) {
                            best_index = index;
                            best_score = score;
                        }
                    }
                }
            }
        }
    }
    if (best_index != -1) {
        firing_position *best = &firing_positions[best_index];
        if (current_position)
            *current_position = *best;
        result = best->original_index;
        if (owner_actor_index) {
            if (!fp_ref_equals(result, FP_REF_CURRENT)) {
                long owner = actor_firing_position_select_firing_position_owner(result);
                if (owner != actor_index)
                    *owner_actor_index = owner;
                else
                    *owner_actor_index = -1;
            }
            else {
                *owner_actor_index = -1;
            }
        }
        if ((best->concealed && context->prefer_concealed) || best->reachable) {
            if (!*area_path_state_valid) {
                path_input input;
                actor_path_input_new(actor_index, &input);
                path_state_new(&input, area_path_state, 0, 0, 0, 0);
                *area_path_state_valid = true;
            }
            area_path_state->flight_cache = context->flight_cache;
        }
    }
    if (context->evaluate_danger && context->target_prop_index != -1) {
        if ((!danger_close && !context->ignore_unassailable_test &&
            actor_firing_position_select_prop_ref_get(context->target_prop_index)->visibility_type <= 1) ||
            (context->type == 0 && best_index == -1))
            actor_perception_unassailable(actor_index, context->target_prop_index, true, true);
    }
    return result;
}
static const float actor_firing_position_attack_default_radius = 4.0f;
static const float actor_firing_position_attack_exposed_scale = 1.0f;
static const float actor_firing_position_attack_leader_scale = 5.0f;
__forceinline c_combat_range::c_combat_range()
{
  m_leader_index = -1;
  m_weapon_range_valid = false;
  m_leader_distance_valid = false;
  m_target_valid = false;
  m_leader_valid = false;
  m_leader_position_valid = false;
  m_actor_count = 0;
  m_maximum = 0.0f;
  m_minimum = actor_firing_position_guard_maximum;
  m_distance = 0.0f;
  m_follower_minimum = 0.0f;
  m_follower_maximum = actor_firing_position_guard_maximum;
  m_leader_minimum = 0.0f;
  m_leader_maximum = actor_firing_position_guard_maximum;
 }

__forceinline void c_combat_range::add_actors_from_task(short objective_index, short task_index)
{
  task_actor_iterator iterator;
  task_actor_iterator_new(objective_index, task_index, &iterator);
  while (task_actor_iterator_next(&iterator))
   add_actor(iterator.actor_index);
 }

__forceinline void c_combat_range::add_actors_from_squad_task(long squad_index)
{
  if (squad_index != -1) {
   squad_datum *squad = &((squad_datum *)squad_data->data)[datum_index_to_absolute_index(squad_index)];
   if (squad_valid_task((squad_datum *)squad))
    add_actors_from_task(squad->objective_index, squad->task_index);
  }
 }

__forceinline void c_combat_range::set_custom_weapon_range(float minimum, float maximum, float distance)
{
  m_weapon_minimum = minimum;
  m_weapon_maximum = maximum;
  m_weapon_distance = distance;
  m_weapon_range_valid = true;
 }

__forceinline void c_combat_range::set_target_danger_radius(float radius)
{
  m_leader_distance = radius;
  m_leader_distance_valid = true;
 }

__forceinline void c_combat_range::set_target(real_point3d *point)
{
  m_leader_position = *point;
  m_leader_position_valid = true;
 }

__forceinline void c_combat_range::set_target(real_point3d *point, float radius)
{
  set_target(point);
  set_target_danger_radius(radius);
 }
static __forceinline void actor_firing_position_attack_add_evaluation(
    firing_position_evaluation_context *context, firing_position *position, float evaluation, short type, short mode)
{
 if (mode == 0)
  position->score += evaluation;
 else if (mode == 1)
  position->hide_score += evaluation;
}
static __forceinline short actor_firing_position_reference_frame(const c_ai_point3d *point) { return point->sector_ref[0]; }

static __forceinline float actor_firing_position_attack_dot(const real_vector3d *a, const real_vector3d *b)
{
 return a->i * b->i + a->j * b->j + a->k * b->k;
}
static __forceinline real_vector3d *actor_firing_position_attack_point_from_line(
    const real_vector3d *point, const real_vector3d *vector, float t, real_vector3d *result)
{
 result->i = point->i + vector->i * t;
 result->j = point->j + vector->j * t;
 result->k = point->k + vector->k * t;
 return result;
}
// @address 0x55fe30 ?pre_evaluator_attack@@YAXJPAUfiring_position_evaluation_context@@FPAUfiring_position@@@Z
void pre_evaluator_attack(long actor_index, firing_position_evaluation_context *context, short count, firing_position *positions)
{
 actor_datum *actor = &((actor_datum *)actor_data->data)[datum_index_to_absolute_index(actor_index)];
 if (context->attack) {
  c_combat_range combat_range;
  if (actor->squad_index != -1 && squad_get_leader((short)actor->squad_index) != -1)
   combat_range.add_actors_from_squad_task(actor->squad_index);
  if (context->attack_range_minimum > 0.0f && context->attack_range_maximum > 0.0f && context->attack_range_distance > 0.0f)
   combat_range.set_custom_weapon_range(context->attack_range_minimum, context->attack_range_maximum, context->attack_range_distance);
  combat_range.set_target(&context->target_position, context->leader_distance);
  if (combat_range.place_actor(actor_index)) {
   short index;
   firing_position *position;
   for (index = 0, position = positions; index < count; index++, position++) {
    if (position->valid) {
     float distance = square_root(position->danger_distance_squared);
     if (combat_range.m_range_vertical > 0.0f && distance > 0.8f * combat_range.m_range_vertical) {
      float penalty = -1.0f * actor_firing_position_attack_exposed_scale * (distance - 0.8f * combat_range.m_range_vertical);
      penalty = PIN(penalty, -500.0f, 0.0f);
      actor_firing_position_attack_add_evaluation(context, position, penalty, 13, 1);
     }
     float bonus = 0.0f;
     float penalty = 0.0f;
     if (distance > combat_range.m_range_maximum) {
      penalty = -10.0f * MIN((distance - combat_range.m_range_maximum) / (combat_range.m_range_minimum - combat_range.m_range_maximum), 1.0f);
     } else if (distance > combat_range.m_range_ideal) {
      bonus = (10.0f * (combat_range.m_range_maximum - distance)) / (combat_range.m_range_maximum - combat_range.m_range_ideal);
     } else if (context->attack_range_full) {
      bonus = 10.0f;
     } else if (distance > combat_range.m_range_center) {
      bonus = (10.0f * (distance - combat_range.m_range_center)) / (combat_range.m_range_ideal - combat_range.m_range_center);
     } else {
      penalty = -10.0f * MIN((combat_range.m_range_center - distance) / (combat_range.m_range_center - combat_range.m_range_spread), 1.0f);
     }
     penalty = PIN(penalty, -500.0f, 0.0f);
     actor_firing_position_attack_add_evaluation(context, position, bonus, 14, 0);
     actor_firing_position_attack_add_evaluation(context, position, penalty, 14, 1);
     if (actor_firing_position_reference_frame((const c_ai_point3d *)position->definition) !=
         actor_firing_position_reference_frame(&context->target_point))
     {
      float penalty = -20.0f;
      actor_firing_position_attack_add_evaluation(context, position, penalty, 38, 1);
     }
    }
   }
  }
 }
 if (actor->squad_index != -1 && actor->root_vehicle_index == -1 && actor_get_standing(actor_index) != 3) {
  character_general_properties *properties =
      (character_general_properties *)character_definition_general_properties_get(actor->character_index);
  if (!properties || !(properties->flags & 0x100)) {
   long leader_index = squad_get_leader((short)actor->squad_index);
   if (leader_index != -1) {
    float radius = actor_firing_position_attack_default_radius;
    real_point3d destination;
    actor_get_destination(leader_index, &destination);
    if (properties && properties->leader_distance > 0.0f)
     radius = properties->leader_distance;
    short index;
    firing_position *position;
    for (index = 0, position = positions; index < count; index++, position++) {
     if (position->valid) {
      float distance_squared = distance_squared2d(&destination.xy, &position->position.xy);
      if (distance_squared > (1.1f * radius) * (1.1f * radius)) {
       float penalty = -actor_firing_position_attack_leader_scale * (square_root(distance_squared) - 1.1f * radius);
       penalty = PIN(penalty, -500.0f, 0.0f);
       actor_firing_position_attack_add_evaluation(context, position, penalty, 48, 1);
      }
     }
    }
   }
  }
 }
 short index;
 firing_position *position;
 for (index = 0, position = positions; index < count; index++, position++) {
  if (position->valid && context->attack_test_count > 0) {
   float minimum_distance_squared = actor_firing_position_guard_maximum;
   for (short vector_index = 0; vector_index < context->combat_test_count; vector_index++) {
    if (context->combat_tests[vector_index].type == 2) {
     real_vector3d vector;
     actor_firing_position_combat_vector_from_points3d(&context->combat_tests[vector_index].origin, &position->position, &vector);
     float dot = actor_firing_position_attack_dot(&vector, &context->combat_tests[vector_index].aiming);
     if (dot > 0.0f) {
      actor_firing_position_attack_point_from_line(&vector, &context->combat_tests[vector_index].aiming, -dot, &vector);
      float distance_squared = actor_firing_position_combat_magnitude_squared3d(&vector);
      if (distance_squared < minimum_distance_squared)
       minimum_distance_squared = distance_squared;
     }
    }
   }
   float evaluation = 6.0f;
   if (minimum_distance_squared < 12.25f)
   {
    float distance = square_root(minimum_distance_squared);
    evaluation = distance / 4.0f;
   }
   actor_firing_position_attack_add_evaluation(context, position, evaluation, 16, 0);
  }
 }
}
// @address 0x5621a0 ?new_firing_position@@YA_NJUfiring_position_ref@@PAUfiring_position@@PAUfiring_position_evaluation_context@@@Z
bool new_firing_position(long actor_index, firing_position_ref reference, firing_position *position, firing_position_evaluation_context *context)
{
 bool result = false;
 firing_position_definition *definition = firing_position_ref_get_definition(reference);
 if (definition != 0) {
  position->definition = definition;
  position->original_index = reference;
  position->type = 0;
  position->target_distance = 3.402823466e+38F;
  position->target_direction = *global_zero_vector3d;
  position->evaluation_distance = 3.402823466e+38F;
  position->second = *global_zero_vector3d;
  position->distance = 3.402823466e+38F;
  position->firing_direction = *global_zero_vector3d;
  position->danger_distance_squared = 0.0f;
  position->penalty = 0.0f;
  position->hide_score = 0.0f;
  position->evaluated_penalty = 0.0f;
  position->evaluated_hide_score = 0.0f;
  position->valid = true;
  position->preferred = false;
  position->evaluated = 0;
  position->forced = false;
  position->primary = false;
  position->enabled = false;
  position->special = false;
  position->transparent_line_of_sight = false;
  position->concealed = false;
  position->reachable = false;
  position->cornering = 0;
  ai_point_get_position((c_ai_point3d const *)definition, &position->position);
  if (context != 0)
   runtime_firing_position_setup_cornering_info(actor_index, position, context);
  result = true;
 }
 return result;
}
// ---- 0x55e3b8 pre_evaluator_global ----
// @rdata 0x82024264
static const float actor_firing_position_global_influence_scale = 5.0f;
// @rdata 0x8202490c
static const float actor_firing_position_global_epsilon = 0.0001f;
static const float actor_firing_position_global_danger_penalty = -20.0f;
static const float actor_firing_position_global_influence_reject = -10.0f;
// @rdata 0x82022850
static const float actor_firing_position_global_influence_bonus = 10.0f;
static const float actor_firing_position_global_former_strong_bonus = 100.0f;
static const float actor_firing_position_global_moving_threshold = 0.1f;
// match: noalias declaration preserves caller live registers
static __forceinline real_vector3d *actor_firing_position_global_scale_vector3d(const real_vector3d *vector, float scale, real_vector3d *result)
{
 result->i = vector->i * scale;
 result->j = vector->j * scale;
 result->k = vector->k * scale;
 return result;
}
static __forceinline void actor_firing_position_global_add_score(
    firing_position_evaluation_context *context, firing_position *position, float evaluation, short type, short mode)
{
    position->score += evaluation;
}
static __forceinline void actor_firing_position_global_add_hide_score(
    firing_position_evaluation_context *context, firing_position *position, float evaluation, short type, short mode)
{
    position->hide_score += evaluation;
}
static __forceinline bool actor_firing_position_global_reject(firing_position_evaluation_context *context, firing_position *position, short reason)
{
 position->rejected = true;
 return !context->debug;
}
static __forceinline float actor_firing_position_global_magnitude3d(const real_vector3d *vector)
{
 return sqrtf(actor_firing_position_combat_magnitude_squared3d(vector));
}
static __forceinline float actor_firing_position_global_distance_squared3d(const real_point3d *a, const real_point3d *b)
{
 real_vector3d vector;
 return actor_firing_position_combat_magnitude_squared3d(actor_firing_position_combat_vector_from_points3d(a, b, &vector));
}
// @address 0x55e3b8 ?pre_evaluator_global@@YAXJPAUfiring_position_evaluation_context@@FPAUfiring_position@@@Z
void pre_evaluator_global(long actor_index, firing_position_evaluation_context *context, short count, firing_position *positions)
{
 firing_position *position;
 actor_datum *actor = &((actor_datum *)actor_data->data)[datum_index_to_absolute_index(actor_index)];
 short index;
 real_vector3d danger_vector;
 bool moving_away = false;
 if (context->evaluate_danger_zone) {
  actor_firing_position_combat_vector_from_points3d(&actor->danger_zone.position, &actor->danger_zone.projected_position, &danger_vector);
  if (actor_firing_position_combat_magnitude_squared3d(&danger_vector) > actor_firing_position_global_moving_threshold) {
   real_vector3d direction;
   actor_firing_position_combat_vector_from_points3d(&actor->position.body, &actor->danger_zone.position, &direction);
   normalize3d(&direction);
   moving_away = actor_firing_position_attack_dot(&direction, &danger_vector) > actor_firing_position_global_moving_threshold;
  }
 }
 for (index = 0, position = positions; index < count; index++, position++) {
  if (!position->valid) continue;
  if (actor_firing_position_discarded(actor_index, position->original_index)) {
   if (actor_firing_position_global_reject(context, position, 0)) {
    position->valid = false;
    continue;
   }
  }
  if (!position->enabled && actor->goal_status == 3 && actor->situation.best_prop_class <= 3) {
   if (actor_firing_position_global_reject(context, position, 0)) {
    position->valid = false;
    continue;
   }
  }
  if (context->evaluate_danger_zone) {
   float penalty = 0.0f;
   bool behind = false;
   if (moving_away) {
    real_vector3d direction;
    actor_firing_position_combat_vector_from_points3d(&position->position, &actor->danger_zone.position, &direction);
    normalize3d(&direction);
    if (actor_firing_position_attack_dot(&direction, &danger_vector) > actor_firing_position_global_moving_threshold) behind = true;
   }
   if (!behind) {
    if (actor_firing_position_global_distance_squared3d(&actor->danger_zone.midpoint, &position->position) <
        (actor->danger_zone.extent + 2.5f) * (actor->danger_zone.extent + 2.5f))
    {
     float distance_squared;
     if (actor_firing_position_global_magnitude3d(&danger_vector) > 0.0f)
      distance_squared = point_to_line_distance_squared3d(&position->position, &actor->danger_zone.position, &danger_vector);
     else
     {
      real_vector3d vector;
      actor_firing_position_combat_vector_from_points3d(&position->position, &actor->danger_zone.position, &vector);
      distance_squared = actor_firing_position_combat_magnitude_squared3d(&vector);
     }
     if (distance_squared < actor->danger_zone.radius * actor->danger_zone.radius) {
      if (actor_firing_position_global_reject(context, position, 0)) {
       position->valid = false;
       continue;
      }
      penalty = actor_firing_position_global_danger_penalty;
     }
     else if (distance_squared < (actor->danger_zone.radius + 2.5f) * (actor->danger_zone.radius + 2.5f)) {
      float distance = sqrtf(distance_squared);
      penalty = actor_firing_position_global_danger_penalty * (1.0f - (distance - actor->danger_zone.radius) / 2.5f);
     }
    }
    actor_firing_position_global_add_hide_score(context, position, penalty, 0, 1);
    if (actor->danger_zone.midpoint_distance_squared < (actor->danger_zone.extent + 3.0f) * (actor->danger_zone.extent + 3.0f) &&
        actor->danger_zone.distance > actor->danger_zone.radius)
    {
     float line_distance_squared = point_to_line_distance_squared3d(&actor->position.body, &actor->danger_zone.position, &danger_vector);
     if (line_distance_squared > actor->danger_zone.radius * actor->danger_zone.radius) {
      real_vector3d movement;
      actor_firing_position_global_scale_vector3d(&position->target_direction, 3.0f, &movement);
      if (actor_firing_position_combat_magnitude_squared3d(&movement) > 0.0001f &&
          vector_to_line_distance_squared3d(&actor->position.body, &movement, &actor->danger_zone.position, &danger_vector) <
              actor->danger_zone.radius * actor->danger_zone.radius)
          {
       if (actor_firing_position_global_reject(context, position, 0)) {
        position->valid = false;
        continue;
       }
      }
     }
    }
   }
  }
  if (context->influence_count > 0) {
   float positive_distance = 1.0f;
   float negative_distance = 1.0f;
   float positive = 0.0f;
   float negative = 0.0f;
   bool reject = false;
   for (short influence_index = 0; influence_index < context->influence_count; ++influence_index) {
    if (context->influences[influence_index].radius > 0.0f) {
     float distance_squared = actor_firing_position_global_distance_squared3d(&position->position, &context->influences[influence_index].position);
     float scaled_distance = distance_squared / (context->influences[influence_index].radius * context->influences[influence_index].radius);
     if (scaled_distance < 1.0f && context->influences[influence_index].reject) {
      reject = true;
     }
     else {
      float *minimum = context->influences[influence_index].positive ? &positive_distance : &negative_distance;
      if (scaled_distance < *minimum) *minimum = scaled_distance;
     }
    }
   }
   if (reject) {
    if (actor_firing_position_global_reject(context, position, 0)) {
     position->valid = false;
     continue;
    }
    negative = actor_firing_position_global_influence_reject;
   }
   else {
    if (negative_distance < 1.0f) {
     negative = -actor_firing_position_global_influence_scale;
     negative += sqrtf(1.0f - negative_distance) * (actor_firing_position_global_influence_reject + actor_firing_position_global_influence_scale);
    }
    if (positive_distance < 1.0f) {
     positive = sqrtf(1.0f - positive_distance) * actor_firing_position_global_influence_bonus;
    }
   }
   actor_firing_position_global_add_hide_score(context, position, negative, 0, 1);
   actor_firing_position_global_add_score(context, position, positive, 0, 0);
  }
 }
 for (index = 0, position = positions; index < count; index++, position++) {
  if (!position->valid) continue;
  if (context->prefer_flags && (context->preferred_flags & position->definition->flags) == position->definition->flags) {
   float evaluation = 4.0f;
   actor_firing_position_global_add_score(context, position, evaluation, 0, 0);
  }
  if (position->special && (context->standing == 3 || context->standing == 2)) {
   float evaluation = 4.0f;
   actor_firing_position_global_add_score(context, position, evaluation, 0, 0);
  }
  if (position->concealed && context->penalize_unconcealed) {
   float evaluation = 4.0f;
   actor_firing_position_global_add_score(context, position, evaluation, 0, 0);
  }
  if (actor->goal_status != 3 && position->enabled && actor->root_vehicle_index == -1) {
   float evaluation = 4.0f;
   actor_firing_position_global_add_score(context, position, evaluation, 0, 0);
  }
  if (actor->flying && !position->primary && !actor->searching) {
   float evaluation = 4.0f;
   actor_firing_position_global_add_score(context, position, evaluation, 0, 0);
  }
 }
 if (context->combat_disabled) {
  long object_index = actor->root_vehicle_index != -1 ? actor->root_vehicle_index : actor->unit_index;
  object_datum *object = (object_datum *)((s_object_header *)object_header_data->data)[datum_index_to_absolute_index(object_index)].datum;
  bool moving;
  real_point3d origin;
  if (context->require_facing) {
   moving = true;
  }
  else {
   real_vector3d velocity;
   object_get_velocities(object_index, &velocity, 0);
   moving = actor_firing_position_combat_magnitude_squared3d(&velocity) > 6.25f;
  }
  object_get_origin(actor->unit_index, &origin);
  for (index = 0, position = positions; index < count; index++, position++) {
   if (position->valid) {
    real_vector3d direction;
    actor_firing_position_combat_vector_from_points3d(&origin, &position->position, &direction);
    float distance_squared = actor_firing_position_combat_magnitude_squared3d(&direction);
    if (!(fabsf(distance_squared - 0.0f) < actor_firing_position_global_epsilon) && distance_squared < 900.0f) {
     float cosine = actor_firing_position_attack_dot(&direction, &object->forward) / sqrtf(distance_squared);
     if (moving && distance_squared < 64.0f && cosine < 0.70710677f) {
      if (actor_firing_position_global_reject(context, position, 0)) {
       position->valid = false;
       continue;
      }
     }
     float weight = context->facing_weight > 0.0f ? context->facing_weight / 2.0f : 5.0f;
     float evaluation = weight;
     if (cosine < 0.0f) evaluation -= weight * (cosine - 0.0f) / -1.0f;
     else if (cosine > 0.8660254f) evaluation += weight * (cosine - 0.8660254f) / 0.13397461f;
     actor_firing_position_global_add_score(context, position, evaluation, 0, 0);
    }
   }
  }
 }
 if (context->former_enabled) {
  short former_index;
  firing_position *former_position;
  for (former_index = 0, former_position = positions; former_index < count; former_index++, former_position++) {
   if (former_position->valid && fp_ref_equals(context->former_position_index, former_position->original_index)) {
    actor_firing_position_global_add_score(context, former_position, context->former_strongly ? actor_firing_position_global_former_strong_bonus
        : 10.0f, 0, 0);
   }
  }
 }
}

static __forceinline bool actor_firing_position_obstacles_is_giant(long index)
{
 return actor_is_giant(actor_firing_position_leader_datum_get(actor_data, index));
}
static __forceinline bool actor_firing_position_obstacles_sign(const real_vector3d *normal, short axis)
{
 return normal->n[axis] > 0.0f;
}
static __forceinline float actor_firing_position_obstacles_magnitude_squared2d(const real_vector2d *vector)
{
    return vector->i * vector->i + vector->j * vector->j;
}
static __forceinline float actor_firing_position_obstacles_distance2d(const real_point2d *a, const real_point2d *b)
{
 real_vector2d vector;
 vector.i = b->x - a->x;
 vector.j = b->y - a->y;
 return sqrtf(actor_firing_position_obstacles_magnitude_squared2d(&vector));
}
// @address 0x55ecb8 ?pre_evaluator_obstacles@@YAXJPAUfiring_position_evaluation_context@@FPAUfiring_position@@@Z
void pre_evaluator_obstacles(long actor_index, firing_position_evaluation_context *context, short count, firing_position *positions)
{
 real_vector3d normal;
 special_movement movement_data;
 obstacles obstacle_list;
 actor_datum *actor = actor_firing_position_leader_datum_get(actor_data, actor_index);
 short projection_axis = 2;
 bool projection_sign = true;
 long obstacle_flags = 0x181;
 long ignored_object_index = actor->unit_index;
 if (actor->root_vehicle_index == -1) obstacle_flags = 0x187;
 actor_find_pathfinding_location(actor_index);
 c_sector_ref *sector_ptr = &actor->pathfinding_sector;
 if (sector_ptr->valid()) {
  get_sector_normal(pathfinding_get_sector(*sector_ptr), &normal, true);
  float x = fabsf(normal.i);
  float y = fabsf(normal.j);
  float z = fabsf(normal.k);
  if (!(z < y) && !(z < x)) projection_axis = 2;
  else { if (y >= x) projection_axis = 1; else projection_axis = 0; }
  projection_sign = actor_firing_position_obstacles_sign(&normal, projection_axis);
 }
 if (!actor_firing_position_obstacles_is_giant(actor_index) && actor->driver != 0) ignored_object_index = actor->root_vehicle_index;
 float speed = 0.0f;
 if (actor->driver == 0 && !actor_firing_position_obstacles_is_giant(actor_index)) {
  character_movement_properties *props = character_definition_movement_properties_get(actor->character_index);
  if (props != 0) speed = props->pathfinding_radius;
 } else {
  character_vehicle_properties *vprops = character_vehicle_properties_get(actor_index, *(long *)((s_object_header
      *)object_header_data->data)[datum_index_to_absolute_index(ignored_object_index)].datum);
  if (vprops != 0) speed = vprops->pathfinding_radius;
 }
 speed = speed > 0.2f ? speed : 0.2f;
 obstacle_list.obstacle_count = 0;
 obstacle_list.disc_count = 0;
 obstacle_list.flag1_count = 0;
 obstacle_list.flag8_count = 0;
 obstacle_list.unused = 0;
 actor_special_movement_new(actor_index, &movement_data);
 float limit;
 if (actor->root_vehicle_index == -1) limit = 10.0f;
 else limit = 20.0f;
 obstacles_add_nearby_discs(actor_index, obstacle_flags, projection_axis, projection_sign, &movement_data, &obstacle_list, 0,
     &actor->position.body, 0, limit, global_zero_vector3d, 0, ignored_object_index, -1);
 obstacles_recompute(&obstacle_list, speed);
 short obstacle_count = obstacle_list.disc_count;
 if (obstacle_count > 0 && count > 0) {
  short index = 0;
  firing_position *position = positions;
  for (; index < count; index++, position++) {
   bool blocked = false;
   if (!position->valid) continue;
   if (obstacle_count > 0) {
    short disc_index = 0;
    for (; disc_index < obstacle_count; disc_index++) {
     const obstacle_disc *disc = &obstacle_list.discs[disc_index];
     if (disc->radius + speed - actor_firing_position_obstacles_distance2d(&position->position.xy, &disc->center) > speed) {
      blocked = true;
      break;
     }
    }
   }
   if (blocked) {
    position->rejected = true;
    if (context->debug == 0) position->valid = false;
   }
  }
 }
}

static inline long actor_firing_position_pursuit_is_static(firing_position *position)
{
 return (position->flags & 0x8000) == 0 || fp_ref_equals(position->original_index, FP_REF_CURRENT);
}
// @address 0x55f288 ?pre_evaluator_pursuit@@YAXJPAUfiring_position_evaluation_context@@FPAUfiring_position@@@Z
void pre_evaluator_pursuit(long actor_index, firing_position_evaluation_context *context, short count, firing_position *positions)
{
 long prop_index = -1;
 short last_block = -1;
 short last_area = -1;
 bool discarded = false;
 bool second_direction = false;
 bool first_direction = false;
 if (context->hint && magnitude_squared3d(&context->target_hint_vector) > 0.0f) {
  second_direction = context->has_second_direction;
  first_direction = context->has_firing_direction;
 }
 if (context->panic) prop_index = context->panic_prop_index;
 for (short index = 0; index < count; index++, positions++) {
  if (prop_index != -1) {
   if (actor_firing_position_pursuit_is_static(positions) &&
       !fp_ref_equals(positions->original_index, FP_REF_CURRENT)) {
    unsigned short flags = positions->flags;
    short area = -1;
    firing_position_definition *element = firing_position_ref_get_definition(positions->original_index);
    if (element) area = element->area;
    if ((short)flags != last_block || area != last_area) {
     prop_ref_datum *prop = &((prop_ref_datum *)tls_prop_ref_data()->data)[datum_index_to_absolute_index(prop_index)];
     discarded = pursuit_area_discarded(prop->prop_index, flags, area);
     last_block = flags;
     last_area = area;
    }
    if (discarded) {
     if (actor_firing_position_reject(context, positions)) { positions->valid = false; continue; }
    }
   } else {
    if (actor_firing_position_reject(context, positions)) { positions->valid = false; continue; }
   }
  }
  if (!positions->valid) continue;
  float evaluation = 5.0f;
  if (context->danger_maximum) evaluation = 1.0f;
  if (!(positions->target_distance < context->panic_maximum_distance / 2.0f)) {
   if (!(positions->target_distance >= context->panic_maximum_distance))
    evaluation *= (context->panic_maximum_distance - positions->target_distance) * (1.0f / (context->panic_maximum_distance / 2.0f));
   else
    evaluation = 0.0f;
  }
  float score = evaluation + positions->score;
  positions->score = score;
  if (context->evaluate_danger) {
   actor_datum *actor = actor_firing_position_leader_datum_get(actor_data, actor_index);
   float limit = actor_is_giant(actor) ? 50.0f : 20.0f;
   if (!(positions->evaluation_distance >= limit)) {
    positions->score = (limit - positions->evaluation_distance) / limit * 5.0f + score;
   } else if (!context->danger_maximum) {
    if (actor_firing_position_reject(context, positions)) { positions->valid = false; continue; }
   }
   if (first_direction) {
    float current;
    float dot = dot_product3d(&positions->firing_direction, &context->target_hint_vector);
    float value = 0.0f;
    if (!(dot >= 0.0f)) {
     if (actor_firing_position_reject(context, positions)) { positions->valid = false; continue; }
    } else {
     float scaled = dot * 1.4142135f;
     if (!(scaled >= 0.0f)) scaled = 0.0f;
     value = scaled * 15.0f;
    }
    current = positions->score;
    positions->score = value + current;
   }
   if (second_direction) {
    float dot = dot_product3d(&positions->second, &context->target_hint_vector);
    if (!(dot >= 0.0f)) {
     if (actor_firing_position_reject(context, positions)) positions->valid = false;
     continue;
    }
    float scaled = dot * 1.4142135f;
    if (!(scaled >= 0.0f)) scaled = 0.0f;
    actor_firing_position_uncover_add_evaluation(context, positions, scaled * 15.0f, 4, false);
   }
  }
 }
}

struct path_error_info;
// @rdata 0x820271bc
static const float actor_firing_position_cornering_ray_length = 0.35f;
// @rdata 0x8202b10c
static const float actor_firing_position_cornering_back_distance = 0.15f;
// @rdata 0x82033bc0
static const float actor_firing_position_cornering_side_distance = 0.25f;
// @rdata 0x8202251c
static const float actor_firing_position_cornering_zero = 0.0f;
static __forceinline float actor_firing_position_cornering_dot(const real_vector3d *a, const real_vector3d *b)
{
 return a->i * b->i + (a->j * b->j + a->k * b->k);
}
static __forceinline float actor_firing_position_manual_cornering_dot(const real_vector3d *a, const real_vector3d *b)
{
 return a->i * b->i + (a->k * b->k + a->j * b->j);
}
static __forceinline bool actor_firing_position_cornering_world_frame(short frame)
{
 // match: 0x561cd0: temporary keeps the zero-extended word compare; direct short compare emits different bytes
 const unsigned long &index = (unsigned short)frame;
 return index == 0xffff;
}
static __forceinline bool actor_firing_position_cornering_info(pathfinding_data *data, long link_index, const c_ai_point3d *point,
    real_vector3d *direction, real_point3d *corner, real_vector3d *normal, char *type)
{
 return link_index >= 0 ? runtime_cornering_get_info(data, link_index, point, direction, corner, normal, type) : false;
}
static __forceinline void actor_firing_position_cornering_test_point(
    const real_point3d *point, const real_vector3d *normal, short type, bool left, real_point3d *result)
{
 real_vector3d side;
 if (left) {
  side.i = -normal->j;
  side.j = normal->i;
 } else {
  side.i = normal->j;
  side.j = -normal->i;
 }
 side.k = 0.0f;
 point_from_line3d(point, normal, -actor_firing_position_cornering_back_distance, result);
 switch (type) {
 case 1: point_from_line3d(result, &side, -actor_firing_position_cornering_side_distance, result); break;
 case 2: point_from_line3d(result, &side, actor_firing_position_cornering_side_distance, result); break;
 }
}
// @address 0x561cd0
void runtime_firing_position_setup_cornering_info(long actor_index, firing_position *position, firing_position_evaluation_context *context)
{
 if (!context->evaluate_cornering) return;
 firing_position_definition *definition = position->definition;
 short reference_frame = definition->attachment_index;
 if (actor_firing_position_cornering_world_frame(reference_frame) &&
     ((definition->flags & 16) || definition->unknown) &&
     !fp_ref_equals(position->original_index, FP_REF_CURRENT)) {
  c_ai_point3d corner;
  bool found = false;
  char type;
  real_vector3d normal;
  bool direction_valid = false;
  real_vector3d direction;
  bool evaluate_danger = context->evaluate_danger;
  if (evaluate_danger) {
   ai_vector_from_points3d(&context->target_position, (const c_ai_point3d *)definition, &direction);
   scale_vector3d(&direction, -1.0f, &direction);
   direction_valid = true;
  } else if (context->guard) {
   direction = context->guard_plane;
   direction_valid = true;
  } else {
   direction_valid = false;
  }
  if (direction_valid && normalize3d(&direction) > actor_firing_position_cornering_zero) {
   if ((position->flags & 0x8000) == 0) {
    if (definition->unknown) {
     if (runtime_manual_cornering_get_info(actor_index, position->original_index, &corner, &normal, &type) &&
         actor_firing_position_manual_cornering_dot(&direction, &normal) > 0.70710677f)
      found = true;
    } else {
     path_collision_result collision;
     if (pathfinding_test_vector(&definition->position, definition->sector_ref, g_sector_none, &direction,
         actor_firing_position_cornering_ray_length, 0, &collision) &&
         collision.collision && collision.link_index != -1 && collision.link_index != 0xffff) {
      pathfinding_data *data = pathfinding_data_get(collision.sector.structure_bsp_index);
      sector_link *link = &((sector_link *)data->links.address)[collision.link_index];
      if (link->flags & (1 << 9)) {
       real_point3d point;
       if (actor_firing_position_cornering_info(data, collision.link_index, (const c_ai_point3d *)position->definition, &direction, &point,
           &normal, &type)) {
        corner.set(&point, position->definition->attachment_index);
        found = true;
       }
      }
     }
    }
   } else if (context->dynamic_firing_positions_valid) {
    real_vector3d marker_normal;
    short marker_index = position->area;
    scale_vector3d(&context->cornering_markers[marker_index].matrix.up, -1.0f, &marker_normal);
    marker_normal.k = 0.0f;
    if (normalize3d(&marker_normal) > actor_firing_position_cornering_zero &&
        (!evaluate_danger || actor_firing_position_cornering_dot(&marker_normal, &direction) > 0.70710677f)) {
     real_point3d point;
     if (ai_reference_frame_inverse_transform_point(reference_frame, &context->cornering_markers[marker_index].matrix.position, &point) &&
         ai_reference_frame_inverse_transform_vector(reference_frame, &marker_normal, &normal)) {
      type = (char)context->cornering_marker_types[marker_index];
      corner.set(&point, reference_frame);
      found = true;
     }
    } else {
     position->cornering = 0;
    }
   }
   if (found && type != 0) {
    bool left = context->type == 0 || context->type == 3;
    position->cornering = type;
    position->cornering_left = left;
    position->cornering_point = corner;
    position->cornering_normal = normal;
    if (type == 1 || type == 2) {
     real_point3d point;
     actor_firing_position_cornering_test_point(corner.get_point(), &normal, type, left, &point);
     ai_reference_frame_transform_point(position->definition->attachment_index, &point, &position->position);
    }
   }
  }
 }
}
