// interfacelibxbox/gui_screen_matchmaking_searching.obj: from the records 0x4d9498 0x4d94b0 0x4d9568 0x4d9638 0x4d96e8 0x4d97a0 0x4d9870 0x4d98f0 0x4d99b0 0x4d9a38 0x4d9ac8 0x4d9b58 0x4d9bf0 0x4d9c78 0x4d9d10 0x4d9dc8 0x4d9ee0 0x4da418 0x4da478 0x4da550 0x4da840 0x4dab48 0x4dae10 0x4db0d0 0x4db1d0 0x4db278 0x4db438 0x4db578
// @flags /TP /O2 /Oc
// @address 0x4d9870 ?parse_xml_hopper_name@@YA_NPAXPA_WJ@Z
// @address 0x4da418 ?post_initialize@c_gui_screen_matchmaking_searching@@UAAXXZ
// @address 0x4d98f0 ?parse_xml_hopper_population@@YA_NPAXPA_WJ@Z
// @address 0x4d99b0 ?parse_xml_current_players@@YA_NPAXPA_WJ@Z
// @address 0x4d9d10 ?parse_xml_search_preference@@YA_NPAXPA_WJ@Z
// @address 0x4d9dc8 ?parse_xml_party_leader@@YA_NPAXPA_WJ@Z
// @address 0x4da840 ?update_status@c_gui_screen_matchmaking_searching@@AAAXXZ
// @address 0x4d9ee0 ?initialize@c_gui_screen_matchmaking_searching@@UAAXXZ
// @address 0x4db1d0 ?handle_dialog_result@c_gui_screen_matchmaking_searching@@UAA_NPBVc_dialog_result_message@@@Z
// @address 0x4dae10 ?update_ping_pong@c_gui_screen_matchmaking_searching@@AAAXXZ
// @address 0x4db0d0 ?update_progress@c_gui_screen_matchmaking_searching@@AAAXK@Z

#include "interface/gui_render_data_size_types.h"
#include "interface/user_interface_matchmaking_progress.h"
#include "interface/gui_screen_widget_text.h"
#include "cseries/crt_xdk.h"
#include "networking/online/online_url.h"
#include "interface/user_interface_time.h"
#include "interface/user_interface_matchmaking.h"
#include "interface/user_interface_utilities.h"
#include "shell/shell.h"
#include <stddef.h>
#include "game/player_types.h"
#include "game/game_types.h"
#include "interface/gui_widget_types.h"
#include "memory/static_arrays.h"
#include "interface/user_interface_messages.h"
#include <ppcintrinsics.h>
#include <math.h>
#include <stdlib.h>
#include "interface/gui_screens/scoreboard/gui_screen_scoreboard.h"
#include "interface/user_interface_memory.h"
#include "interface/user_interface_session.h"
#include "networking/logic/network_session_interface.h"
#include "text/unicode.h"
#include "interface/user_interface_networking.h"
#include "networking/logic/network_life_cycle.h"
#include "networking/online/online_xenon.h"
#include "cseries/cseries.h"
#include "interface/gui_screens/matchmaking/gui_screen_matchmaking_searching.h"
#include "interface/gui_screens/matchmaking/gui_screen_matchmaking_searching_string_ids.h"
#include "interface/user_interface_utilities_string_ids.h"
#include "networking/session/network_session_parameter_matchmaking_types.h"
#include "interface/user_interface_sound_effect_types.h"
#include "interface/c_deletable.h"
#include "cseries/language.h"
#include "networking/logic/storage/dynamic_matchmaking_hopper_statistics_types.h"
#include "saved_games/content_catalogue.h"
#include "interface/game_tag_parser.h"
#include "interface/gui_screens/pregame_lobby/advanced_screen/hopper_statistics_types.h"
#include "interface/ui_allocation_marker_types.h"
#include "game/game_results.h"
#include "game/game_results_storage_types.h"
#include "interface/gui_roster_list_externs.h"
#include "interface/c_gui_bitmap_widget.h"
#include "interface/c_gui_text_widget.h"
#include "networking/online/online_guide_xenon.h"
#include "networking/tools/network_blf.h"
#include "networking/tools/network_blf_payload.h"
#include "interface/user_interface_allocation.h"
#include "interface/user_interface_squad_session.h"
#include "interface/gui_widget.h"
#include "interface/gui_data.h"
#include <new>
#include "interface/gui_roster_data.h"
#include "interface/c_gui_screen_widget.h"
#include "networking/logic/storage/network_http_buffer_url.h"
#include "networking/logic/storage/network_http_buffer_downloader.h"
#include "networking/tools/blf_authentication_types.h"
#include "networking/session/network_session_parameters_generic.h"
#include "networking/session/network_session.h"
#include "networking/session/network_session_parameters.h"
#include "networking/logic/life_cycle/life_cycle_runtime_globals.h"
#include "math/random_math.h"
#include "math/random_math_range.h"
#include "game/game_engine_runtime_constants.h"
#include "interface/user_interface_matchmaking_statistics.h"
#include "networking/logic/storage/network_http_buffer_url_api.h"
#include "interface/c_gui_list_item_widget.h"
#include "interface/c_gui_list_widget.h"
#include "interface/list_widget_definition.h"
#include "interface/gui_screens/player_select/player_select_screen_message.h"

struct utf8;
enum e_matchmaking_search_preference;

enum
{
    k_matchmaking_progress_animation = 30,
    k_leave_searching_squad_dialog = 0x30022,
    k_abort_matchmaking_dialog = 0x30021,
    k_scoreboard_controller_component = 13
};

static __forceinline void gui_screen_matchmaking_searching_delete_data(c_gui_data *data)
{
    data->~c_gui_data();
    user_interface_free(data);
}

// @address 0xf11308 g_network_life_cycle_globals


static __forceinline bool gui_screen_matchmaking_searching_get_squad(c_network_session **session)
{
    bool result = false;
    if (g_network_life_cycle_globals.initialized) {
        c_network_session *current = (c_network_session *)g_network_life_cycle_globals.manager.m_active_squad_session;
        if (current->current_local_state() != 0) {
            *session = current;
            result = true;
        }
    }
    return result;
}

