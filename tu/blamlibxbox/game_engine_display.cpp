#include <stdio.h>
// @address 0x187780 _vsnwprintf_s
// blamlibxbox/game_engine_display.obj: from the records 0x134638 0x187710 0x29a740 0x29a880 0x29a978 0x29aa38 0x29aab0
// @compiler xdk6130
#include "cseries/cseries_unicode.h"
#include "game/game_engine_local_globals.h"
// @flags /TP /O1 /J /Oc
#include "math/real_math_types.h"
#include "tag_files/tag_types.h"
#include "game/game_globals.h"
#include <stdarg.h>
#include "memory/static_arrays.h"
#include "game/game_engine.h"
#include "simulation/simulation.h"
#include "cutscene/cinematics.h"
#include "game/game_engine_display.h"
#include "game/game_engine_team.h"
#include "game/game_engine_util.h"
#include "math/color_math.h"
#include "math/real_math_constants.h"
#include "cseries/global_real_argb_colors.h"
#include "rasterizer/rasterizer_main.h"
#include "rasterizer/rasterizer.h"
#include "tag_files/string_ids.h"
#include "game/game_engine_default.h"
#include "text/text_group.h"
#include "rasterizer/rasterizer_video_mode_types.h"
#include "cache/cache_files.h"
#include "rasterizer/rasterizer_render_globals_types.h"
#include "memory/tls_globals.h"
#include "game/game_globals_storage.h"
#include "game/player_mapping_types.h"
#include "game/game_engine_settings_definitions.h"
struct s_game_engine_event_data;


#include "scenario/scenario.h"
#include "tag_files/tag_groups.h"
#include "game/game_engine_globals_messages.h"

#include "game/game_engine_display_globals.h"
#include "game/game_engine_scoring_globals.h"
#include "game/multiplayer_runtime_globals.h"
#include "game/game_engine_list.h"


namespace {
#include "scenario/scenario_game_globals.h"
}
struct s_game_engine_event_data;
#include "game/game_engine_hud_state_types.h"
namespace {
static __forceinline int game_engine_display_hud_engine_index_out_of_range(long engine_index)
{
    if (engine_index < 1)
        return 1;
    if (engine_index > 10)
        return 1;
    return 0;
}
}



static __forceinline c_game_variant *game_engine_display_current_variant()
{
    game_options *options = &tls_game_globals()->options;
    c_game_variant *result = 0;
    if (options->game_mode == 2)
        result = &options->game_variant;
    return result;
}
// match: 0x29a740: /Ot keeps and+cmpwi-cr6 split plus the wider frame; /O1 fuses and. and shrinks pdata 4f04->4d04 (also covers 0x29a880 mulhw divide).
#pragma optimize("t", on)
// @address 0x29a740 ?game_engine_render_ui@@YAXJ@Z
void game_engine_render_ui(long user_index)
{
    if ((unsigned long)user_index <= 3) {
        if (tls_game_engine_globals()->pending_flags & (1 << user_index)) {
            ((void (*)(e_game_engine_globals_message_type,
                signed char,
                short))post_game_engine_globals_message)(_game_engine_globals_message_set_pending_flag,
                (signed char)user_index,
                0);
        } else {
            ((unsigned char *)&g_game_engine_display_user_flags)[user_index] = 0;
        }
        float fade = 1.0f;
        if (user_index != -1 && ::simulation_in_progress()) {
            fade = tls_game_engine_globals()->user_fade[user_index];
            if (fade <= 0.0f)
                return;
        }
        struct {
            rectangle2d bounds;
            char padding[8];
            real_argb_color color;
        } locals;
        short width, height;
        if (rasterizer_main_get_video_mode()->fIsWideScreen) {
            width = (short)c_rasterizer::render_globals.width;
            height = (short)c_rasterizer::render_globals.height;
        } else {
            width = 640;
            height = 480;
        }
        locals.color.alpha = fade;
        locals.bounds.x0 = 0;
        locals.bounds.y0 = 0;
        locals.bounds.y1 = height;
        locals.bounds.x1 = width;
        locals.color.rgb = global_real_argb_colors[_real_argb_black].rgb;
        c_rasterizer::begin_high_quality_blend();
        draw_quad(&locals.bounds, real_argb_color_to_pixel32(&locals.color));
    }
}

// @address 0x29a880 game_engine_get_time_string
extern "C" void game_engine_get_time_string(wchar_t *buffer)
{
    int seconds;
    int minutes;
    int remainder;

    c_static_wchar_string<256> time_text;
    seconds = tls_game_engine_globals()->timer_seconds;
    if (seconds <= 0)
        seconds = 0;
    c_static_wchar_string<256> minutes_text;
    c_static_wchar_string<256> seconds_text;
    minutes = seconds / 60;
    remainder = seconds - minutes * 60;
    if (minutes == 0) {
        minutes_text.print(L" ");
    } else {
        minutes_text.print(L"%d", minutes);
    }
    if (remainder <= 9) {
        seconds_text.print(L"0%d", remainder);
    } else {
        seconds_text.print(L"%d", remainder);
    }
    time_text.print(L"%s:%s", minutes_text.m_string, seconds_text.m_string);
    usnzprintf(buffer, 20, L"%s", time_text.m_string);
}

