// ailibxbox/actor_stimulus.obj: from the records 0xaf858 0xb4f70 0xbf420 0x11fd90 0x513b38 0x513ca8 0x513dd8 0x513fc8 0x514600 0x514898 0x514a28 0x514bd8 0x514ed8 0x515070
// @compiler xdk6130
// @flags /TP /O2 /Oc
#include "memory/tls_globals.h"
#include "ai/behavior_stimulus_types.h"
#include "memory/data_types.h"
#include <string.h>
#include "game/game_globals_storage.h"
#include "math/real_math_types.h"
#include "tag_files/tag_types.h"
#include "game/game_time_types.h"
#include "game/player_types.h"
#include "units/unit_types.h"
#include "ai/squad_types.h"
#include "ai/squad_leader.h"
#include "objects/object_definitions.h"
#include "math/real_math_constants.h"
// @address 0x3aa90 k_real_math_origin3d
#include "ai/actor_datum.h"
#include "ai/character_properties_types.h"
#include "ai/ai_information_data.h"
#include "ai/ai_player_types.h"
#include <math.h>
#include <stdlib.h>
#include "ai/actors.h"
#include "motor/motor_animation.h"
#include "objects/objects.h"
#include "ai/actor_perception.h"
#include "ai/actor_stimulus.h"
#include "ai/ai_player.h"
#include "ai/prop_dialogue.h"
#include "ai/props.h"
#include "ai/character_definitions.h"
#include "ai/pathfinding_utilities.h"
#include "game/game_allegiance.h"
#include "ai/prop_datum.h"
#include "ai/prop_ref_datum.h"
#include "ai/prop_dialogue_event_types.h"
#include "ai/prop_stimulus_types.h"
#include "cache/cache_files.h"
#include "motor/action_system.h"
#include "math/real_math.h"
#include "ai/actor_prop_ref_iterator.h"
#include "cseries/cseries.h"
#include "game/player_access.h"
#include "ai/prop_view_inline.h"
#include "math/random_math_range.h"
#include "memory/data_inline.h"
#include "tag_files/tag_groups.h"
#include "game/game_time_access.h"
#include "game/game_time_inline.h"
#include "objects/object_header.h"
#include "objects/object_type_access.h"
// @address 0xaf858 ?actor_stimulus_allegiance_update@@YAXJJ@Z
// @address 0xb4f70 ?stimulus_register@@YA_NJW4e_behavior_stimulus@@FF@Z
// @address 0xbf420 ?actor_stimuli_update@@YAXJ@Z
// @address 0x513b38 ?actor_stimulus_surprise@@YAXJFJPBTreal_vector3d@@@Z
// @address 0x513ca8 ?actor_stimulus_prop_acknowledged@@YAXJJ_N@Z
// @address 0x513dd8 ?actor_stimulus_acknowledged_danger_zone@@YAXJJE@Z
// @address 0x513fc8 ?actor_stimulus_friend_body_sighted@@YAXJJ@Z
// @address 0xb50a0 ?actor_perception_danger_zone_interesting@@YA_NJJMF@Z
// @address 0x502e70 ?actor_initiate_combat@@YAXJ@Z
// @address 0xae678 ?normalize2d@@YAMPATreal_vector2d@@@Z
// @address 0x3559c8 ?motor_animation_busy@@YA_NJ@Z
// @address 0xaf9f0 ?prop_state_get@@YAPAUprop_state@@PBUprop_ref_datum@@@Z
// @address 0xc1188 ?prop_find_pathfinding_location@@YAXJJ@Z
// @address 0x514268 ?prop_orphan@@YAXJJ_N@Z
// @address 0x514600 ?actor_stimulus_tracking_or_locking@@YAXJJF@Z
// @address 0x9c9f8 ?game_team_is_enemy@@YA_NW4e_game_team@@0@Z
// @address 0xac748 ?object_try_and_get_and_verify_type@@YAPAXJK@Z
// @address 0xae150 ?character_definition_movement_properties_get@@YAPAUcharacter_movement_properties@@J@Z
// @address 0x564b68 ?prop_dialogue_event_internal@@YAXJJW4e_prop_dialogue_event@@FM@Z
// @address 0x7cff0c ?global_tag_instances@@3PBUcache_file_tag_instance@@B
// @address 0x514898 ?actor_stimulus_switched_danger_zone@@YAXJJ@Z
// @address 0x514a28 ?actor_stimulus_damage@@YAXJFJJMPBTreal_vector3d@@@Z
// @address 0x514bd8 ?actor_stimulus_heard_shooting@@YAXJJJ@Z
// @address 0x514ed8 ?actor_stimulus_prop_just_killed@@YAXJJJ@Z
// @address 0x515070 ?actor_stimulus_vehicle_eviction@@YAXJJ@Z




