// ailibxbox/actor_type_scarab.obj: from the records 0x5740a8 0x574110 0x574180 0x5742e0 0x574338 0x5743b0 0x5743c8 0x574a00 0x574ad0 0x574b18 0x574b50 0x574b80 0x574bc0 0x574c60 0x574d68 0x574e68 0x574e90 0x574f90 0x575030 0x5750e0
#include "tag_files/tag_groups.h"
#include "memory/data.h"
#include "objects/object_header_access.h"
#include "math/random_math_range.h"
#include "game/game_time_inline.h"
#include "ai/firing_position_ref_inline.h"
#include "ai/ai_math_constants.h"
#include "ai/pathfinding_collision.h"
#include "scenario/scenario.h"
#include "memory/tls_data_arrays.h"
#include "memory/tls_globals.h"
#include "memory/data_types.h"
#include "objects/object_reference_types.h"
#include "ai/firing_position_ref.h"
#include "cache/cache_files.h"
#include "objects/object_datum.h"
#include "math/real_math_types.h"
#include "math/real_math.h"
#include "game/game_time_types.h"
#include "ai/sector_ref.h"
#include "ai/ai_point3d.h"
#include "ai/prop_ref_datum.h"
#include "ai/behavior.h"
#include "tag_files/tag_types.h"
#include <ppcintrinsics.h>
#include <math.h>
#include <stdlib.h>
#include "cseries/crt_xdk.h"
#include "ai/actor_moving.h"
#include "ai/actor_type_scarab.h"
#include "ai/ai.h"
#include "ai/props.h"
#include "ai/prop_view_inline.h"
#include "ai/character_definitions.h"
#include "ai/pathfinding_utilities.h"
#include "ai/path_types.h"
#include "ai/path_obstacle_types.h"
#include "ai/sector.h"
#include "ai/prop_state.h"
#include "ai/actor_datum.h"
#include "math/real_math_constants.h"
#include "ai/prop_datum.h"
#include "ai/character_behavior_properties.h"
#include "ai/character_scarab_properties.h"
#include "ai/actor_firing_position.h"
#include "ai/pathfinding_pill_result.h"
#include "ai/path_state.h"
#include "ai/ai_reference_frame.h"
#include "ai/sector_geometry.h"
#include "ai/actor_firing_position_selection.h"
// @flags /TP /O2 /Oc
#include "ai/behavior_info.h"
// @address 0x5740a8 ?behavior_scarab_cure_isolation_get_desire@@YAFJPBUs_behavior_input@@PBUs_behavior_arguments@@@Z
// @address 0x5742e0 ?behavior_scarab_fight_get_desire@@YAFJPBUs_behavior_input@@PBUs_behavior_arguments@@@Z
// @address 0x5743b0 ?behavior_scarab_fight_activate@@YA_NJPAVc_behavior_state@@@Z
// @address 0x574ad0 ?behavior_scarab_combat_get_desire@@YAFJPBUs_behavior_input@@PBUs_behavior_arguments@@@Z
// @address 0x574b18 ?behavior_scarab_combat_next@@YAFJPAVc_behavior_state@@0@Z
// @address 0x574b50 ?behavior_scarab_combat_activate@@YA_NJPAVc_behavior_state@@@Z
// @address 0x574b80 ?behavior_scarab_combat_deactivate@@YAXJPAVc_behavior_state@@@Z
// @address 0x574bc0 ?behavior_scarab_search_get_desire@@YAFJPBUs_behavior_input@@PBUs_behavior_arguments@@@Z
// @address 0x574c60 ?behavior_scarab_search_next@@YAFJPAVc_behavior_state@@0@Z
// @address 0x574e68 ?behavior_scarab_search_deactivate@@YAXJPAVc_behavior_state@@@Z
// @address 0x574e90 ?behavior_scarab_search_pause_activate@@YA_NJPAVc_behavior_state@@@Z


