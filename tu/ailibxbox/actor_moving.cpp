// ailibxbox/actor_moving.obj: from the records 0xb3838 0xb7c88 0xb7dd0 0xb8308 0xb8a00 0xb8c70 0xb9dd8 0xbb3b0 0xbb720 0xbbbd0 0xbbf40 0xc1570 0xc4f50 0xc7868 0xc78e0 0xc7aa0 0xca9f8 0xcc5b0 0xccbf8 0xccf60 0xcd3d8 0xd0fb8 0xd10f0 0x11e1f8 0x11ea28 0x11eac8 0x11eba8 0x11ed38 0x515190 0x515218 0x515290 0x5153e8 0x515560 0x515638 0x5157b0 0x515870 0x515a18 0x515ae8 0x515bd0 0x515f70 0x516228 0x516360 0x516748 0x516ac0 0x516c40 0x517238 0x5172f8 0x517438 0x5174e0 0x517540 0x518118 0x5182d8 0x518400 0x518520 0x518650 0x518688 0x5189b8 0x530cf0
#include "memory/data.h"
#include "ai/clump_iterator.h"
#include "ai/actor_behavior_state.h"
#include "ai/squad_data.h"
#include "math/real_constant_pool.h"
#include "ai/actor_queries.h"
#include "tag_files/string_ids.h"
#include "ai/behavior_cover_state.h"
#include "ai/firing_position_types.h"
#include "ai/actor_moving_globals.h"
#include "ai/command_script_data.h"
#include "game/game_allegiance_types.h"
#include "ai/actor_firing_position_selection.h"
#include "ai/pathfinding_geometry.h"
#include "ai/ai_globals_definition.h"
#include "units/biped_definitions.h"
#include "objects/object_definitions.h"
#include "ai/command_script_types.h"
#include "units/biped_types.h"
#include "memory/tls_globals.h"
#include "ai/actor_type_flood_types.h"
#include "ai/path_types.h"
#include "physics/havok_component.h"
#include "physics/havok_component_class.h"
#include "cseries/long_sort.h"
#include "math/matrix_math_types.h"
#include "physics/collision_types.h"
#include "game/game_globals_storage.h"
#include "memory/data_types.h"
#include "objects/object_types.h"
#include "tag_files/tag_types.h"
#include "game/game_types.h"
#include "units/motor_enums.h"
#include "ai/ai_point3d.h"
#include "ai/squad_types.h"
#include "ai/prop_ref_datum.h"
#include "ai/clump_datum.h"
#include <float.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include "cseries/crt_xdk.h"
#include "math/real_math.h"
#include "game/game_globals.h"
#include "ai/actors.h"
#include "motor/action_system.h"
#include "motor/motor_animation.h"
#include "objects/objects.h"
#include "units/units.h"
#include "units/unit_types.h"
#include "ai/actor_looking.h"
#include "ai/actor_moving.h"
#include "scenario/scenario.h"
#include "ai/actor_perception.h"
#include "ai/ai.h"
#include "ai/ai_actions.h"
#include "ai/ai_hint_definitions.h"
#include "ai/ai_player.h"
#include "ai/ai_reference_frame.h"
#include "units/motor_task.h"
#include "units/action_request.h"
#include "ai/path_obstacle_types.h"
#include "ai/ai_player_types.h"
#include "ai/behavior.h"
#include "ai/path.h"
#include "ai/path_flying.h"
#include "ai/props.h"
#include "ai/sector.h"
#include "ai/squads.h"
#include "items/projectiles.h"
#include "motor/motor_system.h"
#include "render/render_objects_static_lighting.h"
#include "units/unit_animation.h"
#include "objects/object_datum.h"
#include "ai/pathfinding_utilities.h"
#include "ai/character_definitions.h"
#include "game/game_allegiance.h"
#include "ai/clump.h"
#include "math/real_math_types.h"
#include "game/player_types.h"
#include "ai/prop_state.h"
#include "ai/prop_datum.h"
#include "animations/animation_manager.h"
#include "ai/actor_datum.h"
#include "ai/pathfinding_mode_types.h"
#include "cache/cache_files.h"
#include "ai/actor_moving_giant_scarab.h"
#include "ai/actor_moving_vehicle_directional_flying.h"
#include "ai/actor_moving_vehicle_nondirectional_flying.h"
#include "ai/special_movement_types.h"
#include "ai/behavior_posture_info_types.h"
#include "ai/firing_position_definition.h"
#include "ai/path_state.h"
#include "ai/path_state_types.h"
#include "ai/firing_position_accessors.h"
#include "ai/pathfinding_collision.h"
#include "ai/character_properties_types.h"
#include "objects/object_header.h"
#include "ai/firing_position_ref.h"
#include "ai/hint_ref.h"
#include "ai/pathfinding_data.h"
#include "ai/sector_analysis.h"
#include "memory/tls_data_arrays.h"
#include "ai/path_result.h"
#include "ai/pureform_types.h"
#include "ai/squad_area_iterator.h"
#include "ai/objective_definitions.h"
#include "ai/actor_firing_position_queries.h"
#include "ai/character_idle_properties.h"
#include "game/game_engine_team.h"
#include "ai/actor_moving_types.h"
#include "ai/actor_moving_ground.h"
#include "physics/collision_vector_query.h"
#include "tag_files/tag_groups.h"
#include "ai/pathfinding_pill_result.h"
#include "ai/character_weapon_properties.h"
#include "motor/mover_inline.h"
#include "game/game_time_inline.h"
#include "math/random_math_range.h"

// @compiler xdk6021
// @flags /TP /O2 /Oc
// @address 0x73d02c g_actor_berserk_duration_limit
// @address 0x518520 ?actor_path_step_get_vector@@YA_NJFPATreal_vector3d@@@Z
// @address 0x11eac8 ?clear@c_destination_orders@@QAAXXZ

struct s_task;


// match: 0xb7dd0: retain the orders subobject pointer across path_step_finish/path_finish.
struct actor_advance_path_state
{
    c_destination_orders orders;
    unsigned short state;
    bool path_refresh_disabled;
    short path_refresh_failures;
    actor_path path;
};
struct actor_camera_data
{
    real_point3d position;
    real_point3d focus;
};
struct moving_prop_ref_iterator
{
    long next_index;
};
enum e_deceleration_obstacle_results { _deceleration_obstacle_none };
union path_refresh_scratch
{
    actor_moving_reserved_hints reserved;
    real_point3d point;
};

// @address 0x7c37a4 g_actor_moving_destination_direction_threshold
float g_actor_moving_destination_direction_threshold = 0.70710677f;

// match: 0xb8308: the crouch test re-reads the actor datum through this copy; datum_get<actor_datum> there swaps the
// x/y load order of the position delta (actor+0x358 / +0x7f8) later in actor_move_update.
// match: 0xbb720: datum_get<actor_datum> for the obstacle actor swaps r10/r11 around the clrlwi/mulli 2700 index math.
static __forceinline actor_datum *actor_moving_deceleration_movement_actor_get(long index)
{
    return &((actor_datum *)actor_data->data)[DATUM_INDEX_TO_ABSOLUTE_INDEX(index)];
}
static __forceinline actor_datum *actor_move_update_actor_get(long actor_index)
{
    return &((actor_datum *)actor_data->data)[DATUM_INDEX_TO_ABSOLUTE_INDEX(actor_index)];
}

static __forceinline float actor_move_dot3d(const real_vector3d *a, const real_vector3d *b)
{
    return a->i * b->i + a->j * b->j + a->k * b->k;
}
static __forceinline float actor_move_dot2d(const real_vector3d *a, const real_vector3d *b)
{
    return a->i * b->i + a->j * b->j;
}


bool c_destination_orders::ground_destination()
{
    bool result = true;
    switch (type)
    {
    case 0:
    case 1:
        result = true;
        break;
    case 2:
        result = false;
        break;
    case 3:
        result = !flags.test(_destination_flag_6) && !flags.test(_destination_flag_5);
        break;
    case 4:
        result = true;
        break;
    default:
        // match: 0xc7868: drops the cmplwi/bgt range check on type ahead of the jump table
        __assume(0);
        break;
    }
    return result;
}

extern "C" {
// @address 0x239e0 k_behavior_posture_info
}
static __forceinline long type_entry_find(long type)
{
  long result = 0;
  for (short i = 0; i < 60; i++) {
    if (k_behavior_posture_info[i].name == type)
      result = i;
  }
  return result;
}
// @address 0xb3838 actor_get_posture
extern "C" long actor_get_posture(unsigned short object_index)
{
  long result = 0;
  actor_datum *object = datum_get<actor_datum>(actor_data, object_index);
  if (object->unit_index != -1) {
    int first, second;
    if (::motor_animation_get_mode_and_state(object->unit_index, (long *)(&first), (long *)(&second)))
      result = type_entry_find(first);
  }
  return result;
}

__forceinline float actor_moving_plane_distance(const real_plane3d *plane, const real_point3d *point)
{
    return plane->normal.i * point->x + plane->normal.j * point->y + plane->normal.k * point->z - plane->d;
}

// @address 0x11e1f8 actor_get_destination
extern "C" void actor_get_destination(unsigned short player_index, real_point3d *result)
{
    actor_datum *player = datum_get<actor_datum>(actor_data, player_index);
    bool attached = false;
    if (player->path.valid && player->path_state == 1)
        attached = true;
    if (attached) {
        ::ai_point_get_position(&player->path.destination, result);
        return;
    }
    *result = player->position.body;
}


static __forceinline void actor_moving_point_set(c_ai_point3d *point, const real_vector3d *vector)
{
    memcpy(&point->point, vector, sizeof(point->point));
    point->location[0] = -1;
    point->location[1] = -1;
}

static __forceinline void actor_moving_direction_clear(c_ai_direction *direction)
{
    direction->type = -1;
    actor_moving_point_set(&direction->m_point, global_forward3d);
}

// match: 0x11eac8: action clear reads global_zero_vector3d and stores the direction's
// location -1/-1; c_ai_action::clear() / c_ai_direction::clear() in ai/ai_actions.h do neither.
static __forceinline void actor_moving_action_clear(c_ai_action *action)
{
    action->type = 0;
    action->point_valid = false;
    action->b5 = false;
    action->ticks = 0;
    action->posture_name = 0;
    actor_moving_point_set(&action->point, global_zero_vector3d);
    actor_moving_direction_clear(&action->direction);
}

static __forceinline void actor_moving_firing_flag_set(unsigned short *flags, int mask)
{
    *flags = *flags | mask;
}

// @address 0x11ea28 ?to_firing_position@c_destination_orders@@QAA_NUfiring_position_ref@@@Z
bool c_destination_orders::to_firing_position(firing_position_ref position_reference)
{
    firing_position_definition *position_definition;
    short position_flags;
    bool result;
    result = 0;
    clear();
    position_definition = firing_position_ref_get_definition(position_reference);
    if (!((position_definition == 0))) {
        firing_position = position_reference;
        type = 3;
        position_flags = position_definition->flags;
        if (!(((position_flags & 32) == 0))) {
            flags.m_storage |= 32;
            result = true;
        }
        else if (!(((position_flags & 64) != 0))) {
            actor_moving_firing_flag_set((unsigned short *)&flags.m_storage, 64);
            actor_moving_firing_flag_set((unsigned short *)&flags.m_storage, 4);
        }
        result = true;
    }
    return result;
}

// @address 0x11eac8 ?clear@c_destination_orders@@QAAXXZ
void c_destination_orders::clear()
{
    type = 0;
    flags.m_storage = 0;
    memset(&point_destination, 0, sizeof(point_destination));
    memset(&firing_position_index, 0, sizeof(firing_position_index));
    memset(&prop_ref_index, 0, sizeof(prop_ref_index));
    target_index = -1;
    f1c = 0.0f;
    f20 = 0.0f;
    f24 = 0.0f;
    f28 = 0.0f;
    actor_moving_action_clear(&action);
    m_destination_facing = *global_zero_vector3d;
}


__forceinline pathfinding_hint *actor_moving_get_hint(const c_hint_ref *ref)
{
    pathfinding_data *data = pathfinding_data_get(ref->structure_bsp_index);
    return &((pathfinding_hint *)data->hints.address)[ref->hint_index];
}
__forceinline void actor_moving_flags_set(unsigned long *flags, long bit, bool value)
{
    if (value)
        *flags |= 1 << bit;
    else
        *flags &= ~(1 << bit);
}
__forceinline void actor_moving_unit_pathfinding_attach(long index, c_sector_ref ref)
{
    unit_datum *unit = (unit_datum *)object_get(index);
    actor_moving_flags_set(&unit->unit.flags400, 0, true);
    unit->unit.pathfinding_attached_sector = ref;
}
__forceinline void actor_moving_unit_pathfinding_detach(long index)
{
    unit_datum *unit = (unit_datum *)object_get(index);
    actor_moving_flags_set(&unit->unit.flags400, 0, false);
    unit->unit.pathfinding_attached_sector.clear();
}


static __forceinline real_vector3d *actor_move_set_vector3d(real_vector3d *v, float i, float j, float k)
{
    v->i = i;
    v->j = j;
    v->k = k;
    return v;
}

#define actor_move_build_axes_from_forward(facing, out_left, out_up) \
    do { \
        real_vector3d forward_out_; \
        real_vector3d_build_axes_from_vectors((facing), &k_real_math_identity_matrix_storage.up, \
            &forward_out_, (out_left), (out_up)); \
    } while (0)

// @address 0xbbbd0 ?actor_move_calculate_free@@YAXJ_NPBTreal_vector3d@@1PAT1@@Z
void actor_move_calculate_free(long actor_index, bool three_d, union real_vector3d const *movement_direction,
    union real_vector3d const *facing_direction, union real_vector3d *result)
{
    if (three_d) {
        real_vector3d left, up;
        actor_move_build_axes_from_forward(facing_direction, &left, &up);
        result->i = actor_move_dot3d(movement_direction, facing_direction);
        result->j = actor_move_dot3d(movement_direction, &left);
        result->k = actor_move_dot3d(movement_direction, &up);
        normalize3d(result);
    } else {
        real_vector3d left;
        actor_move_set_vector3d(&left, -facing_direction->j, facing_direction->i, 0.0f);
        result->i = actor_move_dot2d(movement_direction, facing_direction);
        result->j = actor_move_dot2d(movement_direction, &left);
        result->k = 0.0f;
        normalize3d(result);
    }
}



static __forceinline prop_state *prop_state_get_inline(const prop_ref_datum *prop_ref)
{
    prop_state *result;
    if (prop_ref->view_index != -1)
        result = &datum_get<tracking_datum>(TLS_PROP_DATA, prop_ref->view_index)->state;
    else
        result = &datum_get<prop_datum>(prop_data, prop_ref->prop_index)->state;
    return result;
}

// @address 0xbbf40 ?to_prop@c_destination_orders@@QAA_NJ@Z
bool c_destination_orders::to_prop(long prop_ref_index)
{
    prop_ref_datum *prop_ref = datum_get<prop_ref_datum>(prop_ref_data, prop_ref_index);
    prop_state *state = prop_state_get_inline(prop_ref);
    clear();
    type = 4;
    this->prop_ref_index = prop_ref_index;
    target_index = state->object_index != -1 ? state->object_index : prop_ref->object_index;
    return true;
}

// @address 0xc1570 ?actor_move_animation_busy@@YA_NJ@Z
bool actor_move_animation_busy(long actor_index)
{
    actor_datum *actor = datum_get<actor_datum>(actor_data, actor_index);
    return actor->unit_index != -1 && motor_animation_busy(actor->unit_index);
}

// @address 0x515190 ?to_point@c_destination_orders@@QAA_NPAVc_ai_point3d@@PAVc_sector_ref@@@Z
bool c_destination_orders::to_point(c_ai_point3d *point, c_sector_ref *sector)
{
    bool result = false;
    clear();
    if (sector->valid()) {
        type = 1;
        point_destination.point = *point;
        point_destination.sector = *sector;
        result = true;
    }
    return result;
}

// @address 0x515218 ?to_air_point@c_destination_orders@@QAA_NPAVc_ai_point3d@@@Z
bool c_destination_orders::to_air_point(c_ai_point3d *point)
{
    clear();
    type = 2;
    point_destination.point = *point;
    point_destination.sector.clear();
    return true;
}

// @address 0x515638 ?path_step_finish@@YAXJF@Z
void path_step_finish(long actor_index, short step_index)
{
    actor_datum *actor = datum_get<actor_datum>(actor_data, actor_index);
    actor_path_point *step = &actor->path.points[step_index];
    switch (step->frame_index) {
    case 8:
        if (actor->unit_index != -1 && step->hint.valid() && step->sector.valid()) {
            pathfinding_hint *hint = actor_moving_get_hint(&step->hint);
            if ((short)hint->raw_data[1] & 1) {
                c_sector_ref ref;
                ref.set(step->hint.structure_bsp_index, (unsigned short)hint->raw_data[0]);
                actor_moving_unit_pathfinding_attach(actor->unit_index, ref);
            } else
                actor_moving_unit_pathfinding_detach(actor->unit_index);
        }
        break;
    }
}

