#ifndef GAME_GAME_VARIANT_H
#define GAME_GAME_VARIANT_H

#include <stddef.h>

#include "game/game_variant_parameter_types.h"

#include <string.h>
#include "cseries/cseries_wrapped.h"
#include "game/game_types.h"
#include "game/player_traits.h"
#include "interface/text_value_pair_types.h"

class c_bitstream;
struct s_game_engine_setting_variant;

class c_game_engine_base_variant;
class c_game_engine_ctf_variant;
class c_game_engine_slayer_variant;
class c_game_engine_oddball_variant;
class c_game_engine_king_variant;
class c_game_engine_sandbox_variant;
class c_game_engine_vip_variant;
class c_game_engine_juggernaut_variant;
class c_game_engine_territories_variant;
class c_game_engine_assault_variant;
class c_game_engine_infection_variant;

class c_game_variant {
public:
    union {
        long m_game_engine_index;
        long engine_type;
        long game_engine_index;
    };
    // Four-byte packing keeps the storage union aligned as before: the
    // eight-byte metadata fields below must not raise its alignment.
#pragma pack(push, 4)
    union {
        char m_variant_storage[0x260];
        char variant_storage[0x260];
        char storage[0x260];
        struct {
            // Overlaid c_game_engine_base_variant head: vtable pointer, checksum,
            // then the saved-game metadata (name, description, author, ...).
            // First word is the overlaid variant's vtable pointer, written by
            // recreate_variant_vtable_for_game_engine_index; it has no declared
            // field name of its own, so it stays unnamed.
#ifdef HALO3_64BIT
            long : 32;
            long : 32;
#else
            long : 32;
#endif
            long m_checksum;
            unsigned __int64 unique_id;
            union {
                wchar_t m_name[16];
                wchar_t name[16];
            };
            char description[128];
            char author[16];
            long file_type;
            bool author_is_online;
            // The compiler pads here before author_id, as in s_saved_game_item_metadata.
            unsigned __int64 author_id;
            unsigned __int64 size_in_bytes;
            unsigned __int64 date;
            long length_seconds;
            long campaign_id;
            long map_id;
            long game_engine_type;
            long campaign_difficulty;
            short campaign_insertion_point;
            short campaign_survival_enabled;
            unsigned __int64 game_id;
            unsigned char flags;
            unsigned char round_time_limit_minutes;
            unsigned char round_limit;                      // miscellaneous options (March get_round_limit, +2)
            unsigned char early_victory_win_count;
            unsigned char respawn_option_flags;             // respawn options m_flags (inherit time, with teammate, at location, on kills)
            unsigned char lives_per_round;
            unsigned char team_lives_per_round;
            unsigned char respawn_time_seconds;
            unsigned char suicide_penalty_seconds;
            unsigned char betrayal_penalty_seconds;
            unsigned char respawn_growth_seconds;
            unsigned char respawn_player_traits_duration_seconds; // (March get_respawn_player_traits_duration_seconds, +7)
            c_player_traits respawn_player_traits;          // respawn options m_respawn_player_traits (March get_respawn_player_traits, +8)
            unsigned short social_options_flags;            // social options m_flags
            short team_changing;                            // social options m_team_changing
            unsigned long variant_flags;                    // map override options m_flags
            c_player_traits base_player_traits;             // map override options (March get_base_player_traits, +4)
            short weapon_set_absolute_index;
            unsigned short vehicle_set_absolute_index;
            c_player_traits red_powerup_traits;             // (March get_red_powerup_traits, +0x24)
            c_player_traits blue_powerup_traits;            // (March get_blue_powerup_traits, +0x40)
            c_player_traits yellow_powerup_traits;          // (March get_yellow_powerup_traits, +0x5c)
            unsigned char red_powerup_duration_seconds;
            unsigned char blue_powerup_duration_seconds;
            unsigned char yellow_powerup_duration_seconds;
            unsigned short base_variant_flags;
            short team_scoring_method;
        };
    };
#pragma pack(pop)

    c_game_variant()
    {
        m_game_engine_index = 0;
        memset(m_variant_storage, 0, sizeof(m_variant_storage));
        memset(m_variant_storage, 0, sizeof(m_variant_storage));
    }