// The scarab behaviors' data after the layer header.
class c_scarab_fight_data : public c_behavior_state {
public:
    unsigned char close_to_target;
    unsigned char done;
};
class c_scarab_search_data : public c_behavior_state {
public:
    short search_duration;
    long search_start;
    long search_target;
};
class c_scarab_search_pause_data : public c_behavior_state {
public:
    long pause_end_time;
    bool pause_flag;
};
struct s_behavior_input;
struct s_behavior_arguments;








// 16-byte aligned holder for a firing position value.
struct __declspec(align(16)) aligned_firing_position
{
    firing_position value;
    aligned_firing_position() {}
};

static __forceinline void actor_type_scarab_flag_clear(short *flags, int mask)
{
    *flags = *flags & ~mask;
}


short behavior_scarab_cure_isolation_get_desire(long actor_index, const s_behavior_input *, const s_behavior_arguments *)
{
    short result = 0;
    actor_datum *actor = datum_get<actor_datum>(actor_data, actor_index);
    if (actor->pathfinding_location_invalid != 0) {
        if (actor->dialogue_sector.valid())
            result = 3;
    }
    return result;
}

short behavior_scarab_cure_isolation_next(long actor_index, c_behavior_state *, c_behavior_state *)
{
    short result = -2;
    actor_datum *actor = datum_get<actor_datum>(actor_data, actor_index);
    actor_find_pathfinding_location(actor_index);
    if (actor->pathfinding_sector.valid())
        result = -1;
    return result;
}

// @address 0x574180 ?behavior_scarab_cure_isolation_perform@@YA_NJPAVc_behavior_state@@@Z
bool behavior_scarab_cure_isolation_perform(long actor_index, c_behavior_state *)
{
    actor_datum *actor = datum_get<actor_datum>(actor_data, actor_index);
    if (actor->unit_index != -1) {
        object_datum *object = object_get(actor->unit_index);
        real_vector3d direction;
        ai_vector_from_points3d(&object->bounding_center, &actor->dialogue_point, &direction);
#if defined(_XBOX) && !defined(__clang__)
        float zero = AI_REAL_CONSTANTS[-553];
#else
        float zero = 0.0f;
#endif
        direction.k = zero;
        float distance = normalize3d(&direction);
        if (distance > zero) {
#if defined(_XBOX) && !defined(__clang__)
            float extra_distance = AI_REAL_CONSTANTS[1350];
#else
            float extra_distance = 2.0f;
#endif
            character_vehicle_properties *properties = (character_vehicle_properties *)character_vehicle_properties_get(actor_index, object->definition_index);
            if (properties)
                extra_distance = properties->pathfinding_radius * AI_REAL_CONSTANTS[0];
            scale_vector3d(&direction, distance + extra_distance, &direction);
            actor->blind_panic_control_active = true;
            actor->control_vector = direction;
        }
    }
    long target_prop_index = actor->target_prop_index;
    if (target_prop_index != -1) {
        prop_ref_datum *prop_ref = datum_get<prop_ref_datum>(prop_ref_data, target_prop_index);
        if (prop_ref->status >= 1 && prop_ref->status <= 2) {
            actor->look_mode = 4;
            actor->look_direction.to_target();
        }
    }
    return true;
}

short behavior_scarab_fight_get_desire(long actor_index, const s_behavior_input *, const s_behavior_arguments *)
{
    actor_datum *actor = datum_get<actor_datum>(actor_data, actor_index);
    prop_ref_datum *first = datum_get<prop_ref_datum>(prop_ref_data, actor->target_prop_index);
    long high = ((unsigned long)(unsigned char)first->flags_1e << 24) & 0xf0000000;
    if (high < 0x10000000)
        return 0;
    return 3;
}

