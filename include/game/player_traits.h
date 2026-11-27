#ifndef GAME_PLAYER_TRAITS_H
#define GAME_PLAYER_TRAITS_H

#include <string.h>
#include "cseries/cseries_wrapped.h"

class c_bitstream;
struct s_game_engine_player_traits;

enum e_damage_resistance_percentage_setting
{
    _damage_resistance_percentage_setting_unchanged = 0,
    _e_damage_resistance_percentage_setting_default = 4,
    _damage_resistance_10_percent = 1,
    _damage_resistance_50_percent = 2,
    _damage_resistance_90_percent = 3,
    _damage_resistance_110_percent = 5,
    _damage_resistance_150_percent = 6,
    _damage_resistance_200_percent = 7,
    _damage_resistance_300_percent = 8,
    _damage_resistance_500_percent = 9,
    _damage_resistance_1000_percent = 10,
    _damage_resistance_2000_percent = 11,
    _damage_resistance_100000_percent = 12
};
enum e_shield_recharge_rate_percentage_setting
{
    _shield_recharge_rate_percentage_setting_unchanged = 0,
    _e_shield_recharge_rate_percentage_setting_default = 7,
    _shield_recharge_rate_negative_25_percent = 1,
    _shield_recharge_rate_negative_10_percent = 2,
    _shield_recharge_rate_negative_5_percent = 3,
    _shield_recharge_rate_zero = 4,
    _shield_recharge_rate_50_percent = 5,
    _shield_recharge_rate_90_percent = 6,
    _shield_recharge_rate_100_percent = 7,
    _shield_recharge_rate_110_percent = 8,
    _shield_recharge_rate_200_percent = 9
};
enum e_vampirism_percentage_setting
{
    _vampirism_percentage_setting_unchanged = 0,
    _e_vampirism_percentage_setting_default = 1
};
enum e_headshot_immunity_setting
{
    _headshot_immunity_setting_unchanged = 0,
    _e_headshot_immunity_setting_default = 2
};
enum e_shield_multiplier_setting
{
    _shield_multiplier_setting_unchanged = 0,
    _e_shield_multiplier_setting_default = 2
};
enum e_grenade_count_setting
{
    _grenade_count_setting_unchanged = 0,
    _e_grenade_count_setting_default = 1
};
enum e_damage_modifier_percentage_setting
{
    _damage_modifier_percentage_setting_unchanged = 0,
    _e_damage_modifier_percentage_setting_default = 6,
    _damage_modifier_zero = 1,
    _damage_modifier_25_percent = 2,
    _damage_modifier_50_percent = 3,
    _damage_modifier_75_percent = 4,
    _damage_modifier_90_percent = 5,
    _damage_modifier_110_percent = 7,
    _damage_modifier_125_percent = 8,
    _damage_modifier_150_percent = 9,
    _damage_modifier_200_percent = 10,
    _damage_modifier_300_percent = 11,
    _damage_modifier_100000_percent = 12
};
enum e_recharging_grenades_setting
{
    _recharging_grenades_setting_unchanged = 0,
    _e_recharging_grenades_setting_default = 2
};
enum e_infinite_ammo_setting
{
    _infinite_ammo_setting_unchanged = 0,
    _e_infinite_ammo_setting_default = 1
};
enum e_weapon_pickup_setting
{
    _weapon_pickup_setting_unchanged = 0,
    _weapon_pickup_disallowed = 2,
    _e_weapon_pickup_setting_default = 1
};
enum e_player_speed_setting
{
    _player_speed_setting_unchanged = 0,
    _e_player_speed_setting_default = 5,
    _player_speed_25_percent = 1,
    _player_speed_50_percent = 2,
    _player_speed_75_percent = 3,
    _player_speed_90_percent = 4,
    _player_speed_110_percent = 6,
    _player_speed_125_percent = 7,
    _player_speed_150_percent = 8,
    _player_speed_200_percent = 9,
    _player_speed_300_percent = 10
};
enum e_player_gravity_setting
{
    _player_gravity_setting_unchanged = 0,
    _e_player_gravity_setting_default = 3,
    _player_gravity_50_percent = 1,
    _player_gravity_75_percent = 2,
    _player_gravity_150_percent = 4,
    _player_gravity_200_percent = 5
};
enum e_vehicle_usage_setting
{
    _vehicle_usage_setting_unchanged = 0,
    _e_vehicle_usage_setting_default = 3
};
enum e_active_camo_setting
{
    _active_camo_setting_unchanged = 0,
    _e_active_camo_setting_default = 1,
    _active_camo_poor = 2,
    _active_camo_good = 3,
    _active_camo_invisible = 4
};
enum e_waypoint_setting
{
    _waypoint_setting_unchanged = 0,
    _e_waypoint_setting_default = 2,
    _waypoint_allies = 2,
    _waypoint_everyone = 3
};
enum e_aura_setting {
    _aura_setting_unchanged = 0,
    _e_aura_setting_default = 1
};
enum e_forced_change_color_setting
{
    _forced_change_color_setting_unchanged = 0,
    _e_forced_change_color_setting_default = 1
};
enum e_motion_tracker_setting
{
    _motion_tracker_setting_unchanged = 0,
    _e_motion_tracker_setting_default = 3
};
enum e_motion_tracker_range_setting
{
    _motion_tracker_range_setting_unchanged = 0,
    _e_motion_tracker_range_setting_default = 3,
    _motion_tracker_range_10_meters = 1,
    _motion_tracker_range_15_meters = 2,
    _motion_tracker_range_25_meters = 3,
    _motion_tracker_range_50_meters = 4,
    _motion_tracker_range_75_meters = 5,
    _motion_tracker_range_100_meters = 6,
    _motion_tracker_range_150_meters = 7
};

