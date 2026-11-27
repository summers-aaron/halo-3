// blamlibxbox/game_engine_spawn_influencer.obj: from the records 0x20a038 0x27ba08 0x27bb78 0x27bc80 0x27bd90 0x27bfa8 0x27c0e0 0x27c5c8 0x27c840 0x27c958 0x27caf8 0x27cc68 0x27cd78 0x27cea8 0x27cf90 0x27d110 0x27d218 0x27d458 0x27d608 0x27d8a8 0x27d9e8 0x27db20 0x27dc58 0x27dd60 0x27ddc0 0x27dec0 0x27e018 0x27e148 0x27e248
// @flags /TP /O2 /Oc
#include "memory/tls_globals.h"
#include "game/game_time_inline.h"
#include "units/unit_seat.h"
#include "objects/object_header.h"
#include "tag_files/tag_groups.h"
#include "math/matrix_math_types.h"
#include "math/real_math.h"
#include "math/random_math_range.h"
#include "memory/data_types.h"
#include "tag_files/tag_types.h"
#include "game/game_time_types.h"
#include "game/player_types.h"
#include "game/game_types.h"
#include "game/player_traits.h"
#include "game/game_globals.h"
#include "game/game_globals_storage.h"
#include "objects/object_iterator.h"
#include <math.h>
#include <string.h>
#include "cseries/cseries.h"
#include "game/game_engine.h"
#include "objects/objects.h"
#include "game/game_engine_spawn_influencer.h"
#include "game/game_engine_team.h"
#include "game/game_engine_util.h"
#include "game/game_engine_current_entry.h"
#include "items/weapons.h"
#include "math/collisions_simple.h"
#include "game/game_engine_vip.h"
#include "memory/data.h"
#include "memory/datum_storage_access.h"
#include "objects/object_datum.h"
#include "units/unit_types.h"
#include "game/game_engine_settings_definitions.h"
#include "game/multiplayer_runtime_globals.h"
#include "units/unit_definitions.h"
#include "objects/object_multiplayer_access.h"
#include "objects/multiplayer_object_types.h"
#include "game/game_engine_default.h"
#include "game/player_in_game_iterator.h"
#include "scenario/scenario_tag.h"
#include "game/dynamic_spawn_influencer_collection_types.h"
#include "game/dynamic_spawn_influencer_types.h"
#include "items/item_types.h"
#include "items/multiplayer_weapon_tracker.h"
#include "math/real_math_types.h"
#include "game/game_engine_list.h"
#include "scenario/scenario.h"

#include "game/game_engine_scoring_globals.h"
#include "game/multiplayer_constants.h"
#include "animations/animation_definition_storage.h"
#include "game/game_engine_ctf.h"

template<typename t_object_type>
class c_multiplayer_object_iterator : public c_object_iterator_base
{
public:
    void begin(unsigned long type_mask, unsigned char header_mask, unsigned long match_flags)
    {
        object_iterator_begin_internal(type_mask, header_mask, match_flags);
    }
    bool next() { return object_iterator_next_with_match_flags_internal(); }
};

enum e_dynamic_spawn_influencer_type
{
    _dynamic_spawn_influencer_type_enemy_forbid = 0,
    _dynamic_spawn_influencer_type_enemy_bias,
    _dynamic_spawn_influencer_type_ally_bias,
    _dynamic_spawn_influencer_type_ally_bias_special,
    _dynamic_spawn_influencer_type_dead_teammate,
    _dynamic_spawn_influencer_type_respawn_zone,
    _dynamic_spawn_influencer_type_weapon,
    _dynamic_spawn_influencer_type_vehicle,
    _dynamic_spawn_influencer_type_projectile,
    _dynamic_spawn_influencer_type_equipment,
    _dynamic_spawn_influencer_type_netgame_goal_0,
    _dynamic_spawn_influencer_type_netgame_goal_1,
    _dynamic_spawn_influencer_type_netgame_goal_2,
    _dynamic_spawn_influencer_type_netgame_goal_3,
    _dynamic_spawn_influencer_type_netgame_goal_4,
    _dynamic_spawn_influencer_type_netgame_goal_5,
    _dynamic_spawn_influencer_type_netgame_goal_6,
    _dynamic_spawn_influencer_type_netgame_goal_7,
};

static __forceinline void spawn_influencer_copy_vector3d(const real_vector3d *source, real_vector3d *destination) { *destination = *source; }

static __forceinline void *game_engine_spawn_influencer_current_engine_inline()
{
    if (game_engine_globals->game_engine_index >= 1 && game_engine_globals->game_engine_index <= 10)
        return game_engines[game_engine_globals->game_engine_index];
    return 0;
}

static __forceinline c_game_variant *game_engine_spawn_influencer_multiplayer_get()
{
    game_options *options = &game_globals->options;
    if (options->game_mode == 2) return &options->game_variant;
    return 0;
}

// @address 0x20a038 game_engine_spawn_influencers_add_ally_bias
extern "C" float game_engine_spawn_influencers_add_ally_bias(long a, long b)
{
    float result = 1.0f;
    if (current_game_engine_entry() && a != b)
    {
        c_game_variant *mp = game_engine_spawn_influencer_multiplayer_get();
        if (a != -1)
        {
            player_datum *player = (player_datum *)datum_try_and_get(tls_player_data(), a);
            if (player)
            {
                if ((mp->social_options_flags & 1) != 0 ||
                    b == -1 ||
                    ::game_engine_team_is_enemy((e_game_team)player->configuration.host.team,
                    (e_game_team)datum_get((player_datum *)tls_player_data()->data, b)->configuration.host.team))
                {
                    result = player->traits.get_weapons_traits()->get_damage_modifier_percentage();
                }
                else
                {
                    result = 0.0f;
                }
            }
        }
    }
    return result;
}



// 0x27ba08
void game_engine_spawn_influencers_update(void)
{
    const s_multiplayer_constants *constants = game_engine_multiplayer_constants_try_and_get();
    c_multiplayer_object_iterator<object_datum> iterator;
    iterator.begin(64, 0, 0);
    while (iterator.next())
    {
        s_data_array *object_data = tls_object_header_data();
        long try_object_index = iterator.get_index();
        void *verified_object = 0;
        if (try_object_index != -1)
        {
            long absolute_index = try_object_index & 0xffff;
            if (absolute_index < object_data->first_unallocated)
            {
                s_object_header *header = (s_object_header *)((char *)object_data->data + object_data->size * absolute_index);
                long identifier = header->identifier;
                if (identifier != 0 && identifier == (try_object_index >> 16) && ((1 << header->object_type) & ~0UL) != 0)
                    verified_object = header->datum;
            }
        }
        unit_datum *object = (unit_datum *)verified_object;
        s_multiplayer_object_properties *multiplayer = 0;
        if (object && object->object.multiplayer_data_offset != -1)
            multiplayer = (s_multiplayer_object_properties *)((char *)object + object->object.multiplayer_data_offset);
        if (multiplayer)
        {
            multiplayer->spawn_weight_adjustment = _real_random_range(
                TLS_RANDOM_SEED, 0.0f, constants ? constants->maximum_random_spawn_bias : 10.0f);
        }
    }
}

static __forceinline bool game_engine_spawn_influencer_game_options_valid()
{
    return game_globals && (game_globals->initializing || game_globals->map_active);
}