namespace {
static __forceinline short scarab_fight_desire(long actor_index)
{
    short result = 0;
    actor_datum *actor = datum_get<actor_datum>(actor_data, actor_index);
    prop_ref_datum *prop_ref = datum_get<prop_ref_datum>(prop_ref_data, actor->target_prop_index);
    long level = ((unsigned long)(unsigned char)prop_ref->flags_1e << 24) & 0xf0000000;
    if (level >= 0x10000000)
        result = 3;
    return result;
}
}
// @address 0x574338 ?behavior_scarab_fight_next@@YAFJPAVc_behavior_state@@0@Z
short behavior_scarab_fight_next(long actor_index, c_behavior_state *state, c_behavior_state *)
{
    c_scarab_fight_data *fight = (c_scarab_fight_data *)state;
    short result = -2;
    short desire = scarab_fight_desire(actor_index);
    if (desire == 0 || (signed char)fight->done != 0)
        result = -1;
    return result;
}

bool behavior_scarab_fight_activate(long actor_index, c_behavior_state *behavior_state)
{
    c_scarab_fight_data *fight = (c_scarab_fight_data *)behavior_state;
    fight->done = 0;
    fight->close_to_target = 0;
    return true;
}

short behavior_scarab_combat_get_desire(long actor_index, const s_behavior_input *, const s_behavior_arguments *)
{
    short result = 0;
    actor_datum *actor = datum_get<actor_datum>(actor_data, actor_index);
    if (actor->target_prop_index != -1) {
        bool locked = (actor->stimulus_flags & 0x40000) != 0;
        if (locked)
            result = 3;
    }
    return result;
}

short behavior_scarab_combat_next(long actor_index, c_behavior_state *, c_behavior_state *)
{
    short next = -2;
    if (datum_get<actor_datum>(actor_data, actor_index)->target_prop_index == -1)
        next = -1;
    return next;
}

bool behavior_scarab_combat_activate(long actor_index, c_behavior_state *)
{
    actor_datum *actor = datum_get<actor_datum>(actor_data, actor_index);
    actor->target_flags |= 4;
    return true;
}

void behavior_scarab_combat_deactivate(long actor_index, c_behavior_state *)
{
    actor_datum *actor = datum_get<actor_datum>(actor_data, actor_index);
    actor_type_scarab_flag_clear(&actor->target_flags, 8);
    actor_type_scarab_flag_clear(&actor->target_flags, 4);
}

short behavior_scarab_search_get_desire(long actor_index, const s_behavior_input *, const s_behavior_arguments *)
{
    short result = 0;
    long first_index = datum_get<actor_datum>(actor_data, actor_index)->target_prop_index;
    if (first_index != -1) {
        prop_ref_datum *first = datum_get<prop_ref_datum>(prop_ref_data, first_index);
        long second_index = first->view_index;
        if (second_index != -1) {
            tracking_datum *second = datum_get<tracking_datum>(TLS_PROP_DATA, second_index);
            if (second != 0) {
                short *flags = &second->tracking_view.flags;
                if (flags != 0) {
                    if (first->status >= 3) {
                        if ((*flags & 16) == 0) {
                            result = 3;
                        }
                    }
                }
            }
        }
    }
    return result;
}



namespace {

static __forceinline bool actor_type_scarab_path_finished(long actor_index)
{
    actor_datum *actor = datum_get<actor_datum>(actor_data, actor_index);
    return actor->path_state == 2;
}
static __forceinline void actor_type_scarab_path_result_reset(long actor_index, bool reset_time)
{
    actor_datum *actor = datum_get<actor_datum>(actor_data, actor_index);
    actor->path.valid = 0;
    actor->path.obstacle_object_index = -1;
    actor->path.result_value = 0;
    if (reset_time) {
        actor->path_result_time = 0;
        actor->path_result_count = 0;
    }
}
static __forceinline void actor_type_scarab_actor_path_clear(long actor_index)
{
    actor_datum *actor = datum_get<actor_datum>(actor_data, actor_index);
    actor_type_scarab_path_result_reset(actor_index, true);
    actor->path_state = 0;
    actor->destination_orders.clear();
}
}

