// interfacelibxbox/user_interface_text.obj: from the records 0x3c4a70 0x3c5b30 0x487638 0x492c20 0x492fa0 0x493258 0x4932c8 0x493478 0x4934d8 0x493538 0x493590 0x493690 0x493710 0x4937a8 0x4937f0 0x493858 0x4938f0 0x4939a8 0x4939c0 0x493a70 0x493ac0 0x493b28 0x493ce8 0x493d08 0x493d18 0x493d78 0x493ec8 0x493f58 0x493fe8 0x494060 0x4943b8 0x4944c8 0x4c6af0
// @flags /TP /O2 /Oc
enum { k_number_of_users = 4 };
#include "text/font_id_types.h"
#include "interface/user_interface_shared_constants.h"
#include "interface/hopper_information_types.h"
#include "cseries/cseries_types.h"
#include "math/real_math_types.h"
#include "memory/static_arrays.h"
#include "main/levels_map_datum_types.h"
#include "tag_files/tag_types.h"
#include "tag_files/string_ids.h"
#include "game/player_types.h"
#include "game/game_types.h"
#include "shell/shell.h"
#include "interface/user_interface.h"
#include "interface/user_interface_session.h"
#include "networking/logic/network_session_interface.h"
#include "text/unicode.h"
#include "game/multiplayer_game_hopper.h"
#include "input/input_abstraction.h"
#include "interface/user_interface_matchmaking.h"
#include "interface/user_interface_text.h"
#include "interface/user_interface_text_render.h"
#include "math/color_math.h"
#include "text/draw_string.h"
#include "game/game_engine_display.h"
#include "interface/user_interface_controller.h"
#include "interface/game_tag_parser.h"
#include "interface/controller_types.h"
#include "networking/session/network_quality.h"
#include "interface/utf8_types.h"
#include "interface/user_interface_text_parser.h"
#include "networking/logic/network_leaderboard.h"
#include "cache/cache_file_progress_types.h"
#include "input/input_abstraction_types.h"
#include "interface/user_interface_shared_globals.h"
#include "networking/session/network_session_interface_types.h"
#include "memory/data_types.h"
#include "game/game_engine_base_variant.h"
#include "game/game_variant.h"
#include "game/map_variant.h"
#include "cseries/crt_xdk.h"
#include "text/text_group.h"
#include "networking/logic/network_squad_session_countdown.h"
#include "game/game_globals_storage.h"
#include "memory/tls_globals.h"
#include <string.h>
#include <ppcintrinsics.h>
#include "main/levels_globals.h"
#include "interface/user_interface_clock.h"
#include "main/levels.h"
#include <wchar.h>
#include "interface/user_interface_utilities.h"
#include "networking/logic/storage/network_http_buffer_downloader.h"
#include "networking/logic/network_leaderboard_globals_types.h"
#include <math.h>
#include "text/font_cache_lock.h"
#include "text/font_cache_types.h"
#include "text/draw_string_types.h"
#include "text/rasterizer_draw_string_types.h"
#include "interface/user_interface_text_types.h"
#include "interface/controller_inline.h"
#include "interface/user_interface_text_character_types.h"
#include "cseries/maximum.h"
#include "game/game_globals_inline.h"
#if !defined(_XBOX) || defined(__clang__)
#include "../../port/sdk/input/port_input.h"

static inline bool user_interface_kbm_prompts_active(long user_index)
{
    return (user_index == 0 || user_index == -1) && port_input_last_source_is_kbm() != 0;
}

static bool format_kbm_literal(const char *label, wchar_t *out, long cap)
{
    if (!label || !label[0] || cap < 4)
        return false;
    long pos = 0;
    out[pos++] = L'[';
    for (const char *s = label; *s && pos < cap - 2; ++s)
        out[pos++] = (wchar_t)(unsigned char)*s;
    out[pos++] = L']';
    out[pos] = 0;
    return true;
}

static bool format_kbm_prompt(int kbm_action, wchar_t *out, long cap)
{
    char buf[32];
    if (!port_input_kbm_action_label(kbm_action, buf, sizeof(buf)))
        return false;
    return format_kbm_literal(buf, out, cap);
}

static bool format_kbm_static_glyph(long character, wchar_t *out, long cap)
{
    switch (character)
    {
    case _ui_character_a_button:      return format_kbm_literal("Enter", out, cap);
    case _ui_character_b_button:      return format_kbm_literal("Esc", out, cap);
    case _ui_character_x_button:      return format_kbm_literal("X", out, cap);
    case _ui_character_y_button:      return format_kbm_literal("Y", out, cap);
    case _ui_character_left_bumper:   return format_kbm_literal("Q", out, cap);
    case _ui_character_right_bumper:  return format_kbm_literal("E", out, cap);
    case _ui_character_left_trigger:  return format_kbm_literal("PgUp", out, cap);
    case _ui_character_right_trigger: return format_kbm_literal("PgDn", out, cap);
    case _ui_character_dpad_up:       return format_kbm_prompt(PORT_KBM_DPAD_UP, out, cap);
    case _ui_character_dpad_down:     return format_kbm_prompt(PORT_KBM_DPAD_DOWN, out, cap);
    case _ui_character_dpad_left:     return format_kbm_prompt(PORT_KBM_DPAD_LEFT, out, cap);
    case _ui_character_dpad_right:    return format_kbm_prompt(PORT_KBM_DPAD_RIGHT, out, cap);
    case _ui_character_start_button:  return format_kbm_literal("Esc", out, cap);
    case _ui_character_back_button:   return format_kbm_literal("Tab", out, cap);
    case _ui_character_left_stick:    return format_kbm_literal("WASD", out, cap);
    case _ui_character_right_stick:   return format_kbm_literal("Mouse", out, cap);
    default: return false;
    }
}
#endif

struct utf8;
struct s_font_header;
struct utf32;
struct s_font_character;

// @address 0x96e058 ?g_level_globals@@3Us_levels_globals@@A
// @address 0xf283c8 g_network_session_interface_local_users
// @address 0x10b0 k_font_cache_vtable
// @address 0x958c8 ??4?$c_http_stored_buffer_downloader@$0IA@@@QAAAAV0@ABV0@@Z
// @address 0x493258 ?string_contains_special_characters@@YA_NPB_W@Z
// @address 0x493478 ?parse_variant_name_string@@YAXJW4e_utf32@@PAV?$c_static_wchar_string@$0EAA@@@@Z
// @address 0x4934d8 ?parse_hopper_name_string@@YAXJW4e_utf32@@PAV?$c_static_wchar_string@$0EAA@@@@Z
// @address 0x493538 ?parse_hopper_description_string@@YAXJW4e_utf32@@PAV?$c_static_wchar_string@$0EAA@@@@Z
// @address 0x493690 ?parse_motion_sensor_enabled_string@@YAXJW4e_utf32@@PAV?$c_static_wchar_string@$0EAA@@@@Z
// @address 0x493710 ?parse_teams_enabled_string@@YAXJW4e_utf32@@PAV?$c_static_wchar_string@$0EAA@@@@Z
// @address 0x4937a8 ?parse_weapon_set_string@@YAXJW4e_utf32@@PAV?$c_static_wchar_string@$0EAA@@@@Z
// @address 0x4937f0 ?parse_controller_display_name_string@@YAXJW4e_utf32@@PAV?$c_static_wchar_string@$0EAA@@@@Z
// @address 0x493858 ?parse_controller_gamertag_string@@YAXJW4e_utf32@@PAV?$c_static_wchar_string@$0EAA@@@@Z
// @address 0x4938f0 ?parse_multiplayer_game_countdown_string@@YAXJW4e_utf32@@PAV?$c_static_wchar_string@$0EAA@@@@Z
// @address 0x4939a8 ?parse_matchmaking_game_countdown_string@@YAXJW4e_utf32@@PAV?$c_static_wchar_string@$0EAA@@@@Z
// @address 0x4939c0 ?parse_map_load_percent@@YAXJW4e_utf32@@PAV?$c_static_wchar_string@$0EAA@@@@Z
// @address 0x493d18 ?parse_game_engine_string@@YAXJW4e_utf32@@PAV?$c_static_wchar_string@$0EAA@@@@Z
// @address 0x493ce8 ?parse_hud_quantity_string@@YAXJW4e_utf32@@PAV?$c_static_wchar_string@$0EAA@@@@Z
// @address 0x493d08 ?parse_hud_betrayer_string@@YAXJW4e_utf32@@PAV?$c_static_wchar_string@$0EAA@@@@Z

