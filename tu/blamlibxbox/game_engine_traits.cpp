// blamlibxbox/game_engine_traits.obj: from the records 0x244238 0x2454a8 0x263fb8 0x264088 0x264120 0x264280 0x264418 0x264500 0x2645f0 0x2648f8 0x264a40 0x264bc0 0x264c58 0x264cd0 0x264d40 0x264dc8 0x265048 0x2655f0 0x265748
// @flags /TP /O2 /Oc
#include "cseries/cseries_wrapped.h"
#include "memory/bitstream.h"
#include "cseries/bit_operations.h"
#include "game/game_types.h"
#include "game/player_traits.h"
#include "game/map_variant.h"
#include <string.h>
#include "tag_files/tag_types.h"
#include "game/multiplayer_definitions.h"
#include "game/game_engine.h"
#include "game/game_engine_group.h"
#include "game/game_engine_traits.h"
#include "game/game_engine_base_variant.h"
#include "game/universal_types.h"
#include "game/weapon_set_types.h"
#include "game/game_results_statistic_types.h"
#include "game/game_results_statistic_globals.h"
#include "game/game_engine_variant_definitions.h"
#include "saved_games/saved_game_file_types.h"
#include "memory/static_arrays.h"
#include "game/game_results.h"
#include "main/main_game.h"
#include "game/game_engine_traits_externs.h"
#include "networking/network_game_definitions.h"
#include "game/player_configuration.h"
#include "game/game_results_incremental_update.h"
struct s_multiplayer_weapon_set;
// @address 0x1e97e0 saved_game_files_decode_metadata
// @address 0x221648 ?read_bit_internal@c_bitstream@@AAA_NXZ
// @address 0x221768 ?read_value_internal@c_bitstream@@AAAKJ@Z
// @address 0x266ea8 ?decode@c_player_traits@@QAAXPAVc_bitstream@@@Z
// @address 0x265048 ?decode@c_game_engine_base_variant@@UAAXPAVc_bitstream@@@Z
// @address 0x265748 game_engine_base_variant_set
// @address 0x1e8578 ?game_engine_index_to_saved_game_file_type@@YA?AW4e_saved_game_file_type@@W4e_game_engine_type@@@Z
// @address 0x264120 game_engine_respawn_options_copy
// @address 0x264418 ?set@c_game_engine_social_options@@QAAXPBUs_game_engine_social_options_definition@@@Z
// @address 0x264500 game_engine_social_options_copy
// @address 0x2648f8 game_engine_map_override_options_copy
// @address 0x264bc0 ?get_weapon_set@c_game_engine_map_override_options@@QBAJXZ
// @address 0x264c58 ?set_weapon_set_absolute_index@c_game_engine_map_override_options@@QAAXF@Z
// @address 0x266a50 ?set@c_player_traits@@QAAXPBV1@_N@Z
// @address 0x266b48 ?set@c_player_traits@@QAAXPBUs_game_engine_player_traits@@_N@Z
// @address 0x2bc148 ?game_engine_settings_player_traits_get_by_name@@YAPBUs_game_engine_player_traits@@J@Z

struct s_game_engine_zone_variant_definition {
    char base[56];
    unsigned long flags;
    short count;
    short index;
    long traits;
};
struct s_game_engine_zone_variant : c_game_engine_base_variant {
    char m_base_padding[0x1b0 - sizeof(c_game_engine_base_variant)];
    char m_flags;
    unsigned char m_count;
    short m_time;
    c_player_traits m_traits;
};

// @address 0x263fb8 ?set@c_game_engine_miscellaneous_options@@QAAXPBUs_game_engine_miscellaneous_options_definition@@@Z
void c_game_engine_miscellaneous_options::set(const s_game_engine_miscellaneous_options_definition *definition)
{
    m_flags.set(_game_engine_miscellaneous_option_teams_enabled, (definition->flags & 1) != 0);
    m_flags.set(_game_engine_miscellaneous_option_round_reset_players, definition->round_reset_setting == 1 || definition->round_reset_setting == 2);
    m_flags.set(_game_engine_miscellaneous_option_round_reset_map, definition->round_reset_setting == 2);
    set_round_time_limit_minutes(definition->round_time_limit_minutes);
    set_round_limit(definition->round_limit);
    set_early_victory_win_count(definition->early_victory_win_count);
}

struct s_game_engine_player_traits;

