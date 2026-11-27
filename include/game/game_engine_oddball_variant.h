#ifndef GAME_GAME_ENGINE_ODDBALL_VARIANT_H
#define GAME_GAME_ENGINE_ODDBALL_VARIANT_H

#include "game/game_engine_base_variant.h"

enum e_oddball_variant_flags
{
    _oddball_variant_flags_auto_ball_pickup = 0,
    _oddball_variant_flags_ball_effect_enabled,
    k_oddball_variant_flags
};

class c_game_engine_oddball_variant : public c_game_engine_base_variant
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
    virtual void custom_team_score_stats(long team_index, long first_value, long last_value) const;
    void set(const c_game_engine_oddball_variant *variant);

    __forceinline bool get_auto_ball_pickup() const { return m_flags.test(_oddball_variant_flags_auto_ball_pickup); }
    __forceinline void set_auto_ball_pickup(bool auto_ball_pickup) { m_flags.set(_oddball_variant_flags_auto_ball_pickup, auto_ball_pickup); }
    __forceinline bool get_ball_effect_enabled() const { return m_flags.test(_oddball_variant_flags_ball_effect_enabled); }
    __forceinline void set_ball_effect_enabled(bool ball_effect_enabled) { m_flags.set(_oddball_variant_flags_ball_effect_enabled, ball_effect_enabled); }

    __forceinline short get_score_to_win() const { return m_score_to_win; }
    __forceinline void set_score_to_win(short score_to_win)
    {
        if (score_to_win < -1 || score_to_win > 1000)
            score_to_win = 200;
        m_score_to_win = score_to_win;
    }

    __forceinline short get_carrying_points() const { return m_carrying_points; }
    __forceinline void set_carrying_points(short carrying_points)
    {
        if (carrying_points >= -10 && carrying_points <= 10)
            m_carrying_points = carrying_points;
        else
            m_carrying_points = 1;
    }

    __forceinline char get_kill_points() const { return m_kill_points; }
    __forceinline void set_kill_points(char kill_points)
    {
        if (kill_points >= -10 && kill_points <= 10)
            m_kill_points = kill_points;
        else
            m_kill_points = 0;
    }

    __forceinline char get_ball_kill_points() const { return m_ball_kill_points; }
    __forceinline void set_ball_kill_points(char ball_kill_points)
    {
        if (ball_kill_points >= -10 && ball_kill_points <= 10)
            m_ball_kill_points = ball_kill_points;
        else
            m_ball_kill_points = 0;
    }

    __forceinline char get_carrier_kill_points() const { return m_carrier_kill_points; }
    __forceinline void set_carrier_kill_points(char carrier_kill_points)
    {
        if (carrier_kill_points >= -10 && carrier_kill_points <= 10)
            m_carrier_kill_points = carrier_kill_points;
        else
            m_carrier_kill_points = 0;
    }

    __forceinline char get_ball_count() const { return m_ball_count; }
    __forceinline void set_ball_count(char ball_count)
    {
        if (ball_count >= 1 && ball_count <= 3)
            m_ball_count = ball_count;
        else
            m_ball_count = 1;
    }

    __forceinline short get_ball_spawn_delay() const { return m_ball_spawn_delay; }
    __forceinline void set_ball_spawn_delay(short ball_spawn_delay)
    {
        if (ball_spawn_delay < 0 || ball_spawn_delay > 120)
            ball_spawn_delay = 5;
        m_ball_spawn_delay = ball_spawn_delay;
    }

    __forceinline short get_ball_inactive_respawn_delay() const { return m_ball_inactive_respawn_delay; }
    __forceinline void set_ball_inactive_respawn_delay(short ball_inactive_respawn_delay)
    {
        if (ball_inactive_respawn_delay < 0 || ball_inactive_respawn_delay > 120)
            ball_inactive_respawn_delay = 30;
        m_ball_inactive_respawn_delay = ball_inactive_respawn_delay;
    }

    __forceinline const c_player_traits *get_carrier_traits() const { return &m_carrier_traits; }
    __forceinline void set_carrier_traits(const c_player_traits *traits, bool force)
    {
        m_carrier_traits.set(traits, force);
    }

#ifdef HALO3_64BIT
    char m_base_padding[4];
#else
    char m_base_padding[0x1b0 - sizeof(c_game_engine_base_variant)];
#endif
    c_flags_no_init<e_oddball_variant_flags, unsigned long, k_oddball_variant_flags> m_flags;
    short m_score_to_win;
    short m_carrying_points;
    char m_kill_points;
    char m_ball_kill_points;
    char m_carrier_kill_points;
    char m_ball_count;
    short m_ball_spawn_delay;
    short m_ball_inactive_respawn_delay;
    c_player_traits m_carrier_traits;
};

#endif