// @address 0x515ae8 actor_persistent_override_get_movement
// match: 0x515ae8: keeps the actor flag and short field reads after the movement stores; without it their registers swap and the short test loses its extsh
extern "C" bool actor_persistent_override_get_movement(long index, real_vector3d *__restrict movement)
{
    actor_datum *a = datum_get<actor_datum>(actor_data, index);
    bool result = false;
    if (a->flags & 32) {
        result = true;
        if (actor_moving_plane_distance(&a->persistent_movement.plane, &a->position.body) > a->persistent_movement.tolerance)
            result = false;
        else if (a->persistent_movement.ticks == 0)
            result = false;
        else if (a->flags & 8)
            result = false;
        else {
            *movement = a->persistent_movement.plane.normal;
            if (a->persistent_movement.ticks > 0)
                --a->persistent_movement.ticks;
        }
        if (!result) {
            a->flags &= ~32;
            a->persistent_movement.ticks = 0;
        }
    }
    return result;
}

__forceinline unsigned char actor_moving_path_executing(long actor_index)
{
    actor_datum *actor = datum_get<actor_datum>(actor_data, actor_index);
    bool executing = false;
    if (actor->path.valid != 0 && actor->path_state == 1)
        executing = true;
    return executing;
}

static __forceinline void actor_moving_qsort_4byte(void *base, unsigned long count, bool (*compare)(long, long, const void *), const void *context)
{
    if (count >= 2)
        templated_sort<long>::qsort((long *)base, count, compare, context);
}

static __forceinline float actor_moving_reserve_distance3d(const real_point3d *a, const real_point3d *b)
{
    float dx = b->x - a->x;
    float dy = b->y - a->y;
    float dz = b->z - a->z;
    return sqrtf(dx * dx + dy * dy + dz * dz);
}

static __forceinline float actor_moving_reserve_actor_distance3d(const real_point3d *a, const real_point3d *b)
{
    float dx = b->x - a->x;
    float dy = b->y - a->y;
    float dz = b->z - a->z;
    return sqrtf(dy * dy + (dz * dz + dx * dx));
}

// @address 0xccbf8 actor_moving_compute_reserved_hints
// match: 0xccbf8: without it the linked body grows by 4 bytes (872 to 876)
extern "C" void actor_moving_compute_reserved_hints(long actor_index, actor_moving_reserved_hints *__restrict reserved)
{
    actor_datum *actor = datum_get<actor_datum>(actor_data, actor_index);
    memset(reserved, 0, sizeof(*reserved));
    if (actor->clump_index == -1)
        return;

    actor_moving_reserved_hint hints[25];
    short hint_count = 0;
    clump_actor_iterator iterator;
    clump_actor_iterator_new(&iterator, datum_get<clump_datum>(clump_data, actor->clump_index));
    actor_datum *member;
    while ((member = clump_actor_iterator_next(&iterator)) != 0)
    {
        if (!actor_path_executing(iterator.actor_index))
            continue;

        actor_path *path = &member->path;
        for (short step_index = 0; step_index < path->point_count; step_index++)
        {
            actor_path_point *step = &path->points[step_index];
            if (step->frame_index != 5 || !step->hint.valid())
                continue;

            short found = -1;
            for (short i = 0; i < hint_count; i++)
            {
                if (hints[found].hint_index == step->hint.hint_index && hints[found].structure_bsp_index == step->hint.structure_bsp_index)
                {
                    found = i;
                    break;
                }
            }
            if (found == -1 && hint_count < 25)
            {
                found = hint_count;
                hints[hint_count].hint_index = step->hint.hint_index;
                hints[hint_count].structure_bsp_index = (signed char)step->hint.structure_bsp_index;
                hints[hint_count].count = 0;
                hint_count++;
            }
            if (found != -1)
            {
                if (member == actor)
                {
                    hints[found].count--;
                }
                else
                {
                    unsigned short vertex_index = (unsigned short)actor_moving_get_hint(&step->hint)->raw_data[1];
                    real_point3d *point = &pathfinding_data_get(step->hint.structure_bsp_index)->vertices[vertex_index];
                    if (actor_moving_reserve_actor_distance3d(point, &actor->position.body) > actor_moving_reserve_distance3d(point,
                        &member->position.body) - 0.2f)
                        hints[found].count++;
                }
            }
        }
    }

    if (hint_count > 4)
    {
        actor_moving_qsort_4byte(hints, hint_count, actor_moving_is_reserved_hint_less, 0);
        hint_count = 4;
    }
    for (short i = 0; i < hint_count; i++)
    {
        if (hints[i].count != 0)
        {
            reserved->hints[i] = hints[i];
            reserved->count++;
        }
    }
}

static __forceinline void actor_moving_path_destination_set(
    c_path_destination *destination, const c_ai_point3d *point, const c_sector_ref *sector, float unknown)
{
    destination->point = *point;
    destination->sector = *sector;
    destination->vector = *global_zero_vector3d;
    destination->distance = unknown;
}


// @address 0x516ac0 ?actor_impulse_submit@@YA_NJJEPBTreal_vector3d@@@Z
bool actor_impulse_submit(long actor_index, long impulse_type, unsigned char impulse_flags, union real_vector3d const * impulse_position)
{
    bool result = false;
    actor_datum *player = datum_get<actor_datum>(actor_data, actor_index);
    if (player->root_vehicle_index == -1)
    {
        action_request request;
        memset(&request, 0, sizeof(request));
        memset(&request, 0, sizeof(request));
        request.type = 42;
        request.impulse.type = impulse_type;
        request.impulse.flags = impulse_flags;
        if (impulse_position)
        {
            request.impulse.has_position = 1;
            request.impulse.position.words[0] = impulse_position->words[0];
            request.impulse.position.words[1] = impulse_position->words[1];
            request.impulse.position.words[2] = impulse_position->words[2];
        }
        switch (impulse_type)
        {
        case _string_id_dive_front:
        case _string_id_dive_back:
        case _string_id_dive_left:
        case _string_id_dive_right:
        case _string_id_surprise_front:
        case _string_id_surprise_back:
            request.impulse.mode = 1;
            break;
        default:
            request.impulse.mode = 2;
            break;
        }
        result = action_submit(player->unit_index, &request);
    }
    return result;
}

// @address 0x517238 ?actor_test_movement_type@@YA_NJJ@Z
bool actor_test_movement_type(long tested_actor, long tested_movement)
{
    int unit_index;
    object_datum *unit;
    bool animation_available;
    character_charge_properties *charge_properties;
    bool result;
    result = 0;
    unit_index = datum_get<actor_datum>(actor_data, tested_actor)->unit_index;
    if (unit_index != -1)
    {
        unit = (*object_header_get(unit_index)).datum;
        animation_available = ((const c_animation_manager *)(object_header_block_get(unit, &unit->animation)))->find_goal(tested_movement, 226, 226, 12, 0);
        if (animation_available && tested_movement == 51)
        {
            charge_properties = character_definition_charge_multi_properties_get(tested_actor);
            if (charge_properties && (charge_properties->flags & 4) != 0)
                return false;
        }
        result = animation_available;
    }
    return result;
}

// @address 0x518400 actor_path_distance_remaining
extern "C" float actor_path_distance_remaining(long actor_index, short path_index)
{
    float distance = 0.0f;
    if (actor_moving_path_executing(actor_index)) {
        actor_datum *actor = datum_get<actor_datum>(actor_data, actor_index);
        actor_path *path = &actor->path;
        real_point3d previous;
        short i;
        distance = 0.0f;
        previous = actor->position.body;
        if (path_index == -1)
            path_index = path->point_count;
        for (i = path->point_index; i < path_index; i++) {
            real_point3d point;
            ::ai_point_get_position(&path->points[i].point, &point);
            distance += distance3d(&point, &previous);
            previous = point;
        }
    }
    return distance;
}

// @address 0x518520 ?actor_path_step_get_vector@@YA_NJFPATreal_vector3d@@@Z
static __forceinline bool actor_moving_step_vector_path_executing(long actor_index)
{
    actor_datum *actor = datum_get<actor_datum>(actor_data, actor_index);
    bool executing = false;
    if (actor->path.valid != 0 && actor->path_state == 1)
        executing = true;
    return executing;
}
bool actor_path_step_get_vector(long actor_index, short step_index, real_vector3d *vector)
{
    bool result = false;
    if (actor_moving_step_vector_path_executing(actor_index))
    {
        actor_datum *actor = datum_get<actor_datum>(actor_data, actor_index);
        actor_path *path = &actor->path;
        if (step_index >= 0 && step_index < path->point_count)
        {
            actor_path_point *step = &path->points[step_index];
            real_vector3d step_vector;
            if (step_index == 0)
                ai_vector_from_points3d(&actor->position.body, &step->point, &step_vector);
            else
                ai_vector_from_points3d(&path->points[step_index - 1].point, &step->point, &step_vector);
            long unit_index = ((actor_datum *)actor)->unit_index;
            if (unit_index != -1 && !mover_has_variable_camera_up(unit_index))
                step_vector.k = 0.0f;
            *vector = step_vector;
            result = true;
        }
    }
    return result;
}

struct path_state;
// @address 0xd10f0 actor_moving_compute_path_object_distance
extern "C" float actor_moving_compute_path_object_distance(path_state *table, const c_ai_point3d *start, c_sector_ref key, long object_index)
{
    short current = path_node_from_hash_table((path_state *)table, key);
    float maximum = 3.4028234663852886e38f;
    float minimum = maximum;
    real_point3d origin;
    ::ai_point_get_position(start, &origin);
    real_point3d target;
    object_get_origin(object_index, &target);
    if (current != -1)
    {
        real_point3d point;
        do
        {
            path_node *entry = &table->node_list[current];
            const c_ai_point3d *attached = current == 0 ? &table->input.start : &entry->point;
            if ((unsigned short)attached->attachment_index == 0xffff ||
                !ai_reference_frame_transform_point(attached->attachment_index, &attached->position, &point))
                point = attached->position;
            real_vector3d vector;
            vector.i = point.x - origin.x;
            vector.j = point.y - origin.y;
            vector.k = point.z - origin.z;
            float distance = point_to_line_distance_squared3d(&target, &origin, &vector);
            origin = point;
            if (!(distance > minimum))
                minimum = distance;
            current = entry->parent_node_index;
        } while (current != -1);
        if (minimum != maximum)
            return sqrtf(minimum);
        return maximum;
    }
    return maximum;
}


// match: 0x504ee8: retain the opaque firing-position call summary; naming this call changes actor_handle_change_internal.

// @address 0xc4f50 ?actor_move_to_firing_position_update_status@@YA_NJUfiring_position_ref@@_N@Z
bool actor_move_to_firing_position_update_status(long player_index, struct firing_position_ref handle, bool flag)
{
    actor_datum *player = datum_get<actor_datum>(actor_data, player_index);
    bool result = false;
    if (player->root_vehicle_index != -1 || player->attached_vehicle_index == -1 || player->dynamic_firing_set_index == -1 ||
        (handle.block & 0x8000) || handle.value == FP_REF_CURRENT.value)
    {
        long value = player->squad_index;
        if (value != -1)
        {
            squad_area_iterator iterator;
            squad_area_iterator_new(&iterator, (short)value, actor_get_area_flags(player_index), actor_get_area_type(player_index), flag,
                (e_pureform)actor_pureform_get_firing_point_selection_form(player_index));
            while (squad_area_iterator_next(&iterator) != -1)
            {
                result = area_valid_firing_positions((short)iterator.zone_index, (short)iterator.area_index, handle);
                if (result)
                    break;
            }
        }
    }
    return result;
}



// @address 0x518118 actor_moving_try_and_get_path_direction
extern "C" bool actor_moving_try_and_get_path_direction(long actor_index, float *distance_squared, real_vector3d *direction)
{
    bool tr3;
    tr3 = false;
    actor_datum *tr29;
    tr29 = datum_get<actor_datum>(actor_data, actor_index);
    actor_path *tr31;
    tr31 = &tr29->path;
    if (tr31->valid != 0) {
        unsigned short tr28;
        tr28 = (unsigned short)tr29->pathfinding_point.location[0];
        short tr30;
        tr30 = 0;
        int tr9;
        tr9 = tr31->point_count;
        float tf31;
        tf31 = k_real_constant_pool_zero[0];
        float tf30;
        tf30 = tf31;
        // Holds the candidate path point, then the direction derived from it.
        union
        {
            real_point3d l50;
            real_vector3d l50_vector;
        };
        if (tr9 > 0) {
            int tr11;
            tr11 = 0;
            actor_path_point *tr10;
            for (;;) {
                tr10 = &tr31->points[tr11];
                if (tr10->frame_index != 1) {
                    tr30 = (short)(tr11 + 1);
                    tr11 = tr30;
                    if (tr11 < tr9) {
                        continue;
                    }
                } else {
                    ::ai_point_get_position(&tr10->point, &l50);
                    real_point3d l60;
                    ::ai_reference_frame_inverse_transform_point(tr28, &l50, &l60);
                    tr11 = tr30;
                    if (tr11 == 0) {
                        memcpy(&l50.x, &tr29->pathfinding_point.point.x, sizeof(l50.x));
                        memcpy(&l50.y, &tr29->pathfinding_point.point.y, sizeof(l50.y));
                        memcpy(&l50.z, &tr29->pathfinding_point.point.z, sizeof(l50.z));
                    } else {
                        real_point3d l70;
                        ::ai_point_get_position(&tr31->points[tr11 - 1].point, &l70);
                        ::ai_reference_frame_inverse_transform_point(tr28, &l70, &l50);
                    }
                    float tf0;
                    tf0 = l50.x - tr29->pathfinding_point.point.x;
                    float tf13;
                    tf13 = l50.z - tr29->pathfinding_point.point.z;
                    float tf12;
                    tf12 = l50.y - tr29->pathfinding_point.point.y;
                    l50.x = l60.x - l50.x;
                    l50.z = tf31;
                    l50.y = l60.y - l50.y;
                    tf30 = tf12 * tf12 + (tf13 * tf13 + tf0 * tf0);
                    tr3 = normalize3d(&l50_vector) > tf31;
                }
                break;
            }
        }
        if (tr3) {
            if (distance_squared) {
                *distance_squared = tf30;
            }
            if (direction) {
                memcpy(direction, &l50, sizeof(*direction));
            }
        }
    }
    return tr3;
}

// @address 0xd0fb8 actor_moving_update_nearest_object_distance
extern "C" void actor_moving_update_nearest_object_distance(long actor_index, path_state *table)
{
    actor_datum *actor = datum_get<actor_datum>(actor_data, actor_index);
    actor->nearest_object_distance = 0.0f;
    if (table->has_goal != 0 && actor->unit_index != -1)
    {
        float initial_nearest = FLT_MAX;
        float nearest = initial_nearest;
        c_sector_ref key = table->goal.sector;
        c_ai_point3d *start = &table->goal.point;
        c_data_iterator<player_datum> it;
        it.m_iterator.absolute_index = -1;
        it.m_iterator.index = -1;
        it.m_iterator.data = player_data;
        it.next();
        if (it.m_datum != 0)
        {
            do
            {
                long object_index = it.m_datum->unit_index;
                if (object_index != -1)
                {
                    real_point3d origin;
                    object_get_origin(object_index, &origin);
                    float dz = 0.0f, dx = 0.0f, dy = 0.0f;
                    dx = origin.x - actor->position.body.x;
                    dz = origin.z - actor->position.body.z;
                    dy = origin.y - actor->position.body.y;
                    float distance = sqrtf(dy * dy + (dz * dz + dx * dx));
                    if (actor_moving_compute_path_object_distance(table, start, key, object_index) > distance - 1.0f && nearest > distance)
                        nearest = distance;
                }
                it.next();
            } while (it.m_datum != 0);
            if (nearest != initial_nearest)
                actor->nearest_object_distance = nearest;
        }
    }
}

