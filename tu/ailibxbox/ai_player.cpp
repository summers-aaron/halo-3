// ailibxbox/ai_player.obj: from the records 0x4fac30 0x4facc0 0x4fad58 0x4fadb8 0x4fae50 0x4fb468 0x4fb688 0x4fb770 0x4fb810 0xa8c90
// @flags /TP /O2
#include "ai/ai_math_constants.h"
#include "ai/ai_information_data.h"
#include "ai/clump_importance_types.h"
#include "ai/ai_player_types.h"
#include "ai/ai_player_tls.h"
#include "objects/object_reference_types.h"
#include "objects/object_datum.h"
#include "game/game_time_types.h"
#include "game/game_types.h"
#include "game/game_globals_storage.h"
#include "units/vehicle_types.h"
#include "ai/clump_datum.h"
#include "ai/actor_datum.h"
#include "ai/clump_actor_cursor_types.h"
#include "ai/prop_datum.h"
#include "ai/clump_iterator.h"
#include <string.h>
#include "math/real_math.h"
#include "memory/data.h"
#include <math.h>
#include "objects/objects.h"
#include "sound/game_sound.h"
#include "units/units.h"
#include "ai/ai_player.h"
#include "ai/props.h"
#include "ai/prop_dialogue.h"
#include "ai/ai_vehicles.h"
#include "game/game_allegiance.h"
#include "units/vehicles.h"
#include "game/player_types.h"
#include "math/real_math_types.h"
#include "game/game_engine_default.h"
#include "memory/tls_globals.h"
#include "game/game_globals_inline.h"
#include "cache/restricted_memory_registration.h"
#include "cache/restricted_memory_regions_inline.h"
#include "cache/restricted_memory_regions.h"
#include "multithreading/thread_tls.h"
#include "objects/object_header.h"
#include "units/unit_types.h"
#include "units/vehicle_datum.h"
#include "ai/actor_prop_ref_iterator.h"
#include "tag_files/tag_groups.h"
#include "ai/character_definitions.h"
#include "ai/actor_globals.h"
#include "game/game_engine_scoring_globals.h"
#include "game/game_allegiance_types.h"
#include "ai/dialogue_event.h"
#include "ai/ai_dialogue.h"
#include "game/game_engine_get_inline.h"
#include "objects/object_header_access.h"
// @address 0x4fad58 ?ai_player_state_get@@YAPAUai_player_state@@J@Z
// @address 0x4facc0 ?ai_player_state_create@@YAXJ@Z
// @address 0x4fac20 ?__tls_set_g_game_sound_scripted_impulses_allocator@@YAXPAX@Z

// @address 0x11df784 g_ai_players
__declspec(thread) ai_player_state *g_ai_players;

static __forceinline long ai_player_get_prop_ref(long actor_index, long object_index)
{
    return prop_ref_get_by_object_index(actor_index, object_index, false, _clump_importance_none);
}

void ai_players_reset(void)
{
  ai_player_state *entries = TLS_AI_PLAYERS;
  short i = 0;
  do {
    ai_player_state *entry = &entries[i];
    memset(entry, 0, sizeof(*entry));
    entry->id = -1;
    entry->unit_index = -1;
    i = (short)(i + 1);
    entry->position = *global_zero_vector3d;
    entry->reset_a = 0xffff;
    entry->reset_b = 0xffff;
  } while (i < 4);
}

void ai_player_state_create(long id)
{
    if (game_is_campaign()) {
        bool created = false;
        for (long player_index = 0; player_index < 4; player_index++) {
            ai_player_state *state = &TLS_AI_PLAYERS[player_index];
            if (state->id == -1 && !created) {
                memset(state, 0, sizeof(ai_player_state));
                state->id = id;
                state->unit_index = -1;
                state->squad_index = -1;
                state->flags = 0;
                created = true;
            }
        }
    }
}

// Returns the thread's entry with this id, or null.
ai_player_state *ai_player_state_get(long id)
{
  ai_player_state *entry = 0;
  short index = -1;
  for (long player_index = 0; player_index < 4; player_index++) {
    if (TLS_AI_PLAYERS[player_index].id == id) {
      index = (short)player_index;
      break;
    }
  }
  if (index != -1)
    entry = &TLS_AI_PLAYERS[index];
  return entry;
}

