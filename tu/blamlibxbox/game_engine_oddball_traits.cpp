// blamlibxbox/game_engine_oddball_traits.obj: from the records 0x264e28 0x267c98 0x268410 0x268540 0x268598 0x2685a8
// @flags /TP /O2 /Oc
#include "cseries/cseries_wrapped.h"
#include <string.h>
#include "memory/bitstream.h"
#include "game/game_types.h"
#include "game/game_engine_group.h"
// @address 0x244bb8 ?game_results_statistic_increment@@YAXJW4e_game_team@@W4e_game_results_statistic@@J@Z
// @address 0x264e28 ?encode@c_game_engine_base_variant@@UBAXPAVc_bitstream@@@Z
// @address 0x266a50 ?set@c_player_traits@@QAAXPBV1@_N@Z
// @address 0x267c98 game_engine_slayer_variant_set_from_definition
#include "game/game_results_types.h"
#include "game/game_results.h"

#include "game/player_traits.h"



#include "game/game_engine_oddball_variant.h"
#include "game/game_engine_oddball_traits_internals.h"
#include "game/game_engine_player_traits.h"

// @address 0x268598 ?custom_team_score_stats@c_game_engine_oddball_variant@@UBAXJJJ@Z
void c_game_engine_oddball_variant::custom_team_score_stats(long team_index, long first_value, long last_value) const
{
    game_results_statistic_increment(-1, (e_game_team)team_index, _game_results_statistic_oddball_time_with_ball, last_value - first_value);
}

static __forceinline void game_engine_oddball_traits_clear_shield(c_player_trait_shield_vitality *traits)
{
    traits->m_damage_resistance = _damage_resistance_percentage_setting_unchanged;
    traits->m_shield_recharge_rate = _shield_recharge_rate_percentage_setting_unchanged;
    traits->m_vampirism = _vampirism_percentage_setting_unchanged;
    traits->m_headshot_immunity = _headshot_immunity_setting_unchanged;
    traits->m_shield_multiplier = _shield_multiplier_setting_unchanged;
    memset(traits->m_padding, 0, sizeof(traits->m_padding));
}

static __forceinline void game_engine_oddball_traits_clear_weapons(c_player_trait_weapons *traits)
{
    traits->m_damage_modifier = _damage_modifier_percentage_setting_unchanged;
    traits->m_initial_primary_weapon = -2;
    traits->m_initial_secondary_weapon = -2;
    traits->m_grenade_count = _grenade_count_setting_unchanged;
    traits->m_recharging_grenades = _recharging_grenades_setting_unchanged;
    traits->m_infinite_ammo = _infinite_ammo_setting_unchanged;
    traits->m_weapon_pickup = _weapon_pickup_setting_unchanged;
}

static __forceinline void game_engine_oddball_traits_clear_movement(c_player_trait_movement *traits)
{
    traits->m_speed = _player_speed_setting_unchanged;
    traits->m_gravity = _player_gravity_setting_unchanged;
    traits->m_vehicle_usage = _vehicle_usage_setting_unchanged;
    traits->m_padding = 0;
}

static __forceinline void game_engine_oddball_traits_clear_appearance(c_player_trait_appearance *traits)
{
    traits->m_active_camo = _active_camo_setting_unchanged;
    traits->m_waypoint = _waypoint_setting_unchanged;
    traits->m_aura = _aura_setting_unchanged;
    traits->m_forced_change_color = _forced_change_color_setting_unchanged;
}

static __forceinline void game_engine_oddball_traits_clear_sensors(c_player_trait_sensors *traits)
{
    traits->m_motion_tracker = _motion_tracker_setting_unchanged;
    traits->m_motion_tracker_range = _motion_tracker_range_setting_unchanged;
}

