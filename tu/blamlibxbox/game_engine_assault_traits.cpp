// blamlibxbox/game_engine_assault_traits.obj: from the records 0x26bdf8 0x26c4c8 0x26c678 0x26c6d0 0x26c8b0 0x26ca60 0x26cb50
// @compiler xdk6130
// @flags /TP /O2 /Oc
#include "cseries/cseries_wrapped.h"
#include "game/player_traits.h"
#include <string.h>
#include "game/game_engine_group.h"
#include "memory/bitstream.h"
#include "game/game_engine_base_variant.h"
#include "game/game_engine_player_traits.h"
#include "game/game_engine_variant_definitions.h"
#include "game/game_engine_assault_variant.h"
#include "game/game_engine_traits.h"
// @address 0x26bdf8 game_engine_juggernaut_variant_set_from_definition
// @address 0x266b48 ?set@c_player_traits@@QAAXPBUs_game_engine_player_traits@@_N@Z
// @address 0x2bc148 ?game_engine_settings_player_traits_get_by_name@@YAPBUs_game_engine_player_traits@@J@Z
// @address 0x26c678 ?validate@c_game_engine_assault_variant@@UAAXXZ
// @address 0x26c8b0 ?set@c_game_engine_assault_variant@@QAAXPBV1@@Z
// @address 0x26ca60 ?encode@c_game_engine_assault_variant@@UBAXPAVc_bitstream@@@Z
// @address 0x266a50 ?set@c_player_traits@@QAAXPBV1@_N@Z

struct s_assault_unit_definition
{
    s_game_engine_base_variant_definition base;
    unsigned long flags;
    short initial_juggernaut;
    short next_juggernaut;
    short zone_movement;
    short zone_order;
    short score_to_win_round;
    signed char kill_points;
    signed char juggernaut_kill_points;
    signed char kill_as_juggernaut_points;
    signed char destination_arrival_points;
    signed char suicide_points;
    signed char betrayal_points;
    unsigned char juggernaut_delay;
    long juggernaut_traits;
};

struct s_assault_unit_variant : c_game_engine_base_variant
{
    char m_base_padding[0x1b0 - sizeof(c_game_engine_base_variant)];
    short m_score_to_win;
    unsigned short m_pad1b2;
    char m_initial_juggernaut;
    char m_next_juggernaut;
    unsigned char m_flags;
    char m_zone_movement;
    char m_zone_order;
    signed char m_kill_points;
    signed char m_juggernaut_kill_points;
    signed char m_kill_as_juggernaut_points;
    signed char m_destination_arrival_points;
    signed char m_suicide_points;
    signed char m_betrayal_points;
    unsigned char m_juggernaut_delay;
    c_player_traits m_juggernaut_traits;
};

struct s_game_engine_assault_variant_definition
{
    s_game_engine_base_variant_definition base;
    unsigned long flags;
    short respawn;
    short game_type;
    short enemy_bomb_waypoint;
    short sudden_death_time;
    short score_to_win;
    short bomb_reset_time;
    short bomb_arming_time;
    short bomb_disarming_time;
    short bomb_fuse_time;
    long carrier_traits;
    long arming_traits;
};

static __forceinline void assault_traits_enable_teams(unsigned char *flags)
{
    *flags |= 1;
}

static __forceinline void assault_traits_initialize_shield_vitality(c_player_trait_shield_vitality *traits)
{
    traits->m_damage_resistance = _damage_resistance_percentage_setting_unchanged;
    traits->m_shield_recharge_rate = _shield_recharge_rate_percentage_setting_unchanged;
    traits->m_vampirism = _vampirism_percentage_setting_unchanged;
    traits->m_headshot_immunity = _headshot_immunity_setting_unchanged;
    traits->m_shield_multiplier = _shield_multiplier_setting_unchanged;
    memset(traits->m_padding, 0, sizeof(traits->m_padding));
}

static __forceinline void assault_traits_initialize_weapon(c_player_trait_weapons *traits)
{
    traits->m_damage_modifier = _damage_modifier_percentage_setting_unchanged;
    traits->m_initial_primary_weapon = -2;
    traits->m_initial_secondary_weapon = -2;
    traits->m_grenade_count = _grenade_count_setting_unchanged;
    traits->m_recharging_grenades = _recharging_grenades_setting_unchanged;
    traits->m_infinite_ammo = _infinite_ammo_setting_unchanged;
    traits->m_weapon_pickup = _weapon_pickup_setting_unchanged;
}