// @address 0x4fadb8 ai_player_has_no_nearby_vehicle
bool ai_player_has_no_nearby_vehicle(short index)
{
  ai_player_state *state = &TLS_AI_PLAYERS[index];
  bool result = false;
  if (state->id != -1) {
    result = state->flag;
    if (TLS_GAME_TIME_GLOBALS->time - state->last_change_time <
#if defined(_XBOX) && !defined(__clang__)
        (long)((float)TLS_GAME_TIME_GLOBALS->ticks_per_second * AI_REAL_CONSTANTS[1321] + AI_REAL_CONSTANTS[0]))
#else
        (long)((float)TLS_GAME_TIME_GLOBALS->ticks_per_second * 5.0f + AI_REAL_CONSTANTS[0]))
#endif
      result = !result;
  }
  return result;
}

// @address 0x4fb770 ?reserve_memory@?$t_restricted_allocation_manager@$00$0A@$0A@$1?__tls_set_g_ai_players_allocator@@YAXPAX@Z@@QAAPAXPBD0IJ@Z
template<> void *ai_player_allocation_manager::reserve_memory(const char *name, const char *type, unsigned int size,
    long alignment_bits)
{
  int member_index = restricted_memory_register_member(&g_restricted_regions[1], name, type, size, 0,
      __tls_set_g_game_sound_scripted_impulses_allocator, 0, 0);
  unsigned int offset = restricted_region_get_member_offset(1, member_index);
  void *address = restricted_region_get_base_address(1) + offset;
  int owner = get_current_thread_index();
  m_thread_id = owner;
  m_member_index = member_index;
  TLS_AI_PLAYERS = (ai_player_state *)address;
  return (char *)restricted_region_get_member_address(1, member_index);
}

// @address 0x4fb810 ?allocate@?$t_restricted_allocation_manager@$00$0A@$0A@$1?__tls_set_g_ai_players_allocator@@YAXPAX@Z@@UAAPAXIPBD@Z
template<> void *ai_player_allocation_manager::allocate(unsigned int size, const char *name)
{
  return reserve_memory(name, "", size, 0);
}


static __forceinline bool ai_player_recent_change(const ai_player_state *state)
{
  return state->reset_b < (long)((float)TLS_GAME_TIME_GLOBALS->ticks_per_second * 0.1f + 0.5f);
}

// @address 0x4fb688 ai_player_try_and_get_combat_position
// match: 0x4fb688: pointer __restrict qualifier prevents alias reload
unsigned char ai_player_try_and_get_combat_position(long object_index, real_vector3d *__restrict position)
{
  bool result = false;
  unit_datum *unit = (unit_datum *)object_get(object_index);
  if (unit->unit.player_index != -1) {
    ai_player_state *state = ai_player_state_get(unit->unit.player_index);
    if (state != 0) {
      result = ai_player_recent_change(state);
      *position = unit->unit.aiming_vector;
    }
  }
  return result;
}


// @rdata 0x820242d8 k_vehicle_boarding_minimum_seconds
static const float k_vehicle_boarding_minimum_seconds = 2.0f;
// @rdata 0x8202a1a4 k_vehicle_boarding_maximum_decay_seconds
static const float k_vehicle_boarding_maximum_decay_seconds = 4.0f;

static __forceinline long ai_player_seconds_to_ticks_round(float seconds)
{
    return (long)(seconds * (float)TLS_GAME_TIME_GLOBALS->ticks_per_second + 0.5f);
}

static __forceinline short ai_player_state_get_index(long id)
{
    short index = -1;
    for (long player_index = 0; player_index < 4; player_index++) {
        if (TLS_AI_PLAYERS[player_index].id == id) {
            index = (short)player_index;
            break;
        }
    }
    return index;
}

