#ifndef INTERFACE_GUI_SCREENS_PLAYER_SELECT_GUI_PLAYER_SELECT_SCREEN_WIDGET_H
#define INTERFACE_GUI_SCREENS_PLAYER_SELECT_GUI_PLAYER_SELECT_SCREEN_WIDGET_H

#include "cseries/cseries_unicode.h"
#include "interface/gui_data.h"
#include "interface/c_gui_screen_widget.h"
#include "game/player_configuration.h"
#include "game/player_types.h"

#include "shell/shell.h"

// Functions defined in tu/interfacelibxbox/gui_player_select_screen_widget.cpp.

class c_controller_input_message;
class c_game_results;
class c_gui_list_item_widget;
class c_gui_ordered_data;
class c_gui_screen_widget;
struct s_player_identifier;
struct s_player_configuration;
class c_player_select_actions_datasource;

// String ids the player select screens name their widgets, datasource columns, actions and dialogs by.
enum e_player_select_string_id
{
    _player_select_string_masterchief = 197,
    _player_select_string_dervish = 198,
    _player_select_string_enabled = 252,
    _player_select_string_mute = 284,
    _player_select_string_unmute = 285,
    _player_select_string_boot_from_squad = 286,
    _player_select_string_abdicate_leadership = 444,
    _player_select_string_difficulty = 0x272,
    _player_select_string_name = 0x29c,
    _player_select_string_image = 0x3f5,
    _player_select_string_count = 0x3f6,
    _player_select_string_medal_image = 0x428,
    _player_select_string_medal_name = 0x429,
    _player_select_string_medal_description = 0x42a,
    _player_select_string_extermination_description = 0x465,
    _player_select_string_perfection_description = 0x466,
    _player_select_string_multiple_kill_2_description = 0x467,
    _player_select_string_multiple_kill_3_description = 0x468,
    _player_select_string_multiple_kill_4_description = 0x469,
    _player_select_string_multiple_kill_5_description = 0x46a,
    _player_select_string_multiple_kill_6_description = 0x46b,
    _player_select_string_multiple_kill_7_description = 0x46c,
    _player_select_string_multiple_kill_8_description = 0x46d,
    _player_select_string_multiple_kill_9_description = 0x46e,
    _player_select_string_multiple_kill_10_description = 0x46f,
    _player_select_string_5_kills_in_a_row_description = 0x470,
    _player_select_string_10_kills_in_a_row_description = 0x471,
    _player_select_string_15_kills_in_a_row_description = 0x472,
    _player_select_string_20_kills_in_a_row_description = 0x473,
    _player_select_string_25_kills_in_a_row_description = 0x474,
    _player_select_string_30_kills_in_a_row_description = 0x475,
    _player_select_string_sniper_kill_5_description = 0x476,
    _player_select_string_sniper_kill_10_description = 0x477,
    _player_select_string_shotgun_kill_5_description = 0x478,
    _player_select_string_shotgun_kill_10_description = 0x479,
    _player_select_string_collision_kill_5_description = 0x47a,
    _player_select_string_collision_kill_10_description = 0x47b,
    _player_select_string_sword_kill_5_description = 0x47c,
    _player_select_string_sword_kill_10_description = 0x47d,
    _player_select_string_juggernaut_kill_5_description = 0x47e,
    _player_select_string_juggernaut_kill_10_description = 0x47f,
    _player_select_string_zombie_kill_human_5_description = 0x480,
    _player_select_string_zombie_kill_human_10_description = 0x481,
    _player_select_string_human_kill_zombie_5_description = 0x482,
    _player_select_string_human_kill_zombie_10_description = 0x483,
    _player_select_string_human_kill_zombie_15_description = 0x484,
    _player_select_string_koth_kill_defence_5_description = 0x485,
    _player_select_string_shotgun_kill_sword_description = 0x486,
    _player_select_string_vehicle_impact_kill_description = 0x487,
    _player_select_string_vehicle_hijack_description = 0x488,
    _player_select_string_aircraft_hijack_description = 0x489,
    _player_select_string_deadplayer_kill_description = 0x48a,
    _player_select_string_player_kill_spreeplayer_description = 0x48b,
    _player_select_string_spartanlaser_kill_description = 0x48c,
    _player_select_string_stickygrenade_kill_description = 0x48d,
    _player_select_string_sniper_kill_description = 0x48e,
    _player_select_string_bashbehind_kill_description = 0x48f,
    _player_select_string_bash_kill_description = 0x490,
    _player_select_string_flame_kill_description = 0x491,
    _player_select_string_driver_assist_gunner_description = 0x492,
    _player_select_string_assault_bomb_planted_description = 0x493,
    _player_select_string_assault_player_kill_carrier_description = 0x494,
    _player_select_string_vip_player_kill_vip_description = 0x495,
    _player_select_string_juggernaut_player_kill_juggernaut_description = 0x496,
    _player_select_string_oddball_carrier_kill_player_description = 0x497,
    _player_select_string_ctf_flag_captured_description = 0x498,
    _player_select_string_ctf_flag_player_kill_carrier_description = 0x499,
    _player_select_string_ctf_flag_carrier_kill_player_description = 0x49a,
    _player_select_string_infection_survive_description = 0x49b,
    _player_select_string_unused1_description = 0x49c,
    _player_select_string_unused2_description = 0x49d,
    _player_select_string_unused3_description = 0x49e,
    _player_select_string_spinner = 0x10006,
    _player_select_string_player_index = 0x10014,
    _player_select_string_gamercard = 0x1001b,
    _player_select_string_service_record = 0x1001c,
    _player_select_string_player_list = 0x10027,
    _player_select_string_next_player = 0x1003d,
    _player_select_string_player_select = 0x100af,
    _player_select_string_carnage_report_player_details = 0x100ba,
    _player_select_string_scoreboard = 0x100bb,
    _player_select_string_medals_list = 0x100c4,
    _player_select_string_implement_image = 0x100c5,
    _player_select_string_implement = 0x100c6,
    _player_select_string_implement_name = 0x100c7,
    _player_select_string_campaign_completion = 0x10103,
    _player_select_string_campaign_completion_none = 0x10104,
    _player_select_string_campaign_completion_partial = 0x10105,
    _player_select_string_campaign_completion_complete = 0x10106,
    _player_select_string_campaign_settings = 0x10107,
    _player_select_string_base_color = 0x10127,
    _player_select_string_achievements = 0x10200,
    _player_select_string_shared_files = 0x10202,
    _player_select_string_caption_column = 0x10291,
    _player_select_string_value_column = 0x10292,
    _player_select_string_game_browser = 0x10334,
    _player_select_string_roster = 0x103a7,
    _player_select_string_player_rank = 0x103a9,
    _player_select_string_player_grade = 0x103aa,
    _player_select_string_base_color_hilite = 0x103b8,
    _player_select_string_experience = 0x103bc,
    _player_select_string_add_friend = 0x103be,
    _player_select_string_submit_review = 0x103bf,
    _player_select_string_service_tag = 0x103cd,
    _player_select_string_status_assembling_summary_ranked_party = 0x10400,
    _player_select_string_timer_next_match = 0x10424,
    _player_select_string_primary_skulls = 0x1046c,
    _player_select_string_primary_skull_image = 0x1046d,
    _player_select_string_primary_skull_state = 0x1046e,
    _player_select_string_secondary_skulls = 0x1046f,
    _player_select_string_secondary_skull_image = 0x10470,
    _player_select_string_secondary_skull_state = 0x10471,
    _player_select_string_skull_status = 0x10472,
    _player_select_string_skull_description = 0x10473,
    _player_select_string_skull_name = 0x10474,
    _player_select_string_main_skull = 0x10475,
    _player_select_string_primary_status = 0x10476,
    _player_select_string_primary_skull_description = 0x10477,
    _player_select_string_primary_skull_name = 0x10478,
    _player_select_string_secondary_status = 0x10479,
    _player_select_string_secondary_skull_description = 0x1047a,
    _player_select_string_secondary_skull_name = 0x1047b,
    _player_select_string_scoring_description = 0x1047c,
    _player_select_string_scoring_name = 0x1047d,
    _player_select_string_scoring_option = 0x1047e,
    _player_select_string_status_on = 0x1047f,
    _player_select_string_status_off = 0x10480,
    _player_select_string_skulls_locked = 0x10481,
    _player_select_string_skulls_locked_title = 0x10482,
    _player_select_string_killed = 0x10597,
    _player_select_string_killed_emblem = 0x10598,
    _player_select_string_killed_image = 0x10599,
    _player_select_string_killed_count = 0x1059a,
    _player_select_string_killed_most_name = 0x1059b,
    _player_select_string_killed_by = 0x1059c,
    _player_select_string_killed_by_emblem = 0x1059d,
    _player_select_string_killed_by_image = 0x1059e,
    _player_select_string_killed_by_count = 0x1059f,
    _player_select_string_killed_most_by_name = 0x105a0,
    _player_select_string_confirm_boot_from_squad = 0x30005,
    _player_select_string_need_to_select_storage_device_to_save = 0x3001e,
    _player_select_string_campaign_changing_setting_will_delete_saved_game = 0x3001f,
};