// @address 0x27bb78 ?game_engine_spawn_influencers_build_collection_for_player@@YAXJPAUs_dynamic_spawn_influencer_collection@@@Z
void game_engine_spawn_influencers_build_collection_for_player(long player_index, s_dynamic_spawn_influencer_collection *collection)
{
    player_datum *player = (player_datum *)(tls_player_data()->data + (player_index & 0xffff) * sizeof(player_datum));
    collection->player_index = (short)player_index;
    collection->count = 0;
    game_engine_spawn_influencers_add_user_placed_influence(player_index, collection);
    if (game_engine_spawn_influencer_current_engine_inline())
        game_engine_spawn_influence_add_netgame_goal_object_influence(player_index, collection);
    game_engine_spawn_influencers_add_enemy_forbid_influence(player_index, player, collection);
    game_engine_spawn_influencers_add_enemy_bias_influence(player_index, player, collection);
    game_engine_spawn_influencers_add_teammate_influence(player_index, player, collection);
    game_engine_spawn_influencers_add_dead_teammate_influence(player_index, player, collection);
    game_engine_spawn_influencers_add_enemy_weapon_influence(player_index, player, collection);
    game_engine_spawn_influencers_add_vehicle_influence(player, collection);
    game_engine_spawn_influence_add_projectile_influence(player, collection);
    game_engine_spawn_influence_add_equipment_influence(player, collection);
}


static __forceinline void game_engine_spawn_influencer_distance_weight(const s_dynamic_spawn_influencer *influencer, float distance, float *weight)
{
    if (distance > influencer->inner_radius)
    {
        float range = influencer->outer_radius - influencer->inner_radius;
        if (range > 0.0001f)
            *weight = (influencer->outer_radius - distance) / range;
        else
            *weight = 1.0f;
    }
    else
        *weight = 1.0f;
}

// @address 0x27bd90 game_engine_spawn_influencer_test_point
// match: 0x27bd90: pointer __restrict qualifier prevents alias reload
extern "C" bool game_engine_spawn_influencer_test_point(const real_point3d *point, const s_dynamic_spawn_influencer *influencer, float *__restrict weight)
{
    bool result;
    *weight = 0.0f;
    switch (influencer->shape)
    {
    case 0:
        {
            float distance;
            result = ::collision_point_in_cylinder(point, &influencer->transform,
                influencer->inner_radius > influencer->outer_radius ? influencer->inner_radius : influencer->outer_radius,
                influencer->height, (float *)(&distance));
            if (result)
                game_engine_spawn_influencer_distance_weight(influencer, distance, weight);
        }
        break;
    case 1:
        result = ::collision_point_in_pill(point, &influencer->transform,
            influencer->inner_radius, influencer->outer_radius, (float *)(0));
        if (result)
            *weight = 1.0f;
        break;
    case 2:
        {
            real_vector3d axis;
            float distance;
            matrix4x3_transform_vector(&influencer->transform, &k_real_math_identity_matrix_storage.forward, &axis);
            fast_normalize3d(&axis);
            result = ::collision_point_in_cone(point, &influencer->transform,
                &axis,
                influencer->inner_radius > influencer->outer_radius ? influencer->inner_radius : influencer->outer_radius,
                influencer->height, (float *)(&distance), (float *)(0));
            if (result)
                game_engine_spawn_influencer_distance_weight(influencer, distance, weight);
        }
        break;
    case 3:
        result = ::collision_point_in_box(point, &influencer->transform,
            influencer->inner_radius, influencer->outer_radius, influencer->height);
        if (result)
            *weight = 1.0f;
        break;
    case 4:
        result = ::collision_point_in_sphere(
            point, &influencer->transform.position, influencer->inner_radius);
        if (result)
            *weight = 1.0f;
        break;
    default:
        // match: 0x27bd90: unreachable hint omits dead branch check
        __assume(0);
    }
    *weight = (*weight > 0.0f ? *weight : 0.0f) < 1.0f ? (*weight > 0.0f ? *weight : 0.0f) : 1.0f;
    return result;
}

const float k_real_max = 3.4028234663852886e+38f;
const float k_real_min = -3.4028234663852886e+38f;
// @address 0x27bfa8 game_engine_spawn_influencer_compute_score
extern "C" float game_engine_spawn_influencer_compute_score(const real_point3d *point, bool enabled, bool unbounded, float initial,
    float adjustment, const s_dynamic_spawn_influencer_collection *collection)
{
    float total = initial + adjustment;
    for (long i = 0; i < collection->count; i++)
    {
        const s_dynamic_spawn_influencer *influencer = &collection->influencers[i];
        float factor;
        if (game_engine_spawn_influencer_test_point(point, influencer, &factor))
        {
            total += influencer->weight * factor;
            if (!enabled || !unbounded)
            {
                float minimum;
                long type = influencer->type;
                if (type == 5 || (type > 12 && type <= 14))
                {
                    minimum = influencer->weight;
                    if (minimum > 0.0f)
                        minimum = -minimum;
                }
                else
                {
                    minimum = k_real_min;
                }
                float maximum = influencer->weight;
                if (!(type == 5 || (type > 12 && type <= 14)))
                    maximum = k_real_max;
                total = PIN(total, minimum, maximum);
            }
        }
    }
    return total;
}

// 0x27bc80
void game_engine_spawn_influencer_record_player_death(long player_index)
{
    if (game_engine_spawn_influencer_game_options_valid() && game_globals->options.game_mode == 2)
    {
        short absolute_player_index = (short)player_index;
        if (absolute_player_index >= 0 && absolute_player_index < 16)
        {
            player_datum *player = (player_datum *)datum_try_and_get(tls_player_data(), player_index);
            if (player && player->unit_index != -1)
            {
                real_matrix4x3 matrix;
                ::object_get_world_matrix(player->unit_index, &matrix);
                memmove(&game_engine_globals->dead_players[1], &game_engine_globals->dead_players[0], 63 * sizeof(s_dead_player_info));
                game_engine_globals->dead_players[0].player_index = player_index;
                game_engine_globals->dead_players[0].time = game_time_get();
                game_engine_globals->dead_players[0].position = matrix.position;
            }
        }
    }
}

// player influences come from the scenario's tag block when it has any, else from the multiplayer constants
template <s_player_spawn_influence s_multiplayer_constants::*k_constants_member>
static __forceinline bool game_engine_spawn_influencer_build_player(long block_index, s_dynamic_spawn_influencer *influencer)
{
    bool result = false;
    const s_player_spawn_influence *influence = 0;
    scenario *map = global_scenario;
    if (map && map->player_influences[block_index].count() > 0)
        influence = map->player_influences[block_index].begin();
    else
    {
        const s_multiplayer_constants *constants = game_engine_multiplayer_constants_try_and_get();
        if (constants)
            influence = &(constants->*k_constants_member);
    }
    real_matrix4x3 matrix;
    if (influence)
        result = ::game_engine_build_player_spawn_influencer(influence,
            &matrix, influencer);
    return result;
}

#define SPAWN_INFLUENCER_PICK(scenario_block, constants_influence) \
    { \
        scenario *map = global_scenario; \
        if (map && (scenario_block).count() > 0) \
            influence = (scenario_block).begin(); \
        else \
        { \
            const s_multiplayer_constants *constants = game_engine_multiplayer_constants_try_and_get(); \
            if (constants) \
                influence = &constants->constants_influence; \
        } \
    }

template <long k_block_index, s_player_spawn_influence s_multiplayer_constants::*k_constants_member>
static __forceinline const s_player_spawn_influence *game_engine_spawn_influencer_get_player_influence()
{
    const s_player_spawn_influence *influence = 0;
    scenario *map = global_scenario;
    if (map && map->player_influences[k_block_index].count() > 0)
        influence = map->player_influences[k_block_index].begin();
    else
    {
        const s_multiplayer_constants *constants = game_engine_multiplayer_constants_try_and_get();
        if (constants)
            influence = &(constants->*k_constants_member);
    }
    return influence;
}

static __forceinline const s_player_spawn_influence *game_engine_spawn_influencer_get_dead_teammate_influence(float *duration)
{
    const s_player_spawn_influence *influence = 0;
    scenario *map = global_scenario;
    const s_multiplayer_constants *constants = game_engine_multiplayer_constants_try_and_get();
    if (map && map->player_influences[4].count() > 0)
        influence = map->player_influences[4].begin();
    else if (constants)
        influence = &constants->dead_teammate_influence;
    *duration = constants ? constants->dead_teammate_influence_duration : 15.0f;
    return influence;
}

struct s_netgame_goal_spawn_influence;