struct s_player_trait_weapons {
    unsigned char damage_modifier;
    unsigned char recharging_grenades;
    unsigned char weapon_pickup;
    unsigned char infinite_ammo;
    long initial_primary_weapon;
    long initial_secondary_weapon;
    short grenade_count;
};

class c_player_trait_shield_vitality {
public:
    void set(const c_player_trait_shield_vitality *source, bool force);
    float get_damage_resistance_percentage() const;
    float get_shield_recharge_rate_percentage() const;
    float get_maximum_shield_vitality() const;
    void encode(c_bitstream *packet) const;
    void decode(c_bitstream *packet);

    __forceinline bool equivalent(const c_player_trait_shield_vitality *other) const {
        return (unsigned)m_damage_resistance == (unsigned)other->m_damage_resistance
            && (unsigned)m_shield_recharge_rate == (unsigned)other->m_shield_recharge_rate
            && (unsigned)m_vampirism == (unsigned)other->m_vampirism
            && (unsigned)m_headshot_immunity == (unsigned)other->m_headshot_immunity
            && (unsigned)m_shield_multiplier == (unsigned)other->m_shield_multiplier;
    }
    __forceinline void initialize() {
        m_damage_resistance = _e_damage_resistance_percentage_setting_default;
        m_shield_recharge_rate = _e_shield_recharge_rate_percentage_setting_default;
        m_vampirism = _e_vampirism_percentage_setting_default;
        m_headshot_immunity = _e_headshot_immunity_setting_default;
        m_shield_multiplier = _e_shield_multiplier_setting_default;
        memset(m_padding, 0, sizeof(m_padding));
    }
    __forceinline void set_damage_resistance_percentage_setting(long value, bool force) {
        if ((unsigned long)value <= 12) { if (value != 0 || force) m_damage_resistance = (e_damage_resistance_percentage_setting)value; }
        else if (force) m_damage_resistance = _damage_resistance_percentage_setting_unchanged;
    }
    __forceinline void set_shield_recharge_rate_percentage_setting(long value, bool force) {
        if ((unsigned long)value <= 9) { if (value != 0 || force) m_shield_recharge_rate = (e_shield_recharge_rate_percentage_setting)value; }
        else if (force) m_shield_recharge_rate = _shield_recharge_rate_percentage_setting_unchanged;
    }
    __forceinline void set_vampirism_percentage_setting(long value, bool force) {
        if ((unsigned long)value <= 5) { if (value != 0 || force) m_vampirism = (e_vampirism_percentage_setting)value; }
        else if (force) m_vampirism = _vampirism_percentage_setting_unchanged;
    }
    __forceinline void set_headshot_immunity_setting(long value, bool force) {
        if ((unsigned long)value <= 2) { if (value != 0 || force) m_headshot_immunity = (e_headshot_immunity_setting)value; }
        else if (force) m_headshot_immunity = _headshot_immunity_setting_unchanged;
    }
    __forceinline void set_shield_multiplier_setting(long value, bool force) {
        if ((unsigned long)value <= 5) { if (value != 0 || force) m_shield_multiplier = (e_shield_multiplier_setting)value; }
        else if (force) m_shield_multiplier = _shield_multiplier_setting_unchanged;
    }
    __forceinline unsigned long get_damage_resistance_percentage_setting() const { return m_damage_resistance; }
    __forceinline unsigned long get_shield_recharge_rate_percentage_setting() const { return m_shield_recharge_rate; }
    __forceinline unsigned long get_vampirism_percentage_setting() const { return m_vampirism; }
    __forceinline unsigned long get_headshot_immunity_setting() const { return m_headshot_immunity; }
    __forceinline unsigned long get_shield_multiplier_setting() const { return m_shield_multiplier; }
    __forceinline void clear() {
        m_damage_resistance = _damage_resistance_percentage_setting_unchanged;
        m_shield_recharge_rate = _shield_recharge_rate_percentage_setting_unchanged;
        m_vampirism = _vampirism_percentage_setting_unchanged;
        m_headshot_immunity = _headshot_immunity_setting_unchanged;
        m_shield_multiplier = _shield_multiplier_setting_unchanged;
        memset(m_padding, 0, sizeof(m_padding));
    }