static __forceinline const c_game_results *gui_screen_matchmaking_searching_final_results()
{
    const c_game_results *results = 0;
    bool finalized = g_current_game_results.initialized &&
        g_current_game_results.finalized;
    if (finalized && g_current_game_results.finish_valid)
        results = &g_current_game_results;
    return results;
}

// @address 0x4d94b0 ?parse_xml_their_skill_range@@YA_NPAXPA_WJ@Z
bool parse_xml_their_skill_range(void *context, wchar_t *buffer, long buffer_length)
{
    c_gui_screen_matchmaking_searching *screen = (c_gui_screen_matchmaking_searching *)context;
    bool success = false;
    if (screen) {
        c_static_wchar_string<1024> text;
        const s_life_cycle_matchmaking_progress *progress = &screen->m_matchmaking_progress;
        if (progress && (progress->type == 1 || progress->type == 2 || progress->type == 3)) {
            const s_matchmaking_search_criteria *criteria = &progress->search_criteria;
            if (criteria) {
                text.print(L"%d-%d", criteria->skill_minimum, criteria->skill_maximum);
            }
        }
        if (text.length() > 0) {
            text.copy_to(buffer, buffer_length);
            success = true;
        }
    }
    return success;
}

// @address 0x4d9568 ?parse_xml_their_skill_dynamic@@YA_NPAXPA_WJ@Z
bool parse_xml_their_skill_dynamic(void *context, wchar_t *buffer, long buffer_length)
{
    c_gui_screen_matchmaking_searching *screen = (c_gui_screen_matchmaking_searching *)context;
    bool success = false;
    if (screen) {
        c_static_wchar_string<1024> text;
        const s_life_cycle_matchmaking_progress *progress = &screen->m_matchmaking_progress;
        if (progress && (progress->type == 1 || progress->type == 2 || progress->type == 3)) {
            const s_matchmaking_search_criteria *criteria = &progress->search_criteria;
            if (criteria) {
                long total = criteria->skill_maximum;
                long current = criteria->skill_minimum;
                if (current == total) {
                    text.print(game_engine_number_format(), current);
                } else {
                    text.print(L"%d-%d", current, total);
                }
            }
        }
        if (text.length() > 0) {
            text.copy_to(buffer, buffer_length);
            success = true;
        }
    }
    return success;
}

// @address 0x4d9638 ?parse_xml_their_size_minimum@@YA_NPAXPA_WJ@Z
bool parse_xml_their_size_minimum(void *context, wchar_t *buffer, long buffer_length)
{
    c_gui_screen_matchmaking_searching *screen = (c_gui_screen_matchmaking_searching *)context;
    bool success = false;
    if (screen) {
        c_static_wchar_string<1024> text;
        const s_life_cycle_matchmaking_progress *progress = &screen->m_matchmaking_progress;
        if (progress && (progress->type == 1 || progress->type == 2 || progress->type == 3)) {
            const s_matchmaking_search_criteria *criteria = &progress->search_criteria;
            if (criteria) {
                text.print(game_engine_number_format(), criteria->size_minimum);
            }
        }
        if (text.length() > 0) {
            text.copy_to(buffer, buffer_length);
            success = true;
        }
    }
    return success;
}

// @address 0x4d96e8 ?parse_xml_their_size_range@@YA_NPAXPA_WJ@Z
bool parse_xml_their_size_range(void *context, wchar_t *buffer, long buffer_length)
{
    c_gui_screen_matchmaking_searching *screen = (c_gui_screen_matchmaking_searching *)context;
    bool success = false;
    if (screen) {
        c_static_wchar_string<1024> text;
        const s_life_cycle_matchmaking_progress *progress = &screen->m_matchmaking_progress;
        if (progress && (progress->type == 1 || progress->type == 2 || progress->type == 3)) {
            const s_matchmaking_search_criteria *criteria = &progress->search_criteria;
            if (criteria) {
                text.print(L"%d-%d", criteria->size_minimum, criteria->size_maximum);
            }
        }
        if (text.length() > 0) {
            text.copy_to(buffer, buffer_length);
            success = true;
        }
    }
    return success;
}

// @address 0x4d97a0 ?parse_xml_their_size_dynamic@@YA_NPAXPA_WJ@Z
bool parse_xml_their_size_dynamic(void *context, wchar_t *buffer, long buffer_length)
{
    c_gui_screen_matchmaking_searching *screen = (c_gui_screen_matchmaking_searching *)context;
    bool success = false;
    if (screen) {
        c_static_wchar_string<1024> text;
        const s_life_cycle_matchmaking_progress *progress = &screen->m_matchmaking_progress;
        if (progress && (progress->type == 1 || progress->type == 2 || progress->type == 3)) {
            const s_matchmaking_search_criteria *criteria = &progress->search_criteria;
            if (criteria) {
                long total = criteria->size_maximum;
                long current = criteria->size_minimum;
                if (current == total) {
                    text.print(game_engine_number_format(), current);
                } else {
                    text.print(L"%d-%d", current, total);
                }
            }
        }
        if (text.length() > 0) {
            text.copy_to(buffer, buffer_length);
            success = true;
        }
    }
    return success;
}

static bool parse_xml_hopper_name(void *context, wchar_t *buffer, long buffer_length)
{
    c_gui_screen_matchmaking_searching *screen = (c_gui_screen_matchmaking_searching *)context;
    bool success = false;
    if (screen) {
        c_static_wchar_string<1024> text;
        const s_life_cycle_matchmaking_progress *progress = &screen->m_matchmaking_progress;
        if (progress) {
            text.set(progress->name);
        }
        if (text.length() > 0) {
            text.copy_to(buffer, buffer_length);
            success = true;
        }
    }
    return success;
}

static bool parse_xml_hopper_population(void *context, wchar_t *buffer, long buffer_length)
{
    c_gui_screen_matchmaking_searching *screen = (c_gui_screen_matchmaking_searching *)context;
    bool success = false;
    if (screen) {
        c_static_wchar_string<1024> text;
        const s_life_cycle_matchmaking_progress *current = screen->get_current_matchmaking_progress();
        const s_life_cycle_matchmaking_progress *progress = 0;
        if (current)
            progress = current;
        if (progress) {
            long population;
            const s_hopper_statistics *statistics = screen->get_hopper_statistics();
            if (statistics && statistics->population >= 0)
                population = statistics->population;
            else
                population = 0;
            text.print(game_engine_number_format(), population);
        }
        if (text.length() > 0) {
            text.copy_to(buffer, buffer_length);
            success = true;
        }
    }
    return success;
}