static __forceinline void assault_traits_initialize_movement(c_player_trait_movement *traits)
{
    traits->m_speed = _player_speed_setting_unchanged;
    traits->m_gravity = _player_gravity_setting_unchanged;
    traits->m_vehicle_usage = _vehicle_usage_setting_unchanged;
    traits->m_padding = 0;
}

static __forceinline void assault_traits_initialize_appearance(c_player_trait_appearance *traits)
{
    traits->m_active_camo = _active_camo_setting_unchanged;
    traits->m_waypoint = _waypoint_setting_unchanged;
    traits->m_aura = _aura_setting_unchanged;
    traits->m_forced_change_color = _forced_change_color_setting_unchanged;
}

static __forceinline void assault_traits_initialize_sensor(c_player_trait_sensors *traits)
{
    traits->m_motion_tracker = _motion_tracker_setting_unchanged;
    traits->m_motion_tracker_range = _motion_tracker_range_setting_unchanged;
}

static __forceinline void assault_traits_initialize_player_traits(c_player_traits *traits)
{
    assault_traits_initialize_shield_vitality(&traits->m_shield_vitality_traits);
    assault_traits_initialize_weapon(&traits->m_weapon_traits);
    assault_traits_initialize_movement(&traits->m_movement_traits);
    assault_traits_initialize_appearance(&traits->m_appearance_traits);
    assault_traits_initialize_sensor(&traits->m_sensor_traits);
}

// @address 0x26c4c8 ?initialize@c_game_engine_assault_variant@@UAAXXZ
void c_game_engine_assault_variant::initialize()
{
    c_game_engine_base_variant::initialize();
    assault_traits_enable_teams(&m_miscellaneous_options.m_flags.m_storage);
    m_assault_flags.clear();
    m_assault_flags.set(0, false);
    m_game_type = _assault_game_type_default;
    m_respawn = _assault_respawn_default;
    m_enemy_bomb_waypoint = _assault_enemy_bomb_waypoint_default;
    m_score_to_win = 3;
    m_sudden_death_time = 0;
    m_bomb_arming_time = 5;
    m_bomb_disarming_time = 5;
    m_bomb_fuse_time = 5;
    m_bomb_reset_time = 30;
    assault_traits_initialize_player_traits(&m_carrier_traits);
    assault_traits_initialize_player_traits(&m_arming_traits);
}

// @address 0x26c678 ?validate@c_game_engine_assault_variant@@UAAXXZ
void c_game_engine_assault_variant::validate()
{
    c_game_engine_assault_variant local;
    local.set(this);
    set(&local);
}

// @address 0x26c8b0 ?set@c_game_engine_assault_variant@@QAAXPBV1@@Z
void c_game_engine_assault_variant::set(const c_game_engine_assault_variant *variant)
{
    c_game_engine_base_variant::set(variant);
    m_miscellaneous_options.m_flags.m_storage |= 1;
    set_game_type(variant->m_game_type);
    set_respawn(variant->m_respawn);
    set_enemy_bomb_waypoint(variant->m_enemy_bomb_waypoint);
    set_reset_bomb_on_disarm(variant->m_assault_flags.test(0));
    set_score_to_win(variant->m_score_to_win);
    set_sudden_death_time(variant->m_sudden_death_time);
    set_bomb_arming_time(variant->m_bomb_arming_time);
    set_bomb_disarming_time(variant->m_bomb_disarming_time);
    set_bomb_fuse_time(variant->m_bomb_fuse_time);
    set_bomb_reset_time(variant->m_bomb_reset_time);
    set_carrier_traits(variant->get_carrier_traits(), true);
    set_arming_traits(variant->get_arming_traits(), true);
}

// @address 0x26ca60 ?encode@c_game_engine_assault_variant@@UBAXPAVc_bitstream@@@Z
void c_game_engine_assault_variant::encode(c_bitstream *packet) const
{
    c_game_engine_base_variant::encode(packet);
    packet->write_bool("assault-reset-bomb-on-disarm", m_assault_flags.test(0));
    packet->write_integer("assault-game-type", m_game_type, 2);
    packet->write_integer("assault-respawn", m_respawn, 3);
    packet->write_integer("assault-enemy-bomb-waypoint", m_enemy_bomb_waypoint, 3);
    packet->write_integer("assault-score-to-win", m_score_to_win, 6);
    packet->write_signed_integer("assault-sudden-death-time", m_sudden_death_time, 9);
    packet->write_integer("assault-bomb-arming-time", m_bomb_arming_time, 5);
    packet->write_integer("assault-bomb-disarming-time", m_bomb_disarming_time, 5);
    packet->write_integer("assault-bomb-fuse-time", m_bomb_fuse_time, 5);
    packet->write_integer("assault-bomb-reset-time", m_bomb_reset_time, 6);
    m_carrier_traits.encode(packet);
    m_arming_traits.encode(packet);
}