    c_enum<e_damage_resistance_percentage_setting, unsigned char, 0, 13> m_damage_resistance;
    c_enum<e_shield_recharge_rate_percentage_setting, unsigned char, 0, 10> m_shield_recharge_rate;
    c_enum<e_vampirism_percentage_setting, unsigned char, 0, 6> m_vampirism;
    c_enum<e_headshot_immunity_setting, unsigned char, 0, 3> m_headshot_immunity;
    c_enum<e_shield_multiplier_setting, unsigned char, 0, 6> m_shield_multiplier;
    unsigned char m_padding[3];
};

class c_player_trait_weapons {
public:
    void set(const c_player_trait_weapons *source, bool force);
    void set(const s_player_trait_weapons *traits, bool force);
    void set_initial_primary_weapon_absolute_index(char index, bool force);
    void set_initial_secondary_weapon_absolute_index(char index, bool force);
    void set_initial_primary_weapon(long name, bool force);
    void set_initial_secondary_weapon(long name, bool force);
    float get_damage_modifier_percentage() const;
    void encode(c_bitstream *packet) const;
    void decode(c_bitstream *packet);

    __forceinline bool get_weapon_pickup_allowed() const {
        e_weapon_pickup_setting setting = m_weapon_pickup;
        return setting != _weapon_pickup_disallowed;
    }
    __forceinline bool equivalent(const c_player_trait_weapons *other) const {
        return (unsigned)m_damage_modifier == (unsigned)other->m_damage_modifier
            && (unsigned char)m_initial_primary_weapon == (unsigned char)other->m_initial_primary_weapon
            && (unsigned char)m_initial_secondary_weapon == (unsigned char)other->m_initial_secondary_weapon
            && (unsigned)m_grenade_count == (unsigned)other->m_grenade_count
            && (unsigned)m_recharging_grenades == (unsigned)other->m_recharging_grenades
            && (unsigned)m_infinite_ammo == (unsigned)other->m_infinite_ammo
            && (unsigned)m_weapon_pickup == (unsigned)other->m_weapon_pickup;
    }
    __forceinline void initialize() {
        m_damage_modifier = _e_damage_modifier_percentage_setting_default;
        m_initial_primary_weapon = -1;
        m_initial_secondary_weapon = -1;
        m_grenade_count = _e_grenade_count_setting_default;
        m_recharging_grenades = _e_recharging_grenades_setting_default;
        m_infinite_ammo = _e_infinite_ammo_setting_default;
        m_weapon_pickup = _e_weapon_pickup_setting_default;
    }
    __forceinline void clear() {
        m_damage_modifier = _damage_modifier_percentage_setting_unchanged;
        m_initial_primary_weapon = -2;
        m_initial_secondary_weapon = -2;
        m_grenade_count = _grenade_count_setting_unchanged;
        m_recharging_grenades = _recharging_grenades_setting_unchanged;
        m_infinite_ammo = _infinite_ammo_setting_unchanged;
        m_weapon_pickup = _weapon_pickup_setting_unchanged;
    }
    __forceinline void set_damage_modifier_percentage_setting(long value, bool force) {
        if ((unsigned long)value <= 12) { if (value != 0 || force) m_damage_modifier = (e_damage_modifier_percentage_setting)value; }
        else if (force) m_damage_modifier = _damage_modifier_percentage_setting_unchanged;
    }
    __forceinline void set_initial_grenade_count_setting(long value, bool force) {
        if ((unsigned long)value <= 2) { if (value != 0 || force) m_grenade_count = (e_grenade_count_setting)value; }
        else if (force) m_grenade_count = _grenade_count_setting_unchanged;
    }
    __forceinline void set_infinite_ammo_setting(long value, bool force) {
        if ((unsigned long)value <= 2) { if (value != 0 || force) m_infinite_ammo = (e_infinite_ammo_setting)value; }
        else if (force) m_infinite_ammo = _infinite_ammo_setting_unchanged;
    }
    __forceinline void set_recharging_grenades_setting(long value, bool force) {
        if ((unsigned long)value <= 2) { if (value != 0 || force) m_recharging_grenades = (e_recharging_grenades_setting)value; }
        else if (force) m_recharging_grenades = _recharging_grenades_setting_unchanged;
    }
    __forceinline void set_weapon_pickup_allowed_setting(long value, bool force) {
        if ((unsigned long)value <= 2) { if (value != 0 || force) m_weapon_pickup = (e_weapon_pickup_setting)value; }
        else if (force) m_weapon_pickup = _weapon_pickup_setting_unchanged;
    }