// @address 0x530cf0 actor_moving_vehicle_test_destination
extern "C" bool actor_moving_vehicle_test_destination(long i,long fp,firing_position *f,c_cover_state_data *o,void *path,bool use_path,long owner)
{
 actor_datum *a=datum_get<actor_datum>(actor_data, i);
 bool success;
 o->flags &= ~16;
 if (!use_path) path=0;
 firing_position_ref reference;
 reference.value = fp;
 firing_position_ref result = actor_change_firing_position(i, reference, f, owner, (path_state *)path, true);
 o->flags &= ~2;
 if (*(long *)&result==(*(int *)&FP_REF_NONE)) { datum_get<actor_datum>(actor_data, i)->uncovering=false; success=false; } else {
 prop_state *p=prop_state_get(datum_get<prop_ref_datum>(prop_ref_data, a->target_prop_index));
 a->flags &= ~0x10000000;
 success=true;
 if (f && (f->type==3 ||  f->transparent_line_of_sight)) {
   vector_from_points3d(&f->position,&p->position,&o->direction);
   if (normalize3d(&o->direction)>k_real_constant_pool_zero[0]) o->flags |= 16;
 } else if (a->path.point_count>0) {
   actor_find_pathfinding_location(i);
   c_ai_point3d *next=&a->path.points[a->path.point_count-1].point;
   c_ai_point3d *prev=a->path.point_count>1? &a->path.points[a->path.point_count-2].point : &a->pathfinding_point;
   if (next && prev) {
     ai_vector_from_points3d(next,prev,&o->direction);
     float zero=k_real_constant_pool_zero[0];
     o->direction.k=zero;
     if (normalize3d(&o->direction)>zero) {
       real_vector3d v;
       ai_vector_from_points3d(&p->position,next,&v);
       v.k=zero;
       if (normalize3d(&v)>zero && dot_product3d(&o->direction,&v)<g_actor_moving_destination_direction_threshold) o->flags |=16;
     }
   }
 }
 }
 return success;
}



// @address 0x5157b0 ?path_step_update@@YAXJ@Z
void path_step_update(long index)
{
    actor_datum *actor = datum_get<actor_datum>(actor_data, index);
    actor_path *path = &actor->path;
    actor->flags |= 1;
    real_point3d center;
    if (actor->use_object) {
        if (actor->root_vehicle_index != -1)
            ai_get_center_of_mass(actor->root_vehicle_index, &center);
        else
            ai_get_center_of_mass(actor->unit_index, &center);
    } else
        center = actor->position.body;
    ai_vector_from_points3d(&center, &path->points[path->point_index].point, &actor->throttle);
}

// @address 0x518650 actor_moving_is_reserved_hint_less
extern "C" bool actor_moving_is_reserved_hint_less(long a, long b, const void *context)
{
    signed char *pa = (signed char *)&a;
    signed char *pb = (signed char *)&b;
    signed char ab = pa[3];
    signed char bb = pb[3];
    bool result = false;
    if (ab > bb)
        return result;
    else if (ab < bb)
        result = true;
    return result;
}

static __forceinline float ai_hint_get_jump_velocity(short index, float velocity)
{
    s_game_globals *globals = global_game_globals;
    if (index >= 0 && index < 6 && globals && globals->ai_globals.count() > 0)
        velocity = ((s_ai_globals_definition *)globals->ai_globals.address)->jump_velocities[index];
    return velocity;
}
// @address 0x5172f8 ?actor_get_max_jump_speed@@YAMJPA_N@Z
float actor_get_max_jump_speed(long actor_index, bool *out_limited)
{
    actor_datum *actor = datum_get<actor_datum>(actor_data, actor_index);
    object_datum *unit = object_get(actor->unit_index);
    character_movement_properties *movement = character_definition_movement_properties_get(actor->character_index);
#if defined(_XBOX) && !defined(__clang__)
    // match: 0x5172f8: the float loads through the constant-pool base register the function already holds
    float max_speed = k_real_constant_pool_zero[4058];
#else
    float max_speed = 1.5f;
#endif
    if (out_limited)
        *out_limited = false;
    if (movement && movement->jump_height > 0) {
        max_speed = ai_hint_get_jump_velocity((movement->jump_height < 6 ? movement->jump_height : 6) - 1, k_real_constant_pool_zero[0]);
        if (out_limited)
            *out_limited = movement->jump_height >= 7;
    } else if (unit->identifier.get_type() == 0) {
        biped_definition *definition = TAG_GET('bipd', biped_definition, unit->definition_index);
        max_speed = definition->jump_velocity;
    }
    return max_speed;
}

// @address 0x517438 ?actor_test_jump@@YA_NJPATreal_point3d@@0@Z
bool actor_test_jump(long actor_index, union real_point3d *point, union real_point3d *point2)
{
    bool limited;
    float jump_speed = actor_get_max_jump_speed(actor_index, &limited);
    if (limited)
        return true;
    real_vector3d aim;
    return projectile_aim_ballistic(jump_speed, actor_get_gravity_scale(actor_index), point, point2, 0, 0, 0, false, &aim, 0, 0, 0, 0, 0);
}




static __forceinline bool sector_references_differ(const c_sector_ref *a, const c_sector_ref *b)
{
    return (unsigned short)a->structure_bsp_index != (unsigned short)b->structure_bsp_index
        || a->sector_index != b->sector_index;
}

// @address 0x11ed38 ?equals@c_destination_orders@@QAA_NJPAV1@@Z
bool c_destination_orders::equals(long actor_index, c_destination_orders *other)
{
    bool result = true;
    if (type == other->type
        && flags.m_storage == other->flags.m_storage
        && target_index == other->target_index
        && f1c == other->f1c
        && f20 == other->f20
        && f24 == other->f24
        && f28 == other->f28
        && fabsf(m_destination_facing.i - other->m_destination_facing.i) < 0.0001f
        && fabsf(m_destination_facing.j - other->m_destination_facing.j) < 0.0001f
        && fabsf(m_destination_facing.k - other->m_destination_facing.k) < 0.0001f)
    {
        bool has_action = (bool)(_rotl(flags.m_storage, 29) & 1);
        if ((has_action) != 0)
        {
            if (action.type != other->action.type || action.posture_name != other->action.posture_name)
                result = false;
        }
    }
    else
    {
        result = false;
    }
    if (result)
    switch (type)
    {
    case 0:
        result = false;
        break;
    case 1:
        if (sector_references_differ(&point_destination.sector, &other->point_destination.sector))
        {
            result = false;
            break;
        }
    case 2:
    {
        float dist = ::ai_distance_squared3d(&point_destination.point, &other->point_destination.point);
        if (dist > 0.010000001f)
        {
            result = false;
            break;
        }
        break;
    }
    case 3:
        result = firing_position_index == other->firing_position_index;
        break;
    case 4:
        if (prop_ref_index != other->prop_ref_index)
        {
            result = false;
            break;
        }
        if (prop_ref_index == -1)
            break;
        {
            actor_datum *actor = (actor_datum *)(actor_data->data + (actor_index & 0xffff) * sizeof(actor_datum));
            prop_state *prop = (prop_state *)prop_state_get_by_index(prop_ref_index);
            prop_find_pathfinding_location(actor_index, prop_ref_index);
            const c_sector_ref *actor_sector = &actor->path.destination_sector;
            const c_sector_ref *prop_sector = &prop->pathfinding_sector;
            float distance;
            if (actor->use_object != 0)
            {
                distance = sqrtf(::ai_distance_squared3d(&actor->path.destination, &prop->center));
            }
            else
            {
                if (actor->path.destination.location[0] != prop->pathfinding_point.location[0])
                {
                    result = false;
                    break;
                }
                if (*actor_sector != *prop_sector)
                {
                    result = false;
                    break;
                }
                distance = sqrtf(::ai_distance_squared3d(&actor->path.destination, &prop->pathfinding_point));
            }
            if (distance > f24)
                result = false;
            break;
        }
    default:
        // match: 0x11ed38: drops the cmplwi/bgt range check on the jump table index
        __assume(0);
        break;
    }
    return result;
}

// Callees from other accepted units (see KIT.md)


// @address 0x516228 actor_moving_can_move_along_vector
extern "C" bool actor_moving_can_move_along_vector(
    long actor_index, const real_vector3d *vector, float distance, float minimum_distance, bool *blocked, float *fraction)
{
    actor_datum *moving_actor;
    c_sector_ref *actor_sector_ref;
    c_sector_ref current_sector;
    c_sector_ref none_sector;
    unsigned char has_valid_sector;
    bool can_move;
    real_vector3d transformed_vector;
    path_collision_result pathfinding_result;
    moving_actor = datum_get<actor_datum>(actor_data, actor_index);
    can_move = false;
    if (fraction != NULL) {
#if defined(_XBOX) && !defined(__clang__)
        // match: 0x516228: the float loads through the constant-pool base register the function already holds
        *fraction = k_real_constant_pool_zero[-200];
#else
        *fraction = 1.0f;
#endif
    }
    if (!moving_actor->use_object)
    {
        actor_find_pathfinding_location(actor_index);
        actor_sector_ref = &moving_actor->pathfinding_sector;
        if (actor_sector_ref->valid())
        {
            ::ai_reference_frame_inverse_transform_vector(
                moving_actor->pathfinding_point.location[0], vector, &transformed_vector);
            current_sector = *actor_sector_ref;
            none_sector = g_sector_none;
            if ((((unsigned char)pathfinding_test_vector(
                     &moving_actor->pathfinding_point.point, current_sector, none_sector, &transformed_vector, distance, 0, &pathfinding_result)) == 0))
            {
                has_valid_sector = ((unsigned short)pathfinding_result.sector.structure_bsp_index != 0xffff
                    || pathfinding_result.sector.sector_index != 0xffff);

                if (!((((unsigned char)has_valid_sector) == 0))) {
                    can_move = true;
                }
                else if (fraction != NULL)
                    *fraction = (k_real_constant_pool_zero[0]);
            } else {
                if (fraction != NULL)
                    *fraction = pathfinding_result.t;
            }
        }
    }
    if (blocked != NULL) {
        *blocked = false;
    }
    return can_move;
}

static const float actor_moving_path_input_cautious_cost = 5.0f;

static __forceinline void actor_moving_path_input_initialize(
    path_input *input, float radius, long unit_index, unsigned char pathfinding_type, float danger_cost)
{
    memset(input, 0, sizeof(*input));
    input->radius = radius;
    input->unit_index = unit_index;
    input->hint_cost = danger_cost;
    input->pathfinding_type = pathfinding_type;
    input->destination_object_index = -1;
}
static __forceinline void actor_moving_path_input_set_start(path_input *input, const c_ai_point3d *point, c_sector_ref sector)
{
    input->start_set = true;
    input->start = *point;
    input->start_sector_ref = sector;
}
static __forceinline void actor_moving_path_input_set_start(path_input *input, const c_ai_point3d *point, const c_sector_ref *sector)
{
    input->start_set = true;
    input->start = *point;
    input->start_sector_ref = *sector;
}
static __forceinline void actor_moving_path_input_set_air_point(c_ai_point3d *point, const real_point3d *position)
{
    point->location[0] = -1;
    point->location[1] = -1;
    point->point = *position;
}

// @address 0xccf60 ?to_raw@c_destination_orders@@QAA_NJPAVc_path_destination@@PAW4e_pathfinding_mode@@@Z
bool c_destination_orders::to_raw(long actor_index, c_path_destination *destination, e_pathfinding_mode *mode)
{
    // match: 0xccf60: datum_get<actor_datum> here swaps the operands of the datum address add from r11,r9 to r9,r11.
    actor_datum *actor = &((actor_datum *)actor_data->data)[DATUM_INDEX_TO_ABSOLUTE_INDEX(actor_index)];
    bool result = true;
    long path_mode = actor->use_object != 0;
    if (mover_get_motor_program(actor->unit_index) == 5)
    {
        long character_index = actor->character_index;
        character_general_properties *general = character_definition_general_properties_get(character_index);
        character_movement_properties *movement = character_definition_movement_properties_get(character_index);
        if (general != 0 && (general->flags & 2) == 0 && movement != 0 && (movement->flags & 0x200) != 0)
            path_mode = 1;
    }
    switch (type)
    {
    case 1:
        actor_moving_path_destination_set(destination, &point_destination.point, &point_destination.sector, f28);
        break;
    case 2:
        path_mode = 1;
        actor_moving_path_destination_set(destination, &point_destination.point, &point_destination.sector, 0.0f);
        break;
    case 3:
    {
        firing_position_definition *firing =
            (firing_position_definition *)firing_position_ref_get_definition(firing_position);
        if (firing != 0)
        {
            character_movement_properties *move_props = character_definition_movement_properties_get(actor->character_index);
            c_sector_ref sector = firing->sector;
            if (move_props != 0)
            {
                if (flags.test(_destination_flag_6))
                {
                    sector.clear();
                    if ((move_props->flags & 0x200) != 0)
                        path_mode = 1;
                }
                else if (flags.test(_destination_flag_5))
                {
                    if ((move_props->flags & 0x40) != 0 || (move_props->flags & 0x80) != 0)
                    {
                        result = true;
                        if ((move_props->flags & 0x200) != 0)
                            path_mode = 1;
                    }
                    else
                        result = false;
                }
            }
            if (result)
                actor_moving_path_destination_set(destination, &firing->point, &sector, 0.0f);
        }
        else
            result = false;
        break;
    }
    case 4:
    {
        prop_ref_datum *ref = datum_get<prop_ref_datum>(prop_ref_data, prop_ref_index);
        prop_state *prop = (prop_state *)prop_state_get(ref);
        prop_find_pathfinding_location(actor_index, prop_ref_index);
        c_ai_point3d local_point;
        if (path_mode == 1)
        {
            local_point.point = prop->center;
            local_point.location[0] = -1;
            local_point.location[1] = -1;
        }
        else
            local_point = prop->pathfinding_point;
        actor_moving_path_destination_set(destination, &local_point, &prop->pathfinding_sector, f28);
        break;
    }
    default:
        // match: 0xccf60: drops the default edge; without it the r10/r11 assignment of the datum loads (lwz 44h) after the switch swaps
        __assume(0);
        break;
    }
    if (action.type != 0)
    {
        c_ai_point3d entry_point;
        real_vector3d entry_vector;
        c_sector_ref entry_sector;
        if (action.get_entry_point(actor_index, &destination->point, &destination->sector, &entry_point, &entry_vector, &entry_sector))
        {
            destination->sector = entry_sector;
            destination->vector = entry_vector;
            destination->point = entry_point;
        }
    }
    if (result && path_mode == 0 && destination->distance == 0.0f && !destination->sector.valid())
        result = false;
    if (mode != 0)
        *mode = (e_pathfinding_mode)path_mode;
    return result;
}

// @address 0xcd3d8 ?actor_path_input_new@@YAXJPAUpath_input@@@Z
// match: 0xcd3d8: keeps input in r30 and the actor array base in r29; without it both move up a register (r31/r28)
void actor_path_input_new(long actor_index, path_input *__restrict input)
{
    actor_datum *actor = datum_get<actor_datum>(actor_data, actor_index);
    character_movement_properties *movement = character_definition_movement_properties_get(actor->character_index);
    long unit_index = actor->unit_index;
    float radius = movement->pathfinding_radius;
    if (actor->driver || actor_is_giant(actor))
    {
        unit_index = actor_is_giant(actor) ? actor->unit_index : actor->root_vehicle_index;
        const object_datum *object = (const object_datum *)((*object_header_get(unit_index)).object);
        const character_vehicle_properties *vehicle = character_vehicle_properties_get(actor_index, object->definition_index);
        if (vehicle && vehicle->pathfinding_radius > 0.0f)
            radius = vehicle->pathfinding_radius;
    }
    float danger_cost;
    if (movement->jump_height < 7)
    {
        if (actor->combat_status >= 3)
            danger_cost = 1.0f;
        else
            danger_cost = actor_moving_path_input_cautious_cost;
    }
    else
        danger_cost = 0.0f;
    actor_moving_path_input_initialize(input, radius, unit_index, actor->pathfinding_type, danger_cost);
    if (actor->use_object)
    {
        c_ai_point3d point;
        actor_moving_path_input_set_air_point(&point, &actor->position.body);
        actor_moving_path_input_set_start(input, &point, g_sector_none);
    }
    else
    {
        actor_find_pathfinding_location(actor_index);
        actor_moving_path_input_set_start(input, &actor->pathfinding_point, &actor->pathfinding_sector);
    }
    input->allow_narrow_links = actor_is_giant(actor);
    input->allow_vehicle_hints = actor->root_vehicle_index != -1;
}



// @address 0x515870 ?actor_test_destination_internal@@YA_NJM@Z
bool actor_test_destination_internal(long actor_index, float tolerance)
{
    actor_datum *actor = datum_get<actor_datum>(actor_data, actor_index);
    float distance_squared = ai_distance_squared3d(&actor->path.destination, &actor->position.body);
    bool result = false;
    if (actor->root_vehicle_index == -1 && actor->destination_orders.flags.test(_destination_flag_2))
    {
        if (actor->path.point_index == actor->path.point_count - 1)
        {
            real_vector3d path_vector;
            real_vector3d destination_vector;
            if (actor->path.point_index - 1 >= 0)
                ai_vector_from_points3d(&actor->path.points[actor->path.point_index - 1].point, &actor->path.destination, &path_vector);
            else
                ai_vector_from_points3d(&actor->path.start_point, &actor->path.destination, &path_vector);
            ai_vector_from_points3d(&actor->position.body, &actor->path.destination, &destination_vector);
            result = actor_move_dot3d(&path_vector, &destination_vector) < 0.0f;
        }
    }
    else if (distance_squared < tolerance * tolerance)
    {
        result = true;
    }
    else
    {
        if (actor->destination_orders.flags.test(_destination_flag_2) && distance_squared < (tolerance + 0.5f) * (tolerance + 0.5f))
        {
            real_vector3d velocity;
            real_vector3d destination_vector;
            ai_vector_from_points3d(&actor->position.body, &actor->path.destination, &destination_vector);
            object_get_velocities(actor->unit_index, &velocity, 0);
            if (actor_move_dot3d(&velocity, &destination_vector) < 0.0f)
                result = true;
        }
    }
    return result;
}