// @address 0x574a00 ?scarab_target_lock_get_impulse@@YAFJPAVc_behavior_state@@@Z
short scarab_target_lock_get_impulse(long actor_index, c_behavior_state *)
{
    actor_datum *actor = datum_get<actor_datum>(actor_data, actor_index);
    prop_ref_datum *prop_ref = datum_get<prop_ref_datum>(prop_ref_data, actor->target_prop_index);
    short result;
    if (datum_get<prop_datum>(prop_data, prop_ref->prop_index)->flag != 0) {
        if (prop_ref->view_index != -1) {
            tracking_datum *prop = datum_get<tracking_datum>(TLS_PROP_DATA, prop_ref->view_index);
            if (prop != 0) {
                prop_view *view = &prop->tracking_view;
                result = -1;
                if (view != 0 && view->status >= 9)
                    actor->target_flags |= 8;
            } else {
                result = -1;
            }
        } else {
            result = -1;
        }
    } else {
        actor->target_flags &= ~8;
        result = -1;
    }
    return result;
}

// @address 0x574c60 ?behavior_scarab_search_next@@YAFJPAVc_behavior_state@@0@Z
short behavior_scarab_search_next(long actor_index, c_behavior_state *state, c_behavior_state *)
{
    short result = -1;
    actor_datum *actor = datum_get<actor_datum>(actor_data, actor_index);
    c_scarab_search_data *search = (c_scarab_search_data *)state;
    prop_ref_datum *prop_ref = datum_get<prop_ref_datum>(prop_ref_data, actor->target_prop_index);
    if (prop_ref->status >= 3) {
        prop_view *view = prop_view_get(prop_ref);
        if (view && !(view->flags & 16)) {
            if (search->search_start + search->search_duration < TLS_GAME_TIME_GLOBALS->time) {
                actor_prop_abandon_search(actor_index, actor->target_prop_index, 1);
                actor->target_flags &= ~8;
            } else {
                result = -2;
            }
        }
    }
    return result;
}

// @address 0x574d68 ?behavior_scarab_search_activate@@YA_NJPAVc_behavior_state@@@Z
bool behavior_scarab_search_activate(long actor_index, c_behavior_state *state)
{
    bool result = true;
    actor_datum *actor = datum_get<actor_datum>(actor_data, actor_index);
    c_scarab_search_data *search = (c_scarab_search_data *)state;
    character_search_properties *properties = character_definition_search_multi_properties_get(actor_index);
    float search_time = 40.0f;
    if (properties)
        search_time = _real_random_range(TLS_RANDOM_SEED, properties->search_time_lower, properties->search_time_upper);
    search->search_duration = (short)game_seconds_to_ticks_round(search_time);
    search->search_start = TLS_GAME_TIME_GLOBALS->time;
    search->search_target = -1;
    actor->searching = 1;
    return result;
}

void behavior_scarab_search_deactivate(long actor_index, c_behavior_state *)
{
    datum_get<actor_datum>(actor_data, actor_index)->searching = 0;
}

// @address 0x574e90 ?behavior_scarab_search_pause_activate@@YA_NJPAVc_behavior_state@@@Z
bool behavior_scarab_search_pause_activate(long actor_index, c_behavior_state *state)
{
    c_scarab_search_pause_data *pause = (c_scarab_search_pause_data *)state;
    actor_datum *actor = datum_get<actor_datum>(actor_data, actor_index);
    prop_state *prop = (prop_state *)prop_state_get_by_index(actor->target_prop_index);
    bool pause_flag = (actor->position.head.z - prop->position.z > 2.0f);
    game_time_globals_definition *time_globals = TLS_GAME_TIME_GLOBALS;
    pause->pause_flag = pause_flag;
    pause->pause_end_time = (long)((float)time_globals->ticks_per_second * 15.0f + 0.5f) + time_globals->time;
    actor_datum *path_actor = datum_get<actor_datum>(actor_data, actor_index);
    path_actor->path.valid = 0;
    path_actor->path.obstacle_object_index = -1;
    path_actor->path.result_value = 0;
    path_actor->path_result_time = 0;
    path_actor->path_result_count = 0;
    path_actor->path_state = 0;
    path_actor->destination_orders.clear();
    return true;
}

