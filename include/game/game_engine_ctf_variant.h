#ifndef GAME_GAME_ENGINE_CTF_VARIANT_H
#define GAME_GAME_ENGINE_CTF_VARIANT_H

#include "game/game_engine_base_variant.h"

enum e_ctf_option_a { _ctf_option_a_default = 0 };
enum e_ctf_option_b { _ctf_option_b_default = 0 };
enum e_ctf_option_c { _ctf_option_c_default = 0 };
enum e_ctf_variant_flags { _ctf_variant_flags_0 = 0, k_ctf_variant_flags = 1 };

class c_game_engine_ctf_variant : public c_game_engine_base_variant
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
    virtual void custom_team_score_stats(long team_index, long old_score, long new_score) const;
public:
    __forceinline const c_player_traits *get_carrier_traits() const { return &m_carrier_traits; }

#ifdef HALO3_64BIT
    char m_base_padding[4];
#else
    char m_base_padding[0x1b0 - sizeof(c_game_engine_base_variant)];
#endif
    c_flags_no_init<e_ctf_variant_flags, signed char, k_ctf_variant_flags> m_flags;
    c_enum<e_ctf_option_a, signed char, 0, 301> m_option_a;
    c_enum<e_ctf_option_b, signed char, 0, 301> m_option_b;
    c_enum<e_ctf_option_c, signed char, 0, 301> m_option_c;
    unsigned short m_option_d;
    unsigned short m_option_e;
    short m_option_f;
    short m_option_g;
    c_player_traits m_carrier_traits;
};

#endif