static bool parse_xml_current_players(void *context, wchar_t *buffer, long buffer_length)
{
    c_gui_screen_matchmaking_searching *screen = (c_gui_screen_matchmaking_searching *)context;
    bool success = false;
    if (screen) {
        c_static_wchar_string<1024> text;
        const s_life_cycle_matchmaking_progress *progress = &screen->m_matchmaking_progress;
        if (progress) {
            text.print(game_engine_number_format(), progress->group_player_count);
        }
        if (text.length() > 0) {
            text.copy_to(buffer, buffer_length);
            success = true;
        }
    }
    return success;
}

// @address 0x4d9a38 ?parse_xml_games_found@@YA_NPAXPA_WJ@Z
bool parse_xml_games_found(void *context, wchar_t *buffer, long buffer_length)
{
    c_gui_screen_matchmaking_searching *screen = (c_gui_screen_matchmaking_searching *)context;
    bool success = false;
    if (screen) {
        c_static_wchar_string<1024> text;
        if (screen->m_matchmaking_progress.type == 1) {
            text.print(game_engine_number_format(), screen->m_matchmaking_progress.search_statistics.games_found);
        }
        if (text.length() > 0) {
            text.copy_to(buffer, buffer_length);
            success = true;
        }
    }
    return success;
}

// @address 0x4d9ac8 ?parse_xml_possible_matches@@YA_NPAXPA_WJ@Z
bool parse_xml_possible_matches(void *context, wchar_t *buffer, long buffer_length)
{
    c_gui_screen_matchmaking_searching *screen = (c_gui_screen_matchmaking_searching *)context;
    bool success = false;
    if (screen) {
        c_static_wchar_string<1024> text;
        if (screen->m_matchmaking_progress.type == 1) {
            text.print(game_engine_number_format(), screen->m_matchmaking_progress.search_statistics.possible_matches);
        }
        if (text.length() > 0) {
            text.copy_to(buffer, buffer_length);
            success = true;
        }
    }
    return success;
}

// @address 0x4d9b58 ?parse_xml_need_more_player_count@@YA_NPAXPA_WJ@Z
bool parse_xml_need_more_player_count(void *context, wchar_t *buffer, long buffer_length)
{
    c_gui_screen_matchmaking_searching *screen = (c_gui_screen_matchmaking_searching *)context;
    bool success = false;
    if (screen) {
        c_static_wchar_string<1024> text;
        if (screen->m_matchmaking_progress.type == 2) {
            text.print(game_engine_number_format(),
                MAX(0, screen->m_matchmaking_progress.search_criteria.need_player_count - screen->m_matchmaking_progress.group_player_count));
        }
        if (text.length() > 0) {
            text.copy_to(buffer, buffer_length);
            success = true;
        }
    }
    return success;
}

// @address 0x4d9c78 ?parse_xml_want_more_player_count@@YA_NPAXPA_WJ@Z
bool parse_xml_want_more_player_count(void *context, wchar_t *buffer, long buffer_length)
{
    c_gui_screen_matchmaking_searching *screen = (c_gui_screen_matchmaking_searching *)context;
    bool success = false;
    if (screen) {
        c_static_wchar_string<1024> text;
        if (screen->m_matchmaking_progress.type == 2) {
            text.print(game_engine_number_format(),
                MAX(0, screen->m_matchmaking_progress.search_criteria.want_player_count - screen->m_matchmaking_progress.group_player_count));
        }
        if (text.length() > 0) {
            text.copy_to(buffer, buffer_length);
            success = true;
        }
    }
    return success;
}

// @address 0x4d9bf0 ?parse_xml_give_up_timer@@YA_NPAXPA_WJ@Z
bool parse_xml_give_up_timer(void *context, wchar_t *buffer, long buffer_length)
{
    c_gui_screen_matchmaking_searching *screen = (c_gui_screen_matchmaking_searching *)context;
    bool success = false;
    if (screen) {
        c_static_wchar_string<1024> text;
        if (screen->m_matchmaking_progress.type == 2) {
            user_interface_get_timespan_string(screen->m_matchmaking_progress.give_up_time, 2, false, &text);
        }
        if (text.length() > 0) {
            text.copy_to(buffer, buffer_length);
            success = true;
        }
    }
    return success;
}

static bool parse_xml_search_preference(void *context, wchar_t *buffer, long buffer_length)
{
    c_gui_screen_matchmaking_searching *screen = (c_gui_screen_matchmaking_searching *)context;
    bool success = false;
    if (screen) {
        c_static_wchar_string<1024> text;
        long search_preference = screen->m_matchmaking_progress.search_preference;
        long string_id = 0;
        if (search_preference != 1) {
            if (search_preference != 2) {
                if (search_preference == 3) {
                    string_id = _matchmaking_searching_string_search_preference_language;
                }
            } else {
                string_id = _matchmaking_searching_string_search_preference_connection;
            }
        } else {
            string_id = _matchmaking_searching_string_search_preference_skill;
        }
        if (string_id != 0) {
            screen->get_string_by_string_id(string_id, &text);
        }
        if (text.length() > 0) {
            text.copy_to(buffer, buffer_length);
            success = true;
        }
    }
    return success;
}

static bool parse_xml_party_leader(void *context, wchar_t *buffer, long buffer_length)
{
    bool success = false;
    if (context != 0) {
        c_static_wchar_string<1024> text;
        if (user_interface_squad_exists()) {
            long leader_player_index = user_interface_squad_get_leader_player_index();
            if (leader_player_index != -1) {
                const s_player_configuration *leader = user_interface_squad_get_player_data(leader_player_index);
                if (leader != 0) {
                    ustrnzcpy(buffer, leader->host.player_name, buffer_length);
                    success = true;
                }
            }
        }
        if (text.length() > 0) {
            text.copy_to(buffer, buffer_length);
            success = true;
        }
    }
    return success;
}