    __forceinline long get_game_engine_index() const { return m_game_engine_index; }
    __forceinline const wchar_t *get_name() const { return m_name; }
    __forceinline const c_game_engine_base_variant *get_active_variant() const {
        return (const c_game_engine_base_variant *)m_variant_storage;
    }
    c_game_engine_base_variant *get_active_variant_writeable();
    __forceinline c_game_engine_ctf_variant *get_ctf_variant_writeable() { return (c_game_engine_ctf_variant *)get_active_variant_writeable(); }
    __forceinline c_game_engine_slayer_variant *get_slayer_variant_writeable() { return (c_game_engine_slayer_variant *)get_active_variant_writeable(); }
    __forceinline c_game_engine_oddball_variant *get_oddball_variant_writeable() { return (c_game_engine_oddball_variant *)get_active_variant_writeable(); }
    __forceinline c_game_engine_king_variant *get_king_variant_writeable() { return (c_game_engine_king_variant *)get_active_variant_writeable(); }
    __forceinline c_game_engine_sandbox_variant *get_sandbox_variant_writeable() { return (c_game_engine_sandbox_variant *)get_active_variant(); }
    __forceinline c_game_engine_vip_variant *get_vip_variant_writeable() { return (c_game_engine_vip_variant *)get_active_variant(); }
    __forceinline c_game_engine_juggernaut_variant *get_juggernaut_variant_writeable()
    {
        return (c_game_engine_juggernaut_variant *)get_active_variant_writeable();
    }
    __forceinline c_game_engine_territories_variant *get_territories_variant_writeable()
    {
        return (c_game_engine_territories_variant *)get_active_variant_writeable();
    }
    __forceinline c_game_engine_assault_variant *get_assault_variant_writeable() { return (c_game_engine_assault_variant *)get_active_variant_writeable(); }
    __forceinline c_game_engine_infection_variant *get_infection_variant_writeable()
    {
        return (c_game_engine_infection_variant *)get_active_variant_writeable();
    }
    __forceinline bool has_teams() const { return (m_variant_storage[0x100 + sizeof(void *) - 4] & 1) != 0; }
    __forceinline void initialize() {
        engine_type = 0;
        memset(variant_storage, 0, sizeof(variant_storage));
        memset(variant_storage, 0, sizeof(variant_storage));
    }
    __forceinline void clear() { initialize(); }

    void encode(c_bitstream *packet) const;
    __forceinline bool get_integer_game_engine_setting(e_game_variant_parameter parameter, long *out_value) const {
        return get_game_engine_setting(parameter, _text_value_pair_parameter_integer, out_value);
    }
    __forceinline bool get_string_id_game_engine_setting(e_game_variant_parameter parameter, long *out_value) const {
        return get_game_engine_setting(parameter, _text_value_pair_parameter_string_id, out_value);
    }
    __forceinline bool set_integer_game_engine_setting(e_game_variant_parameter parameter, long value) {
        return set_game_engine_setting(parameter, _text_value_pair_parameter_integer, value);
    }
    __forceinline bool set_string_id_game_engine_setting(e_game_variant_parameter parameter, long value) {
        return set_game_engine_setting(parameter, _text_value_pair_parameter_string_id, value);
    }
    void copy_from_and_validate(const c_game_variant *source);
    bool is_equal_to(const c_game_variant *other) const;
    void recreate_variant_vtable_for_game_engine_index(e_game_engine_type game_engine_index);
    void set_game_engine_index(e_game_engine_type engine_type);
    bool get_rules_are_symmetric() const;
    static e_text_value_pair_parameter_type get_expected_game_engine_setting_parameter_type(e_game_variant_parameter parameter);
    s_game_engine_setting_variant *writeable();

protected:
    bool get_game_engine_setting(e_game_variant_parameter parameter, e_text_value_pair_parameter_type parameter_type, long *out_value) const;
    bool set_game_engine_setting(e_game_variant_parameter parameter, e_text_value_pair_parameter_type parameter_type, long value);
};

