// blamlibxbox/game_engine_player_traits.obj: from the records 0xf0660 0xf0748 0x265a98 0x265bc8 0x265c30 0x265cf8 0x265e08 0x265ed8 0x266038 0x266110 0x266198 0x2662b8 0x2663c8 0x266410 0x266490 0x2664d8 0x266558 0x266610 0x266698 0x266780 0x266828 0x2668a0 0x266908 0x2669c0 0x266a50 0x266b48 0x266df0 0x266ea8 0x266f10 0x267048 0x2671c8 0x267298 0x267380 0x267488 0x267610 0x267708
// @flags /TP /O2 /Oc
#include "memory/datum_index.h"
#include "math/real_literal_pool.h"
#include "memory/data_types.h"
#include "cseries/cseries_wrapped.h"
#include "game/game_time_types.h"
#include "game/player_traits.h"
#include "memory/tls_globals.h"
#include "objects/object_header.h"
#include "units/unit_types.h"
#include <string.h>
#include "game/game_engine.h"
#include "simulation/game_interface/simulation_game_action.h"
#include "units/units.h"
#include "game/game_engine_player_traits.h"
#include "game/player_traits_source_types.h"
#include "game/game_engine_util.h"
#include "math/real_constant_pool.h"
#include "memory/bitstream.h"
#include "game/multiplayer_powerup_flavor_types.h"
#include "rasterizer/rasterizer_depth_bias.h"
#include "game/game_engine_scoring_globals.h"
#include "game/player_types.h"
#include "game/game_globals_storage.h"
#include "game/game_engine_default.h"
#include "game/game_engine_list.h"
#include "game/game_variant_inline.h"
#include "game/game_engine_get_inline.h"
// @address 0x336f0 literal_1000_0f
// @address 0x242d8 k_real_constant_pool_two
// @address 0x221480 ?write_value_internal@c_bitstream@@QAAXKJ@Z
// @address 0x221768 ?read_value_internal@c_bitstream@@AAAKJ@Z
// @address 0x266a50 ?set@c_player_traits@@QAAXPBV1@_N@Z
// @address 0x266b48 ?set@c_player_traits@@QAAXPBUs_game_engine_player_traits@@_N@Z
// @address 0x266df0 ?encode@c_player_traits@@QBAXPAVc_bitstream@@@Z
// @address 0x266ea8 ?decode@c_player_traits@@QAAXPAVc_bitstream@@@Z
// @address 0xf0660 ?get_speed@c_player_trait_movement@@QBAMXZ
// @address 0xf0748 ?get_gravity@c_player_trait_movement@@QBAMXZ
// @address 0x265a98 ?set@c_player_trait_shield_vitality@@QAAXPBV1@_N@Z
// @address 0x265cf8 ?get_damage_resistance_percentage@c_player_trait_shield_vitality@@QBAMXZ
// @address 0x265e08 ?get_shield_recharge_rate_percentage@c_player_trait_shield_vitality@@QBAMXZ
// @address 0x266410 ?set_initial_primary_weapon_absolute_index@c_player_trait_weapons@@QAAXD_N@Z
// @address 0x2664d8 ?set_initial_secondary_weapon_absolute_index@c_player_trait_weapons@@QAAXD_N@Z
// @address 0x265bc8 ?encode@c_player_trait_shield_vitality@@QBAXPAVc_bitstream@@@Z
// @address 0x265c30 ?decode@c_player_trait_shield_vitality@@QAAXPAVc_bitstream@@@Z
// @address 0x266038 ?set@c_player_trait_weapons@@QAAXPBUs_player_trait_weapons@@_N@Z
// @address 0x266110 ?encode@c_player_trait_weapons@@QBAXPAVc_bitstream@@@Z
// @address 0x266198 player_trait_weapons_decode
// @address 0x2663c8 ?set_initial_primary_weapon@c_player_trait_weapons@@QAAXJ_N@Z
// @address 0x266490 ?set_initial_secondary_weapon@c_player_trait_weapons@@QAAXJ_N@Z
// @address 0x266558 ?set@c_player_trait_movement@@QAAXPBV1@_N@Z
// @address 0x266610 ?decode@c_player_trait_movement@@QAAXPAVc_bitstream@@@Z
// @address 0x266698 ?set@c_player_trait_appearance@@QAAXPBV1@_N@Z
// @address 0x266780 ?decode@c_player_trait_appearance@@QAAXPAVc_bitstream@@@Z
// @address 0x266828 ?set@c_player_trait_sensors@@QAAXPBV1@_N@Z
// @address 0x2668a0 ?decode@c_player_trait_sensors@@QAAXPAVc_bitstream@@@Z
// @address 0x266908 ?get_motion_tracker_range@c_player_trait_sensors@@QBAMXZ
// @address 0x2669c0 ?initialize@c_player_traits@@QAAXXZ

 


// @address 0x24fce0 ?game_engine_get_multiplayer_weapon_selection_absolute_index@@YAFJ@Z


struct s_game_engine_player_traits;


float c_player_trait_movement::get_speed() const
{
#if defined(_XBOX) && !defined(__clang__)
    // match: 0xf0660: the multipliers are read off one k_real_constant_pool_three base register;
    // plain literals load a base per case
    switch ((unsigned char)m_speed) {
    case _player_speed_25_percent: return 0.25f;
    case _player_speed_50_percent: return k_real_constant_pool_three[-1349];
    case _player_speed_75_percent: return k_real_constant_pool_three[-1520];
    case _player_speed_90_percent: return k_real_constant_pool_three[2234];
    case _player_speed_110_percent: return k_real_constant_pool_three[-1912];
    case _player_speed_125_percent: return k_real_constant_pool_three[7119];
    case _player_speed_150_percent: return k_real_constant_pool_three[2156];
    case _player_speed_200_percent: return k_real_constant_pool_three[1];
    case _player_speed_300_percent: return k_real_constant_pool_three[0];
    default: return k_real_constant_pool_three[-2102];
    }
#else
    switch ((unsigned char)m_speed) {
    case _player_speed_25_percent: return 0.25f;
    case _player_speed_50_percent: return 0.5f;
    case _player_speed_75_percent: return 0.75f;
    case _player_speed_90_percent: return 0.9f;
    case _player_speed_110_percent: return 1.1f;
    case _player_speed_125_percent: return 1.25f;
    case _player_speed_150_percent: return 1.5f;
    case _player_speed_200_percent: return 2.0f;
    case _player_speed_300_percent: return 3.0f;
    default: return 1.0f;
    }
#endif
}

