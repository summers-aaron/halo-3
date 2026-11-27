// blamlibxbox/game_engine_vip_traits.obj: from the records 0x26a928 0x26acd8 0x26b508 0x26b668
// @flags /TP /O2 /Oc
#include "cseries/cseries_wrapped.h"
#include <string.h>
#include "memory/bitstream.h"
#include "game/player_traits.h"
#include <stdlib.h>
#include "game/game_engine_group.h"
#include "game/game_engine_traits.h"
#include "game/game_engine_variant_definitions.h"
#include "game/game_engine_vip_variant.h"
#include "game/game_engine_territories_variant.h"
#include "game/game_engine_player_traits.h"
// @address 0x221480 ?write_value_internal@c_bitstream@@QAAXKJ@Z
// @address 0x221398 ?write_bit_internal@c_bitstream@@AAAX_N@Z
// @address 0x264e28 ?encode@c_game_engine_base_variant@@UBAXPAVc_bitstream@@@Z
// @address 0x266df0 ?encode@c_player_traits@@QBAXPAVc_bitstream@@@Z
// @address 0x221648 ?read_bit_internal@c_bitstream@@AAA_NXZ
// @address 0x221768 ?read_value_internal@c_bitstream@@AAAKJ@Z
// @address 0x266ea8 ?decode@c_player_traits@@QAAXPAVc_bitstream@@@Z
// @address 0x265048 ?decode@c_game_engine_base_variant@@UAAXPAVc_bitstream@@@Z
// @address 0x2bc148 ?game_engine_settings_player_traits_get_by_name@@YAPBUs_game_engine_player_traits@@J@Z
// @address 0x266b48 ?set@c_player_traits@@QAAXPBUs_game_engine_player_traits@@_N@Z
// @address 0x264d40 ?initialize@c_game_engine_base_variant@@UAAXXZ

struct s_game_engine_territories_variant_definition
{
    s_game_engine_base_variant_definition base;
    unsigned long flags;
    short selection;
    short movement;
    short order;
    long traits_a;
    long traits_b;
};

// @address 0x26b508 ?encode@c_game_engine_vip_variant@@UBAXPAVc_bitstream@@@Z
void c_game_engine_vip_variant::encode(c_bitstream *packet) const
{
    c_game_engine_base_variant::encode(packet);
    packet->write_bool("vip-single-vip", m_variant_flags.m_storage & 1);
    packet->write_bool("vip-destination-zones-enabled", _rotl(m_variant_flags.m_storage, 31) & 1);
    packet->write_bool("vip-end-round-on-vip-death", _rotl(m_variant_flags.m_storage, 30) & 1);
    packet->write_integer("vip-score-to-win-round", get_score_to_win_round(), 10);
    packet->write_integer("vip-kill-points", m_kill_points & 31, 5);
    packet->write_integer("vip-takedown-points", m_takedown_points & 31, 5);
    packet->write_integer("vip-kill-as-vip-points", m_kill_as_vip_points & 31, 5);
    packet->write_integer("vip-vip-death-points", m_vip_death_points & 31, 5);
    packet->write_integer("vip-destination-arrival-points", m_destination_arrival_points & 31, 5);
    packet->write_integer("vip-suicide-points", m_suicide_points & 31, 5);
    packet->write_integer("vip-vip-suicide-points", m_vip_suicide_points & 31, 5);
    packet->write_integer("vip-betrayal-points", m_betrayal_points & 31, 5);
    packet->write_integer("vip-vip-selection", m_vip_selection, 2);
    packet->write_integer("vip-zone-movement", m_zone_movement, 4);
    packet->write_integer("vip-zone-order", m_zone_order, 1);
    packet->write_integer("vip-influence-radius", (short)m_influence_radius, 6);
    m_vip_team_traits.encode(packet);
    m_vip_influence_traits.encode(packet);
    m_vip_traits.encode(packet);
}

static __forceinline void game_engine_vip_traits_copy_limited(
    signed char *destination, signed char value, signed char minimum, signed char maximum, signed char fallback)
{
    if (value >= minimum && value <= maximum)
        *destination = value;
    else
        *destination = fallback;
}

static __forceinline void game_engine_vip_traits_copy_limited(unsigned char *destination, unsigned long value, unsigned long limit, unsigned char fallback)
{
    if (value <= limit)
        *destination = value;
    else
        *destination = fallback;
}

static __forceinline void game_engine_vip_traits_copy_limited(
    c_enum_no_init<e_vip_zone_order_setting, unsigned char, _vip_zone_order_random, k_vip_zone_order_settings_count> *destination,
    unsigned long value,
    unsigned long limit,
    unsigned char fallback)
{
    if (value <= limit)
        *destination = (e_vip_zone_order_setting)value;
    else
        *destination = (e_vip_zone_order_setting)fallback;
}