// @address 0xb8a00 actor_moving_update_vehicle_throttle
extern "C" void actor_moving_update_vehicle_throttle(long actor_index)
{
    actor_datum *actor = datum_get<actor_datum>(actor_data, actor_index);
    bool driving = false;
    if (actor->vehicle_driver_type == 3 || actor->vehicle_driver_type == 5)
        driving = true;
    float distance_squared;
    real_vector3d direction;
    if (driving && actor_moving_try_and_get_path_direction(actor_index, &distance_squared, &direction))
    {
        real_vector3d facing = actor->facing_vector;
        object_datum *vehicle = (object_datum *)(datum_get<s_object_header>(object_header_data, actor->root_vehicle_index)->object);
        character_vehicle_properties *properties = character_vehicle_properties_get(actor_index, vehicle->definition_index);
        facing.k = 0.0f;
        normalize3d(&facing);
        float dot = dot_product3d(&facing, &direction);
        if (distance_squared < properties->deceleration_distance * properties->deceleration_distance && dot > 0.70710677f)
        {
            if (dot > 0.98f)
                actor_unit_control_boost(actor_index, true);
            return;
        }
    }
    if (actor->destination_orders.f1c != 0.0f)
    {
        actor->throttle_override_valid = true;
        actor->throttle_override = actor->destination_orders.f1c;
        return;
    }
    if (actor->squad_index != -1 && !actor->friendly_player_in_vehicle && driving && actor->throttle_override_valid)
    {
        s_task *task = ::squad_get_task((&((squad_datum *)squad_data->data)[DATUM_INDEX_TO_ABSOLUTE_INDEX(actor->squad_index)]));
        if (task)
        {
            switch (task->signed_type)
            {
            case 3:
                if (!game_team_is_enemy((e_game_team)actor->team, _game_team_player))
                {
                    float distance = task->distance > 0.0f ? task->distance : 10.0f;
                    if (actor->nearest_object_distance > distance)
                    {
                        actor->destroying_obstacle = true;
                        return;
                    }
                    float slow_distance = distance * 0.5f;
                    if (actor->nearest_object_distance > slow_distance)
                        actor->throttle_override = (distance - actor->nearest_object_distance) / slow_distance * 0.4f + 0.6f;
                }
                break;
            case 0:
                if (actor->goal_status != 2)
                    actor->throttle_override = 0.6f;
                break;
            }
        }
    }
}

// @rdata 0x8202251c
static const float actor_moving_rays_zero = 0.0f;
// @rdata 0x8202490c
static const float actor_moving_rays_epsilon = 0.0001f;
static __forceinline float actor_moving_rays_get_zero() { return actor_moving_rays_zero; }

static __forceinline float actor_moving_rays_dot_x(const real_vector2d *a, const real_vector2d *b) { float x = a->i * b->i; return x + a->j * b->j; }

static __forceinline float actor_moving_rays_magnitude_xy(float x, float y) { return sqrtf(x * x + y * y); }


// @address 0x5189b8 ?rays_nearest_distance@@YAMPATreal_point2d@@PATreal_vector2d@@01PA_NPAM3@Z
float rays_nearest_distance(real_point2d *point, real_vector2d *vector, real_point2d *point2, real_vector2d *vector2,
    // match: 0x5189b8: keeps the fr31 save and the order of the opening lfs loads; without it the entry loads reorder and fr31 goes unused
    bool *__restrict forward, float *__restrict distance, float *__restrict distance2)
{
    float result = 3.402823466e+38f;
    real_vector2d perpendicular = {-vector->j, vector->i};
    real_vector2d delta;
    vector_from_points2d(point, point2, &delta);
    float denominator = actor_moving_rays_dot_x(&perpendicular, vector2);
    *forward = true;
    *distance = actor_moving_rays_get_zero();
    *distance2 = actor_moving_rays_get_zero();
    if (fabs(denominator) > actor_moving_rays_epsilon)
    {
        float t2 = dot_product2d(&delta, &perpendicular) * (-1.0f / denominator);
        t2 = t2 > actor_moving_rays_zero ? t2 : actor_moving_rays_get_zero();
        real_point2d nearest2;
        point_from_line2d(point2, vector2, t2, &nearest2);
        *distance2 = t2;
        real_vector2d relative;
        vector_from_points2d(point, &nearest2, &relative);
        float t = dot_product2d(vector, &relative);
        *forward = t >= actor_moving_rays_get_zero();
        t = t > actor_moving_rays_zero ? t : actor_moving_rays_get_zero();
        real_point2d nearest;
        point_from_line2d(point, vector, t, &nearest);
        *distance = t;
        result = distance2d(&nearest, &nearest2);
    }
    else
    {
        bool same_direction = dot_product2d(vector, vector2) > actor_moving_rays_get_zero();
        bool in_front = dot_product2d(vector, &delta) > actor_moving_rays_get_zero();
        if (same_direction || in_front)
        {
            *forward = in_front;
            result = (float)fabs(dot_product2d(&perpendicular, &delta));
        }
        else
        {
            *forward = false;
            result = actor_moving_rays_magnitude_xy(delta.i, delta.j);
        }
    }
    return result;
}


// @address 0xc7aa0 ?actor_special_movement_new@@YA_NJPAUspecial_movement@@@Z
bool actor_special_movement_new(long actor_index, special_movement *special)
{
    actor_datum *actor = datum_get<actor_datum>(actor_data, actor_index);
    character_definition *character = TAG_GET('char', character_definition, actor->character_index);
    long style_tag = character->style.index;
    if (style_tag != -1)
    {
        style_definition *style = TAG_GET('styl', style_definition, style_tag);
        if (style == 0)
            special->hint_types = 0x7ff;
        else if (style->special_movement.count() > 0)
            special->hint_types = style->special_movement[0] << 1;
        else
            special->hint_types = 0;
    }
    else
        special->hint_types = 0x7ff;
    long script_index = actor->command_index;
    if (script_index != -1)
    {
        command_script_datum *script = datum_get<command_script_datum>(tls_command_script_data(), script_index);
        if (script->style_index_valid)
        {
            style_definition *command_style = TAG_GET('styl', style_definition, script->style_index);
            if (command_style->special_movement.count() > 0)
                special->hint_types = command_style->special_movement[0] << 1 & special->hint_types;
        }
    }
    special->hint_types |= 0x301;
    character_movement_properties *movement = (character_movement_properties *)character_definition_movement_properties_get(actor->character_index);
    special->infinite_jump = false;
    special->jump_heights = 0;
    special->movement_hints = 0;
    special->obstacle_ignore_size = 0;
    special->obstacle_smashable_size = 0;
    special->obstacle_leap_minimum_size = 0;
    special->obstacle_leap_maximum_size = 0;
    if (!actor->driver || actor->unit_index == actor->root_vehicle_index)
    {
        if (movement != 0)
        {
            if ((movement->flags & 0x20) != 0)
                special->infinite_jump = true;
            else
            {
                long height = movement->jump_height;
                if (height > 0)
                {
                    if (height > 6)
                        height = 6;
                    special->jump_heights = (1 << height) - 1;
                    if (movement->jump_height >= 7)
                        special->infinite_jump = true;
                }
            }
            special->movement_hints = movement->hoist_flags;
            special->obstacle_ignore_size = movement->obstacle_ignore_size;
            special->obstacle_smashable_size = movement->obstacle_smashable_size;
            special->obstacle_leap_minimum_size = movement->obstacle_leap_minimum_size;
            special->obstacle_leap_maximum_size = movement->obstacle_leap_maximum_size;
        }
    }
    else
    {
        const object_datum *object = (const object_datum *)((*object_header_get(actor->root_vehicle_index)).object);
        character_vehicle_properties *properties = character_vehicle_properties_get(actor_index, object->definition_index);
        if (properties != 0)
            special->obstacle_ignore_size = properties->obstacle_ignore_size;
        special->jump_heights = 1;
    }
    special->recent_obstacles = actor->recent_obstacles;
    return true;
}

static __forceinline void actor_move_halt_orders_construct(c_destination_orders *orders)
{
    memset(&orders->action.direction, 0, sizeof(orders->action.direction));
    memset(&orders->action, 0, sizeof(orders->action));
    memset(orders, 0, sizeof(*orders));
}


// @address 0x515f70 ?actor_move_halt@@YA_NJ@Z
bool actor_move_halt(long actor_index)
{
    actor_datum *actor = datum_get<actor_datum>(actor_data, actor_index);
    if (actor->vehicle_driver_type == 4 && (actor->flags & 1))
    {
        c_destination_orders orders;
        actor_move_halt_orders_construct(&orders);
        c_ai_point3d point;
        point.point = actor->position.body;
        point.location[0] = -1;
        point.location[1] = -1;
        orders.to_air_point(&point);
        orders.flags.set(_destination_flag_2, true);
        return actor_move(actor_index, &orders, 0);
    }
    actor_set_current_firing_position(actor_index, FP_REF_NONE.value);
    actor = datum_get<actor_datum>(actor_data, actor_index);
    actor->path.valid = false;
    actor->path.obstacle_object_index = -1;
    actor->path.result_value = 0;
    actor->path_result_time = 0;
    actor->path_result_count = 0;
    actor->path_state = 0;
    actor->destination_orders.clear();
    return true;
}


// @address 0x5174e0 ?actor_moving_jump@@YA_NJPATreal_point3d@@_N@Z
bool actor_moving_jump(long actor_index, real_point3d *position, bool force)
{
    c_ai_point3d point;
    point.point = *position;
    point.location[0] = -1;
    point.location[1] = -1;
    return actor_moving_jump(actor_index, 0, false, 0, &point, g_sector_none, force);
}


// @address 0x5182d8 ?path_finish@@YAXJ_N@Z
void path_finish(long actor_index, bool execute_action)
{
    actor_datum *actor = datum_get<actor_datum>(actor_data, actor_index);
    bool finished = false;
    c_destination_orders *orders = &actor->destination_orders;
    actor->path.valid = true;
    actor->path_state = 2;
    if (!actor->search_active &&
        (orders->type == 3 || ::actor_move_to_firing_position_update_status(actor_index, orders->firing_position, false)))
    {
        float distance = sqrtf(ai_distance_squared3d(
            &((firing_position_definition *)firing_position_ref_get_definition(
                orders->firing_position))->point, &actor->position.body));
        if (distance <= ::actor_destination_tolerance(actor_index) + 0.2)
            finished = true;
    }
    if (execute_action && orders->action.type != 0 && orders->action.execute(actor_index) == _ai_action_result_1)
        finished = true;
    if (finished && orders->type == 3)
    {
        const firing_position_ref *reference = &orders->firing_position;
        if (!(reference->block & 0x8000) || reference->value == FP_REF_CURRENT.value)
            actor->search_active = true;
    }
}

// @address 0x515a18 ?actor_test_destination@@YA_NJ@Z
bool actor_test_destination(long actor_index)
{
    actor_datum *actor = datum_get<actor_datum>(actor_data, actor_index);
    bool result = false;
    bool moving = false;
    if (actor->path.valid && actor->path_state == 1)
        moving = true;
    if (moving)
    {
        if (actor->path.point_index >= actor->path.point_count)
            result = true;
        else
            result = actor_test_destination_internal(actor_index, ::actor_destination_tolerance(actor_index));
    }
    if (result)
        path_finish(actor_index, true);
    return result;
}

static __forceinline bool actor_advance_path_executing(long actor_index)
{
    actor_datum *actor = datum_get<actor_datum>(actor_data, actor_index);
    bool executing = false;
    if (actor->path.valid && actor->path_state == 1)
        executing = true;
    return executing;
}


// @address 0xb7c88 ?actor_destination_update@@YAXJ@Z
void actor_destination_update(long actor_index)
{
    actor_datum *actor = datum_get<actor_datum>(actor_data, actor_index);
    if (actor->uncovering && !actor->path_refresh_disabled) {
        if (actor_advance_path_executing(actor_index) || actor->path_state == 3)
            actor_path_refresh(actor_index, false, NULL);
    }
    actor_test_destination(actor_index);
    if (actor_advance_path_executing(actor_index)) {
        path_step_update(actor_index);
        if (actor->goal_area_valid
            && ai_distance_squared3d(&actor->path.destination, &actor->position.body)
                < (::actor_destination_tolerance(actor_index) * 3.0f) * (::actor_destination_tolerance(actor_index) * 3.0f))
            actor->goal_status = 3;
    } else {
        actor->path.valid = false;
        actor->flags &= ~1;
    }
}

// @address 0xb7dd0 ?actor_advance_path@@YA_NJ@Z
bool actor_advance_path(long actor_index)
{
    actor_datum *actor = datum_get<actor_datum>(actor_data, actor_index);
    bool result = false;
    if (actor_advance_path_executing(actor_index))
    {
        actor_advance_path_state *state = (actor_advance_path_state *)&actor->destination_orders;
        actor_path *path = &state->path;
        path_step_finish(actor_index, path->point_index);
        if (path->point_index < path->point_count - 1)
        {
            path->point_index++;
            path_step_update(actor_index);
            result = true;
        }
        else
        {
            if (actor->path.final_point)
                path_finish(actor_index, true);
            else
                state->state = 4;
            actor->flags &= ~1;
        }
    }
    return result;
}

static __forceinline bool actor_moving_action_setup_land(c_ai_action *action, const c_ai_point3d *point)
{
    bool result = false;
    if (point)
    {
        action->type = 4;
        action->point_valid = true;
        action->point = *point;
        actor_moving_direction_clear(&action->direction);
        result = true;
    }
    return result;
}

static __forceinline void actor_moving_action_setup_posture(c_ai_action *action, const c_ai_point3d *point, const c_ai_direction *direction, long posture)
{
    action->type = 1;
    action->point_valid = true;
    action->point = *point;
    action->direction = *direction;
    action->posture_name = posture;
}

// @address 0x515560 ?land@c_destination_orders@@QAA_NJ@Z
bool c_destination_orders::land(long actor_index)
{
    bool result = false;
    if (type != 0)
    {
        action.type = 0;
        c_path_destination destination;
        if (to_raw(actor_index, &destination, 0))
        {
            actor_moving_action_setup_land(&action, &destination.point);
            flags.set(_destination_flag_3, true);
            result = true;
        }
    }
    return result;
}

static __forceinline bool actor_moving_action_valid(const c_ai_action *action)
{
    return action->type != 0;
}

static __forceinline void action_interrupt_inline(long object_index, e_action action)
{
    c_motor_task *task = motor_task_find_by_action(object_index, action);
    if (task)
        motor_system_interrupted(object_index, task, true);
}
// @address 0x515bd0 ?actor_movement_update_posing@@YAXJ@Z
void actor_movement_update_posing(long actor_index)
{
    actor_datum *actor = datum_get<actor_datum>(actor_data, actor_index);
    bool posing = false;
    bool in_pose = false;
    character_idle_properties *idle = character_definition_idle_multi_properties_get(actor_index);
    if (!actor->swarm && idle) {
        long flags = actor->flags;
        if (flags & 0x80) {
            int mode, state;
            if (::motor_animation_get_mode_and_state(actor->unit_index, (long *)(&mode), (long *)(&state)))
                in_pose = state == 99;
        }
        if (actor->command_index != -1)
            posing = false;
        else if (actor->combat_status >= 6 ||
            (actor->output_animation_state == 138 && unit_animation_weapon_raised(actor->unit_index) && !motor_animation_busy(actor->unit_index)))
            posing = false;
        else if (actor->output_animation_state == 136)
            posing = false;
        else if (!in_pose && motor_animation_busy(actor->unit_index))
            posing = false;
        else if (actor_moving_action_valid(&actor->action))
            posing = false;
        else if (!in_pose && actor->attention_ticks > 0)
            posing = false;
        else if (actor->moving_crouch || actor->crouch)
            posing = false;
        else {
            bool moving = (flags & 1) || (flags & 0x8000000);
            bool can_pose;
            if (actor->combat_status <= 1)
                can_pose = in_pose || !moving;
            else
                can_pose = !moving;
            posing = actor->pose_allowed & can_pose;
        }
        if (in_pose && !posing) {
            action_interrupt_inline(actor->unit_index, _action_ai_impulse);
        } else if (!(flags & 0x80) && posing) {
            actor->aim_object_index = game_seconds_to_ticks_round(
                _real_random_range(TLS_RANDOM_SEED, NULL, NULL, 0, idle->minimum_delay, idle->maximum_delay));
        }
        if (posing && !in_pose) {
            if (--actor->aim_object_index <= 0) {
                float seconds = _real_random_range(TLS_RANDOM_SEED, NULL, NULL, 0, idle->minimum_delay, idle->maximum_delay);
                ::actor_impulse_submit(actor_index, 99, 1, NULL);
                actor->aim_object_index = game_seconds_to_ticks_round(seconds);
            }
        }
    }
    if (posing)
        actor->flags |= 0x80;
    else
        actor->flags &= ~0x80;
}