float c_player_trait_movement::get_gravity() const
{
#if defined(_XBOX) && !defined(__clang__)
  // match: 0xf0748: the multipliers are read off one k_real_constant_pool_two base register;
  // plain literals load a base per case
  switch ((unsigned char)m_gravity) {
  case _player_gravity_50_percent:
    return k_real_constant_pool_two[-1350];
  case _player_gravity_75_percent:
    return k_real_constant_pool_two[-1521];
  case _player_gravity_150_percent:
    return k_real_constant_pool_two[2155];
  case _player_gravity_200_percent:
    return k_real_constant_pool_two[0];
  default:
    return k_real_constant_pool_two[-2103];
  }
#else
  switch ((unsigned char)m_gravity) {
  case _player_gravity_50_percent:
    return 0.5f;
  case _player_gravity_75_percent:
    return 0.75f;
  case _player_gravity_150_percent:
    return 1.5f;
  case _player_gravity_200_percent:
    return 2.0f;
  default:
    return 1.0f;
  }
#endif
}

void c_player_trait_shield_vitality::set(const c_player_trait_shield_vitality *source, bool force)
{
    set_damage_resistance_percentage_setting(source->m_damage_resistance, force);
    set_shield_recharge_rate_percentage_setting(source->m_shield_recharge_rate, force);
    set_vampirism_percentage_setting(source->m_vampirism, force);
    set_shield_multiplier_setting(source->m_shield_multiplier, force);
    set_headshot_immunity_setting(source->m_headshot_immunity, force);
    memset(m_padding, 0, sizeof(m_padding));
}

// 0x265bc8
void c_player_trait_shield_vitality::encode(c_bitstream *packet) const
{
    packet->write_value_internal(get_damage_resistance_percentage_setting(), 4);
    packet->write_value_internal(get_shield_recharge_rate_percentage_setting(), 4);
    packet->write_value_internal(get_vampirism_percentage_setting(), 3);
    packet->write_value_internal(get_headshot_immunity_setting(), 2);
    packet->write_value_internal(get_shield_multiplier_setting(), 3);
}
// 0x265c30
void c_player_trait_shield_vitality::decode(c_bitstream *packet)
{
    set_damage_resistance_percentage_setting(packet->read_integer("player-traits-shield-vitality-damage-resistance", 4), true);
    set_shield_recharge_rate_percentage_setting(packet->read_integer("player-traits-shield-vitality-shield-recharge-rate", 4), true);
    set_vampirism_percentage_setting(packet->read_integer("player-traits-shield-vitality-vampirism", 3), true);
    set_headshot_immunity_setting(packet->read_integer("player-traits-shield-vitality-headshot-immunity", 2), true);
    set_shield_multiplier_setting(packet->read_integer("player-traits-shield-vitality-shield-multiplier", 3), true);
}
// match: 0x265cf8: named constants leave 10.0f and 20.0f as the float pool bases.
// @rdata 0x820221fc pooled
static const float k_damage_resistance_default_percentage = 1.0f;
// @rdata 0x820336f0 pooled
static const float k_damage_resistance_maximum_percentage = 1000.0f;
float c_player_trait_shield_vitality::get_damage_resistance_percentage() const
{
    switch ((unsigned char)m_damage_resistance) {
    case _damage_resistance_10_percent: return 0.1f;
    case _damage_resistance_50_percent: return 0.5f;
    case _damage_resistance_90_percent: return 0.9f;
    case _damage_resistance_110_percent: return 1.1f;
    case _damage_resistance_150_percent: return 1.5f;
    case _damage_resistance_200_percent: return 2.0f;
    case _damage_resistance_300_percent: return 3.0f;
    case _damage_resistance_500_percent: return 5.0f;
    case _damage_resistance_1000_percent: return 10.0f;
    case _damage_resistance_2000_percent: return 20.0f;
    case _damage_resistance_100000_percent: return k_damage_resistance_maximum_percentage;
    }
    return k_damage_resistance_default_percentage;
}

float c_player_trait_shield_vitality::get_shield_recharge_rate_percentage() const
{
    switch ((int)m_shield_recharge_rate - 1) {
    case _shield_recharge_rate_negative_25_percent - 1: return -0.25f;
    case _shield_recharge_rate_negative_10_percent - 1: return rasterizer_states_mode_5_slope_scale_bias;
    case _shield_recharge_rate_negative_5_percent - 1: return -0.05f;
#if defined(_XBOX) && !defined(__clang__)
    // match: 0x265e08: these multipliers are read off one k_real_constant_pool_two base register;
    // plain literals load a base per case
    case _shield_recharge_rate_zero - 1: return k_real_constant_pool_two[-1903];
    case _shield_recharge_rate_50_percent - 1: return k_real_constant_pool_two[-1350];
    case _shield_recharge_rate_90_percent - 1: return k_real_constant_pool_two[2233];
    case _shield_recharge_rate_100_percent - 1: default: return k_real_constant_pool_two[-2103];
    case _shield_recharge_rate_110_percent - 1: return k_real_constant_pool_two[-1913];
    case _shield_recharge_rate_200_percent - 1: return k_real_constant_pool_two[0];
#else
    case _shield_recharge_rate_zero - 1: return 0.0f;
    case _shield_recharge_rate_50_percent - 1: return 0.5f;
    case _shield_recharge_rate_90_percent - 1: return 0.9f;
    case _shield_recharge_rate_100_percent - 1: default: return 1.0f;
    case _shield_recharge_rate_110_percent - 1: return 1.1f;
    case _shield_recharge_rate_200_percent - 1: return 2.0f;
#endif
    }
}