void c_gui_screen_matchmaking_searching::update_status()
{
    long progress_type;
    const s_replicated_life_cycle_matchmaking_progress_search_criteria *criteria = 0;
    long mode;
    bool text_visible, bitmap_visible;
    c_gui_widget *h;
    const s_life_cycle_matchmaking_progress *progress = &m_matchmaking_progress;
    if (progress != 0) {
        progress_type = progress->type;
        if (progress_type == 1 || progress_type == 2 || progress_type == 3) criteria = &progress->replicated_search_criteria;
    }
    mode = progress->type;
    if (mode == 1) {
        set_text_widget_string_id(_matchmaking_searching_string_error_no_map_selected, _matchmaking_searching_string_roster_status_need_more);
        set_text_widget_string_id(_matchmaking_searching_string_title, _matchmaking_searching_string_title_searching_for_match);
        switch ((unsigned int)m_matchmaking_progress.give_up_time) {
        case 0: set_text_widget_string_id(_matchmaking_searching_string_status_search_stage,
            _matchmaking_searching_string_status_search_stage_ideal); break;
        case 1: set_text_widget_string_id(_matchmaking_searching_string_status_search_stage,
            _matchmaking_searching_string_status_search_stage_close_skill); break;
        case 2: set_text_widget_string_id(_matchmaking_searching_string_status_search_stage,
            _matchmaking_searching_string_status_search_stage_for_any); break;
        case 3: set_text_widget_string_id(_matchmaking_searching_string_status_search_stage,
            _matchmaking_searching_string_status_search_stage_desperation); break;
        }
        if (m_matchmaking_progress.has_strict_search_criteria) {
            if (criteria->search_party_big_and_mixed != 0) set_text_widget_string_id(_matchmaking_searching_string_status,
                _matchmaking_searching_string_status_searching_target_ranked_mixed);
            else if (criteria->searching_party_size_min == 1) set_text_widget_string_id(_matchmaking_searching_string_status,
                _matchmaking_searching_string_status_searching_target_ranked_any);
            else set_text_widget_string_id(_matchmaking_searching_string_status, _matchmaking_searching_string_status_searching_target_ranked_party);
        } else {
            if (criteria->searching_party_size_min == 1) set_text_widget_string_id(_matchmaking_searching_string_status,
                _matchmaking_searching_string_status_searching_target_unranked_any);
            else set_text_widget_string_id(_matchmaking_searching_string_status, _matchmaking_searching_string_status_searching_target_unranked_party);
        }

    } else if (mode == 2) {
        set_text_widget_string_id(_matchmaking_searching_string_error_no_map_selected, _matchmaking_searching_string_roster_status_need_more);
        set_text_widget_string_id(_matchmaking_searching_string_title, _matchmaking_searching_string_title_players_found);
        set_text_widget_string_id(_matchmaking_searching_string_status_search_stage,
            _matchmaking_searching_string_status_search_stage_waiting_for_more_players);
        if (m_matchmaking_progress.has_strict_search_criteria) {
            if (criteria->search_party_big_and_mixed != 0) set_text_widget_string_id(_matchmaking_searching_string_status,
                _matchmaking_searching_string_status_assembling_summary_ranked_mixed);
            else if (criteria->searching_party_size_min == 1) set_text_widget_string_id(_matchmaking_searching_string_status,
                _matchmaking_searching_string_status_assembling_summary_ranked_any);
            else set_text_widget_string_id(_matchmaking_searching_string_status, _matchmaking_searching_string_status_assembling_summary_ranked_party);
        } else {
            if (criteria->searching_party_size_min == 1) set_text_widget_string_id(_matchmaking_searching_string_status,
                _matchmaking_searching_string_status_assembling_summary_unranked_any);
            else set_text_widget_string_id(_matchmaking_searching_string_status,
                _matchmaking_searching_string_status_assembling_summary_unranked_party);
        }

    } else if (mode == 3) {
        set_text_widget_string_id(_matchmaking_searching_string_error_no_map_selected, _matchmaking_searching_string_roster_status_have_enough);
        set_text_widget_string_id(_matchmaking_searching_string_title, _matchmaking_searching_string_title_game_setup);
        set_text_widget_string_id(_matchmaking_searching_string_status_search_stage,
            _matchmaking_searching_string_status_search_stage_setting_up_match);
        set_text_widget_string_id(_matchmaking_searching_string_status, _matchmaking_searching_string_status_setting_up_match);
    }
    text_visible = (::user_interface_matchmaking_squad_get_search_preference() != 0);
    h = get_child_widget(_gui_text_widget, _matchmaking_searching_string_search_preference);
    if (h != 0) h->set_visible(text_visible);
    bitmap_visible = (::user_interface_matchmaking_squad_get_search_preference() != 0);
    h = get_child_widget(_gui_bitmap_widget, _matchmaking_searching_string_search_preference);
    if (h != 0) h->set_visible(bitmap_visible);
}

static __forceinline bool gui_screen_matchmaking_searching_set_tip(long index)
{
    e_campaign_difficulty_level tip = (e_campaign_difficulty_level)index;
    bool result = false;
    c_network_session *session = 0;
    if (gui_screen_matchmaking_searching_get_squad(&session))
        result = ((c_network_session_parameters *)session->m_session_parameters)->m_parameters.matchmaking_tip.set((long *)&tip);
    return result;
}

__forceinline long gui_screen_matchmaking_searching_tip_downloader::get_data(const s_matchmaking_tips **out)
{
    long size = 0;
    const char *buffer = 0;
    long status = downloader.get_data(&buffer, &size);
    if (status == 2 && buffer != 0) {
        if (last_retry_count != downloader.m_retry_count)
            data = (const s_matchmaking_tips *)network_blf_find_chunk_in_file(buffer, size, 0x6d6d7470, 1, _blf_file_authentication_none, &data_size);
    } else {
        data = 0;
        data_size = 0;
    }
    *out = data;
    return status;
}

