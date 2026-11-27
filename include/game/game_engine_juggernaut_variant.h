#ifndef GAME_GAME_ENGINE_JUGGERNAUT_VARIANT_H
#define GAME_GAME_ENGINE_JUGGERNAUT_VARIANT_H

#include "game/game_engine_base_variant.h"

#include "game/game_engine_juggernaut_variant_types.h"

class c_game_engine_juggernaut_variant : public c_game_engine_base_variant
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

    void set(const c_game_engine_juggernaut_variant *variant);

    __forceinline bool get_allied_against_juggernaut() const { return m_flags.test(_juggernaut_flags_allied_against_juggernaut); }
    __forceinline void set_allied_against_juggernaut(bool allied_against_juggernaut)
    {
        m_flags.set(_juggernaut_flags_allied_against_juggernaut, allied_against_juggernaut);
    }

    __forceinline bool get_respawn_on_lone_juggernaut() const { return m_flags.test(_juggernaut_flags_respawn_on_lone_juggernaut); }
    __forceinline void set_respawn_on_lone_juggernaut(bool respawn_on_lone_juggernaut)
    {
        m_flags.set(_juggernaut_flags_respawn_on_lone_juggernaut, respawn_on_lone_juggernaut);
    }

    __forceinline bool get_destination_zones_enabled() const { return m_flags.test(_juggernaut_flags_destination_zones_enabled); }
    __forceinline void set_destination_zones_enabled(bool destination_zones_enabled)
    {
        m_flags.set(_juggernaut_flags_destination_zones_enabled, destination_zones_enabled);
    }

    __forceinline void set_score_to_win_round(short score_to_win_round)
    {
        if (score_to_win_round < 0 || score_to_win_round > 500)
            score_to_win_round = 15;
        m_score_to_win = score_to_win_round;
    }

    __forceinline e_juggernaut_initial_juggernaut_settings get_initial_juggernaut() const
    {
        return (e_juggernaut_initial_juggernaut_settings)m_initial_juggernaut.m_storage;
    }
    __forceinline void set_initial_juggernaut(e_juggernaut_initial_juggernaut_settings initial_juggernaut)
    {
        if ((unsigned long)initial_juggernaut <= 2)
            m_initial_juggernaut.m_storage = (unsigned char)initial_juggernaut;
        else
            m_initial_juggernaut.m_storage = 0;
    }

    __forceinline e_juggernaut_next_juggernaut_settings get_next_juggernaut() const
    {
        return (e_juggernaut_next_juggernaut_settings)m_next_juggernaut.m_storage;
    }
    __forceinline void set_next_juggernaut(e_juggernaut_next_juggernaut_settings next_juggernaut)
    {
        if ((unsigned long)next_juggernaut <= 3)
            m_next_juggernaut.m_storage = (unsigned char)next_juggernaut;
        else
            m_next_juggernaut.m_storage = 0;
    }

    __forceinline e_juggernaut_zone_movement_settings get_zone_movement() const { return (e_juggernaut_zone_movement_settings)m_zone_movement.m_storage; }
    __forceinline void set_zone_movement(e_juggernaut_zone_movement_settings zone_movement)
    {
        if ((unsigned long)zone_movement <= 10)
            m_zone_movement.m_storage = (unsigned char)zone_movement;
        else
            m_zone_movement.m_storage = 0;
    }

    __forceinline e_juggernaut_zone_order_settings get_zone_order() const { return (e_juggernaut_zone_order_settings)m_zone_order.m_storage; }
    __forceinline void set_zone_order(e_juggernaut_zone_order_settings zone_order)
    {
        if ((unsigned long)zone_order <= 1)
            m_zone_order.m_storage = (unsigned char)zone_order;
        else
            m_zone_order.m_storage = 1;
    }

    __forceinline char get_kill_points() const { return m_kill_points; }
    __forceinline void set_kill_points(char kill_points)
    {
        if (kill_points >= -10 && kill_points <= 10)
            m_kill_points = kill_points;
        else
            m_kill_points = 0;
    }

    __forceinline char get_juggernaut_kill_points() const { return m_juggernaut_kill_points; }
    __forceinline void set_juggernaut_kill_points(char juggernaut_kill_points)
    {
        if (juggernaut_kill_points >= -10)
            {
                if (juggernaut_kill_points <= 10)
                    m_juggernaut_kill_points = juggernaut_kill_points;
                else
                    m_juggernaut_kill_points = 1;
            }
        else
            m_juggernaut_kill_points = 1;
    }

    __forceinline char get_kill_as_juggernaut_points() const { return m_kill_as_juggernaut_points; }
    __forceinline void set_kill_as_juggernaut_points(char kill_as_juggernaut_points)
    {
        if (kill_as_juggernaut_points >= -10)
            {
                if (kill_as_juggernaut_points <= 10)
                    m_kill_as_juggernaut_points = kill_as_juggernaut_points;
                else
                    m_kill_as_juggernaut_points = 1;
            }
        else
            m_kill_as_juggernaut_points = 1;
    }

    __forceinline char get_destination_arrival_points() const { return m_destination_arrival_points; }
    __forceinline void set_destination_arrival_points(char destination_arrival_points)
    {
        if (destination_arrival_points >= -10)
            {
                if (destination_arrival_points <= 10)
                    m_destination_arrival_points = destination_arrival_points;
                else
                    m_destination_arrival_points = 1;
            }
        else
            m_destination_arrival_points = 1;
    }

    __forceinline char get_suicide_points() const { return m_suicide_points; }
    __forceinline void set_suicide_points(char suicide_points)
    {
        if (suicide_points >= -10)
            {
                if (suicide_points <= 10)
                    m_suicide_points = suicide_points;
                else
                    m_suicide_points = 1;
            }
        else
            m_suicide_points = 1;
    }

    __forceinline char get_betrayal_points() const { return m_betrayal_points; }
    __forceinline void set_betrayal_points(char betrayal_points)
    {
        if (betrayal_points >= -10)
            {
                if (betrayal_points <= 10)
                    m_betrayal_points = betrayal_points;
                else
                    m_betrayal_points = 1;
            }
        else
            m_betrayal_points = 1;
    }

    __forceinline unsigned char get_juggernaut_delay() const { return m_juggernaut_delay; }
    __forceinline void set_juggernaut_delay(unsigned char juggernaut_delay)
    {
        if (juggernaut_delay <= 10)
            m_juggernaut_delay = juggernaut_delay;
        else
            m_juggernaut_delay = 0;
    }

    __forceinline const c_player_traits *get_juggernaut_traits() const { return &m_juggernaut_traits; }
    __forceinline void set_juggernaut_traits(const c_player_traits *traits, bool force)
    {
        m_juggernaut_traits.set(traits, force);
    }

    char m_base_padding[4];
    short m_score_to_win;
    short m_pad1b2;
    c_enum<e_juggernaut_initial_juggernaut_settings, unsigned char, 0, 3> m_initial_juggernaut;
    c_enum<e_juggernaut_next_juggernaut_settings, unsigned char, 0, 4> m_next_juggernaut;
    c_flags_no_init<e_juggernaut_flags, unsigned char, 3> m_flags;
    c_enum<e_juggernaut_zone_movement_settings, unsigned char, 0, 11> m_zone_movement;
    c_enum<e_juggernaut_zone_order_settings, unsigned char, 0, 2> m_zone_order;
    char m_kill_points;
    char m_juggernaut_kill_points;
    char m_kill_as_juggernaut_points;
    char m_destination_arrival_points;
    char m_suicide_points;
    char m_betrayal_points;
    unsigned char m_juggernaut_delay;
    c_player_traits m_juggernaut_traits;
};

typedef char juggernaut_initial_offset[(sizeof(void *) != 4 || offsetof(c_game_engine_juggernaut_variant, m_initial_juggernaut) == 0x1b4) ? 1 : -1];
extern "C" c_game_engine_juggernaut_variant *g_juggernaut_variant;
extern "C" const float k_juggernaut_delay_rounding_bias;

#endif // GAME_GAME_ENGINE_JUGGERNAUT_VARIANT_H