// @address 0x1143340 ?g_last_known_session_state@@3Us_last_known_session_state@@A
s_last_known_session_state g_last_known_session_state;


static inline bool unicode_character_is_magic_character(wchar_t character)
{
    return character >= 0xf7ff && character <= 0xf8fe;
}

bool string_contains_special_characters(const wchar_t *string)
{
    bool result = false;
    for (const wchar_t *character = string; *character; character++) {
        if (unicode_character_is_magic_character(*character)) {
            result = true;
            break;
        }
        if (*character >= 0xe000 && *character <= 0xf8ff) {
            result = true;
            break;
        }
    }
    return result;
}

namespace {
static __forceinline void user_interface_text_gamertag_ascii_to_wchar(const char *source, wchar_t *destination, long maximum_count)
{
    const char *read = source;
    wchar_t *write = destination;
    long remaining = maximum_count;
    while (remaining > 0) {
        unsigned char character = (unsigned char)*read;
        if (remaining == 1)
            *write = 0;
        else if (character <= 127)
            *write = (wchar_t)(signed char)*read;
        else
            *write = 0x25a1;
        if (*read == 0)
            break;
        ++write;
        ++read;
        --remaining;
    }
}
}

// 0x493478
void parse_variant_name_string(long, e_utf32, c_static_wchar_string<1024> *string)
{
    const c_game_variant *game_variant = user_interface_game_settings_get_game_variant();
    if (game_variant)
        g_last_known_session_state.variant_name.set(game_variant->get_name());
    string->set(g_last_known_session_state.variant_name.get_string());
}

void parse_hopper_name_string(long, e_utf32, c_static_wchar_string<1024> *string)
{
    s_hopper_information info;
    unsigned short hopper_identifier = network_squad_session_get_hopper();
    if (hopper_identifier != 0xffff)
        g_last_known_session_state.hopper_identifier = hopper_identifier;
    if (::user_interface_matchmaking_get_hopper_information(hopper_identifier, &info))
        string->set((const wchar_t *)info.name);
}

void parse_hopper_description_string(long, e_utf32, c_static_wchar_string<1024> *static_wchar_string)
{
    unsigned short hopper_identifier = network_squad_session_get_hopper();
    if (hopper_identifier != 0xffff)
        g_last_known_session_state.hopper_identifier = hopper_identifier;
    else
        hopper_identifier = g_last_known_session_state.hopper_identifier;
    const utf8 *description = multiplayer_game_hopper_get_description(hopper_identifier);
    if (description != 0)
        ::utf8_string_to_wchar_string(description, (wchar_t *)(static_wchar_string->m_string), 1024, (long *)(0));
}

namespace {
static __forceinline void user_interface_text_global_string_get(long string_id, c_static_wchar_string<1024> *string)
{
    string->m_string[0] = 0;
    if (string_id != -1) {
        const s_user_interface_shared_globals *globals = ::user_interface_shared_tag_globals_try_and_get();
        if (globals) {
            long string_list_index = globals->global_strings.index;
            if (string_list_index != -1)
                text_group_get_string(string_list_index, string_id, string);
        }
    }
}
}

// 0x493690
void parse_motion_sensor_enabled_string(long, e_utf32, c_static_wchar_string<1024> *string)
{
    if (user_interface_game_settings_get_game_variant())
        user_interface_text_global_string_get(_string_id_on, string);
    else
        user_interface_text_global_string_get(_string_id_off, string);
}

// 0x493710
void parse_teams_enabled_string(long, e_utf32, c_static_wchar_string<1024> *string)
{
    const c_game_variant *game_variant = user_interface_game_settings_get_game_variant();
    if (game_variant != 0)
    {
        if (game_variant->get_active_variant()->get_miscellaneous_options()->get_teams_enabled())
            user_interface_text_global_string_get(_string_id_enabled, string);
        else
            user_interface_text_global_string_get(_string_id_disabled, string);
    }
    else
    {
        user_interface_text_global_string_get(_string_id_disabled, string);
    }
}

// 0x4937a8
void parse_weapon_set_string(long, e_utf32, c_static_wchar_string<1024> *string)
{
    user_interface_text_global_string_get(_string_id_normal, string);
}

// 0x4937f0
void parse_controller_display_name_string(long, e_utf32 character, c_static_wchar_string<1024> *string)
{
    long controller_index;
    switch (character) {
    case _ui_character_player0_profile_name: controller_index = 0; break;
    case _ui_character_player1_profile_name: controller_index = 1; break;
    case _ui_character_player2_profile_name: controller_index = 2; break;
    case _ui_character_player3_profile_name: controller_index = 3; break;
    // match: 0x4937f0: __assume(0) tells c2 the default is dead so it emits no default path
    default: __assume(0);
    }
    string->set(g_controller_globals.controllers[controller_index].display_name);
}

// 0x493858
void parse_controller_gamertag_string(long, e_utf32 character, c_static_wchar_string<1024> *string)
{
    c_static_wchar_string<16> gamertag;
    long controller_index = 0;
    switch (character) {
    case _ui_character_player0_gamertag: break;
    case _ui_character_player1_gamertag: controller_index = 1; break;
    case _ui_character_player2_gamertag: controller_index = 2; break;
    case _ui_character_player3_gamertag: controller_index = 3; break;
    // match: 0x493858: __assume(0) tells c2 the default is dead so it emits no default path
    default: __assume(0);
    }
    if (online_user_get_gamertag(controller_index, &gamertag))
        string->set(gamertag.get_string());
}


// 0x4938f0
void parse_multiplayer_game_countdown_string(long, e_utf32, c_static_wchar_string<1024> *string)
{
    if (::user_interface_squad_in_or_after_countdown())
    {
        long seconds = maximum(::user_interface_squad_get_countdown_timer(), 0L);
        string->print(L"%d:%02d", seconds / 60, seconds % 60);
    }
    else
    {
        string->set(L"-:--");
    }
}

// 0x4939a8
void parse_matchmaking_game_countdown_string(long, e_utf32, c_static_wchar_string<1024> *string)
{
    string->print(L":%02d", 0);
}

// @address 0x493a70 ?parse_name_of_player_who_last_delayed_countdown@@YAXJW4e_utf32@@PAV?$c_static_wchar_string@$0EAA@@@@Z
void parse_name_of_player_who_last_delayed_countdown(long, e_utf32, c_static_wchar_string<1024> *string)
{
    long player_index = network_squad_session_get_countdown_delayed_culprit();
    if (player_index != -1)
    {
        const s_player_configuration *player_data = ::user_interface_squad_get_player_data(player_index);
        if (player_data != 0)
            string->set(player_data->host.name);
    }
}