// @address 0x4dab48 ?update_tips@c_gui_screen_matchmaking_searching@@AAAXXZ
void c_gui_screen_matchmaking_searching::update_tips()
{
    s_network_http_buffer_url url;
    url.name[0] = 0;
    url.service_type = 0;
    url.cache_mode = 0;
    url.lifetime_seconds = 600;
    url.queue_type = 0;
    online_url_set_base_path((c_online_url_string *)&url);
    long language = g_current_language;
    if (language == -1) {
        language = (long)get_e_language_from_xbox_language(XTLGetLanguage());
        g_current_language = (e_language)language;
    }
    const char *suffix = get_language_suffix((e_language)language, false);
    ((c_static_string<256> *)url.name)->append_print("%s/matchmaking_tips.bin", suffix);
    url.cache_mode = 2;
    url.queue_type = 1;
    http_buffer_downloader_set_url(&m_tip_downloader.downloader, &url);
    if (network_life_cycle_is_squad_host()) {
        long interval = get_constants_datasource_integer(_matchmaking_searching_string_tip_cycle_time);
        if (GetTickCount() > m_tip_time + interval * 1000) {
            long index;
            for (;;) {
                random_seed_next(get_local_random_seed_address());
                unsigned short random = (unsigned short)(*get_local_random_seed_address() >> 16);
                // match: 0x4dab48: retain the unsigned word range for branch lowering.
                __assume(random < 0x10000);
                long candidate = random >> 11;
                if (candidate != m_tip_index) {
                    index = candidate;
                    break;
                }
            }
            if (gui_screen_matchmaking_searching_set_tip(index)) {
                m_tip_index = index;
                m_tip_time = GetTickCount();
            }
        }
    }
    const s_matchmaking_tips *tips;
    long status = m_tip_downloader.get_data(&tips);
    if (status == 2 && tips != 0 && network_blf_payload_long(tips->count) > 0) {
        long tip_count = network_blf_payload_long(tips->count);
        long tip_index = 0;
        c_network_session *session = 0;
        if (gui_screen_matchmaking_searching_get_squad(&session)) {
            c_network_session_parameters::s_network_session_parameters_internal *parameters =
                &((c_network_session_parameters *)session->m_session_parameters)->m_parameters;
            if (parameters->matchmaking_tip.get_allowed())
                tip_index = *parameters->matchmaking_tip.get();
        }
        c_static_wchar_string<1024> text;
        ::utf8_string_to_wchar_string((utf8 const *)tips->tips[tip_index % tip_count], (wchar_t *)((unsigned short *)text.m_string), 1024, (long *)(0));
        gui_screen_widget_set_text_widget_text(this, _matchmaking_searching_string_tips, text.m_string);
    }
}

// @address 0x4da550 ?update@c_gui_screen_matchmaking_searching@@UAAXK@Z

void c_gui_screen_matchmaking_searching::update(unsigned long time)
{
    long state;
    c_gui_widget *roster = this->get_child_widget(_gui_list_widget, _matchmaking_searching_string_roster);
    c_gui_widget *bitmap = this->get_child_widget(_gui_bitmap_widget, _matchmaking_searching_string_search_stage);
    const s_hopper_statistics *statistics = get_hopper_statistics();
    long population = statistics && statistics->population > 0 ? statistics->population : -1;
    s_life_cycle_matchmaking_progress progress;
    network_life_cycle_get_matchmaking_progress(&progress);
    g_user_interface_matchmaking_progress = progress;
    // match: 0x4da550: retain the qualified load without reinterpreting the progress field.
    state = static_cast<volatile long &>(progress.type);
    if (state == 1 || state == 2 || state == 3)
        m_matchmaking_progress = progress;
    if (roster != 0)
        ((c_gui_roster_list_widget *)roster)->m_show_party_indicators = true;
    update_status();
    update_tips();
    update_ping_pong();
    update_progress(time);
    user_interface_matchmaking_update_hopper_statistics_for_screen(
        &m_statistics_downloader,
        &m_hopper_identifier,
        (s_matchmaking_single_hopper_statistics *)&m_hopper_statistics);
    bool show_progress = m_matchmaking_progress.type == 3;
    c_gui_widget *progress_bar = this->get_child_widget(_gui_group_widget, _matchmaking_searching_string_spinner);
    if (progress_bar != 0)
        progress_bar->set_visible(show_progress);
    bool party_bar_visible;
    if (user_interface_squad_get_player_count() > 1)
        party_bar_visible = true;
    else
        party_bar_visible = false;
    c_gui_widget *party_bar = this->get_child_widget(_gui_text_widget, _matchmaking_searching_string_party_leader);
    if (party_bar != 0)
        party_bar->set_visible(party_bar_visible);
    bool lobby_bar_visible;
    if (user_interface_squad_get_player_count() > 1)
        lobby_bar_visible = true;
    else
        lobby_bar_visible = false;
    c_gui_widget *lobby_bar = this->get_child_widget(_gui_bitmap_widget, _matchmaking_searching_string_party_leader_underline);
    if (lobby_bar != 0)
        lobby_bar->set_visible(lobby_bar_visible);
    long population_label;
    if (population == -1)
        population_label = _matchmaking_searching_string_hopper_name;
    else
        population_label = _matchmaking_searching_string_hopper_with_population;
    set_text_widget_string_id(_matchmaking_searching_string_hopper_name, population_label);
    if (bitmap != 0)
        ((c_gui_bitmap_widget *)bitmap)->set_sprite_frame(m_matchmaking_progress.type);
    const c_game_results *results = gui_screen_matchmaking_searching_final_results();
    c_gui_widget *result_bar = this->get_child_widget(_gui_text_widget, _matchmaking_searching_string_postgame_stats);
    if (result_bar != 0)
        result_bar->set_visible(results != 0);
    long group;
    if (state == 1 || state == 2) {
        group = gui_screen_matchmaking_searching_final_results() ? _matchmaking_searching_string_search_online_stats
            : _matchmaking_searching_string_search_online;
    } else if (state == 3) {
        group = _matchmaking_searching_string_search_setting_up_match;
    } else {
        c_gui_screen_widget::update(time);
        return;
    }
    gui_screen_widget_set_display_group(this, 1, group);
    c_gui_screen_widget::update(time);
}