// @address 0x265ed8 ?set@c_player_trait_weapons@@QAAXPBV1@_N@Z
void c_player_trait_weapons::set(const c_player_trait_weapons *source, bool force)
{
    set_damage_modifier_percentage_setting(source->m_damage_modifier, force);
    set_initial_primary_weapon_absolute_index(source->m_initial_primary_weapon, force);
    set_initial_secondary_weapon_absolute_index(source->m_initial_secondary_weapon, force);
    set_initial_grenade_count_setting(source->m_grenade_count, force);
    set_infinite_ammo_setting(source->m_infinite_ammo, force);
    set_recharging_grenades_setting(source->m_recharging_grenades, force);
    set_weapon_pickup_allowed_setting(source->m_weapon_pickup, force);
}

// 0x266038
void c_player_trait_weapons::set(const s_player_trait_weapons *traits, bool force)
{
    set_damage_modifier_percentage_setting(traits->damage_modifier, true);
    set_initial_primary_weapon_absolute_index((char)game_engine_get_multiplayer_weapon_selection_absolute_index(traits->initial_primary_weapon),
        true);
    set_initial_secondary_weapon_absolute_index((char)game_engine_get_multiplayer_weapon_selection_absolute_index(traits->initial_secondary_weapon),
        true);
    set_initial_grenade_count_setting(traits->grenade_count, true);
    set_infinite_ammo_setting(traits->infinite_ammo, true);
    set_recharging_grenades_setting(traits->recharging_grenades, true);
    set_weapon_pickup_allowed_setting(traits->weapon_pickup, true);
}
// 0x266110
void c_player_trait_weapons::encode(c_bitstream *packet) const
{
    packet->write_integer("player-trait-damage-modifier", m_damage_modifier, 4);
    packet->write_integer("player-trait-initial-primary-weapon", (unsigned char)m_initial_primary_weapon, 8);
    packet->write_integer("player-trait-initial-secondary-weapon", (unsigned char)m_initial_secondary_weapon, 8);
    packet->write_integer("player-trait-initial-grenade-count", m_grenade_count, 2);
    packet->write_integer("player-traits-infinite-ammo-setting", m_infinite_ammo, 2);
    packet->write_integer("player-traits-recharging-grenades", m_recharging_grenades, 2);
    packet->write_integer("player-traits-weapon-pickup-allowed", m_weapon_pickup, 2);
}
// @address 0x266198 player_trait_weapons_decode
extern "C" void player_trait_weapons_decode(c_player_trait_weapons *traits, c_bitstream *packet)
{
    traits->set_damage_modifier_percentage_setting(packet->read_integer("player-traits-weapons-damage-modifier", 4), true);
    long primary_weapon = packet->read_integer("player-traits-weapons-initial-primary-weapon", 8);
    if (primary_weapon & 0x80u) primary_weapon |= -0x100;
    traits->set_initial_primary_weapon_absolute_index((char)primary_weapon, true);
    long secondary_weapon = packet->read_integer("player-traits-weapons-initial-secondary-weapon", 8);
    if (secondary_weapon & 0x80u) secondary_weapon |= -0x100;
    traits->set_initial_secondary_weapon_absolute_index((char)secondary_weapon, true);
    traits->set_initial_grenade_count_setting(packet->read_integer("player-traits-weapons-grenade-count", 2), true);
    traits->set_infinite_ammo_setting(packet->read_integer("player-traits-weapons-infinite-ammo", 2), true);
    traits->set_recharging_grenades_setting(packet->read_integer("player-traits-weapons-recharging-grenades", 2), true);
    traits->set_weapon_pickup_allowed_setting(packet->read_integer("player-traits-weapons-weapon-pickup", 2), true);
}
float c_player_trait_weapons::get_damage_modifier_percentage() const
{
    int setting = m_damage_modifier;
    switch (setting - 1) {
#if defined(_XBOX) && !defined(__clang__)
    // match: 0x2662b8: the multipliers are read off the k_real_constant_pool_three and literal_1000_0f base
    // registers; plain literals load a base per case
    case _damage_modifier_zero - 1: return k_real_constant_pool_three[-1902];
    case _damage_modifier_25_percent - 1: return literal_1000_0f[308];
    case _damage_modifier_50_percent - 1: return k_real_constant_pool_three[-1349];
    case _damage_modifier_75_percent - 1: return k_real_constant_pool_three[-1520];
    case _damage_modifier_90_percent - 1: return k_real_constant_pool_three[2234];
    case _damage_modifier_110_percent - 1: return k_real_constant_pool_three[-1912];
    case _damage_modifier_125_percent - 1: return k_real_constant_pool_three[7119];
    case _damage_modifier_150_percent - 1: return k_real_constant_pool_three[2156];
    case _damage_modifier_200_percent - 1: return k_real_constant_pool_three[1];
    case _damage_modifier_300_percent - 1: return k_real_constant_pool_three[0];
    case _damage_modifier_100000_percent - 1: return 1000.0f;
    default: return k_real_constant_pool_three[-2102];
#else
    case _damage_modifier_zero - 1: return 0.0f;
    case _damage_modifier_25_percent - 1: return 0.25f;
    case _damage_modifier_50_percent - 1: return 0.5f;
    case _damage_modifier_75_percent - 1: return 0.75f;
    case _damage_modifier_90_percent - 1: return 0.9f;
    case _damage_modifier_110_percent - 1: return 1.1f;
    case _damage_modifier_125_percent - 1: return 1.25f;
    case _damage_modifier_150_percent - 1: return 1.5f;
    case _damage_modifier_200_percent - 1: return 2.0f;
    case _damage_modifier_300_percent - 1: return 3.0f;
    case _damage_modifier_100000_percent - 1: return 1000.0f;
    default: return 1.0f;
#endif
    }
}
// 0x2663c8
void c_player_trait_weapons::set_initial_primary_weapon(long name, bool force)
{
    short absolute_index = game_engine_get_multiplayer_weapon_selection_absolute_index(name);
    set_initial_primary_weapon_absolute_index((char)absolute_index, true);
}
void c_player_trait_weapons::set_initial_primary_weapon_absolute_index(char index, bool force)
{
    signed char v = index;
    bool nonneg = v >= 0;
    bool special = v == -1 || v == -3 || v == -2;
    if (nonneg | special) {
        if (!force) {
            if (v == -2) return;
        }
        m_initial_primary_weapon = index;
        return;
    }
    if (force == 0) return;
    m_initial_primary_weapon = -2;
}