// @address 0x493ac0 ?parse_squad_leader_string@@YAXJW4e_utf32@@PAV?$c_static_wchar_string@$0EAA@@@@Z
void parse_squad_leader_string(long, e_utf32, c_static_wchar_string<1024> *string)
{
    long leader_player_index = ::user_interface_squad_get_leader_player_index();
    if (leader_player_index != -1)
    {
        const s_player_configuration *player_data = ::user_interface_squad_get_player_data(leader_player_index);
        if (player_data != 0)
            g_last_known_session_state.squad_leader_name.set(player_data->host.name);
    }
    string->set(g_last_known_session_state.squad_leader_name.get_string());
}

// 0x493ce8
void parse_hud_quantity_string(long, e_utf32 character, c_static_wchar_string<1024> *string)
{
    long quantity = 0;
    if (character == _ui_character_hud_quantity)
        quantity = 42;
    string->print(L"%d", quantity);
}

void parse_hud_betrayer_string(long, e_utf32, c_static_wchar_string<1024> *static_wchar_string)
{
    static_wchar_string->m_string[0] = 0;
}

// 0x493d18
void parse_game_engine_string(long, e_utf32 character, c_static_wchar_string<1024> *string)
{
    if (game_is_multiplayer())
    {
        wchar_t buffer[24];
        if (character == _ui_character_ge_round_time_left)
            game_engine_get_time_string(buffer);
        string->set(buffer);
    }
}

// @address 0x493d78 ?parse_network_game_player_count@@YAXJW4e_utf32@@PAV?$c_static_wchar_string@$0EAA@@@@Z
void parse_network_game_player_count(long, e_utf32, c_static_wchar_string<1024> *string)
{
    long strings[17] = {312, 313, 314, 315, 316, 317, 318, 319, 320, 321, 322, 323, 324, 325, 326, 327, 328};
    unsigned long count = 0;
    network_group_session_get_membership(0, 0, 0, 0, 0, 0, 0, (long *)&count, 0, 0);
    long player_count = maximum(user_interface_squad_get_player_count(), (long)count);
    if (player_count > 0)
        g_last_known_session_state.last_player_count = player_count < 16 ? player_count : 16;
    user_interface_global_string_get(strings[g_last_known_session_state.last_player_count], reinterpret_cast<unsigned short *>(string->m_string));
}

bool online_user_get_gamertag(long controller_index, c_static_wchar_string<16> *gamertag)
{
    bool result = false;
    char name[16];
    if (XUserGetName(controller_index, name, 16) == 0) {
        user_interface_text_gamertag_ascii_to_wchar(name, gamertag->m_string, 16);
        result = true;
    }
    return result;
}

// @address 0x493590 ?parse_mapname_string@@YAXJW4e_utf32@@PAV?$c_static_wchar_string@$0EAA@@@@Z
void parse_mapname_string(long, e_utf32, c_static_wchar_string<1024> *string)
{
    levels_map_datum level;
    memset(&level, 0, sizeof(level));
    e_campaign_id campaign_id;
    e_map_id map_id;
    if (!level.display_name[0] && network_life_cycle_session_get_map(&campaign_id, &map_id))
    {
        if (campaign_id != -1)
            levels_try_and_get_datum(g_level_globals.map_levels, map_id, (s_datum *)&level);
        else
            levels_try_and_get_datum(g_level_globals.multiplayer_levels, map_id, (s_datum *)&level);
    }
    if (!level.display_name[0])
    {
        const s_user_interface_globals *session = &g_user_interface_globals;
        long campaign = session->campaign_id.peek();
        long map = session->map_id.peek();
        if (campaign != -1 && map != -1)
            levels_try_and_get_datum(g_level_globals.map_levels, map, (s_datum *)&level);
    }
    if (level.display_name[0])
        g_last_known_session_state.map_name.set(level.display_name);
    string->set(g_last_known_session_state.map_name.get_string());
}

void parse_map_load_percent(long, e_utf32, c_static_wchar_string<1024> *string)
{
    long map_id = g_user_interface_globals.map_id.peek();
    float progress = 0.0f;
    levels_map_datum map;
    if (levels_try_and_get_datum(g_level_globals.map_levels,
            map_id, (s_datum *)&map))
        progress = ::cache_file_map_progress_helper(_scenario_type_campaign, (char const *)(map.name), _cache_file_progress_fraction);
    string->print(L"%d", (long)(progress * 100.0f));
}

static __forceinline unsigned int user_interface_text_ustrnlen(const wchar_t *text, long capacity)
{
    unsigned int length;
    for (length = 0; length < (unsigned int)capacity && text[length] != 0; ++length) {}
    return length;
}
// @address 0x487638 ?parse_magic_character@c_user_interface_text_parse_manager@@QAA_N_WPA_WJ@Z
bool c_user_interface_text_parse_manager::parse_magic_character(wchar_t character, wchar_t *text, long capacity)
{
    bool result = false;
    wchar_t replacement[1024];
    replacement[0] = 0;
    for (long parser_index = 0; parser_index < g_parse_manager.m_count; ++parser_index)
    {
        if (g_parse_manager.m_parsers[parser_index].parser->get_magic_character() == character)
        {
            if (static_cast<c_magic_string_game_tag_parser *>(g_parse_manager.m_parsers[parser_index].parser)->get_magic_string(replacement, 1024))
            {
                long replacement_length = user_interface_text_ustrnlen(replacement, 1024);
                long text_length = user_interface_text_ustrnlen(text, capacity);
                if (replacement_length == 0)
                    memmove(text, text + 1, text_length * sizeof(wchar_t));
                else if (replacement_length == 1)
                    *text = *replacement;
                else
                {
                    long total_length = text_length + replacement_length;
                    if (total_length > capacity)
                        replacement_length = replacement_length - total_length + capacity;
                    long length_delta = replacement_length - 1;
                    long new_length = text_length + length_delta + 1;
                    long shift_bytes = length_delta * sizeof(wchar_t);
                    void *destination = reinterpret_cast<char *>(text + 1) + shift_bytes;
                    const void *source = text + 1;
                    long move_count = text_length - 1 > capacity - length_delta - 1 ? capacity - length_delta - 1 : text_length - 1;
                    if (move_count > 0)
                        memmove(destination, source, move_count * sizeof(wchar_t));
                    memcpy(text, replacement, shift_bytes + sizeof(wchar_t));
                    text[new_length - 1] = 0;
                }
                result = true;
            }
            break;
        }
    }
    return result;
}

#if !defined(_XBOX) || defined(__clang__)
static const wchar_t *g_kbm_parse_context_string = 0;

static bool kbm_wcs_contains_ci(const wchar_t *haystack, const char *needle)
{
    if (!haystack || !needle || !needle[0])
        return false;
    for (const wchar_t *h = haystack; *h; ++h) {
        const wchar_t *hp = h;
        const char *np = needle;
        while (*hp && *np) {
            unsigned int hc = (unsigned int)*hp;
            unsigned int nc = (unsigned char)*np;
            if (hc >= 'A' && hc <= 'Z') hc += 'a' - 'A';
            if (nc >= 'A' && nc <= 'Z') nc += 'a' - 'A';
            if (hc != nc)
                break;
            ++hp;
            ++np;
        }
        if (*np == 0)
            return true;
    }
    return false;
}
#endif