void c_gui_screen_matchmaking_searching::initialize()
{
    c_gui_screen_widget::initialize();
    add_game_tag_parser(new(0, 0, _ui_allocation_marker_dummy) c_magic_string_game_tag_parser(L"<their-skill-minimum", this, parse_xml_their_skill_minimum));
    add_game_tag_parser(new(0, 0, _ui_allocation_marker_dummy) c_magic_string_game_tag_parser(L"<their-skill-range", this, parse_xml_their_skill_range));
    add_game_tag_parser(new(0, 0, _ui_allocation_marker_dummy) c_magic_string_game_tag_parser(L"<their-skill-dynamic", this, parse_xml_their_skill_dynamic));
    add_game_tag_parser(new(0, 0, _ui_allocation_marker_dummy) c_magic_string_game_tag_parser(L"<their-size-minimum", this, parse_xml_their_size_minimum));
    add_game_tag_parser(new(0, 0, _ui_allocation_marker_dummy) c_magic_string_game_tag_parser(L"<their-size-range", this, parse_xml_their_size_range));
    add_game_tag_parser(new(0, 0, _ui_allocation_marker_dummy) c_magic_string_game_tag_parser(L"<their-size-dynamic", this, parse_xml_their_size_dynamic));
    add_game_tag_parser(new(0, 0, _ui_allocation_marker_dummy) c_magic_string_game_tag_parser(L"<hopper-name", this, parse_xml_hopper_name));
    add_game_tag_parser(new (0, 0, _ui_allocation_marker_dummy)
            c_magic_string_game_tag_parser(L"<hopper-population", this, parse_xml_hopper_population));
    add_game_tag_parser(new(0, 0, _ui_allocation_marker_dummy) c_magic_string_game_tag_parser(L"<current-players", this, parse_xml_current_players));
    add_game_tag_parser(new(0, 0, _ui_allocation_marker_dummy) c_magic_string_game_tag_parser(L"<games-found", this, parse_xml_games_found));
    add_game_tag_parser(new(0, 0, _ui_allocation_marker_dummy) c_magic_string_game_tag_parser(L"<possible-matches", this, parse_xml_possible_matches));
    add_game_tag_parser(new(0, 0, _ui_allocation_marker_dummy) c_magic_string_game_tag_parser(
        L"<need-more-player-count", this, parse_xml_need_more_player_count));
    add_game_tag_parser(new(0, 0, _ui_allocation_marker_dummy) c_magic_string_game_tag_parser(
        L"<want-more-player-count", this, parse_xml_want_more_player_count));
    add_game_tag_parser(new(0, 0, _ui_allocation_marker_dummy) c_magic_string_game_tag_parser(L"<give-up-timer-seconds", this, parse_xml_give_up_timer));
    add_game_tag_parser(new (0, 0, _ui_allocation_marker_dummy)
            c_magic_string_game_tag_parser(L"<search-preference", this, parse_xml_search_preference));
    add_game_tag_parser(new (0, 0, _ui_allocation_marker_dummy)
            c_magic_string_game_tag_parser(L"<party-leader", this, parse_xml_party_leader));
}

void c_gui_screen_matchmaking_searching::post_initialize()
{
    c_gui_screen_widget::post_initialize();
    c_gui_widget *text_widget = get_child_widget(_gui_text_widget, _matchmaking_searching_string_tips);
    if (text_widget != 0) {
        static_cast<c_gui_text_widget *>(text_widget)->set_text(L" ");
    }
}

// @address 0x4da478 ?initialize_datasource@c_gui_screen_matchmaking_searching@@UAAXXZ

void c_gui_screen_matchmaking_searching::initialize_datasource()
{
    c_gui_screen_widget::initialize_datasource();
    void *memory = ::user_interface_malloc_tracked(sizeof(c_gui_squad_roster_data), (char const *)(0), 0);
    c_gui_data *data = memory != 0 ? new(memory) c_gui_squad_roster_data(get_driving_controller()) : 0;
    if (data != 0) {
        if (data->initialize(_matchmaking_searching_string_roster))
            add_datasource(data);
        else
            gui_screen_matchmaking_searching_delete_data(data);
    }
}

void c_gui_screen_matchmaking_searching::update_ping_pong()
{
    if (m_matchmaking_progress.type != 0) {
        if (m_matchmaking_progress.type != m_previous_state) {
            m_ping_previous = m_ping_current;
            m_ping_pending = 0;
            m_pong_previous = m_matchmaking_progress.search_statistics.pong_current;
            m_pong_pending = 0;
            m_previous_state = m_matchmaking_progress.type;
        }
        {
            bool visible = m_matchmaking_progress.type == 1;
            c_gui_widget *child = get_child_widget(_gui_group_widget, _matchmaking_searching_string_ping);
            if (child)
                child->set_visible(visible);
        }
        {
            bool visible = m_matchmaking_progress.type == 2;
            c_gui_widget *child = get_child_widget(_gui_group_widget, _matchmaking_searching_string_pong);
            if (child)
                child->set_visible(visible);
        }
        if (m_matchmaking_progress.type == 1) {
            long current = m_ping_current;
            if (current > m_ping_previous)
                m_ping_pending += current - m_ping_previous;
            else if (current < m_ping_previous)
                m_ping_pending += 63 - m_ping_previous + current;
            m_ping_previous = current;
            if (m_ping_pending > 0) {
                c_gui_widget *group = this->get_child_widget(_gui_group_widget, _matchmaking_searching_string_ping);
                c_gui_widget *bitmap = group ? group->get_child_widget(_gui_bitmap_widget, _matchmaking_searching_string_ping) : 0;
                if (bitmap && !bitmap->is_animation_active((e_animation_state)k_matchmaking_progress_animation)) {
                    for (c_gui_widget *child = group->get_children(); child; child = child->get_next()) {
                        if (child->m_name == _matchmaking_searching_string_ping)
                            child->start_animation_at_time((e_animation_state)k_matchmaking_progress_animation, -1, true);
                    }
                    --m_ping_pending;
                }
            }
        } else if (m_matchmaking_progress.type == 2) {
            long current = m_matchmaking_progress.search_statistics.pong_current;
            if (current > m_pong_previous)
                m_pong_pending += current - m_pong_previous;
            else if (current < m_pong_previous) {
                long delta = 255 - m_pong_previous + current;
                m_pong_pending += delta < 5 ? delta : 5;
            }
            m_pong_previous = current;
            if (m_pong_pending > 0) {
                c_gui_widget *group = this->get_child_widget(_gui_group_widget, _matchmaking_searching_string_pong);
                c_gui_widget *bitmap = group ? group->get_child_widget(_gui_bitmap_widget, _matchmaking_searching_string_pong) : 0;
                if (group && !bitmap->is_animation_active((e_animation_state)k_matchmaking_progress_animation)) {
                    for (c_gui_widget *child = group->get_children(); child; child = child->get_next()) {
                        if (child->m_name == _matchmaking_searching_string_pong)
                            child->start_animation_at_time((e_animation_state)k_matchmaking_progress_animation, -1, true);
                    }
                    --m_pong_pending;
                }
            }
        }
    }
}