static __forceinline void game_engine_oddball_traits_clear_player_traits(c_player_traits *traits)
{
    game_engine_oddball_traits_clear_shield(&traits->m_shield_vitality_traits);
    game_engine_oddball_traits_clear_weapons(&traits->m_weapon_traits);
    game_engine_oddball_traits_clear_movement(&traits->m_movement_traits);
    game_engine_oddball_traits_clear_appearance(&traits->m_appearance_traits);
    game_engine_oddball_traits_clear_sensors(&traits->m_sensor_traits);
}

// @address 0x268410 ?initialize@c_game_engine_oddball_variant@@UAAXXZ
void c_game_engine_oddball_variant::initialize()
{
    c_game_engine_base_variant::initialize();
    m_flags.clear();
    set_auto_ball_pickup(false);
    set_ball_effect_enabled(true);
    m_score_to_win = 200;
    m_carrying_points = 1;
    m_kill_points = 0;
    m_ball_kill_points = 0;
    m_carrier_kill_points = 0;
    m_ball_count = 1;
    m_ball_spawn_delay = 5;
    m_ball_inactive_respawn_delay = 30;
    c_player_traits *carrier_traits = &m_carrier_traits;
    game_engine_oddball_traits_clear_player_traits(carrier_traits);
}

void c_game_engine_base_variant::encode(c_bitstream *packet) const
{
    saved_game_files_encode_metadata(&m_metadata, packet);
    packet->write_bool("variant-built-in", get_built_in());

    packet->write_bool("miscellaneous-options-teams", get_miscellaneous_options()->get_teams_enabled());
    packet->write_bool("miscellaneous-options-round-reset-players", get_miscellaneous_options()->get_round_reset_players());
    packet->write_bool("miscellaneous-options-round-reset-map", get_miscellaneous_options()->get_round_reset_map());
    packet->write_integer("miscellaneous-options-round-time-limit-minutes", get_miscellaneous_options()->get_round_time_limit_minutes(), 8);
    packet->write_integer("miscellaneous-options-round-limit", get_miscellaneous_options()->get_round_limit(), 4);
    packet->write_integer("miscellaneous-options-early-victory-win-count", get_miscellaneous_options()->get_early_victory_win_count(), 4);

    packet->write_bool("respawn-options-inherit-respawn-time", get_respawn_options()->get_inherit_respawn_time_enabled());
    packet->write_bool("respawn-options-respawn-with-teammate", get_respawn_options()->get_respawn_with_teammate_enabled());
    packet->write_bool("respawn-options-respawn-at-location", get_respawn_options()->get_respawn_at_location_enabled());
    packet->write_bool("respawn-options-respawn-on-kills", get_respawn_options()->get_respawn_on_kills_enabled());
    packet->write_integer("respawn-options-lives-per-round", get_respawn_options()->get_lives_per_round(), 6);
    packet->write_integer("respawn-options-team-lives-per-round", get_respawn_options()->get_team_lives_per_round(), 7);
    packet->write_integer("respawn-options-respawn-time", get_respawn_options()->get_respawn_time_seconds(), 8);
    packet->write_integer("respawn-options-suicide-time", get_respawn_options()->get_suicide_penalty_seconds(), 8);
    packet->write_integer("respawn-options-betrayal-time", get_respawn_options()->get_betrayal_penalty_seconds(), 8);
    packet->write_integer("respawn-options-respawn-growth-time", get_respawn_options()->get_respawn_growth_seconds(), 4);
    packet->write_integer("respawn-options-player-options-duration", get_respawn_options()->get_respawn_player_traits_duration_seconds(), 6);
    get_respawn_options()->get_respawn_player_traits()->encode(packet);

    packet->write_bool("social-options-observers", get_social_options()->get_observers_enabled());
    packet->write_integer("social-options-team-changing", get_social_options()->get_team_changing_setting(), 2);
    packet->write_bool("social-options-friendly-fire", get_social_options()->get_friendly_fire_enabled());
    packet->write_bool("social-options-betrayal-booting", get_social_options()->get_betrayal_booting_enabled());
    packet->write_bool("social-options-enemy-voice", get_social_options()->get_enemy_voice_enabled());
    packet->write_bool("social-options-open-channel-voice", get_social_options()->get_open_channel_voice_enabled());
    packet->write_bool("social-options-dead-player-voice", get_social_options()->get_dead_player_voice_enabled());

    packet->write_bool("map-override-grenades-on-map", get_map_override_options()->get_grenades_on_map_enabled());
    packet->write_bool("map-override-indestructible-vehicles", get_map_override_options()->get_indestructible_vehicles_enabled());
    get_map_override_options()->get_base_player_traits()->encode(packet);
    packet->write_signed_integer("map-override-weapon-set", get_map_override_options()->get_weapon_set_absolute_index(), 8);
    packet->write_signed_integer("map-override-vehicle-set", get_map_override_options()->get_vehicle_set_absolute_index(), 8);
    get_map_override_options()->get_red_powerup_traits()->encode(packet);
    get_map_override_options()->get_blue_powerup_traits()->encode(packet);
    get_map_override_options()->get_yellow_powerup_traits()->encode(packet);
    packet->write_integer("map-override-red-powerup-duration", get_map_override_options()->get_red_powerup_duration_seconds(), 7);
    packet->write_integer("map-override-blue-powerup-duration", get_map_override_options()->get_blue_powerup_duration_seconds(), 7);
    packet->write_integer("map-override-yellow-powerup-duration", get_map_override_options()->get_yellow_powerup_duration_seconds(), 7);

    packet->write_integer("team-scoring-method", get_team_scoring_method(), 3);
}