// @address 0x4932c8 ?user_interface_parse_string@@YA_NJPA_WJ@Z
bool user_interface_parse_string(long user_index, wchar_t *string, long buffer_size)
{
#if !defined(_XBOX) || defined(__clang__)
    const wchar_t *prev_context_string = g_kbm_parse_context_string;
    g_kbm_parse_context_string = string;
#endif
    bool result = true;
    long string_length = wcslen(string);
    for (wchar_t *p = string; *p; ++p) {
        if (unicode_character_is_magic_character(*p)) {
            if (!g_parse_manager.parse_magic_character(*p, p, buffer_size - (p - string))) {
                result = false;
                break;
            }
        } else {
            c_static_wchar_string<1024> replacement;
            long characters_consumed = 0;
            long character = *p;
            replacement.m_string[0] = 0;
            if (character >= 0xe000 && character <= 0xf8ff) {
                for (unsigned int i = 0; i < 127; ++i) {
                    if (character == k_user_interface_string_parsers[i].character) {
                        if (k_user_interface_string_parsers[i].parse) {
                            k_user_interface_string_parsers[i].parse(user_index, (e_utf32)character, &replacement);
                            characters_consumed = 1;
                        }
                        break;
                    }
                }
#if !defined(_XBOX) || defined(__clang__)
                if (characters_consumed == 0 && character >= 0xe100 && character <= 0xe111 &&
                    user_interface_kbm_prompts_active(user_index)) {
                    if (format_kbm_static_glyph(character, replacement.m_string, 1024))
                        characters_consumed = 1;
                }
#endif
            }
            long replacement_length = wcslen(replacement.get_string());
            if (replacement_length > 0) {
                wchar_t *destination = p;
                long length_delta = replacement_length - characters_consumed;
                long position = destination - string;
                long remaining = string_length - position;
                if (string_length + length_delta < buffer_size) {
                    if (remaining > 0)
                        memmove(destination + replacement_length, destination + characters_consumed, remaining * 2);
                    memcpy(destination, replacement.get_string(), replacement_length * 2);
                    string_length += length_delta;
                    string[string_length] = 0;
#if !defined(_XBOX) || defined(__clang__)
                    p += replacement_length - 1;
#endif
                }
            }
        }
    }
#if !defined(_XBOX) || defined(__clang__)
    g_kbm_parse_context_string = prev_context_string;
#endif
    return result;
}

// @address 0x493ec8 ?parse_coop_level_name@@YAXJW4e_utf32@@PAV?$c_static_wchar_string@$0EAA@@@@Z
void parse_coop_level_name(long user_index, e_utf32 character, c_static_wchar_string<1024> *string)
{
    e_campaign_id campaign_id = _campaign_id_none;
    e_map_id map_id = _map_id_none;
    levels_map_datum level;
    string->m_string[0] = 0;
    network_life_cycle_session_get_map(&campaign_id, &map_id);
    if (campaign_id != _campaign_id_none && map_id != _map_id_none)
    {
        if (levels_try_and_get_datum(g_level_globals.map_levels, map_id, (s_datum *)&level))
            string->set(level.display_name);
    }
}

// @address 0x493f58 ?parse_coop_level_description@@YAXJW4e_utf32@@PAV?$c_static_wchar_string@$0EAA@@@@Z
void parse_coop_level_description(long user_index, e_utf32 character, c_static_wchar_string<1024> *string)
{
    e_campaign_id campaign_id = _campaign_id_none;
    e_map_id map_id = _map_id_none;
    levels_map_datum level;
    string->m_string[0] = 0;
    network_life_cycle_session_get_map(&campaign_id, &map_id);
    if (campaign_id != _campaign_id_none && map_id != _map_id_none)
    {
        if (levels_try_and_get_datum(g_level_globals.map_levels, map_id, (s_datum *)&level))
            string->set(level.description);
    }
}

// @address 0x493fe8 ?parse_coop_difficulty@@YAXJW4e_utf32@@PAV?$c_static_wchar_string@$0EAA@@@@Z
void parse_coop_difficulty(long user_index, e_utf32 character, c_static_wchar_string<1024> *string)
{
    e_campaign_difficulty_level difficulty = user_interface_game_settings_get_campaign_difficulty();
    long string_id;
    switch (difficulty)
    {
    case _campaign_difficulty_level_easy:
        string_id = _string_id_easy;
        break;
    case _campaign_difficulty_level_normal:
        string_id = _string_id_normal;
        break;
    case _campaign_difficulty_level_heroic:
        string_id = _string_id_heroic;
        break;
    case _campaign_difficulty_level_legendary:
        string_id = _string_id_legendary;
        break;
    default:
        string_id = _string_id_normal;
        break;
    }
    user_interface_global_string_get(string_id, reinterpret_cast<unsigned short *>(string));
}

static __forceinline wchar_t user_interface_text_animated_button(wchar_t first, c_static_wchar_string<1024> *animated)
{
    unsigned long time = g_user_interface_globals.milliseconds.peek();
    unsigned long phase = time % 500;
    wchar_t glyph;
    if (phase < 166)
        glyph = first;
    else if (phase < 333)
        glyph = first + 1;
    else
        glyph = first + 2;
    wchar_t text[2];
    text[0] = glyph;
    text[1] = 0;
    animated->set(text);
    return animated->m_string[0];
}

