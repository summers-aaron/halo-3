#ifndef GAME_GAME_ENGINE_ODDBALL_TRAITS_INTERNALS_H
#define GAME_GAME_ENGINE_ODDBALL_TRAITS_INTERNALS_H

#include "game/player_traits.h"

class c_bitstream;
struct s_saved_game_item_metadata;

struct oddball_unit_slayer_definition
{
    char base[56];
    short team_scoring;
    short score_to_win;
    signed char kill;
    signed char assist;
    signed char death;
    signed char suicide;
    signed char betrayal;
    signed char leader_killed;
    signed char elimination;
    signed char assassination;
    signed char headshot;
    signed char melee;
    signed char sticky;
    signed char splatter;
    signed char killing_spree;
    char alignment_bytes[3];
    long leader_traits;
};

struct oddball_unit_slayer_variant
{
    char base[426];
    short team_scoring;
    char alignment_bytes[4];
    short score_to_win;
    short kill;
    signed char assist;
    signed char death;
    signed char suicide;
    signed char betrayal;
    signed char leader_killed;
    signed char elimination;
    signed char assassination;
    signed char headshot;
    signed char melee;
    signed char sticky;
    signed char splatter;
    signed char killing_spree;
    c_player_traits leader_traits;

    __forceinline void set_team_scoring(long value)
    {
        if ((unsigned long)value <= 3)
            team_scoring = (short)value;
        else
            team_scoring = 0;
    }

    __forceinline void set_score_to_win(short value)
    {
        if (value < -1 || value > 250)
            value = 25;
        score_to_win = value;
    }

    __forceinline void set_kill_points(short value)
    {
        if (value < -10 || value > 10)
            value = 1;
        kill = value;
    }

    __forceinline void set_assist_points(signed char value) { if (value >= -10 && value <= 10) assist = value; else assist = 0; }
    __forceinline void set_death_points(signed char value) { if (value >= -10 && value <= 10) death = value; else death = 0; }
    __forceinline void set_suicide_points(signed char value) { if (value >= -10 && value <= 10) suicide = value; else suicide = -1; }
    __forceinline void set_betrayal_points(signed char value) { if (value >= -10 && value <= 10) betrayal = value; else betrayal = -1; }
    __forceinline void set_leader_killed_points(signed char value) { if (value >= -10 && value <= 10) leader_killed = value; else leader_killed = 0; }
    __forceinline void set_elimination_points(signed char value) { if (value >= -10 && value <= 10) elimination = value; else elimination = 0; }
    __forceinline void set_assassination_points(signed char value) { if (value >= -10 && value <= 10) assassination = value; else assassination = 0; }
    __forceinline void set_headshot_points(signed char value) { if (value >= -10 && value <= 10) headshot = value; else headshot = 0; }
    __forceinline void set_melee_points(signed char value) { if (value >= -10 && value <= 10) melee = value; else melee = 0; }
    __forceinline void set_sticky_points(signed char value) { if (value >= -10 && value <= 10) sticky = value; else sticky = 0; }
    __forceinline void set_splatter_points(signed char value) { if (value >= -10 && value <= 10) splatter = value; else splatter = 0; }
    __forceinline void set_killing_spree_points(signed char value) { if (value >= -10 && value <= 10) killing_spree = value; else killing_spree = 0; }
};

extern "C" void saved_game_files_encode_metadata(const s_saved_game_item_metadata *metadata, c_bitstream *packet);
extern "C" void game_engine_base_variant_set(oddball_unit_slayer_variant *variant, const oddball_unit_slayer_definition *definition, long engine_index);
extern "C" void game_engine_slayer_variant_set_from_definition(oddball_unit_slayer_variant *variant, const oddball_unit_slayer_definition *definition);

#endif // GAME_GAME_ENGINE_ODDBALL_TRAITS_INTERNALS_H