static __forceinline void game_engine_vip_traits_copy_limited(unsigned short *destination, short value, short minimum, short maximum, short fallback)
{
    if (value >= minimum && value <= maximum)
        *destination = (unsigned short)value;
    else
        *destination = (unsigned short)fallback;
}

static __forceinline void game_engine_vip_traits_decode_signed(c_bitstream *packet, const char *name, signed char *destination)
{
    long value = (long)packet->read_integer(name, 5);
    if ((unsigned long)(value & 16) != 0)
        value |= -32;
    value = (signed char)value;
    game_engine_vip_traits_copy_limited(destination, value, -10, 10, 0);
}

// @address 0x26b668 ?decode@c_game_engine_vip_variant@@UAAXPAVc_bitstream@@@Z
void c_game_engine_vip_variant::decode(c_bitstream *packet)
{
    initialize();
    c_game_engine_base_variant::decode(packet);
    m_variant_flags.set(_vip_variant_single_vip, packet->read_bool("vip-single-vip"));
    m_variant_flags.set(_vip_variant_destination_zones_enabled, packet->read_bool("vip-destination-zones-enabled"));
    m_variant_flags.set(_vip_variant_end_round_on_vip_death, packet->read_bool("vip-end-round-on-vip-death"));

    unsigned short score = (unsigned short)packet->read_integer("vip-score-to-win-round", 10);
    if ((short)score < 0 || (short)score > 500)
        score = 15;
    m_score_to_win_round = (short)score;

    game_engine_vip_traits_decode_signed(packet, "vip-kill-points", &m_kill_points);
    game_engine_vip_traits_decode_signed(packet, "vip-takedown-points", &m_takedown_points);
    game_engine_vip_traits_decode_signed(packet, "vip-kill-as-vip-points", &m_kill_as_vip_points);
    game_engine_vip_traits_decode_signed(packet, "vip-vip-death-points", &m_vip_death_points);
    game_engine_vip_traits_decode_signed(packet, "vip-destination-arrival-points", &m_destination_arrival_points);
    game_engine_vip_traits_decode_signed(packet, "vip-suicide-points", &m_suicide_points);
    game_engine_vip_traits_decode_signed(packet, "vip-vip-suicide-points", &m_vip_suicide_points);
    game_engine_vip_traits_decode_signed(packet, "vip-betrayal-points", &m_betrayal_points);

    game_engine_vip_traits_copy_limited(&m_vip_selection, packet->read_integer("vip-vip-selection", 2), 3, 0);
    game_engine_vip_traits_copy_limited(&m_zone_movement, packet->read_integer("vip-zone-movement", 4), 10, 0);
    game_engine_vip_traits_copy_limited(&m_zone_order, packet->read_integer("vip-zone-order", 1), 1, 1);
    game_engine_vip_traits_copy_limited(&m_influence_radius, (short)packet->read_integer("vip-influence-radius", 6), 0, 50, 0);

    m_vip_team_traits.decode(packet);
    m_vip_influence_traits.decode(packet);
    m_vip_traits.decode(packet);
}

static __forceinline long game_engine_vip_traits_get_movement(short index)
{
    const long values[] = {0, 5, 10, 15, 20, 30, 45, 60, 120};
    return values[index];
}

static __forceinline long game_engine_vip_traits_get_order(short index)
{
    const long values[] = {0, 15, 30, 60, 120, 300, -1};
    return values[index];
}

static __forceinline void game_engine_vip_traits_set_movement(short *movement, short value)
{
    if (value >= 0 && value <= 120)
        *movement = value;
    else
        *movement = 30;
}

static __forceinline void game_engine_vip_traits_set_order(short *order, short value)
{
    if (value >= -1 && value <= 300)
        *order = value;
    else
        *order = -1;
}

// @address 0x26a928 ?set@c_game_engine_territories_variant@@QAAXPBUs_game_engine_territories_variant_definition@@@Z
void c_game_engine_territories_variant::set(const s_game_engine_territories_variant_definition *definition)
{
    game_engine_base_variant_set((c_game_engine_base_variant *)this, (const s_game_engine_base_variant_definition *)definition, _game_engine_territories);
    m_miscellaneous_options.m_flags.m_storage = m_miscellaneous_options.m_flags.m_storage | 1;
    set_one_sided((definition->flags & 1) != 0);
    set_lock_after_first_capture(((definition->flags >> 1) & 1) != 0);
    long respawn_on_capture = definition->selection;
    if ((unsigned long)respawn_on_capture <= _territories_respawn_on_capture_all)
        m_respawn_on_capture = (e_territories_respawn_on_capture_settings)respawn_on_capture;
    else
        m_respawn_on_capture = _territories_respawn_on_capture_disabled;
    game_engine_vip_traits_set_movement(&m_capture_time, (short)game_engine_vip_traits_get_movement(definition->movement));
    game_engine_vip_traits_set_order(&sudden_death_time, (short)game_engine_vip_traits_get_order(definition->order));
    m_defender_traits.set(game_engine_settings_player_traits_get_by_name(definition->traits_a), true);
    m_attacker_traits.set(game_engine_settings_player_traits_get_by_name(definition->traits_b), true);
}

