// interfacelibxbox/gui_screen_carnage_report.obj: from the records 0x2477e0 0x4d6c50 0x4d71d8 0x4d7588 0x4d7b28 0x4d7ba0 0x4d7ee8 0x4d7f58 0x4d8220 0x4d8460 0x4d87e8 0x4d8898 0x4d8920 0x4d89d8 0x4d8ac0 0x4d8b78 0x4d8c10 0x4d8d38 0x4f4a08 0x4f4b48 0x4f4d68 0x4f4ed8 0x4f53c8 0x4f5750 0x4f5800 0x4f5898 0x4f58a0 0x4f58f8 0x4f5a70 0x4f5b00 0x4f5c20 0x4f5cc0 0x4f5cd0 0x4f5d28 0x4f5f10 0x4f5f50 0x4f6088 0x4f6140 0x4f61e8 0x4f61f8 0x4f6400 0x4f6800 0x50ac68
#include "interface/gui_render_data_size_types.h"
#include "ai/ai_math_constants.h"
#include "memory/tls_globals.h"
#include "interface/user_interface_matchmaking_progress.h"
#include "game/game_results_storage_types.h"
// @flags /TP /O2 /Oc
#include "game/campaign_metagame_globals.h"
#include "game/game_engine_display.h"
#include "interface/user_interface_time.h"
#include "game/game_results_medal_info.h"
#include "interface/gui_screen_widget_text.h"
#include "interface/gui_screens/player_select/gui_player_select_screen_widget.h"
#include "cseries/long_sort.h"
#include "memory/tls_data_arrays.h"
#include "scenario/scenario.h"
#include "math/real_math.h"
#include "math/real_math_types.h"
#include "interface/custom_map_image_types.h"
#include <stddef.h>
#include "memory/data_types.h"
#include "memory/datum_index.h"
#include "main/levels_globals.h"
#include "tag_files/tag_types.h"
#include "interface/gui_widget_types.h"
#include "memory/static_arrays.h"
#include "interface/user_interface_messages.h"
#include "game/game_results_team_statistics_types.h"
#include "game/game_results_team_data_types.h"
// @address 0x4d87e8 ?get_current_page_name@c_gui_screen_carnage_report@@QAAJXZ
#include <stdlib.h>
#include <string.h>
#include "interface/gui_screens/scoreboard/gui_screen_scoreboard.h"
#include "interface/gui_screens/scoreboard/gui_screen_scoreboard_types.h"
#include "interface/user_interface.h"
#include "interface/user_interface_memory.h"
#include "interface/user_interface_session.h"
#include "networking/logic/network_session_interface.h"
#include "text/unicode.h"
#include "ai/ai_reference_frame.h"
#include "interface/user_interface_networking.h"
#include "interface/user_interface_utilities.h"
#include "networking/logic/network_life_cycle.h"
#include "ai/ai_script.h"
#include "interface/user_interface_session_dependencies.h"
#include "game/game_engine.h"
#include "game/game_results.h"
#include "main/levels.h"
#include "game/campaign_metagame.h"
#include "interface/gui_screens/carnage_report/gui_screen_carnage_report.h"
#include "tag_files/string_ids.h"
#include "memory/data.h"
#include "game/player_types.h"
#include "interface/game_tag_parser.h"
#include "interface/c_gui_bitmap_widget.h"
#include "scenario/scenario_tag.h"
#include "interface/user_interface_sound_effect_types.h"
#include "interface/list_widget_definition.h"
#include "interface/gui_model_widget_types.h"
#include "interface/gui_animation_transform.h"
#include "interface/core_widget_definition.h"
#include "interface/c_gui_text_widget.h"
#include "interface/c_gui_group_widget.h"
#include "game/player_identifier_types.h"
#include "ai/ai_point3d.h"
#include "game/game_types.h"
#include "interface/user_interface_shared_globals.h"
#include "game/game_results_damage_statistic_types.h"
#include "game/game_results_damage_type_description_types.h"
#include "saved_games/saved_game_files_context_types.h"
#include "game/game_results_player_vs_player_statistic_types.h"
#include "game/game_results_damage_breakdown_types.h"
#include "game/game_results_statistic_shared_types.h"
#include "interface/ui_allocation_marker_types.h"
#include "networking/online/online_guide_xenon.h"
#include "interface/user_interface_allocation.h"
#include "game/game_results_statistics_types.h"
#include "game/game_options.h"
#include "ai/ai_script_dialogue.h"
#include "interface/gui_screens/player_select/player_select_screen_message.h"
#include "interface/gui_custom_bitmap_widget.h"
#include <new>

#include "interface/gui_screen_carnage_report_externs.h"
class c_gui_data;
class c_game_results;
class c_gui_selected_item;

#include "interface/gui_data.h"
struct s_group_widget_definition;
struct s_button_key_definition;
struct s_model_widget_block;
class c_font_cache;
class c_gui_button_key_widget;
#include "interface/gui_widget.h"
#include "interface/c_gui_list_widget.h"
#include "interface/c_gui_screen_widget.h"


#include "interface/c_gui_list_item_widget.h"







#include "game/game_results_types.h"
typedef char carnage_player_data_size[(sizeof(s_game_results_player_data) == 0x130) ? 1 : -1];
typedef char carnage_player_score_offset[(offsetof(s_game_results_player_data, final_score) == 0x12c) ? 1 : -1];

#include "networking/logic/life_cycle/life_cycle_matchmaking_progress.h"


#include "saved_games/saved_game_files_globals.h"

#include "game/game_globals_storage.h"



class c_gui_campaign_metagame_data : public c_gui_ordered_data {
public:
    c_gui_campaign_metagame_data()
    {
        for (short i = 0; i < 4; i++)
            player_indices[i] = -1;
    }
    virtual bool initialize(long name);
    virtual void get_column_names(long *const column_names, long *column_count);
    virtual bool get_integer_value(long element_index, long value_name, long *value);
    virtual bool get_text_value(long element_index, long value_name, c_static_wchar_string<1024> *value);

    long player_indices[4];
    short player_count;

protected:
    virtual long get_current_item_count_internal();
};




// Picks the nearest points of the scenario point set, plays mission dialogue there,
// and returns the longest duration in ticks.



#include "ai/cs_scenario_definitions.h"
struct carnage_report_sort_entry { short index; short priority; };

static __forceinline bool carnage_results_finalized()
{
    const c_game_results *results = &g_current_game_results;
    return results->initialized && results->finalized;
}
static __forceinline const c_game_results *carnage_results_final()
{
    const c_game_results *results = 0;
    if (carnage_results_finalized())
        results = &g_current_game_results;
    return results;
}
static __forceinline const c_game_results *carnage_final_game_results()
{
    const c_game_results *result = 0;
    const c_game_results *final_results = carnage_results_final();
    if (final_results && final_results->finish_valid)
        result = final_results;
    return result;
}
extern "C" {
// @address 0x7ea8e4 g_game_results_sort_results
}
// @address 0x2477e0 carnage_report_compare_teams_by_place
int carnage_report_compare_teams_by_place(const void *a, const void *b)
{
    long team_index_a = *(const long *)a;
    long team_index_b = *(const long *)b;
    const s_game_results_team_data *team_a = &g_game_results_sort_results->teams.team[team_index_a];
    const s_game_results_team_data *team_b = &g_game_results_sort_results->teams.team[team_index_b];
    int result = 0;
    if (team_a->standing < team_b->standing)
        result = -1;
    else if (team_a->standing > team_b->standing)
        result = 1;
    return result;
}

typedef bool (*t_magic_string_callback)(void *context, wchar_t *output, long capacity);
extern "C" {









}

// @address 0x4d6c50 ?initialize@c_gui_screen_carnage_report@@UAAXXZ
void c_gui_screen_carnage_report::initialize()
{
    c_gui_screen_widget::initialize();
    add_game_tag_parser(new (0, 0, _ui_allocation_marker_dummy)
            c_magic_string_game_tag_parser(L"<current-page-name", this, parse_xml_current_page_name));
    add_game_tag_parser(new(0, 0, _ui_allocation_marker_dummy) c_magic_string_game_tag_parser(L"<rematch-timer", this, parse_xml_rematch_timer));
    add_game_tag_parser(new (0, 0, _ui_allocation_marker_dummy)
            c_magic_string_game_tag_parser(L"<game-variant", this, parse_xml_game_variant));
    add_game_tag_parser(new (0, 0, _ui_allocation_marker_dummy)
            c_magic_string_game_tag_parser(L"<map-name", this, gui_screen_carnage_report_parse_map_name));
    add_game_tag_parser(new(0, 0, _ui_allocation_marker_dummy) c_magic_string_game_tag_parser(L"<mvp", this, parse_xml_player_name));
    add_game_tag_parser(new(0, 0, _ui_allocation_marker_dummy) c_magic_string_game_tag_parser(L"<save-film-reminder", this, parse_xml_match_outcome));
    add_game_tag_parser(new (0, 0, _ui_allocation_marker_dummy)
            c_magic_string_game_tag_parser(L"<lobby-campaign-level", this, levels_get_campaign_map_display_name));
    add_game_tag_parser(new (0, 0, _ui_allocation_marker_dummy)
            c_magic_string_game_tag_parser(L"<lobby-campaign-difficulty", this, gui_screen_carnage_report_parse_campaign_difficulty));
    add_game_tag_parser(new(0, 0, _ui_allocation_marker_dummy) c_magic_string_game_tag_parser(L"<time-completed", this, parse_xml_campaign_time));
    add_game_tag_parser(new (0, 0, _ui_allocation_marker_dummy) c_magic_string_game_tag_parser(
        L"<difficulty-complete", this, gui_screen_carnage_report_parse_campaign_difficulty));
    add_game_tag_parser(new (0, 0, _ui_allocation_marker_dummy)
            c_magic_string_game_tag_parser(L"<number-of-skulls", this, parse_xml_campaign_skull_count));
    add_game_tag_parser(new (0, 0, _ui_allocation_marker_dummy)
            c_magic_string_game_tag_parser(L"<difficulty-multiplier-value", this, parse_xml_campaign_player_scale));
    add_game_tag_parser(new (0, 0, _ui_allocation_marker_dummy)
            c_magic_string_game_tag_parser(L"<skulls-multiplier-value", this, parse_xml_campaign_skull_bonus));
    add_game_tag_parser(new (0, 0, _ui_allocation_marker_dummy)
            c_magic_string_game_tag_parser(L"<subtotal-value", this, parse_xml_campaign_difficulty_scale));
    add_game_tag_parser(new (0, 0, _ui_allocation_marker_dummy)
            c_magic_string_game_tag_parser(L"<time-bonus-value", this, parse_xml_campaign_time_multiplier));
    add_game_tag_parser(new (0, 0, _ui_allocation_marker_dummy)
            c_magic_string_game_tag_parser(L"<total-multipliers-value", this, parse_xml_campaign_total_multiplier));
    add_game_tag_parser(new (0, 0, _ui_allocation_marker_dummy)
            c_magic_string_game_tag_parser(L"<total-score-value", this, parse_xml_campaign_total_score));
}