// @address 0x264088 ?initialize@c_game_engine_respawn_options@@QAAXXZ
void c_game_engine_respawn_options::initialize()
{
    m_flags.m_storage = 0;
    m_flags.set(_game_engine_respawn_option_inherit_respawn_time, false);
    m_flags.set(_game_engine_respawn_option_respawn_with_teammate, false);
    m_flags.set(_game_engine_respawn_option_respawn_at_location, false);
    m_flags.set(_game_engine_respawn_option_respawn_on_kills, false);
    m_lives_per_round = 0;
    m_team_lives_per_round = 0;
    m_respawn_time_seconds = 5;
    m_suicide_penalty_seconds = 5;
    m_betrayal_penalty_seconds = 5;
    m_respawn_growth_seconds = 0;
    m_respawn_player_traits_duration_seconds = 5;
    m_respawn_player_traits.clear();
}

static __forceinline void game_engine_traits_copy_flag(unsigned char *destination, const unsigned char *source, int bit)
{
    if ((*source & (1 << bit)) != 0)
        *destination = *destination | (1 << bit);
    else
        *destination = *destination & ~(1 << bit);
}
static __forceinline void game_engine_traits_copy_limited(unsigned char *destination, unsigned char value, unsigned char limit, unsigned char fallback)
{
    if (value <= limit)
        *destination = value;
    else
        *destination = fallback;
}
extern "C" void game_engine_respawn_options_copy(unsigned char *destination, const unsigned char *source)
{
    game_engine_traits_copy_flag(destination, source, 0);
    game_engine_traits_copy_flag(destination, source, 1);
    game_engine_traits_copy_flag(destination, source, 2);
    game_engine_traits_copy_flag(destination, source, 3);
    game_engine_traits_copy_limited(destination + 1, source[1], 50, 0);
    game_engine_traits_copy_limited(destination + 2, source[2], 100, 0);
    game_engine_traits_copy_limited(destination + 3, source[3], 240, 5);
    game_engine_traits_copy_limited(destination + 4, source[4], 240, 5);
    game_engine_traits_copy_limited(destination + 5, source[5], 240, 5);
    game_engine_traits_copy_limited(destination + 6, source[6], 15, 0);
    game_engine_traits_copy_limited(destination + 7, source[7], 60, 5);
    ((c_game_engine_respawn_options *)destination)->m_respawn_player_traits.set(&((const c_game_engine_respawn_options *)source)->m_respawn_player_traits,
        true);
}
static __forceinline bool respawn_definition_flag(unsigned short flags, long bit)
{
    unsigned char result = (flags >> bit) & 1;
    return result;
}
// @address 0x264280 ?set@c_game_engine_respawn_options@@QAAXPBUs_game_engine_respawn_options_definition@@@Z
void c_game_engine_respawn_options::set(const s_game_engine_respawn_options_definition *definition)
{
    m_flags.set(_game_engine_respawn_option_inherit_respawn_time, (definition->flags & 1) != 0);
    m_flags.set(_game_engine_respawn_option_respawn_with_teammate, respawn_definition_flag(definition->flags, 1));
    m_flags.set(_game_engine_respawn_option_respawn_at_location, respawn_definition_flag(definition->flags, 2));
    m_flags.set(_game_engine_respawn_option_respawn_on_kills, respawn_definition_flag(definition->flags, 3));
    game_engine_traits_copy_limited(&m_lives_per_round, definition->lives_per_round, 50, 0);
    game_engine_traits_copy_limited(&m_team_lives_per_round, definition->team_lives_per_round, 100, 0);
    game_engine_traits_copy_limited(&m_respawn_time_seconds, definition->respawn_time_seconds, 240, 5);
    game_engine_traits_copy_limited(&m_suicide_penalty_seconds, definition->suicide_penalty_seconds, 240, 5);
    game_engine_traits_copy_limited(&m_betrayal_penalty_seconds, definition->betrayal_penalty_seconds, 240, 5);
    game_engine_traits_copy_limited(&m_respawn_growth_seconds, definition->respawn_growth_seconds, 15, 0);
    game_engine_traits_copy_limited(&m_respawn_player_traits_duration_seconds, definition->respawn_player_traits_duration_seconds, 60, 5);
    const s_game_engine_player_traits *traits = game_engine_settings_player_traits_get_by_name(definition->respawn_player_traits);
    m_respawn_player_traits.set(traits, true);
}

void c_game_engine_social_options::set(const s_game_engine_social_options_definition *definition)
{
    long chat_setting = definition->chat_setting;
    if ((unsigned long)chat_setting <= 2)
        m_team_changing = (e_game_engine_team_changing_setting)chat_setting;
    m_flags.set(_game_engine_social_options_friendly_fire_enabled, definition->flags.test(0));
    m_flags.set(_game_engine_social_options_betrayal_booting_enabled, definition->flags.test(1));
    m_flags.set(_game_engine_social_options_enemy_voice_enabled, definition->flags.test(2));
    m_flags.set(_game_engine_social_options_open_channel_voice_enabled, definition->flags.test(3));
    m_flags.set(_game_engine_social_options_dead_player_voice_enabled, definition->flags.test(4));
}