static __forceinline void moving_prop_ref_iterator_new(moving_prop_ref_iterator *iterator, long actor_index)
{
    actor_datum *actor = (actor_datum *)(actor_data->data + (unsigned short)actor_index * 2700);
    iterator->next_index = actor->first_prop_ref_index;
}
static __forceinline prop_ref_datum *moving_prop_ref_iterator_next(moving_prop_ref_iterator *iterator)
{
    prop_ref_datum *result = 0;
    if (iterator->next_index != -1)
    {
        result = datum_get<prop_ref_datum>(prop_ref_data, iterator->next_index);
        iterator->next_index = result->next_prop_ref_index;
    }
    return result;
}

// @address 0xb8c70 actor_moving_update_player_visibility
extern "C" void actor_moving_update_player_visibility(long actor_index)
{
    long actor_offset = (unsigned short)actor_index * 2700;
    long game_mode = game_globals->options.game_mode;
    bool enemy_team = true;
    actor_datum *actor = (actor_datum *)(actor_data->data + actor_offset);
    long team = actor->team;
    if (game_mode == 2)
    {
        enemy_team = game_engine_team_is_enemy((e_game_team)team, _game_team_player);
    }
    else if (team == 1)
    {
        enemy_team = false;
    }
    else if (team != -1 && game_mode == 1 && (unsigned int)team <= 15)
    {
        int bit = (team << 4) + 1;
        enemy_team = (tls_game_allegiance_globals()->enemy_bits[bit >> 5] & (1 << (bit & 31))) == 0;
    }
    if (enemy_team == 0 && actor->unit_index != -1 && mover_get_motor_program(actor->unit_index) == 0 && actor->root_vehicle_index == -1 &&
        actor->suppress_player_visibility == 0 && actor->berserking == 0 && actor->command_index == -1)
    {
        bool player_visible = false;
                            if (actor->combat_status > 5)
                            {
                                moving_prop_ref_iterator iterator;
                                moving_prop_ref_iterator_new(&iterator, actor_index);
                                prop_ref_datum *prop_ref;
                                while ((prop_ref = moving_prop_ref_iterator_next(&iterator)) != 0)
                                {
                                    prop_datum *prop = datum_get<prop_datum>(prop_data, prop_ref->prop_index);
                                    prop_state *state = prop_state_get(prop_ref);
                                    if (prop->valid != 0)
                                        continue;
                                    if (prop->flag == 0)
                                        continue;
                                    if (state->index44 != -1)
                                        continue;
                                    if (state->b64)
                                        continue;
                                    long target_object = prop->object_index;
                                    unit_datum *target = (unit_datum *)object_get(target_object);
                                    long player_index = target->unit.player_index;
                                    if (player_index == -1 || target->object.parent_object_index != -1)
                                        continue;
                                    ai_player_state *player_state = ai_player_state_get(player_index);
                                    if (!player_state)
                                        continue;
                                    real_vector3d reset;
                                    if (!ai_player_try_and_get_combat_position(target_object, &reset))
                                        continue;
                                    real_vector3d zero = { 0.0f, 0.0f, 0.0f };
                                    actor_camera_data camera;
                                    unit_get_camera_position(target_object, &camera.position);
                                    if (actor_perception_compute_target_blockage(&camera.position, &reset,
                                            &actor->position.body, actor->unit_index, &zero, 0) < 1)
                                        continue;
                                    player_visible = true;
                                    actor->player_visible = 1;
                                }
                            }

        if (player_visible == 0)
        {
            if (actor->charge_timer > 0)
            {
                actor->charge_timer--;
                return;
            }
        }
        else
        {
            actor->charge_timer = (short)game_seconds_to_ticks_round(1.5f);
            return;
        }
    }
    else
    {
        actor->player_visibility_ticks = 0;
        actor->charge_timer = 0;
    }
    actor->player_visible_last = 0;
    actor->player_visible = 0;
}

struct character_weapon_properties;

// @address 0xb8308 ?actor_move_update@@YAXJ@Z
void actor_move_update(long actor_index)
{
    actor_datum *actor = datum_get<actor_datum>(actor_data, actor_index);
    short iteration = 0;
    long result = 2;
    actor_look_affect_movement(actor_index);
    actor->flags = (actor->flags & ~0x600) | (_rotl(1, 9) & 0x600);
    actor->forward8e0 = actor->facing_vector;
    if ((actor->flags & 0x20) != 0)
    {
        union { real_point3d point; real_vector3d vector; } movement;
        if (actor_persistent_override_get_movement(actor_index, &movement.vector))
        {
            actor->blind_panic_control_active = 1;
            actor->control_point = movement.point;
        }
    }
    if (actor->blind_panic_control_active != 0)
    {
        actor->flags = (actor->flags & ~0x201) | 1;
        actor->throttle = actor->control_vector;
    }
    short timer = actor->move_freeze_ticks;
    if (timer > 0)
    {
        actor->destroying_obstacle = 1;
        actor->move_freeze_ticks = timer - 1;
    }
    if (actor->root_vehicle_index != -1 && actor->driver != 0)
        actor_moving_update_vehicle_throttle(actor_index);
    if (actor->destroying_obstacle != 0)
        actor->flags &= ~1;
    else
    {
        long weapon_index = actor_get_weapon(actor_index);
        if (weapon_index != -1)
        {
            object_datum *weapon = (object_datum *)(datum_get<s_object_header>(object_header_data, weapon_index)->object);
            character_weapon_properties *properties = character_weapon_properties_get(
                datum_get<actor_datum>(actor_data, actor_index)->character_index, weapon->definition_index);
            if (properties != 0 && (properties->flags & 1) != 0 && actor->target_state == 2)
                actor->flags &= ~1;
        }
    }
    if (actor->movement_obstacle_index != -1)
    {
        short remaining = (short)(actor->movement_obstacle_ticks - 1);
        actor->movement_obstacle_ticks = remaining;
        if (remaining <= 0)
        {
            actor->movement_obstacle_ticks = 0;
            actor->movement_obstacle_index = -1;
        }
    }
    long flags = actor->flags;
    if ((flags & 1) != 0 || (flags & 0x8000000) != 0)
        actor->flags = flags | 0x8000000;
    actor_moving_update_player_visibility(actor_index);
    do
    {
        switch (actor->vehicle_driver_type)
        {
        case 5:
            result = actor_move_calculate_directional_ground_movement(
                actor_index, actor->root_vehicle_index, &actor->throttle, &actor->forward8e0, &actor->output_throttle, iteration);
            break;
        case 4:
            result = actor_move_calculate_directional_flying_movement(
                actor_index, actor->root_vehicle_index, &actor->throttle, &actor->forward8e0, &actor->output_throttle, iteration);
            break;
        case 2:
            result = actor_move_calculate_nondirectional_flying_movement(
                actor_index, actor->root_vehicle_index, &actor->throttle, &actor->forward8e0, &actor->output_throttle, iteration);
            break;
        case 3:
            result = actor_move_calculate_nondirectional_ground_movement(
                actor_index, actor->root_vehicle_index, &actor->throttle, &actor->forward8e0, &actor->output_throttle, iteration);
            break;
        case 0:
        {
            long unit_index = actor->unit_index;
            if (actor->use_object != 0)
                result = actor_move_calculate_nondirectional_flying_movement(
                    actor_index, unit_index, &actor->throttle, &actor->forward8e0, &actor->output_throttle, iteration);
            else
            {
                unit_datum *unit = (unit_datum *)(datum_get<s_object_header>(object_header_data, unit_index)->object);
                signed char object_type = unit->object.object_type;
                if (object_type == 0)
                {
                    long motor_mode = *(long *)((char *)unit + unit->unit.motor_program_offset);
                    switch (motor_mode)
                    {
                    case 3:
                        break;
                    case 4:
                        result = actor_move_calculate_nondirectional_flying_movement(
                            actor_index, unit_index, &actor->throttle, &actor->forward8e0, &actor->output_throttle, iteration);
                        break;
                    case 5:
                        result = actor_move_calculate_climbing_movement(actor_index, &actor->throttle, &actor->forward8e0, &actor->output_throttle, iteration);
                        break;
                    default:
                        result = actor_move_calculate_ground_movement(actor_index, &actor->throttle, &actor->forward8e0, &actor->output_throttle, iteration);
                        break;
                    }
                }
                else if (object_type == 12)
                    result = actor_move_calculate_giant_scarab_movement(
                        actor_index, unit_index, &actor->throttle, &actor->forward8e0, &actor->output_throttle, iteration);
            }
            break;
        }
        case 1:
        default:
            actor->flags &= ~1;
            result = 2;
            break;
        }
        iteration++;
    } while (actor->blind_panic_control_active == 0 && result == 1 && actor_advance_path(actor_index));
    if (result == 1)
        actor->flags &= ~1;
    if ((actor->flags & 1) != 0 && result != 2)
    {
        real_vector3d delta;
        if (magnitude_squared3d(vector_from_points3d(&actor->position.body, &actor->movement_point, &delta)) < 0.0009f)
            actor->movement_point_ticks++;
        else
        {
            actor->movement_point_ticks = 0;
            actor->movement_point = actor->position.body;
        }
        if (((game_time_globals_definition *)TLS_GAME_TIME_GLOBALS)->tick_length * (float)actor->movement_point_ticks > 2.0f)
        {
            bool executing = false;
            actor->flags |= 8;
            actor_datum *path_actor = datum_get<actor_datum>(actor_data, actor_index);
            if (path_actor->path.valid != 0 && path_actor->path_state == 1)
                executing = true;
            if (executing)
            {
                short path_timer = (short)(actor->path_result_time + 1);
                actor->path_result_time = path_timer;
                if (path_timer > 32)
                    actor->path_result_time = 0;
                actor->movement_point_ticks = 0;
            }
        }
    }
    else
    {
        actor->flags &= ~8;
        actor->movement_point = actor->position.body;
        actor->movement_point_ticks = 0;
    }
    if ((actor->flags & 1) != 0 || result != 0)
        actor->flags &= ~0x8000000;
    if ((actor->flags & 1) != 0)
        actor->flags &= ~0x200;
    character_movement_properties *movement = character_definition_movement_properties_get(actor->character_index);
    bool crouch = false;
    if ((movement == 0 || (movement->flags & 0x400) == 0) && actor->passenger == 0)
    {
        if (actor_move_update_actor_get(actor_index)->path.valid != 0 && (actor->flags & 1) != 0)
            crouch = actor->moving_crouch;
        else
            crouch = actor->crouch;
    }
    actor_unit_control_crouch(actor_index, crouch);
    actor_movement_update_posing(actor_index);
}

static long actor_select_combat_movement_type(long actor_index)
{
    actor_datum *actor = datum_get<actor_datum>(actor_data, actor_index);
    long movement = 138;
    if (actor->throttle_ticks > 0)
        movement = 140;
    else if (actor->had_shield)
        movement = 139;
    return movement;
}

// @address 0xc78e0 ?actor_get_movement_type@@YAJJ@Z
long actor_get_movement_type(long actor_index)
{
    actor_datum *actor = datum_get<actor_datum>(actor_data, actor_index);
    long movement;
    long stored_movement_type = actor->control_type;
    if (stored_movement_type != -1)
        movement = stored_movement_type;
    else
    {
        movement = 138;
        if (actor->force_movement_71)
            movement = 71;
        else if (actor->flee_movement)
            movement = 145;
        else
        {
            if (actor->berserking && (actor->output_animation_state == 51 || actor_test_movement_type(actor_index, 51)))
                movement = 51;
            else
            {
                switch (actor->behavior_mode)
                {
                case 1:
                case 3:
                    if (actor->weapon_lowered)
                    {
                        movement = 137;
                        break;
                    }
                case 4:
                    movement = actor_select_combat_movement_type(actor_index);
                    break;
                case 5:
                    movement = 145;
                    break;
                default:
                    break;
                }
            }
        }
    }
    if (movement == 136)
        actor->danger_ticks = 0;
    else if (movement != 145)
        return movement;
    else if ((actor->flags & 1) == 0 &&
        (game_globals->options.game_mode != 1 || (actor->team != 1 && game_team_is_enemy((e_game_team)actor->team, _game_team_player))))
        movement = 138;
    return movement;
}


// @address 0xca9f8 ?steering_control2d@@YA_NJJPBTreal_vector3d@@MMMMPAMPAT1@@Z
bool steering_control2d(long actor_index, long vehicle_index, const real_vector3d *desired, float angle, float delta, float scale,
    float current, float *out_angle, real_vector3d *out_direction)
{
    const object_datum *object = (const object_datum *)((*object_header_get(vehicle_index)).object);
    const character_vehicle_properties *properties = (const character_vehicle_properties *)character_vehicle_properties_get(
        actor_index, object->definition_index);
    bool result = false;
    float maximum;
    float difference;
    float limit;
    angle += delta;
    if (properties->steering_angle_scale > 0.0f)
        angle *= properties->steering_angle_scale;
    if (properties->maximum_steering_angle > 0.0f)
        maximum = 0.017453292f * properties->maximum_steering_angle;
    else
        maximum = 1.5707964f;
    if (scale > 0.0f)
        maximum *= scale;
    angle = PIN(angle, -maximum, maximum);
    difference = angle - current;
    if (properties->maximum_steering_delta > 0.0f)
    {
        if (difference * current >= 0.0f && scale > 0.0f)
            limit = properties->maximum_steering_delta * scale * 0.017453292f;
        else
            limit = 0.017453292f * properties->maximum_steering_delta;
    }
    else
        limit = 6.2831855f;
    difference = PIN(difference, -limit, limit);
    angle = difference + current;
    *out_direction = *desired;
    out_direction->k = 0.0f;
    if (normalize3d(out_direction) != 0.0f)
    {
        float cosine = (float)cos(angle);
        float sine = (float)sin(angle);
        rotate_vector_about_axis(out_direction, &k_real_math_identity_matrix_storage.up, sine, cosine);
        *out_angle = angle;
        result = true;
    }
    return result;
}

static __forceinline void actor_moving_direction_set_vector(c_ai_direction *direction, const real_vector3d *vector)
{
    memset(direction, 0, sizeof(*direction));
    direction->type = 5;
    memcpy(&direction->m_point.point, vector, sizeof(direction->m_point.point));
}

static __forceinline void actor_moving_action_setup_corner(c_ai_action *action, const c_ai_point3d *point, const c_ai_direction *direction, short s, bool b)
{
    action->type = 2;
    action->point_valid = true;
    action->point = *point;
    action->direction = *direction;
    action->corner_type = s;
    action->corner_flag = b;
}

// @address 0x515290 ?corner@c_destination_orders@@QAA_NJPBVc_ai_point3d@@PATreal_vector3d@@F_N@Z
bool c_destination_orders::corner(long actor_index, const c_ai_point3d *point, real_vector3d *normal, short count, bool flag)
{
    bool result = false;
    if (type != 0)
    {
        action.type = 0;
        c_path_destination destination;
        if (to_raw(actor_index, &destination, 0) && point->location[0] == destination.point.location[0])
        {
            c_ai_point3d entry;
            c_sector_ref entry_sector;
            if (runtime_cornering_get_entry(actor_index, &destination.point, destination.sector, point, normal, count, flag, &entry, &entry_sector))
            {
                c_ai_direction direction;
                actor_moving_direction_set_vector(&direction, normal);
                actor_moving_action_setup_corner(&action, point, &direction, count, flag);
                flags.set(_destination_flag_3, true);
                result = true;
            }
        }
    }
    return result;
}

// @address 0x5153e8 ?posture@c_destination_orders@@QAA_NJPBVc_ai_point3d@@PATreal_vector3d@@J@Z
bool c_destination_orders::posture(long actor_index, const c_ai_point3d *point, real_vector3d *facing, long posture)
{
    bool result = false;
    if (type != 0)
    {
        action.type = 0;
        c_path_destination destination;
        real_matrix4x3 matrix;
        if (to_raw(actor_index, &destination, 0) && point->location[0] == destination.point.location[0])
        {
            bool entry = false;
            if (actor_index == -1 ||
                ai_hint_find_root_entry_matrix(
                    datum_get<actor_datum>(actor_data, actor_index)->unit_index, posture, 84, &point->point, facing, &matrix))
                entry = true;
            if (entry)
            {
                c_ai_direction direction;
                memset(&direction, 0, sizeof(direction));
                direction.type = 5;
                memcpy(&direction.m_point.point, facing, sizeof(direction.m_point.point));
                actor_moving_action_setup_posture(&action, point, &direction, posture);
                flags.set(_destination_flag_3, true);
                result = true;
            }
        }
    }
    return result;
}