// @address 0x27c0e0 ?game_engine_spawn_influencer_build@@YA_NW4e_dynamic_spawn_influencer_type@@JPAUs_dynamic_spawn_influencer@@PAM@Z
bool game_engine_spawn_influencer_build(e_dynamic_spawn_influencer_type type, long object_index, s_dynamic_spawn_influencer *influencer, float *duration)
{
    bool result = false;
    memset(influencer, 0, sizeof(*influencer));
    influencer->type = (unsigned char)type;
    influencer->team_designator = 8;
    if (object_index != NONE)
        object_get_world_matrix(object_index, &influencer->transform);
    else
        memcpy(&influencer->transform, &k_real_math_identity_matrix_storage, sizeof(real_matrix4x3));

    real_matrix4x3 matrix;
    switch (type)
    {
    case _dynamic_spawn_influencer_type_enemy_forbid:
        {
            const s_player_spawn_influence *influence = game_engine_spawn_influencer_get_player_influence<0, &s_multiplayer_constants::ally_bias_influence>();
            if (influence)
                result = game_engine_build_player_spawn_influencer(influence, &matrix, influencer);
            break;
        }
    case _dynamic_spawn_influencer_type_enemy_bias:
        {
            const s_player_spawn_influence *influence = game_engine_spawn_influencer_get_player_influence<1, &s_multiplayer_constants::enemy_bias_influence>();
            if (influence)
                result = game_engine_build_player_spawn_influencer(influence, &matrix, influencer);
            break;
        }
    case _dynamic_spawn_influencer_type_ally_bias:
        {
            const s_player_spawn_influence *influence = game_engine_spawn_influencer_get_player_influence<2, &s_multiplayer_constants::vehicle_influence>();
            if (influence)
                result = game_engine_build_player_spawn_influencer(influence, &matrix, influencer);
            break;
        }
    case _dynamic_spawn_influencer_type_ally_bias_special:
        {
            const s_player_spawn_influence *influence = game_engine_spawn_influencer_get_player_influence<3, &s_multiplayer_constants::projectile_influence>();
            if (influence)
                result = game_engine_build_player_spawn_influencer(influence, &matrix, influencer);
            break;
        }
    case _dynamic_spawn_influencer_type_dead_teammate:
        {
            const s_player_spawn_influence *influence = game_engine_spawn_influencer_get_dead_teammate_influence(duration);
            if (influence)
                result = game_engine_build_player_spawn_influencer(influence, &matrix, influencer);
            break;
        }
    case _dynamic_spawn_influencer_type_respawn_zone:
        result = game_engine_build_respawn_zone_influencer(object_index, influencer);
        break;
    case _dynamic_spawn_influencer_type_weapon:
        result = game_engine_build_weapon_spawn_influencer(object_index, influencer);
        break;
    case _dynamic_spawn_influencer_type_vehicle:
        result = game_engine_build_vehicle_spawn_influencer(object_index, influencer);
        break;
    case _dynamic_spawn_influencer_type_projectile:
        result = game_engine_build_projectile_spawn_influencer(object_index, influencer);
        break;
    case _dynamic_spawn_influencer_type_equipment:
        result = game_engine_build_equipment_spawn_influencer(object_index, influencer);
        break;
    case _dynamic_spawn_influencer_type_netgame_goal_0:
        {
            const s_netgame_goal_spawn_influence *influence = 0;
            SPAWN_INFLUENCER_PICK(map->netgame_goal_influences[0], netgame_goal_influences[0]);
            if (influence)
                result = game_engine_build_netgame_goal_spawn_influencer(influence, &matrix, influencer);
            break;
        }
    case _dynamic_spawn_influencer_type_netgame_goal_1:
        {
            const s_netgame_goal_spawn_influence *influence = 0;
            SPAWN_INFLUENCER_PICK(map->netgame_goal_influences[1], netgame_goal_influences[1]);
            if (influence)
                result = game_engine_build_netgame_goal_spawn_influencer(influence, &matrix, influencer);
            break;
        }
    case _dynamic_spawn_influencer_type_netgame_goal_2:
        {
            const s_netgame_goal_spawn_influence *influence = 0;
            SPAWN_INFLUENCER_PICK(map->netgame_goal_influences[2], netgame_goal_influences[2]);
            if (influence)
                result = game_engine_build_netgame_goal_spawn_influencer(influence, &matrix, influencer);
            break;
        }
    case _dynamic_spawn_influencer_type_netgame_goal_3:
        {
            const s_netgame_goal_spawn_influence *influence = 0;
            SPAWN_INFLUENCER_PICK(map->netgame_goal_influences[3], netgame_goal_influences[3]);
            if (influence)
                result = game_engine_build_netgame_goal_spawn_influencer(influence, &matrix, influencer);
            break;
        }
    case _dynamic_spawn_influencer_type_netgame_goal_4:
        {
            const s_netgame_goal_spawn_influence *influence = 0;
            SPAWN_INFLUENCER_PICK(map->netgame_goal_influences[4], netgame_goal_influences[4]);
            if (influence)
                result = game_engine_build_netgame_goal_spawn_influencer(influence, &matrix, influencer);
            break;
        }
    case _dynamic_spawn_influencer_type_netgame_goal_5:
        {
            const s_netgame_goal_spawn_influence *influence = 0;
            SPAWN_INFLUENCER_PICK(map->netgame_goal_influences[5], netgame_goal_influences[5]);
            if (influence)
                result = game_engine_build_netgame_goal_spawn_influencer(influence, &matrix, influencer);
            break;
        }
    case _dynamic_spawn_influencer_type_netgame_goal_6:
        {
            const s_netgame_goal_spawn_influence *influence = 0;
            SPAWN_INFLUENCER_PICK(map->netgame_goal_influences[6], netgame_goal_influences[6]);
            if (influence)
                result = game_engine_build_netgame_goal_spawn_influencer(influence, &matrix, influencer);
            break;
        }
    case _dynamic_spawn_influencer_type_netgame_goal_7:
        {
            const s_netgame_goal_spawn_influence *influence = 0;
            SPAWN_INFLUENCER_PICK(map->netgame_goal_influences[7], netgame_goal_influences[7]);
            if (influence)
                result = game_engine_build_netgame_goal_spawn_influencer(influence, &matrix, influencer);
            break;
        }
    default:
        // match: 0x27c0e0: unreachable hint omits dead branch check
        __assume(0);
    }
    return result;
}
#undef SPAWN_INFLUENCER_PICK

static __forceinline bool game_engine_spawn_influencer_designator_flag_is_away(e_multiplayer_team_designator designator)
{
    bool result = false;
    long identifier = game_engine_globals->team_mapping[designator];
    s_game_engine_globals *globals = game_engine_globals;
    if ((unsigned long)designator <= 8 && game_engine_multiplayer_weapon_get((short)identifier))
        result = (globals->flag_state[identifier] & 1) == 0;
    return result;
}
static __forceinline bool game_engine_spawn_influencer_flag_is_away(e_game_team team)
{
    bool result = false;
    if (current_game_engine_entry())
    {
        e_multiplayer_team_designator designator = game_engine_team_index_to_team_designator(team);
        result = game_engine_spawn_influencer_designator_flag_is_away(designator);
    }
    return result;
}
static __forceinline bool game_engine_spawn_influencer_flag_is_home(e_game_team team)
{
    bool result = false;
    if (current_game_engine_entry())
    {
        long weapon_index;
        result = c_ctf_engine::team_flag_is_at_home(game_engine_team_index_to_team_designator(team), &weapon_index);
    }
    return result;
}

