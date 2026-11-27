#ifndef INTERFACE_GUI_SCREENS_CARNAGE_REPORT_GUI_SCREEN_CARNAGE_REPORT_H
#define INTERFACE_GUI_SCREENS_CARNAGE_REPORT_GUI_SCREEN_CARNAGE_REPORT_H

// Functions defined in tu/interfacelibxbox/gui_screen_carnage_report.cpp.

extern "C" int player_find_by_identifier(const unsigned char *identifier);
extern "C" int carnage_report_compare_teams_by_place(const void *a, const void *b);
bool parse_xml_current_page_name(void *context, wchar_t *buffer, long buffer_length);
bool parse_xml_rematch_timer(void *context, wchar_t *buffer, long buffer_length);
bool parse_xml_game_variant(void *context, wchar_t *buffer, long buffer_length);
extern "C" bool gui_screen_carnage_report_parse_map_name(void *context, wchar_t *buffer, long buffer_length);
extern "C" bool parse_xml_player_name(void *context, wchar_t *buffer, long buffer_length);
extern "C" bool parse_xml_match_outcome(void *context, wchar_t *buffer, long buffer_length);
extern "C" bool gui_screen_carnage_report_parse_campaign_difficulty(void *context, wchar_t *buffer, long buffer_length);

class c_game_results;

// Carnage report datasource column/value names (cache string table set 1).
enum e_carnage_report_string_id
{
    _carnage_report_string_timer_next_match = 0x10424,
    _carnage_report_string_page_results = 0x10446,
    _carnage_report_string_page_carnage = 0x10447,
    _carnage_report_string_page_killbreakdown = 0x10448,
    _carnage_report_string_page_fieldstats = 0x10449,
    _carnage_report_string_page_objectives_slayer = 0x1044a,
    _carnage_report_string_page_objectives_ctf = 0x1044b,
    _carnage_report_string_page_objectives_assault = 0x1044c,
    _carnage_report_string_page_objectives_vip = 0x1044d,
    _carnage_report_string_page_objectives_territories = 0x1044e,
    _carnage_report_string_page_objectives_oddball = 0x1044f,
    _carnage_report_string_page_objectives_infection = 0x10450,
    _carnage_report_string_page_objectives_koth = 0x10451,
    _carnage_report_string_page_objectives_juggernaut = 0x10452,
    _carnage_report_string_results_place = 0x10453,
    _carnage_report_string_results_score = 0x10454,
    _carnage_report_string_carnage_kills = 0x10455,
    _carnage_report_string_carnage_assists = 0x10456,
    _carnage_report_string_carnage_deaths = 0x10457,
    _carnage_report_string_carnage_kdspread = 0x10458,
    _carnage_report_string_killbreakdown_weapon = 0x10459,
    _carnage_report_string_killbreakdown_grenade = 0x1045a,
    _carnage_report_string_killbreakdown_melee = 0x1045b,
    _carnage_report_string_killbreakdown_other = 0x1045c,
    _carnage_report_string_fieldstats_headshots = 0x1045d,
    _carnage_report_string_fieldstats_bestspree = 0x1045e,
    _carnage_report_string_fieldstats_averagelife = 0x1045f,
    _carnage_report_string_fieldstats_medals = 0x10460,
    _carnage_report_string_objectives_1 = 0x10461,
    _carnage_report_string_objectives_2 = 0x10462,
    _carnage_report_string_objectives_3 = 0x10463,
    _carnage_report_string_objectives_4 = 0x10464,
    _carnage_report_string_page_columns = 0x10465,
    _carnage_report_string_player_model = 0x10467,
    _carnage_report_string_campaign_results = 0x10468,
    _carnage_report_string_campaign_carnage = 0x10469,
    _carnage_report_string_campaign_enemies = 0x1046a,
    _carnage_report_string_campaign_vehicles = 0x1046b,
    _carnage_report_string_campaign_score = 0x104ad,
    _carnage_report_string_campaign_deaths = 0x104ae,
    _carnage_report_string_campaign_betrayals = 0x104af,
    _carnage_report_string_campaign_friendlyfire = 0x104b0,
    _carnage_report_string_campaign_kills = 0x104b1,
    _carnage_report_string_campaign_infantry = 0x104b2,
    _carnage_report_string_campaign_leader = 0x104b3,
    _carnage_report_string_campaign_hero = 0x104b4,
    _carnage_report_string_campaign_specialist = 0x104b5,
    _carnage_report_string_campaign_light = 0x104b6,
    _carnage_report_string_campaign_standard = 0x104b7,
    _carnage_report_string_campaign_heavy = 0x104b8,
    _carnage_report_string_campaign_giant = 0x104b9,
    _carnage_report_string_campaign_field_stats = 0x104ba,
    _carnage_report_string_campaign_medals = 0x104bb,
    _carnage_report_string_campaign_style = 0x104bc,
    _carnage_report_string_campaign_kill_percent = 0x104bd,
    _carnage_report_string_campaign_points = 0x104be,
    _carnage_report_string_game_summary = 0x105a1,
    _carnage_report_string_mvp = 0x105a2,
    _carnage_report_string_victor = 0x105a3,
    _carnage_report_string_player_medal = 0x105a6,
    // Datasource, list and widget names (cache string table sets 0 and 1).
    _carnage_report_string_page = 0x10025,
    _carnage_report_string_teams = 0x10017,
    _carnage_report_string_teams_list = 0x10028,
    _carnage_report_string_top_medals = 0x105a4,
    _carnage_report_string_map_image = 0x1032a,
    _carnage_report_string_easy = 0x119,
    _carnage_report_string_normal = 0xad,
    _carnage_report_string_heroic = 0x11a,
    _carnage_report_string_legendary = 0x11b,
    _carnage_report_string_sniper = 0x1ba,
    _carnage_report_string_colors = 0x10009,
    _carnage_report_string_widget_name = 0x10026,
    _carnage_report_string_button_key_default = 0x10048,
    _carnage_report_string_button_key_pgcr_user_saved_film = 0x10055,
    _carnage_report_string_film_reminder = 0x1030f,
    _carnage_report_string_film_reminder_saved = 0x10310,
    _carnage_report_string_difficulty_image = 0x1032d
};