// 0x266490
void c_player_trait_weapons::set_initial_secondary_weapon(long name, bool force)
{
    short absolute_index = game_engine_get_multiplayer_weapon_selection_absolute_index(name);
    set_initial_secondary_weapon_absolute_index((char)absolute_index, true);
}
void c_player_trait_weapons::set_initial_secondary_weapon_absolute_index(char index, bool force)
{
    signed char v = index;
    bool nonneg = v >= 0;
    bool special = v == -1 || v == -3 || v == -2;
    if (nonneg | special) {
        if (!force) {
            if (v == -2) return;
        }
        m_initial_secondary_weapon = index;
        return;
    }
    if (force == 0) return;
    m_initial_secondary_weapon = -2;
}

// 0x266558
void c_player_trait_movement::set(const c_player_trait_movement *source, bool force)
{
    set_speed_setting(source->m_speed, force);
    set_gravity_setting(source->m_gravity, force);
    set_vehicle_usage_setting(source->m_vehicle_usage, force);
    m_padding = 0;
}
// 0x266610
void c_player_trait_movement::decode(c_bitstream *packet)
{
    set_speed_setting(packet->read_integer("player-traits-movement-speed", 4), true);
    set_gravity_setting(packet->read_integer("player-traits-movement-gravity", 3), true);
    set_vehicle_usage_setting(packet->read_integer("player-traits-movement-vehicle-usage", 2), true);
}
// 0x266698
void c_player_trait_appearance::set(const c_player_trait_appearance *source, bool force)
{
    set_active_camo_setting(source->m_active_camo, force);
    set_waypoint_setting(source->m_waypoint, force);
    set_aura_setting(source->m_aura, force);
    set_forced_change_color_setting(source->m_forced_change_color, force);
}
// 0x266780
void c_player_trait_appearance::decode(c_bitstream *packet)
{
    set_active_camo_setting(packet->read_integer("player-traits-appearance-active-camo", 3), true);
    set_waypoint_setting(packet->read_integer("player-traits-appearance-waypoint", 2), true);
    set_aura_setting(packet->read_integer("player-traits-appearance-aura", 3), true);
    set_forced_change_color_setting(packet->read_integer("player-traits-appearance-forced-change-color", 4), true);
}
// 0x266828
void c_player_trait_sensors::set(const c_player_trait_sensors *source, bool force)
{
    set_motion_tracker_setting(source->m_motion_tracker, force);
    set_motion_tracker_range_setting(source->m_motion_tracker_range, force);
}
// 0x2668a0
void c_player_trait_sensors::decode(c_bitstream *packet)
{
    set_motion_tracker_setting(packet->read_integer("player-traits-sensors-motion-tracker", 3), true);
    set_motion_tracker_range_setting(packet->read_integer("player-traits-sensors-motion-tracker-range", 3), true);
}
// 0x266908
float c_player_trait_sensors::get_motion_tracker_range() const
{
    switch (m_motion_tracker_range) {
    case _motion_tracker_range_10_meters: return 10.0f;
    case _motion_tracker_range_15_meters: return 15.0f;
    case _motion_tracker_range_25_meters: return 25.0f;
    case _motion_tracker_range_50_meters: return 50.0f;
    case _motion_tracker_range_75_meters: return 75.0f;
    case _motion_tracker_range_100_meters: return 100.0f;
    case _motion_tracker_range_150_meters: return 150.0f;
    }
    return 0.0f;
}
// 0x2669c0
void c_player_traits::initialize()
{
    m_shield_vitality_traits.initialize();
    m_weapon_traits.initialize();
    m_movement_traits.initialize();
    m_appearance_traits.initialize();
    m_sensor_traits.initialize();
}

// 0x266a50
void c_player_traits::set(const c_player_traits *source, bool force)
{
    if (source) {
        m_shield_vitality_traits.set(&source->m_shield_vitality_traits, force);
        m_weapon_traits.set(&source->m_weapon_traits, force);
        m_movement_traits.set(&source->m_movement_traits, force);
        m_appearance_traits.set(&source->m_appearance_traits, force);
        m_sensor_traits.set(&source->m_sensor_traits, force);
    } else if (force) {
        m_shield_vitality_traits.m_damage_resistance = _damage_resistance_percentage_setting_unchanged;
        m_shield_vitality_traits.m_shield_recharge_rate = _shield_recharge_rate_percentage_setting_unchanged;
        m_shield_vitality_traits.m_vampirism = _vampirism_percentage_setting_unchanged;
        m_shield_vitality_traits.m_headshot_immunity = _headshot_immunity_setting_unchanged;
        m_shield_vitality_traits.m_shield_multiplier = _shield_multiplier_setting_unchanged;
        m_shield_vitality_traits.m_padding[0] = 0;
        m_shield_vitality_traits.m_padding[1] = 0;
        m_shield_vitality_traits.m_padding[2] = 0;
        m_weapon_traits.m_damage_modifier = _damage_modifier_percentage_setting_unchanged;
        m_weapon_traits.m_initial_primary_weapon = -2;
        m_weapon_traits.m_initial_secondary_weapon = -2;
        m_weapon_traits.m_grenade_count = _grenade_count_setting_unchanged;
        m_weapon_traits.m_recharging_grenades = _recharging_grenades_setting_unchanged;
        m_weapon_traits.m_infinite_ammo = _infinite_ammo_setting_unchanged;
        m_weapon_traits.m_weapon_pickup = _weapon_pickup_setting_unchanged;
        m_movement_traits.m_speed = _player_speed_setting_unchanged;
        m_movement_traits.m_gravity = _player_gravity_setting_unchanged;
        m_movement_traits.m_vehicle_usage = _vehicle_usage_setting_unchanged;
        m_movement_traits.m_padding = 0;
        m_appearance_traits.m_active_camo = _active_camo_setting_unchanged;
        m_appearance_traits.m_waypoint = _waypoint_setting_unchanged;
        m_appearance_traits.m_aura = _aura_setting_unchanged;
        m_appearance_traits.m_forced_change_color = _forced_change_color_setting_unchanged;
        m_sensor_traits.m_motion_tracker = _motion_tracker_setting_unchanged;
        m_sensor_traits.m_motion_tracker_range = _motion_tracker_range_setting_unchanged;
    }
}


