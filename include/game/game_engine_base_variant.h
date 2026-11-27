#ifndef GAME_GAME_ENGINE_BASE_VARIANT_H
#define GAME_GAME_ENGINE_BASE_VARIANT_H

#include <stddef.h>

#include "game/team_scoring_method_types.h"

#include "cseries/cseries_wrapped.h"
#include "game/game_types.h"
#include "game/map_variant.h"
#include "game/player_traits.h"

class c_bitstream;
struct s_game_engine_base_variant_definition;
struct s_game_engine_miscellaneous_options_definition;
struct s_game_engine_respawn_options_definition;
struct s_game_engine_social_options_definition;
struct s_game_engine_map_override_options_definition;
enum e_game_engine_team_changing_setting { _game_engine_team_changing_default = 1 };

// Option classes shared by every game-engine base variant.
// game/game_engine_traits.cpp rather than a header; this leaf isolates the variant classes.
class c_game_engine_miscellaneous_options
{
public:
    void initialize();
    void set(const c_game_engine_miscellaneous_options *options, bool force);
    void set(const s_game_engine_miscellaneous_options_definition *definition);
    void set(const s_game_engine_miscellaneous_options_definition *definition, bool force);
    void set_round_time_limit_minutes(unsigned char value) { if (value <= 60) m_round_time_limit_minutes = value; else m_round_time_limit_minutes = 8; }
    void set_round_limit(unsigned char value) { if (value <= 10) m_round_limit = value; else m_round_limit = 1; }
    void set_early_victory_win_count(unsigned char value) { if (value <= 5) m_early_victory_win_count = value; else m_early_victory_win_count = 2; }
    enum e_game_engine_miscellaneous_option_flags
    {
        _game_engine_miscellaneous_option_teams_enabled = 0,
        _game_engine_miscellaneous_option_round_reset_players,
        _game_engine_miscellaneous_option_round_reset_map,
        k_game_engine_miscellaneous_option_flags
    };

    void set_teams_enabled(bool teams_enabled)
    {
        m_flags.set(_game_engine_miscellaneous_option_teams_enabled, teams_enabled);
    }

    bool get_teams_enabled() const { return m_flags.test(_game_engine_miscellaneous_option_teams_enabled); }
    bool get_round_reset_players() const { return m_flags.test(_game_engine_miscellaneous_option_round_reset_players); }
    bool get_round_reset_map() const { return m_flags.test(_game_engine_miscellaneous_option_round_reset_map); }
    unsigned char get_round_time_limit_minutes() const { return m_round_time_limit_minutes; }
    unsigned char get_round_limit() const { return m_round_limit; }
    unsigned char get_early_victory_win_count() const { return m_early_victory_win_count; }

public:
    c_flags_no_init<e_game_engine_miscellaneous_option_flags, unsigned char, k_game_engine_miscellaneous_option_flags> m_flags;
    unsigned char m_round_time_limit_minutes;
    unsigned char m_round_limit;
    unsigned char m_early_victory_win_count;
};

class c_game_engine_respawn_options
{
public:
    void initialize();
    void set(const c_game_engine_respawn_options *options, bool force);
    void set(const s_game_engine_respawn_options_definition *definition);
    void set(const s_game_engine_respawn_options_definition *definition, bool force);
    enum e_game_engine_respawn_option_flags
    {
        _game_engine_respawn_option_inherit_respawn_time = 0,
        _game_engine_respawn_option_respawn_with_teammate,
        _game_engine_respawn_option_respawn_at_location,
        _game_engine_respawn_option_respawn_on_kills,
        k_game_engine_respawn_option_flags
    };

    bool get_inherit_respawn_time_enabled() const { return m_flags.test(_game_engine_respawn_option_inherit_respawn_time); }
    bool get_respawn_with_teammate_enabled() const { return m_flags.test(_game_engine_respawn_option_respawn_with_teammate); }
    bool get_respawn_at_location_enabled() const { return m_flags.test(_game_engine_respawn_option_respawn_at_location); }
    bool get_respawn_on_kills_enabled() const { return m_flags.test(_game_engine_respawn_option_respawn_on_kills); }
    unsigned char get_lives_per_round() const { return m_lives_per_round; }
    unsigned char get_team_lives_per_round() const { return m_team_lives_per_round; }
    unsigned char get_respawn_time_seconds() const { return m_respawn_time_seconds; }
    unsigned char get_suicide_penalty_seconds() const { return m_suicide_penalty_seconds; }
    unsigned char get_betrayal_penalty_seconds() const { return m_betrayal_penalty_seconds; }
    unsigned char get_respawn_growth_seconds() const { return m_respawn_growth_seconds; }
    unsigned char get_respawn_player_traits_duration_seconds() const { return m_respawn_player_traits_duration_seconds; }
    const c_player_traits *get_respawn_player_traits() const { return &m_respawn_player_traits; }

public:
    c_flags_no_init<e_game_engine_respawn_option_flags, unsigned char, k_game_engine_respawn_option_flags> m_flags;
    unsigned char m_lives_per_round;
    unsigned char m_team_lives_per_round;
    unsigned char m_respawn_time_seconds;
    unsigned char m_suicide_penalty_seconds;
    unsigned char m_betrayal_penalty_seconds;
    unsigned char m_respawn_growth_seconds;
    unsigned char m_respawn_player_traits_duration_seconds;
    c_player_traits m_respawn_player_traits;
};