extern "C" const c_game_results *g_game_results_sort_results;

#include "interface/c_gui_screen_widget.h"
#include "interface/gui_data.h"

struct s_player_configuration;

class c_gui_game_results_data : public c_gui_ordered_data
{
public:
    c_gui_game_results_data() : player_count(0), players_sorted(false) {}
    virtual void update();
    virtual void get_column_names(long *const column_names, long *column_count);
    virtual bool get_integer_value(long element_index, long value_name, long *value);
    virtual bool get_text_value(long element_index, long value_name, c_static_wchar_string<1024> *value);
    virtual bool get_player_appearance(long element_index, s_player_appearance *appearance);
    virtual bool get_player_configuration(long element_index, s_player_configuration *configuration);

    long player_indices[16];
    long player_count;
    long sorted_player_indices[16];
    bool players_sorted;

protected:
    virtual long get_current_item_count_internal();
};

class c_gui_game_results_team_data : public c_gui_ordered_data
{
public:
    c_gui_game_results_team_data() : team_count(0) {}
    virtual void get_column_names(long *const column_names, long *column_count);
    virtual void update();
    virtual bool get_integer_value(long element_index, long value_name, long *value);
    virtual bool get_text_value(long element_index, long value_name, c_static_wchar_string<1024> *value);

    long team_indices[16];
    long team_count;

protected:
    virtual long get_current_item_count_internal() { return team_count; }
};

struct s_medal_record
{
    unsigned char player;
    unsigned char medal;
    short count;
};

class c_gui_top_medals_data : public c_gui_ordered_data
{
public:
    c_gui_top_medals_data() : medal_count(0) {}
    virtual bool initialize(long name);
    virtual void get_column_names(long *const column_names, long *column_count);
    virtual bool get_integer_value(long element_index, long value_name, long *value);
    virtual bool get_text_value(long element_index, long value_name, c_static_wchar_string<1024> *value);

    s_medal_record records[3];
    short medal_count;

protected:
    virtual long get_current_item_count_internal();
};

class c_gui_screen_carnage_report : public c_gui_screen_widget {
public:
    c_gui_screen_carnage_report(long name);

    long current_value_name;
    long report_type;
    long player_index;
protected:
    void setup_pages(const c_game_results *results);
public:
    virtual void initialize();
    virtual void initialize_datasource();
    virtual void dispose();
    virtual void update_render_state(unsigned long current_milliseconds);
protected:
    void update_render_state_for_list(c_gui_list_widget *list);
public:
    long get_current_page_name();
    virtual bool handle_list_item_chosen(const c_controller_input_message *message, long list_name,
        c_gui_list_item_widget *list_item, c_gui_data *data);
    virtual bool handle_controller_input_message(const c_controller_input_message *message);
    virtual void update(unsigned long current_milliseconds);
    virtual c_gui_bitmap_widget *create_bitmap_widget(const s_runtime_bitmap_widget_definition *definition);

protected:
    virtual e_render_data_size get_render_data_size();
};

typedef char c_gui_screen_carnage_report_size[(sizeof(void *) != 4 || ((sizeof(c_gui_screen_carnage_report) == 0x1a98))) ? 1 : -1];

#endif // INTERFACE_GUI_SCREENS_CARNAGE_REPORT_GUI_SCREEN_CARNAGE_REPORT_H