enum e_prop_stimulus;
const float actor_stimulus_half = 0.5f;
const float actor_stimulus_tracking_delay = 0.4f;
const float actor_stimulus_one = 1.0f;







extern "C" {
}
#define actor_data tls_actor_data()
#define player_data tls_player_data()
#define object_header_data tls_object_header_data()
#define game_time_globals_get() tls_game_time_globals()


extern "C" {
/* These calls do not retain the vector pointer. */


}
// @address 0x564e60 ?actor_dialogue_event@@YA_NJFJPAUai_information_data@@@Z
// @address 0x4fad58 ?ai_player_state_get@@YAPAUai_player_state@@J@Z
extern "C" {



}
#include "ai/character_perception.h"

#include "ai/prop_view.h"





struct actor_stimulus_projectile_definition {
    s_object_definition object;
    unsigned long flags;
};




struct actor_stimulus_tracking_datum {
    long header;
    prop_state state;
    prop_view view;
};

extern "C" {
}


static __forceinline prop_datum *actor_stimulus_prop_get(long prop_index)
{
    return datum_get<prop_datum>(prop_data, prop_index);
}

void actor_stimulus_allegiance_update(long actor_index, long prop_ref_index)
{
    actor_datum *actor = datum_get<actor_datum>(actor_data, actor_index);
    if (actor->unit_index != NONE) {
        prop_ref_datum *prop_ref = datum_get<prop_ref_datum>(prop_ref_data, prop_ref_index);
        prop_state *state = prop_state_get(prop_ref);
        prop_datum *prop = actor_stimulus_prop_get(prop_ref->prop_index);
        if (!state->orphaned && prop->allegiance_changed && (prop->flags & 0x40)) {
            prop_view *view = prop_view_get(prop_ref);
            bool dead = prop->dead;
            bool respond = prop_ref->distance < (dead ? 15.0f : (view && view->awareness <= 2 ? 10.0f : 3.0f));
            if (dead && prop_ref->stimulus == 3)
                respond = true;
            if (respond) {
                actor_dialogue_event(actor_index, 93, prop->object_index, 0);
                prop->flags &= ~0x40;
            }
        }
    }
}

static __forceinline void actor_stimulus_clear(actor_datum *actor, s_stimulus *stimulus)
{
    if (stimulus->type != NONE) {
        actor->stimulus_flags &= ~(1 << stimulus->type);
        stimulus->type = NONE;
    }
}

bool stimulus_register(long actor_index, e_behavior_stimulus stimulus, short time, short priority)
{
    actor_datum *actor = datum_get<actor_datum>(actor_data, actor_index);
    short lowest_priority = priority;
    short stimulus_index = NONE;
    bool success = false;
    for (short index = 0; index < 5; index++) {
        s_stimulus *slot = &actor->stimuli[index];
        if (slot->type == NONE) {
            stimulus_index = index;
            break;
        } else if (stimulus == slot->type) {
            if (time > slot->time) {
                stimulus_index = index;
                priority = priority > slot->priority ? priority : slot->priority;
            } else {
                stimulus_index = NONE;
            }
            break;
        } else if (slot->priority < lowest_priority) {
            lowest_priority = slot->priority;
            stimulus_index = index;
        }
    }
    if (stimulus_index != NONE) {
        s_stimulus *slot = &actor->stimuli[stimulus_index];
        actor_stimulus_clear(actor, slot);
        slot->type = stimulus;
        slot->priority = priority;
        slot->time = time;
        actor->stimulus_flags |= 1 << stimulus;
        success = true;
    }
    return success;
}