extern "C" {
// @address 0x56e88 ??_7c_gui_game_results_data@@6B@
// @address 0x56f00 ??_7c_gui_game_results_team_data@@6B@
// @address 0x56f70 ??_7c_gui_top_medals_data@@6B@
// @address 0x56fe0 ??_7c_gui_campaign_metagame_data@@6B@
}

static __forceinline void carnage_ui_track_delete(c_gui_data *object)
{
    object->~c_gui_data();
    user_interface_free(object);
}

static __forceinline void carnage_add_or_delete(c_gui_screen_carnage_report *screen, c_gui_data *datasource, long name)
{
    if (datasource->initialize(name))
        screen->m_datasource[screen->m_datasource_count++] = datasource;
    else
        carnage_ui_track_delete(datasource);
}

// @address 0x4d71d8 ?initialize_datasource@c_gui_screen_carnage_report@@UAAXXZ
void c_gui_screen_carnage_report::initialize_datasource()
{
    c_gui_screen_widget::initialize_datasource();
    if (report_type == 1)
    {
        c_gui_data *game_results = new(0, 0, _ui_allocation_marker_dummy) c_gui_game_results_data();
        if (game_results)
            carnage_add_or_delete(this, game_results, _player_select_string_player_list);
        c_gui_data *team_results = new(0, 0, _ui_allocation_marker_dummy) c_gui_game_results_team_data();
        if (team_results)
            carnage_add_or_delete(this, team_results, _carnage_report_string_teams_list);
        c_gui_data *top_medals = new(0, 0, _ui_allocation_marker_dummy) c_gui_top_medals_data();
        if (top_medals)
            carnage_add_or_delete(this, top_medals, _carnage_report_string_top_medals);
    }
    else if (report_type == 2)
    {
        c_gui_data *subtitle = new(0, 0, _ui_allocation_marker_dummy) c_gui_campaign_metagame_data();
        if (subtitle)
            carnage_add_or_delete(this, subtitle, _player_select_string_player_list);
    }
    setup_pages(carnage_final_game_results());
    c_gui_data *data = find_data(_carnage_report_string_page, 0);
    if (data)
        current_value_name = data->get_first_element_handle();
}

// @address 0x4d7f58 ?setup_pages@c_gui_screen_carnage_report@@IAAXPBVc_game_results@@@Z
void c_gui_screen_carnage_report::setup_pages(const c_game_results *results)
{
    c_gui_data *pages = find_data(_carnage_report_string_page, 0);
    if (pages) {
        struct s_engine_page { long engine; long name; };
        s_engine_page engine_pages[9] = {
            {1, _carnage_report_string_page_objectives_ctf},
            {2, _carnage_report_string_page_objectives_slayer},
            {3, _carnage_report_string_page_objectives_oddball},
            {4, _carnage_report_string_page_objectives_koth},
            {6, _carnage_report_string_page_objectives_vip},
            {8, _carnage_report_string_page_objectives_territories},
            {9, _carnage_report_string_page_objectives_assault},
            {10, _carnage_report_string_page_objectives_infection},
            {7, _carnage_report_string_page_objectives_juggernaut}
        };
        long team_pages[4] = {
            _carnage_report_string_page_results, _carnage_report_string_page_carnage,
            _carnage_report_string_page_killbreakdown, _carnage_report_string_page_fieldstats
        };
        long player_pages[5] = {
            _carnage_report_string_campaign_results, _carnage_report_string_campaign_carnage,
            _carnage_report_string_campaign_enemies, _carnage_report_string_campaign_vehicles,
            _carnage_report_string_campaign_field_stats
        };
        if (report_type == 1) {
            long game_engine = 2;
            if (results)
                game_engine = results->game_variant.get_game_engine_index();
            pages->clear_disabled_elements();
            for (unsigned long index = 0; index < 9; ++index) {
                if (engine_pages[index].engine != game_engine)
                    pages->set_disabled_element(_string_id_name, engine_pages[index].name);
            }
            for (short index = 0; (unsigned short)index < 5; ++index)
                pages->set_disabled_element(_string_id_name, player_pages[index]);
        } else if (report_type == 2) {
            for (short index = 0; (unsigned short)index < 9; ++index)
                pages->set_disabled_element(_string_id_name, engine_pages[index].name);
            for (short index = 0; (unsigned short)index < 4; ++index)
                pages->set_disabled_element(_string_id_name, team_pages[index]);
        }
    }
}

long c_gui_screen_carnage_report::get_current_page_name()
{
    long result = -1;
    long data_count = this->m_datasource_count;
    long index = 0;
    if (data_count > 0) {
        do {
            c_gui_data *candidate = this->m_datasource[index];
            if (candidate->m_name == _carnage_report_string_page) {
                c_gui_data *data = this->m_datasource[index];
                if (data != 0) {
                    long value;
                    if (data->get_string_id_value(this->current_value_name, _string_id_name, &value))
                        result = value;
                }
                break;
            }
            index++;
        } while (index < data_count);
    }
    return result;
}

// @address 0x4d8898 ?parse_xml_current_page_name@@YA_NPAXPA_WJ@Z
bool parse_xml_current_page_name(void *context, wchar_t *buffer, long buffer_length)
{
    c_gui_screen_carnage_report *screen = (c_gui_screen_carnage_report *)context;
    bool result = false;
    c_static_wchar_string<1024> string;
    if (screen->get_string_by_string_id(screen->get_current_page_name(), &string)) {
        string.copy_to(buffer, buffer_length);
        result = true;
    }
    return result;
}


// @address 0x4d8920 ?parse_xml_rematch_timer@@YA_NPAXPA_WJ@Z
bool parse_xml_rematch_timer(void *context, wchar_t *buffer, long buffer_length)
{
    bool result = false;
    if (network_life_cycle_in_matchmaking()) {
        s_life_cycle_matchmaking_progress progress;
        network_life_cycle_get_matchmaking_progress(&progress);
        g_user_interface_matchmaking_progress = progress;
        if (progress.type == 5 && progress.countdown_started) {
            c_static_wchar_string<1024> string;
            long seconds = progress.countdown_timer - 1 > 0 ? progress.countdown_timer - 1 : 0;
            string.print(L"%02d", seconds);
            string.copy_to(buffer, buffer_length);
            result = true;
        }
    }
    return result;
}
// @address 0x4d89d8 ?parse_xml_game_variant@@YA_NPAXPA_WJ@Z
bool parse_xml_game_variant(void *context, wchar_t *output_buffer, long output_capacity)
{
    int game_engine_type;
    unsigned char results_finalized;
    const c_game_results *finalized_game_results;
    bool parse_succeeded;
    unsigned int copy_length;
    int game_variant[156];
    int multiplayer_string[514];
    finalized_game_results = 0;
    results_finalized = g_current_game_results.initialized && g_current_game_results.finalized;
    if (results_finalized) {
        if (g_current_game_results.finish_valid)
            finalized_game_results = &g_current_game_results;
    }
    parse_succeeded = false;
    if (finalized_game_results != 0) {
        game_variant[0] = 0;
        memset(&game_variant[1], 0, sizeof(game_variant) - 4 * sizeof(int));
        memset(&game_variant[1], 0, sizeof(game_variant) - 4 * sizeof(int));
        game_engine_type = finalized_game_results->game_variant.m_game_engine_index;
        *(unsigned short *)multiplayer_string = 0;
        game_engine_variant_initialize_default((c_game_variant *)game_variant, (e_game_engine_type)game_engine_type);
        typedef void ***t_dispatch_table;
#if defined(__clang__)
        // Slot 0 is get_game_engine_name_string_id, which takes the variant in ECX on x86.
        typedef long (__thiscall *t_get_game_engine_name_string_id)(int *);
#else
        typedef long (*t_get_game_engine_name_string_id)(int *);
#endif
        int *storage = &game_variant[1];
        void **functions = *(t_dispatch_table)storage;
        game_engine_get_multiplayer_string(((t_get_game_engine_name_string_id)functions[0])(storage),
            (c_static_wchar_string_1024 *)multiplayer_string);
        copy_length = output_capacity;
        if ((unsigned long)output_capacity > 0x400)
            copy_length = 0x400;
        ustrnzcpy(output_buffer, (const wchar_t *)multiplayer_string, copy_length);
        parse_succeeded = true;
    }
    return parse_succeeded;
}