// @address 0x494060 ?parse_controller_button_string@@YAXJW4e_utf32@@PAV?$c_static_wchar_string@$0EAA@@@@Z
void parse_controller_button_string(long user_index, e_utf32 character, c_static_wchar_string<1024> *output)
{
#if !defined(_XBOX) || defined(__clang__)
    if (user_interface_kbm_prompts_active(user_index))
    {
        int kbm_action = -1;
        switch (character)
        {
        case _ui_character_button_jump:                   kbm_action = PORT_KBM_JUMP; break;
        case _ui_character_button_switch_grenade:         kbm_action = PORT_KBM_SWITCH_GRENADE; break;
        case _ui_character_button_action_reload:
            kbm_action = kbm_wcs_contains_ci(g_kbm_parse_context_string, "reload")
                ? PORT_KBM_RELOAD : PORT_KBM_ACTION;
            break;
        case _ui_character_button_switch_weapon:          kbm_action = PORT_KBM_SWITCH_WEAPON; break;
        case _ui_character_button_melee_attack:           kbm_action = PORT_KBM_MELEE; break;
        case _ui_character_button_flashlight:             kbm_action = PORT_KBM_FLASHLIGHT; break;
        case _ui_character_button_throw_grenade:
            kbm_action = (kbm_wcs_contains_ci(g_kbm_parse_context_string, "boost") ||
                          kbm_wcs_contains_ci(g_kbm_parse_context_string, "brake"))
                ? PORT_KBM_VEHICLE_BOOST : PORT_KBM_THROW_GRENADE;
            break;
        case _ui_character_button_fire:                   kbm_action = PORT_KBM_FIRE; break;
        case _ui_character_button_start:                  kbm_action = PORT_KBM_PAUSE; break;
        case _ui_character_button_back:                   kbm_action = PORT_KBM_SCOREBOARD; break;
        case _ui_character_button_crouch:                 kbm_action = PORT_KBM_CROUCH; break;
        case _ui_character_button_scope_zoom:             kbm_action = PORT_KBM_ZOOM; break;
        case _ui_character_button_accept:                 kbm_action = PORT_KBM_JUMP; break;
        case _ui_character_button_cancel:                 kbm_action = PORT_KBM_MELEE; break;
        case _ui_character_button_dual_fire:
            if (kbm_wcs_contains_ci(g_kbm_parse_context_string, "grenade") ||
                kbm_wcs_contains_ci(g_kbm_parse_context_string, "plant"))
                kbm_action = PORT_KBM_THROW_GRENADE;
            else if ((port_input_keyboard_context() & PORT_KBM_CONTEXT_SEAT) ||
                     kbm_wcs_contains_ci(g_kbm_parse_context_string, "boost") ||
                     kbm_wcs_contains_ci(g_kbm_parse_context_string, "brake"))
                kbm_action = PORT_KBM_VEHICLE_BOOST;
            else
                kbm_action = PORT_KBM_FIRE_LEFT;
            break;
        case _ui_character_button_action_generic:
        case _ui_character_button_action_weapon_primary:  kbm_action = PORT_KBM_ACTION; break;
        case _ui_character_button_action_weapon_secondary:kbm_action = PORT_KBM_SWITCH_GRENADE; break;
        case _ui_character_button_use_equipment:          kbm_action = PORT_KBM_USE_EQUIPMENT; break;
        default: break;
        }
        wchar_t kbm_label[32];
        if (kbm_action >= 0 && format_kbm_prompt(kbm_action, kbm_label, 32))
        {
            output->set(kbm_label);
            return;
        }
    }
#endif
    signed char action;
    switch (character)
    {
    case _ui_character_button_jump: action = 0; break;
    case _ui_character_button_switch_grenade: action = 1; break;
    case _ui_character_button_action_reload: action = 3; break;
    case _ui_character_button_switch_weapon: action = 2; break;
    case _ui_character_button_melee_attack: action = 5; break;
    case _ui_character_button_flashlight: action = 17; break;
    case _ui_character_button_throw_grenade: action = 7; break;
    case _ui_character_button_fire: action = 8; break;
    case _ui_character_button_start: action = 11; break;
    case _ui_character_button_back: action = 12; break;
    case _ui_character_button_crouch: action = 13; break;
    case _ui_character_button_scope_zoom: action = 14; break;
    case _ui_character_button_lean_left: action = 15; break;
    case _ui_character_button_lean_right: action = 16; break;
    case _ui_character_button_accept: action = 18; break;
    case _ui_character_button_cancel: action = 19; break;
    case _ui_character_button_dual_fire: action = -1; break;
    case _ui_character_button_action_generic:
    case _ui_character_button_action_weapon_primary: action = 3; break;
    case _ui_character_button_action_weapon_secondary: action = 4; break;
    case _ui_character_button_push_to_talk: action = 26; break;
    case _ui_character_button_use_equipment: action = 6; break;
    // match: 0x494060: unreachable hint omits dead branch check
    default: __assume(0);
    }
    long controller_index = user_interface_controller_get_rumble_enabled(user_index);
    c_static_wchar_string<1024> animated;
    s_controller_look_settings preferences;
    if (controller_index != -1)
        memcpy(&preferences, &input_abstraction_globals.look_settings[controller_index], sizeof(preferences));
    else
        input_abstraction_get_default_preferences(&preferences.preferences);
    wchar_t label[2];
    label[1] = 0;
    label[0] = L'?';
    unsigned long button;
    if (action != -1)
        button = preferences.preferences.buttons[action];
    else if (preferences.preferences.buttons[_input_action_right_trigger] == _gamepad_button_right_trigger)
        button = _gamepad_button_left_trigger;
    else
        button = _gamepad_button_right_trigger;
    switch (button)
    {
    default: break;
    case _gamepad_button_a: label[0] = _ui_character_a_button; break;
    case _gamepad_button_b: label[0] = _ui_character_b_button; break;
    case _gamepad_button_x: label[0] = _ui_character_x_button; break;
    case _gamepad_button_y: label[0] = _ui_character_y_button; break;
    case _gamepad_button_left_bumper: label[0] = _ui_character_left_bumper; break;
    case _gamepad_button_right_bumper: label[0] = _ui_character_right_bumper; break;
    case _gamepad_button_left_trigger: label[0] = _ui_character_left_trigger; break;
    case _gamepad_button_right_trigger: label[0] = _ui_character_right_trigger; break;
    case _gamepad_button_dpad_up: label[0] = _ui_character_dpad_up; break;
    case _gamepad_button_dpad_down: label[0] = _ui_character_dpad_down; break;
    case _gamepad_button_dpad_left: label[0] = _ui_character_dpad_left; break;
    case _gamepad_button_dpad_right: label[0] = _ui_character_dpad_right; break;
    case _gamepad_button_start: label[0] = _ui_character_start_button; break;
    case _gamepad_button_back: label[0] = _ui_character_back_button; break;
    case _gamepad_button_left_thumb: label[0] = user_interface_text_animated_button(_unicode_private_font_icon_animating_left_thumbstick0, &animated); break;
    case _gamepad_button_right_thumb: label[0] = user_interface_text_animated_button(_unicode_private_font_icon_animating_right_thumbstick0, &animated); break;
    }
    output->set(label);
}

// @address 0x4943b8 ?parse_thumbstick_string@@YAXJW4e_utf32@@PAV?$c_static_wchar_string@$0EAA@@@@Z
void parse_thumbstick_string(long user_index, e_utf32 button, c_static_wchar_string<1024> *output)
{
#if !defined(_XBOX) || defined(__clang__)
    if (user_interface_kbm_prompts_active(user_index))
    {
        if (button == _ui_character_stick_move)
        {
            output->set(L"[WASD]");
            return;
        }
        if (button == _ui_character_stick_look)
        {
            output->set(L"[Mouse]");
            return;
        }
    }
#endif
    long controller_index = user_interface_controller_get_rumble_enabled(user_index);
    wchar_t label[4];
    memset(label, 0, sizeof(label));
    s_controller_look_settings preferences;
    if (controller_index != -1)
        memcpy(&preferences, &input_abstraction_globals.look_settings[controller_index], sizeof(preferences));
    else
        input_abstraction_get_default_preferences(&preferences.preferences);
    if (button == _ui_character_stick_move)
    {
        switch (preferences.preferences.joystick_preset)
        {
        case 0:
        case 2:
            label[0] = _ui_character_left_stick;
            break;
        case 1:
        case 3:
            label[0] = _ui_character_right_stick;
            break;
        }
    }
    else if (button == _ui_character_stick_look)
    {
        switch (preferences.preferences.joystick_preset)
        {
        case 0:
        case 2:
            label[0] = _ui_character_right_stick;
            break;
        case 1:
        case 3:
            label[0] = _ui_character_left_stick;
            break;
        }
    }
    output->set(label);
}

// @address 0x4944c8 ?parse_animating_thumbstick@@YAXJW4e_utf32@@PAV?$c_static_wchar_string@$0EAA@@@@Z
void parse_animating_thumbstick(long, e_utf32 character, c_static_wchar_string<1024> *string)
{
    const s_user_interface_globals *clock = &g_user_interface_globals;
    unsigned long time = clock->milliseconds.peek();
    unsigned long phase = time % 500;
    wchar_t text[2];
    if (phase < 166)
    {
        switch (character)
        {
        case _ui_character_animating_left_thumbstick: text[0] = _unicode_private_font_icon_animating_left_thumbstick0; break;
        case _ui_character_animating_right_thumbstick: text[0] = _unicode_private_font_icon_animating_right_thumbstick0; break;
        default: text[0] = L'?'; break;
        }
    }
    else if (phase < 333)
    {
        switch (character)
        {
        case _ui_character_animating_left_thumbstick: text[0] = _unicode_private_font_icon_animating_left_thumbstick1; break;
        case _ui_character_animating_right_thumbstick: text[0] = _unicode_private_font_icon_animating_right_thumbstick1; break;
        default: text[0] = L'?'; break;
        }
    }
    else
    {
        switch (character)
        {
        case _ui_character_animating_left_thumbstick: text[0] = _unicode_private_font_icon_animating_left_thumbstick2; break;
        case _ui_character_animating_right_thumbstick: text[0] = _unicode_private_font_icon_animating_right_thumbstick2; break;
        default: text[0] = L'?'; break;
        }
    }
    text[1] = 0;
    string->set(text);
}