    c_enum<e_grenade_count_setting, short, 0, 3> m_grenade_count;
    char m_initial_primary_weapon;
    char m_initial_secondary_weapon;
    c_enum<e_damage_modifier_percentage_setting, unsigned char, 0, 13> m_damage_modifier;
    c_enum<e_recharging_grenades_setting, unsigned char, 0, 3> m_recharging_grenades;
    c_enum<e_infinite_ammo_setting, unsigned char, 0, 3> m_infinite_ammo;
    c_enum<e_weapon_pickup_setting, unsigned char, 0, 3> m_weapon_pickup;
};

class c_player_trait_movement {
public:
    float get_speed() const;
    float get_gravity() const;
    void set(const c_player_trait_movement *source, bool force);
    void encode(c_bitstream *packet) const;
    void decode(c_bitstream *packet);

    __forceinline bool equivalent(const c_player_trait_movement *other) const {
        return (unsigned)m_speed == (unsigned)other->m_speed
            && (unsigned)m_gravity == (unsigned)other->m_gravity
            && (unsigned)m_vehicle_usage == (unsigned)other->m_vehicle_usage;
    }
    __forceinline void set_speed_setting(long value, bool force) {
        if ((unsigned long)value <= 10) { if (value != 0 || force) m_speed = (e_player_speed_setting)value; }
        else if (force) m_speed = _player_speed_setting_unchanged;
    }
    __forceinline void set_gravity_setting(long value, bool force) {
        if ((unsigned long)value <= 5) { if (value != 0 || force) m_gravity = (e_player_gravity_setting)value; }
        else if (force) m_gravity = _player_gravity_setting_unchanged;
    }
    __forceinline void set_vehicle_usage_setting(long value, bool force) {
        if ((unsigned long)value <= 3) { if (value != 0 || force) m_vehicle_usage = (e_vehicle_usage_setting)value; }
        else if (force) m_vehicle_usage = _vehicle_usage_setting_unchanged;
    }
    __forceinline void initialize() {
        m_speed = _e_player_speed_setting_default;
        m_gravity = _e_player_gravity_setting_default;
        m_vehicle_usage = _e_vehicle_usage_setting_default;
        m_padding = 0;
    }
    __forceinline void clear() {
        m_speed = _player_speed_setting_unchanged;
        m_gravity = _player_gravity_setting_unchanged;
        m_vehicle_usage = _vehicle_usage_setting_unchanged;
        m_padding = 0;
    }