// @address 0x574f90 ?behavior_scarab_pause_next@@YAFJPAVc_behavior_state@@0@Z
short behavior_scarab_pause_next(long actor_index, c_behavior_state *state, c_behavior_state *)
{
    c_scarab_search_pause_data *pause = (c_scarab_search_pause_data *)state;
    short result = -2;
    actor_datum *actor = datum_get<actor_datum>(actor_data, actor_index);
    if (TLS_GAME_TIME_GLOBALS->time > pause->pause_end_time)
        result = -1;
    if (result == -1 && actor->target_prop_index != -1) {
        actor_prop_abandon_search(actor_index, actor->target_prop_index, 0);
        actor->target_flags &= ~8;
    }
    return result;
}

// @address 0x575030 ?behavior_scarab_search_pause_control@@YAXJPAVc_behavior_state@@@Z
void behavior_scarab_search_pause_control(long actor_index, c_behavior_state *state)
{
    actor_datum *actor = datum_get<actor_datum>(actor_data, actor_index);
    c_scarab_search_pause_data *pause = (c_scarab_search_pause_data *)state;
    prop_ref_datum *prop_ref = datum_get<prop_ref_datum>(tls_prop_ref_data(), actor->target_prop_index);
    prop_view *view = prop_view_get(prop_ref);
    float facing = dot_product2d(&view->direction.xy, &actor->facing_vector.xy);
    if (!(facing <= 0.8660254f))
        actor->crouch = pause->pause_flag;
    else {
        actor->look_mode = 4;
        actor->look_direction.to_target();
    }
}



// @rdata 0x8202251c
static const float k_scarab_pursuit_zero = 0.0f;
// @rdata 0x8202a1a4
static const float k_scarab_pursuit_distance = 4.0f;
static __forceinline float actor_type_scarab_pursuit_zero()
{
    return k_scarab_pursuit_zero;
}
static __forceinline real_point3d *actor_type_scarab_pursuit_point_from_line(
    const real_point3d *origin, const real_vector3d *direction, float distance, real_point3d *point)
{
    point->x = origin->x + direction->i * distance;
    point->y = origin->y + direction->j * distance;
    point->z = origin->z + direction->k * distance;
    return point;
}
static __forceinline void actor_type_scarab_pursuit_set_point(c_ai_point3d *destination, const real_point3d *point, short frame_index, short bsp_index);
static __forceinline void actor_type_scarab_pursuit_set_point(
    // match: 0x5750e0: __restrict keeps the three point loads ahead of the stores
    c_ai_point3d *__restrict destination, const real_point3d *point, short frame_index, short bsp_index)
{
    destination->point = *point;
    destination->location[0] = frame_index;
    destination->location[1] = bsp_index;
}
static __forceinline void actor_type_scarab_pursuit_calculate(
    long actor_index, actor_datum *actor, prop_ref_datum *prop, prop_view *view, real_vector3d direction, bool &result)
{
        direction.k = actor_type_scarab_pursuit_zero();
        if (normalize3d(&direction) != actor_type_scarab_pursuit_zero())
        {
            actor_find_pathfinding_location(actor_index);
            path_collision_result collision;
            pathfinding_test_vector(&actor->pathfinding_point.point, actor->pathfinding_sector, g_sector_none, &direction, prop->distance, 0, &collision);
            if (!collision.collision || collision.t > k_scarab_pursuit_distance)
            {
                real_point3d point;
                actor_type_scarab_pursuit_point_from_line(&collision.point, &direction, -k_scarab_pursuit_distance, &point);
                c_sector_ref sector = giant_sector_from_point(&point);
                if (sector.valid())
                {
                    s_cluster_reference cluster = scenario_cluster_reference_from_point(sector.structure_bsp_index, &point);
                    view->pursuit.pursuit_type = 1;
                    actor_type_scarab_pursuit_set_point(&view->pursuit.aim_point, &point, -1, sector.structure_bsp_index);
                    view->pursuit.sector = sector;
                    view->pursuit.cluster_reference = cluster;
                    result = true;
                }
            }
        }
}
// @address 0x5750e0 ?giant_find_pursuit_position@@YA_NJJ@Z
bool giant_find_pursuit_position(long actor_index, long prop_index)
{
    bool result = false;
    prop_ref_datum *prop = datum_get<prop_ref_datum>(prop_ref_data, prop_index);
    actor_datum *actor = datum_get<actor_datum>(actor_data, actor_index);
    prop_view *view = prop_view_get(prop);
    if (view && prop->distance > k_scarab_pursuit_distance)
        actor_type_scarab_pursuit_calculate(actor_index, actor, prop, view, view->direction, result);
    return result;
}