// @address 0x11eba8 ?actor_move@@YA_NJPAVc_destination_orders@@PAUpath_state@@@Z
bool actor_move(long actor_index, c_destination_orders *orders, path_state *path_state)
{
    actor_datum *actor = datum_get<actor_datum>(actor_data, actor_index);
    bool changed = false;
    if (!actor->destination_orders.equals(actor_index, orders))
    {
        actor_datum *reset = datum_get<actor_datum>(actor_data, actor_index);
        reset->path.valid = false;
        reset->path.obstacle_object_index = -1;
        reset->path.result_value = 0;
        reset->path_result_time = 0;
        reset->path_result_count = 0;
        reset->path_state = 0;
        reset->destination_orders.clear();
        if (path_state)
        {
            actor->path_error = path_state->error;
        }
        if (actor->use_object)
            actor->movement_mode = 0;
        long motor_program = mover_get_motor_program(actor->unit_index);
        if ((motor_program == 4 || motor_program == 5) && orders->ground_destination())
            orders->land(actor_index);
        memcpy(&actor->destination_orders, orders, sizeof(actor->destination_orders));
        changed = true;
    }
    else if (actor->path_result_time > 0)
    {
        path_state = 0;
    }
    actor->movement_target_valid = false;
    actor->movement_target = *global_origin3d;
    return actor_path_refresh(actor_index, changed, path_state);
}

static __forceinline void actor_moving_deceleration_copy_position(const real_point3d *in, real_point3d *out)
{
    *out = *in;
}
static __forceinline const s_data_array &actor_moving_deceleration_actor_array()
{
    return *actor_data;
}
static __forceinline actor_datum *actor_moving_deceleration_actor_get_absolute(long absolute_index)
{
    return &((actor_datum *)actor_moving_deceleration_actor_array().data)[absolute_index];
}
static __forceinline unit_datum *actor_moving_deceleration_object_get(long index)
{
    return (unit_datum *)(*object_header_get(index)).object;
}
static __forceinline void actor_moving_deceleration_body_position(long index, real_point3d *position)
{
    if (index != -1)
    {
        c_havok_component *component = datum_get<c_havok_component>(g_havok_component_data, index);
        if (component->get_rigid_body_count() > 0)
        {
            component->get_rigid_body_position(0, position);
        }
    }
}
static __forceinline const real_point3d *actor_moving_deceleration_get_position(const unit_datum *object) { return &object->object.bounding_center; }
// @address 0xbb720 ?deceleration_obstacle_attenuation@@YAMJJMMMMPBTreal_vector2d@@PATreal_point3d@@PAMPAW4e_deceleration_obstacle_results@@@Z
float deceleration_obstacle_attenuation(long actor_index, long object_index, float radius, float deceleration_distance,
    float deceleration_velocity, float stop_distance, const real_vector2d *direction, real_point3d *obstacle_position_out,
    float *obstacle_radius_out, e_deceleration_obstacle_results *results)
{
    long actor_slot = actor_index & 0xffff;
    actor_datum *actor = actor_moving_deceleration_actor_get_absolute(actor_slot);
    unit_datum *object = actor_moving_deceleration_object_get(object_index);
    const float one = 1.0f;
    float attenuation = one;
    if (actor->movement_obstacle_index != -1)
    {
        long obstacle_index = actor->movement_obstacle_index;
        unit_datum *obstacle = actor_moving_deceleration_object_get(obstacle_index);
        real_point3d position = *actor_moving_deceleration_get_position(object);
        real_point3d obstacle_position = *actor_moving_deceleration_get_position(obstacle);
        float obstacle_radius = obstacle->object.bounding_radius;
        long other_actor_index = -1;
        bool compatible = true;
        long object_type = object->object.identifier.get_type();
        long obstacle_type = obstacle->object.identifier.get_type();
        if (((1 << object_type) & 1) && ((1 << obstacle_type) & 1))
        {
            const biped_definition *definition = TAG_GET('bipd', const biped_definition, obstacle->object.definition_index);
            obstacle_radius = definition->physics.radius;
            other_actor_index = obstacle->unit.actor_index;
            actor_moving_deceleration_body_position(object->object.havok_component_index, &position);
            actor_moving_deceleration_body_position(obstacle->object.havok_component_index, &obstacle_position);
        }
        else if (((1 << object_type) & 2) && ((1 << obstacle_type) & 2))
        {
            if (obstacle->unit.driver_object_index != -1)
                other_actor_index = actor_moving_deceleration_object_get(obstacle->unit.driver_object_index)->unit.actor_index;
        }
        else
            compatible = false;
        if (!compatible)
            attenuation = one;
        else
        {
        float combined_radius = obstacle_radius + radius;
        float distance = distance2d(&obstacle_position.xy, &position.xy) - combined_radius;
        if (distance <= 0.0f)
            distance = 0.0f;
        if (distance < deceleration_distance)
        {
            bool avoid = true;
            bool obstacle_follows = false;
            bool actor_follows = false;
            {
                real_vector3d velocity;
                object_get_velocities(object_index, &velocity, 0);
            }
            {
                real_vector3d velocity;
                object_get_velocities(obstacle_index, &velocity, 0);
            }
            if (other_actor_index != -1)
            {
                real_vector2d movement = *direction;
                real_vector2d obstacle_movement = actor_moving_deceleration_movement_actor_get(other_actor_index)->throttle.xy;
                if (normalize2d(&movement) > 0.0f && normalize2d(&obstacle_movement) > 0.0f)
                {
                    float along, obstacle_along;
                    float nearest = rays_nearest_distance(&position.xy, &movement, &obstacle_position.xy,
                        &obstacle_movement, &avoid, &along, &obstacle_along);
                    if (avoid)
                    {
                        if (nearest > combined_radius * 0.5f)
                            avoid = false;
                        else
                        {
                            obstacle_follows = obstacle_along - along > 0.15f;
                            actor_follows = along - obstacle_along > 0.15f;
                        }
                    }
                }
            }
            if (avoid && other_actor_index != -1 && datum_get<actor_datum>(actor_data, other_actor_index)->movement_obstacle_index == object_index)
            {
                if (obstacle_follows)
                    avoid = false;
                else if (!actor_follows)
                {
                    short other_rank = actor_get_rank(other_actor_index);
                    short rank = actor_get_rank(actor_index);
                    if (rank > other_rank || actor_slot < (other_actor_index & 0xffff))
                        avoid = false;
                }
            }
            if (avoid)
            {
                float actor_velocity = dot_product2d(&object->object.linear_velocity.xy, direction);
                float obstacle_velocity = dot_product2d(direction, &obstacle->object.linear_velocity.xy);
                float relative_velocity = actor_velocity - obstacle_velocity;
                if (relative_velocity > 0.0f && deceleration_distance > 0.0f && deceleration_velocity > 0.0f)
                {
                    float proximity = (deceleration_distance - distance) / deceleration_distance;
                    proximity = PIN(proximity, 0.0f, one);
                    attenuation = (float)pow(one - proximity, relative_velocity / deceleration_velocity);
                }
                if (distance < stop_distance)
                {
                    float stop_attenuation = one - (stop_distance - distance) / stop_distance;
                    if (attenuation > stop_attenuation)
                        attenuation = stop_attenuation;
                }
            }
        }
        }
    }
    return attenuation;
}



static __forceinline object_datum *actor_moving_vehicle_object_get(long object_index)
{
    return (object_datum *)((*object_header_get(object_index)).object);
}

// @address 0xbb3b0 ?actor_moving_vehicle_desired_speed@@YAMJJPBTreal_point2d@@PBTreal_vector2d@@111M_N@Z
float actor_moving_vehicle_desired_speed(long actor_index, long object_index, const real_point2d *point, const real_vector2d *direction,
    const real_vector2d *facing, const real_vector2d *movement, const real_vector2d *turn, float distance, bool ignore_direction)
{
    real_vector2d velocity;
    actor_datum *actor = datum_get<actor_datum>(actor_data, actor_index);
    object_datum *object = actor_moving_vehicle_object_get(object_index);
    character_vehicle_properties *vehicle = character_vehicle_properties_get(actor_index, object->definition_index);
    actor_path *path = &actor->path;
    float direction_attenuation = 1.0f;
    float turn_attenuation = 1.0f;
    float velocity_attenuation = 1.0f;
    float obstacle_attenuation = 1.0f;
    float throttle = 1.0f;
    float goal_attenuation = 1.0f;
    if (actor->throttle_override_valid)
    {
        float scale = actor->throttle_override;
        if (scale != 0.0f)
            throttle = scale;
    }
    if (!ignore_direction)
    {
        float direction_factor = vehicle->direction_attenuation;
        float alignment = dot_product2d(facing, movement);
        if (!(alignment > 0.0f))
            alignment = 0.0f;
        direction_attenuation = (1.0f - direction_factor) * alignment + direction_factor;
        if (object->object_type == 1)
        {
            velocity = object->linear_velocity.xy;
            normalize2d(&velocity);
            velocity_attenuation = (dot_product2d(&velocity, movement) + 2.0f) / 3.0f;
        }
    }
    float boost = vehicle->deceleration_distance;
    if (turn)
    {
        float limit = boost * 2.0f;
        if (distance < limit)
        {
            float turn_factor = vehicle->turn_attenuation;
            float raw_cosine = dot_product2d(movement, turn);
            float cosine = (float)PIN(raw_cosine, -1.0, 1.0);
            float angle = arccosine(cosine) * 0.31830987f;
            if (!(angle <= 1.0f))
                angle = 1.0f;
            float blend_input = 1.0f - angle;
            float blend = (1.0f - turn_factor) * blend_input * blend_input + turn_factor;
            turn_attenuation = (1.0f - blend) * (distance * distance) / (limit * limit) + blend;
        }
    }
    bool path_executing = actor_moving_step_vector_path_executing(actor_index);
    if (path_executing)
    {
        if ((_rotl(actor->destination_orders.flags.m_storage, 30) & 1) == 0)
        {
            float distance_to_goal = distance2d(point, &path->destination.point.xy);
            if (!(distance_to_goal >= boost))
            {
                float goal_base = vehicle->goal_attenuation;
                goal_attenuation = (1.0f - goal_base) / boost * distance_to_goal + goal_base;
            }
        }
        else
        {
            float actor_goal = actor->destination_orders.f20;
            if (actor_goal != 0.0f)
            {
                real_vector2d delta;
                vector_from_points2d(point, &path->destination.point.xy, &delta);
                float distance_to_goal = sqrtf(delta.j * delta.j + delta.i * delta.i);
                if (!(distance_to_goal >= boost))
                {
                    float scaled_goal = actor_goal / throttle;
                    goal_attenuation = (1.0f - scaled_goal) / boost * distance_to_goal + scaled_goal;
                }
            }
        }
    }
    bool obstacle_checking = actor_moving_step_vector_path_executing(actor_index);
    if (obstacle_checking)
    {
        obstacle_attenuation = deceleration_obstacle_attenuation(actor_index, object_index, vehicle->pathfinding_radius,
            vehicle->deceleration_distance, 2.0f, 2.0f, direction, NULL, NULL, NULL);
    }
    float desired_speed = direction_attenuation > (turn_attenuation > goal_attenuation ? goal_attenuation : turn_attenuation)
        ? (turn_attenuation > goal_attenuation ? goal_attenuation : turn_attenuation) : direction_attenuation;
    desired_speed = desired_speed > obstacle_attenuation ? obstacle_attenuation : desired_speed;
    desired_speed *= velocity_attenuation;
    desired_speed *= throttle;
    return desired_speed;
}

// @address 0x516748 actor_moving_compute_jump_velocity
extern "C" int actor_moving_compute_jump_velocity(long actor_index, long object_index, unsigned char flag, long, real_vector3d *out_vector)
{
    actor_datum *actor = datum_get<actor_datum>(actor_data, actor_index);
    if (actor->root_vehicle_index == -1 && TEST_BIT(actor->flags, 6))
    {
        long prop_ref_index = actor->movement_index;
        if (prop_ref_index != -1 && flag)
        {
            if (TEST_BIT(1 << (*object_header_get(object_index)).object_type, 0))
            {
                character_charge_properties *charge = character_definition_charge_multi_properties_get(actor_index);
                if (charge != 0)
                {
                    prop_ref_datum *prop_ref = (prop_ref_datum *)datum_try_and_get((const s_data_array *)prop_ref_data, prop_ref_index);
                    if (prop_ref != 0)
                    {
                        prop_state *prop = (prop_state *)prop_state_get(prop_ref);
                        float time_scale = actor->jump_horizontal_velocity;
                        float speed = charge->jump_speed;
                        float one;
                        real_vector3d dir;
                        float out_f4;
                        float out_f0;
                        real_point3d target;
                        float flight_time;
                        real_vector3d launch;
                        float minimum_speed;
                        target = prop->body_position;
                        float zero = 0.0f;
                        flight_time = 2.0f;
                        if (time_scale > zero)
                        {
                            float dx = actor->position.body.x - prop->body_position.x;
                            float dy = actor->position.body.y - prop->body_position.y;
                            float dz = actor->position.body.z - prop->body_position.z;
                            flight_time = sqrtf(dx * dx + (dz * dz + dy * dy)) / time_scale;
                        }
                        collision_result collision;
                        dir = prop->velocity;
                        dir.k = zero;
                        scale_vector3d(&dir, flight_time, &launch);
                        float distance = normalize3d(&dir);
                        if (distance > zero)
                        {
                            float radius = TAG_GET('bipd', const biped_definition, object_get(object_index)->definition_index)->physics.radius;
                            if (collision_test_vector(k_flying_vehicle_collision_flags, false, &prop->center, &launch, -1, -1, -1, &collision))
                                distance = sqrtf(magnitude_squared3d(&launch)) * collision.fraction;
                            float t = distance - radius;
                            point_from_line3d(&prop->body_position, &dir, t, &target);
                        }
                        one = 1.0f;
                        unsigned char aimed = projectile_aim_ballistic(
                            speed, one, &actor->position.body, &target, &minimum_speed, 0, 0, false, &dir, 0, 0, 0, &out_f0, &out_f4);
                        if (!aimed)
                            aimed = projectile_aim_ballistic(
                                minimum_speed + 0.03f, one, &actor->position.body, &target, 0, 0, 0, false, &dir, 0, 0, 0, &out_f0, &out_f4);
                        if (aimed)
                        {
                            actor->jump_horizontal_velocity = out_f4;
                            actor->jump_vertical_velocity = out_f0;
                            dir.k = zero;
                            if (normalize3d(&dir) > zero)
                                actor->jump_aim = dir.xy;
                        }
                    }
                }
            }
        }
        float duration = actor->jump_horizontal_velocity;
        float scaled_j = actor->jump_aim.j * duration;
        float k_val = actor->jump_vertical_velocity;
        float scaled_i = actor->jump_aim.i * duration;
        out_vector->i = scaled_i;
        out_vector->k = k_val;
        out_vector->j = scaled_j;
    }
    actor->flags &= ~64;
    return 1;
}



struct path_debug_storage;

struct path_result;










static __forceinline void actor_moving_path_result_reset(long actor_index, bool reset_errors)
{
    actor_datum *actor = datum_get<actor_datum>(actor_data, actor_index);
    actor->path.valid = false;
    actor->path.obstacle_object_index = -1;
    actor->path.result_value = 0;
    if (reset_errors)
    {
        actor->path_result_time = 0;
        actor->path_result_count = 0;
    }
}

static __forceinline void actor_moving_actor_path_clear(long actor_index)
{
    actor_datum *actor = datum_get<actor_datum>(actor_data, actor_index);
    actor_moving_path_result_reset(actor_index, true);
    actor->path_state = 0;
    actor->destination_orders.clear();
}

static __forceinline void actor_moving_path_state_set_destination(path_state *state, const c_path_destination *destination)
{
    state->goal = *destination;
}