// @address 0x4d8ac0 gui_screen_carnage_report_parse_map_name
extern "C" bool gui_screen_carnage_report_parse_map_name(void *context, wchar_t *buffer, long buffer_length)
{
    const c_game_results *results = carnage_final_game_results();
    bool result = false;
    if (results) {
        s_datum level;
        if (levels_try_and_get_datum(g_level_globals.multiplayer_levels, results->map_id, &level)) {
            ustrnzcpy(buffer, level.display_name, buffer_length);
            result = true;
        }
    }
    return result;
}

// @address 0x4d8b78 parse_xml_player_name
extern "C" bool parse_xml_player_name(void *context, wchar_t *buffer, long buffer_length)
{
    c_gui_screen_carnage_report *screen = (c_gui_screen_carnage_report *)context;
    bool result = false;
    const c_game_results *results = carnage_final_game_results();
    if (results && screen->player_index != -1 && results->players[screen->player_index].player_exists) {
        ustrnzcpy(buffer, results->players[screen->player_index].configuration.host.player_name, buffer_length);
        result = true;
    }
    return result;
}

// @address 0x4d8c10 parse_xml_match_outcome
extern "C" bool parse_xml_match_outcome(void *context, wchar_t *buffer, long buffer_length)
{
    c_gui_screen_carnage_report *screen = (c_gui_screen_carnage_report *)context;
    bool flag = (g_saved_game_files_globals.flags & 1) != 0;
    long string_id = flag ? _carnage_report_string_film_reminder_saved : _carnage_report_string_film_reminder;
    c_static_wchar_string<1024> string;
    screen->get_string_by_string_id(string_id, &string);
    string.copy_to(buffer, buffer_length);
    return true;
}

// @address 0x4d8d38 gui_screen_carnage_report_parse_campaign_difficulty
extern "C" bool gui_screen_carnage_report_parse_campaign_difficulty(void *context, wchar_t *buffer, long buffer_length)
{
    bool result = false;
    if (game_globals->options.game_mode == 1) {
        long string_id = -1;
        switch (game_globals->options.campaign_difficulty) {
        case 0: string_id = _carnage_report_string_easy; break;
        case 1: string_id = _carnage_report_string_normal; break;
        case 2: string_id = _carnage_report_string_heroic; break;
        case 3: string_id = _carnage_report_string_legendary; break;
        }
        if (string_id != -1) {
            c_static_wchar_string<1024> string;
            if (user_interface_global_string_get((int)string_id, (unsigned short *)string.m_string)) {
                string.copy_to(buffer, buffer_length);
                result = true;
            }
        }
    }
    return result;
}

// @address 0x4f4b48 ?get_column_names@c_gui_game_results_data@@UAAXQAJPAJ@Z
void c_gui_game_results_data::get_column_names(long *const column_names, long *column_count)
{
    *column_count = 0;
    column_names[(*column_count)++] = _string_id_name;
    column_names[(*column_count)++] = _carnage_report_string_results_place;
    column_names[(*column_count)++] = _carnage_report_string_results_score;
    column_names[(*column_count)++] = _carnage_report_string_carnage_kills;
    column_names[(*column_count)++] = _carnage_report_string_carnage_assists;
    column_names[(*column_count)++] = _carnage_report_string_carnage_deaths;
    column_names[(*column_count)++] = _carnage_report_string_carnage_kdspread;
    column_names[(*column_count)++] = _carnage_report_string_killbreakdown_weapon;
    column_names[(*column_count)++] = _carnage_report_string_killbreakdown_grenade;
    column_names[(*column_count)++] = _carnage_report_string_killbreakdown_melee;
    column_names[(*column_count)++] = _carnage_report_string_killbreakdown_other;
    column_names[(*column_count)++] = _carnage_report_string_fieldstats_headshots;
    column_names[(*column_count)++] = _carnage_report_string_fieldstats_bestspree;
    column_names[(*column_count)++] = _carnage_report_string_fieldstats_averagelife;
    column_names[(*column_count)++] = _carnage_report_string_fieldstats_medals;
    column_names[(*column_count)++] = _carnage_report_string_objectives_1;
    column_names[(*column_count)++] = _carnage_report_string_objectives_2;
    column_names[(*column_count)++] = _carnage_report_string_objectives_3;
    column_names[(*column_count)++] = _carnage_report_string_objectives_4;
}

// @address 0x4f4d68 ?update@c_gui_game_results_data@@UAAXXZ
void c_gui_game_results_data::update()
{
    const c_game_results *results = carnage_final_game_results();
    player_count = 0;
    if (results) {
        for (unsigned long player_index = 0; player_index < 16; ++player_index) {
            if (results->players[player_index].player_exists) {
                player_indices[player_count] = player_index;
                ++player_count;
            }
        }
        g_game_results_sort_results = results;
        qsort(player_indices, player_count, sizeof(long), game_results_sort_players_by_standing);
        g_game_results_sort_results = 0;
    }
}


// @address 0x4f4ed8 ?get_integer_value@c_gui_game_results_data@@UAA_NJJPAJ@Z
bool c_gui_game_results_data::get_integer_value(long element_index, long value_name, long *value)
{
    bool result = false;
    const c_game_results *results = carnage_final_game_results();
    if (results && element_index >= 0 && element_index < player_count) {
        long player_index = player_indices[element_index];
        if (player_index != -1) {
            const s_game_results_player_data *player = &results->players[player_index];
            const s_game_results_player_statistics *statistics = &results->statistics.player[player_index];
            switch (value_name) {
            case _scoreboard_column_base_color:
                *value = player->configuration.client.appearance.primary_color;
                result = true;
                break;
            case _scoreboard_column_team:
                *value = player->configuration.host.default_e8;
                result = true;
                break;
            case _scoreboard_column_controller:
                *value = -1;
                result = true;
                if (user_interface_squad_exists()) {
                    long squad_player_index = user_interface_squad_get_player_index((const s_player_identifier *)&player->identifier);
                    if (squad_player_index != -1 && user_interface_squad_is_local_player(squad_player_index))
                        *value = user_interface_squad_get_controller_index(squad_player_index);
                }
                break;
            case _carnage_report_string_carnage_kills:
                *value = statistics->statistics[8].value;
                result = true;
                break;
            case _carnage_report_string_carnage_assists:
                *value = statistics->statistics[9].value;
                result = true;
                break;
            case _carnage_report_string_carnage_deaths:
                *value = statistics->statistics[10].value;
                result = true;
                break;
            case _carnage_report_string_killbreakdown_weapon:
                *value = game_results_count_damage_statistic(player_index, _game_results_damage_statistic_none, _game_results_damage_breakdown_type_weapon);
                result = true;
                break;
            case _carnage_report_string_killbreakdown_melee:
                *value = game_results_count_damage_statistic(player_index, _game_results_damage_statistic_none, _game_results_damage_breakdown_type_melee);
                result = true;
                break;
            case _carnage_report_string_killbreakdown_grenade:
                *value = game_results_count_damage_statistic(player_index, _game_results_damage_statistic_none, _game_results_damage_breakdown_type_grenade);
                result = true;
                break;
            case _carnage_report_string_killbreakdown_other:
                *value = game_results_count_damage_statistic(player_index, _game_results_damage_statistic_none,
                        _game_results_damage_breakdown_type_all)
                    - game_results_count_damage_statistic(player_index, _game_results_damage_statistic_none,
                        _game_results_damage_breakdown_type_weapon)
                    - game_results_count_damage_statistic(player_index, _game_results_damage_statistic_none,
                        _game_results_damage_breakdown_type_melee)
                    - game_results_count_damage_statistic(player_index, _game_results_damage_statistic_none,
                        _game_results_damage_breakdown_type_grenade);
                result = true;
                break;
            case _carnage_report_string_fieldstats_headshots:
                *value = game_results_count_damage_statistic(player_index, _game_results_damage_statistic_headshots,
                    _game_results_damage_breakdown_type_all);
                result = true;
                break;
            case _carnage_report_string_fieldstats_bestspree:
                *value = statistics->statistics[13].value;
                result = true;
                break;
            case _carnage_report_string_fieldstats_medals:
                *value = 0;
                for (long medal_index = 0; medal_index < 58; medal_index++)
                    *value += statistics->medals[medal_index].value;
                result = true;
                break;
            case _carnage_report_string_objectives_1:
                switch (results->game_variant.get_game_engine_index()) {
                case _game_engine_ctf:
                    *value = statistics->statistics[15].value;
                    result = true;
                    break;
                case _game_engine_slayer:
                    *value = statistics->statistics[8].value;
                    result = true;
                    break;
                case _game_engine_oddball:
                case _game_engine_king:
                case _game_engine_sandbox:
                    break;
                case _game_engine_vip:
                    *value = statistics->statistics[34].value;
                    result = true;
                    break;
                case _game_engine_territories:
                    *value = statistics->statistics[45].value;
                    result = true;
                    break;
                case _game_engine_assault:
                    *value = statistics->statistics[22].value;
                    result = true;
                    break;
                case _game_engine_juggernaut:
                    *value = statistics->statistics[39].value;
                    result = true;
                    break;
                }
                break;
            case _carnage_report_string_objectives_2:
                switch (results->game_variant.get_game_engine_index()) {
                case _game_engine_ctf:
                    *value = statistics->statistics[16].value;
                    result = true;
                    break;
                case _game_engine_slayer:
                    *value = statistics->statistics[12].value;
                    result = true;
                    break;
                case _game_engine_oddball:
                    *value = statistics->statistics[25].value;
                    result = true;
                    break;
                case _game_engine_king:
                case _game_engine_sandbox:
                    break;
                case _game_engine_vip:
                    *value = statistics->statistics[35].value;
                    result = true;
                    break;
                case _game_engine_territories:
                    *value = statistics->statistics[46].value;
                    result = true;
                    break;
                case _game_engine_assault:
                    *value = statistics->statistics[19].value;
                    result = true;
                    break;
                case _game_engine_infection:
                    *value = statistics->statistics[48].value;
                    result = true;
                    break;
                case _game_engine_juggernaut:
                    *value = statistics->statistics[40].value;
                    result = true;
                    break;
                }
                break;
            case _carnage_report_string_objectives_3:
                switch (results->game_variant.get_game_engine_index()) {
                case _game_engine_ctf:
                    *value = statistics->statistics[18].value;
                    result = true;
                    break;
                case _game_engine_slayer:
                    *value = statistics->statistics[11].value;
                    result = true;
                    break;
                case _game_engine_oddball:
                    *value = statistics->statistics[26].value;
                    result = true;
                    break;
                case _game_engine_king:
                case _game_engine_sandbox:
                case _game_engine_vip:
                case _game_engine_juggernaut:
                case _game_engine_territories:
                    break;
                case _game_engine_assault:
                    *value = statistics->statistics[21].value;
                    result = true;
                    break;
                case _game_engine_infection:
                    *value = statistics->statistics[49].value;
                    result = true;
                    break;
                }
                break;
            case _carnage_report_string_objectives_4:
                if (results->game_variant.get_game_engine_index() == 1) {
                    *value = statistics->statistics[17].value;
                    result = true;
                }
                break;
            }
        }
    }
    return result;
}