// @address 0x4c6af0 user_interface_text_get_map_name
extern "C" bool user_interface_text_get_map_name(long user_index, wchar_t *output, unsigned long capacity)
{
    c_static_wchar_string<1024> text;
    const c_map_variant *map_variant =
        (const c_map_variant *)user_interface_game_settings_try_and_get_map_variant();
    if (map_variant != 0 && wcslen(map_variant->m_metadata.name) != 0)
    {
        text.set(map_variant->m_metadata.name);
    }
    else
    {
        e_campaign_id campaign_id;
        e_map_id map_id;
        levels_map_datum level;
        if (network_life_cycle_session_get_map(&campaign_id, &map_id)
            && levels_try_and_get_datum(g_level_globals.multiplayer_levels, map_id, (s_datum *)&level))
            text.set(level.display_name);
    }
    unsigned long count = capacity;
    if (count > 1024)
        count = 1024;
    ustrnzcpy(output, text.get_string(), count);
    return true;
}

// @address 0x3c5b30 parse_player_gamertag_string
// Start leaderboard tracking for every signed-in live user.

static __forceinline qword user_interface_text_controller_get_player_xuid(const c_controller_interface *controller)
{
    qword xuid = 0;
    if (controller->get_user_index() >= 0 && controller->get_user_index() < k_number_of_users)
        xuid = g_network_session_interface_local_users[controller->get_user_index()].xuid;
    return xuid;
}

void parse_player_gamertag_string()
{
    for (e_controller_index controller_index = first_controller(); controller_index != k_no_controller;
        controller_index = next_controller(controller_index))
    {
        c_controller_interface *controller = controller_get(controller_index);
        if (controller->is_signed_in_to_live())
        {
            long user_index = controller->get_user_index();
            s_network_leaderboard_query_user_statistics *user =
                (s_network_leaderboard_query_user_statistics *)&network_leaderboard_globals.users[user_index];
            if (!user->active)
            {
                user->xuid = user_interface_text_controller_get_player_xuid(controller);
                user->controller_index = controller_index;
                online_user_get_gamertag(controller_index, (c_static_wchar_string<16> *)user->gamertag);
                memset(user->statistics, 0, sizeof(user->statistics));
                user->downloader = c_http_stored_buffer_downloader<128>();
                user->downloader.reset();
                user->retry_count = 0;
                user->state = 1;
                user->active = true;
            }
        }
    }
}
// @address 0x493b28 ?parse_live_service_qos_string@@YAXJW4e_utf32@@PAV?$c_static_wchar_string@$0EAA@@@@Z
void parse_live_service_qos_string(long, e_utf32 character, c_static_wchar_string<1024> *string)
{
    long value = 0;
    long nat_type, bandwidth, flags;
    s_network_quality_data quality;
    float bandwidth_kilobytes = 0.0f;
    if (network_session_interface_get_network_quality(&quality, &nat_type, &bandwidth, &flags)) {
        switch (character) {
        case _ui_character_qprs: value = quality.latency_minimum; break;
        case _ui_character_qprr: value = quality.latency_average; break;
        case _ui_character_qpimi: value = quality.latency_maximum; break;
        case _ui_character_qpime: value = quality.packet_loss; break;
        case _ui_character_qbwd: bandwidth_kilobytes = (float)quality.downstream_bandwidth / 1024.0f; break;
        case _ui_character_qbwu: bandwidth_kilobytes = (float)quality.upstream_bandwidth / 1024.0f; break;
        case _ui_character_qmaxm: value = flags; break;
        case _ui_character_qebw: value = (long)((float)bandwidth * (1.0f / 1024.0f)); break;
        case _ui_character_qnat: value = nat_type; break;
        }
    }
    if (bandwidth_kilobytes != 0.0f)
        string->print(L"%.1f", bandwidth_kilobytes);
    else
        string->print(L"%d", value);
}

static __forceinline void user_interface_text_set_font(c_draw_string *draw, long font)
{
    long clamped = (font > 0 ? font : 0) < 10 ? (font > 0 ? font : 0) : 10;
    draw->m_font_id = (e_font_id)clamped;
    draw->m_font_header = font_get_header(clamped);
}
static __forceinline short user_interface_text_tab_count(const c_draw_string *draw)
{
    return draw->m_tab_count;
}
static __forceinline void user_interface_text_set_tabs(c_draw_string *draw, const short *tabs, short count)
{
    if (tabs)
    {
        draw->m_tab_count = (unsigned short)count > 16 ? 16 : count;
        if (user_interface_text_tab_count(draw) > 0) memcpy(draw->m_tab_stops, tabs, user_interface_text_tab_count(draw) * sizeof(short));
    }
    else draw->m_tab_count = 0;
}
static __forceinline void user_interface_text_offset(real_rectangle2d *bounds, float x, float y)
{
    bounds->x0 += x;
    bounds->y0 += y;
    bounds->x1 += x;
    bounds->y1 += y;
}
static __forceinline void user_interface_text_set_real_bounds(c_draw_string *draw, const real_rectangle2d *bounds, const real_rectangle2d *clip)
{
    if (!(bounds->x1 - bounds->x0 < 0.0f) && !(bounds->y1 - bounds->y0 < 0.0f)) draw->m_bounds = *bounds;
    if (!(clip->x1 - clip->x0 < 0.0f) && !(clip->y1 - clip->y0 < 0.0f)) draw->m_clip_bounds = *clip;
}
__forceinline void c_rasterizer_draw_string::set_rotation(float rotation)
{
    while (rotation > 6.2831855f) rotation -= 6.2831855f;
    while (rotation < 0.0f) rotation += 6.2831855f;
    m_rotation = rotation;
    if (rotation == 0.0f)
    {
        m_sine = 0.0f;
        m_cosine = 1.0f;
    }
    else
    {
        m_sine = (float)sin(rotation);
        m_cosine = (float)cos(rotation);
    }
}
static __forceinline void user_interface_text_apply_origin(c_rasterizer_draw_string *draw, const real_point2d *const &origin)
{
    if (origin)
        draw->m_rotation_origin = *origin;
    else
    {
        draw->m_rotation_origin.y = 0.0f;
        draw->m_rotation_origin.x = 0.0f;
    }
}