static __forceinline character_scarab_properties *character_definition_scarab_properties_get(long actor_index)
{
    character_scarab_properties *result = 0;
    long definition_index = datum_get<actor_datum>(actor_data, actor_index)->character_index;
    while (definition_index != -1) {
        character_definition *definition = (character_definition *)tag_get(0, definition_index);
        if (definition->scarab_properties.count > 0) {
            result = (character_scarab_properties *)definition->scarab_properties.address;
            break;
        }
        definition_index = definition->parent_character.index;
    }
    return result;
}

struct path_state;


// @address 0x5743c8 ?behavior_scarab_fight_perform@@YA_NJPAVc_behavior_state@@@Z
bool behavior_scarab_fight_perform(long actor_index, c_behavior_state *state)
{
    c_scarab_fight_data *fight = (c_scarab_fight_data *)state;
    actor_datum *actor = datum_get<actor_datum>(actor_data, actor_index);
    prop_ref_datum *prop_ref = datum_get<prop_ref_datum>(prop_ref_data, actor->target_prop_index);
    if (prop_ref->object_index == -1)
        return true;
    if (((1 << datum_get<object_header_datum>(object_header_data, prop_ref->object_index)->object_type) & 0x1003) == 0)
        return true;
    prop_state *prop = prop_state_get(prop_ref);
    bool select_firing_position = false;
    float distance = 0.0f;
    bool move_to_target = false;
    bool close_to_target = false;
    float min_distance = 14.0f;
    float max_distance = 30.0f;
    character_scarab_properties *scarab = character_definition_scarab_properties_get(actor_index);
    if (scarab) {
        min_distance = scarab->minimum_distance;
        max_distance = scarab->maximum_distance;
    }
    distance = prop_ref->distance;
    if (distance < min_distance && prop_ref->awareness >= 1) {
        if (fight->close_to_target != 0) {
            close_to_target = true;
        } else {
            actor_type_scarab_actor_path_clear(actor_index);
            if (distance > 0.01f) {
                actor_find_pathfinding_location(actor_index);
                if (actor->pathfinding_sector.valid()) {
                    object_datum *object = object_get(actor->unit_index);
                    character_vehicle_properties *properties = (character_vehicle_properties *)character_vehicle_properties_get(
                        actor_index, object->definition_index);
                    float radius = 8.0f;
                    if (properties)
                        radius = properties->pathfinding_radius;
                    real_vector3d direction;
                    vector_from_points3d(&prop->position, &actor->position.body, &direction);
                    direction.k = 0.0f;
                    if (normalize3d(&direction) == 0.0f)
                        direction = *global_forward3d;
                    real_point3d point;
                    ai_point_get_position(&actor->pathfinding_point, &point);
                    pathfinding_test_pill2d_result test_result;
                    if (!pathfinding_test_pill2d(false, 0, -1, 2, true, &point, actor->pathfinding_sector, g_sector_none, &direction,
                            radius, min_distance, false, false, false, 0, &test_result))
                    {
                        scale_vector3d(&direction, min_distance, &direction);
                        actor->blind_panic_control_active = true;
                        actor->control_vector = direction;
                    } else {
                        close_to_target = true;
                        fight->close_to_target = 1;
                    }
                }
            }
        }
    } else if (distance < max_distance && prop_ref->awareness >= 1) {
        fight->close_to_target = 0;
        actor_type_scarab_actor_path_clear(actor_index);
        select_firing_position = true;
    } else {
        fight->close_to_target = 0;
        move_to_target = true;
    }
    if (actor->uncovering != 0) {
        if (move_to_target) {
            prop_find_pathfinding_location(actor_index, actor->target_prop_index);
            if (prop->pathfinding_sector.valid()) {
                c_destination_orders orders;
                memset(&orders.action.direction, 0, sizeof(orders.action.direction));
                memset(&orders.action, 0, sizeof(orders.action));
                memset(&orders, 0, sizeof(orders));
                orders.to_prop(actor->target_prop_index);
                orders.f28 = 10.0f;
                actor_move(actor_index, &orders, 0);
            } else {
                close_to_target = true;
            }
        }
        if (close_to_target) {
            firing_position_evaluation_context context;
            memset(&context, 0, sizeof(context));
            context.type = 0;
            context.attack_range_minimum = min_distance;
            context.attack_range_maximum = max_distance;
            context.attack_range_distance = max_distance;
            c_path_state path;
            aligned_firing_position position;
            long previous_owner;
            bool found;
            firing_position_ref selected = actor_active_select_firing_position(
                actor_index, &context, &position.value, &previous_owner, (path_state *)path.m_data, &found);
            if (!fp_ref_equals(selected, FP_REF_NONE)) {
                actor_change_firing_position(actor_index, selected, &position.value, previous_owner, found ? (path_state *)path.m_data : 0, true);
            }
        }
        if (actor_type_scarab_path_finished(actor_index))
            select_firing_position = true;
    }
    actor->aim_mode = 2;
    actor->aim_direction.to_target();
    actor->look_mode = 4;
    actor->look_direction.to_target();
    if (distance >= min_distance) {
        actor->lower_weapon_blocked_b = 1;
        if (prop_ref->awareness < 1) {
            prop_view *view = prop_view_get(prop_ref);
            if (TLS_GAME_TIME_GLOBALS->time - view->last_visible_time < game_seconds_to_ticks_round(7.0f)) {
                real_point3d point;
                ai_point_get_position(&view->pathfinding_point, &point);
                actor->look_mode = 4;
                actor->look_direction.to_point(&point);
                actor->aim_mode = 2;
                actor->aim_direction.to_point(&point);
                actor->allow_unseen_target_fire = 1;
                actor->move_target_type = 2;
                actor->move_target_point = view->pathfinding_point;
                return true;
            }
            if (select_firing_position)
                fight->done = 1;
        }
    } else {
        if (select_firing_position && prop_ref->awareness < 1)
            fight->done = 1;
    }
    return true;
}

