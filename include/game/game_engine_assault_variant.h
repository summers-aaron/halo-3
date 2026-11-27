#ifndef GAME_GAME_ENGINE_ASSAULT_VARIANT_H
#define GAME_GAME_ENGINE_ASSAULT_VARIANT_H

#include "game/game_engine_base_variant.h"

enum e_assault_game_type { _assault_game_type_default = 0 };
enum e_assault_respawn { _assault_respawn_default = 0 };
enum e_assault_enemy_bomb_waypoint { _assault_enemy_bomb_waypoint_default = 1 };
class c_game_engine_assault_variant : public c_game_engine_base_variant
{
protected:
    virtual long get_game_engine_name_string_id() const;
    virtual long get_game_engine_default_description_string_id() const;
public:
    void set(const c_game_engine_assault_variant *variant);
    virtual void initialize();
    virtual void validate();
    virtual void encode(c_bitstream *packet) const;
    virtual void decode(c_bitstream *packet);
    virtual bool can_be_cast_to(e_game_engine_type engine_type, const void **variant) const;
    virtual long get_score_to_win_round() const;

    __forceinline void set_game_type(long value)
    {
        if (value >= 0 && value < 3)
            m_game_type = (e_assault_game_type)value;
        else
            m_game_type = _assault_game_type_default;
    }
    __forceinline void set_respawn(long value) { if (value >= 0 && value < 4) m_respawn = (e_assault_respawn)value; else m_respawn = _assault_respawn_default; }
    __forceinline void set_enemy_bomb_waypoint(long value)
    {
        if ((unsigned long)value > 3)
            value = 1;
        m_enemy_bomb_waypoint = (e_assault_enemy_bomb_waypoint)value;
    }
    __forceinline void set_reset_bomb_on_disarm(bool value) { m_assault_flags.set(0, value); }
    __forceinline short get_score_to_win() const { return m_score_to_win; }
    __forceinline void set_score_to_win(short value) { if (value < 0 || value > 50) value = 3; m_score_to_win = value; }
    __forceinline void set_sudden_death_time(short value) { if (value >= -1 && value <= 300) m_sudden_death_time = value; else m_sudden_death_time = 0; }
    __forceinline void set_bomb_arming_time(short value) { if (value >= 0 && value <= 30) m_bomb_arming_time = value; else m_bomb_arming_time = 5; }
    __forceinline void set_bomb_disarming_time(short value) { if (value >= 0 && value <= 30) m_bomb_disarming_time = value; else m_bomb_disarming_time = 5; }
    __forceinline void set_bomb_fuse_time(short value) { if (value >= 0 && value <= 30) m_bomb_fuse_time = value; else m_bomb_fuse_time = 5; }
    __forceinline void set_bomb_reset_time(short value) { if (value < 0 || value > 60) value = 30; m_bomb_reset_time = value; }

    __forceinline const c_player_traits *get_carrier_traits() const { return &m_carrier_traits; }
    __forceinline void set_carrier_traits(const c_player_traits *traits, bool force)
    {
        m_carrier_traits.set(traits, force);
    }

    __forceinline const c_player_traits *get_arming_traits() const { return &m_arming_traits; }
    __forceinline void set_arming_traits(const c_player_traits *traits, bool force)
    {
        m_arming_traits.set(traits, force);
    }

private:
#ifdef HALO3_64BIT
    char m_base_padding[4];
#else
    char m_base_padding[0x1b0 - sizeof(c_game_engine_base_variant)];
#endif
public:
    c_flags_no_init<long, short, 1> m_assault_flags;
    c_enum<e_assault_respawn, short, 0, 4> m_respawn;
    c_enum<e_assault_game_type, short, 0, 4> m_game_type;
    c_enum<e_assault_enemy_bomb_waypoint, short, 0, 4> m_enemy_bomb_waypoint;
    short m_score_to_win;
    short m_sudden_death_time;
    short m_bomb_reset_time;
    short m_bomb_arming_time;
    short m_bomb_disarming_time;
    short m_bomb_fuse_time;
    c_player_traits m_carrier_traits;
    c_player_traits m_arming_traits;
};

#endif