// @address 0x29a978 ?game_engine_get_place_string@@YAXJPAV?$c_static_wchar_string@$0BAA@@@@Z
void game_engine_get_place_string(long place_index, c_static_wchar_string<256> *string)
{
    long string_index = 0;
    long string_indices[16] = { 208, 209, 210, 211, 212, 213, 214, 215, 216, 217, 218, 219, 220, 221, 222, 223 };
    if ((unsigned long)place_index <= 15)
        string_index = string_indices[place_index];
    game_engine_get_universal_string(string_index, string);
}

// match: 0x29aa38: /Ot keeps cmplwi-cr6+beq-cr6; /O1 drops to cr0-implied cmplwi+beq.
#pragma optimize("t", on)
// @address 0x29aa38 game_engine_get_score_string
extern "C" void game_engine_get_score_string(long score, c_static_wchar_string<256> *score_string)
{
    int current_engine_index;
    const c_game_engine *current_engine;
    current_engine_index = tls_game_engine_globals()->game_engine_index;
    if ((tls_game_engine_globals()->game_engine_index >= 1) && (tls_game_engine_globals()->game_engine_index <= 10)) {
        current_engine = game_engines[current_engine_index];
        if (current_engine != 0) {
            current_engine->get_score_string(score, score_string);
            return;
        }
    }
    score_string->print(L"%d", score);
}
void game_engine_get_team_name(long team_index, c_static_wchar_string<256> *name) {
    s_game_globals *game_globals = scenario_get_game_globals();
    s_multiplayer_globals_definition *multiplayer_globals = (s_multiplayer_globals_definition *)tag_get(0x6d756c67,
        game_globals->multiplayer_globals_index);
    name->clear();
    if (multiplayer_globals->universal.count > 0) {
        s_multiplayer_universal_globals_definition *universal =
            (s_multiplayer_universal_globals_definition *)tag_block_get_element_with_size(
            &multiplayer_globals->universal, 0, sizeof(s_multiplayer_universal_globals_definition));
        if (universal->team_names.index != -1) {
            long team_string_id = string_id_number(team_index);
            string_list_get_normal_string(universal->team_names.index, team_string_id, name);
        }
    }
}

// @address 0x187710 ?print@?$c_static_wchar_string@$0BAA@@@QAAPB_WPB_WZZ
template<long k_maximum_count>
const wchar_t *c_static_wchar_string<k_maximum_count>::print(const wchar_t *format, ...)
{
    va_list args;
    va_start(args, format);
    _vsnwprintf_s(m_string, k_maximum_count - 1, (unsigned int)-1, format, args);
    m_string[k_maximum_count - 1] = 0;
    return m_string;
}
template const wchar_t *c_static_wchar_string<256>::print(const wchar_t *format, ...);