// @address 0x45fc28 ?behavior_scarab_root_get_desire@@YAFJPBUs_behavior_input@@PBUs_behavior_arguments@@@Z
short behavior_scarab_root_get_desire(long, const s_behavior_input *, const s_behavior_arguments *)
{
    return 3;
}

// @address 0x572e60 ?behavior_scarab_root_next@@YAFJPAVc_behavior_state@@0@Z
short behavior_scarab_root_next(long, c_behavior_state *, c_behavior_state *)
{
    return -2;
}

// @address 0x42d380 ?behavior_scarab_cure_isolation_activate@@YA_NJPAVc_behavior_state@@@Z
bool behavior_scarab_cure_isolation_activate(long, c_behavior_state *)
{
    return true;
}

// @address 0x45fc28 ?behavior_scarab_search_pause_get_desire@@YAFJPBUs_behavior_input@@PBUs_behavior_arguments@@@Z
short behavior_scarab_search_pause_get_desire(long, const s_behavior_input *, const s_behavior_arguments *)
{
    return 3;
}

// @address 0x42d380 ?behavior_scarab_search_pause_perform@@YA_NJPAVc_behavior_state@@@Z
bool behavior_scarab_search_pause_perform(long, c_behavior_state *)
{
    return true;
}

// @address 0x72cc28 ?behavior_scarab_root_children@@3PAUchild_entry@@A
child_entry behavior_scarab_root_children[5] = {
    { 4, 0, true, 0, -1, 0.0f },
    { 179, 0, true, 0, -1, 0.0f },
    { 180, 0, true, 0, -1, 0.0f },
    { 137, 0, true, 0, -1, 0.0f },
    { 5, 0, true, 0, -1, 0.0f },
};