// @address 0x4fae50 ai_player_update_vehicle_boarding
void ai_player_update_vehicle_boarding(long actor_index)
{
    actor_datum *actor = datum_get<actor_datum>(actor_data, actor_index);
    if (actor->unit_index != -1 && actor->attached_vehicle_index != -1 &&
        (!game_is_campaign() || actor->team == _game_team_player || !game_team_is_enemy((e_game_team)actor->team,
            _game_team_player)) &&
        ((1 << vehicle_get_type(actor->attached_vehicle_index)) & 0x10b) != 0)
    {
        c_data_iterator<player_datum> iterator;
        data_iterator_begin(&iterator.m_iterator, player_data);
        while (iterator.next())
        {
            player_datum *player = iterator.get_datum();
            if (!(player->flags & 1) || (player->flags & 2))
                continue;

            long player_index = iterator.get_index();
            ai_player_state *state = ai_player_state_get(player_index);
            short state_index = ai_player_state_get_index(player_index);
            float score = 0.0f;
            bool still_boarding = false;
            short minimum_ticks = (short)ai_player_seconds_to_ticks_round(k_vehicle_boarding_minimum_seconds);
            if (state && !game_team_is_enemy((e_game_team)actor->team,
                (e_game_team)player->configuration.host.team_index) && player->unit_index != -1 &&
                ((short &)actor->pickup_player == -1 || (short &)actor->pickup_player == state_index) &&
                ((unit_datum *)object_get(player->unit_index))->unit.parent_seat_index == -1)
            {
                float approach = 0.0f;
                float facing = 0.0f;
                real_vector3d forward;
                object_get_orientation(player->unit_index, &forward, NULL);
                real_point3d origin;
                object_get_origin(player->unit_index, &origin);
                real_vector3d direction;
                vector_from_points3d(&origin, &actor->position.body, &direction);
                float distance = normalize3d(&direction);
                bool boarding = (short &)actor->pickup_player == state_index && actor->pickup_wait_ticks > minimum_ticks;
                still_boarding = true;
                if (distance > 0.0f)
                {
                    bool visible = true;
                    long prop_ref_index = ai_player_get_prop_ref(actor_index, player->unit_index);
                    real_vector3d unit_velocity;
                    real_vector3d vehicle_velocity;
                    object_get_velocities(player->unit_index, &unit_velocity, NULL);
                    object_get_velocities(actor->attached_vehicle_index, &vehicle_velocity, NULL);
                    real_vector3d relative_velocity;
                    subtract_vectors3d(&unit_velocity, &vehicle_velocity, &relative_velocity);
                    if (normalize3d(&unit_velocity) > 0.0f)
                        approach = dot_product3d(&unit_velocity, &direction);
                    if (normalize3d(&relative_velocity) > 0.0f)
                    {
                        float relative_approach = dot_product3d(&relative_velocity, &direction);
                        if (relative_approach > approach)
                            approach = relative_approach;
                    }
                    if (prop_ref_index != -1 && datum_get<prop_ref_datum>(prop_ref_data, prop_ref_index)->awareness < 1)
                        visible = false;

                    if (distance < 15.0f)
                    {
                        if (visible)
                        {
                            facing = dot_product3d(&forward, &direction);
                            if (facing > 0.8660254f)
                                score = (facing - 0.8660254f) * 7.464101f;
                            else if (distance < 1.0f)
                                score = 1.0f;
                            if (!boarding && distance > 3.5f && approach < 0.5f)
                                score = 0.0f;
                        }
                        if (boarding)
                        {
                            if (distance < 2.5f)
                                score += 1.0f;
                            else if (distance <= 3.5f)
                                score = 1.0f - MAX(distance - 2.5f, 0.0f) + score;
                            if (approach > 0.5f)
                                score += 1.0f;
                        }
                    }

                    if (score > 0.0f)
                    {
                        if ((short &)actor->pickup_player == -1 || (short &)actor->pickup_player == state_index)
                        {
                            if ((short &)actor->pickup_player == -1)
                            {
                                actor->pickup_wait_ticks = 0;
                                actor->pickup_state = 0;
                            }
                            else if (actor->pickup_state > 0)
                            {
                                actor->pickup_state -= 2;
                            }
                            (short &)actor->pickup_player = state_index;
                        }
                    }
                    else if ((short &)actor->pickup_player == state_index)
                    {
                        actor->pickup_state++;
                        if (approach < 0.0f)
                            actor->pickup_state++;
                        if (actor->pickup_state > ai_player_seconds_to_ticks_round(k_vehicle_boarding_maximum_decay_seconds) ||
                            (actor->pickup_state > actor->pickup_wait_ticks && actor->pickup_wait_ticks < minimum_ticks))
                        {
                            still_boarding = false;
                        }
                    }

                    if ((short &)actor->pickup_player == state_index)
                    {
                        actor->pickup_wait_ticks++;
                        if (facing > 0.8660254f && distance < 2.5f)
                            actor->pickup_wait_ticks++;
                    }
                }
            }
            if (!still_boarding && (short &)actor->pickup_player == state_index)
            {
                (short &)actor->pickup_player = -1;
                actor->pickup_wait_ticks = 0;
                actor->pickup_state = 0;
            }
        }
    }
    else
    {
        actor->pickup_state = 0;
        actor->pickup_wait_ticks = 0;
        (short &)actor->pickup_player = -1;
    }
}