class c_game_engine_social_options
{
public:
    void initialize();
    void set(const c_game_engine_social_options *options, bool force);
    void set(const s_game_engine_social_options_definition *definition);
    void set(const s_game_engine_social_options_definition *definition, bool force);
    enum e_game_engine_social_options_flags
    {
        _game_engine_social_options_friendly_fire_enabled = 0,
        _game_engine_social_options_betrayal_booting_enabled,
        _game_engine_social_options_enemy_voice_enabled,
        _game_engine_social_options_open_channel_voice_enabled,
        _game_engine_social_options_dead_player_voice_enabled,
        k_game_engine_social_options_flags
    };

    bool get_observers_enabled() const { return false; }
    short get_team_changing_setting() const { return m_team_changing; }
    bool get_friendly_fire_enabled() const { return m_flags.test(_game_engine_social_options_friendly_fire_enabled); }
    bool get_betrayal_booting_enabled() const { return m_flags.test(_game_engine_social_options_betrayal_booting_enabled); }
    bool get_enemy_voice_enabled() const { return m_flags.test(_game_engine_social_options_enemy_voice_enabled); }
    bool get_open_channel_voice_enabled() const { return m_flags.test(_game_engine_social_options_open_channel_voice_enabled); }
    bool get_dead_player_voice_enabled() const { return m_flags.test(_game_engine_social_options_dead_player_voice_enabled); }

public:
    c_flags_no_init<e_game_engine_social_options_flags, short, k_game_engine_social_options_flags> m_flags;
    c_enum<e_game_engine_team_changing_setting, short, 0, 3> m_team_changing;
};

class c_game_engine_map_override_options
{
public:
    void initialize();
    void set(const c_game_engine_map_override_options *options, bool force);
    void set(const s_game_engine_map_override_options_definition *definition);
    void set(const s_game_engine_map_override_options_definition *definition, bool force);
    long get_weapon_set() const;
    long get_vehicle_set() const;
    void set_weapon_set_absolute_index(short index);
    void set_vehicle_set_absolute_index(short index);
    enum e_game_engine_map_override_option_flags
    {
        _game_engine_map_override_option_grenades_on_map = 0,
        _game_engine_map_override_option_indestructible_vehicles,
        k_game_engine_map_override_option_flags
    };

    bool get_grenades_on_map_enabled() const { return m_flags.test(_game_engine_map_override_option_grenades_on_map); }
    bool get_indestructible_vehicles_enabled() const { return m_flags.test(_game_engine_map_override_option_indestructible_vehicles); }
    const c_player_traits *get_base_player_traits() const { return &m_base_player_traits; }
    short get_weapon_set_absolute_index() const { return m_weapon_set_absolute_index; }
    short get_vehicle_set_absolute_index() const { return m_vehicle_set_absolute_index; }
    const c_player_traits *get_red_powerup_traits() const { return &m_red_powerup_traits; }
    const c_player_traits *get_blue_powerup_traits() const { return &m_blue_powerup_traits; }
    const c_player_traits *get_yellow_powerup_traits() const { return &m_yellow_powerup_traits; }
    unsigned char get_red_powerup_duration_seconds() const { return m_red_powerup_duration_seconds; }
    unsigned char get_blue_powerup_duration_seconds() const { return m_blue_powerup_duration_seconds; }
    unsigned char get_yellow_powerup_duration_seconds() const { return m_yellow_powerup_duration_seconds; }

public:
    c_flags_no_init<e_game_engine_map_override_option_flags, long, k_game_engine_map_override_option_flags> m_flags;
    c_player_traits m_base_player_traits;
    short m_weapon_set_absolute_index;
    short m_vehicle_set_absolute_index;
    c_player_traits m_red_powerup_traits;
    c_player_traits m_blue_powerup_traits;
    c_player_traits m_yellow_powerup_traits;
    unsigned char m_red_powerup_duration_seconds;
    unsigned char m_blue_powerup_duration_seconds;
    unsigned char m_yellow_powerup_duration_seconds;
    unsigned char m_padding;
};

enum e_base_variant_flags {
    _base_variant_flags_built_in = 0,
    k_base_variant_flags = 1
};

