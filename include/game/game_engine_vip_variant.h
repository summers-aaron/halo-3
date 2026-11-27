#ifndef GAME_GAME_ENGINE_VIP_VARIANT_H
#define GAME_GAME_ENGINE_VIP_VARIANT_H

#include "game/game_engine_base_variant.h"
#include <stdlib.h>

enum e_vip_zone_order_setting
{
    _vip_zone_order_random = 0,
    _vip_zone_order_sequential = 1,
    k_vip_zone_order_settings_count = 2
};

// The base variant is followed by four padding bytes
// before the derived settings. Offsets below are checked against game_options.
class c_game_engine_vip_variant : public c_game_engine_base_variant
{
protected:
    virtual long get_game_engine_name_string_id() const;
    virtual long get_game_engine_default_description_string_id() const;
public:
    virtual void initialize();
    virtual void validate();
    virtual void encode(c_bitstream *packet) const;
    virtual void decode(c_bitstream *packet);
    virtual bool can_be_cast_to(e_game_engine_type engine_type, const void **variant) const;
    virtual long get_score_to_win_round() const;

    void set(const c_game_engine_vip_variant *source);

    __forceinline char get_destination_arrival_points() const { return m_destination_arrival_points; }
    __forceinline long get_vip_selection() const { return m_vip_selection; }
    __forceinline long get_zone_movement() const { return m_zone_movement; }
    __forceinline long get_zone_order() const { return m_zone_order.m_storage; }
    __forceinline char get_takedown_points() const { return m_takedown_points; }
    __forceinline char get_kill_as_vip_points() const { return m_kill_as_vip_points; }
    __forceinline char get_vip_suicide_points() const { return m_vip_suicide_points; }
    __forceinline char get_vip_death_points() const { return m_vip_death_points; }
    __forceinline bool get_should_end_round_on_vip_death() const { return (_rotl((unsigned short)m_variant_flags, 30) & 1) != 0; }
    __forceinline char get_kill_points() const { return m_kill_points; }
    __forceinline char get_suicide_points() const { return m_suicide_points; }
    __forceinline char get_betrayal_points() const { return m_betrayal_points; }
    __forceinline bool get_single_vip() const { return (m_variant_flags & 1) != 0; }
    __forceinline bool get_destination_zones_enabled() const { return (_rotl((unsigned short)m_variant_flags, 31) & 1) != 0; }
    __forceinline const c_player_traits *get_vip_team_traits() const { return &m_vip_team_traits; }
    __forceinline const c_player_traits *get_vip_influence_traits() const { return &m_vip_influence_traits; }
    __forceinline const c_player_traits *get_vip_traits() const { return &m_vip_traits; }

    char m_base_padding[4];
    short m_score_to_win_round;
    enum e_vip_variant_flags {
        _vip_variant_single_vip = 0,
        _vip_variant_destination_zones_enabled,
        _vip_variant_end_round_on_vip_death,
        k_vip_variant_flags
    };
    c_flags_no_init<e_vip_variant_flags, short, k_vip_variant_flags> m_variant_flags;
    signed char m_kill_points;
    signed char m_takedown_points;
    signed char m_kill_as_vip_points;
    signed char m_vip_death_points;
    signed char m_destination_arrival_points;
    signed char m_suicide_points;
    signed char m_betrayal_points;
    signed char m_vip_suicide_points;
    unsigned char m_vip_selection;
    unsigned char m_zone_movement;
    c_enum_no_init<e_vip_zone_order_setting, unsigned char, _vip_zone_order_random, k_vip_zone_order_settings_count> m_zone_order;
    unsigned short m_influence_radius;
    c_player_traits m_vip_team_traits;
    c_player_traits m_vip_influence_traits;
    c_player_traits m_vip_traits;
};

typedef char vip_variant_score_offset[(sizeof(void *) != 4 || (offsetof(c_game_engine_vip_variant, m_score_to_win_round) == 0x1b0)) ? 1 : -1];
typedef char vip_variant_radius_offset[(sizeof(void *) != 4 || (offsetof(c_game_engine_vip_variant, m_influence_radius) == 0x1c0)) ? 1 : -1];
typedef char vip_variant_team_traits_offset[(sizeof(void *) != 4 || (offsetof(c_game_engine_vip_variant, m_vip_team_traits) == 0x1c2)) ? 1 : -1];
typedef char vip_variant_size[(sizeof(void *) != 4 || (sizeof(c_game_engine_vip_variant) == 0x218)) ? 1 : -1];

#endif
