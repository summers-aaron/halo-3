#ifndef GAME_GAME_ENGINE_KING_VARIANT_H
#define GAME_GAME_ENGINE_KING_VARIANT_H

#include "game/game_engine_base_variant.h"
#include "game/game_engine_variant_definitions.h"

enum e_king_flags_settings { _king_opaque_hill = 0 };

// King variant settings as authored in the multiplayer game engine settings tag.
struct s_game_engine_king_variant_definition
{
    s_game_engine_base_variant_definition base;
    unsigned long flags;
    short score_to_win;
    short team_scoring;
    short moving_hill;
    short moving_hill_order;
    signed char inside_hill_points;
    signed char outside_hill_points;
    signed char uncontested_hill_bonus;
    signed char kill_points;
    long inside_hill_traits;
};

// King variant hill and scoring options with inside-hill player traits.
class c_game_engine_king_variant : public c_game_engine_base_variant
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
    void set(const c_game_engine_king_variant *source);
    void set(const s_game_engine_king_variant_definition *definition);
    short get_score_to_win() const { return m_score_to_win; }

    char m_base_padding[4];
    c_flags_no_init<e_king_flags_settings, long, 1> m_flags;
    short m_score_to_win;
    signed char m_moving_hill;
    signed char m_moving_hill_order;
    signed char m_inside_hill_points;
    signed char m_outside_hill_points;
    signed char m_uncontested_hill_bonus;
    signed char m_kill_points;
    c_player_traits m_inside_hill_traits;
};

typedef char king_variant_score_offset[(sizeof(void *) != 4 || ((offsetof(c_game_engine_king_variant, m_score_to_win) == 0x1b4))) ? 1 : -1];
typedef char king_variant_inside_traits_offset[(sizeof(void *) != 4 || ((offsetof(c_game_engine_king_variant, m_inside_hill_traits) == 0x1bc))) ? 1 : -1];
typedef char king_variant_size[(sizeof(void *) != 4 || ((sizeof(c_game_engine_king_variant) == 0x1d8))) ? 1 : -1];
typedef char king_variant_definition_size[(sizeof(long) != 4 || sizeof(s_game_engine_king_variant_definition) == 0x4c) ? 1 : -1];

#endif
