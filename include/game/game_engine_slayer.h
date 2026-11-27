#ifndef GAME_GAME_ENGINE_SLAYER_H
#define GAME_GAME_ENGINE_SLAYER_H

#include <stddef.h>

#include "game/game_engine_base_variant.h"
#include "game/game_engine_default.h"
#include "game/game_engine_kill_flags_types.h"
#include "cseries/cseries_wrapped.h"

class c_game_engine_slayer_variant : public c_game_engine_base_variant
{
protected:
    virtual long get_game_engine_name_string_id() const;
    virtual long get_game_engine_default_description_string_id() const;
public:
    // @address 0x267a58 ?set@c_game_engine_slayer_variant@@QAAXPBV1@@Z
    void set(const c_game_engine_slayer_variant *variant);
    virtual void initialize();
    virtual void validate();
    virtual void encode(c_bitstream *packet) const;
    virtual void decode(c_bitstream *packet);
    virtual bool can_be_cast_to(e_game_engine_type engine_type, const void **variant) const;
    virtual long get_score_to_win_round() const;

    __forceinline short get_score_to_win() const { return m_score_to_win; }
    __forceinline void set_score_to_win(short score_to_win)
    {
        if (score_to_win < -1 || score_to_win > 250)
            score_to_win = 25;
        m_score_to_win = score_to_win;
    }

    __forceinline short get_kill_points() const { return m_kill_points; }
    __forceinline void set_kill_points(short kill_points)
    {
        if (kill_points < -10 || kill_points > 10)
            kill_points = 1;
        m_kill_points = kill_points;
    }

    __forceinline char get_assist_points() const { return m_assist_points; }
    __forceinline void set_assist_points(char assist_points)
    {
        if (assist_points >= -10 && assist_points <= 10)
            m_assist_points = assist_points;
        else
            m_assist_points = 0;
    }

    __forceinline char get_death_points() const { return m_death_points; }
    __forceinline void set_death_points(char death_points)
    {
        if (death_points >= -10 && death_points <= 10)
            m_death_points = death_points;
        else
            m_death_points = 0;
    }

    __forceinline char get_suicide_points() const { return m_suicide_points; }
    __forceinline void set_suicide_points(char suicide_points)
    {
        if (suicide_points >= -10 && suicide_points <= 10)
            m_suicide_points = suicide_points;
        else
            m_suicide_points = -1;
    }

    __forceinline char get_betrayal_points() const { return m_betrayal_points; }
    __forceinline void set_betrayal_points(char betrayal_points)
    {
        if (betrayal_points >= -10 && betrayal_points <= 10)
            m_betrayal_points = betrayal_points;
        else
            m_betrayal_points = -1;
    }

    __forceinline char get_leader_killed_points() const { return m_leader_killed_points; }
    __forceinline void set_leader_killed_points(char leader_killed_points)
    {
        if (leader_killed_points >= -10 && leader_killed_points <= 10)
            m_leader_killed_points = leader_killed_points;
        else
            m_leader_killed_points = 0;
    }

    __forceinline char get_elimination_points() const { return m_elimination_points; }
    __forceinline void set_elimination_points(char elimination_points)
    {
        if (elimination_points >= -10 && elimination_points <= 10)
            m_elimination_points = elimination_points;
        else
            m_elimination_points = 0;
    }

    __forceinline char get_assassination_points() const { return m_assassination_points; }
    __forceinline void set_assassination_points(char assassination_points)
    {
        if (assassination_points >= -10 && assassination_points <= 10)
            m_assassination_points = assassination_points;
        else
            m_assassination_points = 0;
    }

    __forceinline char get_headshot_points() const { return m_headshot_points; }
    __forceinline void set_headshot_points(char headshot_points)
    {
        if (headshot_points >= -10 && headshot_points <= 10)
            m_headshot_points = headshot_points;
        else
            m_headshot_points = 0;
    }

    __forceinline char get_melee_points() const { return m_melee_points; }
    __forceinline void set_melee_points(char melee_points)
    {
        if (melee_points >= -10 && melee_points <= 10)
            m_melee_points = melee_points;
        else
            m_melee_points = 0;
    }