// 0x27c840: game_engine_build_player_spawn_influencer and game_engine_build_netgame_goal_spawn_influencer
// have identical bodies; the linker folded them (ICF), so they share one address here
// @address 0x27c840 ?game_engine_build_player_spawn_influencer@@YA_NPBUs_player_spawn_influence@@PBUreal_matrix4x3@@PAUs_dynamic_spawn_influencer@@@Z
bool game_engine_build_player_spawn_influencer(
    struct s_player_spawn_influence const *influence, struct real_matrix4x3 const *matrix, struct s_dynamic_spawn_influencer *out)
{
    float height = fabs(influence->lower_height) + influence->upper_height;
    float offset = influence->upper_height - height * 0.5f;
    bool result = true;
    if (fabs(offset) > 0.0001f)
    {
        real_point3d point;
        point.x = 0.0f;
        point.y = 0.0f;
        point.z = offset;
        matrix4x3_transform_point(&out->transform, &point, &out->transform.position);
    }
    spawn_influencer_copy_vector3d(&k_real_math_identity_matrix_storage.forward, &out->transform.forward);
    spawn_influencer_copy_vector3d(&k_real_math_identity_matrix_storage.left, &out->transform.left);
    spawn_influencer_copy_vector3d(&k_real_math_identity_matrix_storage.up, &out->transform.up);
    out->shape = 0;
    out->inner_radius = influence->inner_radius;
    out->outer_radius = influence->outer_radius;
    out->height = height;
    out->weight = influence->weight;
    return result;
}

// @address 0x27c840 ?game_engine_build_netgame_goal_spawn_influencer@@YA_NPBUs_netgame_goal_spawn_influence@@PBUreal_matrix4x3@@PAUs_dynamic_spawn_influencer@@@Z
// PC build only: on Xbox the image holds one body for both names, the player twin above.
#if !defined(_XBOX) || defined(__clang__)
bool game_engine_build_netgame_goal_spawn_influencer(
    struct s_netgame_goal_spawn_influence const *influence, struct real_matrix4x3 const *matrix, struct s_dynamic_spawn_influencer *out)
{
    float height = fabs(influence->lower_height) + influence->upper_height;
    float offset = influence->upper_height - height * 0.5f;
    bool result = true;
    if (fabs(offset) > 0.0001f)
    {
        real_point3d point;
        point.x = 0.0f;
        point.y = 0.0f;
        point.z = offset;
        matrix4x3_transform_point(&out->transform, &point, &out->transform.position);
    }
    spawn_influencer_copy_vector3d(&k_real_math_identity_matrix_storage.forward, &out->transform.forward);
    spawn_influencer_copy_vector3d(&k_real_math_identity_matrix_storage.left, &out->transform.left);
    spawn_influencer_copy_vector3d(&k_real_math_identity_matrix_storage.up, &out->transform.up);
    out->shape = 0;
    out->inner_radius = influence->inner_radius;
    out->outer_radius = influence->outer_radius;
    out->height = height;
    out->weight = influence->weight;
    return result;
}
#endif

static __forceinline long game_engine_spawn_influencer_object_owner_unit_index(long object_index)
{
    long unit_index = object_index;
    if (object_header_get(object_index)->object_type != 2)
    {
        while (unit_index != -1)
        {
            if (object_header_get(unit_index)->object_type == 0)
                break;
            unit_index = ((unit_datum *)object_header_get(unit_index)->datum)->object.parent_object_index;
        }
    }
    else
    {
        item_datum *weapon = (item_datum *)object_header_get(object_index)->datum;
        unit_index = weapon->owner_relation ? weapon->unit_object_index : -1;
    }
    return unit_index;
}

static __forceinline long game_engine_spawn_influencer_weapon_owner(long weapon_index)
{
    long owner = weapon_index;
    if (object_header_get(weapon_index)->object_type != 2)
    {
        while (owner != -1)
        {
            s_object_header *header = object_header_get(owner);
            if (header->object_type == 0)
                break;
            owner = ((unit_datum *)header->datum)->object.parent_object_index;
        }
    }
    else
    {
        item_datum *weapon = (item_datum *)object_header_get(weapon_index)->datum;
        owner = -1;
        if (weapon->owner_relation)
            owner = weapon->unit_object_index;
    }
    return owner;
}
static __forceinline long game_engine_spawn_influencer_current_weapon(long unit_index)
{
    unit_datum *unit = (unit_datum *)object_header_get(unit_index)->datum;
    long weapon_index = -1;
    if (unit->unit.current_weapon_index != -1)
        weapon_index = unit->unit.weapon_object_indices[unit->unit.current_weapon_index];
    return weapon_index;
}
static __forceinline void game_engine_spawn_influencer_rotation_at(
    real_matrix4x3 *matrix, const real_vector3d *forward, const real_vector3d *up, const real_point3d *position)
{
    real_point3d saved_position = *position;
    matrix4x3_rotation_from_vectors(matrix, forward, up);
    matrix->position = saved_position;
}
static __forceinline void game_engine_spawn_influencer_rotation(real_matrix4x3 *matrix, const real_vector3d *forward, const real_vector3d *up)
{
    real_point3d position = matrix->position;
    matrix4x3_rotation_from_vectors(matrix, forward, up);
    matrix->position = position;
}

static __forceinline void game_engine_spawn_influencer_rotation_along_velocity(real_matrix4x3 *matrix, const real_vector3d *velocity)
{
    real_point3d position = matrix->position;
    real_vector3d perpendicular;
    matrix4x3_rotation_from_vectors(matrix, perpendicular3d(velocity, &perpendicular), velocity);
    matrix->position = position;
}
static __forceinline float game_engine_spawn_influencer_magnitude_squared(const real_vector3d *vector)
{
    return vector->i * vector->i + (vector->k * vector->k + vector->j * vector->j);
}
static __forceinline void game_engine_spawn_influencer_offset_position(real_matrix4x3 *matrix, float distance, const real_vector3d *direction)
{
    matrix->position.x = distance * direction->i + matrix->position.x;
    matrix->position.y = distance * direction->j + matrix->position.y;
    matrix->position.z = distance * direction->k + matrix->position.z;
}

// @address 0x27cc68 ?game_engine_build_projectile_spawn_influencer@@YA_NJPAUs_dynamic_spawn_influencer@@@Z
bool game_engine_build_projectile_spawn_influencer(long object_index, s_dynamic_spawn_influencer *influencer)
{
    real_vector3d velocity;
    const s_projectile_spawn_influence *influence = game_engine_projectile_spawn_influence_try_and_get(object_index, &velocity);
    bool result = false;
    if (influence)
    {
        float speed_squared = game_engine_spawn_influencer_magnitude_squared(&velocity);
        if (speed_squared <= 10000.0f)
        {
            float speed = sqrtf(speed_squared);
            float distance = influence->velocity_scale * speed < 0.01f ? 0.01f : influence->velocity_scale * speed;
            if (speed > 0.01f)
            {
                real_point3d position = influencer->transform.position;
                real_vector3d perpendicular;
                const real_vector3d *left = perpendicular3d(&velocity, &perpendicular);
                real_matrix4x3 *transform = &influencer->transform;
                matrix4x3_rotation_from_vectors(transform, left, &velocity);
                transform->position = position;
                point_from_line3d(&influencer->transform.position, &velocity, distance * 0.5f, &influencer->transform.position);
            }
            result = true;
            influencer->shape = 1;
            influencer->inner_radius = influence->inner_radius;
            influencer->outer_radius = distance;
            influencer->weight = influence->weight;
        }
    }
    return result;
}
// @address 0x27cd78 ?game_engine_build_equipment_spawn_influencer@@YA_NJPAUs_dynamic_spawn_influencer@@@Z
bool game_engine_build_equipment_spawn_influencer(long index, struct s_dynamic_spawn_influencer * out)
{
    const s_equipment_spawn_influence *inf = game_engine_equipment_spawn_influence_try_and_get(index);
    bool result = false;
    if (inf)
    {
        s_multiplayer_object_properties *p = object_try_and_get_multiplayer(index);
        if (p)
        {
            switch (p->boundary_shape)
            {
            case 1: out->shape = 4; out->inner_radius = p->box_width; out->weight = inf->weight; result = true; break;
            case 2:
            out->shape = 0;
            out->outer_radius = p->box_width;
            out->inner_radius = p->box_width;
            out->height = p->box_height + fabsf(p->box_top);
            out->weight = inf->weight;
            result = true;
            break;
            case 3:
            out->shape = 3;
            out->inner_radius = p->box_width;
            out->outer_radius = p->box_length;
            out->height = p->box_height + fabsf(p->box_top);
            out->weight = inf->weight;
            result = true;
            break;
            }
        }
    }
    return result;
}