    c_enum<e_player_speed_setting, unsigned char, 0, 11> m_speed;
    c_enum<e_player_gravity_setting, unsigned char, 0, 6> m_gravity;
    c_enum<e_vehicle_usage_setting, unsigned char, 0, 4> m_vehicle_usage;
    unsigned char m_padding;
};

class c_player_trait_appearance {
public:
    void set(const c_player_trait_appearance *source, bool force);
    void encode(c_bitstream *packet) const;
    void decode(c_bitstream *packet);

    __forceinline bool equivalent(const c_player_trait_appearance *other) const {
        return (unsigned)m_active_camo == (unsigned)other->m_active_camo
            && (unsigned)m_waypoint == (unsigned)other->m_waypoint
            && (unsigned)m_aura == (unsigned)other->m_aura
            && (unsigned)m_forced_change_color == (unsigned)other->m_forced_change_color;
    }
    __forceinline e_waypoint_setting get_waypoint_setting() const { return m_waypoint; }
    __forceinline void set_active_camo_setting(long value, bool force) {
        if ((unsigned long)value <= 4) { if (value != 0 || force) m_active_camo = (e_active_camo_setting)value; }
        else if (force) m_active_camo = _active_camo_setting_unchanged;
    }
    __forceinline void set_waypoint_setting(long value, bool force) {
        if ((unsigned long)value <= 3) { if (value != 0 || force) m_waypoint = (e_waypoint_setting)value; }
        else if (force) m_waypoint = _waypoint_setting_unchanged;
    }
    __forceinline void set_aura_setting(long value, bool force) {
        if ((unsigned long)value <= 4) { if (value != 0 || force) m_aura = (e_aura_setting)value; }
        else if (force) m_aura = _aura_setting_unchanged;
    }
    __forceinline void set_forced_change_color_setting(long value, bool force) {
        if ((unsigned long)value <= 13) { if (value != 0 || force) m_forced_change_color = (e_forced_change_color_setting)value; }
        else if (force) m_forced_change_color = _forced_change_color_setting_unchanged;
    }
    __forceinline void initialize() {
        m_active_camo = _e_active_camo_setting_default;
        m_waypoint = _e_waypoint_setting_default;
        m_aura = _e_aura_setting_default;
        m_forced_change_color = _e_forced_change_color_setting_default;
    }
    __forceinline void clear() {
        m_active_camo = _active_camo_setting_unchanged;
        m_waypoint = _waypoint_setting_unchanged;
        m_aura = _aura_setting_unchanged;
        m_forced_change_color = _forced_change_color_setting_unchanged;
    }

    c_enum<e_active_camo_setting, unsigned char, 0, 5> m_active_camo;
    c_enum<e_waypoint_setting, unsigned char, 0, 4> m_waypoint;
    c_enum<e_aura_setting, unsigned char, 0, 5> m_aura;
    c_enum<e_forced_change_color_setting, unsigned char, 0, 3> m_forced_change_color;
};

class c_player_trait_sensors {
public:
    void set(const c_player_trait_sensors *source, bool force);
    float get_motion_tracker_range() const;
    void encode(c_bitstream *packet) const;
    void decode(c_bitstream *packet);