// @address 0x26bdf8 game_engine_juggernaut_variant_set_from_definition
extern "C" void game_engine_juggernaut_variant_set_from_definition(s_assault_unit_variant *variant, const s_assault_unit_definition *definition)
{
    game_engine_base_variant_set((c_game_engine_base_variant *)variant, (const s_game_engine_base_variant_definition *)definition, _game_engine_juggernaut);
    variant->m_miscellaneous_options.m_flags.m_storage &= ~1;
    unsigned short *reserved = &variant->m_pad1b2;
    *reserved = 0;
    if (definition->flags & 1)
        variant->m_flags |= 1;
    else
        variant->m_flags &= ~1;
    if ((unsigned char)((definition->flags >> 1) & 1))
        variant->m_flags |= 2;
    else
        variant->m_flags &= ~2;
    if ((unsigned char)((definition->flags >> 2) & 1))
        variant->m_flags |= 4;
    else
        variant->m_flags &= ~4;
    short score_to_win_round = definition->score_to_win_round;
    if (score_to_win_round < 0 || score_to_win_round > 500)
        score_to_win_round = 15;
    variant->m_score_to_win = score_to_win_round;
    long initial_juggernaut = definition->initial_juggernaut;
    if ((unsigned long)initial_juggernaut <= 2)
        variant->m_initial_juggernaut = (char)initial_juggernaut;
    else
        variant->m_initial_juggernaut = 0;
    long next_juggernaut = definition->next_juggernaut;
    if ((unsigned long)next_juggernaut <= 3)
        variant->m_next_juggernaut = (char)next_juggernaut;
    else
        variant->m_next_juggernaut = 0;
    long zone_movement = definition->zone_movement;
    if ((unsigned long)zone_movement <= 10)
        variant->m_zone_movement = (char)zone_movement;
    else
        variant->m_zone_movement = 0;
    long zone_order = definition->zone_order;
    if ((unsigned long)zone_order <= 1)
        variant->m_zone_order = (char)zone_order;
    else
        variant->m_zone_order = 1;
    if (definition->kill_points >= -10 && definition->kill_points <= 10)
        variant->m_kill_points = definition->kill_points;
    else
        variant->m_kill_points = 0;
    if (definition->juggernaut_kill_points >= -10 && definition->juggernaut_kill_points <= 10)
        variant->m_juggernaut_kill_points = definition->juggernaut_kill_points;
    else
        variant->m_juggernaut_kill_points = 1;
    if (definition->kill_as_juggernaut_points >= -10 && definition->kill_as_juggernaut_points <= 10)
        variant->m_kill_as_juggernaut_points = definition->kill_as_juggernaut_points;
    else
        variant->m_kill_as_juggernaut_points = 1;
    if (definition->destination_arrival_points >= -10 && definition->destination_arrival_points <= 10)
        variant->m_destination_arrival_points = definition->destination_arrival_points;
    else
        variant->m_destination_arrival_points = 1;
    if (definition->suicide_points >= -10 && definition->suicide_points <= 10)
        variant->m_suicide_points = definition->suicide_points;
    else
        variant->m_suicide_points = 1;
    if (definition->betrayal_points >= -10 && definition->betrayal_points <= 10)
        variant->m_betrayal_points = definition->betrayal_points;
    else
        variant->m_betrayal_points = 1;
    if (definition->juggernaut_delay <= 10)
        variant->m_juggernaut_delay = definition->juggernaut_delay;
    else
        variant->m_juggernaut_delay = 0;
    variant->m_juggernaut_traits.set(game_engine_settings_player_traits_get_by_name(definition->juggernaut_traits), true);
}

// @address 0x40700 k_game_variant_score_time_values
// @rdata 0x82040700..0x82040710
const short k_game_variant_score_time_values[] = { 0, 15, 30, 60, 120, 300, -1, 0 };
// @rdata-end

static __forceinline short assault_traits_lookup_sudden_death_time(short index)
{
    return k_game_variant_score_time_values[index];
}