static __forceinline const s_player_spawn_influence *game_engine_spawn_influencer_get_enemy_bias_influence()
{
    const s_player_spawn_influence *result = 0;
    scenario *map = global_scenario;
    if (map && map->player_influences[1].count() > 0)
        result = map->player_influences[1].begin();
    else
    {
        const s_multiplayer_constants *constants = game_engine_multiplayer_constants_try_and_get();
        if (constants)
            result = &constants->enemy_bias_influence;
    }
    return result;
}

static __forceinline bool game_engine_spawn_influencer_build_player_influencer(
    long type, long object_index, s_dynamic_spawn_influencer *influencer, float *duration)
{
    bool result = false;
    memset(influencer, 0, sizeof(*influencer));
    influencer->type = (unsigned char)type;
    influencer->team_designator = 8;
    if (object_index != -1)
        ::object_get_world_matrix(object_index, &influencer->transform);
    else
        memcpy(&influencer->transform, &k_real_math_identity_matrix_storage, sizeof(real_matrix4x3));
    real_matrix4x3 matrix;
    switch (type)
    {
    case 1:
        {
            const s_player_spawn_influence *influence = game_engine_spawn_influencer_get_enemy_bias_influence();
            if (influence)
                result = ::game_engine_build_player_spawn_influencer(influence,
                    &matrix, influencer);
            break;
        }
    case 4:
        {
            const s_player_spawn_influence *influence = 0;
            scenario *map = global_scenario;
            const s_multiplayer_constants *constants = game_engine_multiplayer_constants_try_and_get();
            if (map && map->player_influences[4].count() > 0)
                influence = map->player_influences[4].begin();
            else if (constants)
                influence = &constants->dead_teammate_influence;
            *duration = constants ? constants->dead_teammate_influence_duration : 15.0f;
            if (influence)
                result = ::game_engine_build_player_spawn_influencer(influence,
                    &matrix, influencer);
            break;
        }
    }
    return result;
}

// @address 0x27cea8 ?game_engine_spawn_influencers_add_enemy_forbid_influence@@YAXJPBUplayer_datum@@PAUs_dynamic_spawn_influencer_collection@@@Z
void game_engine_spawn_influencers_add_enemy_forbid_influence(long player_index, const player_datum *player, s_dynamic_spawn_influencer_collection *collection)
{
    c_player_in_game_iterator iterator;
    iterator.begin();
    while (iterator.next())
    {
        player_datum *other_player = iterator.get_datum();
        if (other_player->unit_index != -1 && game_engine_spawn_influencer_is_enemy(player_index, iterator.get_index()))
        {
            s_dynamic_spawn_influencer influencer;
            if (game_engine_spawn_influencer_build(_dynamic_spawn_influencer_type_enemy_forbid, other_player->unit_index, &influencer, 0))
            {
                if ((unsigned short)collection->count < 222)
                {
                    memcpy(&collection->influencers[collection->count], &influencer, sizeof(influencer));
                    collection->count++;
                }
                else
                    break;
            }
        }
    }
}

// @address 0x27cf90 ?game_engine_spawn_influencers_add_enemy_bias_influence@@YAXJPBUplayer_datum@@PAUs_dynamic_spawn_influencer_collection@@@Z
void game_engine_spawn_influencers_add_enemy_bias_influence(long player_index, const player_datum *player, s_dynamic_spawn_influencer_collection *collection)
{
    c_player_in_game_iterator iterator;
    iterator.begin();
    while (iterator.next())
    {
        player_datum *other_player = iterator.get_datum();
        if (other_player->unit_index != -1 && game_engine_spawn_influencer_is_enemy(player_index, iterator.get_index()))
        {
            s_dynamic_spawn_influencer influencer;
            float duration;
            bool built = game_engine_spawn_influencer_build_player_influencer(1, iterator.get_datum()->unit_index, &influencer, &duration);
            if (built)
            {
                if ((unsigned short)collection->count < 222)
                {
                    memcpy(&collection->influencers[collection->count], &influencer, sizeof(influencer));
                    collection->count++;
                }
                else
                    break;
            }
        }
    }
}

// @address 0x27d110 game_engine_spawn_influencers_add_teammate_influence
extern "C" void game_engine_spawn_influencers_add_teammate_influence(
    long player_index, const player_datum *player, s_dynamic_spawn_influencer_collection *collection)
{
    c_player_in_game_iterator iterator;
    iterator.begin();
    while (iterator.next())
    {
        long other_index = iterator.get_index();
        if (other_index != player_index)
        {
            player_datum *other_player = iterator.get_datum();
            if (other_player->unit_index != -1 && !game_engine_spawn_influencer_is_enemy(player_index, other_index))
            {
                s_dynamic_spawn_influencer influencer;
                e_dynamic_spawn_influencer_type type = (e_dynamic_spawn_influencer_type)(2 + (player->respawn_target_player_index == other_index));
                long unit_index = other_player->unit_index;
                if (game_engine_spawn_influencer_build(type, unit_index, &influencer, 0))
                {
                    if ((unsigned short)collection->count < 222)
                    {
                        memcpy(&collection->influencers[collection->count], &influencer, sizeof(influencer));
                        collection->count++;
                    }
                    else
                        break;
                }
            }
        }
    }
}

// @address 0x27d218 ?game_engine_spawn_influencers_add_dead_teammate_influence@@YAXJPBUplayer_datum@@PAUs_dynamic_spawn_influencer_collection@@@Z
void game_engine_spawn_influencers_add_dead_teammate_influence(long player_index, const player_datum *player, s_dynamic_spawn_influencer_collection *collection)
{
    s_dynamic_spawn_influencer influencer;
    float duration;
    if (game_engine_spawn_influencer_build_player_influencer(4, -1, &influencer, &duration))
    {
        float tick_length = game_tick_length();
        float current_time = (float)game_time_get() * tick_length;
        for (int index = 0; index < 64 && game_engine_globals->dead_players[index].player_index != -1; index++)
        {
            if ((float)game_engine_globals->dead_players[index].time * game_tick_length() < current_time - duration)
            {
                game_engine_globals->dead_players[index].player_index = -1;
                break;
            }
            if (datum_try_and_get(tls_player_data(), game_engine_globals->dead_players[index].player_index) &&
                !game_engine_spawn_influencer_is_enemy(player_index, game_engine_globals->dead_players[index].player_index))
            {
                if ((unsigned short)collection->count < 222)
                {
                    s_dead_player_info *dead_player = &game_engine_globals->dead_players[index];
                    influencer.transform.position = dead_player->position;
                    memcpy(&collection->influencers[collection->count], &influencer, sizeof(influencer));
                    collection->count++;
                }
                else
                    break;
            }
        }
    }
}

static __forceinline bool game_engine_spawn_influencer_build_projectile(long object_index, s_dynamic_spawn_influencer *influencer)
{
    bool result = false;
    memset(influencer, 0, sizeof(*influencer));
    influencer->type = 8;
    influencer->team_designator = 8;
    if (object_index != -1)
        ::object_get_world_matrix(object_index, &influencer->transform);
    else
        memcpy(&influencer->transform, &k_real_math_identity_matrix_storage, sizeof(real_matrix4x3));
    result = game_engine_build_projectile_spawn_influencer(object_index, influencer);
    return result;
}