#pragma pack(push, 4)
class c_game_engine_base_variant {
protected:
    virtual long get_game_engine_name_string_id() const;
    virtual long get_game_engine_default_description_string_id() const;
public:
    // New variants start with the built-in flags cleared.
    c_game_engine_base_variant() {}
    virtual void initialize();
    virtual void validate();
    virtual void encode(c_bitstream *packet) const;
    virtual void decode(c_bitstream *packet);
    virtual bool can_add_to_recent_list() const { return true; }
    virtual long get_score_to_win_round() const { return 0; }
    virtual bool can_be_cast_to(e_game_engine_type engine_type, const void **variant) const;
    virtual void custom_team_score_stats(long team_index, long old_score, long new_score) const;

    // Copies another variant's settings into this variant.
    void set(const c_game_engine_base_variant *variant);
    void set(const s_game_engine_base_variant_definition *definition, bool force);
    void get_game_engine_name(c_static_wchar_string<1024> *name) const;
    void get_game_engine_description(c_static_wchar_string<1024> *description) const;
    const wchar_t *get_name() const { return m_metadata.name; }
    // The metadata stores the description as UTF-8 text.
    const wchar_t *get_description() const;
    void set_name(const wchar_t *name);
    void set_description(const wchar_t *description);
    e_team_scoring_method get_team_scoring_method() const { return (e_team_scoring_method)m_team_scoring_method; }
    void set_team_scoring_method(e_team_scoring_method method);

    bool get_built_in() const { return m_flags.test(_base_variant_flags_built_in); }
    void set_built_in(bool built_in) { m_flags.set(_base_variant_flags_built_in, built_in); }
    const c_game_engine_miscellaneous_options *get_miscellaneous_options() const { return &m_miscellaneous_options; }
    const c_game_engine_respawn_options *get_respawn_options() const { return &m_respawn_options; }
    const c_game_engine_social_options *get_social_options() const { return &m_social_options; }
    const c_game_engine_map_override_options *get_map_override_options() const { return &m_map_override_options; }
    c_game_engine_miscellaneous_options *get_miscellaneous_options_writeable() { return &m_miscellaneous_options; }
    c_game_engine_respawn_options *get_respawn_options_writeable() { return &m_respawn_options; }
    c_game_engine_social_options *get_social_options_writeable() { return &m_social_options; }
    c_game_engine_map_override_options *get_map_override_options_writeable() { return &m_map_override_options; }

    long m_checksum;
    s_saved_game_item_metadata m_metadata;
    c_game_engine_miscellaneous_options m_miscellaneous_options;
    c_game_engine_respawn_options m_respawn_options;
    c_game_engine_social_options m_social_options;
    c_game_engine_map_override_options m_map_override_options;
    c_flags<e_base_variant_flags, unsigned short, k_base_variant_flags> m_flags;
    short m_team_scoring_method;
};
#pragma pack(pop)

typedef char engine_miscellaneous_options_size[(sizeof(c_game_engine_miscellaneous_options) == 4) ? 1 : -1];
typedef char engine_respawn_options_size[(sizeof(c_game_engine_respawn_options) == 0x24) ? 1 : -1];
typedef char engine_social_options_size[(sizeof(c_game_engine_social_options) == 4) ? 1 : -1];
typedef char engine_map_override_options_size[(sizeof(c_game_engine_map_override_options) == 0x7c) ? 1 : -1];
typedef char engine_base_variant_size[(sizeof(void *) != 4 || ((sizeof(c_game_engine_base_variant) == 0x1ac) )) ? 1 : -1];
typedef char engine_base_variant_off_metadata[(sizeof(void *) != 4 || ((offsetof(c_game_engine_base_variant, m_metadata) == 8) )) ? 1 : -1];
typedef char engine_base_variant_off_misc[(sizeof(void *) != 4 || ((offsetof(c_game_engine_base_variant, m_miscellaneous_options) == 0x100) )) ? 1 : -1];
typedef char engine_base_variant_off_respawn[(sizeof(void *) != 4 || ((offsetof(c_game_engine_base_variant, m_respawn_options) == 0x104) )) ? 1 : -1];
typedef char engine_base_variant_off_social[(sizeof(void *) != 4 || ((offsetof(c_game_engine_base_variant, m_social_options) == 0x128) )) ? 1 : -1];
typedef char engine_base_variant_off_map[(sizeof(void *) != 4 || ((offsetof(c_game_engine_base_variant, m_map_override_options) == 0x12c) )) ? 1 : -1];
typedef char engine_base_variant_off_flags[(sizeof(void *) != 4 || ((offsetof(c_game_engine_base_variant, m_flags) == 0x1a8) )) ? 1 : -1];
typedef char engine_base_variant_off_team_scoring[(sizeof(void *) != 4 || ((offsetof(c_game_engine_base_variant, m_team_scoring_method) == 0x1aa) )) ? 1 : -1];

#endif // GAME_GAME_ENGINE_BASE_VARIANT_H