    __forceinline char get_sticky_points() const { return m_sticky_points; }
    __forceinline void set_sticky_points(char sticky_points)
    {
        if (sticky_points >= -10 && sticky_points <= 10)
            m_sticky_points = sticky_points;
        else
            m_sticky_points = 0;
    }

    __forceinline char get_splatter_points() const { return m_splatter_points; }
    __forceinline void set_splatter_points(char splatter_points)
    {
        if (splatter_points >= -10 && splatter_points <= 10)
            m_splatter_points = splatter_points;
        else
            m_splatter_points = 0;
    }

    __forceinline char get_killing_spree_points() const { return m_killing_spree_points; }
    __forceinline void set_killing_spree_points(char killing_spree_points)
    {
        if (killing_spree_points >= -10 && killing_spree_points <= 10)
            m_killing_spree_points = killing_spree_points;
        else
            m_killing_spree_points = 0;
    }

    __forceinline const c_player_traits *get_leader_traits() const { return &m_leader_traits; }
    __forceinline void set_leader_traits(const c_player_traits *traits, bool force)
    {
        m_leader_traits.set(traits, force);
    }

#ifdef HALO3_64BIT
    char m_base_padding[4];
#else
    char m_base_padding[0x1b0 - sizeof(c_game_engine_base_variant)];
#endif
    short m_score_to_win;
    short m_kill_points;
    char m_assist_points;
    char m_death_points;
    char m_suicide_points;
    char m_betrayal_points;
    char m_leader_killed_points;
    char m_elimination_points;
    char m_assassination_points;
    char m_headshot_points;
    char m_melee_points;
    char m_sticky_points;
    char m_splatter_points;
    char m_killing_spree_points;
    c_player_traits m_leader_traits;
};

#ifdef HALO3_64BIT
typedef char game_engine_slayer_variant_size[(sizeof(c_game_engine_slayer_variant) == 0x1e0) ? 1 : -1];
typedef char game_engine_slayer_variant_off_score[(offsetof(c_game_engine_slayer_variant, m_score_to_win) == 0x1b4) ? 1 : -1];
typedef char game_engine_slayer_variant_off_leader_traits[(offsetof(c_game_engine_slayer_variant, m_leader_traits) == 0x1c4) ? 1 : -1];
#else
typedef char game_engine_slayer_variant_size[(sizeof(c_game_engine_slayer_variant) == 0x1dc) ? 1 : -1];
typedef char game_engine_slayer_variant_off_score[(offsetof(c_game_engine_slayer_variant, m_score_to_win) == 0x1b0) ? 1 : -1];
typedef char game_engine_slayer_variant_off_leader_traits[(offsetof(c_game_engine_slayer_variant, m_leader_traits) == 0x1c0) ? 1 : -1];
#endif

// The active slayer variant (March asserts "slayer_variant==NULL", game_engine_slayer.cpp:118), set by
// c_slayer_engine::initialize_for_new_map.
// image keeps it and game_engine_slayer.cpp names it with // @address.
extern c_game_engine_slayer_variant *slayer_variant;

class c_slayer_engine : public c_game_engine
{
public:
    virtual long get_type() const;
    virtual bool initialize_for_new_map() const;
    virtual void dispose_from_old_map() const;
    virtual void apply_game_engine_traits_for_player(long player_index, c_player_traits *player_traits) const;
    virtual void player_killed_player(long killer, long dead_unit, long victim, bool betrayal,
        c_flags<e_game_engine_kill_flags, long, 24> flags, long spree) const;
    virtual void player_assisted_with_kill(long player_index) const;
    virtual bool test_flag(long flag) const;
    virtual e_simulation_entity_type get_simulation_entity_type() const;
    virtual void build_simulation_baseline(long state_data_size, void *state_data) const;
    virtual void build_simulation_update(unsigned long *update_mask, long state_data_size, void *state_data) const;
    virtual bool apply_simulation_update(unsigned long update_mask, long state_data_size, const void *state_data) const;
    virtual void emit_game_start_event(long player_index) const;
};

extern c_slayer_engine internal_slayer_engine;

#endif // GAME_GAME_ENGINE_SLAYER_H