// @address 0x4f53c8 ?get_text_value@c_gui_game_results_data@@UAA_NJJPAV?$c_static_wchar_string@$0EAA@@@@Z
bool c_gui_game_results_data::get_text_value(long element_index, long value_name, c_static_wchar_string<1024> *value)
{
    bool result = false;
    const c_game_results *results = carnage_final_game_results();
    c_static_wchar_string<256> text;
    if (results && element_index >= 0 && element_index < player_count) {
        long player_index = player_indices[element_index];
        if (player_index != -1) {
            const s_game_results_player_data *player = &results->players[player_index];
            const s_game_results_player_statistics *statistics = &results->statistics.player[player_index];
            if (value_name <= _carnage_report_string_fieldstats_averagelife) {
                if (value_name != _carnage_report_string_fieldstats_averagelife) {
                    if (value_name <= _carnage_report_string_results_score) {
                        if (value_name == _carnage_report_string_results_score) {
                            game_engine_get_score_string(player->final_score, &text);
                            value->set(text.m_string);
                            return true;
                        } else if (value_name != _string_id_name) {
                            if (value_name == _carnage_report_string_results_place) {
                                game_engine_get_place_string(player->standing, &text);
                                value->set(text.m_string);
                                return true;
                            }
                        } else {
                            value->set(player->configuration.host.player_name);
                            result = true;
                        }
                    } else if (value_name == _carnage_report_string_carnage_kdspread) {
                        int difference = (statistics->values[8] >> 1) - (statistics->values[10] >> 1);
                        if (difference == 0) {
                            value->set(L"  0");
                        } else if (difference > 0) {
                            value->print(L"+ %d", difference);
                        } else {
                            value->print(L"- %d", -difference);
                        }
                        result = true;
                    }
                } else {
                    return user_interface_get_timespan_string((statistics->values[14] >> 1) / ((statistics->values[10] >> 1) + 1), 1, false, value);
                }
            } else {
                long game_engine_index;
                switch (value_name - _carnage_report_string_objectives_1) {
                case 0:
                    game_engine_index = results->game_variant.get_game_engine_index();
                    if (game_engine_index != 3) {
                        if (game_engine_index != 4) {
                            if (game_engine_index == 10) {
                                user_interface_get_timespan_string(statistics->values[50] >> 1, 1, false, value);
                                result = true;
                            }
                        } else {
                            user_interface_get_timespan_string(statistics->values[29] >> 1, 1, false, value);
                            result = true;
                        }
                    } else {
                        user_interface_get_timespan_string(statistics->values[23] >> 1, 1, false, value);
                        result = true;
                    }
                    break;
                case 1:
                    game_engine_index = results->game_variant.get_game_engine_index();
                    if (game_engine_index == 4) {
                        user_interface_get_timespan_string(statistics->values[30] >> 1, 1, false, value);
                        result = true;
                    }
                    break;
                case 2:
                    switch (results->game_variant.get_game_engine_index() - 4) {
                    case 0:
                        game_engine_get_score_string(player->final_score, &text);
                        value->set(text.m_string);
                        return true;
                    case 2:
                        user_interface_get_timespan_string(statistics->values[37] >> 1, 1, false, value);
                        result = true;
                        break;
                    case 4:
                        user_interface_get_timespan_string(statistics->values[47] >> 1, 1, false, value);
                        result = true;
                        break;
                    case 3:
                        user_interface_get_timespan_string(statistics->values[41] >> 1, 1, false, value);
                        result = true;
                        break;
                    }
                    break;
                case 3:
                    switch (results->game_variant.get_game_engine_index() - 2) {
                    case 0:
                    case 1:
                    case 4:
                    case 5:
                    case 8:
                        game_engine_get_score_string(player->final_score, &text);
                        value->set(text.m_string);
                        return true;
                    case 2:
                    case 3:
                    case 6:
                    case 7:
                    default:
                        break;
                    }
                    break;
                default:
                    break;
                }
            }
        }
    }
    return result;
}

// @address 0x4f5750 ?get_player_configuration@c_gui_game_results_data@@UAA_NJPAUs_player_configuration@@@Z
bool c_gui_game_results_data::get_player_configuration(long element_index, s_player_configuration *configuration)
{
    bool result = false;
    const c_game_results *results = carnage_final_game_results();
    if (results && element_index >= 0 && element_index < player_count) {
        long player_index = player_indices[element_index];
        if (player_index != -1) {
            const s_game_results_player_data * player = &results->players[player_index];
            memcpy(configuration, &player->configuration, sizeof(*configuration));
            result = true;
        }
    }
    return result;
}

// @address 0x4f5800 ?get_player_appearance@c_gui_game_results_data@@UAA_NJPAUs_player_appearance@@@Z
bool c_gui_game_results_data::get_player_appearance(long element_index, s_player_appearance *appearance)
{
    s_player_configuration configuration;
    if (get_player_configuration(element_index, &configuration)) {
        *appearance = configuration.client.appearance;
        return true;
    }
    return false;
}

// @address 0x4f5898 ?get_current_item_count_internal@c_gui_game_results_data@@MAAJXZ
long c_gui_game_results_data::get_current_item_count_internal()
{
    return player_count;
}

// @address 0x4f58a0 ?get_column_names@c_gui_game_results_team_data@@UAAXQAJPAJ@Z
void c_gui_game_results_team_data::get_column_names(long *const column_names, long *column_count)
{
    *column_count = 0;
    column_names[(*column_count)++] = _string_id_name;
    column_names[(*column_count)++] = _carnage_report_string_results_place;
    column_names[(*column_count)++] = _carnage_report_string_results_score;
}

// @address 0x4f58f8 ?update@c_gui_game_results_team_data@@UAAXXZ
void c_gui_game_results_team_data::update()
{
    const c_game_results *results = carnage_final_game_results();
    team_count = 0;
    if (results) {
        for (unsigned long team_index = 0; team_index < 16; ++team_index) {
            if (results->teams.team[team_index].team_exists) {
                team_indices[team_count] = team_index;
                ++team_count;
            }
        }
        if (team_count > 0) {
            g_game_results_sort_results = results;
            qsort(team_indices, team_count, sizeof(long), carnage_report_compare_teams_by_place);
            g_game_results_sort_results = 0;
        }
    }
}

// @address 0x4f5a70 ?get_integer_value@c_gui_game_results_team_data@@UAA_NJJPAJ@Z
bool c_gui_game_results_team_data::get_integer_value(long element_index, long value_name, long *value)
{
    bool result = false;
    const c_game_results *results = carnage_final_game_results();
    if (results && element_index >= 0 && element_index < team_count) {
        long team_index = team_indices[element_index];
        if (team_index != -1) {
            switch (value_name) {
            case _scoreboard_column_team:
                *value = team_index;
                result = true;
                break;
            }
        }
    }
    return result;
}
// @address 0x4f5b00 ?get_text_value@c_gui_game_results_team_data@@UAA_NJJPAV?$c_static_wchar_string@$0EAA@@@@Z
bool c_gui_game_results_team_data::get_text_value(long element_index, long value_name, c_static_wchar_string<1024> *value)
{
    bool result = false;
    const c_game_results *results = carnage_final_game_results();
    c_static_wchar_string<256> text;
    if (results && interface_get_global_datasource(_carnage_report_string_teams) != 0 && element_index >= 0 && element_index < team_count) {
        long team_index = team_indices[element_index];
        if (team_index != -1) {
            switch (value_name) {
            case _string_id_name:
                game_engine_get_team_name(team_index, &text);
                value->set(text.m_string);
                result = true;
                break;
            case _carnage_report_string_results_score:
                game_engine_get_score_string(results->teams.team[team_index].final_score, &text);
                value->set(text.m_string);
                result = true;
                break;
            case _carnage_report_string_results_place:
                game_engine_get_place_string(results->teams.team[team_index].standing, &text);
                value->set(text.m_string);
                result = true;
                break;
            }
        }
    }
    return result;
}