void game_engine_social_options_copy(short *destination, const long *source)
{
    if (bit_test(*source, 3))
        destination[0] |= 1;
    else
        destination[0] &= ~1;
    if (bit_test(*source, 4))
        destination[0] |= 2;
    else
        destination[0] &= ~2;
    if (bit_test(*source, 5))
        destination[0] |= 4;
    else
        destination[0] &= ~4;
    if (bit_test(*source, 6))
        destination[0] |= 8;
    else
        destination[0] &= ~8;
    if (bit_test(*source, 7))
        destination[0] |= 16;
    else
        destination[0] &= ~16;
    if (*source & 4)
        destination[1] = 2;
    else
        destination[1] = (short)(((unsigned long)*source >> 1) & 1);
}

static long weapon_set_name(short absolute_index, long fallback)
{
    long name = fallback;
    if (absolute_index == -2) {
        name = _multiplayer_weapon_selection_name_random;
    } else {
        s_universal *universal = (s_universal *)scenario_multiplayer_globals_try_and_get_universal_data();
        if (universal != 0 && absolute_index >= 0 && absolute_index < universal->weapon_set_count) {
            s_weapon_set *entry = &universal->weapon_sets[absolute_index];
            if (entry != 0)
                name = entry->string_id;
        }
    }
    return name;
}

// @address 0x2645f0 ?initialize@c_game_engine_map_override_options@@QAAXXZ
void c_game_engine_map_override_options::initialize()
{
    m_flags.m_storage = 0;
    m_flags.set(_game_engine_map_override_option_grenades_on_map, true);
    m_flags.set(_game_engine_map_override_option_indestructible_vehicles, false);
    m_base_player_traits.clear();
    m_weapon_set_absolute_index = -1;
    m_vehicle_set_absolute_index = -1;
    m_red_powerup_traits.clear();
    m_blue_powerup_traits.clear();
    m_yellow_powerup_traits.clear();
    m_red_powerup_duration_seconds = 5;
    m_blue_powerup_duration_seconds = 30;
    m_yellow_powerup_duration_seconds = 30;
    m_padding = 0;
}

static __forceinline void game_engine_traits_set_red(unsigned char *dst, unsigned char seconds) {
    if (seconds <= 120 || seconds == 0) { *dst = seconds; } else { *dst = 5; }
}
static __forceinline int game_engine_traits_valid(unsigned char seconds) { return seconds <= 120 || seconds == 0; }
static __forceinline void game_engine_traits_set_blue(unsigned char *dst, unsigned char seconds) {
    if (game_engine_traits_valid(seconds)) { *dst = seconds; } else { *dst = 30; }
}
static __forceinline void game_engine_traits_set_yellow(unsigned char *dst, unsigned char seconds) {
    if (game_engine_traits_valid(seconds)) { *dst = seconds; dst[1] = 0; } else { *dst = 30; dst[1] = 0; }
}

void game_engine_map_override_options_copy(
    c_game_engine_map_override_options *destination, const c_game_engine_map_override_options *source)
{
    destination->m_flags.set(c_game_engine_map_override_options::_game_engine_map_override_option_grenades_on_map,
        bit_test(source->m_flags.m_storage, 0));
    destination->m_flags.set(c_game_engine_map_override_options::_game_engine_map_override_option_indestructible_vehicles,
        bit_test(source->m_flags.m_storage, 1));
    destination->m_base_player_traits.set(&source->m_base_player_traits, true);
    destination->set_weapon_set_absolute_index(source->m_weapon_set_absolute_index);
    destination->set_vehicle_set_absolute_index(source->m_vehicle_set_absolute_index);
    destination->m_red_powerup_traits.set(&source->m_red_powerup_traits, true);
    destination->m_blue_powerup_traits.set(&source->m_blue_powerup_traits, true);
    destination->m_yellow_powerup_traits.set(&source->m_yellow_powerup_traits, true);
    game_engine_traits_set_red(&destination->m_red_powerup_duration_seconds, source->m_red_powerup_duration_seconds);
    game_engine_traits_set_blue(&destination->m_blue_powerup_duration_seconds, source->m_blue_powerup_duration_seconds);
    game_engine_traits_set_yellow(&destination->m_yellow_powerup_duration_seconds, source->m_yellow_powerup_duration_seconds);
}