// match: 0x4fb468: retain direct TLS reads; accessor expansion adds a narrowing instruction.
// @address 0x4fb468 ai_player_update_boarding_dialogue
void ai_player_update_boarding_dialogue(long actor_index)
{
    actor_datum *actor = datum_get<actor_datum>(actor_data, actor_index);
    if (actor->boarding_dialogue_active != 0) {
        if ((actor->boarding_dialogue_player_index) >= 0 && (actor->boarding_dialogue_player_index) < 4 &&
            actor->combat_status < 8) {
            ai_player_state *state = &TLS_AI_PLAYERS[(actor->boarding_dialogue_player_index)];
            long triple = 3 * TLS_GAME_TIME_GLOBALS->ticks_per_second;
            unsigned short reset = state->reset_a;
            if (reset > triple) {
                long tag_index = actor->character_index;
                short wait_a = (short)triple;
                short wait_b = (short)(((TLS_GAME_TIME_GLOBALS->ticks_per_second) << 4) -
                    (TLS_GAME_TIME_GLOBALS->ticks_per_second));
                if (tag_index != -1) {
                    do {
                        character_definition *definition = (character_definition *)tag_get(0, tag_index);
                        if (definition->vocalization_properties.count > 0) {
                            float *timings = (float *)definition->vocalization_properties.address;
                            if (timings != 0) {
                                float rate_f = (float)TLS_GAME_TIME_GLOBALS->ticks_per_second;
                                wait_a = (short)(int)(timings[1] * rate_f + AI_REAL_CONSTANTS[0]);
                                wait_b = (short)(int)(timings[2] * rate_f + AI_REAL_CONSTANTS[0]);
                            }
                            break;
                        }
                        tag_index = definition->parent_character.index;
                    } while (tag_index != -1);
                }
                short count = ++actor->boarding_dialogue_ticks;
                if (state->id == -1)
                    return;
                if (count == wait_a) {
                    actor_dialogue_event(actor_index, 85,
                        datum_get<player_datum>(player_data,
                            TLS_AI_PLAYERS[actor->boarding_dialogue_player_index].id)->unit_index, 0);
                    return;
                }
                if (actor->boarding_dialogue_ticks != wait_b)
                    return;
                actor_dialogue_event(actor_index, 86,
                    datum_get<player_datum>(player_data,
                        TLS_AI_PLAYERS[actor->boarding_dialogue_player_index].id)->unit_index, 0);
                return;
            }
        }
    }
    actor->boarding_dialogue_ticks = 0;
}

struct s_dialogue_object;

static __forceinline void ai_player_clump_iterator_new(clump_actor_cursor *iterator)
{
    if (ai_globals->enabled)
        data_iterator_begin(&iterator->iterator, clump_data);
}
static __forceinline clump_datum *ai_player_clump_iterator_next(clump_actor_cursor *iterator)
{
    clump_datum *result = 0;
    if (ai_globals->enabled)
    {
        iterator->datum = (clump_datum *)data_iterator_next(&iterator->iterator);
        if (iterator->datum)
            result = (clump_datum *)iterator->datum;
        iterator->index = iterator->iterator.index;
    }
    return result;
}
static __forceinline void ai_player_prop_ref_iterator_new(actor_prop_ref_iterator *iterator, long prop_index)
{
    iterator->next_index = ((prop_datum *)prop_data->data)[(unsigned short)prop_index].first_prop_ref_index;
    iterator->index = -1;
}
static __forceinline prop_ref_datum *ai_player_prop_ref_iterator_next(actor_prop_ref_iterator *iterator)
{
    prop_ref_datum *result = 0;
    if (iterator->next_index != -1)
    {
        iterator->index = iterator->next_index;
        result = datum_get<prop_ref_datum>(prop_ref_data, iterator->next_index);
        iterator->next_index = result->prop_next_ref_index;
    }
    return result;
}