// @address 0x4f5c20 ?medal_record_compare@@YA_NJJPAX@Z
bool medal_record_compare(long a,long b,void *context) {
const s_medal_record *record_a=(const s_medal_record *)&a;
const s_medal_record *record_b=(const s_medal_record *)&b;
bool result;
if(g_game_results_medal_info[record_a->medal].priority < g_game_results_medal_info[record_b->medal].priority) result=false;
else if(g_game_results_medal_info[record_a->medal].priority > g_game_results_medal_info[record_b->medal].priority) result=true;
else if(g_game_results_medal_info[record_a->medal].order > g_game_results_medal_info[record_b->medal].order) result=false;
else if(g_game_results_medal_info[record_a->medal].order < g_game_results_medal_info[record_b->medal].order) result=true;
else if(record_a->count > record_b->count) result=false;
else if(record_a->count < record_b->count) result=true;
else if(record_a->medal > record_b->medal) result=false;
else if(record_a->medal < record_b->medal) result=true;
else result=false;
return result;
}


// @address 0x4f5cc0 ?get_current_item_count_internal@c_gui_top_medals_data@@MAAJXZ
long c_gui_top_medals_data::get_current_item_count_internal()
{
    return medal_count;
}

// @address 0x4f5cd0 ?get_column_names@c_gui_top_medals_data@@UAAXQAJPAJ@Z
void c_gui_top_medals_data::get_column_names(long *const column_names, long *column_count)
{
    *column_count = 0;
    column_names[(*column_count)++] = _scoreboard_column_name;
    column_names[(*column_count)++] = _carnage_report_string_player_medal;
    column_names[(*column_count)++] = _player_select_string_image;
}

static __forceinline void gui_screen_carnage_report_sort_medals(s_medal_record *records, unsigned long count)
{
    if (count >= 2)
        templated_sort<long>::qsort((long *)records, count, (templated_sort<long>::t_compare)medal_record_compare, 0);
}

// @address 0x4f5d28 ?initialize@c_gui_top_medals_data@@UAA_NJ@Z
bool c_gui_top_medals_data::initialize(long name)
{
    short record_count = 0;
    this->m_name = name;
    bool result = true;
    const c_game_results *results = carnage_final_game_results();
    s_medal_record medal_records[16 * 58];
    if (results) {
        for (short player_index = 0; player_index < 16; ++player_index) {
            if (results->players[player_index].player_exists) {
                const s_game_results_player_statistics *statistics = &results->statistics.player[player_index];
                for (short medal_index = 0; medal_index < 58; ++medal_index) {
                    if (statistics->medals[medal_index].value > 0) {
                        medal_records[record_count].medal = (unsigned char)medal_index;
                        medal_records[record_count].player = (unsigned char)player_index;
                        medal_records[record_count].count = statistics->medals[medal_index].value;
                        ++record_count;
                    }
                }
            }
        }
    }
    gui_screen_carnage_report_sort_medals(medal_records, record_count);
    short previous_priority = -1;
    medal_count = 0;
    for (short index = 0; index < record_count && medal_count < 3; ++index) {
        short priority = (short)g_game_results_medal_info[medal_records[index].medal].priority;
        if (priority != previous_priority || priority >= 6 || priority < 2 && 3 - medal_count >= record_count - index) {
            records[medal_count] = medal_records[index];
            previous_priority = priority;
            ++medal_count;
        }
    }
    return result;
}
// @address 0x33c10 k_medal_name_string_ids
// @address 0x33cf8 k_medal_description_string_ids
// @rdata 0x82033c10..0x82033de0
extern "C" {
const long k_medal_name_string_ids[58] = {
    _string_id_extermination,
    _string_id_perfection,
    _string_id_multiple_kill_2,
    _string_id_multiple_kill_3,
    _string_id_multiple_kill_4,
    _string_id_multiple_kill_5,
    _string_id_multiple_kill_6,
    _string_id_multiple_kill_7,
    _string_id_multiple_kill_8,
    _string_id_multiple_kill_9,
    _string_id_multiple_kill_10,
    _string_id_5_kills_in_a_row,
    _string_id_10_kills_in_a_row,
    _string_id_15_kills_in_a_row,
    _string_id_20_kills_in_a_row,
    _string_id_25_kills_in_a_row,
    _string_id_30_kills_in_a_row,
    _string_id_sniper_kill_5,
    _string_id_sniper_kill_10,
    _string_id_shotgun_kill_5,
    _string_id_shotgun_kill_10,
    _string_id_collision_kill_5,
    _string_id_collision_kill_10,
    _string_id_sword_kill_5,
    _string_id_sword_kill_10,
    _string_id_juggernaut_kill_5,
    _string_id_juggernaut_kill_10,
    _string_id_zombie_kill_human_5,
    _string_id_zombie_kill_human_10,
    _string_id_human_kill_zombie_5,
    _string_id_human_kill_zombie_10,
    _string_id_human_kill_zombie_15,
    _string_id_koth_kill_defence_5,
    _string_id_shotgun_kill_sword,
    _string_id_vehicle_impact_kill,
    _string_id_vehicle_hijack,
    _string_id_aircraft_hijack,
    _string_id_deadplayer_kill,
    _string_id_player_kill_spreeplayer,
    _string_id_spartanlaser_kill,
    _string_id_stickygrenade_kill,
    _string_id_sniper_kill,
    _string_id_bashbehind_kill,
    _string_id_bash_kill,
    _string_id_flame_kill,
    _string_id_driver_assist_gunner,
    _string_id_assault_bomb_planted,
    _string_id_assault_player_kill_carrier,
    _string_id_vip_player_kill_vip,
    _string_id_juggernaut_player_kill_juggernaut,
    _string_id_oddball_carrier_kill_player,
    _string_id_ctf_flag_captured,
    _string_id_ctf_flag_player_kill_carrier,
    _string_id_ctf_flag_carrier_kill_player,
    _string_id_infection_survive,
    _string_id_unused1,
    _string_id_unused2,
    _string_id_unused3,
};
const long k_medal_description_string_ids[58] = {
    _player_select_string_extermination_description,
    _player_select_string_perfection_description,
    _player_select_string_multiple_kill_2_description,
    _player_select_string_multiple_kill_3_description,
    _player_select_string_multiple_kill_4_description,
    _player_select_string_multiple_kill_5_description,
    _player_select_string_multiple_kill_6_description,
    _player_select_string_multiple_kill_7_description,
    _player_select_string_multiple_kill_8_description,
    _player_select_string_multiple_kill_9_description,
    _player_select_string_multiple_kill_10_description,
    _player_select_string_5_kills_in_a_row_description,
    _player_select_string_10_kills_in_a_row_description,
    _player_select_string_15_kills_in_a_row_description,
    _player_select_string_20_kills_in_a_row_description,
    _player_select_string_25_kills_in_a_row_description,
    _player_select_string_30_kills_in_a_row_description,
    _player_select_string_sniper_kill_5_description,
    _player_select_string_sniper_kill_10_description,
    _player_select_string_shotgun_kill_5_description,
    _player_select_string_shotgun_kill_10_description,
    _player_select_string_collision_kill_5_description,
    _player_select_string_collision_kill_10_description,
    _player_select_string_sword_kill_5_description,
    _player_select_string_sword_kill_10_description,
    _player_select_string_juggernaut_kill_5_description,
    _player_select_string_juggernaut_kill_10_description,
    _player_select_string_zombie_kill_human_5_description,
    _player_select_string_zombie_kill_human_10_description,
    _player_select_string_human_kill_zombie_5_description,
    _player_select_string_human_kill_zombie_10_description,
    _player_select_string_human_kill_zombie_15_description,
    _player_select_string_koth_kill_defence_5_description,
    _player_select_string_shotgun_kill_sword_description,
    _player_select_string_vehicle_impact_kill_description,
    _player_select_string_vehicle_hijack_description,
    _player_select_string_aircraft_hijack_description,
    _player_select_string_deadplayer_kill_description,
    _player_select_string_player_kill_spreeplayer_description,
    _player_select_string_spartanlaser_kill_description,
    _player_select_string_stickygrenade_kill_description,
    _player_select_string_sniper_kill_description,
    _player_select_string_bashbehind_kill_description,
    _player_select_string_bash_kill_description,
    _player_select_string_flame_kill_description,
    _player_select_string_driver_assist_gunner_description,
    _player_select_string_assault_bomb_planted_description,
    _player_select_string_assault_player_kill_carrier_description,
    _player_select_string_vip_player_kill_vip_description,
    _player_select_string_juggernaut_player_kill_juggernaut_description,
    _player_select_string_oddball_carrier_kill_player_description,
    _player_select_string_ctf_flag_captured_description,
    _player_select_string_ctf_flag_player_kill_carrier_description,
    _player_select_string_ctf_flag_carrier_kill_player_description,
    _player_select_string_infection_survive_description,
    _player_select_string_unused1_description,
    _player_select_string_unused2_description,
    _player_select_string_unused3_description
};
}
// @rdata-end

// @address 0x4f5f10 ?get_integer_value@c_gui_top_medals_data@@UAA_NJJPAJ@Z
bool c_gui_top_medals_data::get_integer_value(long element_index, long value_name, long *value)
{
    bool result = false;
    if (value_name == _player_select_string_image && element_index >= 0 && element_index < medal_count) {
        *value = records[element_index].medal;
        result = true;
    }
    return result;
}