// @address 0x264a40 ?set@c_game_engine_map_override_options@@QAAXPBUs_game_engine_map_override_options_definition@@@Z
void c_game_engine_map_override_options::set(const s_game_engine_map_override_options_definition *definition)
{
    m_flags.set(_game_engine_map_override_option_grenades_on_map, (definition->flags & 1) != 0);
    m_flags.set(_game_engine_map_override_option_indestructible_vehicles, (definition->flags & 2) != 0);
    m_base_player_traits.set(game_engine_settings_player_traits_get_by_name(definition->base_player_traits), true);
    set_weapon_set_absolute_index(multiplayer_universal_data_get_weapon_set_absolute_index_from_string_id(definition->weapon_set));
    set_vehicle_set_absolute_index(multiplayer_universal_data_get_vehicle_set_absolute_index_from_string_id(definition->vehicle_set));
    m_red_powerup_traits.set(game_engine_settings_player_traits_get_by_name(definition->red_powerup_traits), true);
    m_blue_powerup_traits.set(game_engine_settings_player_traits_get_by_name(definition->blue_powerup_traits), true);
    m_yellow_powerup_traits.set(game_engine_settings_player_traits_get_by_name(definition->yellow_powerup_traits), true);
    game_engine_traits_set_red(&m_red_powerup_duration_seconds, definition->red_powerup_duration_seconds);
    game_engine_traits_set_blue(&m_blue_powerup_duration_seconds, definition->blue_powerup_duration_seconds);
    game_engine_traits_set_yellow(&m_yellow_powerup_duration_seconds, definition->yellow_powerup_duration_seconds);
}

long c_game_engine_map_override_options::get_weapon_set() const
{
    return weapon_set_name(m_weapon_set_absolute_index, 1);
}

void c_game_engine_map_override_options::set_weapon_set_absolute_index(short absolute_index)
{
    if (absolute_index != -1 && absolute_index != -2 && main_game_globals.status == 2 &&
        ::multiplayer_universal_data_weapon_set_try_and_get(absolute_index) == 0)
    {
        m_weapon_set_absolute_index = -1;
        return;
    }
    m_weapon_set_absolute_index = absolute_index;
}

// @address 0x264cd0 ?set_vehicle_set_absolute_index@c_game_engine_map_override_options@@QAAXF@Z
void c_game_engine_map_override_options::set_vehicle_set_absolute_index(short absolute_index)
{
    if (absolute_index == -1 || main_game_globals.status != 2 || multiplayer_universal_data_vehicle_set_try_and_get(absolute_index)) {
        m_vehicle_set_absolute_index = absolute_index;
    } else {
        m_weapon_set_absolute_index = -1;
    }
}

// @address 0x2655f0 ?set@c_game_engine_base_variant@@QAAXPBV1@@Z
void c_game_engine_base_variant::set(const c_game_engine_base_variant *variant)
{
    unsigned char round_time_limit_minutes;
    unsigned char round_limit;
    unsigned char early_victory_win_count;
    int team_scoring_method;
    game_engine_variant_set_metadata((s_game_engine_base_variant_metadata_view *)this, (const s_saved_game_item_metadata_packed *)&variant->m_metadata);
    if (!(((unsigned int)(variant->m_flags.m_storage & 1) == 0))) {
        m_flags.m_storage = (m_flags.m_storage | 1);
    } else {
        m_flags.m_storage = (m_flags.m_storage & -2);
    }
    if (!(((unsigned int)(variant->m_miscellaneous_options.m_flags.m_storage & 1) == 0))) {
        m_miscellaneous_options.m_flags.m_storage = (m_miscellaneous_options.m_flags.m_storage | 1);
    } else {
        m_miscellaneous_options.m_flags.m_storage = (m_miscellaneous_options.m_flags.m_storage & -2);
    }
    if (!(((unsigned int)(variant->m_miscellaneous_options.m_flags.m_storage & 2) == 0))) {
        m_miscellaneous_options.m_flags.m_storage = (m_miscellaneous_options.m_flags.m_storage | 2);
    } else {
        m_miscellaneous_options.m_flags.m_storage = (m_miscellaneous_options.m_flags.m_storage & 253);
    }
    if (!(((unsigned int)(variant->m_miscellaneous_options.m_flags.m_storage & 4) == 0))) {
        m_miscellaneous_options.m_flags.m_storage = (m_miscellaneous_options.m_flags.m_storage | 4);
    } else {
        m_miscellaneous_options.m_flags.m_storage = (m_miscellaneous_options.m_flags.m_storage & 251);
    }
    round_time_limit_minutes = variant->m_miscellaneous_options.m_round_time_limit_minutes;
    if (!((round_time_limit_minutes <= 60))) {
        round_time_limit_minutes = 8;
    }
    m_miscellaneous_options.m_round_time_limit_minutes = round_time_limit_minutes;
    round_limit = variant->m_miscellaneous_options.m_round_limit;
    if (!((round_limit <= 10))) {
        round_limit = 1;
    }
    m_miscellaneous_options.m_round_limit = round_limit;
    early_victory_win_count = variant->m_miscellaneous_options.m_early_victory_win_count;
    if (!((early_victory_win_count <= 5))) {
        early_victory_win_count = 2;
    }
    m_miscellaneous_options.m_early_victory_win_count = early_victory_win_count;
    game_engine_respawn_options_copy((unsigned char *)&m_respawn_options, (const unsigned char *)&variant->m_respawn_options);
    m_social_options.set((const s_game_engine_social_options_definition *)&variant->m_social_options);
    game_engine_map_override_options_copy(&m_map_override_options, &variant->m_map_override_options);
    team_scoring_method = variant->m_team_scoring_method;
    if (!(((unsigned int)team_scoring_method <= 3))) {
        team_scoring_method = 0;
    }
    m_team_scoring_method = team_scoring_method;
    return;
}