static __forceinline void game_engine_vip_traits_initialize_shield_vitality(c_player_trait_shield_vitality *traits)
{
    traits->m_damage_resistance = _damage_resistance_percentage_setting_unchanged;
    traits->m_shield_recharge_rate = _shield_recharge_rate_percentage_setting_unchanged;
    traits->m_vampirism = _vampirism_percentage_setting_unchanged;
    traits->m_headshot_immunity = _headshot_immunity_setting_unchanged;
    traits->m_shield_multiplier = _shield_multiplier_setting_unchanged;
    memset(traits->m_padding, 0, sizeof(traits->m_padding));
}

static __forceinline void game_engine_vip_traits_initialize_weapons(c_player_trait_weapons *traits)
{
    traits->m_damage_modifier = _damage_modifier_percentage_setting_unchanged;
    traits->m_initial_primary_weapon = -2;
    traits->m_initial_secondary_weapon = -2;
    traits->m_grenade_count = _grenade_count_setting_unchanged;
    traits->m_recharging_grenades = _recharging_grenades_setting_unchanged;
    traits->m_infinite_ammo = _infinite_ammo_setting_unchanged;
    traits->m_weapon_pickup = _weapon_pickup_setting_unchanged;
}

static __forceinline void game_engine_vip_traits_initialize_movement(c_player_trait_movement *traits)
{
    traits->m_speed = _player_speed_setting_unchanged;
    traits->m_gravity = _player_gravity_setting_unchanged;
    traits->m_vehicle_usage = _vehicle_usage_setting_unchanged;
    traits->m_padding = 0;
}

static __forceinline void game_engine_vip_traits_initialize_appearance(c_player_trait_appearance *traits)
{
    traits->m_active_camo = _active_camo_setting_unchanged;
    traits->m_waypoint = _waypoint_setting_unchanged;
    traits->m_aura = _aura_setting_unchanged;
    traits->m_forced_change_color = _forced_change_color_setting_unchanged;
}

static __forceinline void game_engine_vip_traits_initialize_sensors(c_player_trait_sensors *traits)
{
    traits->m_motion_tracker = _motion_tracker_setting_unchanged;
    traits->m_motion_tracker_range = _motion_tracker_range_setting_unchanged;
}

static __forceinline void game_engine_vip_traits_initialize_player(c_player_traits *traits)
{
    game_engine_vip_traits_initialize_shield_vitality(&traits->m_shield_vitality_traits);
    game_engine_vip_traits_initialize_weapons(&traits->m_weapon_traits);
    game_engine_vip_traits_initialize_movement(&traits->m_movement_traits);
    game_engine_vip_traits_initialize_appearance(&traits->m_appearance_traits);
    game_engine_vip_traits_initialize_sensors(&traits->m_sensor_traits);
}

static __forceinline void game_engine_vip_traits_set_teams(unsigned char *flags, bool enabled)
{
    if (enabled)
        *flags |= 1;
    else
        *flags &= ~1;
}

// @address 0x26acd8 ?initialize@c_game_engine_vip_variant@@UAAXXZ
void c_game_engine_vip_variant::initialize()
{
    c_game_engine_base_variant::initialize();
    game_engine_vip_traits_set_teams(&m_miscellaneous_options.m_flags.m_storage, true);
    m_variant_flags.m_storage = 0;
    m_variant_flags.set(_vip_variant_single_vip, false);
    m_variant_flags.set(_vip_variant_destination_zones_enabled, false);
    m_variant_flags.set(_vip_variant_end_round_on_vip_death, false);
    m_score_to_win_round = 15;
    m_kill_points = 0;
    m_takedown_points = 0;
    m_kill_as_vip_points = 0;
    m_vip_death_points = 0;
    m_destination_arrival_points = 0;
    m_suicide_points = 0;
    m_vip_suicide_points = 0;
    m_betrayal_points = 0;
    m_vip_selection = 0;
    m_zone_movement = 0;
    m_zone_order = _vip_zone_order_sequential;
    m_influence_radius = 0;
    game_engine_vip_traits_initialize_player(&m_vip_team_traits);
    game_engine_vip_traits_initialize_player(&m_vip_influence_traits);
    game_engine_vip_traits_initialize_player(&m_vip_traits);
}