// match: 0x134638: /Ot keeps r30 color, split clrlwi/cmplwi-cr6+lhzx/extsh and duplicated epilogues; /O1 recolors r28, fuses, tail-merges.
#pragma optimize("t", on)
// @address 0x134638 ?game_engine_hud_get_state_message@@YA_NJPA_WJ_N@Z
bool game_engine_hud_get_state_message(long user_index, wchar_t *buffer, long buffer_size, bool flag)
{
    bool result = false;
    long engine_index = tls_game_engine_globals()->game_engine_index;
    if (!game_engine_display_hud_engine_index_out_of_range(engine_index) && game_engines[engine_index] != 0) {
        if (simulation_is_pending_start()) {
            result = true;
            buffer[0] = 0;
        } else {
            short state = tls_game_engine_globals()->game_engine_state;
            if (state == 2 && tls_game_globals()->game_finished == 0) {
                c_static_wchar_string<256> state_message;
                game_engine_get_universal_string(230, &state_message);
                usnzprintf(buffer, 256, state_message.m_string);
                result = true;
            } else if (state == 3) {
                c_static_wchar_string<256> state_message;
                game_engine_get_universal_string(231, &state_message);
                usnzprintf(buffer, 256, state_message.m_string);
                result = true;
            } else if (state == 1 && (unsigned char)flag == 0) {
                if (game_engine_display_current_variant() != 0) {
                    {
                        game_options *options = &tls_game_globals()->options;
                        c_game_variant *variant = &options->game_variant;
                        if (options->game_mode != 2)
                            variant = 0;
                        if (variant->round_limit <= 1) {
                            result = true;
                            buffer[0] = 0;
                        } else {
                            s_multiplayer_globals_definition *multiplayer = (s_multiplayer_globals_definition *)tag_get(0x6d756c67,
                                scenario_get_game_globals()->multiplayer_globals_index);
                            s_multiplayer_runtime_globals_definition *runtime =
                        (s_multiplayer_runtime_globals_definition *)tag_block_get_element_with_size(&multiplayer->runtime,
                                0,
                                sizeof(s_multiplayer_runtime_globals_definition));
                            c_static_wchar_string<256> round_count_format;
                            string_list_get_normal_string(runtime->in_game_strings.index, 232, &round_count_format);
                            long player_index = -1;
                            if (user_index != -1)
                                player_index = tls_player_mapping_globals()->player_indices[user_index];
                            ::build_multiplayer_string(player_index, (wchar_t const *)(round_count_format.m_string),
                                0, 256, (wchar_t *)(buffer));
                            result = true;
                        }
                    }
                }
            } else {
                if (g_local_game_engine_globals->player_states[user_index].index == -1 ||
                    g_local_game_engine_globals->player_states[user_index].state_response_index == -1) {
                    buffer[0] = 0;
                } else {
                    bool show_message = true;
                    long message_string_id = -1;
                    s_multiplayer_globals_definition *multiplayer = (s_multiplayer_globals_definition *)tag_get(0x6d756c67,
                        scenario_get_game_globals()->multiplayer_globals_index);
                    s_multiplayer_runtime_globals_definition *runtime =
                        (s_multiplayer_runtime_globals_definition *)tag_block_get_element_with_size(&multiplayer->runtime,
                        0,
                        sizeof(s_multiplayer_runtime_globals_definition));
                    s_hud_state_entry *entry = (s_hud_state_entry *)tag_block_get_element_with_size(
                        &runtime->state_responses, g_local_game_engine_globals->player_states[user_index].state_response_index, sizeof(s_hud_state_entry));
                    if (user_index != -1) {
                        long player_index = tls_player_mapping_globals()->player_indices[user_index];
                        long state_index = -1;
                        if (player_index != -1)
                            state_index = game_engine_get_player_state_index(player_index);
                        if (player_index != -1 && state_index == g_local_game_engine_globals->player_states[user_index].index) {
                            if (game_engine_has_teams() != 0)
                                message_string_id = entry->with_teams_value;
                            else
                                message_string_id = entry->without_teams_value;
                            if ((unsigned char)flag != 0 &&
                                (g_local_game_engine_globals->player_states[user_index].index == 26 ||
                                    g_local_game_engine_globals->player_states[user_index].index == 5 ||
                                    g_local_game_engine_globals->player_states[user_index].index == 27 ||
                                        g_local_game_engine_globals->player_states[user_index].index == 6 ||
                                    g_local_game_engine_globals->player_states[user_index].index == 28 ||
                                        g_local_game_engine_globals->player_states[user_index].index == 7))
                                show_message = false;
                        } else {
                            show_message = false;
                        }
                    } else {
                        show_message = false;
                    }
                    if (!show_message || message_string_id == -1 || message_string_id == 0) {
                        buffer[0] = 0;
                    } else {
                        c_static_wchar_string<256> state_entry_format;
                        string_list_get_normal_string(runtime->in_game_strings.index, message_string_id,
                            &state_entry_format);
                        long player_index = -1;
                        if (user_index != -1)
                            player_index = tls_player_mapping_globals()->player_indices[user_index];
                        ::build_multiplayer_string(player_index, (wchar_t const *)(state_entry_format.m_string),
                            0, 256, (wchar_t *)(buffer));
                        result = true;
                    }
                }
            }
        }
    }
    return result;
}
// match: 0x134638: trailing /O1 restore sets TU-end flags; without it the operator[] instantiation keeps /Ot and strength-reduces mulli r11,r4,12.
#pragma optimize("", on)

// @address 0x7f18b4 g_game_engine_display_user_flags
long g_game_engine_display_user_flags;

// @address 0x3ac90 global_real_argb_colors
#if !defined(_XBOX) || defined(__clang__)
const real_argb_color global_real_argb_colors[k_global_real_argb_color_count] =
{
    { 1.0f, 1.0f, 1.0f, 1.0f },
    { 1.0f, 0.5f, 0.5f, 0.5f },
    { 1.0f, 0.0f, 0.0f, 0.0f },
    { 1.0f, 1.0f, 0.0f, 0.0f },
    { 1.0f, 0.0f, 1.0f, 0.0f },
    { 1.0f, 0.0f, 0.0f, 1.0f },
    { 1.0f, 0.0f, 1.0f, 1.0f },
    { 1.0f, 1.0f, 1.0f, 0.0f },
    { 1.0f, 1.0f, 0.0f, 1.0f },
    { 1.0f, 1.0f, 0.41f, 0.7f },
    { 1.0f, 0.39f, 0.58f, 0.93f },
    { 1.0f, 1.0f, 0.5f, 0.0f },
    { 1.0f, 0.44f, 0.05f, 0.43f },
    { 1.0f, 0.5f, 1.0f, 0.83f },
    { 1.0f, 0.0f, 0.39f, 0.0f },
    { 1.0f, 1.0f, 0.63f, 0.48f },
    { 1.0f, 0.81f, 0.13f, 0.56f },
};
#endif