static __forceinline bool game_engine_spawn_influencer_build_object_influencer(long type, long object_index, s_dynamic_spawn_influencer *influencer)
{
    bool result = false;
    memset(influencer, 0, sizeof(s_dynamic_spawn_influencer));
    influencer->type = (unsigned char)type;
    influencer->team_designator = 8;
    if (object_index != -1)
        ::object_get_world_matrix(object_index, &influencer->transform);
    else
        memcpy(&influencer->transform, &k_real_math_identity_matrix_storage, sizeof(real_matrix4x3));
    switch (type)
    {
    case 6:
        result = game_engine_build_weapon_spawn_influencer(object_index, influencer);
        break;
    case 7:
        result = game_engine_build_vehicle_spawn_influencer(object_index, influencer);
        break;
    case 9:
        result = ::game_engine_build_equipment_spawn_influencer(object_index, influencer);
        break;
    }
    return result;
}

static __forceinline bool game_engine_spawn_influencer_seat_flag_test(unsigned long flags, long bit)
{
    return ((flags >> bit) & 1) != 0;
}

// @address 0x27d608 game_engine_spawn_influencers_add_enemy_weapon_influence
extern "C" void game_engine_spawn_influencers_add_enemy_weapon_influence(
    long player_index, const player_datum *player, s_dynamic_spawn_influencer_collection *collection)
{
    c_player_in_game_iterator iterator;
    iterator.begin();
    while (iterator.next())
    {
        bool found = false;
        s_dynamic_spawn_influencer influencer;
        if (game_engine_spawn_influencer_is_enemy(player_index, iterator.get_index()) && iterator.get_datum()->unit_index != -1)
        {
            bool check_inventory = true;
            unit_datum *unit =
                (unit_datum *)object_header_get(iterator.get_datum()->unit_index)->datum;
            if (unit->object.parent_object_index != -1 && unit->unit.parent_seat_index != -1)
            {
                unit_datum *parent =
                    (unit_datum *)object_header_get(unit->object.parent_object_index)->datum;
                const unit_definition *parent_definition = TAG_GET('unit', const unit_definition, parent->object.definition_index);
                const unit_seat *seat = &parent_definition->seats[unit->unit.parent_seat_index];
                if (game_engine_spawn_influencer_seat_flag_test(seat->flags, 3))
                {
                    check_inventory = false;
                    long weapon_index = game_engine_spawn_influencer_current_weapon(
                        ((unit_datum *)object_header_get(iterator.get_datum()->unit_index)->datum)
                            ->object.parent_object_index);
                    if (weapon_index != -1)
                        found = game_engine_spawn_influencer_build_object_influencer(6, weapon_index, &influencer);
                }
                else if (!game_engine_spawn_influencer_seat_flag_test(seat->flags, 5))
                {
                    check_inventory = false;
                }
            }
            if (check_inventory)
            {
                float best_weight = 0.0f;
                for (long index = 0; index < 4; index++)
                {
                    long weapon_index = unit->unit.weapon_object_indices[index];
                    if (weapon_index != -1)
                    {
                        s_dynamic_spawn_influencer candidate;
                        if (game_engine_spawn_influencer_build_object_influencer(6, weapon_index, &candidate) && candidate.outer_radius > best_weight)
                        {
                            memcpy(&influencer, &candidate, sizeof(candidate));
                            best_weight = candidate.outer_radius;
                            found = true;
                        }
                    }
                }
            }
        }
        if (found)
        {
            if ((unsigned short)collection->count < 222)
            {
                memcpy(&collection->influencers[collection->count], &influencer, sizeof(influencer));
                collection->count++;
            }
            else
                break;
        }
    }
}

// @address 0x27d8a8 game_engine_spawn_influencers_add_vehicle_influence
extern "C" void game_engine_spawn_influencers_add_vehicle_influence(const player_datum *player, s_dynamic_spawn_influencer_collection *collection)
{
    long count = 0;
    c_object_iterator<object_datum> iterator;
    iterator.begin(2, 0);
    while (iterator.next() && count < 16)
    {
        s_dynamic_spawn_influencer influencer;
        if (game_engine_spawn_influencer_build_object_influencer(7, iterator.get_index(), &influencer))
        {
            if ((unsigned short)collection->count < 222)
            {
                memcpy(&collection->influencers[collection->count], &influencer, sizeof(influencer));
                collection->count++;
                count++;
            }
            else
                break;
        }
    }
    // Keep the volatile count read.
    (void)(collection->count == 222 || count == 16);
}

// @address 0x27d9e8 game_engine_spawn_influence_add_projectile_influence
extern "C" void game_engine_spawn_influence_add_projectile_influence(const player_datum *player, s_dynamic_spawn_influencer_collection *collection)
{
    long count = 0;
    c_object_iterator<object_datum> iterator;
    iterator.begin(32, 0);
    while (iterator.next() && count < 32)
    {
        s_dynamic_spawn_influencer influencer;
        if (game_engine_spawn_influencer_build_projectile(iterator.get_index(), &influencer))
        {
            if ((unsigned short)collection->count < 222)
            {
                memcpy(&collection->influencers[collection->count], &influencer, sizeof(influencer));
                collection->count++;
                count++;
            }
            else
                break;
        }
    }
    // Keep the volatile count read.
    (void)(collection->count == 222 || count == 32);
}

// 0x27db20
void game_engine_spawn_influence_add_equipment_influence(const player_datum *player, s_dynamic_spawn_influencer_collection *collection)
{
    long equipment_count = 0;
    c_object_iterator<equipment_datum> iterator;
    iterator.begin(8, 0);
    while (iterator.next() && equipment_count < 16)
    {
        s_dynamic_spawn_influencer influencer;
        if (game_engine_spawn_influencer_build_object_influencer(9, iterator.get_index(), &influencer))
        {
            if (collection->count < sizeof(collection->influencers) / sizeof(collection->influencers[0]))
            {
                memcpy(&collection->influencers[collection->count], &influencer, sizeof(s_dynamic_spawn_influencer));
                collection->count++;
                equipment_count++;
            }
            else
            {
                break;
            }
        }
    }
    // Keep the volatile count read.
    (void)(collection->count == sizeof(collection->influencers) / sizeof(collection->influencers[0]) || equipment_count == 16);
}

// @address 0x27dc58 ?game_engine_spawn_influence_add_netgame_goal_object_influence@@YAXJPAUs_dynamic_spawn_influencer_collection@@@Z
void game_engine_spawn_influence_add_netgame_goal_object_influence(long player_index, s_dynamic_spawn_influencer_collection *collection)
{
    s_netgame_goal_influencer goal_influencers[16];
    long goal_influencer_count = 16;
    ((c_game_engine *)game_engine_spawn_influencer_current_engine_inline())
        ->assemble_spawn_influencers_for_player(player_index, goal_influencers, &goal_influencer_count);
    for (long goal_index = 0; goal_index < goal_influencer_count; goal_index++)
    {
        s_dynamic_spawn_influencer influencer;
        if (game_engine_spawn_influencer_build((e_dynamic_spawn_influencer_type)goal_influencers[goal_index].type,
                goal_influencers[goal_index].object_index, &influencer, 0))
        {
            if ((unsigned short)collection->count < 222)
            {
                memcpy(&collection->influencers[collection->count], &influencer, sizeof(influencer));
                collection->count++;
            }
            else
                break;
        }
    }
}

const s_multiplayer_constants *game_engine_multiplayer_constants_try_and_get(void)
{
    if (global_game_globals && global_game_globals->multiplayer_globals_index != -1)
    {
        const s_multiplayer_globals_definition *multiplayer_globals =
            TAG_GET('mulg', const s_multiplayer_globals_definition, global_game_globals->multiplayer_globals_index);
        if (multiplayer_globals && multiplayer_globals->runtime.count > 0 && multiplayer_globals->runtime.address)
        {
            return (const s_multiplayer_constants *)((const s_multiplayer_runtime_globals_definition *)multiplayer_globals->runtime.address)
                ->multiplayer_constants.address;
        }
    }
    return 0;
}