// @address 0x4f5f50 ?get_text_value@c_gui_top_medals_data@@UAA_NJJPAV?$c_static_wchar_string@$0EAA@@@@Z
bool c_gui_top_medals_data::get_text_value(long element_index, long value_name, c_static_wchar_string<1024> *value)
{
    const c_game_results *results = carnage_final_game_results();
    bool success = false;
    if (results && element_index >= 0 && element_index < medal_count) {
        const s_medal_record *record = &records[element_index];
        switch (value_name) {
        case _scoreboard_column_name:
            if (results->players[record->player].player_exists)
                value->set(results->players[record->player].configuration.host.player_name);
            break;
        case _carnage_report_string_player_medal:
            if (record->medal < 58) {
                const s_user_interface_shared_globals *globals = ::user_interface_shared_tag_globals_try_and_get();
                if (globals && globals->global_strings.index != -1) {
                    text_group_get_string(globals->global_strings.index, k_medal_name_string_ids[record->medal], value);
                    value->append_print(L" x%i", record->count);
                    success = true;
                }
            }
            break;
        }
    }
    return success;
}

// @address 0x4f61e8 ?get_current_item_count_internal@c_gui_campaign_metagame_data@@MAAJXZ
long c_gui_campaign_metagame_data::get_current_item_count_internal()
{
    return player_count;
}

// @address 0x4f61f8 ?get_column_names@c_gui_campaign_metagame_data@@UAAXQAJPAJ@Z
void c_gui_campaign_metagame_data::get_column_names(long *const column_names, long *column_count)
{
    *column_count = 0;
    column_names[(*column_count)++] = _string_id_name;
    column_names[(*column_count)++] = _carnage_report_string_campaign_score;
    column_names[(*column_count)++] = _carnage_report_string_campaign_deaths;
    column_names[(*column_count)++] = _carnage_report_string_campaign_betrayals;
    column_names[(*column_count)++] = _carnage_report_string_campaign_friendlyfire;
    column_names[(*column_count)++] = _carnage_report_string_campaign_kills;
    column_names[(*column_count)++] = _carnage_report_string_campaign_infantry;
    column_names[(*column_count)++] = _carnage_report_string_campaign_leader;
    column_names[(*column_count)++] = _carnage_report_string_campaign_hero;
    column_names[(*column_count)++] = _carnage_report_string_campaign_specialist;
    column_names[(*column_count)++] = _carnage_report_string_campaign_light;
    column_names[(*column_count)++] = _carnage_report_string_campaign_standard;
    column_names[(*column_count)++] = _carnage_report_string_campaign_heavy;
    column_names[(*column_count)++] = _carnage_report_string_campaign_giant;
    column_names[(*column_count)++] = _carnage_report_string_campaign_medals;
    column_names[(*column_count)++] = _carnage_report_string_campaign_style;
    column_names[(*column_count)++] = _carnage_report_string_campaign_kill_percent;
    column_names[(*column_count)++] = _carnage_report_string_campaign_points;
}

// @address 0x4f6400 ?get_integer_value@c_gui_campaign_metagame_data@@UAA_NJJPAJ@Z
bool c_gui_campaign_metagame_data::get_integer_value(long element_index, long value_name, long *value)
{
    s_campaing_metagame_globals *globals = g_campaing_metagame_globals;
    s_campaing_metagame_globals *metagame = 0;
    if (globals && (globals->flags & 2))
        metagame = globals;
    s_campaign_metagame_player_globals *slots = 0;
    if (g_campaign_metagame_player_globals.active)
        slots = &g_campaign_metagame_player_globals;
    bool result = false;
    if (metagame && slots && element_index >= 0 && element_index < player_count) {
        long player_index = player_indices[element_index];
        if ((unsigned long)player_index <= 3) {
            const s_campaing_metagame_player *player = &metagame->players[player_index];
            const s_campaign_metagame_player_slot *slot = &slots->slots[player_index];
            int datum_index = player_find_by_identifier(player->identifier);
            if (datum_index != -1) {
                switch (value_name) {
                case _scoreboard_column_controller:
                    *value = -1;
                    result = true;
                    if (user_interface_squad_exists()) {
                        long squad_player_index = user_interface_squad_get_player_index((const s_player_identifier *)player);
                        if (squad_player_index != -1 && user_interface_squad_is_local_player(squad_player_index))
                            *value = user_interface_squad_get_controller_index(squad_player_index);
                    }
                    break;
                case _scoreboard_column_base_color:
                {
                    player_datum *players = (player_datum *)tls_player_data()->data;
                    *value = players[datum_index_to_absolute_index(datum_index)].configuration.client.appearance.primary_color;
                    result = true;
                    break;
                }
                case _carnage_report_string_campaign_score: *value = player->scaled; result = true; break;
                case _carnage_report_string_campaign_deaths: *value = slot->score_without_bonus; result = true; break;
                case _carnage_report_string_campaign_betrayals: *value = slot->bonus; result = true; break;
                case _carnage_report_string_campaign_friendlyfire: *value = player->ai_kills; result = true; break;
                case _carnage_report_string_campaign_kills: *value = player->total_kills; result = true; break;
                case _carnage_report_string_campaign_infantry: *value = (short)player->sums[0]; result = true; break;
                case _carnage_report_string_campaign_leader: *value = (short)player->sums[1]; result = true; break;
                case _carnage_report_string_campaign_hero: *value = (short)player->sums[2]; result = true; break;
                case _carnage_report_string_campaign_specialist: *value = (short)player->sums[3]; result = true; break;
                case _carnage_report_string_campaign_light: *value = (short)player->sums[4]; result = true; break;
                case _carnage_report_string_campaign_standard: *value = (short)player->sums[7]; result = true; break;
                case _carnage_report_string_campaign_heavy: *value = (short)player->sums[5]; result = true; break;
                case _carnage_report_string_campaign_giant: *value = (short)player->sums[6]; result = true; break;
                case _carnage_report_string_campaign_medals: *value = player->extra_total; result = true; break;
                case _carnage_report_string_campaign_style: *value = player->bonus; result = true; break;
                case _carnage_report_string_campaign_kill_percent: *value = player->total - player->bonus; result = true; break;
                case _carnage_report_string_campaign_points: *value = player->total; result = true; break;
                }
            }
        }
    }
    return result;
}

// @address 0x4f6800 ?get_text_value@c_gui_campaign_metagame_data@@UAA_NJJPAV?$c_static_wchar_string@$0EAA@@@@Z
bool c_gui_campaign_metagame_data::get_text_value(long element_index, long value_name, c_static_wchar_string<1024> *value)
{
    s_campaing_metagame_globals *globals = 0;
    s_campaing_metagame_globals *metagame = g_campaing_metagame_globals;
    if (metagame && (metagame->flags & 2))
        globals = metagame;
    bool result = false;
    if (globals && element_index >= 0 && element_index < player_count) {
        long player_index = player_indices[element_index];
        if ((unsigned long)player_index <= 3) {
            int index = player_find_by_identifier(globals->players[player_index].identifier);
            if (index != -1 && value_name == _string_id_name) {
                s_data_array *players = tls_player_data();
                player_datum *player = &((player_datum *)players->data)[datum_index_to_absolute_index(index)];
                value->set(player->configuration.host.player_name);
                result = true;
            }
        }
    }
    return result;
}

static __forceinline void gui_screen_carnage_report_iterator_begin(c_data_iterator<player_datum> *it, s_data_array *data)
{
    it->m_iterator.data = data;
    it->m_iterator.index = it->m_iterator.absolute_index = -1;
}


// @address 0x4d7ee8 ?dispose@c_gui_screen_carnage_report@@UAAXXZ
void c_gui_screen_carnage_report::dispose()
{
    bool metagame_finished = false;
    if (g_campaing_metagame_globals != 0) {
        long flags = g_campaing_metagame_globals->flags;
        if ((flags & 2) != 0)
            metagame_finished = (((unsigned long)flags >> 3) & 1);
    }
    if (metagame_finished)
        campaign_metagame_reset_finished();
    c_gui_screen_widget::dispose();
}

// @address 0x4f6088 player_find_by_identifier
extern "C" int player_find_by_identifier(const unsigned char *identifier)
{
    int result = -1;
    c_data_iterator<player_datum> it;
    gui_screen_carnage_report_iterator_begin(&it, player_data);
    while ((it.m_datum = (player_datum *)data_iterator_next(&it.m_iterator)) != 0) {
        if (memcmp(it.m_datum->player_identifier.data, identifier, sizeof(it.m_datum->player_identifier)) == 0) {
            result = it.m_iterator.index;
            break;
        }
    }
    return result;
}

// @address 0x4d7b28 ?update_render_state@c_gui_screen_carnage_report@@UAAXK@Z
void c_gui_screen_carnage_report::update_render_state(unsigned long current_milliseconds)
{
    c_gui_widget::update_render_state(current_milliseconds);
    c_gui_list_widget *first_list = (c_gui_list_widget *)get_child_widget(_gui_list_widget, _player_select_string_player_list);
    c_gui_list_widget *second_list = (c_gui_list_widget *)get_child_widget(_gui_list_widget, _carnage_report_string_teams_list);
    if (first_list != 0)
        update_render_state_for_list(first_list);
    if (second_list != 0)
        update_render_state_for_list(second_list);
}