    __forceinline bool equivalent(const c_player_trait_sensors *other) const {
        return (unsigned)m_motion_tracker == (unsigned)other->m_motion_tracker
            && (unsigned)m_motion_tracker_range == (unsigned)other->m_motion_tracker_range;
    }
    __forceinline void initialize() {
        m_motion_tracker = _e_motion_tracker_setting_default;
        m_motion_tracker_range = _e_motion_tracker_range_setting_default;
    }
    __forceinline void clear() {
        m_motion_tracker = _motion_tracker_setting_unchanged;
        m_motion_tracker_range = _motion_tracker_range_setting_unchanged;
    }
    __forceinline void set_motion_tracker_setting(long value, bool force) {
        if ((unsigned long)value <= 4) { if (value != 0 || force) m_motion_tracker = (e_motion_tracker_setting)value; }
        else if (force) m_motion_tracker = _motion_tracker_setting_unchanged;
    }
    __forceinline void set_motion_tracker_range_setting(long value, bool force) {
        if ((unsigned long)value <= 7) { if (value != 0 || force) m_motion_tracker_range = (e_motion_tracker_range_setting)value; }
        else if (force) m_motion_tracker_range = _motion_tracker_range_setting_unchanged;
    }

    c_enum<e_motion_tracker_setting, unsigned short, 0, 5> m_motion_tracker;
    c_enum<e_motion_tracker_range_setting, unsigned short, 0, 8> m_motion_tracker_range;
};

class c_player_traits {
public:
    void initialize();
    void set(const c_player_traits *source, bool force);
    void set(const s_game_engine_player_traits *value, bool force);
    void encode(c_bitstream *packet) const;
    void decode(c_bitstream *packet);

    __forceinline void clear() {
        m_shield_vitality_traits.clear();
        m_weapon_traits.clear();
        m_movement_traits.clear();
        m_appearance_traits.clear();
        m_sensor_traits.clear();
    }
    // March inline getters (simulation_game_engine_player.obj): each returns its member's address
    __forceinline const c_player_trait_shield_vitality *get_shield_vitality_traits() const { return &m_shield_vitality_traits; }
    __forceinline const c_player_trait_weapons *get_weapons_traits() const { return &m_weapon_traits; }
    __forceinline const c_player_trait_movement *get_movement_traits() const { return &m_movement_traits; }
    __forceinline const c_player_trait_appearance *get_appearance_traits() const { return &m_appearance_traits; }
    __forceinline const c_player_trait_sensors *get_sensor_traits() const { return &m_sensor_traits; }
    __forceinline c_player_trait_shield_vitality *get_shield_vitality_traits_writeable() { return &m_shield_vitality_traits; }
    __forceinline c_player_trait_weapons *get_weapons_traits_writeable() { return &m_weapon_traits; }
    __forceinline c_player_trait_movement *get_movement_traits_writeable() { return &m_movement_traits; }
    __forceinline c_player_trait_appearance *get_appearance_traits_writeable() { return &m_appearance_traits; }
    __forceinline c_player_trait_sensors *get_sensor_traits_writeable() { return &m_sensor_traits; }

    c_player_trait_shield_vitality m_shield_vitality_traits;
    c_player_trait_weapons m_weapon_traits;
    c_player_trait_movement m_movement_traits;
    c_player_trait_appearance m_appearance_traits;
    c_player_trait_sensors m_sensor_traits;
};

typedef char player_traits_size_shield[(sizeof(c_player_trait_shield_vitality) == 8) ? 1 : -1];
typedef char player_traits_size_weapons[(sizeof(c_player_trait_weapons) == 8) ? 1 : -1];
typedef char player_traits_size_movement[(sizeof(c_player_trait_movement) == 4) ? 1 : -1];
typedef char player_traits_size_appearance[(sizeof(c_player_trait_appearance) == 4) ? 1 : -1];
typedef char player_traits_size_sensors[(sizeof(c_player_trait_sensors) == 4) ? 1 : -1];
typedef char player_traits_size_all[(sizeof(c_player_traits) == 0x1c) ? 1 : -1];

#endif // GAME_PLAYER_TRAITS_H