// Local copy of the multiplayer-constants lookup used by the spawn-influence helpers below
static __forceinline const s_multiplayer_constants *game_engine_spawn_influencer_multiplayer_constants_try_and_get(void)
{
    if (global_game_globals && global_game_globals->multiplayer_globals_index != -1)
    {
        const s_multiplayer_globals_definition *multiplayer_globals =
            TAG_GET('mulg', const s_multiplayer_globals_definition, global_game_globals->multiplayer_globals_index);
        if (multiplayer_globals && multiplayer_globals->runtime.count > 0 && multiplayer_globals->runtime.address)
        {
            return (const s_multiplayer_constants *)((const s_multiplayer_runtime_globals_definition *)multiplayer_globals->runtime.address)
                ->multiplayer_constants.address;
        }
    }
    return 0;
}

const s_weapon_spawn_influence *game_engine_weapon_spawn_influence_try_and_get(long object_index)
{
    const unit_datum *object = (unit_datum *)object_header_get(object_index)->datum;
    const s_weapon_spawn_influence *result = 0;
    if (global_scenario)
    {
        for (long index = 0; index < global_scenario->weapon_influences.count(); index++)
        {
            const s_weapon_spawn_influence *influence = &global_scenario->weapon_influences[index];
            if (influence->weapon.index == object->object.definition_index)
            {
                result = influence;
                break;
            }
        }
    }
    if (!result)
    {
        const s_multiplayer_constants *constants = game_engine_spawn_influencer_multiplayer_constants_try_and_get();
        if (constants)
        {
            for (long index = 0; index < constants->weapon_selections.count(); index++)
            {
                const s_weapon_spawn_influence *influence = &constants->weapon_selections[index];
                if (influence->weapon.index == object->object.definition_index)
                {
                    result = influence;
                    break;
                }
            }
        }
    }
    return result;
}

static __forceinline bool game_engine_spawn_influencer_user_object_has_multiplayer_properties(long index)
{
    return ((unit_datum *)object_get_and_verify_type(index, NONE))->object.multiplayer_data_offset != -1;
}

static __forceinline bool game_engine_spawn_influencer_user_can_be_used(long designator, long team)
{
    return designator == 8 || team == 8 || designator == team;
}

static __forceinline bool game_engine_spawn_influencer_user_build_respawn_zone(long object_index, s_dynamic_spawn_influencer *entry)
{
    bool result = false;
    memset(entry, 0, sizeof(*entry));
    entry->type = 5;
    entry->team_designator = 8;
    if (object_index != -1)
        ::object_get_world_matrix(object_index, &entry->transform);
    else
        entry->transform = k_real_math_identity_matrix_storage;
    result = game_engine_build_respawn_zone_influencer(object_index, entry);
    return result;
}

// @address 0x27d458 ?game_engine_spawn_influencers_add_user_placed_influence@@YAXJPAUs_dynamic_spawn_influencer_collection@@@Z
void game_engine_spawn_influencers_add_user_placed_influence(long player_index, s_dynamic_spawn_influencer_collection *influencers)
{
    long count = 0;
    c_multiplayer_object_iterator<unit_datum> objects;
    objects.begin(64, 0, 8);
    while (objects.next() && count < 16)
    {
        if (game_engine_spawn_influencer_user_object_has_multiplayer_properties(objects.get_index()))
        {
            s_dynamic_spawn_influencer entry;
            bool valid = game_engine_spawn_influencer_user_build_respawn_zone(objects.get_index(), &entry);
            if (valid &&
                game_engine_spawn_influencer_user_can_be_used((unsigned short)entry.team_designator,
                    current_game_engine()->get_player_team_designator(player_index)))
            {
                if ((unsigned short)influencers->count < 222)
                {
                    memcpy(&influencers->influencers[influencers->count], &entry, sizeof(entry));
                    influencers->count++;
                    count++;
                }
                else
                    break;
            }
        }
    }
    // Keep the volatile count read.
    (void)(influencers->count == 222 || count == 16);
}

// @address 0x27dec0 ?game_engine_vehicle_spawn_influence_try_and_get@@YAPBUs_vehicle_spawn_influence@@JPATreal_vector3d@@PAM@Z
const s_vehicle_spawn_influence *game_engine_vehicle_spawn_influence_try_and_get(long object_index, real_vector3d *velocity, float *radius)
{
    const unit_datum *object = (unit_datum *)object_header_get(object_index)->datum;
    const s_vehicle_spawn_influence *influence = 0;
    const s_vehicle_spawn_influence *result = 0;
    if (global_scenario)
    {
        long index = 0;
        if (index < global_scenario->vehicle_influences.count())
        {
            long definition_index = object->object.definition_index;
            influence = global_scenario->vehicle_influences.begin();
            for (; index < global_scenario->vehicle_influences.count(); ++index, ++influence)
            {
            if (influence->vehicle.index == definition_index)
            {
                result = influence;
                break;
            }
            }
        }
    }
    if (!result)
    {
        const s_multiplayer_constants *constants = game_engine_spawn_influencer_multiplayer_constants_try_and_get();
        if (constants)
        {
            long index = 0;
            if (index < constants->vehicle_selections.count())
            {
                long definition_index = object->object.definition_index;
                influence = constants->vehicle_selections.begin();
                for (; index < constants->vehicle_selections.count(); ++index, ++influence)
                {
                if (influence->vehicle.index == definition_index)
                {
                    result = influence;
                    break;
                }
                }
            }
        }
    }
    if (result)
    {
        object_get_velocities(object_index, velocity, 0);
        float influence_radius = influence->inner_radius;
        if (!(influence_radius > 0.0001f))
        {
            *radius = influence_radius
                = ((unit_datum *)object_header_get(object_index)->datum)->object.bounding_radius;
        } else *radius = influence_radius;
    }
    return result;
}

// @address 0x27e018 ?game_engine_projectile_spawn_influence_try_and_get@@YAPBUs_projectile_spawn_influence@@JPATreal_vector3d@@@Z
const s_projectile_spawn_influence *game_engine_projectile_spawn_influence_try_and_get(long object_index, real_vector3d *velocity)
{
    scenario *map = global_scenario;
    const unit_datum *object = (unit_datum *)object_header_get(object_index)->datum;
    const s_projectile_spawn_influence *result = 0;
    if (map)
    {
        for (long index = 0; index < map->projectile_influences.count(); index++)
        {
            const s_projectile_spawn_influence *influence = &map->projectile_influences[index];
            if (influence->projectile.index == object->object.definition_index)
            {
                result = influence;
                break;
            }
        }
    }
    if (!result)
    {
        const s_multiplayer_constants *constants = game_engine_spawn_influencer_multiplayer_constants_try_and_get();
        if (constants)
        {
            for (long index = 0; index < constants->projectile_selections.count(); index++)
            {
                const s_projectile_spawn_influence *selection = &constants->projectile_selections[index];
                if (selection->projectile.index == object->object.definition_index)
                {
                    result = selection;
                    break;
                }
            }
        }
    }
    if (result)
        object_get_velocities(object_index, velocity, 0);
    return result;
}

const s_equipment_spawn_influence *game_engine_equipment_spawn_influence_try_and_get(long object_index)
{
    const unit_datum *object = (unit_datum *)object_header_get(object_index)->datum;
    const s_equipment_spawn_influence *result = 0;
    if (global_scenario)
    {
        for (long index = 0; index < global_scenario->equipment_influences.count(); index++)
        {
            const s_equipment_spawn_influence *influence = &global_scenario->equipment_influences[index];
            if (influence->equipment.index == object->object.definition_index)
            {
                result = influence;
                break;
            }
        }
    }
    if (!result)
    {
        const s_multiplayer_constants *constants = game_engine_spawn_influencer_multiplayer_constants_try_and_get();
        if (constants)
        {
            for (long index = 0; index < constants->equipment_selections.count(); index++)
            {
                const s_equipment_spawn_influence *influence = &constants->equipment_selections[index];
                if (influence->equipment.index == object->object.definition_index)
                {
                    result = influence;
                    break;
                }
            }
        }
    }
    return result;
}