void game_engine_base_variant_set(c_game_engine_base_variant *variant, const s_game_engine_base_variant_definition *definition, e_game_engine_type engine_type)
{
    c_static_wchar_string<1024> description;
    c_static_wchar_string<1024> name;
    name.get_buffer()[0] = 0;
    description.get_buffer()[0] = 0;
    game_engine_get_multiplayer_string(definition->name, (c_static_wchar_string_1024 *)&name);
    game_engine_get_multiplayer_string(definition->description, (c_static_wchar_string_1024 *)&description);
    saved_game_files_initialize_metadata(
        &variant->m_metadata, -1, game_engine_index_to_saved_game_file_type(engine_type), name.get_string(), description.get_string(), 0, 0x3bc);
    variant->m_metadata.game_engine_type = engine_type;
    variant->m_flags.set(_base_variant_flags_built_in, true);
    if (definition->miscellaneous_options.count() > 0) {
        variant->m_miscellaneous_options.set(definition->miscellaneous_options.begin());
    } else {
        variant->m_miscellaneous_options.m_flags.m_storage = 0;
        variant->m_miscellaneous_options.m_round_limit = 1;
        variant->m_miscellaneous_options.m_round_time_limit_minutes = 8;
        variant->m_miscellaneous_options.m_early_victory_win_count = 2;
    }
    if (definition->respawn_options.count() > 0)
        variant->m_respawn_options.set(definition->respawn_options.begin());
    else
        variant->m_respawn_options.initialize();
    if (definition->social_options.count() > 0) {
        game_engine_social_options_copy((short *)&variant->m_social_options, definition->social_options.begin());
    } else {
        variant->m_social_options.m_flags.m_storage = 7;
        variant->m_social_options.m_team_changing = _game_engine_team_changing_default;
    }
    if (definition->map_override_options.count() > 0)
        variant->m_map_override_options.set(definition->map_override_options.begin());
    else
        variant->m_map_override_options.initialize();
    variant->m_team_scoring_method = 0;
}

// @address 0x264d40 ?initialize@c_game_engine_base_variant@@UAAXXZ
void c_game_engine_base_variant::initialize()
{
    memset(&m_metadata, 0, sizeof(m_metadata));
    m_miscellaneous_options.m_flags.m_storage = 0;
    m_miscellaneous_options.m_round_time_limit_minutes = 8;
    m_miscellaneous_options.m_round_limit = 1;
    m_miscellaneous_options.m_early_victory_win_count = 2;
    m_respawn_options.initialize();
    m_social_options.m_flags.m_storage = 7;
    m_social_options.m_team_changing = _game_engine_team_changing_default;
    m_map_override_options.initialize();
    m_flags.m_storage = 0;
    m_team_scoring_method = 0;
}

// @address 0x264dc8 ?validate@c_game_engine_base_variant@@UAAXXZ
void c_game_engine_base_variant::validate()
{
    c_game_engine_base_variant variant;
    variant.c_game_engine_base_variant::initialize();
    variant.set(this);
    set(&variant);
}

// @address 0x1c1258 ?get_game_engine_default_description_string_id@c_game_engine_base_variant@@MBAJXZ
long c_game_engine_base_variant::get_game_engine_default_description_string_id() const
{
    return -1;
}

static __forceinline void game_engine_traits_set_powerup(unsigned char *dst, unsigned char seconds, unsigned char fallback) {
    if (game_engine_traits_valid(seconds)) { *dst = seconds; } else { *dst = fallback; }
}