// @address 0xcc5b0 ?actor_path_refresh@@YA_NJ_NPAUpath_state@@@Z
bool actor_path_refresh(long actor_index, bool changed, path_state *extra)
{
    actor_datum *actor = datum_get<actor_datum>(actor_data, actor_index);
    c_destination_orders *orders = &actor->destination_orders;
    bool trivial = false;
    bool result = true;
    e_pathfinding_mode mode;
    c_path_destination destination;
    if (actor->movement_3d)
    {
        result = false;
        actor_moving_actor_path_clear(actor_index);
    }
    else if (actor->destroying_obstacle)
    {
        result = true;
    }
    else
    {
        if ((unsigned short)datum_get<actor_datum>(actor_data, actor_index)->path_state == 2 &&
            actor_test_destination_internal(actor_index, actor_destination_tolerance(actor_index) * 3.0f))
        {
            path_finish(actor_index, false);
            result = true;
        }
        else if (orders->type == 0)
        {
            actor_moving_actor_path_clear(actor_index);
        }
        else
        {
            path_refresh_scratch scratch;
            actor_moving_compute_reserved_hints(actor_index, &scratch.reserved);
            actor_moving_path_result_reset(actor_index, false);
            actor->path_state = 0;
            result = orders->to_raw(actor_index, &destination, &mode);
            if (result)
            {
                switch (mode)
                {
                case 1:
                {
                    long second_ignore = -1;
                    firing_position_ref reference = FP_REF_NONE;
                    if (orders->type == 3)
                    {
                        reference = orders->firing_position;
                    }
                    else if (orders->type == 4)
                    {
                        long object_index = datum_get<prop_ref_datum>(prop_ref_data, orders->prop_ref_index)->object_index;
                        long parent_index = NONE;
                        while (object_index != NONE)
                        {
                            parent_index = object_index;
                            object_index = object_get(object_index)->parent_object_index;
                        }
                        second_ignore = parent_index;
                    }
                    if (orders->target_index != -1)
                        second_ignore = orders->target_index;
                    if (actor->root_vehicle_index != -1)
                        ai_get_center_of_mass(actor->root_vehicle_index, &scratch.point);
                    else
                        ai_get_center_of_mass(actor->unit_index, &scratch.point);
                    flight_evaluation_cache *cache = extra != 0 ? &extra->flight_cache : 0;
                    result = path_3d_build_path(actor_index, orders, &scratch.point, &destination, reference, cache, second_ignore,
                        (path_result *)&actor->path.valid);
                    if (result)
                        trivial = true;
                }
                    break;
                case 0:
                {
                    if (extra == 0 || extra->node_count <= 0)
                    {
                        result = false;
                        path_input input;
                        actor_path_input_new(actor_index, &input);
                        if (orders->target_index != -1)
                            input.destination_object_index = orders->target_index;
                        if (actor->danger_zone.danger_type > 0 && actor->danger_zone.forced == 0)
                        {
                            input.use_attractor = true;
                            input.pad47 = true;
                            input.attractor_weight = 10.0f;
                            input.attractor_object_index = actor->danger_zone.object_index;
                            input.attractor_radius = actor->danger_zone.radius;
                            input.attractor_strength = actor->danger_zone.radius;
                            input.attractor = actor->danger_zone.position;
                        }
                        special_movement special;
                        actor_special_movement_new(actor_index, &special);
                        c_path_state path;
                        path_state *scratch_state = (path_state *)path.m_data;
                        path_state_new(&input, scratch_state, &actor->path_error, 0, &special, &scratch.reserved);
                        actor_moving_path_state_set_destination(scratch_state, &destination);
                        scratch_state->has_goal = true;
                        if (path_state_find(scratch_state))
                        {
                            result = path_state_build_path(actor_index, scratch_state, (path_result *)&actor->path.valid);
                            if (result)
                            {
                                actor_moving_update_nearest_object_distance(actor_index, (path_state *)scratch_state);
                                trivial = scratch_state->best_x98 == 0.0f;
                            }
                            else if (actor->path.failed != 0 && orders->type == 3)
                            {
                                actor_discard_firing_position(actor_index, orders->firing_position, false);
                            }
                        }
                        else
                        {
                            actor->path_error = scratch_state->error;
                        }
                    }
                    else
                    {
                        actor_moving_path_state_set_destination(extra, &destination);
                        extra->has_goal = true;
                        result = path_state_build_path(actor_index, extra, (path_result *)&actor->path.valid);
                        if (result)
                        {
                            actor_moving_update_nearest_object_distance(actor_index, (path_state *)extra);
                            trivial = extra->best_x98 == 0.0f;
                        }
                    }
                }
                    break;
                default:
                    result = false;
                    break;
                }
                actor->path_refresh_disabled = true;
            }
            if (result)
            {
                actor->path_state = 1;
                actor->isolation_level = 0;
            }
            else
            {
                actor_datum *failed = datum_get<actor_datum>(actor_data, actor_index);
                failed->path.valid = false;
                failed->path_state = 3;
                failed->path_refresh_failures++;
            }
        }
    }
    if (changed)
    {
        bool set_flag = false;
        if (result && !actor->berserking && actor_executing_layer(actor_index, 27) == -1)
        {
            character_movement_properties *movement = character_definition_movement_properties_get(actor->character_index);
            if (movement != 0)
            {
                float max_distance = movement->max_path_distance;
                if (!(max_distance < 0.0f) && actor->path.final_point)
                {
                    float distance = actor_path_distance_remaining(actor_index, -1);
                    set_flag = distance != 0.0f && distance < max_distance;
                }
            }
        }
        if (set_flag)
            actor->flags |= 0x10000000;
        else
            actor->flags &= ~0x10000000;
    }
    switch (orders->type)
    {
    case 3:
        if (changed)
        {
            const firing_position_ref *reference = &orders->firing_position;
            if (!(reference->block & 0x8000) || orders->firing_position_index == FP_REF_CURRENT.value)
            {
                if (reference->value != FP_REF_CURRENT.value)
                    firing_position_valid(actor_index, *reference);
            }
        }
        break;
    case 4:
        if (result == 0 || trivial)
            actor_perception_unreachable(actor_index, orders->firing_position_index, result == 0, true);
        break;
    }
    return result;
}

static __forceinline character_guardian_properties *actor_moving_jump_guardian_properties_get(long character_index)
{
    character_guardian_properties *result = 0;
    while (character_index != -1)
    {
        character_definition *definition = TAG_GET('char', character_definition, character_index);
        if (definition->guardian_properties.count > 0)
        {
            result = (character_guardian_properties *)definition->guardian_properties.address;
            break;
        }
        character_index = definition->parent_character.index;
    }
    return result;
}
static __forceinline bool actor_moving_jump_get_destination_normal(
    const c_hint_ref *hint_ref, const hint_jump_data *jump, real_vector3d *normal)
{
    bool result = false;
    if (TEST_BIT(jump->flags, 1))
    {
        if (TEST_BIT(jump->flags, 2))
        {
            result = false;
        }
        else
        {
            pathfinding_data *data = pathfinding_data_get(hint_ref->structure_bsp_index);
            long index = jump->vertex2;
            memcpy(normal, &data->vertices[index], sizeof(*normal));
            normalize3d(normal);
            result = true;
        }
    }
    return result;
}
static __forceinline float actor_moving_jump_game_seconds_to_ticks_real(float seconds)
{
    return seconds * (float)TLS_GAME_TIME_GLOBALS->ticks_per_second;
}
// @rdata 0x82022db8
static const float actor_moving_jump_maneuver_padding = 0.4f;
// @rdata 0x82022dc0
static const float actor_moving_jump_half = 0.5f;
// @rdata 0x8203385c
static const float actor_moving_jump_ground_offset = 0.2f;
static __forceinline long actor_moving_jump_game_seconds_to_ticks_round(float seconds)
{
    return (long)(seconds * (float)TLS_GAME_TIME_GLOBALS->ticks_per_second + actor_moving_jump_half);
}
static __forceinline long actor_moving_jump_round_ticks(float ticks)
{
    long rounded;
    __stfiwx(__fctiwz(ticks + actor_moving_jump_half), 0, &rounded);
    return rounded;
}
static __forceinline void actor_moving_jump_set_persistent_movement_override(
    long actor_index, const real_point3d *point, const real_vector3d *vector, float distance, short ticks)
{
    actor_datum *actor = datum_get<actor_datum>(actor_data, actor_index);
    actor_persistent_movement *persistent = &actor->persistent_movement;
    persistent->direction = *vector;
    persistent->offset = dot_product3d(point, &persistent->direction);
    actor->persistent_movement.speed = distance;
    actor->persistent_movement.ticks = ticks;
    actor->flags = (actor->flags & ~0x8) | 0x20;
}

// match: 0x517540: datum_get<actor_datum> for the jumping actor swaps the operands of the datum address add.
static __forceinline actor_datum *actor_moving_jump_actor_get(long actor_index)
{
    return &((actor_datum *)actor_data->data)[DATUM_INDEX_TO_ABSOLUTE_INDEX(actor_index)];
}
// @address 0x517540 ?actor_moving_jump@@YA_NJPAVc_hint_ref@@_NDPAVc_ai_point3d@@Vc_sector_ref@@1@Z
bool actor_moving_jump(long actor_index, c_hint_ref *hint_ref, bool test_levels, char jump_levels, c_ai_point3d *destination,
    c_sector_ref destination_sector, bool unlimited)
{
    actor_datum *actor = actor_moving_jump_actor_get(actor_index);
    character_movement_properties *movement = character_definition_movement_properties_get(actor->character_index);
    bool limited = false;
    bool success = false;
    float minimum_speed = 0.0f;
    bool small_jump = false;
    bool boosted = false;
    bool forced_boost = false;
    short jump_level = 3;
    real_point3d destination_position;
    float max_speed;

    ai_point_get_position(destination, &destination_position);
    max_speed = actor_get_max_jump_speed(actor_index, &limited);
    if (movement && TEST_BIT(movement->flags, 5))
    {
        unlimited = false;
        forced_boost = true;
    }
    if (!test_levels)
    {
        success = true;
    }
    else if (movement)
    {
        short max_index = (movement->jump_height < 6 ? movement->jump_height : 6) - 1;
        if (unlimited)
        {
            short mask = (short)(((1 << (max_index + 1)) - 1) | jump_levels);
            if (mask != 0)
            {
                small_jump = (jump_levels & 1) != 0;
                success = true;
            }
        }
        else
        {
            for (short level = 0; level < 6; level++)
            {
                if (TEST_BIT(jump_levels, level))
                {
                    jump_level = level + 1;
                    if (level <= max_index)
                        max_index = level;
                    max_speed = ai_hint_get_jump_velocity(max_index, 0.0f);
                    small_jump = level == 0;
                    success = true;
                    break;
                }
            }
        }
    }
    if (limited)
        unlimited = true;
    if (forced_boost)
        success = true;

    if (success)
    {
        pathfinding_hint *hint = 0;
        float jump_speed = max_speed;
        float horizontal_velocity;
        float vertical_velocity;
        float speed_padding = 0.03f;
        real_vector3d aim_vector;
        action_request maneuver_request;
        action_request destination_request;
        action_request guardian_request;
        action_request motor_request;
        if (hint_ref && hint_ref->valid())
        {
            hint = (pathfinding_hint *)actor_moving_get_hint(hint_ref);
            if (hint && TEST_BIT(hint->jump.flags, 4))
                speed_padding = actor_moving_jump_maneuver_padding;
        }
        success = false;
        if (hint && TEST_BIT(hint->jump.flags, 5) && actor->root_vehicle_index == -1)
        {
            c_ai_point3d start_point;
            c_ai_point3d end_point;
            pathfinding_data *data = pathfinding_data_get(hint_ref->structure_bsp_index);
            if (ai_hint_get_jump_maneuver_point(data, (struct pathfinding_hint_data *)hint, destination, destination_sector,
                    movement->pathfinding_radius, &start_point, 0, &end_point, 0))
            {
                real_point3d start_position;
                real_point3d end_position;
                ai_point_get_position(&start_point, &start_position);
                ai_point_get_position(&end_point, &end_position);
                vector_from_points3d(&start_position, &end_position, &aim_vector);
                aim_vector.k = 0.0f;
                if (normalize3d(&aim_vector) > 0.0f)
                {
                    real_point3d test_point;
                    real_vector3d test_vector;
                    collision_result collision;
                    s_collision_test_flags test_flags;
                    point_from_line3d(&start_position, &aim_vector, 0.15f, &test_point);
                    point_from_line3d(&test_point, global_up3d, actor_moving_jump_maneuver_padding + actor_moving_jump_ground_offset, &test_point);
                    scale_vector3d(global_down3d, 2.0f * actor_moving_jump_ground_offset, &test_vector);
                    test_flags.collision_flags = 0x809;
                    test_flags.object_flags = 1;
                    if (collision_test_vector(test_flags, false, &test_point, &test_vector, -1, -1, -1, &collision))
                    {
                        test_point = collision.point;
                    }
                    else
                    {
                        point_from_line3d(&test_point, global_down3d, actor_moving_jump_ground_offset, &test_point);
                    }
                    csmemset(&maneuver_request, 0, sizeof(maneuver_request));
                    maneuver_request.action = 40;
                    maneuver_request.hoist.alignment = aim_vector;
                    maneuver_request.hoist.position = test_point;
                    ((c_interaction_target *)&maneuver_request.hoist.reference)->clear();
                    success = action_submit(actor->unit_index, &maneuver_request);
                }
            }
            if (!success)
                speed_padding = actor_moving_jump_maneuver_padding;
        }
        if (!success)
        {
            if (!forced_boost || (jump_level <= 4 && test_levels))
            {
                success = projectile_aim_ballistic(max_speed, actor_get_gravity_scale(actor_index), &actor->position.body, &destination_position,
                    &minimum_speed, 0, 0, true, &aim_vector, 0, 0, 0, &vertical_velocity, &horizontal_velocity);
                if (unlimited || minimum_speed < max_speed * 1.25f)
                {
                    float boosted_speed = speed_padding + minimum_speed;
                    float boosted_minimum_speed;
                    float boosted_vertical;
                    float boosted_horizontal;
                    if (projectile_aim_ballistic(boosted_speed, actor_get_gravity_scale(actor_index), &actor->position.body,
                            &destination_position, &boosted_minimum_speed, 0, 0, true, &aim_vector, 0, 0, 0, &boosted_vertical,
                            &boosted_horizontal))
                    {
                        horizontal_velocity = boosted_horizontal;
                        success = true;
                        vertical_velocity = boosted_vertical;
                        jump_speed = boosted_speed;
                    }
                }
            }
            else
            {
                success = false;
            }
            if (!success && forced_boost)
            {
                float boost_minimum_speed;
                float vertical;
                float horizontal;
                float boost_speed = max_speed;
                bool aimed = projectile_aim_ballistic(boost_speed, actor_moving_jump_half, &actor->position.body, &destination_position,
                    &boost_minimum_speed, 0, 0, true, &aim_vector, 0, 0, 0, &vertical, &horizontal);
                if (!aimed)
                {
                    boost_speed = boost_minimum_speed + speed_padding;
                    aimed = projectile_aim_ballistic(boost_speed, actor_moving_jump_half, &actor->position.body, &destination_position,
                        &boost_minimum_speed, 0, 0, true, &aim_vector, 0, 0, 0, &vertical, &horizontal);
                }
                if (aimed)
                {
                    success = true;
                    horizontal_velocity = horizontal;
                    vertical_velocity = vertical;
                    jump_speed = boost_speed;
                    boosted = true;
                }
            }
            if (success)
            {
                long motor_program = mover_get_motor_program(actor->unit_index);
                if (motor_program == 5)
                {
                    csmemset(&motor_request, 0, sizeof(motor_request));
                    motor_request.action = 29;
                    motor_request.position = destination_position;
                    success = action_submit(actor->unit_index, &motor_request);
                }
                else if (actor->type == 24)
                {
                    csmemset(&guardian_request, 0, sizeof(guardian_request));
                    float gravity_scale = 1.0f;
                    character_guardian_properties *guardian = actor_moving_jump_guardian_properties_get(actor->character_index);
                    if (guardian)
                        gravity_scale = guardian->phase_time;
                    guardian_request.action = 49;
                    guardian_request.phase.destination = destination_position;
                    guardian_request.phase.duration = gravity_scale;
                    success = action_submit(actor->unit_index, &guardian_request);
                }
                else if (hint && hint->type == 6 && !boosted)
                {
                    csmemset(&destination_request, 0, sizeof(destination_request));
                    real_vector3d normal = *global_up3d;
                    if (TEST_BIT(hint->jump.flags, 1))
                    {
                        if (!actor_moving_jump_get_destination_normal(hint_ref, &hint->jump, &normal))
                            normal = *global_up3d;
                    }
                    destination_request.action = 32;
                    destination_request.hoist.position = destination_position;
                    destination_request.hoist.alignment = normal;
                    success = action_submit(actor->unit_index, &destination_request);
                }
                else if (!small_jump && jump_speed > ai_hint_get_jump_velocity(0, 0.0f) * 1.2f)
                {
                    actor->leap_speed = horizontal_velocity;
                    actor->leap_requested = true;
                    actor->leap_target_valid = true;
                    actor->leap_height = vertical_velocity > 1.2f ? vertical_velocity : 1.2f;
                    actor->leap_prop_index = -1;
                    aim_vector.k = 0.0f;
                    actor->jump_boosted = boosted;
                    if (normalize2d(&aim_vector.xy) > 0.0f)
                    {
                        actor->leap_direction = aim_vector.xy;
                    }
                    else
                    {
                        aim_vector = actor->facing_vector;
                        if (aim_vector.k != 0.0f)
                        {
                            aim_vector.k = 0.0f;
                            if (normalize2d(&aim_vector.xy) <= 0.0f)
                                aim_vector = *global_forward3d;
                        }
                        actor->leap_direction = aim_vector.xy;
                    }
                }
                else if (!TEST_BIT(actor->flags, 5) && motor_program == 0)
                {
                    real_vector3d direction;
                    vector_from_points3d(&actor->position.body, &destination_position, &direction);
                    direction.k = 0.0f;
                    float distance = normalize3d(&direction) + movement->pathfinding_radius;
                    actor_moving_jump_set_persistent_movement_override(actor_index, &actor->position.body, &direction, distance,
                        (short)actor_moving_jump_game_seconds_to_ticks_round((distance + movement->pathfinding_radius) * 4.0f));
                }
            }
        }
    }
    if (success)
    {
        s_object_header *header = datum_get<s_object_header>(object_header_data, actor->unit_index);
        if (TEST_BIT(1 << ((s_object_header *)header)->object_type, 0))
        {
            biped_datum *unit = (biped_datum *)header->object;
            unit->biped.falling_danger_cooldown = (unsigned char)((long)(actor_moving_jump_game_seconds_to_ticks_real(2.0f) + actor_moving_jump_half) > 0xffff
                    ? 0xffff
                    : actor_moving_jump_round_ticks(actor_moving_jump_game_seconds_to_ticks_real(2.0f)));
        }
    }
    return success;
}