void actor_stimuli_update(long actor_index)
{
    actor_datum *actor = datum_get<actor_datum>(actor_data, actor_index);
    actor->stunned = 0;
    if (actor->danger_zone_tracking_last_time != NONE) {
        if (game_time_get() - actor->danger_zone_tracking_last_time > game_seconds_to_ticks_round(1.0f)) {
            actor->danger_zone_tracking_start_time = NONE;
            actor->danger_zone_tracking_last_time = NONE;
        }
    }
    for (short index = 0; index < 5; index++) {
        s_stimulus *stimulus = &actor->stimuli[index];
        if (stimulus->type != NONE) {
            stimulus->time--;
            if (stimulus->time <= 0) {
                actor->stimulus_flags &= ~(1 << stimulus->type);
                stimulus->type = NONE;
            }
        }
    }
}

void actor_stimulus_surprise(long actor_index, short surprise_level, long prop_index, const real_vector3d *vector)
{
    actor_datum *actor = datum_get<actor_datum>(actor_data, actor_index);
    if (surprise_level > actor->surprise_level) {
        actor->surprise_level = surprise_level;
        actor->surprise_prop_index = prop_index;
        if (surprise_level >= 1) {
            long object_index = NONE;
            if (prop_index != NONE) {
                prop_ref_datum *prop_ref = datum_get<prop_ref_datum>(prop_ref_data, prop_index);
                object_index = prop_ref->object_index;
            }
            stimulus_register(actor_index, _behavior_stimulus_surprise, 1, 3);
            actor_dialogue_event(actor_index, 19, object_index, 0);
            if (vector && actor->unit_index != NONE && !motor_animation_busy(actor->unit_index)) {
                real_vector2d direction = vector->xy;
                if (normalize2d(&direction) != 0.0f) {
                    float dot = dot_product2d(&actor->facing_vector.xy, &direction);
                    if (dot < -0.70710677f) {
                        real_vector3d reversed;
                        scale_vector3d(vector, -1.0f, &reversed);
                        ((bool)::actor_impulse_submit(actor_index, 46, false, &reversed));
                    } else {
                        ((bool)::actor_impulse_submit(actor_index, 45, false, vector));
                    }
                }
            }
        }
    }
}

void actor_stimulus_prop_acknowledged(long actor_index, long prop_ref_index, bool new_prop)
{
    actor_datum *actor = datum_get<actor_datum>(actor_data, actor_index);
    prop_ref_datum *prop_ref = datum_get<prop_ref_datum>(prop_ref_data, prop_ref_index);
    if (new_prop && prop_ref->type == 3)
        prop_orphan(actor_index, prop_ref_index, new_prop);
    else if (prop_ref->awareness > 0)
        actor_stimulus_acknowledged_danger_zone(actor_index, prop_ref_index, new_prop);
    if (prop_ref->type == 1 && new_prop && (prop_ref->flags & 1)) {
        s_mode_entry *perception = actor_perception_properties_get(actor_index);
        if (perception && (prop_ref->distance < perception->surprise_distance || actor->combat_status == 0)) {
            prop_state *state = prop_state_get(prop_ref);
            real_vector3d direction;
            vector_from_points3d(&actor->position.body, &state->position, &direction);
            actor_stimulus_surprise(actor_index, 1, prop_ref_index, &direction);
        }
    }
}

void actor_stimulus_acknowledged_danger_zone(long actor_index, long prop_ref_index, unsigned char acknowledged)
{
    prop_ref_datum *prop_ref = datum_get<prop_ref_datum>(prop_ref_data, prop_ref_index);
    actor_datum *actor = datum_get<actor_datum>(actor_data, actor_index);
    prop_view *view = prop_view_get(prop_ref);
    prop_datum *prop = actor_stimulus_prop_get(prop_ref->prop_index);
    if (view && view->acknowledge_time == NONE) {
        acknowledged = true;
        view->acknowledge_time = game_time_get();
    }
    if (actor->squad_index != NONE) {
        squad_datum *squad = datum_get<squad_datum>(tls_squad_data(), actor->squad_index);
        if (prop->type == 1)
            squad->flags |= 0x10;
        if (prop->enemy)
            squad->flags |= 8;
    }
    if (view)
        ::actor_perception_unassailable(actor_index, prop_ref_index, false, true);
    if (acknowledged && prop_ref->type == 7)
        actor_stimulus_friend_body_sighted(actor_index, prop_ref_index);
    short dialogue_index = NONE;
    if (view && prop_ref_index == actor->target_prop_index
        && (float)(game_time_get() - view->orphaned_time) * game_time_globals_get()->tick_length > 2.0f
        && ((view->flags & 0x10) || view->pursuit_type == 1))
        dialogue_index = 12;
    prop_dialogue_event_internal(actor_index, prop_ref_index, prop_dialogue_event_5, dialogue_index, 0.3f);
}