void c_game_engine_base_variant::decode(c_bitstream *stream)
{
    initialize();
    saved_game_files_decode_metadata(&m_metadata, stream);
    m_flags.set(_base_variant_flags_built_in, stream->read_bool("teams-enabled"));
    m_miscellaneous_options.m_flags.set(c_game_engine_miscellaneous_options::_game_engine_miscellaneous_option_teams_enabled,
        stream->read_bool("miscellaneous-options-teams"));
    m_miscellaneous_options.m_flags.set(c_game_engine_miscellaneous_options::_game_engine_miscellaneous_option_round_reset_players,
        stream->read_bool("miscellaneous-options-round-reset-players"));
    m_miscellaneous_options.m_flags.set(c_game_engine_miscellaneous_options::_game_engine_miscellaneous_option_round_reset_map,
        stream->read_bool("miscellaneous-options-round-reset-map"));
    m_miscellaneous_options.set_round_time_limit_minutes(stream->read_integer("miscellaneous-options-round-time-limit-minutes", 8));
    m_miscellaneous_options.set_round_limit(stream->read_integer("miscellaneous-options-round-limit", 4));
    m_miscellaneous_options.set_early_victory_win_count(stream->read_integer("miscellaneous-options-early-victory-win-count", 4));
    m_respawn_options.m_flags.set(c_game_engine_respawn_options::_game_engine_respawn_option_inherit_respawn_time,
        stream->read_bool("respawn-options-inherit-respawn-time"));
    m_respawn_options.m_flags.set(c_game_engine_respawn_options::_game_engine_respawn_option_respawn_with_teammate,
        stream->read_bool("respawn-options-respawn-with-teammate"));
    m_respawn_options.m_flags.set(c_game_engine_respawn_options::_game_engine_respawn_option_respawn_at_location,
        stream->read_bool("respawn-options-respawn-at-location"));
    m_respawn_options.m_flags.set(
        c_game_engine_respawn_options::_game_engine_respawn_option_respawn_on_kills, stream->read_bool("respawn-options-respawn-on-kills"));
    game_engine_traits_copy_limited(&m_respawn_options.m_lives_per_round, stream->read_integer("respawn-options-lives-per-round", 6), 50, 0);
    game_engine_traits_copy_limited(&m_respawn_options.m_team_lives_per_round, stream->read_integer("respawn-options-team-lives-per-round", 7), 100, 0);
    game_engine_traits_copy_limited(&m_respawn_options.m_respawn_time_seconds, stream->read_integer("respawn-options-respawn-time", 8), 240, 5);
    game_engine_traits_copy_limited(&m_respawn_options.m_suicide_penalty_seconds, stream->read_integer("respawn-options-suicide-time", 8), 240, 5);
    game_engine_traits_copy_limited(&m_respawn_options.m_betrayal_penalty_seconds, stream->read_integer("respawn-options-betrayal-time", 8), 240, 5);
    game_engine_traits_copy_limited(&m_respawn_options.m_respawn_growth_seconds, stream->read_integer("respawn-options-respawn-growth-time", 4), 15, 0);
    game_engine_traits_copy_limited(&m_respawn_options.m_respawn_player_traits_duration_seconds,
        stream->read_integer("respawn-options-player-traits-duration", 6), 60, 5);
    m_respawn_options.m_respawn_player_traits.decode(stream);
    stream->read_bool("social-options-observers");
    long chat_setting = stream->read_integer("social-options-chat", 2);
    if ((unsigned long)chat_setting <= 2)
        m_social_options.m_team_changing = (e_game_engine_team_changing_setting)chat_setting;
    m_social_options.m_flags.set(
        c_game_engine_social_options::_game_engine_social_options_friendly_fire_enabled, stream->read_bool("social-options-friendly-fire"));
    m_social_options.m_flags.set(c_game_engine_social_options::_game_engine_social_options_betrayal_booting_enabled,
        stream->read_bool("social-options-betrayal-booting"));
    m_social_options.m_flags.set(
        c_game_engine_social_options::_game_engine_social_options_enemy_voice_enabled, stream->read_bool("social-options-enemy-voice"));
    m_social_options.m_flags.set(c_game_engine_social_options::_game_engine_social_options_open_channel_voice_enabled,
        stream->read_bool("social-options-open-channel-voice"));
    m_social_options.m_flags.set(c_game_engine_social_options::_game_engine_social_options_dead_player_voice_enabled,
        stream->read_bool("social-options-dead-player-voice"));
    c_game_engine_map_override_options *map_override = &m_map_override_options;
    map_override->m_flags.set(c_game_engine_map_override_options::_game_engine_map_override_option_grenades_on_map,
        stream->read_bool("map-override-grenades-on-map"));
    map_override->m_flags.set(c_game_engine_map_override_options::_game_engine_map_override_option_indestructible_vehicles,
        stream->read_bool("map-override-indestructible-vehicles"));
    m_map_override_options.m_base_player_traits.decode(stream);
    long weapon_set = stream->read_integer("map-override-weapon-set", 8);
    if ((unsigned long)weapon_set & 0x80)
        weapon_set |= -256;
    map_override->set_weapon_set_absolute_index((short)weapon_set);
    long vehicle_set = stream->read_integer("map-override-vehicle-set", 8);
    if ((unsigned long)vehicle_set & 0x80)
        vehicle_set |= -256;
    map_override->set_vehicle_set_absolute_index((short)vehicle_set);
    m_map_override_options.m_red_powerup_traits.decode(stream);
    m_map_override_options.m_blue_powerup_traits.decode(stream);
    m_map_override_options.m_yellow_powerup_traits.decode(stream);
    game_engine_traits_set_powerup(&map_override->m_red_powerup_duration_seconds, stream->read_integer("map-override-red-powerup-duration", 7), 5);
    game_engine_traits_set_powerup(&map_override->m_blue_powerup_duration_seconds, stream->read_integer("map-override-blue-powerup-duration", 7), 30);
    game_engine_traits_set_powerup(&map_override->m_yellow_powerup_duration_seconds, stream->read_integer("map-override-yellow-powerup-duration", 7), 30);
    long team_scoring = stream->read_integer("team-scoring-method", 3);
    if ((unsigned long)team_scoring <= 3)
        m_team_scoring_method = (short)team_scoring;
    else
        m_team_scoring_method = 0;
}