// @address 0x26c6d0 game_engine_assault_variant_set_from_definition
extern "C" void game_engine_assault_variant_set_from_definition(
    c_game_engine_assault_variant *variant, const s_game_engine_assault_variant_definition *definition)
{
    game_engine_base_variant_set((c_game_engine_base_variant *)variant, (const s_game_engine_base_variant_definition *)definition, _game_engine_assault);
    variant->m_miscellaneous_options.m_flags.m_storage |= 1;
    variant->set_game_type(definition->game_type);
    variant->set_respawn(definition->respawn);
    variant->set_enemy_bomb_waypoint(definition->enemy_bomb_waypoint);
    if ((definition->flags & 1) != 0)
        variant->m_assault_flags.m_storage |= 1;
    else
        variant->m_assault_flags.m_storage &= ~1;
    short score_to_win = definition->score_to_win;
    if (score_to_win < 0 || score_to_win > 50)
        variant->m_score_to_win = 3;
    else
        variant->m_score_to_win = score_to_win;
    short sudden_death_time = assault_traits_lookup_sudden_death_time(definition->sudden_death_time);
    if (sudden_death_time < -1 || sudden_death_time > 300)
        variant->m_sudden_death_time = 0;
    else
        variant->m_sudden_death_time = sudden_death_time;
    short bomb_arming_time = definition->bomb_arming_time;
    if (bomb_arming_time < 0 || bomb_arming_time > 30)
        variant->m_bomb_arming_time = 5;
    else
        variant->m_bomb_arming_time = bomb_arming_time;
    short bomb_disarming_time = definition->bomb_disarming_time;
    if (bomb_disarming_time < 0 || bomb_disarming_time > 30)
        variant->m_bomb_disarming_time = 5;
    else
        variant->m_bomb_disarming_time = bomb_disarming_time;
    short bomb_fuse_time = definition->bomb_fuse_time;
    if (bomb_fuse_time < 0 || bomb_fuse_time > 30)
        variant->m_bomb_fuse_time = 5;
    else
        variant->m_bomb_fuse_time = bomb_fuse_time;
    short bomb_reset_time = definition->bomb_reset_time;
    if (bomb_reset_time < 0 || bomb_reset_time > 60)
        variant->m_bomb_reset_time = 30;
    else
        variant->m_bomb_reset_time = bomb_reset_time;
    variant->m_carrier_traits.set(game_engine_settings_player_traits_get_by_name(definition->carrier_traits), true);
    variant->m_arming_traits.set(game_engine_settings_player_traits_get_by_name(definition->arming_traits), true);
}

// @address 0x26cb50 ?decode@c_game_engine_assault_variant@@UAAXPAVc_bitstream@@@Z
void c_game_engine_assault_variant::decode(c_bitstream *packet)
{
    initialize();
    c_game_engine_base_variant::decode(packet);
    set_reset_bomb_on_disarm(packet->read_bool("assault-reset-bomb-on-disarm"));
    set_game_type(packet->read_integer("assault-game-type", 2));
    set_respawn(packet->read_integer("assault-respawn", 3));
    unsigned long enemy_bomb_waypoint = packet->read_integer("assault-enemy-bomb-waypoint", 3);
    if (enemy_bomb_waypoint <= 3)
        m_enemy_bomb_waypoint = (e_assault_enemy_bomb_waypoint)(short)enemy_bomb_waypoint;
    else
        m_enemy_bomb_waypoint = _assault_enemy_bomb_waypoint_default;
    set_score_to_win((short)packet->read_integer("assault-score-to-win", 6));
    set_sudden_death_time((short)packet->read_signed_integer("assault-sudden-death-time", 9));
    short bomb_arming_time = (short)packet->read_integer("assault-bomb-arming-time", 5);
    if (bomb_arming_time >= 0)
    {
        if (bomb_arming_time <= 30)
            m_bomb_arming_time = bomb_arming_time;
        else
            m_bomb_arming_time = 5;
    }
    else
        m_bomb_arming_time = 5;
    short bomb_disarming_time = (short)packet->read_integer("assault-bomb-disarming-time", 5);
    if (bomb_disarming_time >= 0)
    {
        if (bomb_disarming_time <= 30)
            m_bomb_disarming_time = bomb_disarming_time;
        else
            m_bomb_disarming_time = 5;
    }
    else
        m_bomb_disarming_time = 5;
    set_bomb_fuse_time((short)packet->read_integer("assault-bomb-fuse-time", 5));
    set_bomb_reset_time((short)packet->read_integer("assault-bomb-reset-time", 6));
    m_carrier_traits.decode(packet);
    m_arming_traits.decode(packet);
}