// @address 0x27e248 game_engine_spawn_influencer_is_enemy
extern "C" bool game_engine_spawn_influencer_is_enemy(long player_index, long other_player_index)
{
    if (player_index != other_player_index)
    {
        if (game_engine_has_teams())
        {
            player_datum *player = (player_datum *)(tls_player_data()->data + (player_index & 0xffff) * sizeof(player_datum));
            player_datum *other_player = (player_datum *)(tls_player_data()->data + (other_player_index & 0xffff) * sizeof(player_datum));
            return ::game_engine_team_is_enemy((e_game_team)player->configuration.host.team, (e_game_team)other_player->configuration.host.team);
        }
        const c_game_engine *engine = current_game_engine();
        long designator = engine->get_player_team_designator(player_index);
        long other_designator = engine->get_player_team_designator(other_player_index);
        if (designator != 8 && other_designator != 8)
            return designator != other_designator;
        return true;
    }
    return false;
}

static __forceinline long game_engine_spawn_influencer_unit_get_weapon(const unit_datum *unit, signed char inventory_index)
{
    long weapon_index = -1;
    if (inventory_index != -1)
        weapon_index = unit->unit.weapon_object_indices[inventory_index];
    return weapon_index;
}

// @address 0x27c958 ?game_engine_build_weapon_spawn_influencer@@YA_NJPAUs_dynamic_spawn_influencer@@@Z
bool game_engine_build_weapon_spawn_influencer(long weapon_index, s_dynamic_spawn_influencer *influencer)
{
    const s_weapon_spawn_influence *influence = game_engine_weapon_spawn_influence_try_and_get(weapon_index);
    bool result = false;
    if (influence)
    {
        long unit_index = weapon_index;
        if (object_header_get(weapon_index)->object_type != 2)
        {
            while (unit_index != -1)
            {
                s_object_header *header = object_header_get(unit_index);
                if (header->object_type == 0)
                    break;
                unit_index = ((unit_datum *)header->datum)->object.parent_object_index;
            }
        }
        else
        {
            item_datum *weapon = (item_datum *)object_header_get(weapon_index)->datum;
            long owner_index = -1;
            if (weapon->owner_relation)
                owner_index = weapon->unit_object_index;
            unit_index = owner_index;
        }
        if (unit_index != -1)
        {
            unit_datum *unit = (unit_datum *)object_header_get(unit_index)->datum;
            long current_weapon_index = game_engine_spawn_influencer_unit_get_weapon(unit, unit->unit.current_weapon_index);
            if (current_weapon_index == weapon_index)
            {
                real_vector3d forward = ((unit_datum *)((s_object_header *)tls_object_header_data()->data)
                    [DATUM_INDEX_TO_ABSOLUTE_INDEX(unit_index)].datum)->unit.aiming_vector;
                real_point3d position;
                if (!weapon_get_trigger_position(current_weapon_index, 0, &position))
                    position = influencer->transform.position;
                real_vector3d up;
                real_vector3d *up_vector = perpendicular3d(&forward, &up);
                real_matrix4x3 *matrix = &influencer->transform;
                matrix4x3_rotation_from_vectors(matrix, &forward, up_vector);
                matrix->position = position;
                influencer->shape = 2;
                influencer->inner_radius = influence->inner_radius;
                influencer->outer_radius = influence->outer_radius;
                influencer->height = influence->height;
                influencer->weight = influence->weight;
                result = true;
            }
        }
    }
    return result;
}

static __forceinline bool game_engine_vehicle_spawn_object_hidden(long object_index)
{
    return (((unit_datum *)object_header_get(object_index)->datum)->object.physics_flags >> 9) & 1;
}

static __forceinline void game_engine_vehicle_rotation_at(real_matrix4x3 *matrix, const real_vector3d *forward, const real_vector3d *up, real_point3d position);
static __forceinline void game_engine_vehicle_rotation_at(
    real_matrix4x3 * matrix, const real_vector3d *forward, const real_vector3d *up, real_point3d position)
{
    matrix4x3_rotation_from_vectors(matrix, forward, up);
    matrix->position = position;
}

static __forceinline real_point3d *game_engine_vehicle_point_from_line(
    const real_point3d *point, const real_vector3d *vector, float distance, real_point3d *result);
static __forceinline real_point3d *game_engine_vehicle_point_from_line(
    const real_point3d *point, const real_vector3d *vector, float distance, real_point3d *result)
{
    result->x = point->x + vector->i * distance;
    result->y = point->y + vector->j * distance;
    result->z = point->z + vector->k * distance;
    return result;
}

// @address 0x27caf8 ?game_engine_build_vehicle_spawn_influencer@@YA_NJPAUs_dynamic_spawn_influencer@@@Z
bool game_engine_build_vehicle_spawn_influencer(long object_index, s_dynamic_spawn_influencer *influencer)
{
    bool result = false;
    if (!game_engine_vehicle_spawn_object_hidden(object_index)) {
        real_vector3d velocity;
        float radius;
        const s_vehicle_spawn_influence *influence = game_engine_vehicle_spawn_influence_try_and_get(object_index, &velocity, &radius);
        if (influence && radius > 0.0f) {
            float minimum_speed = influence->minimum_speed;
            if (minimum_speed <= 0.05f)
                minimum_speed = 0.05f;
            float speed_squared = game_engine_spawn_influencer_magnitude_squared(&velocity);
            if (speed_squared >= minimum_speed * minimum_speed) {
                float speed = sqrtf(speed_squared);
                float distance = influence->velocity_scale * speed;
                if (distance < 0.01f)
                    distance = 0.01f;
                if (speed > 0.01f) {
                    real_vector3d perpendicular;
                    game_engine_vehicle_rotation_at(
                        &influencer->transform, perpendicular3d(&velocity, &perpendicular), &velocity, influencer->transform.position);
                    game_engine_vehicle_point_from_line(&influencer->transform.position, &velocity, distance * 0.5f, &influencer->transform.position);
                }
                influencer->inner_radius = radius;
                influencer->outer_radius = distance;
                influencer->shape = 1;
                influencer->weight = influence->weight;
                result = true;
            }
        }
    }
    return result;
}
// @address 0x27c5c8 ?game_engine_build_respawn_zone_influencer@@YA_NJPAUs_dynamic_spawn_influencer@@@Z
bool game_engine_build_respawn_zone_influencer(long object_index, s_dynamic_spawn_influencer *influencer)
{
    bool result = false;
    s_multiplayer_object_properties *multiplayer = object_try_and_get_multiplayer(object_index);
    if (multiplayer)
    {
        influencer->team_designator = multiplayer->team_designator;
        influencer->weight = multiplayer->initial_spawn_weight;
        const c_game_engine *engine = (const c_game_engine *)game_engine_spawn_influencer_current_engine_inline();
        if (engine->get_type() == 1)
        {
            e_game_team team = (e_game_team)game_engine_globals->team_mapping[multiplayer->team_designator];
            if (game_engine_spawn_influencer_flag_is_away(team))
                influencer->weight += multiplayer->boundary_outer_radius;
            else if (game_engine_spawn_influencer_flag_is_home(team))
                influencer->weight += multiplayer->boundary_inner_radius;
        }
        s_multiplayer_object_boundary_geometry_data geometry;
        calculate_influence_boundary_geometry(object_index, &geometry, 0);
        switch (multiplayer->boundary_shape)
        {
        case 1:
            influencer->shape = 4;
            influencer->inner_radius = geometry.width;
            result = true;
            break;
        case 2:
            influencer->shape = 0;
            memcpy(&influencer->transform, &geometry.matrix, sizeof(geometry.matrix));
            influencer->inner_radius = geometry.width;
            influencer->outer_radius = geometry.width;
            influencer->height = (float)fabs(geometry.neg_height) + geometry.height;
            result = true;
            break;
        case 3:
            influencer->shape = 3;
            memcpy(&influencer->transform, &geometry.matrix, sizeof(geometry.matrix));
            influencer->inner_radius = geometry.width;
            influencer->outer_radius = geometry.depth;
            influencer->height = (float)fabs(geometry.neg_height) + geometry.height;
            result = true;
            break;
        }
    }
    return result;
}