static __forceinline s_object_header *actor_moving_destination_vehicle_object_header_get(long object_index)
{
    return &((s_object_header *)object_header_data->data)[(unsigned short)object_index];
}
static __forceinline object_datum *actor_moving_destination_vehicle_object_get(long object_index)
{
    s_object_header *header = actor_moving_destination_vehicle_object_header_get(object_index);
    return (object_datum *)header->object;
}

// @address 0xb9dd8 actor_moving_has_vehicle_reached_step
extern "C" bool actor_moving_has_vehicle_reached_step(long actor_index, long vehicle_index, real_point3d *position, float distance,
    bool flying, bool *last_step_ref, bool *next_step_valid_ref, real_vector3d *next_step_direction_ref)
{
    bool result = false;
    actor_datum *actor = datum_get<actor_datum>(actor_data, actor_index);
    actor_path *path = &actor->path;
    object_datum *object = actor_moving_destination_vehicle_object_get(vehicle_index);
    s_object_definition *definition = (s_object_definition *)tag_get('vehi', object->definition_index);
    long current_step_index = path->point_index;
    bool last_step = current_step_index + 1 >= path->point_count;
    long definition_index = object->definition_index;
    bool next_step_valid = false;
    real_vector3d next_step_direction;
    if (actor->blind_panic_control_active)
    {
        next_step_valid = false;
        last_step = true;
    }
    else if (!last_step)
    {
        ai_vector_from_points3d(&path->points[path->point_index].point, &path->points[path->point_index + 1].point, &next_step_direction);
        if (!flying)
            next_step_direction.k = 0.0f;
        if (normalize3d(&next_step_direction) > 0.0001f)
            next_step_valid = true;
    }
    else
    {
        if (actor->destination_orders.flags.test(_destination_flag_1) || actor->destination_orders.flags.test(_destination_flag_0))
        {
            next_step_direction = actor->destination_orders.m_destination_facing;
            if (!flying)
                next_step_direction.k = 0.0f;
            if (normalize3d(&next_step_direction) != 0.0f)
                next_step_valid = true;
        }
        float tolerance = actor_destination_tolerance(actor_index);
        if (distance < tolerance)
            result = true;
    }
    if (!result && next_step_valid && (!last_step || actor->destination_orders.flags.test(_destination_flag_0)))
    {
        real_vector3d *direction = 0;
        real_vector3d step_direction;
        if (!last_step)
        {
            const c_ai_point3d *previous;
            if (current_step_index > 0)
                previous = &path->points[current_step_index - 1].point;
            else
                previous = &path->start_point;
            ai_vector_from_points3d(previous, &path->points[current_step_index].point, &step_direction);
            if (!flying)
                step_direction.k = 0.0f;
            float length = normalize3d(&step_direction);
            if (length > 0.0f)
                direction = &step_direction;
        }
        else
            direction = &next_step_direction;
        if (direction)
        {
            character_vehicle_properties *properties = character_vehicle_properties_get(actor_index, definition_index);
            float radius = definition->bounding_radius;
            if (properties)
                radius = properties->pathfinding_radius > properties->steering_distance ? properties->steering_distance : properties->pathfinding_radius;
            const real_point3d *step_position = &path->points[current_step_index].point.point;
            float step_projection = dot_product3d(step_position, direction);
            float projection = dot_product3d(position, direction);
            if (step_projection - projection < radius * 1.2f)
                result = true;
        }
    }
    *last_step_ref = last_step;
    if (last_step && !actor->destination_orders.flags.test(_destination_flag_1))
        *next_step_valid_ref = false;
    else
    {
        *next_step_valid_ref = next_step_valid;
        *next_step_direction_ref = next_step_direction;
    }
    return result;
}





// @address 0x516360
bool actor_move_find_evasion_direction(long actor_index, real_vector3d *avoidance_vector, real_vector3d *alignment_vector,
    short *evade_direction_reference, float minimum_distance, bool *blocked_reference)
{
    actor_datum *evading_actor = datum_get<actor_datum>(actor_data, actor_index);
    object_datum *object = 0;
    bool success = false;
    short iteration = 0;
    short direction_count = 1;
    float best_score = -0.1f;
    short best_direction = -1;
    bool best_blocked = false;
    float radius = 0.2f;
    character_movement_properties *movement = character_definition_movement_properties_get(evading_actor->character_index);
    if (movement)
        radius = movement->pathfinding_radius;
    short evade_direction = *evade_direction_reference;
    object = (object_datum *)((*object_header_get(evading_actor->unit_index)).object);
    const real_vector3d *up = &object->up;
    if (evade_direction == 5)
    {
        evade_direction = 0;
        direction_count = 4;
    }
    else if (evade_direction == 4)
    {
        evade_direction = 0;
        direction_count = 2;
        if ((long)(evading_actor->flags & 1))
        {
            real_vector3d side;
            cross_product3d(up, alignment_vector, &side);
            float projection = dot_product3d(&evading_actor->throttle, &side);
            if (projection > 0.5f)
            {
                evade_direction = 0;
                direction_count = 1;
            }
            else if (projection < -0.5f)
            {
                evade_direction = 1;
                direction_count = 1;
            }
        }
    }
    for (; iteration < direction_count; ++iteration, evade_direction = (evade_direction + 1) % 4)
    {
        short component;
        real_vector3d direction;
        long animation;
        switch (evade_direction)
        {
        case 0:
            cross_product3d(up, alignment_vector, &direction);
            animation = 54;
            component = 1;
            break;
        case 1:
            cross_product3d(alignment_vector, up, &direction);
            animation = 55;
            component = 1;
            break;
        case 2:
            direction = *alignment_vector;
            animation = 56;
            component = 0;
            break;
        case 3:
            scale_vector3d(alignment_vector, -1.0f, &direction);
            animation = 57;
            component = 0;
            break;
        }
        float score = 1.0f - fabsf(avoidance_vector->i * direction.i + (avoidance_vector->j * direction.j + avoidance_vector->k * direction.k))
            + 0.5f * _real_random_range(TLS_RANDOM_SEED, NULL, NULL, 0, 0.0f, 1.0f);
        float distance;
        if (score > best_score && motor_animation_get_range(evading_actor->unit_index, 226, animation, component, true, &distance, 0, 0, 0))
        {
            distance = fabsf(distance) + radius + 0.1f;
            bool blocked;
            if (actor_moving_can_move_along_vector(actor_index, &direction, distance, 0.0f, &blocked, 0))
            {
                best_direction = evade_direction;
                best_blocked = blocked;
                best_score = score;
                success = true;
            }
        }
    }
    *evade_direction_reference = best_direction;
    if (blocked_reference)
        *blocked_reference = best_blocked;
    return success;
}



static __forceinline const real_matrix4x3 *actor_moving_climbing_object_get_node_matrix(long object_index, short node_index)
{
    object_datum *object = (object_datum *)((*object_header_get(object_index)).object);
    return &((const real_matrix4x3 *)((char *)object + object->node_matrices.offset))[node_index];
}

// @address 0x516c40 ?actor_move_calculate_climbing_movement@@YA?AW4e_movement_result@@JPBTreal_vector3d@@PAT2@1F@Z
e_movement_result actor_move_calculate_climbing_movement(
    long actor_index, const real_vector3d *movement, real_vector3d *desired, real_vector3d *result, short iteration)
{
    actor_datum *actor = datum_get<actor_datum>(actor_data, actor_index);
    float step_distance = 3.4028234663852886e38f;
    actor_path_point *step = NULL;
    pathfinding_hint *hint = NULL;
    e_movement_result movement_result = _movement_result_none;
    real_vector3d facing = actor->facing_vector;
    real_vector3d climb_vector = *global_zero_vector3d;
    actor->flags &= ~FLAG(9);
    if (TEST_BIT(actor->flags, 0) && VALID_INDEX(actor->path.point_index, actor->path.point_count))
    {
        real_point3d position = actor->position.body;
        if (actor->unit_index != NONE)
            position.z = actor_moving_climbing_object_get_node_matrix(actor->unit_index, 0)->position.z;
        if (actor_moving_step_vector_path_executing(actor_index))
        {
            step = &actor->path.points[actor->path.point_index];
            if (step->hint.valid())
                hint = (pathfinding_hint *)actor_moving_get_hint(&step->hint);
            step_distance = sqrtf(ai_distance_squared3d(&step->point, &position));
        }
        real_vector3d movement_direction = *movement;
        if (normalize3d(&movement_direction) == 0.0f)
            movement_direction = *global_up3d;
        object_get_orientation(actor->unit_index, &facing, NULL);
        facing.k = 0.0f;
        if (normalize3d(&facing) == 0.0f)
        {
            facing = actor->facing_vector;
            facing.k = 0.0f;
            if (normalize3d(&facing) == 0.0f)
                facing = *global_forward3d;
        }
        if (TEST_BIT(actor->flags, 0))
        {
            character_movement_properties *movement_properties = character_definition_movement_properties_get(actor->character_index);
            if (movement_properties && !motor_animation_busy(actor->unit_index) && actor_moving_step_vector_path_executing(actor_index))
            {
                bool handled = false;
                if (step->frame_index == NONE && step_distance < 0.1f)
                {
                    movement_result = _movement_result_advance;
                    handled = true;
                }
                if (!handled && actor->destination_orders.type == 3 && !actor->destination_orders.ground_destination() &&
                    !actor->use_object && TEST_BIT(movement_properties->flags, 9))
                {
                    action_submit_event(actor->unit_index, 27);
                    handled = true;
                }
                if (!handled && step->frame_index == NONE)
                {
                    character_general_properties *general_properties = character_definition_general_properties_get(actor->character_index);
                    if (TEST_BIT(movement_properties->flags, 9) || general_properties && TEST_BIT(general_properties->flags, 1))
                        action_submit_event(actor->unit_index, 27);
                    else
                        actor_exit_posture(actor_index);
                    handled = true;
                }
                if (!handled && step->frame_index == 1 || step->frame_index == 6)
                {
                    bool jumping = false;
                    handled = true;
                    actor_find_pathfinding_location(actor_index);
                    *result = *global_zero_vector3d;
                    actor->flags |= FLAG(10);
                    actor->flags &= ~FLAG(9);
                    if (actor->pathfinding_sector == step->sector)
                    {
                        movement_result = _movement_result_advance;
                    }
                    else
                    {
                        bool blocked = hint && hint->jump_geometry_end != hint->jump_geometry_start;
                        char jump_height = hint ? hint->jump_height : 0;
                        jumping = actor_moving_jump(actor_index, &step->hint, hint != NULL, jump_height, &step->point, step->sector, !blocked);
                        if (jumping)
                        {
                            *result = *global_forward3d;
                        }
                        else if (blocked)
                        {
                            *result = *global_zero_vector3d;
                            actor->flags &= ~FLAG(0);
                        }
                        else
                        {
                            actor->leap_requested = true;
                            actor->leap_target_valid = true;
                            actor->leap_direction = movement_direction.xy;
                            actor->leap_speed = 1.5f;
                            actor->leap_height = 1.5f;
                            actor->leap_prop_index = NONE;
                            jumping = true;
                        }
                    }
                }
                if (handled)
                    actor->flags &= ~FLAG(0);
            }
        }
        if (movement_result != _movement_result_advance && TEST_BIT(actor->flags, 0))
        {
            if (movement_direction.k > 0.0f)
                actor_move_set_vector3d(&climb_vector, 0.0f, 0.0f, 1.0f);
            else if (movement_direction.k < 0.0f)
                actor_move_set_vector3d(&climb_vector, 0.0f, 0.0f, -1.0f);
        }
    }
    if (desired)
        *desired = facing;
    if (result)
        *result = climb_vector;
    return movement_result;
}

// @rdata 0x8273d02c
static const float k_lean_pivot_ratio = 1.3f;

// @address 0x518688
float actor_moving_vehicle_round_turn(long actor_index, long vehicle_index, short type, real_point2d *position, real_point2d *step,
    real_vector2d *next_step_direction, obstacles *obstacle_list)
{
    actor_datum *actor = datum_get<actor_datum>(actor_data, actor_index);
    object_datum *vehicle = (object_datum *)((*object_header_get(vehicle_index)).object);
    character_vehicle_properties *vehicle_properties = character_vehicle_properties_get(actor_index, vehicle->definition_index);
    float result = 0.0f;
    bool valid = false;
    float radius = vehicle_properties->turn_radius;
    real_vector2d direction;
    vector_from_points2d(position, step, &direction);
    normalize2d(&direction);
    real_vector2d perpendicular;
    perpendicular.i = -direction.j;
    perpendicular.j = direction.i;
    bool left = dot_product2d(&perpendicular, next_step_direction) > 0.0f;
    real_point2d center;
    // match: 0x518688: an early return for an unsupported type merges the restore paths and copies the turn angle into f22.
    for (;;)
    {
        if (type == 0)
        {
            real_vector2d next_perpendicular;
            next_perpendicular.i = -next_step_direction->j;
            next_perpendicular.j = next_step_direction->i;
            if (left)
                point_from_line2d(step, &next_perpendicular, radius, &center);
            else
                point_from_line2d(step, &next_perpendicular, -radius, &center);
        }
        else if (type == 1)
        {
            real_vector2d bisector;
            subtract_vectors2d(&direction, next_step_direction, &bisector);
            normalize2d(&bisector);
            point_from_line2d(step, &bisector, -radius, &center);
        }
        else
            break;

        real_point2d aim_point;
        if (left)
            point_from_line2d(&center, &perpendicular, -radius, &aim_point);
        else
            point_from_line2d(&center, &perpendicular, radius, &aim_point);
        float forward_projection = dot_product2d(&direction, position);
        float side_projection = dot_product2d(&perpendicular, position);
        float side_distance = dot_product2d(&perpendicular, &aim_point) - side_projection;
        float forward_distance = dot_product2d(&direction, &aim_point) - forward_projection;
        if (forward_distance > 0.11f * radius)
        {
            float scale = 1.0f;
            if (forward_distance > k_lean_pivot_ratio * radius)
                scale = 1.0f;
            else
                scale = (forward_distance - 0.11f * radius) / ((k_lean_pivot_ratio - 0.11f) * radius);
            point_from_line2d(&aim_point, &perpendicular, (1.5f * scale - 1.0f) * side_distance, &aim_point);
            valid = true;
        }
        if (!valid)
            break;
        real_vector2d aim_direction;
        vector_from_points2d(position, &aim_point, &aim_direction);
        float length = normalize2d(&aim_direction);
        real_vector3d local_direction = {aim_direction.i, aim_direction.j, 0.0f};
        real_point3d local_position = {position->x, position->y, actor->position.body.z};
        ai_reference_frame_inverse_transform_vector(actor->pathfinding_point.reference_frame(), &local_direction, &local_direction);
        ai_reference_frame_inverse_transform_point(actor->pathfinding_point.reference_frame(), &local_position, &local_position);
        pathfinding_test_pill2d_result collision;
        if (actor->pathfinding_sector.valid() &&
            pathfinding_test_pill2d(false, obstacle_list, -1, 2, true, &local_position, actor->pathfinding_sector, g_sector_none,
                &local_direction, vehicle_properties->pathfinding_radius, length, false, false, false, 0, &collision))
            break;
        float cosine = dot_product2d(&aim_direction, &direction);
        result = arccosine(cosine > 1.0f ? 1.0 : cosine);
        if (left)
            result = -result;
        break;
    }
    return result;
}