// @address 0x4d7ba0 ?update_render_state_for_list@c_gui_screen_carnage_report@@IAAXPAVc_gui_list_widget@@@Z
void c_gui_screen_carnage_report::update_render_state_for_list(c_gui_list_widget *list)
{
    c_gui_data *page_data = 0;
    long data_count = this->m_datasource_count;
    long index = 0;
    if (data_count > 0) {
        do {
            c_gui_data *candidate = m_datasource[index];
            if (candidate->m_name == _carnage_report_string_page_columns) {
                page_data = (c_gui_data *)m_datasource[index];
                break;
            }
            index++;
        } while (index < data_count);
    }
    c_gui_data *list_data = (c_gui_data *)(list)->get_data();
    c_gui_widget *next;
    long current_page = get_current_page_name();
    bool team_game = false;
    if (report_type == 1) {
        const c_game_results *results = user_interface_networking_get_final_game_results();
        if (results != 0)
            team_game = results->teams_enabled;
    }
    if (list_data != 0 && page_data != 0) {
        c_gui_widget *head = gui_widget_find_child_by_name((c_gui_widget *)list, 5);
        if (head != 0) {
            do {
                c_gui_widget *item = head->get_children();
                if (item != 0) {
                    do {
                        bool visible = false;
                        bool colored = false;
                        long element = page_data->get_first_element_handle();
                        if (element != -1) {
                            do {
                                long name = -1;
                                if (page_data->get_string_id_value(element, _carnage_report_string_widget_name, &name)
                                    && name == item->m_name) {
                                    long page = -1;
                                    long flags = -1;
                                    if (page_data->get_string_id_value(element, _carnage_report_string_page, &page)) {
                                        visible = (unsigned char)(visible | (page == current_page || page == _carnage_report_string_sniper));
                                        if (page_data->get_integer_value(element, _carnage_report_string_colors, &flags))
                                            colored = colored | (flags != 0);
                                    }
                                }
                                element = page_data->get_next_element_handle(element);
                            } while (element != -1);
                        }
                        item->set_visible(visible);
                        if (visible != 0 && colored != 0) {
                            if (team_game != 0) {
                                long color_index = -1;
                                if (list_data->get_integer_value(head->get_element_handle(), _scoreboard_column_team, &color_index))
                                    tint_widget_to_change_color(item, color_index, true);
                            } else {
                                long color_index = -1;
                                if (list_data->get_integer_value(head->get_element_handle(), _scoreboard_column_base_color, &color_index))
                                    tint_widget_to_change_color(item, color_index, false);
                            }
                        }
                        item = item->get_next();
                    } while (item != 0);
                }
                next = gui_widget_get_next_sibling_of_type(head, 5);
                if (next != 0 && next->get_element_handle() == -1)
                    next = 0;
                head = next;
            } while (next != 0);
        }
    }
}

// @address 0x4f6140 ?initialize@c_gui_campaign_metagame_data@@UAA_NJ@Z
bool c_gui_campaign_metagame_data::initialize(long name)
{
    s_campaing_metagame_globals *metagame = 0;
    if (g_campaing_metagame_globals != 0 && (g_campaing_metagame_globals->flags & 2) != 0)
        metagame = g_campaing_metagame_globals;
    bool result = false;
    if (metagame != 0) {
        result = true;
        m_name = name;
        short match_count = 0;
        short index = 0;
        do {
            if (player_find_by_identifier(metagame->players[index].identifier) != -1) {
                short slot = match_count;
                match_count = (short)(slot + 1);
                player_indices[slot] = index;
            }
            index = (short)(index + 1);
        } while (index < 4);
        player_count = match_count;
    }
    return result;
}


static __forceinline bool gui_screen_carnage_report_is_playback()
{
    bool playback = false;
    if (game_globals->options.game_simulation == 4)
        playback = true;
    return playback;
}

// @address 0x4d8220 ?handle_controller_input_message@c_gui_screen_carnage_report@@UAA_NPBVc_controller_input_message@@@Z
bool c_gui_screen_carnage_report::handle_controller_input_message(const c_controller_input_message *message)
{
    bool handled = false;
    c_gui_data *pages = find_data(_carnage_report_string_page, 0);
    if (pages && message->get_event_type() == _event_type_button_press) {
        switch (message->get_component()) {
        case 5:
            if (current_value_name == pages->get_first_element_handle()) {
                while (pages->get_next_element_handle(current_value_name) != -1)
                    current_value_name = pages->get_next_element_handle(current_value_name);
            } else {
                current_value_name = pages->get_previous_element_handle(current_value_name);
            }
            handled = true;
            break;
        case 4:
            if (pages->get_next_element_handle(current_value_name) == -1)
                current_value_name = pages->get_first_element_handle();
            else
                current_value_name = pages->get_next_element_handle(current_value_name);
            handled = true;
            break;
        case 1:
            if (game_globals->options.game_mode == 1 && !gui_screen_carnage_report_is_playback()) {
                campaign_metagame_reset_finished();
                handled = true;
            }
            break;
        case _controller_component_button_start:
            if (game_globals->options.game_mode == 1)
                handled = true;
            break;
        }
    }
    if (!handled)
        handled = c_gui_screen_widget::handle_controller_input_message(message);
    return handled;
}


static __forceinline long gui_screen_carnage_report_find_sorted_player(c_gui_game_results_data *data, const c_game_results *results, long team)
{
    long found = -1;
    for (long index = 0; index < data->player_count; ++index) {
        if (team == -1 || results->players[data->sorted_player_indices[index]].configuration.host.results_team_index == team) {
            found = index;
            break;
        }
    }
    return found;
}

// @address 0x4f4a08 gui_game_results_data_find_first_player_for_team
extern "C" short gui_game_results_data_find_first_player_for_team(c_gui_game_results_data *data, long team)
{
    long result = -1;
    long player_count = data->player_count;
    if (0 < player_count) {
        const c_game_results *results = user_interface_networking_get_final_game_results();
        if (results) {
            if (results->teams_enabled) {
                long count = 0;
                if (!data->players_sorted) {
                    for (unsigned long player = 0; player < 16; ++player) {
                        if (results->players[player].player_exists)
                            data->sorted_player_indices[count++] = player;
                    }
                    g_game_results_sort_results = results;
                    qsort(data->sorted_player_indices, count, sizeof(long), game_results_sort_players_by_statistics);
                    data->players_sorted = true;
                    g_game_results_sort_results = 0;
                }
                long index = gui_screen_carnage_report_find_sorted_player(data, results, team);
                if (index != -1)
                    result = (short)data->sorted_player_indices[index];
            } else {
                short first = -1;
                if (player_count > 0)
                    first = (short)data->player_indices[0];
                result = first;
            }
        }
    }
    return (short)result;
}


static __forceinline cs_point_set *gui_screen_carnage_report_get_point_set(short index)
{
    return &((cs_point_set *)((cs_script_data *)global_scenario->script_data.address)->point_sets.address)[index];
}

// @address 0x50ac68 ai_play_line_at_point
extern "C" short ai_play_line_at_point(long object, long point_ref, short count, long flags)
{
    scenario *scenario = global_scenario;
#if defined(_XBOX) && !defined(__clang__)
    float duration = AI_REAL_CONSTANTS[-553];
#else
    float duration = 0.0f;
#endif
    carnage_report_sort_entry entries[32];
    if (scenario->script_data.count > 0) {
        short set_index = (short)(point_ref >> 16);
        if (set_index >= 0 && set_index < ((cs_script_data *)scenario->script_data.address)->point_sets.count) {
            cs_point_set *set = gui_screen_carnage_report_get_point_set(set_index);
            if (count >= 5)
                count = 5;
            short limit = (short)(count <= set->points.count ? count : set->points.count);
            if (limit > 0) {
                for (short i = 0; i < set->points.count; i++) {
                    cs_point *source = &((cs_point *)set->points.address)[i];
                    entries[i].index = i;
                    entries[i].priority = players_get_nearest_player_distance(source);
                }
                if ((unsigned long)set->points.count >= 2)
                    templated_sort<long>::qsort((long *)entries, set->points.count, (templated_sort<long>::t_compare)ai_script_duration_compare, 0);
                for (short i = 0; i < limit; i++) {
                    if (entries[i].priority < 32767) {
                        cs_point *point = &((cs_point *)set->points.address)[entries[i].index];
                        real_point3d position;
                        real_vector3d forward;
                        ::ai_point_get_position(&point->position, &position);
                        vector3d_from_euler_angles2d(&forward, &point->facing);
                        ::ai_reference_frame_transform_vector(point->position.attachment_index, &forward, &forward);
                        float value = scripted_sound_new_mission_dialogue_at_point(&position, &forward, object, flags);
                        if (duration <= value)
                            duration = value;
                    }
                }
            }
        }
    }
    return (short)(int)(duration * 30.0f + AI_REAL_CONSTANTS[0]);
}