void c_gui_screen_matchmaking_searching::update_progress(unsigned long time)
{
    c_gui_widget *bar = this->get_child_widget(_gui_group_widget, _matchmaking_searching_string_progress);
    if (bar && m_matchmaking_progress.type != 0) {
        if (m_matchmaking_progress.type == 2) {
            long total = m_matchmaking_progress.search_statistics.progress_total;
            long current = m_matchmaking_progress.give_up_time;
            if (total == 0) {
                current = 0;
                total = 1;
            }
            float progress = (float)current / (float)total;
            m_progress = progress;
            bar->start_animation_at_time((e_animation_state)k_matchmaking_progress_animation, (unsigned long)((float)time - (1.0f - progress) * 999.0f), true);
            bar->set_visible(true);
            return;
        }
        bar->set_visible(false);
    }
}

// @address 0x4a8e80 ?dispose@c_gui_screen_matchmaking_searching@@UAAXXZ
void c_gui_screen_matchmaking_searching::dispose()
{
    c_gui_screen_widget::dispose();
}

// @address 0x4cbb48 ?create_list_widget@c_gui_screen_matchmaking_searching@@UAAPAVc_gui_list_widget@@PBUs_list_widget_block@@@Z
c_gui_list_widget *c_gui_screen_matchmaking_searching::create_list_widget(const s_list_widget_block *definition)
{
    if (definition->definition.core.name == _matchmaking_searching_string_roster) {
        void *memory = user_interface_malloc_tracked(sizeof(c_gui_roster_list_widget), 0, 0);
        if (memory)
            return new (memory) c_gui_roster_list_widget(true, false);
    } else {
        void *memory = user_interface_malloc_tracked(sizeof(c_gui_list_widget), 0, 0);
        if (memory)
            return new (memory) c_gui_list_widget;
    }
    return 0;
}

// @address 0x499198 ?get_render_data_size@c_gui_screen_matchmaking_searching@@MAA?AW4e_render_data_size@@XZ
e_render_data_size c_gui_screen_matchmaking_searching::get_render_data_size()
{
    return k_render_data_size_large;
}

bool c_gui_screen_matchmaking_searching::handle_dialog_result(const c_dialog_result_message *message)
{
    bool dialog_handled = false;
    switch (message->get_dialog_name()) {
    case k_leave_searching_squad_dialog:
        if (message->get_dialog_result() == 0)
            user_interface_leave_squad(dialog_handled);
        dialog_handled = true;
        break;
    case k_abort_matchmaking_dialog:
        if (message->get_dialog_result() == 0)
            network_life_cycle_abort_matchmaking();
        dialog_handled = true;
        break;
    }
    if (!dialog_handled) {
        dialog_handled = false;
        if (message->get_dialog_name() == _user_interface_utilities_string_need_to_select_storage_device_to_save) {
            if (message->get_dialog_result() == 0)
                content_catalogue_display_device_selection_guide_interface(message->get_controller());
            dialog_handled = true;
        }
    }
    return dialog_handled;
}

// @address 0x4db438 ?handle_list_item_chosen@c_gui_screen_matchmaking_searching@@UAA_NPBVc_controller_input_message@@JPAVc_gui_list_item_widget@@PAVc_gui_data@@@Z
bool c_gui_screen_matchmaking_searching::handle_list_item_chosen(
    const c_controller_input_message *message, long list_name, c_gui_list_item_widget *list_item, c_gui_data *data)
{
    bool handled = false;
    if (list_name == _matchmaking_searching_string_roster) {
        long selected = -1;
        if (data->get_integer_value(list_item->get_element_handle(), _roster_data_column_player_index, &selected)) {
            e_controller_index controller_index = message->get_controller();
            long player_index = ::user_interface_session_get_player_at_local_controller(controller_index);
            const s_player_identifier *local_player = user_interface_session_is_local_player(player_index);
            const s_player_identifier *selected_player = user_interface_session_is_local_player(selected);
            if (local_player != 0 && selected_player != 0) {
                c_load_player_select_screen_message *load_message = (c_load_player_select_screen_message *)::user_interface_malloc_tracked(
                    sizeof(c_load_player_select_screen_message), (char const *)(0), 0);
                if (load_message != 0) {
                    long layer = m_name;
                    long controller = message->get_controller();
                    c_load_player_select_screen_message *posted = new (load_message) c_load_player_select_screen_message(
                        _matchmaking_searching_string_player_select, (e_controller_index)controller,
                        get_render_window(), layer, local_player, selected_player, true);
                    if (posted != 0) {
                        posted->set_parent_screen_index(m_screen_index);
                        ::user_interface_messaging_post(posted);
                    }
                }
            }
            else {
                ::user_interface_play_sound(_ui_sound_error, m_core_definition.sound_override_index);
            }
        }
        handled = true;
    }
    if ((unsigned char)handled == 0)
        handled = false;
    return handled;
}