// match: 0x492c20: inline_depth(0) keeps c_draw_string::set_shadow_color a call; default depth inlines it
#pragma inline_depth(0)
// @address 0x492c20 user_interface_text_render
extern "C" void user_interface_text_render(const s_user_interface_text_render_data *data, const rectangle2d *viewport)
{
    const wchar_t *text = static_cast<const s_gui_text_widget_extra_large_render_data *>(data)->text;
    c_rasterizer_draw_string rasterizer_draw;
    c_draw_string &draw = rasterizer_draw;
    c_font_cache cache;
    real_rectangle2d bounds = data->bounds;
    real_rectangle2d clip = data->clip_bounds;
    float vertical_offset = (float)viewport->y0;
    float horizontal_offset = (float)viewport->x0;
    user_interface_text_offset(&bounds, horizontal_offset, vertical_offset);
    user_interface_text_offset(&clip, horizontal_offset, vertical_offset);
    user_interface_text_set_font(&draw, data->font);
    draw.m_style = (e_text_style)data->style;
    draw.m_justification = (e_text_justification)data->justification;
    draw.m_initial_line = data->shadow_style;
    real_argb_color color;
    real_argb_color *converted_color = pixel32_to_real_argb_color(data->color, &color);
    draw.set_color_deprecated(converted_color, true);
    draw.set_shadow_color(pixel32_to_real_argb_color(data->shadow_color, &color));
    draw.m_flags.set(c_draw_string::_text_flag_wrap_horizontally, data->wrap_horizontally);
    draw.m_flags.set(c_draw_string::_text_flag_align_bottom, data->align_vertically);
    user_interface_text_set_tabs(&draw, data->tabs, data->tab_count);
    const real_point2d *origin = &data->rotation_origin;
    user_interface_text_apply_origin(&rasterizer_draw, origin);
    rasterizer_draw.set_rotation(data->rotation);
    draw.m_scale = data->scale;
    user_interface_text_set_real_bounds(&draw, &bounds, &clip);
    draw.m_flags.set(c_draw_string::_text_flag_precache, true);
    draw.c_draw_string::draw(&cache, text);
}

static __forceinline void user_interface_text_copy(wchar_t *destination, const wchar_t *source, long capacity)
{
    for (long i = 0; i <= capacity - 1; ++i, ++source)
    {
        if ((destination[i] = *source) == 0) break;
    }
    destination[capacity - 1] = 0;
}
static __forceinline void user_interface_text_real_bounds(const rectangle2d *source, real_rectangle2d *destination)
{
    destination->x0 = (float)source->x0;
    destination->y0 = (float)source->y0;
    destination->x1 = (float)source->x1;
    destination->y1 = (float)source->y1;
}
static __forceinline void user_interface_text_short_bounds(const real_rectangle2d *source, rectangle2d *destination)
{
    destination->x0 = (short)source->x0;
    destination->y0 = (short)source->y0;
    destination->x1 = (short)source->x1;
    destination->y1 = (short)source->y1;
}
// @address 0x492fa0 ?compute_bounds_internal@c_user_interface_text@@IAA_NPAVc_font_cache@@JPBTrectangle2d@@M1FPAT3@2@Z
bool c_user_interface_text::compute_bounds_internal(c_font_cache *cache, long user, const rectangle2d *bounds, float scale,
    const rectangle2d *clip, short caret, rectangle2d *text_bounds, rectangle2d *caret_bounds)
{
    const wchar_t *source = get_string();
    c_user_interface_draw_string draw;
    wchar_t text[1024];
    if (source && *source)
    {
        user_interface_text_copy(text, source, 1024);
        if (m_flags & 1) ustrnupr(text, 1024);
    }
    else text[0] = 0;
    text[caret] = 0;
    user_interface_parse_string(0, text, 1024);
    rectangle2d scaled_bounds;
    real_rectangle2d real_bounds;
    user_interface_text_real_bounds(bounds, &real_bounds);
    user_interface_text_short_bounds(&real_bounds, &scaled_bounds);
    user_interface_text_set_font(&draw, m_font);
    draw.m_initial_line = m_drop_shadow_style;
    draw.m_style = (e_text_style)m_style;
    draw.m_justification = (e_text_justification)m_justification;
    draw.m_flags.set(c_draw_string::_text_flag_align_bottom, (m_flags & 2) != 0);
    draw.m_flags.set(c_draw_string::_text_flag_wrap_horizontally, (m_flags & 4) != 0);
    user_interface_text_set_tabs(&draw, m_tabs, (short)m_tab_count);
    draw.set_bounds(&scaled_bounds);
    return draw.compute_wchar_string_bounds(cache, text, caret_bounds, text_bounds);
}


// @address 0x5bec98 ?parse_vehicle_set_string@@YAXJW4e_utf32@@PAV?$c_static_wchar_string@$0EAA@@@@Z
void parse_vehicle_set_string(long, e_utf32, c_static_wchar_string<1024> *)
{
}

// @address 0x5bec98 ?parse_build_number_string@@YAXJW4e_utf32@@PAV?$c_static_wchar_string@$0EAA@@@@Z
void parse_build_number_string(long, e_utf32, c_static_wchar_string<1024> *)
{
}

// @address 0x5bec98 ?parse_hud_scoreboard_string@@YAXJW4e_utf32@@PAV?$c_static_wchar_string@$0EAA@@@@Z
void parse_hud_scoreboard_string(long, e_utf32, c_static_wchar_string<1024> *)
{
}