// @address 0x4d8460 ?handle_list_item_chosen@c_gui_screen_carnage_report@@UAA_NPBVc_controller_input_message@@JPAVc_gui_list_item_widget@@PAVc_gui_data@@@Z
bool c_gui_screen_carnage_report::handle_list_item_chosen(
    const c_controller_input_message *message, long list_name, c_gui_list_item_widget *list_item, c_gui_data *data)
{
    bool handled = false;
    if (this->report_type == 2) {
        if (list_name == _player_select_string_player_list) {
            c_gui_game_results_data *results_data = (c_gui_game_results_data *)find_data(_player_select_string_player_list, 0);
            if (results_data) {
                s_campaing_metagame_globals *metagame = g_campaing_metagame_globals;
                if (metagame && (metagame->flags & 2)) {
                    unsigned long slot = list_item->get_element_handle();
                    if (slot <= 3) {
                        unsigned long player_index = results_data->player_indices[slot];
                        if (player_index <= 3) {
                            game_options *options = &game_globals->options;
                            const s_campaing_metagame_player *player = &metagame->players[player_index];
                            if (options) {
                                for (short index = 0; index < 16; ++index) {
                                    const game_player_options *candidate = &options->players[index];
                                    if (!memcmp(&player->identifier, &candidate->player_identifier, sizeof(s_player_identifier))) {
                                        online_guide_show_gamer_card_ui(message->get_controller(), candidate->configuration.client.xuid);
                                        handled = true;
                                        break;
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    } else {
        const s_player_identifier *local_player = 0;
        const s_player_identifier *selected_player = 0;
        if (list_name == _player_select_string_player_list || list_name == _carnage_report_string_teams_list) {
            e_controller_index controller = message->get_controller();
            const c_game_results *results = user_interface_networking_get_final_game_results();
            if (results) {
                long squad_player = ::user_interface_squad_get_player_at_local_controller(controller);
                short result_player = -1;
                if (squad_player != -1)
                    local_player = ::user_interface_squad_get_player_identifier(squad_player);
                if (list_name == _player_select_string_player_list) {
                    long slot = list_item->get_element_handle();
                    short player = -1;
                    if (slot >= 0 && slot < ((c_gui_game_results_data *)data)->player_count)
                        player = (short)((c_gui_game_results_data *)data)->player_indices[slot];
                    result_player = player;
                } else if (list_name == _carnage_report_string_teams_list && data) {
                    long team;
                    if (data->get_integer_value(list_item->get_element_handle(), _scoreboard_column_team, &team)) {
                        c_gui_game_results_data *results_data = (c_gui_game_results_data *)find_data(_player_select_string_player_list, 0);
                        if (results_data)
                            result_player = gui_game_results_data_find_first_player_for_team(results_data, team);
                    }
                }
                if (result_player != -1)
                    selected_player = (const s_player_identifier *)&results->players[result_player].identifier;
            }
        }
        if (local_player && selected_player) {
            c_load_player_select_screen_message *load = (c_load_player_select_screen_message *)user_interface_malloc_tracked(sizeof(c_load_player_select_screen_message), 0, 0);
            if (load) {
                long layer = this->m_name;
                e_controller_index controller = message->get_controller();
                c_load_player_select_screen_message *posted = new (load) c_load_player_select_screen_message(
                    _player_select_string_carnage_report_player_details, controller, this->get_render_window(), layer, local_player, selected_player, true);
                if (posted) {
                    posted->set_parent_screen_index(this->m_screen_index);
                    ::user_interface_messaging_post((c_message const *)posted);
                    handled = true;
                }
            }
        } else {
            ::user_interface_play_sound(_ui_sound_error, this->m_core_definition.sound_override_index);
        }
    }
    if ((unsigned char)handled == 0)
        handled = false;
    return handled;
}

// @address 0x499198 ?get_render_data_size@c_gui_screen_carnage_report@@MAA?AW4e_render_data_size@@XZ
e_render_data_size c_gui_screen_carnage_report::get_render_data_size()
{
    return k_render_data_size_large;
}

// @address 0x4d7500 ?create_bitmap_widget@c_gui_screen_carnage_report@@UAAPAVc_gui_bitmap_widget@@PBUs_runtime_bitmap_widget_definition@@@Z
c_gui_bitmap_widget *c_gui_screen_carnage_report::create_bitmap_widget(const s_runtime_bitmap_widget_definition *definition)
{
    if (definition->name == _carnage_report_string_map_image)
        return new (0, 0, _ui_allocation_marker_dummy) c_gui_custom_bitmap_widget();
    return new (0, 0, _ui_allocation_marker_dummy) c_gui_bitmap_widget();
}

static __forceinline short carnage_report_get_campaign_difficulty()
{
    return game_globals->options.game_mode == 1 ? game_globals->options.campaign_difficulty : 1;
}

// @address 0x4d7588 ?update@c_gui_screen_carnage_report@@UAAXK@Z
void c_gui_screen_carnage_report::update(unsigned long current_milliseconds)
{
    if (!transitioning_out())
    {
        long current_page_name = get_current_page_name();
        long current_report_type = report_type;
        if (current_report_type == 0)
        {
            transition_out_with_transition_type(0, _screen_transition_normal);
        }
        else if (current_report_type == 1)
        {
            const c_game_results *results = user_interface_networking_get_final_game_results();
            c_gui_list_widget *players_list = (c_gui_list_widget *)get_child_widget(_gui_list_widget, _player_select_string_player_list);
            c_gui_list_widget *teams_list = (c_gui_list_widget *)get_child_widget(_gui_list_widget, _carnage_report_string_teams_list);
            c_gui_data *columns_data = find_data(_carnage_report_string_page_columns, 0);
            c_gui_model_widget *model_widget = (c_gui_model_widget *)get_child_widget(_gui_model_widget, _carnage_report_string_player_model);
            if (results == 0)
            {
                transition_out_with_transition_type(0, _screen_transition_normal);
            }
            else if (players_list != 0 && teams_list != 0 && columns_data != 0)
            {
                bool show_teams = results->teams_enabled && current_page_name == _carnage_report_string_page_results;
                players_list->set_visible(!show_teams);
                teams_list->set_visible(show_teams);
                if (show_teams && !teams_list->within_focus_chain())
                {
                    c_gui_data *team_data = (teams_list)->get_data();
                    long first_handle = team_data != 0 ? team_data->get_first_element_handle() : -1;
                    (this)->transfer_focus_to_list(teams_list, first_handle, false, false);
                }
                else if (!show_teams && !players_list->within_focus_chain())
                {
                    c_gui_data *player_data = (players_list)->get_data();
                    long first_handle = player_data != 0 ? player_data->get_first_element_handle() : -1;
                    (this)->transfer_focus_to_list(players_list, first_handle, false, false);
                }
                c_gui_widget *header_group = get_child_widget(_gui_group_widget, _carnage_report_string_game_summary);
                if (header_group != 0)
                {
                    c_gui_widget *teams_header = header_group->get_child_widget(_gui_text_widget, _carnage_report_string_mvp);
                    c_gui_widget *players_header = header_group->get_child_widget(_gui_text_widget, _carnage_report_string_victor);
                    header_group->set_visible(current_page_name == _carnage_report_string_page_results);
                    teams_header->set_visible(show_teams);
                    players_header->set_visible(!show_teams);
                }
                gui_screen_widget_set_display_group(this, 0, current_page_name);
            }

            c_gui_game_results_data *results_data = (c_gui_game_results_data *)find_data(_player_select_string_player_list, 0);
            long current_player_index = player_index;
            long target_player_index = results_data != 0 ? gui_game_results_data_find_first_player_for_team(results_data, -1) : -1;
            if (model_widget != 0 && results != 0 && target_player_index != -1 && target_player_index != current_player_index &&
                results->players[target_player_index].player_exists)
            {
                if (results->teams_enabled)
                    model_widget->set_player_appearance(
                        &results->players[target_player_index].configuration.client.appearance,
                        results->players[target_player_index].configuration.host.results_team_index);
                else
                    model_widget->set_player_appearance(
                        &results->players[target_player_index].configuration.client.appearance, -1);
                player_index = target_player_index;
            }

            bool show_rematch_timer = false;
            if (network_life_cycle_in_matchmaking())
            {
                s_life_cycle_matchmaking_progress progress;
                network_life_cycle_get_matchmaking_progress(&progress);
                g_user_interface_matchmaking_progress = progress;
                if (progress.type == 5)
                    show_rematch_timer = progress.countdown_started;
            }
            c_gui_widget *rematch_widget = get_child_widget(_gui_text_widget, _carnage_report_string_timer_next_match);
            if (rematch_widget != 0)
                rematch_widget->set_visible(show_rematch_timer);

            bool button_key_set = false;
            if ((g_saved_game_files_globals.flags & 1u) != 0)
            {
                gui_screen_widget_set_display_group(this, 1, _carnage_report_string_button_key_pgcr_user_saved_film);
                button_key_set = true;
            }
            if (!button_key_set)
                gui_screen_widget_set_display_group(this, 1, _carnage_report_string_button_key_default);
        }
        else if (current_report_type == 2)
        {
            c_gui_list_widget *players_list = (c_gui_list_widget *)get_child_widget(_gui_list_widget, _player_select_string_player_list);
            players_list->set_visible(true);
            gui_screen_widget_set_display_group(this, 0, current_page_name);
            c_gui_widget *summary_group = get_child_widget(_gui_group_widget, _carnage_report_string_game_summary);
            if (summary_group != 0)
            {
                bool show_summary = current_page_name == _carnage_report_string_campaign_results;
                summary_group->set_visible(show_summary);
                if (show_summary)
                {
                    c_gui_bitmap_widget *difficulty_bitmap = (c_gui_bitmap_widget *)get_child_widget(
                        _gui_bitmap_widget, _carnage_report_string_difficulty_image);
                    if (difficulty_bitmap != 0)
                        difficulty_bitmap->set_sprite_frame(carnage_report_get_campaign_difficulty());
                    c_gui_custom_bitmap_widget *map_bitmap = (c_gui_custom_bitmap_widget *)get_child_widget(
                        _gui_bitmap_widget, _carnage_report_string_map_image);
                    if (map_bitmap != 0)
                        map_bitmap->set_map_image(_custom_map_image_type_normal, (e_map_id)game_globals->options.map_id, true);
                }
            }
        }
    }
    c_gui_screen_widget::update(current_milliseconds);
}

typedef char carnage_countdown_timer_offset[(offsetof(s_life_cycle_matchmaking_progress, countdown_timer) == 0x54) ? 1 : -1];
typedef char carnage_progress_size[(sizeof(s_life_cycle_matchmaking_progress) == 248) ? 1 : -1];

typedef char carnage_scenario_script_data_offset[(offsetof(scenario, script_data) == 0x424) ? 1 : -1];
typedef char carnage_point_position_offset[(offsetof(cs_point, position) == 0x20) ? 1 : -1];
typedef char carnage_point_facing_offset[(offsetof(cs_point, facing) == 0x34) ? 1 : -1];
typedef char carnage_top_medals_allocation_size[(sizeof(void *) != 4 || ((sizeof(c_gui_top_medals_data) == 0x11c))) ? 1 : -1];
typedef char carnage_campaign_metagame_allocation_size[(sizeof(void *) != 4 || ((sizeof(c_gui_campaign_metagame_data) == 0x120))) ? 1 : -1];