// @address 0x4db578 ?parse_xml_matchmaking_searching_callback@@YA_NW4e_parse_xml_stat@@PAXPA_WJ@Z
bool parse_xml_matchmaking_searching_callback(e_parse_xml_stat parse_xml_stat, void *context, wchar_t *buffer, long buffer_length)
{
    c_gui_screen_matchmaking_searching *screen = (c_gui_screen_matchmaking_searching *)context;
    bool success = false;
    if (screen) {
        c_static_wchar_string<1024> text;
        const s_life_cycle_matchmaking_progress *progress = screen->get_current_matchmaking_progress();
        const s_matchmaking_search_criteria *criteria = 0;
        if (progress && (progress->type == 1 || progress->type == 2 || progress->type == 3))
            criteria = &progress->search_criteria;
        switch (parse_xml_stat) {
        case _parse_xml_their_skill_minimum:
            if (criteria)
                text.print(game_engine_number_format(), criteria->skill_minimum);
            break;
        case _parse_xml_their_skill_range:
            if (criteria)
                text.print(L"%d-%d", criteria->skill_minimum, criteria->skill_maximum);
            break;
        case _parse_xml_their_skill_dynamic:
            if (criteria) {
                long total = criteria->skill_maximum;
                long current = criteria->skill_minimum;
                if (current == total)
                    text.print(game_engine_number_format(), current);
                else
                    text.print(L"%d-%d", current, total);
            }
            break;
        case _parse_xml_their_size_minimum:
            if (criteria)
                text.print(game_engine_number_format(), criteria->size_minimum);
            break;
        case _parse_xml_their_size_range:
            if (criteria)
                text.print(L"%d-%d", criteria->size_minimum, criteria->size_maximum);
            break;
        case _parse_xml_their_size_dynamic:
            if (criteria) {
                long total = criteria->size_maximum;
                long current = criteria->size_minimum;
                if (current == total)
                    text.print(game_engine_number_format(), current);
                else
                    text.print(L"%d-%d", current, total);
            }
            break;
        case _parse_xml_hopper_name:
            if (progress)
                text.set(progress->name);
            break;
        case _parse_xml_hopper_population:
            if (progress) {
                long population;
                const s_hopper_statistics *statistics = screen->get_hopper_statistics();
                if (statistics && statistics->population >= 0)
                    population = statistics->population;
                else
                    population = 0;
                text.print(game_engine_number_format(), population);
            }
            break;
        case _parse_xml_current_players:
            if (progress)
                text.print(game_engine_number_format(), progress->group_player_count);
            break;
        case _parse_xml_games_found:
            if (progress->type == 1)
                text.print(game_engine_number_format(), progress->search_statistics.games_found);
            break;
        case _parse_xml_possible_matches:
            if (progress->type == 1)
                text.print(game_engine_number_format(), progress->search_statistics.possible_matches);
            break;
        case _parse_xml_need_players:
            if (progress->type == 2)
                text.print(game_engine_number_format(), MAX(0, progress->search_criteria.need_player_count - progress->group_player_count));
            break;
        case _parse_xml_want_players:
            if (progress->type == 2)
                text.print(game_engine_number_format(), MAX(0, progress->search_criteria.want_player_count - progress->group_player_count));
            break;
        case _parse_xml_give_up_time:
            if (progress->type == 2)
                user_interface_get_timespan_string(progress->give_up_time, 2, false, &text);
            break;
        case _parse_xml_search_preference:
            if (progress->search_preference != 1) {
                if (progress->search_preference != 2) {
                    if (progress->search_preference == 3)
                        screen->get_string_by_string_id(_matchmaking_searching_string_search_preference_language, &text);
                } else {
                    screen->get_string_by_string_id(_matchmaking_searching_string_search_preference_connection, &text);
                }
            } else {
                screen->get_string_by_string_id(_matchmaking_searching_string_search_preference_skill, &text);
            }
            break;
        case _parse_xml_party_leader:
            if (user_interface_squad_exists()) {
                long leader_player_index = ::user_interface_squad_get_leader_player_index();
                if (leader_player_index != -1) {
                    const s_player_configuration *leader = ::user_interface_squad_get_player_data(leader_player_index);
                    if (leader) {
                        ustrnzcpy(buffer, leader->host.name, buffer_length);
                        success = true;
                    }
                }
            }
            break;
        default:
            // match: 0x4db578: unreachable hint omits dead branch check
            __assume(0);
        }
        if (text.length() > 0) {
            text.copy_to(buffer, buffer_length);
            return true;
        }
    }
    return success;
}

// @address 0x4d9498 ?parse_xml_their_skill_minimum@@YA_NPAXPA_WJ@Z
bool parse_xml_their_skill_minimum(void *context, wchar_t *buffer, long buffer_length)
{
    return parse_xml_matchmaking_searching_callback(_parse_xml_their_skill_minimum, context, buffer, buffer_length);
}

class c_game_results;

// @address 0x4db278 ?handle_controller_input_message@c_gui_screen_matchmaking_searching@@UAA_NPBVc_controller_input_message@@@Z
bool c_gui_screen_matchmaking_searching::handle_controller_input_message(const c_controller_input_message *message)
{
    bool handled = false;
    s_life_cycle_matchmaking_progress progress;
    network_life_cycle_get_matchmaking_progress(&progress);
    g_user_interface_matchmaking_progress = progress;
    if (message->get_event_type() == 13) {
        switch (message->get_component()) {
        case 1:
            if (progress.type == 1 || progress.type == 2) {
                long dialog = ::network_life_cycle_squad_local_peer_is_leader() ? k_abort_matchmaking_dialog : k_leave_searching_squad_dialog;
                c_load_dialog_screen_message *load = (c_load_dialog_screen_message *)::user_interface_malloc_tracked(
                    sizeof(c_load_dialog_screen_message), (char const *)(0), 0);
                if (load) {
                    long layer = m_name;
                    e_controller_index controller = message->get_controller();
                    c_load_dialog_screen_message *posted = new (load) c_load_dialog_screen_message(controller, get_render_window(), layer, dialog, layer);
                    if (posted) {
                        posted->set_parent_screen_index(m_screen_index);
                        ::user_interface_messaging_post(posted);
                    }
                }
                handled = true;
            }
            break;
        case 3:
            if ((progress.type == 1 || progress.type == 2) && online_local_xuid_is_online_enabled())
                online_guide_show_friends_ui(message->get_controller());
            break;
        case k_scoreboard_controller_component:
            if ((progress.type == 1 || progress.type == 2) && user_interface_networking_get_final_game_results()) {
                c_load_screen_message *load = (c_load_screen_message *)::user_interface_malloc_tracked(sizeof(c_load_screen_message), (char const *)(0), 0);
                if (load) {
                    long layer = m_name;
                    c_load_screen_message *posted = new (load) c_load_screen_message(_matchmaking_searching_string_carnage_report,
                        k_any_controller, get_render_window(), layer);
                    if (posted)
                        ::user_interface_messaging_post(posted);
                }
                handled = true;
            }
            break;
        }
    }
    if (!handled)
        handled = c_gui_screen_widget::handle_controller_input_message(message);
    return handled;
}

STATIC_ASSERT_ABI32(offsetof(c_gui_screen_matchmaking_searching, m_hopper_statistics) == 0x5b0c, searching_hopper_statistics_offset);
STATIC_ASSERT_ABI32(sizeof(c_gui_screen_matchmaking_searching) == 0x5b30, searching_size);