// 0x266b48
void c_player_traits::set(const s_game_engine_player_traits *value, bool force)
{
    if (value && value->shield_vitality.count == 1) {
        const s_player_traits_shield_source *source = (const s_player_traits_shield_source *)value->shield_vitality.address;
        c_player_trait_shield_vitality scratch;
        scratch.set_damage_resistance_percentage_setting(source->damage_resistance, true);
        scratch.set_shield_recharge_rate_percentage_setting(source->shield_recharge_rate, true);
        scratch.set_vampirism_percentage_setting(source->vampirism, true);
        scratch.set_shield_multiplier_setting(source->shield_multiplier, true);
        scratch.set_headshot_immunity_setting(source->headshot_immunity, true);
        memset(scratch.m_padding, 0, sizeof(scratch.m_padding));
        m_shield_vitality_traits.set(&scratch, true);
    } else {
        m_shield_vitality_traits.m_damage_resistance = _damage_resistance_percentage_setting_unchanged;
        m_shield_vitality_traits.m_shield_recharge_rate = _shield_recharge_rate_percentage_setting_unchanged;
        m_shield_vitality_traits.m_vampirism = _vampirism_percentage_setting_unchanged;
        m_shield_vitality_traits.m_headshot_immunity = _headshot_immunity_setting_unchanged;
        m_shield_vitality_traits.m_shield_multiplier = _shield_multiplier_setting_unchanged;
        m_shield_vitality_traits.m_padding[0] = 0;
        m_shield_vitality_traits.m_padding[1] = 0;
        m_shield_vitality_traits.m_padding[2] = 0;
    }
    if (value && value->weapons.count == 1) {
        c_player_trait_weapons scratch;
        scratch.set((const s_player_trait_weapons *)value->weapons.address, true);
        m_weapon_traits.set(&scratch, true);
    } else {
        m_weapon_traits.m_damage_modifier = _damage_modifier_percentage_setting_unchanged;
        m_weapon_traits.m_grenade_count = _grenade_count_setting_unchanged;
        m_weapon_traits.m_initial_primary_weapon = -2;
        m_weapon_traits.m_initial_secondary_weapon = -2;
        m_weapon_traits.m_recharging_grenades = _recharging_grenades_setting_unchanged;
        m_weapon_traits.m_infinite_ammo = _infinite_ammo_setting_unchanged;
        m_weapon_traits.m_weapon_pickup = _weapon_pickup_setting_unchanged;
    }
    if (value && value->movement.count == 1) {
        const s_player_traits_movement_source *source = (const s_player_traits_movement_source *)value->movement.address;
        c_player_trait_movement scratch;
        scratch.set_speed_setting(source->speed, true);
        scratch.set_gravity_setting(source->gravity, true);
        scratch.set_vehicle_usage_setting(source->vehicle_usage, true);
        scratch.m_padding = 0;
        m_movement_traits.set(&scratch, true);
    } else {
        m_movement_traits.m_speed = _player_speed_setting_unchanged;
        m_movement_traits.m_gravity = _player_gravity_setting_unchanged;
        m_movement_traits.m_vehicle_usage = _vehicle_usage_setting_unchanged;
        m_movement_traits.m_padding = 0;
    }
    if (value && value->appearance.count == 1) {
        const c_player_trait_appearance *source = (const c_player_trait_appearance *)value->appearance.address;
        c_player_trait_appearance scratch;
        scratch.set_active_camo_setting(source->m_active_camo, true);
        scratch.set_waypoint_setting(source->m_waypoint, true);
        scratch.set_aura_setting(source->m_aura, true);
        scratch.set_forced_change_color_setting(source->m_forced_change_color, true);
        m_appearance_traits.set(&scratch, true);
    } else {
        m_appearance_traits.m_active_camo = _active_camo_setting_unchanged;
        m_appearance_traits.m_waypoint = _waypoint_setting_unchanged;
        m_appearance_traits.m_aura = _aura_setting_unchanged;
        m_appearance_traits.m_forced_change_color = _forced_change_color_setting_unchanged;
    }
    if (value && value->sensors.count == 1) {
        const s_player_traits_sensors_source *source = (const s_player_traits_sensors_source *)value->sensors.address;
        c_player_trait_sensors scratch;
        scratch.set_motion_tracker_setting(source->motion_tracker, true);
        scratch.set_motion_tracker_range_setting(source->motion_tracker_range, true);
        m_sensor_traits.set(&scratch, true);
    } else {
        m_sensor_traits.m_motion_tracker = _motion_tracker_setting_unchanged;
        m_sensor_traits.m_motion_tracker_range = _motion_tracker_range_setting_unchanged;
    }
}