static __forceinline void actor_stimulus_force_combat_status(long actor_index, short combat_status)
{
    actor_datum *actor = datum_get<actor_datum>(actor_data, actor_index);
    actor->combat_status = combat_status;
    actor->combat_status_time = 0;
}

void actor_stimulus_friend_body_sighted(long actor_index, long prop_ref_index)
{
    actor_datum *actor = datum_get<actor_datum>(actor_data, actor_index);
    if (actor->combat_status < 3) {
        prop_ref_datum *body_ref = datum_get<prop_ref_datum>(prop_ref_data, prop_ref_index);
        object_datum *body = (object_datum *)object_get_and_verify_type(body_ref->object_index, NONE);
        if ((1 << body->object_identifier.m_source) & 3)
            return;
        long closest_prop_ref_index = NONE;
        bool success = false;
        float closest_distance = 3.402823466e38f;
        actor_prop_ref_iterator iterator;
        actor_prop_ref_iterator_new(&iterator, actor_index);
        prop_ref_datum *prop_ref;
        while ((prop_ref = actor_prop_ref_iterator_next(&iterator)) != 0) {
            if (prop_ref->type == 1) {
                prop_datum *prop = actor_stimulus_prop_get(prop_ref->prop_index);
                if (prop->enemy) {
                    closest_prop_ref_index = iterator.index;
                    break;
                } else if (prop_ref->distance < closest_distance) {
                    closest_prop_ref_index = iterator.index;
                }
            }
        }
        if (closest_prop_ref_index != NONE) {
            prop_ref = datum_get<prop_ref_datum>(prop_ref_data, closest_prop_ref_index);
            if (::prop_perceive(actor_index, prop_ref->object_index, 1, (e_prop_stimulus)NONE, 0, closest_prop_ref_index, NONE, NONE)) {
                prop_state *body_state = prop_state_get(body_ref);
                prop_state *state = prop_state_get(prop_ref);
                if (body_state && state) {
                    prop_find_pathfinding_location(actor_index, prop_ref_index);
                    state->target_position = body_state->target_position;
                    state->position = body_state->position;
                    state->location = body_state->location;
                    state->pathfinding_sector = body_state->pathfinding_sector;
                    state->pathfinding_point = body_state->pathfinding_point;
                    success = true;
                }
            }
        }
        stimulus_register(actor_index, _behavior_stimulus_friend_body, (short)game_seconds_to_ticks_round(2.0f), 3);
        if (!success)
            actor_stimulus_force_combat_status(actor_index, 3);
    }
}


extern "C" {
}

static __forceinline unsigned long actor_stimulus_campaign_flags()
{
    return tls_game_globals()->options.game_mode == 1 ? tls_game_globals()->options.campaign_skulls : 0;
}

static __forceinline actor_stimulus_projectile_definition *actor_stimulus_projectile_definition_get(long tag_index)
{
    return (actor_stimulus_projectile_definition *)tag_get('proj', tag_index);
}

static __forceinline long actor_stimulus_tracking_seconds_to_ticks_round(float seconds)
{
    return (long)((float)game_time_globals_get()->ticks_per_second * seconds + actor_stimulus_half);
}