// @address 0x2685a8 ?set@c_game_engine_oddball_variant@@QAAXPBV1@@Z
void c_game_engine_oddball_variant::set(const c_game_engine_oddball_variant *variant)
{
    c_game_engine_base_variant::set(variant);
    set_auto_ball_pickup(variant->get_auto_ball_pickup());
    set_ball_effect_enabled(variant->get_ball_effect_enabled());
    set_score_to_win(variant->get_score_to_win());
    set_carrying_points(variant->get_carrying_points());
    set_kill_points(variant->get_kill_points());
    set_ball_kill_points(variant->get_ball_kill_points());
    set_carrier_kill_points(variant->get_carrier_kill_points());
    set_ball_count(variant->get_ball_count());
    set_ball_spawn_delay(variant->get_ball_spawn_delay());
    set_ball_inactive_respawn_delay(variant->get_ball_inactive_respawn_delay());
    set_carrier_traits(variant->get_carrier_traits(), true);
}

// @address 0x268540 ?validate@c_game_engine_oddball_variant@@UAAXXZ
void c_game_engine_oddball_variant::validate()
{
    c_game_engine_oddball_variant validated;
    validated.set(this);
    set(&validated);
}

void game_engine_slayer_variant_set_from_definition(
    oddball_unit_slayer_variant *variant,
    const oddball_unit_slayer_definition *definition)
{
    game_engine_base_variant_set(variant, definition, 2);
    variant->set_team_scoring(definition->team_scoring);
    variant->set_score_to_win(definition->score_to_win);
    variant->set_kill_points(definition->kill);
    variant->set_assist_points(definition->assist);
    variant->set_death_points(definition->death);
    variant->set_suicide_points(definition->suicide);
    variant->set_betrayal_points(definition->betrayal);
    variant->set_leader_killed_points(definition->leader_killed);
    variant->set_elimination_points(definition->elimination);
    variant->set_assassination_points(definition->assassination);
    variant->set_headshot_points(definition->headshot);
    variant->set_melee_points(definition->melee);
    variant->set_sticky_points(definition->sticky);
    variant->set_splatter_points(definition->splatter);
    variant->set_killing_spree_points(definition->killing_spree);
    variant->leader_traits.set(game_engine_settings_player_traits_get_by_name(definition->leader_traits), true);
}