namespace {
static __forceinline void game_engine_player_traits_encode_movement(const c_player_trait_movement *traits, c_bitstream *packet)
{
    packet->write_integer("player-traits-movement-speed", traits->m_speed, 4);
    packet->write_integer("player-traits-movement-gravity", traits->m_gravity, 3);
    packet->write_integer("player-traits-movement-vehicle-usage", traits->m_vehicle_usage, 2);
}
static __forceinline void game_engine_player_traits_encode_appearance(const c_player_trait_appearance *traits, c_bitstream *packet)
{
    packet->write_integer("player-traits-appearance-active-camo", traits->m_active_camo, 3);
    packet->write_integer("player-traits-appearance-waypoint", traits->m_waypoint, 2);
    packet->write_integer("player-traits-appearance-aura", traits->m_aura, 3);
    packet->write_integer("player-traits-appearance-forced-change-color", traits->m_forced_change_color, 4);
}
static __forceinline void game_engine_player_traits_encode_sensors(const c_player_trait_sensors *traits, c_bitstream *packet)
{
    packet->write_integer("player-traits-sensors-motion-tracker", traits->m_motion_tracker, 3);
    packet->write_integer("player-traits-sensors-motion-tracker-range", traits->m_motion_tracker_range, 3);
}
}
void c_player_traits::encode(c_bitstream *packet) const
{
    m_shield_vitality_traits.encode(packet);
    m_weapon_traits.encode(packet);
    game_engine_player_traits_encode_movement(&m_movement_traits, packet);
    game_engine_player_traits_encode_appearance(&m_appearance_traits, packet);
    game_engine_player_traits_encode_sensors(&m_sensor_traits, packet);
}
void c_player_traits::decode(c_bitstream *packet)
{
    m_shield_vitality_traits.decode(packet);
    player_trait_weapons_decode(&m_weapon_traits, packet);
    m_movement_traits.decode(packet);
    m_appearance_traits.decode(packet);
    m_sensor_traits.decode(packet);
}


static __forceinline c_game_variant *game_engine_player_traits_current_game_variant_deref()
{
    game_options *options = &tls_game_globals()->options;
    return options->game_mode == 2 ? &options->game_variant : 0;
}
static __forceinline long game_engine_player_traits_get_recently_spawned_grace_period_seconds()
{
    long seconds = 5;
    if (current_game_variant())
        seconds = game_engine_player_traits_current_game_variant_deref()->respawn_player_traits_duration_seconds;
    return seconds;
}
// @address 0xf5268 ?game_engine_running@@YA_NXZ
// @address 0x2e0e18 ?game_get_powerup_traits@@YAXW4e_multiplayer_powerup_flavor@@PAVc_player_traits@@@Z
namespace {
static __forceinline player_datum *game_engine_player_traits_get_player(long player_index)
{
    return &((player_datum *)tls_player_data()->data)[datum_index_to_absolute_index(player_index)];
}
static __forceinline long game_engine_player_traits_time_get()
{
    return tls_game_time_globals()->time;
}
static __forceinline float game_engine_player_traits_ticks_to_seconds(float ticks)
{
    return ticks * tls_game_time_globals()->tick_length;
}
static __forceinline bool game_engine_player_traits_is_multiplayer()
{
    return tls_game_globals()->options.game_mode == 2;
}
static __forceinline float game_engine_player_traits_get_powerup_duration_seconds(e_multiplayer_powerup_flavor flavor)
{
    float seconds = 0.0f;
    if (game_engine_player_traits_is_multiplayer() && game_engine_running()) {
        const c_game_variant *variant = game_engine_player_traits_current_game_variant_deref();
        switch (flavor) {
        case _multiplayer_powerup_flavor_red:
            seconds = (float)variant->red_powerup_duration_seconds;
            break;
        case _multiplayer_powerup_flavor_blue:
            seconds = (float)variant->blue_powerup_duration_seconds;
            break;
        case _multiplayer_powerup_flavor_yellow:
            seconds = (float)variant->yellow_powerup_duration_seconds;
            break;
        }
    }
    return seconds;
}
}
// @address 0x267048 ?game_engine_add_powerup_traits@@YAXJPAVc_player_traits@@@Z
void game_engine_add_powerup_traits(long player_index, c_player_traits *traits)
{
    player_datum *player = game_engine_player_traits_get_player(player_index);
    long latest_powerup_time = 0;
    e_multiplayer_powerup_flavor latest_powerup = (e_multiplayer_powerup_flavor)NONE;
    for (e_multiplayer_powerup_flavor flavor = _multiplayer_powerup_flavor_red; flavor < k_multiplayer_powerup_flavor_count;
        flavor = (e_multiplayer_powerup_flavor)(flavor + 1)) {
        if (player->powerup_times[flavor] > latest_powerup_time) {
            latest_powerup_time = player->powerup_times[flavor];
            latest_powerup = flavor;
        }
    }
    if (latest_powerup >= 0 && latest_powerup < k_multiplayer_powerup_flavor_count) {
        long time = game_engine_player_traits_time_get();
        float elapsed = game_engine_player_traits_ticks_to_seconds((float)(time - player->powerup_times[latest_powerup]));
        float duration = game_engine_player_traits_get_powerup_duration_seconds(latest_powerup);
        if (elapsed <= duration) {
            c_player_traits powerup_traits;
            game_get_powerup_traits(latest_powerup, &powerup_traits);
            traits->set(&powerup_traits, false);
        }
    }
}

// @address 0x2671c8 game_engine_apply_respawn_player_traits
extern "C" void game_engine_apply_respawn_player_traits(long player_index, const c_player_traits *traits)
{
    player_datum *player = (player_datum *)(tls_player_data()->data + (datum_index_to_absolute_index(player_index)) * sizeof(player_datum));
    if (game_engine_player_traits_get_recently_spawned_grace_period_seconds() <= 0)
        return;
    if (player->grace_ticks <= 0)
        return;
    if (player->b2ae)
        return;
    if (player->flags & 8)
        return;
    if (!current_game_variant())
        return;
    c_game_variant *variant = game_engine_player_traits_current_game_variant_deref();
    ((c_player_traits *)traits)->set(&variant->respawn_player_traits, false);
}