void actor_stimulus_tracking_or_locking(long actor_index, long object_index, short flags)
{
    s_mode_entry *perception = actor_perception_properties_get(actor_index);
    bool allow_tracking = !perception || !((unsigned long)perception->flags & 4);
    if ((!perception || !((unsigned long)perception->flags & 2)) && object_index != NONE && (flags & 8)
        && ((1 << object_get_type(object_index)) & 32)) {
        object_datum *projectile = (object_datum *)object_get_and_verify_type(object_index, NONE);
        actor_stimulus_projectile_definition *definition = actor_stimulus_projectile_definition_get(projectile->definition_index);
        actor_datum *actor = datum_get<actor_datum>(actor_data, actor_index);
        real_vector3d displacement;
        vector_from_points3d(&actor->position.body, &projectile->position, &displacement);
        float distance = sqrtf(displacement.i * displacement.i +
            (displacement.j * displacement.j + displacement.k * displacement.k));
        bool interesting;
        if ((bool)(_rotl(definition->flags, 18) & 1)) {
            if (!allow_tracking)
                return;
            if (actor->danger_zone_tracking_start_time == NONE)
                actor->danger_zone_tracking_start_time = game_time_get();
            actor->danger_zone_tracking_last_time = game_time_get();
            interesting = game_time_get() - actor->danger_zone_tracking_start_time >
                actor_stimulus_tracking_seconds_to_ticks_round(actor_stimulus_tracking_delay);
        } else {
            interesting = true;
        }
        if (interesting && actor_perception_danger_zone_interesting(actor_index, object_index, distance, 2)) {
            long source_object_index = NONE;
            memset(&actor->danger_zone, 0, sizeof(actor->danger_zone));
            actor->danger_zone.danger_type = 2;
            actor->danger_zone.object_index = object_index;
            actor->danger_zone.radius = actor_stimulus_one;
            actor->danger_zone.state = 0;
            actor->danger_zone.pref_index = NONE;
            if (projectile->damage_owner.object_index != NONE) {
                unit_datum *source = (unit_datum *)object_try_and_get_and_verify_type(projectile->damage_owner.object_index, NONE);
                if (source && ((1 << source->object.type) & 0x1003)) {
                    source_object_index = projectile->damage_owner.object_index;
                    if (actor->unit_index != NONE && source_object_index == actor->unit_index)
                        actor->danger_zone.state = 2;
                    else if (!game_team_is_enemy((e_game_team)actor->team, source->unit.team))
                        actor->danger_zone.state = 1;
                }
            }
            actor->danger_zone.parent_index = source_object_index;
            actor_stimulus_switched_danger_zone(actor_index, object_index);
        }
    }
}

void actor_stimulus_switched_danger_zone(long actor_index, long object_index)
{
    actor_datum *actor = datum_get<actor_datum>(actor_data, actor_index);
    actor->danger_zone.active = false;
    if (actor->danger_zone.forced) {
        actor->danger_zone.evade = false;
        actor_dialogue_event(actor_index, 87, object_index, 0);
    } else {
        if (actor->danger_zone.state == 0 && actor->danger_zone.danger_type != 1 && !flag_test(actor_stimulus_campaign_flags(), 2L)) {
            character_movement_properties *movement = character_definition_movement_properties_get(actor->character_index);
            actor->danger_zone.evade = _real_random_range(tls_random_seed(), 0.0f, 1.0f) < movement->dive_evasion_chance;
        } else {
            actor->danger_zone.evade = true;
        }
        if (actor->danger_zone.danger_type != 2)
            actor->danger_zone.active = true;
        switch (actor->danger_zone.danger_type) {
        case 1:
            actor_dialogue_event(actor_index, 95, object_index, 0);
            break;
        case 3:
            if ((1 << object_get_type(object_index)) & 2)
                actor_dialogue_event(actor_index, 96, object_index, 0);
            break;
        }
    }
}

void actor_stimulus_damage(long actor_index, short, long flags, long prop_index, float, const real_vector3d *vector)
{
    actor_datum *actor = datum_get<actor_datum>(actor_data, actor_index);
    prop_ref_datum *prop_ref = 0;
    prop_view *view = 0;
    const real_vector3d *direction = 0;
    real_vector3d scaled;
    if (prop_index != NONE) {
        prop_ref = datum_get<prop_ref_datum>(prop_ref_data, prop_index);
        view = prop_view_get(prop_ref);
    }
    if (view) {
        direction = &view->direction;
    } else if (vector) {
        float magnitude_squared = vector->i * vector->i + vector->j * vector->j + vector->k * vector->k;
        if (magnitude_squared > 0.25f) {
            scale_vector3d(vector, -1.0f / sqrtf(magnitude_squared), &scaled);
            direction = &scaled;
        }
    }
    if (flags & 2)
        stimulus_register(actor_index, _behavior_stimulus_shield_depleted, 1, 3);
    if (!prop_ref || actor_stimulus_prop_get(prop_ref->prop_index)->dead) {
        if (actor->behavior_mode < 4)
            actor_stimulus_surprise(actor_index, 2, prop_index, direction);
    }
    if (prop_ref && actor_stimulus_prop_get(prop_ref->prop_index)->dead)
        actor_initiate_combat(actor_index);
}