class c_player_select_medals_datasource : public c_gui_ordered_data
{
public:
    c_player_select_medals_datasource() : m_cell_count(0) {}
    virtual long get_current_item_count_internal();
    virtual void get_column_names(long *const names, long *count);
    virtual bool get_integer_value(long index, long property, long *out_value);
    virtual bool get_string_id_value(long index, long property, long *out_value);

    struct s_medal_cell
    {
        short value_a;
        short value_b;
    };
    s_medal_cell m_cells[58];
    short m_cell_count;
};

// The skull lists the player-select screen offers; m_skull_states: 0 locked, 1 on, 2 off.
class c_player_select_primary_skulls_datasource : public c_gui_ordered_data
{
public:
    c_player_select_primary_skulls_datasource() { update(); }
    virtual void update();
    virtual void get_column_names(long *const names, long *count);
    virtual bool get_integer_value(long index, long property, long *out_value);
    virtual bool get_string_id_value(long index, long property, long *out_value);

protected:
    virtual long get_current_item_count_internal() { return 9; }

public:
    long m_skull_states[9];
};

class c_player_select_secondary_skulls_datasource : public c_gui_ordered_data
{
public:
    c_player_select_secondary_skulls_datasource() { update(); }
    virtual void update();
    virtual void get_column_names(long *const names, long *count);
    virtual bool get_integer_value(long index, long property, long *out_value);
    virtual bool get_string_id_value(long index, long property, long *out_value);

protected:
    virtual long get_current_item_count_internal() { return 4; }

public:
    long m_skull_states[5];
};