template<typename t_enum, typename t_storage, long k_minimum, long k_maximum>
static __forceinline void game_engine_player_traits_apply_shield_setting(
    c_enum<t_enum, t_storage, k_minimum, k_maximum> *setting, int value, unsigned long maximum, bool *changed)
{
    if ((unsigned int)(t_enum)*setting != (unsigned int)value) {
        if ((unsigned int)value <= maximum && value != 0)
            *setting = (t_enum)value;
        *changed = true;
    }
}
// @address 0x267380 ?game_engine_apply_shield_vitality_traits@@YAXJPBVc_player_trait_shield_vitality@@@Z
void game_engine_apply_shield_vitality_traits(long player_index, const c_player_trait_shield_vitality *traits)
{
    player_datum *player = game_engine_player_traits_get_player(player_index);
    bool changed = false;
    game_engine_player_traits_apply_shield_setting(
        &player->traits.m_shield_vitality_traits.m_damage_resistance, traits->get_damage_resistance_percentage_setting(), 12, &changed);
    game_engine_player_traits_apply_shield_setting(&player->traits.m_shield_vitality_traits.m_shield_recharge_rate,
        traits->get_shield_recharge_rate_percentage_setting(), 9, &changed);
    game_engine_player_traits_apply_shield_setting(
        &player->traits.m_shield_vitality_traits.m_vampirism, traits->get_vampirism_percentage_setting(), 5, &changed);
    game_engine_player_traits_apply_shield_setting(
        &player->traits.m_shield_vitality_traits.m_headshot_immunity, traits->get_headshot_immunity_setting(), 2, &changed);
    game_engine_player_traits_apply_shield_setting(
        &player->traits.m_shield_vitality_traits.m_shield_multiplier, traits->get_shield_multiplier_setting(), 5, &changed);
    if (changed)
        ::simulation_action_game_engine_player_update((short)player_index, 2);
}
// @address 0x267488 ?game_engine_apply_weapons_traits@@YAXJPBVc_player_trait_weapons@@@Z
void game_engine_apply_weapons_traits(long player_index, const c_player_trait_weapons *traits)
{
    bool changed = false;
    int damage_modifier = traits->m_damage_modifier;
    player_datum *player = game_engine_player_traits_get_player(player_index);
    if ((unsigned int)player->traits.m_weapon_traits.m_damage_modifier != (unsigned int)damage_modifier) {
        if ((unsigned int)damage_modifier <= 12 && damage_modifier != 0)
            player->traits.m_weapon_traits.m_damage_modifier = (e_damage_modifier_percentage_setting)damage_modifier;
        changed = true;
    }
    unsigned char primary = traits->m_initial_primary_weapon;
    if ((signed char)player->traits.m_weapon_traits.m_initial_primary_weapon != (signed char)primary) {
        player->traits.m_weapon_traits.set_initial_primary_weapon_absolute_index((signed char)primary, false);
        changed = true;
    }
    unsigned char secondary = traits->m_initial_secondary_weapon;
    if ((signed char)player->traits.m_weapon_traits.m_initial_secondary_weapon != (signed char)secondary) {
        player->traits.m_weapon_traits.set_initial_secondary_weapon_absolute_index((signed char)secondary, false);
        changed = true;
    }
    int grenade_count = traits->m_grenade_count;
    if (player->traits.m_weapon_traits.m_grenade_count != grenade_count) {
        if ((unsigned int)grenade_count <= 2 && grenade_count != 0)
            player->traits.m_weapon_traits.m_grenade_count = (e_grenade_count_setting)grenade_count;
        changed = true;
    }
    int recharging_grenades = traits->m_recharging_grenades;
    if ((unsigned int)player->traits.m_weapon_traits.m_recharging_grenades != (unsigned int)recharging_grenades) {
        if ((unsigned int)recharging_grenades <= 2 && recharging_grenades != 0)
            player->traits.m_weapon_traits.m_recharging_grenades = (e_recharging_grenades_setting)recharging_grenades;
        changed = true;
    }
    int infinite_ammo = traits->m_infinite_ammo;
    if ((unsigned int)player->traits.m_weapon_traits.m_infinite_ammo != (unsigned int)infinite_ammo) {
        if ((unsigned int)infinite_ammo <= 2 && infinite_ammo != 0)
            player->traits.m_weapon_traits.m_infinite_ammo = (e_infinite_ammo_setting)infinite_ammo;
        changed = true;
    }
    int weapon_pickup = traits->m_weapon_pickup;
    if ((unsigned int)player->traits.m_weapon_traits.m_weapon_pickup != (unsigned int)weapon_pickup) {
        if ((unsigned int)weapon_pickup <= 2 && weapon_pickup != 0)
            player->traits.m_weapon_traits.m_weapon_pickup = (e_weapon_pickup_setting)weapon_pickup;
        changed = true;
    }
    if (changed != 0)
        ::simulation_action_game_engine_player_update((short)player_index, 4);
}
// @address 0x267610 ?game_engine_apply_movement_traits@@YAXJPBVc_player_trait_movement@@@Z
void game_engine_apply_movement_traits(long player_index, const c_player_trait_movement *traits)
{
    player_datum *player = game_engine_player_traits_get_player(player_index);
    bool changed = false;
    const c_player_trait_movement *stored = &player->traits.m_movement_traits;
    float stored_speed = stored->get_speed();
    if (stored_speed != traits->get_speed()) {
        int value = traits->m_speed;
        if ((unsigned int)value <= 10 && value != 0)
            player->traits.m_movement_traits.m_speed = (e_player_speed_setting)value;
        changed = true;
    }
    float stored_gravity = stored->get_gravity();
    if (stored_gravity != traits->get_gravity()) {
        int value = traits->m_gravity;
        if ((unsigned int)value <= 5 && value != 0)
            player->traits.m_movement_traits.m_gravity = (e_player_gravity_setting)value;
        changed = true;
    }
    int vehicle_value = traits->m_vehicle_usage;
    if (player->traits.m_movement_traits.m_vehicle_usage != (unsigned int)vehicle_value) {
        if ((unsigned int)vehicle_value <= 3 && vehicle_value != 0)
            player->traits.m_movement_traits.m_vehicle_usage = (e_vehicle_usage_setting)vehicle_value;
        changed = true;
    }
    if (changed)
        ::simulation_action_game_engine_player_update((short)player_index, 8);
}