void actor_stimulus_heard_shooting(long actor_index, long prop_ref_index, long shooter_actor_index)
{
    prop_ref_datum *prop_ref = 0;
    if (prop_ref_index != NONE)
        prop_ref = datum_get<prop_ref_datum>(prop_ref_data, prop_ref_index);
    actor_initiate_combat(actor_index);
    if (prop_ref && datum_get<prop_datum>(prop_data, prop_ref->prop_index)->dead) {
        ::prop_perceive(actor_index, prop_ref->object_index, 3, _prop_stimulus_shooting,
            (short)game_seconds_to_ticks_round(1.0f), prop_ref_index, NONE, NONE);
        prop_ref = datum_get<prop_ref_datum>(prop_ref_data, prop_ref_index);
        prop_state *state = prop_state_get(prop_ref);
        state->heard_shooting = true;
    } else {
        unit_datum *unit = 0;
        actor_datum *shooter = 0;
        if (shooter_actor_index != NONE)
            shooter = datum_get<actor_datum>(actor_data, shooter_actor_index);
        if (prop_ref)
            unit = (unit_datum *)object_get_and_verify_type(prop_ref->object_index, 0x1003);
        else if (shooter)
            unit = (unit_datum *)object_get_and_verify_type(shooter->unit_index, 0x1003);
        if (unit) {
            if (unit->unit.player_index != NONE) {
                player_datum *player = player_get(unit->unit.player_index);
                if (player->last_shot_at_object_index != NONE) {
                    if (player->last_shot_at_time + game_seconds_to_ticks_round(3.0f) >= game_time_get()
                        && object_try_and_get_and_verify_type(player->last_shot_at_object_index, NONE)
                        && ((1 << object_get_type(player->last_shot_at_object_index)) & 0x1003)) {
                        actor_datum *actor = datum_get<actor_datum>(actor_data, actor_index);
                        unit_datum *target_unit = (unit_datum *)object_get_and_verify_type(player->last_shot_at_object_index, 0x1003);
                        if (game_team_is_enemy((e_game_team)actor->team, (e_game_team)target_unit->unit.team))
                            ::prop_perceive(actor_index, player->last_shot_at_object_index, 1, (e_prop_stimulus)NONE, 0, NONE, NONE, NONE);
                    }
                }
            } else if (unit->unit.actor_index != NONE) {
                if (!shooter)
                    shooter = datum_get<actor_datum>(actor_data, unit->unit.actor_index);
                if (shooter->combat_status >= 6) {
                    long target_prop_index = shooter->target_prop_index;
                    if (target_prop_index != NONE) {
                        prop_ref_datum *target_prop_ref = datum_get<prop_ref_datum>(prop_ref_data, target_prop_index);
                        ::prop_perceive(actor_index, target_prop_ref->object_index, 1, (e_prop_stimulus)NONE, 0, NONE,
                            unit->unit.actor_index, target_prop_index);
                    }
                }
            }
        }
    }
}


void actor_stimulus_prop_just_killed(long actor_index, long prop_ref_index, long killed_actor_index)
{
    actor_datum *actor = datum_get<actor_datum>(actor_data, actor_index);
    actor_datum *killed = datum_get<actor_datum>(actor_data, killed_actor_index);
    if (!game_team_is_enemy((e_game_team)actor->team, (e_game_team)killed->team)) {
        if (prop_ref_index != NONE)
            ::prop_perceive(actor_index, datum_get<prop_ref_datum>(prop_ref_data, prop_ref_index)->object_index, 3,
                _prop_stimulus_shooting, (short)game_seconds_to_ticks_round(1.0f), NONE, NONE, NONE);
        if (distance_squared3d(&killed->position.body, &actor->position.body) < 64.0f) {
            if (actor->squad_index != NONE && squad_get_leader((short)actor->squad_index) == killed_actor_index)
                stimulus_register(actor_index, _behavior_stimulus_leader_killed, 1, 3);
            else if (killed->type == actor->type)
                stimulus_register(actor_index, _behavior_stimulus_peer_killed_nearby, 1, 3);
        }
        if (killed->type == actor->type)
            stimulus_register(actor_index, _behavior_stimulus_peer_killed, 1, 3);
    }
}