typedef char player_select_primary_skulls_datasource_size[(sizeof(void *) != 4 || sizeof(c_player_select_primary_skulls_datasource) == 0x130) ? 1 : -1];
typedef char player_select_secondary_skulls_datasource_size[(sizeof(void *) != 4 || sizeof(c_player_select_secondary_skulls_datasource) == 0x120) ? 1 : -1];

extern "C" void player_select_actions_datasource_rebuild(c_player_select_actions_datasource *datasource);
extern "C" void player_select_actions_datasource_set_target(c_player_select_actions_datasource *datasource, const s_player_identifier *identifier,
    const s_player_configuration *configuration);
extern "C" const long k_medal_description_string_ids[58];

short player_index_from_identifier(class c_game_results const * results, struct s_player_identifier const * identifier);
bool parse_xml_target_player_implement_count(void *context, wchar_t *out, long size);
extern "C" bool parse_xml_target_player_first_player_name(void *context, wchar_t *out, long size);
bool online_user_is_friend(e_controller_index controller_index, unsigned __int64 user_id);

// Skull title and description string ids: [0] titles, [1] descriptions.
extern "C" const long k_primary_skull_string_ids[2][9];
extern "C" const long k_secondary_skull_string_ids[2][5];
extern "C" const long k_medal_name_string_ids[58];

class c_gui_player_select_screen_widget;
struct s_emblem_info;
extern "C" void player_select_update_statistics_display(c_gui_player_select_screen_widget *screen);

class c_gui_player_select_screen_widget : public c_gui_screen_widget
{
public:
    c_gui_player_select_screen_widget(long name);
    virtual void initialize();
    virtual void update(unsigned long current_milliseconds);
    virtual void update_render_state(unsigned long current_milliseconds);
    virtual void initialize_datasource();
    virtual bool handle_list_item_chosen(const c_controller_input_message *message, long list_name,
        c_gui_list_item_widget *list_item_widget, c_gui_data *datasource);
    virtual bool handle_controller_input_message(const c_controller_input_message *message);
    virtual bool handle_dialog_result(const c_dialog_result_message *message);
    virtual bool get_emblem_info_for_widget(c_gui_widget *widget, s_emblem_info *emblem_info);

    friend void ::player_select_update_statistics_display(c_gui_player_select_screen_widget *screen);

private:
    bool update_player_information(short player_index);
    void update_rematch_timer();
public:
    s_player_identifier m_target_player_identifier;
    s_player_identifier m_player_identifier;
    s_player_configuration m_configuration;
    bool m_valid;
    bool m_show_service_record;
    long m_implement_index;
    long m_first_player_index;
    unsigned short m_first_player_count;
    long m_second_player_index;
    unsigned short m_second_player_count;
};

typedef char gui_player_select_screen_widget_size[(sizeof(void *) != 4 || sizeof(c_gui_player_select_screen_widget) == 7104) ? 1 : -1];


#endif // INTERFACE_GUI_SCREENS_PLAYER_SELECT_GUI_PLAYER_SELECT_SCREEN_WIDGET_H
