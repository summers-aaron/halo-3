#ifndef GAME_GAME_ENGINE_SANDBOX_VARIANT_H
#define GAME_GAME_ENGINE_SANDBOX_VARIANT_H

#include <stddef.h>

#include "game/game_engine_base_variant.h"

class c_game_engine_sandbox_variant : public c_game_engine_base_variant
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
    virtual bool can_add_to_recent_list() const { return false; }
    virtual long get_score_to_win_round() const { return 0; }
    void set(const c_game_engine_sandbox_variant *source);

    char m_base_padding[4];
    enum e_sandbox_variant_flags { _sandbox_variant_open_channel_voice = 0, k_sandbox_variant_flags = 1 };
    c_flags_no_init<e_sandbox_variant_flags, char, k_sandbox_variant_flags> m_variant_flags;
    enum e_sandbox_edit_mode_settings { _sandbox_edit_mode_all_players = 0, _sandbox_edit_mode_leader_only = 1, k_sandbox_edit_mode_settings = 2 };
    c_enum<e_sandbox_edit_mode_settings, char, 0, k_sandbox_edit_mode_settings> m_edit_mode;
    short m_respawn_time;
    c_player_traits m_player_traits;
};

typedef char game_engine_sandbox_variant_size[(sizeof(void *) != 4 || ((sizeof(c_game_engine_sandbox_variant) == 0x1d0) )) ? 1 : -1];
typedef char game_engine_sandbox_variant_off_flags[(sizeof(void *) != 4 || ((offsetof(c_game_engine_sandbox_variant, m_variant_flags) == 0x1b0) )) ? 1 : -1];
typedef char game_engine_sandbox_variant_off_traits[(sizeof(void *) != 4 || ((offsetof(c_game_engine_sandbox_variant, m_player_traits) == 0x1b4) )) ? 1 : -1];

#endif // GAME_GAME_ENGINE_SANDBOX_VARIANT_H