void actor_stimulus_vehicle_eviction(long actor_index, long player_index)
{
    actor_datum *actor = datum_get<actor_datum>(actor_data, actor_index);
    if (actor->root_vehicle_index != NONE) {
        player_datum *player = player_get(player_index);
        unit_datum *unit = (unit_datum *)object_get_and_verify_type(actor->unit_index, 8195);
        long unit_field_246 = unit->unit.parent_seat_index;
        long unit_field_10 = unit->object.parent_object_index;
        short dialogue_event;
        if (actor->driver != 0) {
            dialogue_event = 62;
        } else {
            dialogue_event = 63;
            if (!actor->controlling_other_unit)
                dialogue_event = 64;
        }
        actor_dialogue_event(actor_index, dialogue_event, player->unit_index, 0);
        if (action_submit_event(actor->unit_index, 13)) {
            game_time_globals_definition *time = game_time_globals_get();
            actor->seat_reentry_timer = (short)(time->ticks_per_second * 15);
            if (player_index != NONE) {
                ai_player_state *state = ai_player_state_get(player_index);
                if (state != 0) {
                    state->unit_index = unit_field_10;
                    state->squad_index = (short)unit_field_246;
                    long ticks = time->ticks_per_second;
                    state->flags = (unsigned short)(ticks + (ticks << 2));
                }
            }
        }
    }
}



static __forceinline prop_state *actor_stimulus_state_get(const prop_ref_datum *const &prop_ref)
{
    if (prop_ref->view_index != NONE) {
        return &datum_get<tracking_datum>(tls_prop_data(), prop_ref->view_index)->state;
    }
    return &actor_stimulus_prop_get(prop_ref->prop_index)->state;
}

static __forceinline void actor_stimulus_subtract_points(const real_point3d *const &a, const real_point3d *const &b, real_vector3d *const &result)
{
    result->i = a->x - b->x;
    result->j = a->y - b->y;
    result->k = a->z - b->z;
}


static __forceinline void actor_stimulus_record_awareness(prop_view *view, short awareness)
{
    view->sighting_search_state = view->search_state;
    view->sighting_awareness = awareness;
}

// @address 0x11fd90 ?actor_stimulus_prop_sighted@@YAXJJ_N@Z
void actor_stimulus_prop_sighted(long actor_index, long prop_ref_index, bool force)
{
    actor_datum *actor = datum_get<actor_datum>(actor_data, actor_index);
    prop_ref_datum *prop_ref = datum_get<prop_ref_datum>(prop_ref_data, prop_ref_index);
    prop_state *state = actor_stimulus_state_get(prop_ref);
    prop_view *view = prop_view_get(prop_ref);
    bool already_sighted = prop_ref->sighting_state >= 3;
    prop_ref->sighting_state = 3;
    if (view && (!already_sighted || force)) {
        view->sighting_count = 0;
        view->flags |= 0x80;
        view->sighting_position = state->position;
        actor_stimulus_record_awareness(view, prop_ref->awareness);
        ::prop_ref_reset_search(prop_ref_index, true);
        view->sighting_ticks = (short)(long)((float)game_time_globals_get()->ticks_per_second * 30.0f + 0.5f);
        view->orphaned_time = game_time_get();
        view->flags &= ~1;
        view->sighting_status = 0;
        if (view->tracking_index >= 0) {
            real_point3d origin;
            object_get_origin(prop_ref->object_index, &origin);
            actor_stimulus_subtract_points(&origin, &state->position, &view->sighting_direction);
            normalize3d(&view->sighting_direction);
        } else {
            union { const real_point3d *as_point; const real_vector3d *as_vector; } origin_alias;
            origin_alias.as_point = &k_real_math_origin3d;
            view->sighting_direction = *origin_alias.as_vector;
        }
    }
    if (!already_sighted) {
        if (prop_ref->type == 1)
            prop_dialogue_event_internal(actor_index, prop_ref_index, prop_dialogue_event_3,
                (short)-(prop_ref_index == actor->target_prop_index), 2.0f);
    }
    if (!already_sighted)
        actor_compute_prop_salience(actor_index, prop_ref_index, _prop_stimulus_none);
}