static __forceinline void game_engine_player_traits_apply_sensor_traits(long player_index, const c_player_trait_sensors *traits)
{
    player_datum *player = game_engine_player_traits_get_player(player_index);
    bool changed = false;
    if (player->traits.m_sensor_traits.m_motion_tracker != traits->m_motion_tracker) {
        player->traits.m_sensor_traits.set_motion_tracker_setting(traits->m_motion_tracker, false);
        changed = true;
    }
    if (player->traits.m_sensor_traits.m_motion_tracker_range != traits->m_motion_tracker_range) {
        player->traits.m_sensor_traits.set_motion_tracker_range_setting(traits->m_motion_tracker_range, false);
        changed = true;
    }
    if (changed)
        ::simulation_action_game_engine_player_update((short)player_index, 32);
}
// @address 0x267298 ?game_engine_apply_player_traits@@YAXJPBVc_player_traits@@@Z
void game_engine_apply_player_traits(long player_index, const c_player_traits *traits)
{
    game_engine_apply_shield_vitality_traits(player_index, &traits->m_shield_vitality_traits);
    game_engine_apply_weapons_traits(player_index, &traits->m_weapon_traits);
    game_engine_apply_movement_traits(player_index, &traits->m_movement_traits);
    game_engine_apply_appearance_traits(player_index, &traits->m_appearance_traits);
    game_engine_player_traits_apply_sensor_traits(player_index, &traits->m_sensor_traits);
}

enum e_game_team;
enum e_team_scoring_method;
enum e_multiplayer_team_designator;
template<long capacity> class c_static_wchar_string;
struct s_chud_navpoint;
// @address 0x266f10 ?game_engine_assemble_player_traits@@YAXJ@Z
void game_engine_assemble_player_traits(long player_index)
{
    game_options *options = &tls_game_globals()->options;
    if (options->game_mode == 2) {
        bool predicted = false;
        if (tls_game_globals()->options.game_simulation == 4)
            predicted = true;
        if (!predicted && current_game_engine() && current_game_variant()) {
            c_player_traits traits;
            traits.initialize();
            game_engine_get()->assemble_baseline_traits_for_player(player_index, &traits);
            game_engine_get()->apply_game_engine_traits_for_player(player_index, &traits);
            game_engine_add_powerup_traits(player_index, &traits);
            game_engine_apply_respawn_player_traits(player_index, &traits);
            game_engine_apply_player_traits(player_index, &traits);
        }
    }
}

// @address 0x58890 k_player_traits_good_camo_maximum
const float k_player_traits_good_camo_maximum = 0.66667f;
static __forceinline bool game_engine_player_traits_is_predicted()
{
    bool predicted = false;
    if (tls_game_globals()->options.game_simulation == 4)
        predicted = true;
    return predicted != 0;
}
static __forceinline void game_engine_player_traits_set_camo_maximum(long unit_index, float maximum)
{
    unit_datum *unit = (unit_datum *)object_get(unit_index);
    unit->unit.active_camouflage_target = maximum;
    ::simulation_action_object_update(unit_index, 0x08000000);
}
// @address 0x267708 ?game_engine_apply_appearance_traits@@YAXJPBVc_player_trait_appearance@@@Z
void game_engine_apply_appearance_traits(long player_index, const c_player_trait_appearance *traits)
{
    player_datum *player = game_engine_player_traits_get_player(player_index);
    bool changed = false;
    bool update_camouflage = false;
    int camo = traits->m_active_camo;
    if (player->traits.m_appearance_traits.m_active_camo != (unsigned int)camo) {
        if ((unsigned int)camo <= 4 && camo != 0)
            player->traits.m_appearance_traits.m_active_camo = (e_active_camo_setting)camo;
        changed = true;
        update_camouflage = !game_engine_player_traits_is_predicted();
    }
    game_engine_player_traits_apply_shield_setting(&player->traits.m_appearance_traits.m_waypoint, traits->m_waypoint, 3, &changed);
    game_engine_player_traits_apply_shield_setting(&player->traits.m_appearance_traits.m_aura, traits->m_aura, 4, &changed);
    game_engine_player_traits_apply_shield_setting(&player->traits.m_appearance_traits.m_forced_change_color,
        traits->m_forced_change_color, 13, &changed);
    if (changed)
        ::simulation_action_game_engine_player_update((short)player_index, 16);
    if (update_camouflage && player->unit_index != -1) {
        switch ((int)traits->m_active_camo) {
        case _active_camo_poor:
            game_engine_player_traits_set_camo_maximum(player->unit_index, 0.33333f);
            unit_active_camouflage_enable(player->unit_index, 4.0f, false);
            break;
        case _active_camo_good:
            game_engine_player_traits_set_camo_maximum(player->unit_index, k_player_traits_good_camo_maximum);
            unit_active_camouflage_enable(player->unit_index, 4.0f, false);
            break;
        case _active_camo_invisible:
            game_engine_player_traits_set_camo_maximum(player->unit_index, 1.0f);
            unit_active_camouflage_enable(player->unit_index, 4.0f, false);
            break;
        default:
            game_engine_player_traits_set_camo_maximum(player->unit_index, 1.0f);
            unit_active_camouflage_disable(player->unit_index, 4.0f);
            break;
        }
    }
}