// match: 0xa8c90: shared game_engine_team_is_enemy stays out of line (bl) and resizes ai_player_update.
static __forceinline bool ai_player_game_engine_team_is_enemy(e_game_team team, e_game_team other_team)
{
    bool result = false;
    if (game_engine_get())
        result = game_engine_get()->team_is_enemy(team, other_team);
    return result;
}

static __forceinline bool ai_player_team_is_hostile(e_game_team team_a, e_game_team team_b)
{
    bool result = true;
    long mode = game_globals->options.game_mode;
    if (mode == 2)
        result = ai_player_game_engine_team_is_enemy(team_a, team_b);
    else if (team_a == team_b)
        result = false;
    else if (team_a != -1 && team_b != -1 && mode == 1 && (unsigned long)team_a <= 15 && (unsigned long)team_b <= 15)
    {
        long bit = (long)team_a * 16 + (long)team_b;
        result = (tls_game_allegiance_globals()->enemy_bits[bit >> 5] & (1 << (bit & 31))) == 0;
    }
    return result;
}
static __forceinline bool ai_player_vehicle_type_supported(long vehicle_index)
{
    return ((1 << vehicle_get_type(vehicle_index)) & 0x10b) != 0;
}

// @rdata 0x8202490c ai_player_epsilon
static const float ai_player_epsilon = 0.0001f;
// @rdata 0x82024440 ai_player_follow_distance
static const float ai_player_follow_distance = 9.0f;
// @rdata 0x82022850 ai_player_search_radius
static const float ai_player_search_radius = 10.0f;
// @rdata 0x820226b4 ai_player_slow_weight
static const float ai_player_slow_weight = 0.005f;
// @rdata 0x82022dbc ai_player_fast_weight
static const float ai_player_fast_weight = 0.1f;