// @address 0x7c5c40 ?scarab_root_behavior@@3VMutex_group@@A
Mutex_group scarab_root_behavior = Mutex_group(178, -1, behavior_scarab_root_get_desire,
    behavior_scarab_root_next, 0, 0, -1, behavior_decide_prioritized, 5, behavior_scarab_root_children);

// @address 0x7c5aa0 ?scarab_cure_isolation_behavior@@3VPrimitive_behavior@@A
Primitive_behavior scarab_cure_isolation_behavior = Primitive_behavior(179, -1,
    behavior_scarab_cure_isolation_get_desire, behavior_scarab_cure_isolation_next,
    behavior_scarab_cure_isolation_activate, 0, -1, behavior_scarab_cure_isolation_perform, 0);

// @address 0x72cb08 ?behavior_scarab_combat_children@@3PAUchild_entry@@A
child_entry behavior_scarab_combat_children[4] = {
    { 182, 0, true, 0, -1, 0.0f },
    { 181, 0, true, 0, -1, 0.0f },
    { 183, 0, true, 0, -1, 0.0f },
    { 5, 0, true, 0, -1, 0.0f },
};

// @address 0x7c5b50 ?scarab_combat_behavior@@3VMutex_group@@A
Mutex_group scarab_combat_behavior = Mutex_group(180, -1, behavior_scarab_combat_get_desire,
    behavior_scarab_combat_next, behavior_scarab_combat_activate, behavior_scarab_combat_deactivate, -1,
    behavior_decide_prioritized, 4, behavior_scarab_combat_children);

// @address 0x7c5af0 ?scarab_fight_behavior@@3VPrimitive_behavior@@A
Primitive_behavior scarab_fight_behavior = Primitive_behavior(181, 0x3ff007f0, behavior_scarab_fight_get_desire,
    behavior_scarab_fight_next, behavior_scarab_fight_activate, 0, -1, behavior_scarab_fight_perform, 0);

// @address 0x7c5b3c ?scarab_target_lock_impulse@@3VBehavior_impulse@@A
Behavior_impulse scarab_target_lock_impulse = Behavior_impulse(182, 0x3ff007ff, scarab_target_lock_get_impulse);

// @address 0x72cbb0 ?behavior_scarab_search_children@@3PAUchild_entry@@A
child_entry behavior_scarab_search_children[3] = {
    { 60, 0, true, 0, -1, 0.0f },
    { 58, 0, true, 0, -1, 0.0f },
    { 184, 0, true, 0, -1, 0.0f },
};

// @address 0x7c5ba0 ?scarab_search_behavior@@3VMutex_group@@A
Mutex_group scarab_search_behavior = Mutex_group(183, 0x3ff0047f, behavior_scarab_search_get_desire,
    behavior_scarab_search_next, behavior_scarab_search_activate, behavior_scarab_search_deactivate, -1,
    behavior_decide_sequential, 3, behavior_scarab_search_children);

// @address 0x7c5bf0 ?scarab_search_pause_behavior@@3VPrimitive_behavior@@A
Primitive_behavior scarab_search_pause_behavior = Primitive_behavior(184, 0x3ff0047f,
    behavior_scarab_search_pause_get_desire, behavior_scarab_pause_next, behavior_scarab_search_pause_activate,
    0, -1, behavior_scarab_search_pause_perform, behavior_scarab_search_pause_control);