typedef char game_variant_size_check[(sizeof(c_game_variant) == 0x264) ? 1 : -1];
#ifdef HALO3_64BIT
static_assert(offsetof(c_game_variant, m_checksum) == 0xc, "64-bit variant checksum offset");
#else
typedef char game_variant_off_name[(offsetof(c_game_variant, m_name) == 0x14) ? 1 : -1];
typedef char game_variant_off_checksum[(offsetof(c_game_variant, m_checksum) == 8) ? 1 : -1];
typedef char game_variant_off_unique_id[(offsetof(c_game_variant, unique_id) == 0x0c) ? 1 : -1];
typedef char game_variant_off_description[(offsetof(c_game_variant, description) == 0x34) ? 1 : -1];
typedef char game_variant_off_author_id[(offsetof(c_game_variant, author_id) == 0xcc) ? 1 : -1];
typedef char game_variant_off_game_id[(offsetof(c_game_variant, game_id) == 0xfc) ? 1 : -1];
typedef char game_variant_off_flags[(offsetof(c_game_variant, flags) == 0x104) ? 1 : -1];
typedef char game_variant_off_round_time_limit_minutes[(offsetof(c_game_variant, round_time_limit_minutes) == 0x105) ? 1 : -1];
typedef char game_variant_off_early_victory_win_count[(offsetof(c_game_variant, early_victory_win_count) == 0x107) ? 1 : -1];
typedef char game_variant_off_lives_per_round[(offsetof(c_game_variant, lives_per_round) == 0x109) ? 1 : -1];
typedef char game_variant_off_team_lives_per_round[(offsetof(c_game_variant, team_lives_per_round) == 0x10a) ? 1 : -1];
typedef char game_variant_off_respawn_time_seconds[(offsetof(c_game_variant, respawn_time_seconds) == 0x10b) ? 1 : -1];
typedef char game_variant_off_suicide_penalty_seconds[(offsetof(c_game_variant, suicide_penalty_seconds) == 0x10c) ? 1 : -1];
typedef char game_variant_off_betrayal_penalty_seconds[(offsetof(c_game_variant, betrayal_penalty_seconds) == 0x10d) ? 1 : -1];
typedef char game_variant_off_respawn_growth_seconds[(offsetof(c_game_variant, respawn_growth_seconds) == 0x10e) ? 1 : -1];
typedef char game_variant_off_round_limit[(offsetof(c_game_variant, round_limit) == 0x106) ? 1 : -1];
typedef char game_variant_off_respawn_option_flags[(offsetof(c_game_variant, respawn_option_flags) == 0x108) ? 1 : -1];
typedef char game_variant_off_respawn_player_traits_duration_seconds[(offsetof(c_game_variant, respawn_player_traits_duration_seconds) == 0x10f) ? 1 : -1];
typedef char game_variant_off_social_options_flags[(offsetof(c_game_variant, social_options_flags) == 0x12c) ? 1 : -1];
typedef char game_variant_off_team_changing[(offsetof(c_game_variant, team_changing) == 0x12e) ? 1 : -1];
typedef char game_variant_off_variant_flags[(offsetof(c_game_variant, variant_flags) == 0x130) ? 1 : -1];
typedef char game_variant_off_weapon_set_absolute_index[(offsetof(c_game_variant, weapon_set_absolute_index) == 0x150) ? 1 : -1];
typedef char game_variant_off_vehicle_set_absolute_index[(offsetof(c_game_variant, vehicle_set_absolute_index) == 0x152) ? 1 : -1];
typedef char game_variant_off_red_powerup_duration_seconds[(offsetof(c_game_variant, red_powerup_duration_seconds) == 0x1a8) ? 1 : -1];
typedef char game_variant_off_blue_powerup_duration_seconds[(offsetof(c_game_variant, blue_powerup_duration_seconds) == 0x1a9) ? 1 : -1];
typedef char game_variant_off_yellow_powerup_duration_seconds[(offsetof(c_game_variant, yellow_powerup_duration_seconds) == 0x1aa) ? 1 : -1];
typedef char game_variant_off_respawn_player_traits[(offsetof(c_game_variant, respawn_player_traits) == 0x110) ? 1 : -1];
typedef char game_variant_off_base_player_traits[(offsetof(c_game_variant, base_player_traits) == 0x134) ? 1 : -1];
typedef char game_variant_off_red_powerup_traits[(offsetof(c_game_variant, red_powerup_traits) == 0x154) ? 1 : -1];
typedef char game_variant_off_blue_powerup_traits[(offsetof(c_game_variant, blue_powerup_traits) == 0x170) ? 1 : -1];
typedef char game_variant_off_yellow_powerup_traits[(offsetof(c_game_variant, yellow_powerup_traits) == 0x18c) ? 1 : -1];
typedef char game_variant_off_base_variant_flags[(offsetof(c_game_variant, base_variant_flags) == 0x1ac) ? 1 : -1];
typedef char game_variant_off_team_scoring_method[(offsetof(c_game_variant, team_scoring_method) == 0x1ae) ? 1 : -1];
#endif

extern "C" void game_variant_set_from(c_game_variant *variant, const c_game_variant *source);

#endif // GAME_GAME_VARIANT_H