// match: 0xa8c90: direct TLS reads keep the load schedule.
// @address 0xa8c90 ai_player_update
extern "C" void ai_player_update()
{
    for (short player_number = 0; player_number < 4; ++player_number)
    {
        ai_player_state *state = &TLS_AI_PLAYERS[player_number];
        if (state->id == -1)
            continue;
        player_datum *player = datum_get<player_datum>(player_data, state->id);
        if (player->unit_index == -1)
            continue;
        unit_datum *unit = (unit_datum *)((s_object_header *)object_header_data->data)[(unsigned short)player->unit_index].datum;
        if (state->reset_a < 0xffff)
            ++state->reset_a;
        bool seen_enemy = false;
        real_vector3d forward = ((unit_datum *)((s_object_header *)object_header_data->data)[(unsigned short)player->unit_index].datum)->unit.aiming_vector;
        real_point3d camera;
        unit_get_camera_position(player->unit_index, &camera);
        clump_actor_cursor clumps;
        ai_player_clump_iterator_new(&clumps);
        clump_datum *clump;
        while ((clump = ai_player_clump_iterator_next(&clumps)) != 0)
        {
            if (!clump->active || !ai_player_team_is_hostile((e_game_team)clump->team, _game_team_player))
                continue;
            clump_prop_iterator props;
            clump_prop_iterator_new(&props, datum_get<clump_datum>(clump_data, clumps.index));
            prop_datum *clump_prop;
            while ((clump_prop = clump_prop_iterator_next(&props)) != 0)
            {
                if (clump_prop->object_index != player->unit_index)
                    continue;
                actor_prop_ref_iterator refs;
                ai_player_prop_ref_iterator_new(&refs, props.index);
                prop_ref_datum *prop;
                while ((prop = ai_player_prop_ref_iterator_next(&refs)) != 0)
                {
                    if ((long)(prop->flags_1e << 28) > 0x10000000)
                        continue;
                    actor_datum *actor = &((actor_datum *)actor_data->data)[(unsigned short)prop->actor_index];
                    if (actor->unit_index == -1)
                        continue;
                    real_vector3d direction;
                    vector_from_points3d(&camera, &actor->position.body, &direction);
                    float distance = sqrtf(magnitude_squared3d(&direction));
                    if (fabsf(distance) < ai_player_epsilon)
                        continue;
                    scale_vector3d(&direction, 1.0f / distance, &direction);
                    if (distance > 0.0f && dot_product3d(&forward, &direction) > 0.866f)
                    {
                        seen_enemy = true;
                        break;
                    }
                }
                if (seen_enemy)
                    break;
            }
            if (seen_enemy)
                break;
        }
        if (seen_enemy)
            state->reset_b = 0;
        else
            ++state->reset_b;
        if ((short)state->flags > 0 && state->unit_index != -1)
        {
            unit_datum *target = (unit_datum *)((s_object_header *)object_header_data->data)[(unsigned short)state->unit_index].datum;
            const real_point3d *center = &unit->object.bounding_center;
            const real_point3d *target_center = &target->object.bounding_center;
            float i = target_center->x - center->x;
            float j = target_center->y - center->y;
            float k = target_center->z - center->z;
            float distance = sqrtf(k * k + (j * j + i * i));
            if (!(distance - target->object.bounding_radius < ai_player_follow_distance))
            {
                --state->flags;
                if ((short)state->flags <= 0)
                {
                    state->unit_index = -1;
                    state->squad_index = -1;
                    state->flags = 0;
                }
            }
        }
        else
            state->flags = 0;
        if (unit->unit.parent_seat_index != -1 && object_header_get(unit->object.parent_object_index)->object_type == 1)
        {
            vehicle_datum *vehicle = (vehicle_datum *)((s_object_header *)object_header_data->data)[(unsigned short)unit->object.parent_object_index].datum;
            real_vector3d vehicle_forward, up;
            object_get_orientation(unit->object.parent_object_index, &vehicle_forward, &up);
            if (vehicle->unit.driver_object_index == player->unit_index &&
                ai_player_vehicle_type_supported(unit->object.parent_object_index))
            {
                if (vehicle->vehicle.braking_ticks &&
                    state->vehicle_counter > (long)((float)TLS_GAME_TIME_GLOBALS->ticks_per_second * 0.75f + 0.5f) && up.k > 0.5f)
                {
                    c_dialogue_event event(46, player->unit_index, -1, -1, -1, 0);
                    ai_dialogue_event(&event);
                }
                else if (up.k < 0.0f && !state->airborne_dialogue)
                {
                    c_dialogue_event event(48, player->unit_index, -1, -1, -1, 0);
                    if (ai_dialogue_event(&event))
                        state->airborne_dialogue = true;
                }
            }
            state->vehicle_counter = (signed char)vehicle->vehicle.airborne_ticks;
        }
        else
        {
            state->vehicle_counter = 0;
            state->airborne_dialogue = false;
        }
        bool no_nearby_vehicle = false;
        if (unit->object.parent_object_index == -1)
        {
            bool found_vehicle = false;
            s_location location;
            real_point3d origin;
            long indices[512];
            object_get_location(player->unit_index, &location);
            object_get_origin(player->unit_index, &origin);
            short count = objects_in_sphere(0, 2, &location, &origin, ai_player_search_radius, indices, 512);
            for (short index = 0; index < count; ++index)
            {
                long vehicle_index = indices[index];
                bool blocked = ((((unit_datum *)((s_object_header *)object_header_data->data)[(unsigned short)vehicle_index].datum)
                    ->object.damage_flags >> 2) & 1) != 0;
                if (!blocked && ai_player_vehicle_type_supported(vehicle_index) &&
                    !ai_vehicle_occupied_by_enemy(vehicle_index, (e_game_team)player->configuration.host.team_index))
                {
                    found_vehicle = true;
                    break;
                }
            }
            no_nearby_vehicle = !found_vehicle;
        }
        if ((unsigned char)(state->flag ^ no_nearby_vehicle))
            state->last_change_time = TLS_GAME_TIME_GLOBALS->time;
        state->flag = no_nearby_vehicle;
        if (unit->object.parent_object_index == -1)
        {
            float weight = ai_player_slow_weight;
            if (!(unit->unit.primary_trigger < 1.0f))
                weight = ai_player_fast_weight;
            unit_datum *player_unit = (unit_datum *)((s_object_header *)object_header_data->data)[(unsigned short)player->unit_index].datum;
            real_vector3d current_forward = player_unit->unit.aiming_vector;
            scale_vector3d(&current_forward, weight, &current_forward);
            float previous_weight = 1.0f - weight;
            scale_vector3d(&state->position, previous_weight, &state->position);
            add_vectors3d(&state->position, &current_forward, &state->position);
        }
        else
            state->position = *global_zero_vector3d;
    }
}