static __forceinline void game_results_encode_statistics(c_bitstream *packet, const char *name, const s_game_results_statistic *statistics,
    const s_integer_statistic_definition *definitions, long count)
{
    for (long index = 0; index < count; ++index)
    {
        packet->write_bool(name, statistics[index].packed & 1);
        unsigned short packed = statistics[index].packed;
        if (packed & 1)
            packet->write_integer("statistic-value", (packed >> 1) - definitions[index].minimum, definitions[index].bit_count);
    }
}

static __forceinline void game_results_write_bytes(c_bitstream *packet, const unsigned char *buffer, long size_in_bits)
{
    unsigned __int64 value = 0;
    unsigned long bytes = size_in_bits / 8;
    do
    {
        value = (value << 8) | *buffer++;
        --bytes;
    } while (bytes != 0);
    packet->write_value_internal(value, size_in_bits);
}
static __forceinline const s_game_results_packet_player *game_results_packet_player(const unsigned char *identifier)
{
    return (const s_game_results_packet_player *)(identifier - offsetof(s_game_results_packet_player, definition.identifier));
}

// @address 0x2454a8 game_results_encode_packet
extern "C" void game_results_encode_packet(c_bitstream *stream, const s_game_results_incremental_update *results)
{
    stream->write_bool("started", results->summary_valid);
    if (results->summary_valid)
        stream->write_raw_data("start-time", &results->summary_value, 32);
    stream->write_bool("finished", results->finish_valid);
    if (results->finish_valid)
        stream->write_raw_data("finish-time", &results->finish_value, 32);
    stream->write_bool("finalized", results->finalized);
    // match: 0x2454a8: the loop walks the player identifiers, the address every player field access is based on.
    const unsigned char *player_identifier = results->players[0].definition.identifier;
    for (long player_index = 0; player_index < 16; ++player_index, player_identifier += sizeof(s_game_results_packet_player))
    {
        stream->write_bool("player-exists", game_results_packet_player(player_identifier)->exists);
        if (game_results_packet_player(player_identifier)->exists)
        {
            stream->write_bool("player-has-definition", game_results_packet_player(player_identifier)->has_definition);
            if (game_results_packet_player(player_identifier)->has_definition)
            {
                stream->write_bool("player-definition-exists", game_results_packet_player(player_identifier)->definition.player_exists);
                stream->write_integer("player-machine-index", game_results_packet_player(player_identifier)->definition.machine_index + 1, 5);
                stream->write_raw_data("player-identifier", game_results_packet_player(player_identifier)->definition.identifier, 64);
                player_configuration_client_encode_packet(stream,
                    (const s_network_game_definitions_client_configuration *)&game_results_packet_player(player_identifier)->definition.configuration,
                    _player_configuration_encoding_full);
                player_configuration_host_encode(
                    stream, &game_results_packet_player(player_identifier)->definition.configuration.host, _player_configuration_encoding_full);
                stream->write_integer("player-standing", game_results_packet_player(player_identifier)->definition.standing + 1, 5);
                stream->write_raw_data("player-final-score", &game_results_packet_player(player_identifier)->definition.final_score, 16);
            }
            else
            {
                stream->write_bool("player-definition-exists", game_results_packet_player(player_identifier)->definition.player_exists);
                stream->write_integer("player-standing", game_results_packet_player(player_identifier)->definition.standing + 1, 5);
            }
        }
    }
    for (long team_index = 0; team_index < 16; ++team_index)
    {
        const s_game_results_packet_team *team = &results->teams[team_index];
        stream->write_bool("team-exists", team->exists);
        if (team->exists)
        {
            stream->write_bool("team-has-identifier", team->has_identifier);
            if (team->has_identifier)
            {
                game_results_write_bytes(stream, team->identifier, 32);
            }
            else
            {
                stream->write_bool("team-flag", team->flag);
                stream->write_integer("team-standing", team->standing + 1, 5);
                stream->write_integer("team-score", (unsigned short)team->final_score, 16);
            }
        }
    }
    for (long player_index = 0; player_index < 16; ++player_index)
    {
        const s_game_results_packet_player_statistics *player = &results->player_statistics[player_index];
        stream->write_bool("player-statistics-exist", player->exists);
        if (player->exists)
        {
            stream->write_bool("player-statistics-valid", player->statistics_valid);
            if (player->statistics_valid)
                game_results_encode_statistics(stream, "player-statistic-valid", player->statistics, g_game_results_statistic_definitions, 51);
            stream->write_bool("medal-statistics-valid", player->statistics_58_valid);
            if (player->statistics_58_valid)
                game_results_encode_statistics(stream, "medal-statistic-valid", player->statistics_58, g_game_results_medal_statistic_definitions, 58);
            stream->write_bool("achievement-statistics-valid", player->statistics_49_valid);
            if (player->statistics_49_valid)
                game_results_encode_statistics(
                    stream, "achievement-statistic-valid", player->statistics_49, g_game_results_achievement_statistic_definitions, 49);
            for (long damage_index = 0; damage_index < 61; ++damage_index)
            {
                const s_game_results_damage_statistics *damage = &player->damage[damage_index];
                stream->write_bool("damage-statistics-active", damage->active);
                if (damage->active)
                    game_results_encode_statistics(stream, "damage-statistic-valid", damage->values, g_game_results_damage_statistic_definitions, 5);
            }
        }
    }
    for (long player_index = 0; player_index < 16; ++player_index)
    {
        for (long opponent_index = 0; opponent_index < 16; ++opponent_index)
        {
            const s_game_results_packet_player_vs_player_statistics *statistics = &results->player_vs_player_statistics[player_index][opponent_index];
            stream->write_bool("versus-statistics-valid", statistics->valid);
            if (statistics->valid)
                game_results_encode_statistics(
                    stream, "versus-statistic-valid", statistics->statistics, g_game_results_player_vs_player_statistic_definitions, 2);
        }
    }
    for (long team_index = 0; team_index < 16; ++team_index)
    {
        const s_game_results_packet_team_statistics *statistics = &results->team_statistics[team_index];
        stream->write_bool("team-statistics-valid", statistics->valid);
        if (statistics->valid)
            game_results_encode_statistics(stream, "team-statistic-valid", statistics->statistics, g_game_results_statistic_definitions, 51);
    }
    for (long machine_index = 0; machine_index < 16; ++machine_index)
    {
        const s_game_results_packet_machine *machine = &results->machines[machine_index];
        stream->write_bool("machine-valid", machine->valid);
        if (machine->valid)
        {
            game_results_write_bytes(stream, machine->identifier, 48);
            stream->write_bool("machine-exists", machine->exists);
            stream->write_bool("machine-connected", machine->connected);
            stream->write_bool("machine-initial-host", machine->initial_host);
            stream->write_bool("machine-host", machine->host);
            stream->write_bool("machine-voluntary-quit", machine->voluntary_quit);
            stream->write_raw_data("machine-bandwidth-estimate", machine->bandwidth_estimate, 80);
        }
    }
}

// @address 0x40710 k_game_engine_zone_variant_times
// @rdata 0x82040710..0x82040728
extern "C" {
const short k_game_engine_zone_variant_times[12] = { 0, 3, 4, 5, 6, 7, 8, 9, 10, 15, 30, 60 };
}
// @rdata-end

// @address 0x244238 game_engine_zone_variant_set
extern "C" void game_engine_zone_variant_set(const s_game_engine_zone_variant_definition *definition, s_game_engine_zone_variant *variant)
{
    game_engine_base_variant_set(variant, (const s_game_engine_base_variant_definition *)definition, _game_engine_sandbox);
    long count = definition->count;
    if ((unsigned long)count > 1)
        variant->m_count = 0;
    else
        variant->m_count = count;

    unsigned short time = k_game_engine_zone_variant_times[definition->index];
    if ((short)time < 0 || (short)time > 60)
        variant->m_time = 5;
    else
        variant->m_time = time;

    unsigned char flag = definition->flags & 1;
    if (flag)
        variant->m_flags |= 1;
    else
        variant->m_flags &= ~1;
    variant->m_traits.set(game_engine_settings_player_traits_get_by_name(definition->traits), true);
}