// @address 0x72fe38 k_user_interface_string_parsers
s_user_interface_string_parser k_user_interface_string_parsers[127] = {
    { L"&", L'&', 0 },
    { L"a_button", _ui_character_a_button, 0 },
    { L"b_button", _ui_character_b_button, 0 },
    { L"x_button", _ui_character_x_button, 0 },
    { L"y_button", _ui_character_y_button, 0 },
    { L"left_bumper", _ui_character_left_bumper, 0 },
    { L"right_bumper", _ui_character_right_bumper, 0 },
    { L"left_trigger", _ui_character_left_trigger, 0 },
    { L"right_trigger", _ui_character_right_trigger, 0 },
    { L"dpad_up", _ui_character_dpad_up, 0 },
    { L"dpad_down", _ui_character_dpad_down, 0 },
    { L"dpad_left", _ui_character_dpad_left, 0 },
    { L"dpad_right", _ui_character_dpad_right, 0 },
    { L"start_button", _ui_character_start_button, 0 },
    { L"back_button", _ui_character_back_button, 0 },
    { L"left_thumb", _ui_character_left_thumb, 0 },
    { L"right_thumb", _ui_character_right_thumb, 0 },
    { L"left_stick", _ui_character_left_stick, 0 },
    { L"right_stick", _ui_character_right_stick, 0 },
    { L"mapname", _ui_character_mapname, parse_mapname_string },
    { L"variant", _ui_character_variant, parse_variant_name_string },
    { L"hopper_name", _ui_character_hopper_name, parse_hopper_name_string },
    { L"hopper_description", _ui_character_hopper_description, parse_hopper_description_string },
    { L"motion_sensor_enabled", _ui_character_motion_sensor_enabled, parse_motion_sensor_enabled_string },
    { L"teams_enabled", _ui_character_teams_enabled, parse_teams_enabled_string },
    { L"vehicle_set", _ui_character_vehicle_set, parse_vehicle_set_string },
    { L"weapon_set", _ui_character_weapon_set, parse_weapon_set_string },
    { L"player0_profile_name", _ui_character_player0_profile_name, parse_controller_display_name_string },
    { L"player1_profile_name", _ui_character_player1_profile_name, parse_controller_display_name_string },
    { L"player2_profile_name", _ui_character_player2_profile_name, parse_controller_display_name_string },
    { L"player3_profile_name", _ui_character_player3_profile_name, parse_controller_display_name_string },
    { L"player0_gamertag", _ui_character_player0_gamertag, parse_controller_gamertag_string },
    { L"player1_gamertag", _ui_character_player1_gamertag, parse_controller_gamertag_string },
    { L"player2_gamertag", _ui_character_player2_gamertag, parse_controller_gamertag_string },
    { L"player3_gamertag", _ui_character_player3_gamertag, parse_controller_gamertag_string },
    { L"countdown", _ui_character_countdown, parse_multiplayer_game_countdown_string },
    { L"name_of_last_person_to_delay_countdown", _ui_character_name_of_last_person_to_delay_countdown, parse_name_of_player_who_last_delayed_countdown },
    { L"build_number", _ui_character_build_number, parse_build_number_string },
    { L"leader", _ui_character_leader, parse_squad_leader_string },
    { L"qprs", _ui_character_qprs, parse_live_service_qos_string },
    { L"qprr", _ui_character_qprr, parse_live_service_qos_string },
    { L"qpimi", _ui_character_qpimi, parse_live_service_qos_string },
    { L"qpime", _ui_character_qpime, parse_live_service_qos_string },
    { L"qbwd", _ui_character_qbwd, parse_live_service_qos_string },
    { L"qbwu", _ui_character_qbwu, parse_live_service_qos_string },
    { L"qmaxm", _ui_character_qmaxm, parse_live_service_qos_string },
    { L"qebw", _ui_character_qebw, parse_live_service_qos_string },
    { L"qnat", _ui_character_qnat, parse_live_service_qos_string },
    { L"assault_rifle", _ui_character_assault_rifle, 0 },
    { L"ball", _ui_character_ball, 0 },
    { L"battle_rifle", _ui_character_battle_rifle, 0 },
    { L"bomb", _ui_character_bomb, 0 },
    { L"brute_shot", _ui_character_brute_shot, 0 },
    { L"carbine", _ui_character_carbine, 0 },
    { L"claymore_grenade", _ui_character_claymore_grenade, 0 },
    { L"energy_sword", _ui_character_energy_sword, 0 },
    { L"excavator", _ui_character_excavator, 0 },
    { L"firebomb_grenade", _ui_character_firebomb_grenade, 0 },
    { L"flag", _ui_character_flag, 0 },
    { L"flamethrower", _ui_character_flamethrower, 0 },
    { L"frag_grenade", _ui_character_frag_grenade, 0 },
    { L"fuel_rod_gun", _ui_character_fuel_rod_gun, 0 },
    { L"gravity_hammer", _ui_character_gravity_hammer, 0 },
    { L"guardian_eye_beam", _ui_character_guardian_eye_beam, 0 },
    { L"machinegun_turret", _ui_character_machinegun_turret, 0 },
    { L"magnum", _ui_character_magnum, 0 },
    { L"missile_launcher", _ui_character_missile_launcher, 0 },
    { L"needler", _ui_character_needler, 0 },
    { L"particle_beam_rifle", _ui_character_particle_beam_rifle, 0 },
    { L"plasma_grenade", _ui_character_plasma_grenade, 0 },
    { L"plasma_mortar_turret", _ui_character_plasma_mortar_turret, 0 },
    { L"plasma_pistol", _ui_character_plasma_pistol, 0 },
    { L"plasma_rifle", _ui_character_plasma_rifle, 0 },
    { L"plasma_turret", _ui_character_plasma_turret, 0 },
    { L"pump_laser", _ui_character_pump_laser, 0 },
    { L"rocket_launcher", _ui_character_rocket_launcher, 0 },
    { L"sentinel_beam", _ui_character_sentinel_beam, 0 },
    { L"shotgun", _ui_character_shotgun, 0 },
    { L"smg", _ui_character_smg, 0 },
    { L"sniper_rifle", _ui_character_sniper_rifle, 0 },
    { L"spike_rifle", _ui_character_spike_rifle, 0 },
    { L"hud_quantity", _ui_character_hud_quantity, parse_hud_quantity_string },
    { L"hud_betraying_player", _ui_character_hud_betraying_player, parse_hud_betrayer_string },
    { L"ge_round_time_left", _ui_character_ge_round_time_left, parse_game_engine_string },
    { L"target_player_gamertag", _ui_character_target_player_gamertag, 0 },
    { L"game_player_count", _ui_character_game_player_count, parse_network_game_player_count },
    { L"coop_level_name", _ui_character_coop_level_name, parse_coop_level_name },
    { L"coop_level_description", _ui_character_coop_level_description, parse_coop_level_description },
    { L"coop_difficulty", _ui_character_coop_difficulty, parse_coop_difficulty },
    { L"hud_scoreboard_player_score", _ui_character_hud_scoreboard_player_score, parse_hud_scoreboard_string },
    { L"hud_scoreboard_other_player_score", _ui_character_hud_scoreboard_other_player_score, parse_hud_scoreboard_string },
    { L"hud_scoreboard_variant_name", _ui_character_hud_scoreboard_variant_name, parse_hud_scoreboard_string },
    { L"hud_scoreboard_time_left", _ui_character_hud_scoreboard_time_left, parse_hud_scoreboard_string },
    { L"hour_12", _ui_character_hour_12, 0 },
    { L"hour_24", _ui_character_hour_24, 0 },
    { L"minute", _ui_character_minute, 0 },
    { L"day", _ui_character_day, 0 },
    { L"month", _ui_character_month, 0 },
    { L"year", _ui_character_year, 0 },
    { L"button_jump", _ui_character_button_jump, parse_controller_button_string },
    { L"button_switch_grenade", _ui_character_button_switch_grenade, parse_controller_button_string },
    { L"button_action_reload", _ui_character_button_action_reload, parse_controller_button_string },
    { L"button_switch_weapon", _ui_character_button_switch_weapon, parse_controller_button_string },
    { L"button_melee_attack", _ui_character_button_melee_attack, parse_controller_button_string },
    { L"button_flashlight", _ui_character_button_flashlight, parse_controller_button_string },
    { L"button_throw_grenade", _ui_character_button_throw_grenade, parse_controller_button_string },
    { L"button_fire", _ui_character_button_fire, parse_controller_button_string },
    { L"button_start", _ui_character_button_start, parse_controller_button_string },
    { L"button_back", _ui_character_button_back, parse_controller_button_string },
    { L"button_crouch", _ui_character_button_crouch, parse_controller_button_string },
    { L"button_scope_zoom", _ui_character_button_scope_zoom, parse_controller_button_string },
    { L"button_lean_left", _ui_character_button_lean_left, parse_controller_button_string },
    { L"button_lean_right", _ui_character_button_lean_right, parse_controller_button_string },
    { L"button_accept", _ui_character_button_accept, parse_controller_button_string },
    { L"button_cancel", _ui_character_button_cancel, parse_controller_button_string },
    { L"stick_move", _ui_character_stick_move, parse_thumbstick_string },
    { L"stick_look", _ui_character_stick_look, parse_thumbstick_string },
    { L"animating_left_thumbstick", _ui_character_animating_left_thumbstick, parse_animating_thumbstick },
    { L"animating_right_thumbstick", _ui_character_animating_right_thumbstick, parse_animating_thumbstick },
    { L"matchmaking_countdown", _ui_character_matchmaking_countdown, parse_matchmaking_game_countdown_string },
    { L"map_load_percent", _ui_character_map_load_percent, parse_map_load_percent },
    { L"button_dual_fire", _ui_character_button_dual_fire, parse_controller_button_string },
    { L"button_action_generic", _ui_character_button_action_generic, parse_controller_button_string },
    { L"button_action_weapon_primary", _ui_character_button_action_weapon_primary, parse_controller_button_string },
    { L"button_action_weapon_secondary", _ui_character_button_action_weapon_secondary, parse_controller_button_string },
    { L"button_push_to_talk", _ui_character_button_push_to_talk, parse_controller_button_string },
    { L"button_use_equipment", _ui_character_button_use_equipment, parse_controller_button_string },
};
