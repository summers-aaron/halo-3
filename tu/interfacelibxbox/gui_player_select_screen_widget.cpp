// interfacelibxbox/gui_player_select_screen_widget.obj: from the records 0x198168 0x3c00d0 0x4c1fe0 0x4c2080 0x4c2118 0x4c2190 0x4c2278 0x4c2308 0x4c2348 0x4c23b0 0x4c24a0 0x4c2560 0x4c2998 0x4c2a30 0x4c2af8 0x4c2dc0 0x4c30c0 0x4cc938 0x4cca30 0x4ccad0 0x4ccad8 0x4ccb70 0x4ccbe8 0x4cd2c0 0x4cd300 0x4cd358 0x4cd368 0x4cd4b0 0x4cd518 0x4cd580 0x4cd628 0x4cd6b8 0x4cdd28 0x4ce330 0x4ce4b8 0x4ce890 0x4ce990 0x4ceb40 0x4ced48 0x4cf410 0x4cf4f8 0x4cfca0 0x4cfdd8 0x4cff28 0x4cff88 0x4d0010 0x4d00a0 0x4d0138 0x4d01d8 0x4d0278 0x4d0310 0x4d03b0 0x4d0450 0x4d04f0 0x4d05c8 0x4d06a0 0x4d07b0 0x4d08c8 0x4d09c8 0x4d0aa0 0x4d0b90 0x4d0c68 0x4d0ce0 0x4d0db8 0x4d0e30 0x50a6c8
#include "bitmaps/bitmap_group_definitions.h"
// @address 0xf283c8 g_network_session_interface_local_users
// @address 0x11401e8 g_controller_globals
#include "interface/gui_screens/player_select/player_select_screen_message.h"
#include "interface/c_gui_bitmap_widget.h"
#include "interface/gui_screen_widget_legacy.h"
#include "memory/tls_globals.h"
#include "game/game_globals_storage.h"
#include "networking/session/network_session_interface_types.h"
#include "networking/logic/life_cycle/life_cycle_matchmaking_progress.h"
#include "interface/user_interface_matchmaking_progress.h"
#include "interface/gui_screen_construction.h"
#include <stdio.h>
// @address 0x187780 _vsnwprintf_s
#include "game/game_results_storage_types.h"
// @flags /TP /O2 /J /Oc
// @address 0x4cd580 ?get_string_id_value@c_player_select_medals_datasource@@UAA_NJJPAJ@Z
// @address 0x4cd358 ?get_current_item_count_internal@c_player_select_medals_datasource@@UAAJXZ
// @address 0x4ccbe8 player_select_actions_datasource_rebuild
// @address 0x4ccb70 player_select_actions_datasource_set_target
#include "interface/gui_player_select_dependencies.h"
#include "interface/gui_screen_widget_text.h"
#include "interface/gui_widget_hierarchy.h"
#include "game/game_results.h"
#include "objects/damage_reporting.h"
#include "cseries/long_sort.h"
#include "memory/tls_data_arrays.h"
#include "cache/cache_files.h"
#include "text/text_group.h"
#include <stddef.h>
#include "memory/data_types.h"
#include "tag_files/tag_types.h"
#include "shell/shell.h"
#include "game/player_types.h"
#include "interface/gui_widget_types.h"
#include "memory/static_arrays.h"
#include "interface/c_gui_list_widget.h"
#include "interface/gui_widget.h"
#include "interface/user_interface_shared_globals.h"
#include "interface/user_interface_sound_effect_types.h"
// @address 0x8a3c0 ?ustrnzcpy@@YAPA_WPA_WPB_WJ@Z
// @address 0x187710 ?print@?$c_static_wchar_string@$0BAA@@@QAAPB_WPB_WZZ
// @address 0x1bfe40 ?set@?$c_static_wchar_string@$0BAA@@@QAAXPB_W@Z
// @address 0x483fa8 ?user_interface_get_player_rank_string@@YA_NW4e_experience_rank@@W4e_experience_grade@@PAV?$c_static_wchar_string@$0EAA@@@@Z
// @address 0x484068 ?user_interface_get_player_grade_string@@YA_NW4e_experience_rank@@W4e_experience_grade@@_N2PAV?$c_static_wchar_string@$0EAA@@@@Z
// @address 0x4797a8 ?user_interface_networking_get_final_game_results@@YAPBVc_game_results@@XZ
// @address 0x46c9d0 ?user_interface_session_get_player_data@@YAPBUs_player_configuration@@J@Z
// @address 0x46db58 ?user_interface_session_player_get_player_index@@YAJPBUs_player_identifier@@@Z
// @address 0x48a580 ?update_render_state@c_gui_widget@@UAAXK@Z
// @address 0x1e9d80 ?saved_game_files_get_personal_storage_content_item@@YAJW4e_controller_index@@_N@Z
// @address 0x39e008 ?online_guide_show_gamer_card_ui@@YAKW4e_controller_index@@_K@Z
// @address 0x3b21e0 network_squad_session_delegate_leadership
// @address 0x3c0408 ?voice_set_mute@@YAXJJ_N@Z
// @address 0x3c05a8 ?voice_get_local_mute_through_guide@@YA_NJJ@Z
// @address 0x489948 ?get_any_responding_controller@c_gui_widget@@QBA?AW4e_controller_index@@XZ
// @address 0x494790 ?post_error_with_custom_message@c_gui_error_manager@@QAAXJPB_W0W4e_controller_index@@_N2@Z
// @address 0x8af98 ?is_signed_in_to_live@c_controller_interface@@QBA_NXZ
// @address 0x1b7030 ?in_use@c_controller_interface@@QBA_NXZ
// @address 0x8b050 ?is_silver_or_gold_live@c_controller_interface@@QBA_NXZ
// @address 0x807a0 ?game_is_cooperative@@YA_NXZ
// @address 0x3c04b0 ?voice_get_mute@@YA_NJJ@Z
// @address 0x46b698 ?user_interface_squad_is_booting_allowed@@YA_NXZ
// @address 0x46c470 ?user_interface_session_is_player_valid@@YA_NJ@Z
// @address 0x46dbb8 ?user_interface_interactive_session_is_group@@YA_NXZ
// @address 0x4cfeb0 online_guide_update_get_service_record
// @address 0x59ffa0 XShowPlayerReviewUI
// @address 0x59ffc8 XShowFriendRequestUI
#include "cseries/crt_xdk.h"
#include <string.h>
#include <stdarg.h>
#include <stdlib.h>
#include <ppcintrinsics.h>
#include "ai/actor_datum.h"
#include "game/game.h"
#include "interface/gui_screens/scoreboard/gui_screen_scoreboard.h"
#include "interface/user_interface.h"
#include "interface/user_interface_memory.h"
#include "interface/user_interface_session.h"
#include "networking/logic/network_session_interface.h"
#include "text/unicode.h"
#include "ai/ai_script.h"
#include "interface/user_interface_networking.h"
#include "interface/user_interface_utilities.h"
#include "networking/logic/network_life_cycle.h"
#include "networking/online/online_xenon.h"
#include "networking/online/voice_xenon.h"
#include "sound/sound_classes.h"
#include "interface/c_controller.h"
#include "saved_games/saved_game_files.h"
#include "ai/ai_mission_dialogue.h"
#include "memory/data.h"
#include "interface/gui_screens/player_select/gui_player_select_screen_widget.h"
#include "saved_games/content_catalogue.h"
#include "interface/game_tag_parser.h"
#include "interface/user_interface_error_manager.h"
#include "interface/controller_types.h"
#include "interface/gui_data.h"
#include "interface/gui_roster_data.h"
#include "interface/c_gui_list_item_widget.h"
#include "interface/experience_rank_types.h"
#include "interface/experience_grade_types.h"
#include "interface/ui_allocation_marker_types.h"
#include "networking/online/online_guide_xenon.h"
#include "interface/user_interface_allocation.h"
#include "ai/squad_iterator_types.h"
#include "sound/sound_class_definitions.h"

class c_message;
class c_game_results;

// @address 0x486578 ?get_render_window@c_gui_screen_widget@@QAA?AW4e_window_index@@XZ
// @address 0x4869e8 ?set_text_widget_string_id@c_gui_screen_widget@@QAAXJJ@Z
// @address 0x486f08 ?add_game_tag_parser@c_gui_screen_widget@@IAAXPAVc_game_tag_parser@@@Z
// @address 0x48b648 ?get_parent_screen@c_gui_widget@@QAAPAVc_gui_screen_widget@@XZ
// @address 0x483240 ??0c_load_screen_message@@QAA@JW4e_controller_index@@W4e_window_index@@J@Z
#include "interface/c_gui_screen_widget.h"
#include "interface/user_interface_messages.h"
#include "networking/logic/network_session_game_setup.h"
#include "game/game_state_accessors.h"
#include "tag_files/tag_groups.h"
#include <new>
#include "interface/gui_screens/campaign/gui_screen_campaign_options.h"

class c_font_cache;

class c_player_select_actions_datasource : public c_gui_ordered_data {
public:
    c_player_select_actions_datasource(e_controller_index controller_index, const s_player_identifier *player);
    virtual void get_column_names(long *const column_names, long *column_count);
    virtual bool get_string_id_value(long index, long property, long *out_value);
protected:
    virtual long get_current_item_count_internal(void);
private:
    friend void ::player_select_actions_datasource_rebuild(c_player_select_actions_datasource *datasource);
    friend void ::player_select_actions_datasource_set_target(c_player_select_actions_datasource *datasource, const s_player_identifier *identifier,
        const s_player_configuration *configuration);
    __forceinline void add_action(long action, bool available)
    {
        if (available && m_item_count < 8)
        {
            m_item_ids[m_item_count] = action;
            m_item_count++;
        }
    }
    e_controller_index m_controller_index;
    s_player_identifier m_player;
    s_player_identifier m_target_player;
    s_player_configuration m_target_configuration;
    long m_item_ids[8];
    long m_item_count;
};

typedef char player_select_actions_datasource_size[(sizeof(void *) != 4 || ((sizeof(c_player_select_actions_datasource) == 0x258))) ? 1 : -1];

struct s_medal { short first; short second; };

class c_player_select_focus_item : public c_gui_widget
{
public:
};

__forceinline void c_gui_player_select_screen_widget::update_rematch_timer()
{
    bool show_rematch_timer = false;
    if (network_life_cycle_in_matchmaking())
    {
        s_life_cycle_matchmaking_progress progress;
        network_life_cycle_get_matchmaking_progress(&progress);
        g_user_interface_matchmaking_progress = progress;
        if (progress.type == 5)
            show_rematch_timer = progress.countdown_started;
    }
    c_gui_widget *timer = get_child_widget(_gui_text_widget, _player_select_string_timer_next_match);
    if (timer)
        timer->set_visible(show_rematch_timer);
}

static __forceinline void gui_player_select_screen_widget_set_child_visible(c_gui_widget *widget, e_gui_widget_type type, long name, bool visible)
{
    c_gui_widget *child = widget->get_child_widget(type, name);
    if (child)
        child->set_visible(visible);
}


// @address 0x3c00d0 ?online_user_is_friend@@YA_NW4e_controller_index@@_K@Z

// @address 0x5a01b0 XUserAreUsersFriends




bool online_user_is_friend(e_controller_index controller_index, unsigned __int64 user_id)
{
    int is_friend;
    unsigned __int64 xuid;
    bool result;
    xuid = user_id;
    result = false;
    is_friend = 0;
    if (XUserAreUsersFriends(controller_index, &xuid, 1, &is_friend, 0) == 0)
        result = is_friend != 0;
    return result;
}


// @address 0x4c1fe0 ?update@c_player_select_primary_skulls_datasource@@UAAXXZ
void c_player_select_primary_skulls_datasource::update()
{
    long first_mask = (long)controller_get_signed_in_mask1();
    long second_mask = network_life_cycle_session_get_primary_skulls();
    for (short index = 0; index < 9; index++)
    {
        long bit = 1 << index;
        if ((bit & first_mask) != 0)
            m_skull_states[index] = ((bit & second_mask) == 0) + 1;
        else
            m_skull_states[index] = 0;
    }
}

// @address 0x4c2080 ?get_column_names@c_player_select_primary_skulls_datasource@@UAAXQAJPAJ@Z
void c_player_select_primary_skulls_datasource::get_column_names(long *const names, long *count)
{
    *count = 0;
    names[0] = _player_select_string_primary_skull_image;
    names[++*count] = _player_select_string_primary_skull_state;
    names[++*count] = _player_select_string_skull_status;
    names[++*count] = _player_select_string_skull_description;
    names[++*count] = _player_select_string_skull_name;
    ++*count;
}

// @address 0x4c2118 ?get_integer_value@c_player_select_primary_skulls_datasource@@UAA_NJJPAJ@Z
bool c_player_select_primary_skulls_datasource::get_integer_value(long index, long property, long *out_value)
{
    bool result = false;
    switch (property)
    {
    case _player_select_string_primary_skull_image:
        if (index >= 0 && index < 9)
        {
            if (m_skull_states[index] == 0)
                *out_value = 0;
            else
                *out_value = index + 1;
            result = true;
        }
        break;
    case _player_select_string_primary_skull_state:
        if (index >= 0 && index < 9)
        {
            *out_value = m_skull_states[index];
            result = true;
        }
        break;
    }
    return result;
}

// @address 0x245fc k_primary_skull_string_ids
#if !defined(_XBOX) || defined(__clang__)
const long k_primary_skull_string_ids[2][9] = {
    { 0x10483, 0x10484, 0x10485, 0x10486, 0x10487, 0x10488, 0x10489, 0x1048a, 0x1048b },
    { 0x10491, 0x10492, 0x10493, 0x10494, 0x10495, 0x10496, 0x10497, 0x10498, 0x10499 },
};
#endif

// @address 0x4c2190 ?get_string_id_value@c_player_select_primary_skulls_datasource@@UAA_NJJPAJ@Z
bool c_player_select_primary_skulls_datasource::get_string_id_value(long index, long property, long *out_value)
{
    bool result = false;
    long slot = index;
    if ((unsigned long)slot <= 8) {
        long value = m_skull_states[slot];
        switch (property) {
        case _player_select_string_skull_status:
            switch ((unsigned long)value) {
            case 0: *out_value = _player_select_string_skulls_locked_title; break;
            case 1: *out_value = _player_select_string_status_on; break;
            case 2: *out_value = _player_select_string_status_off; break;
            }
            break;
        case _player_select_string_skull_description:
            if (value == 0) *out_value = _player_select_string_skulls_locked;
            else *out_value = k_primary_skull_string_ids[1][slot];
            break;
        case _player_select_string_skull_name:
            if (value == 0) *out_value = _player_select_string_skulls_locked_title;
            else *out_value = k_primary_skull_string_ids[0][slot];
            break;
        }
        result = true;
    }
    return result;
}

// @address 0x4c2278 ?update@c_player_select_secondary_skulls_datasource@@UAAXXZ
void c_player_select_secondary_skulls_datasource::update()
{
    long controllers = controller_get_signed_in_mask();
    long precache_status = network_life_cycle_session_get_global_map_precache_status();
    for (short i = 0; i < 5; i++)
    {
        if ((controllers & (1 << i)) != 0)
            m_skull_states[i] = ((precache_status & (1 << i)) == 0) + 1;
        else
            m_skull_states[i] = 0;
    }
}

// @address 0x4c2308 ?get_column_names@c_player_select_secondary_skulls_datasource@@UAAXQAJPAJ@Z
void c_player_select_secondary_skulls_datasource::get_column_names(long *const names, long *count)
{
    *count = 0;
    names[0] = _player_select_string_secondary_skull_image;
    names[++*count] = _player_select_string_secondary_skull_state;
    ++*count;
}


// @address 0x4c2348 ?get_integer_value@c_player_select_secondary_skulls_datasource@@UAA_NJJPAJ@Z
bool c_player_select_secondary_skulls_datasource::get_integer_value(long index, long property, long *out_value)
{
    bool result = false;
    long slot = index + 1;
    switch (property)
    {
    case _player_select_string_secondary_skull_image:
        if ((unsigned long)slot <= 4)
        {
            if (m_skull_states[slot] == 0)
                *out_value = 0;
            else
                *out_value = slot;
            result = true;
        }
        break;
    case _player_select_string_secondary_skull_state:
        if ((unsigned long)slot <= 4)
        {
            *out_value = m_skull_states[slot];
            result = true;
        }
        break;
    }
    return result;
}
// @address 0x24644 k_secondary_skull_string_ids
#if !defined(_XBOX) || defined(__clang__)
const long k_secondary_skull_string_ids[2][5] = {
    { 0x1048c, 0x1048d, 0x1048e, 0x1048f, 0x10490 },
    { 0x1049a, 0x1049b, 0x1049c, 0x1049d, 0x1049e },
};
#endif

// @address 0x4c23b0 ?get_string_id_value@c_player_select_secondary_skulls_datasource@@UAA_NJJPAJ@Z
bool c_player_select_secondary_skulls_datasource::get_string_id_value(long index, long property, long *out_value)
{
    bool result = false;
    long slot = index + 1;
    if ((unsigned long)slot <= 4) {
        long value = m_skull_states[slot];
        switch (property) {
        case _player_select_string_skull_status:
            switch ((unsigned long)value) {
            case 0: *out_value = _player_select_string_skulls_locked_title; break;
            case 1: *out_value = _player_select_string_status_on; break;
            case 2: *out_value = _player_select_string_status_off; break;
            }
            break;
        case _player_select_string_skull_description:
            if (value == 0) *out_value = _player_select_string_skulls_locked;
            else *out_value = k_secondary_skull_string_ids[1][slot];
            break;
        case _player_select_string_skull_name:
            if (value == 0) *out_value = _player_select_string_skulls_locked_title;
            else *out_value = k_secondary_skull_string_ids[0][slot];
            break;
        }
        result = true;
    }
    return result;
}



// @address 0x4b4128 ?get_column_names@c_player_select_actions_datasource@@UAAXQAJPAJ@Z
void c_player_select_actions_datasource::get_column_names(long *const column_names, long *column_count)
{
    *column_count = 0;
    column_names[*column_count] = _player_select_string_name;
    (*column_count)++;
}

// @address 0x4ccad0 ?get_current_item_count_internal@c_player_select_actions_datasource@@MAAJXZ
long c_player_select_actions_datasource::get_current_item_count_internal(void)
{
    return m_item_count;
}

// @address 0x4cd2c0 ?get_string_id_value@c_player_select_actions_datasource@@UAA_NJJPAJ@Z
bool c_player_select_actions_datasource::get_string_id_value(long index, long property, long *out_value)
{
    bool result = false;
    if (index >= 0 && index < m_item_count) {
        if (property == _player_select_string_name) {
            *out_value = m_item_ids[index];
            result = true;
        }
    }
    return result;
}


// @address 0x4cd4b0 ?get_column_names@c_player_select_medals_datasource@@UAAXQAJPAJ@Z
void c_player_select_medals_datasource::get_column_names(long *const names, long *count)
{
    *count = 0;
    names[0] = _player_select_string_count;
    names[++*count] = _player_select_string_image;
    names[++*count] = _player_select_string_medal_name;
    names[++*count] = _player_select_string_medal_description;
    ++*count;
}

// @address 0x4cd518 ?get_integer_value@c_player_select_medals_datasource@@UAA_NJJPAJ@Z
bool c_player_select_medals_datasource::get_integer_value(long index, long property, long *out_value)
{
    bool result = false;
    if (index >= 0 && index < m_cell_count) {
        if (property != _player_select_string_image) {
            if (property == _player_select_string_count) {
                *out_value = m_cells[index].value_b;
                result = true;
            }
        } else {
            *out_value = m_cells[index].value_a;
            result = true;
        }
    }
    return result;
}



// @address 0x4c2a30 player_select_test_and_show_load_dialog
extern "C" bool player_select_test_and_show_load_dialog(c_gui_screen_widget *screen, e_controller_index controller)
{
    bool has_content = false;
    if (saved_game_files_get_personal_storage_content_item(controller, false) != -1)
        has_content = true;
    bool result = has_content && user_interface_squad_get_player_count() == 1;
    if (result)
    {
        void *memory = user_interface_malloc_tracked(sizeof(c_load_dialog_screen_message), 0, 0);
        if (memory != 0)
        {
            int layered_position = screen->get_name();
            c_load_dialog_screen_message *message = new (memory) c_load_dialog_screen_message(
                controller, screen->get_render_window(), layered_position,
                _player_select_string_campaign_changing_setting_will_delete_saved_game, layered_position);
            if (message != 0)
            {
                message->set_parent_screen_index(screen->get_screen_index());
                ::user_interface_messaging_post(message);
            }
        }
    }
    return result;
}


// @address 0x4cd300 medal_compare
bool medal_compare(s_medal a, s_medal b, void *context)
{
    const s_medal *x = &a;
    const s_medal *y = &b;
    if (x->second > y->second)
        return false;
    if (x->second < y->second)
        return true;
    if (x->first < y->first)
        return false;
    if (x->first > y->first)
        return true;
    return false;
}






// @address 0x4cff88 ?player_index_from_identifier@@YAFPBVc_game_results@@PBUs_player_identifier@@@Z
short player_index_from_identifier(class c_game_results const * results, struct s_player_identifier const * identifier)
{
    short result = -1;
    for (short i = 0; i < 16; i++)
    {
        const s_game_results_player_data *p = &results->players[i];
        if (p->player_exists && player_identifier_compare(identifier, (const s_player_identifier *)&p->identifier))
        {
            result = i;
            break;
        }
    }
    return result;
}


// @address 0x4cca30 ?apply_initial_state@c_load_player_select_screen_message@@UBAXPAVc_gui_screen_widget@@@Z
void c_load_player_select_screen_message::apply_initial_state(c_gui_screen_widget *screen_widget) const
{
    c_gui_player_select_screen_widget *screen = static_cast<c_gui_player_select_screen_widget *>(screen_widget);
    screen->m_target_player_identifier = m_target_player_identifier;
    screen->m_player_identifier = m_player_identifier;
    screen->m_configuration = m_configuration;
    screen->m_valid = m_valid;
    screen->m_show_service_record = m_show_service_record;
}

long c_player_select_medals_datasource::get_current_item_count_internal()
{
    return m_cell_count;
}

// @address 0xa33f80 ?g_current_game_results@@3Vc_game_results@@A

static __forceinline const c_game_results *gui_player_select_screen_widget_final_game_results()
{
    const c_game_results *results = 0;
    bool finalized = g_current_game_results.initialized && g_current_game_results.finalized;
    if (finalized && g_current_game_results.finish_valid)
        results = &g_current_game_results;
    return results;
}

// Populates the medal cells for one player from the final game results.
// @address 0x4cd368 player_select_medals_datasource_set_player
extern "C" void player_select_medals_datasource_set_player(c_player_select_medals_datasource *datasource, s_player_identifier identifier)
{
    const c_game_results *results = gui_player_select_screen_widget_final_game_results();
    datasource->m_cell_count = 0;
    if (results)
    {
        short player_index = ::player_index_from_identifier(results, &identifier);
        if (player_index != -1)
        {
            const s_game_results_player_statistics *statistics = &results->statistics.player[player_index];
            for (short medal_index = 0; medal_index < 58; ++medal_index)
            {
                datasource->m_cells[medal_index].value_a = medal_index;
                datasource->m_cells[medal_index].value_b = statistics->medals[medal_index].value;
            }
            templated_sort<long>::qsort((long *)datasource->m_cells, 58,
                (templated_sort<long>::t_compare)medal_compare, 0);
            for (short medal_index = 0; medal_index < 58; ++medal_index)
            {
                if (datasource->m_cells[medal_index].value_b <= 0)
                    break;
                ++datasource->m_cell_count;
            }
        }
    }
}

bool c_player_select_medals_datasource::get_string_id_value(long index, long property, long *out_value)
{
    bool result = false;
    if (index >= 0 && index < m_cell_count) {
        if (property != _player_select_string_medal_name) {
            if (property == _player_select_string_medal_description) {
                short value = m_cells[index].value_a;
                if (value >= 0 && value < 58) {
                    *out_value = k_medal_description_string_ids[value];
                    result = true;
                }
            }
        } else {
            short value = m_cells[index].value_a;
            if (value >= 0 && value < 58) {
                *out_value = k_medal_name_string_ids[value];
                result = true;
            }
        }
    }
    return result;
}


static __forceinline void sort_datums(int *d, unsigned int n)
{
    if (n >= 2)
        templated_sort<long>::qsort((long *)d, n, (templated_sort<long>::t_compare)dialogue_compare_player_weighting, 0);
}

// @address 0x50a6c8 ?ai_play_line@@YAFJJ@Z
short ai_play_line(long ai_index, long dialogue_line)
{
    float zero = 0.0f;
    float best = zero;
    if (dialogue_line != -1) {
        const sound_class_definition *limits = sound_class_get(47);
        short limit = 4;
        short hits = 0;
        short count;
        short i;
        ai_index_actor_iterator iterator;
        actor_datum *found;
        int datums[30];
        if (limits && *(const short *)limits > 0) limit = *(const short *)limits;
        count = 0;
        ai_index_actor_iterator_new(ai_index, &iterator);
        found = ai_index_actor_iterator_next(&iterator);
        while (found && count < 30) {
            if (found->unit_index != -1) {
                datums[count] = iterator.actor_index;
                count++;
            }
            found = ai_index_actor_iterator_next(&iterator);
        }
        if (count > limit)
            sort_datums(datums, count);
        for (i = 0; i < count; i++) {
            float value;
            mission_dialogue_play_line(datums[i], -1, 0, dialogue_line, &value);
            if (value > zero) {
                if (value > best) best = value;
                hits++;
                if (hits >= limit) break;
            }
        }
    }
    return (short)(int)(best * 30.0f + 0.5f);
}


// @address 0x198168 ?append_print@?$c_static_wchar_string@$0BAA@@@QAAPB_WPB_WZZ
template<long k_maximum_count>
const wchar_t *c_static_wchar_string<k_maximum_count>::append_print(const wchar_t *format, ...)
{
    int length = this->length();
    int size = k_maximum_count - length;
    wchar_t *end = m_string + length;
    va_list args;
    va_start(args, format);
    _vsnwprintf_s(end, size - 1, (unsigned int)-1, format, args);
    end[size - 1] = 0;
    return m_string;
}
template const wchar_t *c_static_wchar_string<256>::append_print(const wchar_t *format, ...);









// Looks up a player configuration by identifier in the session, game results, or player data.
// @address 0x4ce890 player_select_find_player_configuration
extern "C" const s_player_configuration *player_select_find_player_configuration(const s_player_identifier *identifier)
{
    const s_player_configuration *configuration = 0;
    if (identifier)
    {
        long player_index = user_interface_session_player_get_player_index(identifier);
        if (player_index != -1)
            configuration = user_interface_session_get_player_data(player_index);
        if (!configuration)
        {
            const c_game_results *results = user_interface_networking_get_final_game_results();
            if (results)
            {
                short result_player_index = ::player_index_from_identifier(results, identifier);
                if (result_player_index != -1)
                    configuration = (const s_player_configuration *)&results->players[result_player_index].configuration;
            }
        }
        if (!configuration)
        {
            c_data_iterator<player_datum> players;
            players.begin(player_data);
            while (players.next())
            {
                if (player_identifier_compare(&players.m_datum->player_identifier, identifier))
                {
                    configuration = &players.m_datum->configuration;
                    break;
                }
            }
        }
    }
    return configuration;
}


// @address 0x4c2dc0 player_select_set_focused_list_state
extern "C" void player_select_set_focused_list_state(c_gui_screen_widget *screen, long state, short player_index)
{
    c_gui_widget *select_list = ((c_gui_widget *)screen)->get_child_widget(_gui_list_widget, _player_select_string_campaign_settings);
    c_gui_widget *team_list = ((c_gui_widget *)screen)->get_child_widget(_gui_list_widget, _player_select_string_primary_skulls);
    c_gui_widget *other_list = ((c_gui_widget *)screen)->get_child_widget(_gui_list_widget, _player_select_string_secondary_skulls);
    c_gui_widget *second_single = ((c_gui_widget *)screen)->get_child_widget(_gui_text_widget, _player_select_string_primary_status);
    c_gui_widget *second_team = ((c_gui_widget *)screen)->get_child_widget(_gui_text_widget, _player_select_string_primary_skull_description);
    c_gui_widget *second_other = ((c_gui_widget *)screen)->get_child_widget(_gui_text_widget, _player_select_string_primary_skull_name);
    c_gui_widget *second_team_icon = ((c_gui_widget *)screen)->get_child_widget(_gui_bitmap_widget, _player_select_string_primary_skulls);
    c_gui_widget *third_single = ((c_gui_widget *)screen)->get_child_widget(_gui_text_widget, _player_select_string_secondary_status);
    c_gui_widget *third_team = ((c_gui_widget *)screen)->get_child_widget(_gui_text_widget, _player_select_string_secondary_skull_description);
    c_gui_widget *third_other = ((c_gui_widget *)screen)->get_child_widget(_gui_text_widget, _player_select_string_secondary_skull_name);
    c_gui_widget *third_other_icon = ((c_gui_widget *)screen)->get_child_widget(_gui_bitmap_widget, _player_select_string_secondary_skulls);
    c_gui_widget *any_other_icon = ((c_gui_widget *)screen)->get_child_widget(_gui_bitmap_widget, _player_select_string_main_skull);
    c_gui_widget *first_line = ((c_gui_widget *)screen)->get_child_widget(_gui_text_widget, _player_select_string_scoring_description);
    c_gui_widget *second_line = ((c_gui_widget *)screen)->get_child_widget(_gui_text_widget, _player_select_string_scoring_name);
    bool second = false;
    bool third = false;
    c_gui_widget *focus_list = 0;
    switch (state)
    {
    case 1:
        third = false;
        focus_list = select_list;
        second = false;
        break;
    case 2:
        second = true;
        third = false;
        focus_list = team_list;
        break;
    case 3:
        third = true;
        focus_list = other_list;
        second = false;
        break;
    }
    if (focus_list)
        ((c_gui_screen_widget *)(c_gui_widget *)screen)->transfer_focus_to_list((c_gui_list_widget *)focus_list, player_index, false, false);
    if (second_single)
        second_single->set_visible(second);
    if (second_team)
        second_team->set_visible(second);
    if (second_other)
        second_other->set_visible(second);
    if (second_team_icon)
        second_team_icon->set_visible(second);
    if (third_single)
        third_single->set_visible(third);
    if (third_team)
        third_team->set_visible(third);
    if (third_other)
        third_other->set_visible(third);
    if (third_other_icon)
        third_other_icon->set_visible(third);
    if (any_other_icon)
        any_other_icon->set_visible(second || third);
    if (first_line)
        first_line->set_visible(!second && !third);
    if (second_line)
        second_line->set_visible(!second && !third);
}






// @address 0x484f88 ?user_interface_free@@YAXPBX@Z
// @address 0x537f0 ??_7c_player_select_primary_skulls_datasource@@6B@
// @address 0x4c0ee0 ??_Gc_player_select_primary_skulls_datasource@@UAAPAXI@Z
// @address 0x4c0ee0 ??_Ec_player_select_primary_skulls_datasource@@UAAPAXI@Z
// @address 0x42e810 ?get_current_item_count_internal@c_player_select_primary_skulls_datasource@@MAAJXZ
// @address 0x53860 ??_7c_player_select_secondary_skulls_datasource@@6B@
// @address 0x4c0ee0 ??_Gc_player_select_secondary_skulls_datasource@@UAAPAXI@Z
// @address 0x4c0ee0 ??_Ec_player_select_secondary_skulls_datasource@@UAAPAXI@Z
// @address 0x3fc550 ?get_current_item_count_internal@c_player_select_secondary_skulls_datasource@@MAAJXZ

template<class t_type> static __forceinline void gui_player_select_screen_widget_track_delete(t_type *data)
{
    data->~t_type();
    user_interface_free(data);
}

// @address 0x4c2560 ?initialize_datasource@c_gui_screen_campaign_settings@@UAAXXZ
void c_gui_screen_campaign_settings::initialize_datasource()
{
    c_gui_screen_widget::initialize_datasource();
    void *primary_memory = user_interface_malloc_tracked(sizeof(c_player_select_primary_skulls_datasource), 0, 0);
    if (primary_memory)
    {
        c_player_select_primary_skulls_datasource *primary = new (primary_memory) c_player_select_primary_skulls_datasource();
        if (primary->initialize(_player_select_string_primary_skulls))
        {
            m_datasource[m_datasource_count] = primary;
            m_datasource_count++;
        }
        else
        {
            gui_player_select_screen_widget_track_delete(primary);
        }
    }
    void *secondary_memory = user_interface_malloc_tracked(sizeof(c_player_select_secondary_skulls_datasource), 0, 0);
    if (secondary_memory)
    {
        c_player_select_secondary_skulls_datasource *secondary = new (secondary_memory) c_player_select_secondary_skulls_datasource();
        if (secondary->initialize(_player_select_string_secondary_skulls))
        {
            m_datasource[m_datasource_count] = secondary;
            m_datasource_count++;
        }
        else
        {
            gui_player_select_screen_widget_track_delete(secondary);
        }
    }
}



// @address 0x4ceb40 ?update_render_state@c_gui_player_select_screen_widget@@UAAXK@Z
void c_gui_player_select_screen_widget::update_render_state(unsigned long current_milliseconds)
{
    c_gui_widget::update_render_state(current_milliseconds);
    if (m_valid)
    {
        const s_player_configuration *configuration = &m_configuration;
        const c_game_results *results = user_interface_networking_get_final_game_results();
        if (configuration)
        {
            long color = configuration->client.appearance.primary_color;
            tint_widget_to_change_color(get_child_widget(_gui_bitmap_widget, _player_select_string_base_color), color, false);
            color = configuration->client.appearance.primary_color;
            tint_widget_to_change_color(get_child_widget(_gui_text_widget, _player_select_string_name), color, false);
            color = configuration->client.appearance.primary_color;
            tint_widget_to_change_color(get_child_widget(_gui_text_widget, _player_select_string_service_tag), color, false);
            color = configuration->client.appearance.primary_color;
            tint_widget_to_change_color(get_child_widget(_gui_text_widget, _player_select_string_player_rank), color, false);
            color = configuration->client.appearance.primary_color;
            tint_widget_to_change_color(get_child_widget(_gui_text_widget, _player_select_string_player_grade), color, false);
        }
        if (results)
        {
            c_gui_widget *first = get_child_widget(_gui_bitmap_widget, _player_select_string_killed_image);
            c_gui_widget *second = get_child_widget(_gui_bitmap_widget, _player_select_string_killed_by_image);
            c_gui_widget *first_background = get_child_widget(_gui_bitmap_widget, _player_select_string_killed_emblem);
            c_gui_widget *second_background = get_child_widget(_gui_bitmap_widget, _player_select_string_killed_by_emblem);
            if (first && m_first_player_index != -1)
            {
                const s_player_configuration *player = &results->players[m_first_player_index].configuration;
                tint_widget_to_change_color(first, player->client.appearance.primary_color, false);
                if (first_background)
                    tint_widget_to_change_color(first_background, player->client.appearance.primary_color, false);
            }
            if (second && m_second_player_index != -1)
            {
                const s_player_configuration *player = &results->players[m_second_player_index].configuration;
                tint_widget_to_change_color(second, player->client.appearance.primary_color, false);
                if (second_background)
                    tint_widget_to_change_color(second_background, player->client.appearance.primary_color, false);
            }
        }
    }
}

// @address 0x4cfdd8 ?get_emblem_info_for_widget@c_gui_player_select_screen_widget@@UAA_NPAVc_gui_widget@@PAUs_emblem_info@@@Z
bool c_gui_player_select_screen_widget::get_emblem_info_for_widget(c_gui_widget *widget, s_emblem_info *emblem_info)
{
    bool result = false;
    if ((_rotl(static_cast<c_gui_bitmap_widget *>(widget)->m_definition.flags, 27) & 1) != 0)
    {
        const c_game_results *results = user_interface_networking_get_final_game_results();
        const s_player_configuration *configuration = 0;
        switch (widget->name)
        {
        case _player_select_string_killed_emblem:
            if (results && m_first_player_index != -1)
                configuration = &results->players[m_first_player_index].configuration;
            break;
        case _player_select_string_killed_by_emblem:
            if (results && m_second_player_index != -1)
                configuration = &results->players[m_second_player_index].configuration;
            break;
        default:
            if (m_valid)
                configuration = &m_configuration;
            break;
        }
        if (configuration)
        {
            *emblem_info = configuration->client.appearance.emblem;
            result = true;
        }
    }
    return result;
}

// 0x4d0010 parse_xml_target_player_name (dev name ?parse_xml_target_player_name@@YA_NPAXPA_WJ@Z): the target player's name
bool parse_xml_target_player_name(void *context, wchar_t *out, long size)
{
    bool success = false;
    c_gui_player_select_screen_widget *self = static_cast<c_gui_player_select_screen_widget *>(context);
    if (self && self->m_valid)
    {
        const s_player_configuration *configuration = &self->m_configuration;
        if (configuration)
        {
            c_static_wchar_string<256> string;
            string.set(configuration->host.name);
            string.copy_to(out, size);
            success = true;
        }
    }
    return success;
}

bool parse_xml_target_player_rank(void *context, wchar_t *out, long size)
{
    bool success = false;
    c_gui_player_select_screen_widget *self = static_cast<c_gui_player_select_screen_widget *>(context);
    const s_player_configuration *configuration = self && self->m_valid ? &self->m_configuration : 0;
    c_static_wchar_string<1024> string;
    long rank = 0, grade = 0;
    if (configuration) { rank = configuration->host.global_rank; grade = configuration->host.global_grade; }
    if (user_interface_get_player_rank_string((e_experience_rank)rank, (e_experience_grade)grade, &string))
    {
        string.copy_to(out, size);
        success = true;
    }
    return success;
}

// @address 0x4d0138 ?parse_xml_target_player_grade@@YA_NPAXPA_WJ@Z
bool parse_xml_target_player_grade(void *context, wchar_t *out, long size)
{
    bool success = false;
    c_gui_player_select_screen_widget *self = static_cast<c_gui_player_select_screen_widget *>(context);
    const s_player_configuration *configuration = self && self->m_valid ? &self->m_configuration : 0;
    c_static_wchar_string<1024> string;
    long rank = 0, grade = 0;
    if (configuration) { rank = configuration->host.global_rank; grade = configuration->host.global_grade; }
    if (user_interface_get_player_grade_string((e_experience_rank)rank, (e_experience_grade)grade, false, true, &string))
    {
        string.copy_to(out, size);
        success = true;
    }
    return success;
}


// 0x4d0278 parse_xml_target_player_highest_skill (dev name ?parse_xml_target_player_highest_skill@@YA_NPAXPA_WJ@Z): the player's highest skill value
bool parse_xml_target_player_highest_skill(void *context, wchar_t *out, long size)
{
    c_gui_player_select_screen_widget *self = static_cast<c_gui_player_select_screen_widget *>(context);
    const s_player_configuration *configuration = self && self->m_valid ? &self->m_configuration : 0;
    c_static_wchar_string<256> string;
    long value = 0;
    if (configuration)
        value = configuration->client.global_statistics.highest_skill;
    string.print(L"%d", value);
    string.copy_to(out, size);
    return true;
}

// 0x4d01d8 parse_xml_target_player_xp (dev name ?parse_xml_target_player_xp@@YA_NPAXPA_WJ@Z): the target player's experience points
bool parse_xml_target_player_xp(void *context, wchar_t *out, long size)
{
    c_gui_player_select_screen_widget *self = static_cast<c_gui_player_select_screen_widget *>(context);
    const s_player_configuration *configuration = self && self->m_valid ? &self->m_configuration : 0;
    c_static_wchar_string<256> string;
    long xp = configuration && configuration->host.global_experience > 0 ? configuration->host.global_experience : 0;
    string.print(L"%d", xp);
    string.copy_to(out, size);
    return true;
}

// 0x4d0310 parse_xml_target_player_ranked_games (dev name ?parse_xml_target_player_ranked_games@@YA_NPAXPA_WJ@Z): the target player's ranked games count
bool parse_xml_target_player_ranked_games(void *context, wchar_t *out, long size)
{
    c_gui_player_select_screen_widget *self = static_cast<c_gui_player_select_screen_widget *>(context);
    const s_player_configuration *configuration = self && self->m_valid ? &self->m_configuration : 0;
    c_static_wchar_string<256> string;
    long count = 0;
    if (configuration && configuration->client.displayed_statistics.valid)
        count = configuration->client.displayed_statistics.ranked_completed;
    string.print(L"%d", count);
    string.copy_to(out, (unsigned long)size);
    return true;
}

// @address 0x4d03b0 ?parse_xml_target_player_social_games@@YA_NPAXPA_WJ@Z
bool parse_xml_target_player_social_games(void *context, wchar_t *out, long size)
{
    c_gui_player_select_screen_widget *self = static_cast<c_gui_player_select_screen_widget *>(context);
    const s_player_configuration *configuration = self && self->m_valid ? &self->m_configuration : 0;
    c_static_wchar_string<256> string;
    long count = 0;
    if (configuration && configuration->client.displayed_statistics.valid)
        count = configuration->client.displayed_statistics.unranked_completed;
    string.print(L"%d", count);
    string.copy_to(out, size);
    return true;
}

// @address 0x4d0450 ?parse_xml_target_player_custom_games@@YA_NPAXPA_WJ@Z
bool parse_xml_target_player_custom_games(void *context, wchar_t *out, long size)
{
    c_gui_player_select_screen_widget *self = static_cast<c_gui_player_select_screen_widget *>(context);
    const s_player_configuration *configuration = self && self->m_valid ? &self->m_configuration : 0;
    c_static_wchar_string<256> string;
    long count = 0;
    if (configuration && configuration->client.displayed_statistics.valid)
        count = configuration->client.displayed_statistics.custom_completed;
    string.print(L"%d", count);
    string.copy_to(out, size);
    return true;
}


// 0x4d04f0 parse_xml_target_player_kills (dev name ?parse_xml_target_player_kills@@YA_NPAXPA_WJ@Z): the player's kill count
bool parse_xml_target_player_kills(void *context, wchar_t *out, long size)
{
    c_gui_player_select_screen_widget *self = static_cast<c_gui_player_select_screen_widget *>(context);
    const c_game_results *results = gui_player_select_screen_widget_final_game_results();
    long kills = 0;
    if (results)
    {
        short player_index = ::player_index_from_identifier(results, &self->m_player_identifier);
        if (player_index != -1)
            kills = results->statistics.player[player_index].game_statistics[8].value;
    }
    c_static_wchar_string<256> string;
    string.print(L"%d", kills);
    string.copy_to(out, size);
    return true;
}

// 0x4d05c8 parse_xml_target_player_deaths (dev name ?parse_xml_target_player_deaths@@YA_NPAXPA_WJ@Z): the player's death count
bool parse_xml_target_player_deaths(void *context, wchar_t *out, long size)
{
    c_gui_player_select_screen_widget *self = static_cast<c_gui_player_select_screen_widget *>(context);
    const c_game_results *results = gui_player_select_screen_widget_final_game_results();
    long deaths = 0;
    if (results)
    {
        short player_index = ::player_index_from_identifier(results, &self->m_player_identifier);
        if (player_index != -1)
            deaths = results->statistics.player[player_index].game_statistics[10].value;
    }
    c_static_wchar_string<256> string;
    string.print(L"%d", deaths);
    string.copy_to(out, size);
    return true;
}


// 0x4d06a0 parse_xml_target_player_kd_spread (dev name ?parse_xml_target_player_kd_spread@@YA_NPAXPA_WJ@Z): kills minus deaths, signed
bool parse_xml_target_player_kd_spread(void *context, wchar_t *out, long size)
{
    c_gui_player_select_screen_widget *self = static_cast<c_gui_player_select_screen_widget *>(context);
    const c_game_results *results = gui_player_select_screen_widget_final_game_results();
    long spread = 0;
    if (results)
    {
        short player_index = ::player_index_from_identifier(results, &self->m_player_identifier);
        if (player_index != -1)
        {
            const s_game_results_player_statistics *statistics = &results->statistics.player[player_index];
            spread = (statistics->game_statistics[8].value) - (statistics->game_statistics[10].value);
        }
    }
    c_static_wchar_string<256> string;
    if ((float)spread <= 0.0f)
        string.print(L"%d", spread);
    else
        string.print(L"+%d", spread);
    string.copy_to(out, size);
    return true;
}
// 0x4d07b0 parse_xml_target_player_implement (gui_screen_postgame_*.cpp): the damage reporting type's name
// @address 0x21420 ?g_damage_reporting_type_string_list@@3QBJB
#if !defined(_XBOX) || defined(__clang__)
const long g_damage_reporting_type_string_list[_damage_reporting_type_count] = {
    0xf9, 0x3f7, 0x3f8, 0xa, 0x3f9, 0x3fa, 0x3fb, 0x3fc,
    0x3fd, 0x3fe, 0x3ff, 0x400, 0x401, 0x402, 0x403, 0x404,
    0x405, 0x406, 0x407, 0x408, 0x409, 0x40a, 0x40b, 0x40c,
    0x40d, 0x40e, 0x40f, 0x410, 0x411, 0x412, 0x413, 0x414,
    0x415, 0x416, 0x417, 0x418, 0x419, 0x41a, 0x41b, 0x35f,
    0x359, 0x364, 0x358, 0x422, 0x41c, 0x41d, 0x41e, 0x41f,
    0x4a7, 0x35c, 0x420, 0x421, 0x362, 0x361, 0x423, 0x363,
    0x424, 0x425, 0x426, 0x427, 0x4aa,
};
#endif

bool parse_xml_target_player_implement(void *data, wchar_t *buffer, long buffer_length)
{
    const c_game_results *results = gui_player_select_screen_widget_final_game_results();
    c_static_wchar_string<256> text;
    c_gui_player_select_screen_widget *screen = static_cast<c_gui_player_select_screen_widget *>(data);
    if (results && screen->m_implement_index >= 0 && screen->m_implement_index < _damage_reporting_type_count)
    {
        const s_user_interface_shared_globals *shared_globals =
            ((const s_user_interface_shared_globals *)::user_interface_shared_tag_globals_try_and_get());
        if (shared_globals && shared_globals->damage_reporting_strings.index != -1)
        {
            c_static_wchar_string<1024> name;
            if (text_group_get_string(shared_globals->damage_reporting_strings.index, g_damage_reporting_type_string_list[screen->m_implement_index], &name))
                text.print(L"%s", name.m_string);
        }
    }
    text.copy_to(buffer, buffer_length);
    return true;
}


// 0x4ce990 (unpaired; March's update_player_information is the nearest): the target player's rank icon, rank and
// service tag line and skill text


static __forceinline long gui_player_select_screen_widget_rank_element_handle(unsigned long rank, unsigned long grade)
{
    long element_handle = -1;
    if (rank <= 13 && grade <= 3)
        element_handle = g_experience_rank_icon_element_handles[rank][grade];
    return element_handle;
}

static __forceinline bool gui_player_select_screen_widget_player_string(long string_id, c_static_wchar_string<1024> *string)
{
    bool result = false;
    const s_user_interface_shared_globals *shared_globals = ((const s_user_interface_shared_globals *)::user_interface_shared_tag_globals_try_and_get());
    if (shared_globals && shared_globals->global_strings.index != -1)
        result = text_group_get_string(shared_globals->global_strings.index, string_id, string);
    return result;
}


// @address 0x4ce990 player_select_update_rank_display
extern "C" void player_select_update_rank_display(c_gui_player_select_screen_widget *screen)
{
    if (screen->m_valid)
    {
        const s_player_configuration *configuration = &screen->m_configuration;
        if (configuration)
        {
            c_gui_bitmap_widget *skill_bitmap =
                (c_gui_bitmap_widget *)((c_gui_widget *)screen)->get_child_widget(_gui_bitmap_widget, _player_select_string_difficulty);
            c_gui_bitmap_widget *rank_bitmap =
                (c_gui_bitmap_widget *)((c_gui_widget *)screen)->get_child_widget(_gui_bitmap_widget, _player_select_string_experience);
            if (rank_bitmap)
                rank_bitmap->m_bitmap_override = gui_player_select_screen_widget_rank_element_handle(configuration->host.global_rank,
                    configuration->host.global_grade);
            c_static_wchar_string<1024> rank_name;
            bool has_rank_name;
            if (configuration->client.appearance.player_model_choice == 1)
                has_rank_name = gui_player_select_screen_widget_player_string(_player_select_string_dervish, &rank_name);
            else
                has_rank_name = gui_player_select_screen_widget_player_string(_player_select_string_masterchief, &rank_name);
            if (has_rank_name)
            {
                c_static_wchar_string<256> service_tag;
                c_static_wchar_string<256> text;
                service_tag.set(configuration->client.appearance.service_tag);
                text.print(L"%s - %s", rank_name.m_string, service_tag.m_string);
                gui_screen_widget_set_text_widget_text((c_gui_screen_widget *)screen, _player_select_string_service_tag, text.m_string);
            }
            if (skill_bitmap)
                skill_bitmap->m_bitmap_override = configuration->client.bungienet_user_role;
            long skill_string;
            if (configuration->client.bungienet_user_role >= 5 && configuration->client.bungienet_user_role <= 8)
                skill_string = _player_select_string_campaign_completion_partial;
            else if (configuration->client.bungienet_user_role >= 0 && configuration->client.bungienet_user_role <= 3)
                skill_string = _player_select_string_campaign_completion_complete;
            else
                skill_string = _player_select_string_campaign_completion_none;
            ((c_gui_screen_widget *)screen)->set_text_widget_string_id(_player_select_string_campaign_completion, skill_string);
        }
    }
}
// 0x4d08c8 parse_xml_target_player_headshots (dev name in March 0x823d8b98 differs): the player's headshot total over the implement counters
bool parse_xml_target_player_headshots(void *context, wchar_t *out, long size)
{
    c_gui_player_select_screen_widget *self = static_cast<c_gui_player_select_screen_widget *>(context);
    const c_game_results *results = gui_player_select_screen_widget_final_game_results();
    long count = 0;
    if (results)
    {
        short player_index = ::player_index_from_identifier(results, &self->m_player_identifier);
        if (player_index != -1)
        {
            const s_game_results_player_statistics *statistics = &results->statistics.player[player_index];
            for (short i = 0; i < _damage_reporting_type_count; ++i)
                count += statistics->damage_statistics[i].values[4].value;
        }
    }
    c_static_wchar_string<256> string;
    string.print(L"%d", count);
    string.copy_to(out, size);
    return true;
}

// 0x4d09c8 (no dev name): the player's general statistic 13 (halved) in the spree line
// @address 0x4d09c8 ?parse_xml_target_player_spree@@YA_NPAXPA_WJ@Z
bool parse_xml_target_player_spree(void *context, wchar_t *out, long size)
{
    c_gui_player_select_screen_widget *self = static_cast<c_gui_player_select_screen_widget *>(context);
    const c_game_results *results = gui_player_select_screen_widget_final_game_results();
    long count = 0;
    if (results)
    {
        short player_index = ::player_index_from_identifier(results, &self->m_player_identifier);
        if (player_index != -1)
            count = results->statistics.player[player_index].game_statistics[13].value;
    }
    c_static_wchar_string<256> string;
    string.print(L"%d", count);
    string.copy_to(out, size);
    return true;
}


// @address 0x4d0aa0 ?parse_xml_target_player_implement_count@@YA_NPAXPA_WJ@Z
bool parse_xml_target_player_implement_count(void *context, wchar_t *out, long size)
{
    c_gui_player_select_screen_widget *self = static_cast<c_gui_player_select_screen_widget *>(context);
    const c_game_results *results = gui_player_select_screen_widget_final_game_results();
    c_static_wchar_string<256> string;
    if (results && self->m_implement_index >= 0 && self->m_implement_index < _damage_reporting_type_count)
    {
        short player_index = ::player_index_from_identifier(results, &self->m_player_identifier);
        if (player_index != -1)
            string.print(L"%i", results->statistics.player[player_index].damage_statistics[self->m_implement_index].values[0].value);
    }
    string.copy_to(out, size);
    return true;
}


// @address 0x4d0b90 parse_xml_target_player_first_player_name
extern "C" bool parse_xml_target_player_first_player_name(void *context, wchar_t *out, long size)
{
    c_gui_player_select_screen_widget *self = static_cast<c_gui_player_select_screen_widget *>(context);
    const c_game_results *results = gui_player_select_screen_widget_final_game_results();
    c_static_wchar_string<256> string;
    if (results && self->m_first_player_index >= 0 && self->m_first_player_index < 16)
        string.set(results->players[self->m_first_player_index].configuration.host.name);
    string.copy_to(out, size);
    return true;
}
// @address 0x4d0c68 parse_xml_target_player_first_player_count
extern "C" bool parse_xml_target_player_first_player_count(void *context, wchar_t *out, long size)
{
    c_gui_player_select_screen_widget *self = static_cast<c_gui_player_select_screen_widget *>(context);
    c_static_wchar_string<256> string;
    string.print(L"%d", (short)self->m_first_player_count);
    string.copy_to(out, size);
    return true;
}


// @address 0x4d0ce0 parse_xml_target_player_second_player_name
extern "C" bool parse_xml_target_player_second_player_name(void *context, wchar_t *out, long size)
{
    c_gui_player_select_screen_widget *self = static_cast<c_gui_player_select_screen_widget *>(context);
    const c_game_results *results = gui_player_select_screen_widget_final_game_results();
    c_static_wchar_string<256> string;
    if (results && self->m_second_player_index >= 0 && self->m_second_player_index < 16)
        string.set(results->players[self->m_second_player_index].configuration.host.name);
    string.copy_to(out, size);
    return true;
}

// @address 0x4d0db8 parse_xml_target_player_second_player_count
extern "C" bool parse_xml_target_player_second_player_count(void *context, wchar_t *out, long size)
{
    c_gui_player_select_screen_widget *self = static_cast<c_gui_player_select_screen_widget *>(context);
    c_static_wchar_string<256> string;
    string.append_print(L"%i", (short)self->m_second_player_count);
    string.copy_to(out, size);
    return true;
}

// @address 0x4d0e30 parse_xml_matchmaking_progress_value
extern "C" bool parse_xml_matchmaking_progress_value(void *context, wchar_t *out, long size)
{
    bool result = false;
    if (network_life_cycle_in_matchmaking())
    {
        s_life_cycle_matchmaking_progress progress;
        network_life_cycle_get_matchmaking_progress(&progress);
        g_user_interface_matchmaking_progress = progress;
        if (progress.type == 5)
        {
            c_static_wchar_string<1024> string;
            string.print(L"%02d", progress.countdown_timer);
            string.copy_to(out, size);
            result = true;
        }
    }
    return result;
}


// @address 0x549e8 ??_7c_player_select_actions_datasource@@6B@
// @address 0x4c0ee0 ??_Ec_player_select_actions_datasource@@UAAPAXI@Z

// @address 0x4ccad8 ??0c_player_select_actions_datasource@@QAA@W4e_controller_index@@PBUs_player_identifier@@@Z
c_player_select_actions_datasource::c_player_select_actions_datasource(e_controller_index controller_index, const s_player_identifier *player)
    : m_controller_index(controller_index), m_player(*player), m_item_count(0)
{
}

// builds a throw-away action datasource for one controller/player
// pair and reports whether it offers any action.

// @address 0x4cff28 player_select_actions_has_available_action
extern "C" bool player_select_actions_has_available_action(int controller_index, const s_player_identifier *player, const s_player_identifier *target,
    const s_player_configuration *configuration, bool unused)
{
    bool result = false;
    c_player_select_actions_datasource datasource((e_controller_index)controller_index, player);
    datasource.set_name(-1);
    player_select_actions_datasource_set_target(&datasource, target, configuration);
    if (datasource.c_gui_ordered_data::get_current_item_count() > 0)
        result = true;
    return result;
}


// @address 0x54920 ??_7c_gui_player_select_screen_widget@@6B@
// @address 0x4a8838 ??_Ec_gui_player_select_screen_widget@@UAAPAXI@Z
// @address 0x4cd628 ??0c_gui_player_select_screen_widget@@QAA@J@Z
c_gui_player_select_screen_widget::c_gui_player_select_screen_widget(long name) :
    c_gui_screen_widget(name),
    m_implement_index(0),
    m_first_player_index(-1),
    m_first_player_count(0),
    m_second_player_index(-1),
    m_second_player_count(0)
{
}
// @address 0x4cc938 ??0c_load_player_select_screen_message@@QAA@JW4e_controller_index@@W4e_window_index@@JPBUs_player_identifier@@2_N@Z
c_load_player_select_screen_message::c_load_player_select_screen_message(long screen_name, e_controller_index controller, e_window_index window,
    long layered_position, const s_player_identifier *target_player_identifier, const s_player_identifier *player_identifier, bool)
    : c_load_screen_message(screen_name, controller, window, layered_position),
    m_target_player_identifier(*target_player_identifier),
    m_player_identifier(*player_identifier),
    m_valid(false),
    m_show_service_record(true)
{
    const s_player_configuration *configuration = player_select_find_player_configuration(player_identifier);
    memset(&m_configuration, 0, sizeof(m_configuration));
    if (configuration)
    {
        memcpy(&m_configuration, configuration, sizeof(m_configuration));
        m_valid = true;
    }
}




// Rebuilds the action list (the body makes no
// c_gui_data::initialize call and no return value): the actions the target player offers
// Availability checks include whether the local user is online-enabled.

static __forceinline unsigned __int64 gui_player_select_screen_widget_user_xuid(short user_index)
{
    unsigned __int64 xuid = 0;
    if (user_index >= 0 && user_index < 4)
        xuid = g_network_session_interface_local_users[user_index].xuid;
    return xuid;
}

static __forceinline bool gui_player_select_screen_widget_xuid_is_guest_account(unsigned __int64 xuid)
{
    unsigned long high = (unsigned long)(xuid >> 48);
    bool guest = (high & 15) == 9 && (high & 192) != 0;
    return guest;
}

// 0x4ccb70: stores the target identifier/configuration into the screen's player slots, then rebuilds
// the action list.
extern "C" void player_select_actions_datasource_set_target(c_player_select_actions_datasource *datasource, const s_player_identifier *identifier,
    const s_player_configuration *configuration)
{
    if (identifier == 0)
        return;
    if (configuration == 0)
        return;
    memcpy(&datasource->m_target_player, identifier, sizeof(s_player_identifier));
    memcpy(&datasource->m_target_configuration, configuration, sizeof(s_player_configuration));
    player_select_actions_datasource_rebuild(datasource);
}

extern "C" void player_select_actions_datasource_rebuild(c_player_select_actions_datasource *datasource)
{
    datasource->m_item_count = 0;
    bool in_ui_shell = game_in_progress() && game_is_ui_shell();
    c_controller_interface *controller = &g_controller_globals.controllers[datasource->m_controller_index];
    if (controller && controller->in_use())
    {
        long player_index = user_interface_session_player_get_player_index(&datasource->m_player);
        long target_index = user_interface_session_player_get_player_index(&datasource->m_target_player);
        long squad_index = ::user_interface_squad_get_player_index(&datasource->m_target_player);
        unsigned __int64 target_xuid = datasource->m_target_configuration.client.xuid;
        unsigned __int64 local_xuid = gui_player_select_screen_widget_user_xuid(controller->user_index);
        bool same_player = 0 == memcmp(&datasource->m_player, &datasource->m_target_player, sizeof(s_player_identifier));
        bool online = ::online_local_xuid_is_online_enabled();
        bool silver_or_gold_live = controller->is_silver_or_gold_live();
        bool local_guest = silver_or_gold_live && gui_player_select_screen_widget_xuid_is_guest_account(local_xuid);
        bool target_local = target_index != -1 ? user_interface_session_is_local_peer_player(target_index) : false;
        bool target_silver_or_gold_live = datasource->m_target_configuration.client.online;
        bool target_guest = target_silver_or_gold_live && gui_player_select_screen_widget_xuid_is_guest_account(target_xuid);
        bool friends = online && target_silver_or_gold_live && !target_guest && silver_or_gold_live && !local_guest &&
            online_user_is_friend(datasource->m_controller_index, target_xuid);
        bool squad_leader = !user_interface_interactive_session_is_group() && ::network_life_cycle_squad_local_peer_is_leader();
        bool squad_player_valid = squad_index != -1 && user_interface_session_is_player_valid(squad_index);
        datasource->add_action(_player_select_string_service_record,
            in_ui_shell && online && silver_or_gold_live && target_silver_or_gold_live && !target_guest);
        datasource->add_action(_player_select_string_shared_files,
            in_ui_shell && online && silver_or_gold_live && target_silver_or_gold_live && !target_guest);
        datasource->add_action(_player_select_string_gamercard, online && silver_or_gold_live && target_silver_or_gold_live && !target_guest);
        datasource->add_action(_player_select_string_abdicate_leadership,
            game_is_map_active() && !game_is_cooperative() && squad_leader && squad_player_valid && !target_local);
        datasource->add_action(_player_select_string_boot_from_squad,
            squad_leader && squad_player_valid && !target_local && user_interface_squad_is_booting_allowed());
        datasource->add_action(_player_select_string_add_friend,
            online && silver_or_gold_live && target_silver_or_gold_live && !same_player && !friends && !target_guest);
        datasource->add_action(_player_select_string_submit_review,
            online && silver_or_gold_live && target_silver_or_gold_live && !same_player && !target_guest);
        if (player_index != -1 && target_index != -1)
        {
            datasource->add_action(_player_select_string_mute,
                !same_player && !target_guest && !target_local && !voice_get_mute(player_index, target_index));
            datasource->add_action(_player_select_string_unmute,
                !same_player && !target_guest && !target_local && voice_get_mute(player_index, target_index));
        }
    }
}






// @address 0x4ced48 ?handle_list_item_chosen@c_gui_player_select_screen_widget@@UAA_NPBVc_controller_input_message@@JPAVc_gui_list_item_widget@@PAVc_gui_data@@@Z
bool c_gui_player_select_screen_widget::handle_list_item_chosen(
    const c_controller_input_message *message, long list_name, c_gui_list_item_widget *list_item, c_gui_data *data)
{
    bool result = false;
    long target_index = user_interface_session_player_get_player_index(&m_target_player_identifier);
    long player_index = user_interface_session_player_get_player_index(&m_player_identifier);
    bool muted = target_index != -1 && player_index != -1 && voice_get_local_mute_through_guide(target_index, player_index);
    if (list_name == _player_select_string_player_select)
    {
        long chosen = -1;
        if (data->get_string_id_value(list_item->get_element_handle(), _player_select_string_name, &chosen))
        {
            switch (chosen)
            {
            case _player_select_string_abdicate_leadership:
                transition_out_with_transition_type(0, _screen_transition_normal);
                if (::network_life_cycle_squad_local_peer_is_leader())
                {
                    long squad_index = ::user_interface_squad_get_player_index(&m_player_identifier);
                    if (squad_index != -1 && !::user_interface_squad_is_local_player(squad_index))
                    {
                        if (::network_life_cycle_squad_local_peer_is_leader() && ::user_interface_squad_is_player_valid(squad_index) &&
                            !::user_interface_squad_is_local_player(squad_index))
                            network_squad_session_delegate_leadership(squad_index);
                    }
                }
                result = true;
                break;
            case _player_select_string_mute:
                if (muted)
                {
                    e_controller_index responding = get_any_responding_controller();
                    g_gui_error_manager.post_error_with_custom_message(0x2001c, 0, 0, responding, false, true);
                }
                else
                {
                    transition_out_with_transition_type(0, _screen_transition_normal);
                    long first_index = user_interface_session_player_get_player_index(&m_target_player_identifier);
                    long second_index = user_interface_session_player_get_player_index(&m_player_identifier);
                    if (first_index != -1 && second_index != -1)
                        voice_set_mute(first_index, second_index, true);
                }
                result = true;
                break;
            case _player_select_string_unmute:
                if (muted)
                {
                    e_controller_index responding = get_any_responding_controller();
                    g_gui_error_manager.post_error_with_custom_message(0x2001c, 0, 0, responding, false, true);
                }
                else
                {
                    transition_out_with_transition_type(0, _screen_transition_normal);
                    long first_index = user_interface_session_player_get_player_index(&m_target_player_identifier);
                    long second_index = user_interface_session_player_get_player_index(&m_player_identifier);
                    if (first_index != -1 && second_index != -1)
                        voice_set_mute(first_index, second_index, false);
                }
                result = true;
                break;
            case _player_select_string_boot_from_squad:
            {
                void *memory = user_interface_malloc_tracked(sizeof(c_load_dialog_screen_message), 0, 0);
                if (memory != 0)
                {
                    int layered_position = get_name();
                    e_controller_index controller = message->get_controller();
                    c_load_dialog_screen_message *dialog = new (memory) c_load_dialog_screen_message(
                        controller, get_render_window(), layered_position,
                        _player_select_string_confirm_boot_from_squad, layered_position);
                    if (dialog != 0)
                    {
                        dialog->set_parent_screen_index(get_screen_index());
                        ::user_interface_messaging_post(dialog);
                    }
                }
                result = true;
                break;
            }
            case _player_select_string_gamercard:
                if (m_valid)
                {
                    unsigned __int64 xuid = m_configuration.client.xuid;
                    if (xuid != 0)
                        online_guide_show_gamer_card_ui(message->get_controller(), xuid);
                }
                result = true;
                break;
            case _player_select_string_achievements:
                online_guide_update_get_service_record(this, message->get_controller(), 0);
                result = true;
                break;
            case _player_select_string_service_record:
                online_guide_update_get_service_record(this, message->get_controller(), 1);
                result = true;
                break;
            case _player_select_string_add_friend:
            {
                e_controller_index controller = message->get_controller();
                if (m_valid)
                {
                    unsigned __int64 xuid = m_configuration.client.xuid;
                    if (xuid != 0 && (&g_controller_globals.controllers[controller])->is_signed_in_to_live())
                        XShowFriendRequestUI(controller, xuid);
                }
                result = true;
                break;
            }
            case _player_select_string_status_assembling_summary_ranked_party:
            {
                e_controller_index controller = message->get_controller();
                if (m_valid)
                {
                    unsigned __int64 xuid = m_configuration.client.xuid;
                    if (xuid != 0 && (&g_controller_globals.controllers[controller])->is_signed_in_to_live())
                        XShowPlayerReviewUI(controller, xuid);
                }
                result = true;
                break;
            }
            default:
                // match: 0x4ced48: __assume(0) omits switch range guards (cmplwi 160/484, cmpw 0x10400) and keeps bne epilogue with result in r30
                __assume(0);
            }
        }
    }
    if (!result)
        result = c_gui_screen_widget::handle_list_item_chosen(message, list_name, list_item, data);
    return result;
}

// @address 0x7cff0c ?global_tag_instances@@3PBUcache_file_tag_instance@@B
static __forceinline short gui_player_select_screen_widget_bitmap_frame_count(long bitmap_index)
{
    short count = 0;
    const bitmap_group *bitmap = TAG_GET('bitm', const bitmap_group, bitmap_index);
    if (bitmap)
    {
        if (bitmap->sequences.count)
            count = (short)((const bitmap_sequence *)bitmap->sequences.address)->sprites.count;
        else
            count = (short)bitmap->bitmaps.count;
    }
    return count;
}
// 0x4cd6b8 c_gui_player_select_screen_widget::initialize (dev name ?initialize@c_gui_player_select_screen_widget@@UAAXXZ)





// @address 0x54a58 ??_7c_player_select_medals_datasource@@6B@





// @address 0x4ce330 ?initialize_datasource@c_gui_player_select_screen_widget@@UAAXXZ
void c_gui_player_select_screen_widget::initialize_datasource()
{
    c_gui_screen_widget::initialize_datasource();
    if (m_valid)
    {
        // match: 0x4ce330: pointer __restrict qualifier prevents alias reload
        s_player_configuration *__restrict player_configuration = &m_configuration;
        void *memory = user_interface_malloc_tracked(sizeof(c_player_select_actions_datasource), 0, 0);
        if (memory)
        {
            long controller_index = get_any_responding_controller();
            c_player_select_actions_datasource *object = new (memory) c_player_select_actions_datasource(
                (e_controller_index)controller_index,
                &m_target_player_identifier);
            if (object)
            {
                if (object->initialize(_player_select_string_player_select))
                {
                    player_select_actions_datasource_set_target(object, &m_player_identifier, player_configuration);
                    m_datasource[m_datasource_count] = object;
                    m_datasource_count++;
                }
                else
                {
                    object->~c_player_select_actions_datasource();
                    user_interface_free(object);
                }
            }
        }
    }
    void *message_memory = user_interface_malloc_tracked(sizeof(c_player_select_medals_datasource), 0, 0);
    if (message_memory)
    {
        c_player_select_medals_datasource *message = new (message_memory) c_player_select_medals_datasource;
        if (((c_gui_data *)message)->initialize(_player_select_string_medals_list))
        {
            player_select_medals_datasource_set_player((c_player_select_medals_datasource *)message, m_player_identifier);
            m_datasource[m_datasource_count] = (c_gui_data *)message;
            m_datasource_count++;
        }
        else
        {
            ((c_gui_data *)message)->~c_gui_data();
            user_interface_free(message);
        }
    }
}


// @address 0x483310 ??0c_load_dialog_screen_message@@QAA@W4e_controller_index@@W4e_window_index@@JJJ@Z
// @address 0x4842c0 ?tint_widget_to_change_color@@YAXPAVc_gui_widget@@J_N@Z
// @address 0x484d20 ?print@?$c_static_wchar_string@$0EAA@@@QAAPB_WPB_WZZ
// @address 0x484e90 ?user_interface_malloc_tracked@@YAPAXIPBDJ@Z
// @address 0x489840 ?set_visible@c_gui_widget@@QAAX_N@Z
// @address 0x48bfa0 ?get_child_widget@c_gui_widget@@QAAPAV1@W4e_gui_widget_type@@J@Z
// @address 0x3a2908 ?network_life_cycle_get_matchmaking_progress@@YAXPAUs_life_cycle_matchmaking_progress@@@Z
// @address 0x3b08f8 ?network_life_cycle_in_matchmaking@@YA_NXZ
// @address 0x84740 ?data_iterator_next@@YAPAXPAUs_data_iterator@@@Z
// @address 0x46b890 ?user_interface_squad_get_player_count@@YAJXZ

// @address 0x4c24a0 ?initialize@c_gui_screen_campaign_settings@@UAAXXZ
void c_gui_screen_campaign_settings::initialize()
{
    c_gui_screen_widget::initialize();
    player_select_set_focused_list_state(this, 1, 0);
    c_gui_list_widget *list =
        static_cast<c_gui_list_widget *>(get_child_widget(_gui_list_widget, _player_select_string_scoring_option));
    c_gui_data *data = list ? list->get_data() : 0;
    long variant_index = network_life_cycle_session_get_game_variant();
    if (list && data && variant_index >= 0 && variant_index < data->get_current_item_count())
        list->set_focused_element_handle(variant_index, false);
}

// @address 0x4c2af8 ?handle_list_item_chosen@c_gui_screen_campaign_settings@@UAA_NPBVc_controller_input_message@@JPAVc_gui_list_item_widget@@PAVc_gui_data@@@Z
bool c_gui_screen_campaign_settings::handle_list_item_chosen(
    const c_controller_input_message *message, long list_name, c_gui_list_item_widget *list_item_widget,
    c_gui_data *data)
{
    bool handled = false;
    if (list_name == _player_select_string_campaign_settings)
    {
        if (!player_select_test_and_show_load_dialog(this, message->get_controller()))
        {
            c_gui_list_widget *list =
                static_cast<c_gui_list_widget *>(get_child_widget(_gui_list_widget, _player_select_string_spinner));
            if (list)
            {
                c_gui_data *list_data = list->get_data();
                long variant_index = network_life_cycle_session_get_game_variant();
                if (variant_index >= 0 && variant_index < list_data->get_current_item_count())
                    list->set_focused_element_handle(variant_index, false);
            }
        }
        handled = true;
    }
    else if (list_name == _player_select_string_spinner)
    {
        if (!player_select_test_and_show_load_dialog(this, message->get_controller()))
        {
            c_gui_list_item_widget *child = !m_suppress_focus && m_focused_widget
                && m_focused_widget->type == 5 ? static_cast<c_gui_list_item_widget *>(m_focused_widget) : 0;
            short element = (short)list_item_widget->get_element_handle();
            if (element >= 0 && element < 3)
                network_squad_session_set_campaign_metagame_scoring(element);
            c_gui_list_widget *list =
                static_cast<c_gui_list_widget *>(get_child_widget(_gui_list_widget, _player_select_string_scoring_option));
            if (list)
                list->set_focused_element_handle(element, false);
            if (child)
                child->handle_widget_back_out();
        }
        handled = true;
    }
    else if (list_name == _player_select_string_primary_skulls)
    {
        long element = list_item_widget->get_element_handle();
        long available = (long)controller_get_signed_in_mask1();
        if ((unsigned long)element <= 8)
        {
            if (((1 << element) & available) != 0)
            {
                if (!player_select_test_and_show_load_dialog(this, message->get_controller()))
                {
                    long current = network_life_cycle_session_get_primary_skulls();
                    if ((current & (1 << element)) == 0)
                        network_squad_session_set_primary_skulls(current | (1 << element));
                    else
                        network_squad_session_set_primary_skulls(current & ~(1 << element));
                }
                handled = true;
            }
            else
            {
                ::user_interface_play_sound(_ui_sound_error, m_core_definition.sound_override_index);
            }
        }
    }
    else if (list_name == _player_select_string_secondary_skulls)
    {
        long element = list_item_widget->get_element_handle();
        long available = (long)controller_get_signed_in_mask();
        long slot = element + 1;
        if ((unsigned long)slot <= 4)
        {
            if (((1 << slot) & available) != 0)
            {
                if (!player_select_test_and_show_load_dialog(this, message->get_controller()))
                {
                    long current = network_life_cycle_session_get_global_map_precache_status();
                    if ((current & (1 << slot)) == 0)
                        network_squad_session_set_secondary_skulls(current | (1 << slot));
                    else
                        network_squad_session_set_secondary_skulls(current & ~(1 << slot));
                }
                handled = true;
            }
            else
            {
                ::user_interface_play_sound(_ui_sound_error, m_core_definition.sound_override_index);
            }
        }
    }
    if (!handled)
        handled = c_gui_screen_widget::handle_list_item_chosen(message, list_name, list_item_widget, data);
    return handled;
}

// @address 0x4c30c0 ?handle_controller_input_message@c_gui_screen_campaign_settings@@UAA_NPBVc_controller_input_message@@@Z
bool c_gui_screen_campaign_settings::handle_controller_input_message(const c_controller_input_message *message)
{
    c_gui_list_widget *players =
        static_cast<c_gui_list_widget *>(get_child_widget(_gui_list_widget, _player_select_string_campaign_settings));
    c_gui_list_widget *teams =
        static_cast<c_gui_list_widget *>(get_child_widget(_gui_list_widget, _player_select_string_primary_skulls));
    c_gui_list_widget *others =
        static_cast<c_gui_list_widget *>(get_child_widget(_gui_list_widget, _player_select_string_secondary_skulls));
    long state;
    short target;
    bool handled=false;
    bool suppress=false;
    if (!get_child_widget(_gui_list_widget, _player_select_string_spinner))
    {
        do
        {
            switch (message->get_event_type())
            {
            case 1:
                if (!teams)
                    continue;
                if (players && players->get_item_count(0) > 0 && teams->within_focus_chain())
                {
                    if (teams->m_focused_item_index >= 5)
                        continue;
                    state = 1;
                    target = (short)(players->get_selectable_item_count() - 1);
                    handled = true;
                }
                else
                {
                    if (teams->get_item_count(0) <= 5 || !others || !others->within_focus_chain())
                        continue;
                    long index = others->m_focused_item_index + 5;
                    if (index >= 9)
                        index -= 5;
                    target = (short)index;
                    handled = true;
                    state = 2;
                }
                break;
            case 3:
                if (!teams)
                    continue;
                if (players && teams->get_item_count(0) > 0 && players->within_focus_chain())
                {
                    long index = players->m_focused_item_index;
                    if (index != players->get_selectable_item_count() - 1)
                        continue;
                    state = 2;
                    target = 0;
                    handled = true;
                }
                else
                {
                    if (!others || others->get_item_count(0) <= 0 || !teams->within_focus_chain())
                        continue;
                    long index = teams->m_focused_item_index;
                    if (index < 5)
                        continue;
                    state = 3;
                    target = (short)(index % 5);
                    handled = true;
                }
                break;
            case _event_type_button_press:
                if (players->within_focus_chain() && message->get_component() == 0 &&
                    player_select_test_and_show_load_dialog(this, message->get_controller()))
                    suppress = true;
                continue;
            default:
                continue;
            }
            player_select_set_focused_list_state(this, state, target);
        } while (false);
    }
    if (!handled && !suppress)
        return c_gui_screen_widget::handle_controller_input_message(message);
    return handled;
}

// @address 0x4c2998 ?handle_dialog_result@c_gui_screen_campaign_settings@@UAA_NPBVc_dialog_result_message@@@Z
bool c_gui_screen_campaign_settings::handle_dialog_result(const c_dialog_result_message *message)
{
    bool handled = false;
    if (message->m_dialog_name == _player_select_string_campaign_changing_setting_will_delete_saved_game) {
        if (message->m_dialog_result == 0)
            saved_game_files_delete_personal_storage(message->m_controller);
        handled = true;
    }
    if ((unsigned char)handled == 0) {
        handled = false;
        if (message->m_dialog_name == _player_select_string_need_to_select_storage_device_to_save) {
            if (message->m_dialog_result == 0)
                content_catalogue_display_device_selection_guide_interface(message->m_controller);
            handled = true;
        }
    }
    return handled;
}







// @address 0x4cd6b8 ?initialize@c_gui_player_select_screen_widget@@UAAXXZ
void c_gui_player_select_screen_widget::initialize()
{
    c_gui_screen_widget::initialize();
    add_game_tag_parser(new(0, 0, _ui_allocation_marker_dummy) c_magic_string_game_tag_parser(L"<target-player-name", this, parse_xml_target_player_name));
    add_game_tag_parser(new(0, 0, _ui_allocation_marker_dummy) c_magic_string_game_tag_parser(L"<player-rank", this, parse_xml_target_player_rank));
    add_game_tag_parser(new(0, 0, _ui_allocation_marker_dummy) c_magic_string_game_tag_parser(L"<player-grade", this, parse_xml_target_player_grade));
    add_game_tag_parser(new(0, 0, _ui_allocation_marker_dummy) c_magic_string_game_tag_parser(L"<xp-value", this, parse_xml_target_player_xp));
    add_game_tag_parser(new (0, 0, _ui_allocation_marker_dummy)
            c_magic_string_game_tag_parser(L"<skill-value", this, parse_xml_target_player_highest_skill));
    add_game_tag_parser(new (0, 0, _ui_allocation_marker_dummy)
            c_magic_string_game_tag_parser(L"<ranked-value", this, parse_xml_target_player_ranked_games));
    add_game_tag_parser(new (0, 0, _ui_allocation_marker_dummy)
            c_magic_string_game_tag_parser(L"<social-value", this, parse_xml_target_player_social_games));
    add_game_tag_parser(new (0, 0, _ui_allocation_marker_dummy)
            c_magic_string_game_tag_parser(L"<custom-value", this, parse_xml_target_player_custom_games));
    add_game_tag_parser(new(0, 0, _ui_allocation_marker_dummy) c_magic_string_game_tag_parser(L"<kills", this, parse_xml_target_player_kills));
    add_game_tag_parser(new (0, 0, _ui_allocation_marker_dummy)
            c_magic_string_game_tag_parser(L"<deaths", this, parse_xml_target_player_deaths));
    add_game_tag_parser(new (0, 0, _ui_allocation_marker_dummy)
            c_magic_string_game_tag_parser(L"<kd-ratio", this, parse_xml_target_player_kd_spread));
    add_game_tag_parser(new (0, 0, _ui_allocation_marker_dummy)
            c_magic_string_game_tag_parser(L"<headshots", this, parse_xml_target_player_headshots));
    add_game_tag_parser(new(0, 0, _ui_allocation_marker_dummy) c_magic_string_game_tag_parser(L"<best-spree", this, parse_xml_target_player_spree));
    add_game_tag_parser(new (0, 0, _ui_allocation_marker_dummy)
            c_magic_string_game_tag_parser(L"<implement-name", this, parse_xml_target_player_implement));
    add_game_tag_parser(new (0, 0, _ui_allocation_marker_dummy)
            c_magic_string_game_tag_parser(L"<implement-count", this, parse_xml_target_player_implement_count));
    add_game_tag_parser(new (0, 0, _ui_allocation_marker_dummy)
            c_magic_string_game_tag_parser(L"<most-killed-name", this, parse_xml_target_player_first_player_name));
    add_game_tag_parser(new (0, 0, _ui_allocation_marker_dummy)
            c_magic_string_game_tag_parser(L"<most-killed-count", this, parse_xml_target_player_first_player_count));
    add_game_tag_parser(new (0, 0, _ui_allocation_marker_dummy)
            c_magic_string_game_tag_parser(L"<most-killed-by-name", this, parse_xml_target_player_second_player_name));
    add_game_tag_parser(new (0, 0, _ui_allocation_marker_dummy)
            c_magic_string_game_tag_parser(L"<most-killed-by-count", this, parse_xml_target_player_second_player_count));
    add_game_tag_parser(new (0, 0, _ui_allocation_marker_dummy)
            c_magic_string_game_tag_parser(L"<rematch-timer", this, parse_xml_matchmaking_progress_value));
    player_select_update_statistics_display(this);
}
// @address 0x4cf410 ?update_player_information@c_gui_player_select_screen_widget@@AAA_NF@Z
bool c_gui_player_select_screen_widget::update_player_information(short player_index)
{
    bool result = false;
    c_gui_widget *list = this->get_child_widget(_gui_list_widget, _player_select_string_medals_list);
    if (list)
    {
        c_gui_widget *first_widget = this->get_child_widget(_gui_bitmap_widget, _player_select_string_medal_image);
        c_gui_widget *second_widget = this->get_child_widget(_gui_text_widget, _player_select_string_medal_name);
        c_gui_widget *third_widget = this->get_child_widget(_gui_text_widget, _player_select_string_medal_description);
        bool visible;
        if (player_index != -1)
        {
            ((c_gui_screen_widget *)(c_gui_widget *)this)
                ->transfer_focus_to_list((c_gui_list_widget *)list, player_index, false, false);
            result = true;
            visible = true;
        }
        else
            visible = false;
        if (first_widget)
            first_widget->set_visible(visible);
        if (second_widget)
            second_widget->set_visible(visible);
        if (third_widget)
            third_widget->set_visible(visible);
    }
    return result;
}
// @address 0x4cdd28 player_select_update_statistics_display
extern "C" void player_select_update_statistics_display(c_gui_player_select_screen_widget *screen)
{
    c_gui_widget *implement = ((c_gui_widget *)screen)->get_child_widget(_gui_bitmap_widget, _player_select_string_implement_image);
    if (implement)
    {
        const c_game_results *results = user_interface_networking_get_final_game_results();
        if (results)
        {
            short player_index = ::player_index_from_identifier(
                results, &screen->m_player_identifier);
            if (player_index != -1)
            {
                short best_implement = -1;
                short best_count = 0;
                bool visible = false;
                const s_game_results_player_statistics *statistics = &results->statistics.player[player_index];
                for (short implement_index = 0; implement_index < _damage_reporting_type_count; ++implement_index)
                {
                    short count = statistics->damage_statistics[implement_index].values[0].value;
                    if (count > best_count)
                    {
                        best_count = count;
                        best_implement = implement_index;
                    }
                }
                if (best_implement >= 0 && best_implement < _damage_reporting_type_count)
                {
                    short frame_count = gui_player_select_screen_widget_bitmap_frame_count(
                        ((c_gui_bitmap_widget *)implement)->m_definition.bitmap_definition_index);
                    visible = true;
                    ((c_gui_bitmap_widget *)implement)->m_bitmap_override = best_implement % frame_count;
                    screen->m_implement_index = best_implement;
                }
                implement->set_visible(visible);
                c_gui_widget *name = ((c_gui_widget *)screen)->get_child_widget(_gui_text_widget, _player_select_string_implement);
                c_gui_widget *description = ((c_gui_widget *)screen)->get_child_widget(_gui_text_widget, _player_select_string_implement_name);
                if (name) name->set_visible(visible);
                if (description) description->set_visible(visible);
            }
        }
    }
    c_gui_widget *list = ((c_gui_widget *)screen)->get_child_widget(_gui_list_widget, _player_select_string_player_select);
    if (list) ((c_gui_screen_widget *)(c_gui_widget *)screen)->transfer_focus_to_list((c_gui_list_widget *)list, 0, false, false);
    screen->update_player_information(-1);
    c_gui_widget *victim = ((c_gui_widget *)screen)->get_child_widget(_gui_text_widget, _player_select_string_killed);
    c_gui_widget *victim_count = ((c_gui_widget *)screen)->get_child_widget(_gui_text_widget, _player_select_string_killed_most_name);
    const c_game_results *results = gui_player_select_screen_widget_final_game_results();
    short player_index = -1;
    if (results)
        player_index = ::player_index_from_identifier(
            results, &screen->m_player_identifier);
    if (victim)
    {
        c_gui_widget *victim_icon = ((c_gui_widget *)screen)->get_child_widget(_gui_bitmap_widget, _player_select_string_killed_emblem);
        short best_player = -1;
        short best_count = 0;
        bool visible = false;
        if (results && player_index != -1)
        {
            for (short opponent_index = 0; opponent_index < 16; ++opponent_index)
            {
                if (opponent_index != player_index && results->players[opponent_index].player_exists)
                {
                    short count = results->statistics.pvp_statistics[player_index][opponent_index][0].value;
                    if (count > best_count)
                    {
                        bool enemy;
                        if (results->teams_enabled)
                            enemy = (results->players[opponent_index].configuration.host.team_index
                                - results->players[player_index].configuration.host.team_index) != 0;
                        else
                            enemy = true;
                        if (enemy)
                        {
                            best_count = count;
                            best_player = opponent_index;
                        }
                    }
                }
            }
        }
        if (best_player != -1)
        {
            visible = true;
            screen->m_first_player_index = best_player;
            screen->m_first_player_count = best_count;
        }
        c_gui_widget *victim_background = ((c_gui_widget *)screen)->get_child_widget(_gui_bitmap_widget, _player_select_string_killed_image);
        c_gui_widget *victim_name = ((c_gui_widget *)screen)->get_child_widget(_gui_text_widget, _player_select_string_killed_count);
        if (victim_background) victim_background->set_visible(visible);
        if (victim_icon) victim_icon->set_visible(visible);
        if (victim_count) victim_count->set_visible(visible);
        victim->set_visible(visible);
        victim_name->set_visible(visible);
    }
    c_gui_widget *nemesis = ((c_gui_widget *)screen)->get_child_widget(_gui_text_widget, _player_select_string_killed_by);
    c_gui_widget *nemesis_count = ((c_gui_widget *)screen)->get_child_widget(_gui_text_widget, _player_select_string_killed_most_by_name);
    if (nemesis)
    {
        c_gui_widget *nemesis_icon = ((c_gui_widget *)screen)->get_child_widget(_gui_bitmap_widget, _player_select_string_killed_by_emblem);
        short best_player = -1;
        short best_count = 0;
        bool visible = false;
        if (results && player_index != -1)
        {
            for (short opponent_index = 0; opponent_index < 16; ++opponent_index)
            {
                if (opponent_index != player_index && results->players[opponent_index].player_exists)
                {
                    short count = results->statistics.pvp_statistics[player_index][opponent_index][1].value;
                    if (count > best_count)
                    {
                        bool enemy;
                        if (results->teams_enabled)
                            enemy = (results->players[opponent_index].configuration.host.team_index
                                - results->players[player_index].configuration.host.team_index) != 0;
                        else
                            enemy = true;
                        if (enemy)
                        {
                            best_count = count;
                            best_player = opponent_index;
                        }
                    }
                }
            }
        }
        if (best_player != -1)
        {
            visible = true;
            screen->m_second_player_index = best_player;
            screen->m_second_player_count = best_count;
        }
        c_gui_widget *nemesis_background = ((c_gui_widget *)screen)->get_child_widget(_gui_bitmap_widget, _player_select_string_killed_by_image);
        c_gui_widget *nemesis_name = ((c_gui_widget *)screen)->get_child_widget(_gui_text_widget, _player_select_string_killed_by_count);
        if (nemesis_background) nemesis_background->set_visible(visible);
        if (nemesis_icon) nemesis_icon->set_visible(visible);
        if (nemesis_count) nemesis_count->set_visible(visible);
        nemesis->set_visible(visible);
        nemesis_name->set_visible(visible);
    }
    if (!screen->m_show_service_record)
    {
        c_gui_widget *service_record = ((c_gui_widget *)screen)->get_child_widget(_gui_text_widget, _player_select_string_caption_column);
        c_gui_widget *service_record_description = ((c_gui_widget *)screen)->get_child_widget(_gui_text_widget, _player_select_string_value_column);
        if (service_record) service_record->set_visible(false);
        if (service_record_description) service_record_description->set_visible(false);
    }
}

static inline c_gui_list_widget *gui_player_select_screen_widget_get_child_list_widget(c_gui_widget *widget, long name)
{
    return (c_gui_list_widget *)widget->get_child_widget(_gui_list_widget, name);
}

// @address 0x4ce4b8 ?update@c_gui_player_select_screen_widget@@UAAXK@Z
void c_gui_player_select_screen_widget::update(unsigned long current_milliseconds)
{
    const c_controller_interface *controller = (const c_controller_interface *)&g_controller_globals.controllers[get_any_responding_controller()];
    long current_player_index = user_interface_session_player_get_player_index(&m_target_player_identifier);
    long target_player_index = user_interface_session_player_get_player_index(&m_player_identifier);
    bool show_navigation =
        (get_parent_screen() != 0 &&
         gui_player_select_screen_widget_get_child_list_widget((c_gui_widget *)get_parent_screen(), _player_select_string_roster) != 0 &&
         ((c_gui_ordered_data *)(gui_player_select_screen_widget_get_child_list_widget((c_gui_widget *)get_parent_screen(),
             _player_select_string_roster))->get_data()) != 0) ||
        ((c_gui_widget *)gui_player_select_screen_widget_get_child_list_widget(this, _player_select_string_medals_list) != 0 &&
         user_interface_networking_get_final_game_results() != 0);
    bool muted_through_guide =
        current_player_index != -1 &&
        target_player_index != -1 &&
        voice_get_local_mute_through_guide(current_player_index, target_player_index);
    c_gui_list_widget *actions_list = (c_gui_list_widget *)get_child_widget(_gui_list_widget, _player_select_string_player_select);
    c_gui_data *actions_data = actions_list ? (c_gui_data *)((c_gui_ordered_data *)(actions_list)->get_data()) : 0;

    if (controller)
    {
        s_player_identifier controller_player_identifier;
        (controller)->get_player_identifier(&controller_player_identifier);
        if (memcmp(&controller_player_identifier, &m_target_player_identifier, sizeof(s_player_identifier)) != 0)
            transition_out_with_transition_type(0, _screen_transition_normal);
    }

    if (!m_valid)
        transition_out_with_transition_type(0, _screen_transition_normal);

    if (m_name == _player_select_string_carnage_report_player_details && m_valid)
    {
        e_controller_index controller_index = get_any_responding_controller();
        if (!player_select_actions_has_available_action(controller_index, &m_target_player_identifier, &m_player_identifier, &m_configuration, false))
        {
            c_gui_list_item_widget *focused_widget = (c_gui_list_item_widget *)get_focused_widget();
            c_gui_widget *focused_list = (focused_widget && focused_widget->type == 5) ? gui_widget_get_parent_list_widget((c_gui_widget *)focused_widget) : 0;
            c_gui_widget *medals_list = get_child_widget(_gui_list_widget, _player_select_string_medals_list);
            if (medals_list && focused_list != medals_list)
            {
                this->transfer_focus_to_list((c_gui_list_widget *)medals_list, 0, false, false);
                update_player_information(0);
            }
        }
    }

    player_select_update_rank_display(this);
    update_rematch_timer();
    gui_player_select_screen_widget_set_child_visible(this, _gui_text_widget, _player_select_string_next_player, show_navigation);

    if (actions_list && actions_data)
    {
        for (c_player_select_focus_item *item = gui_widget_find_child_by_name((c_gui_widget *)actions_list, 5);
             item != 0;
             item = gui_widget_get_next_list_item(item, true))
        {
            long item_name = -1;
            if (actions_data->get_string_id_value(item->get_element_handle(), _player_select_string_name, &item_name) &&
                (item_name == _player_select_string_mute || item_name == _player_select_string_unmute))
            {
                gui_widget_set_alternate_state(item, muted_through_guide);
            }
        }
    }

    c_gui_screen_widget::update(current_milliseconds);
}

// 0x4cf4f8 c_gui_player_select_screen_widget::handle_controller_input_message: move the focused
// player selection in the roster, session-player and carnage-report lists, or the medals/actions
// lists, then fall through to the base screen handler when nothing was handled.



// @address 0x4cf4f8 ?handle_controller_input_message@c_gui_player_select_screen_widget@@UAA_NPBVc_controller_input_message@@@Z
bool c_gui_player_select_screen_widget::handle_controller_input_message(const c_controller_input_message *message)
{
    bool handled = false;
    switch (message->get_event_type())
    {
    case 2:
    {
        c_gui_list_widget *actions = (c_gui_list_widget *)get_child_widget(_gui_list_widget, _player_select_string_player_select);
        if (actions && !actions->within_focus_chain())
        {
            bool blocked = false;
            c_gui_list_widget *medals = (c_gui_list_widget *)get_child_widget(_gui_list_widget, _player_select_string_medals_list);
            if (medals && medals->within_focus_chain())
            {
                c_gui_ordered_data *data = (c_gui_ordered_data *)medals->get_data();
                if (data && data->get_current_item_count() > 0 && medals->m_focused_item_index % 3 != 0)
                    blocked = true;
            }
            if (m_valid &&
                !player_select_actions_has_available_action(
                    message->get_controller(), &m_target_player_identifier, &m_player_identifier, &m_configuration, false))
                blocked = true;
            if (!blocked)
            {
                this->transfer_focus_to_list(actions, 0, false, false);
                if (actions->within_focus_chain())
                {
                    update_player_information(-1);
                    handled = true;
                }
            }
        }
        break;
    }
    case 4:
    {
        c_gui_list_widget *medals = (c_gui_list_widget *)get_child_widget(_gui_list_widget, _player_select_string_medals_list);
        if (medals && medals->get_item_count(0) != 0 && !medals->within_focus_chain())
            handled = update_player_information(0);
        break;
    }
    case _event_type_button_press:
    {
        if ((unsigned int)(message->get_component() - 4) <= 1)
        {
        bool forward = message->get_component() == 4;
        c_gui_screen_widget *parent = get_parent_screen();
        if (parent)
        {
            s_player_identifier old_player;
            s_player_identifier candidate;
            s_player_configuration candidate_configuration;
            const s_player_identifier *new_identifier = 0;
            const s_player_configuration *new_configuration = 0;
            c_gui_widget *new_list = 0;
            long new_element = -1;
            c_gui_widget *roster_list = ((c_gui_widget *)parent)->get_child_widget(_gui_list_widget, _player_select_string_roster);
            c_gui_widget *players_list = ((c_gui_widget *)parent)->get_child_widget(_gui_list_widget, _player_select_string_scoreboard);
            c_gui_widget *results_list = ((c_gui_widget *)parent)->get_child_widget(_gui_list_widget, _player_select_string_player_list);
            old_player = m_player_identifier;
            if (roster_list)
            {
                c_gui_roster_data *data = (c_gui_roster_data *)((c_gui_list_widget *)roster_list)->get_data();
                for (long element = data->get_first_element_handle(); element != -1; element = data->get_next_element_handle(element))
                {
                    if (data->get_player_identifier(element, &candidate) &&
                        memcmp(&candidate, &old_player, sizeof(s_player_identifier)) == 0)
                    {
                        long next_element = element;
                        for (;;)
                        {
                            next_element = forward ? data->get_next_element_handle(next_element) : data->get_previous_element_handle(next_element);
                            if (next_element == -1)
                                break;
                            if (data->get_player_identifier(next_element, &candidate) &&
                                data->get_player_configuration(next_element, &candidate_configuration))
                            {
                                new_identifier = &candidate;
                                new_configuration = &candidate_configuration;
                                new_list = roster_list;
                                new_element = next_element;
                                break;
                            }
                        }
                        break;
                    }
                }
            }
            else if (players_list)
            {
                c_gui_roster_data *data = (c_gui_roster_data *)((c_gui_list_widget *)players_list)->get_data();
                for (long element = data->get_first_element_handle(); element != -1; element = data->get_next_element_handle(element))
                {
                    long element_index = -1;
                    if (data->get_integer_value(element, _player_select_string_player_index, &element_index) && element_index != -1)
                    {
                        s_data_array *array = (s_data_array *)player_data;
                        const s_player_identifier *identifier = &((player_datum *)array->data)[(unsigned short)element_index].player_identifier;
                        if (identifier && memcmp(&old_player, identifier, sizeof(s_player_identifier)) == 0)
                        {
                            long next_element = element;
                            for (;;)
                            {
                                next_element = forward ? data->get_next_element_handle(next_element) : data->get_previous_element_handle(next_element);
                                if (next_element == -1)
                                    break;
                                if (data->get_integer_value(next_element, _player_select_string_player_index, &element_index) && element_index != -1)
                                {
                                    s_data_array *array = (s_data_array *)player_data;
                                    long selected_index = element_index;
                                    player_datum *player = &((player_datum *)array->data)[(unsigned short)selected_index];
                                    new_identifier = &player->player_identifier;
                                    new_configuration = &player->configuration;
                                    new_list = players_list;
                                    new_element = next_element;
                                    break;
                                }
                            }
                            break;
                        }
                    }
                }
            }
            else if (results_list)
            {
                const c_game_results *results = user_interface_networking_get_final_game_results();
                if (results)
                {
                    short index = ::player_index_from_identifier(results, &old_player);
                    short step = forward ? 1 : -1;
                    for (short i = 0; i < 16; i++)
                    {
                        index = (short)(index + step);
                        if (index < 0)
                            index = 15;
                        else if (index >= 16)
                            index = 0;
                        if (results->players[index].player_exists)
                        {
                            new_identifier = (const s_player_identifier *)&results->players[index].identifier;
                            new_configuration = (const s_player_configuration *)&results->players[index].configuration;
                            new_list = results_list;
                            new_element = index;
                            break;
                        }
                    }
                }
            }
            if (new_identifier && memcmp(new_identifier, &old_player, sizeof(s_player_identifier)) != 0 && new_configuration)
            {
                m_player_identifier = *new_identifier;
                m_configuration = *new_configuration;
                c_gui_screen_widget *lists = (c_gui_screen_widget *)this;
                for (long i = 0; i < lists->m_datasource_count; i++)
                {
                    if (lists->m_datasource[i]->get_name() == _player_select_string_player_select)
                    {
                        c_player_select_actions_datasource *datasource = (c_player_select_actions_datasource *)lists->m_datasource[i];
                        if (datasource)
                            player_select_actions_datasource_set_target(datasource, &m_player_identifier, new_configuration);
                        break;
                    }
                }
                for (long i = 0; i < lists->m_datasource_count; i++)
                {
                    if (lists->m_datasource[i]->get_name() == _player_select_string_medals_list)
                    {
                        c_player_select_medals_datasource *datasource = (c_player_select_medals_datasource *)lists->m_datasource[i];
                        if (datasource)
                            player_select_medals_datasource_set_player(datasource, m_player_identifier);
                        break;
                    }
                }
                player_select_update_statistics_display(this);
                parent->transfer_focus_to_list((c_gui_list_widget *)new_list, new_element, true, true);
            }
        }
        }
        break;
    }
    }
    if (!handled)
        handled = this->c_gui_screen_widget::handle_controller_input_message(message);
    return handled;
}

// @address 0x4cfca0 ?handle_dialog_result@c_gui_player_select_screen_widget@@UAA_NPBVc_dialog_result_message@@@Z
bool c_gui_player_select_screen_widget::handle_dialog_result(const c_dialog_result_message *dialog_result_message)
{
    bool handled = false;
    if (dialog_result_message->get_dialog_name() == _player_select_string_confirm_boot_from_squad && dialog_result_message->get_dialog_result() == 0)
    {
        if (network_life_cycle_squad_local_peer_is_leader())
        {
            long boot_target_index = user_interface_squad_get_player_index(&m_player_identifier);
            if (boot_target_index != -1 && !user_interface_squad_is_local_player(boot_target_index))
            {
                int boot_result = 0;
                if (user_interface_squad_is_booting_allowed()
                    && user_interface_squad_is_player_valid(boot_target_index)
                    && !user_interface_squad_is_local_player(boot_target_index))
                    boot_result = network_squad_session_boot_player(boot_target_index, 4);
                if ((unsigned char)boot_result == 0)
                    user_interface_play_sound(_ui_sound_error, m_core_definition.sound_override_index);
            }
        }
        transition_out_with_transition_type(0, _screen_transition_normal);
        handled = true;
    }
    if (!handled)
    {
        // The base screen's storage-device dialog handling (gui_screen_widget_handle_storage_dialog 0x486448), inlined.
        handled = false;
        if (dialog_result_message->get_dialog_name() == _player_select_string_need_to_select_storage_device_to_save)
        {
            if (dialog_result_message->get_dialog_result() == 0)
                content_catalogue_display_device_selection_guide_interface(dialog_result_message->get_controller());
            handled = true;
        }
    }
    return handled;
}

typedef char gui_player_configuration_bounds_offset[((offsetof(s_player_configuration, client)
    + offsetof(s_player_configuration_from_client, appearance) + offsetof(s_player_appearance, emblem)) == 0x26) ? 1 : -1];
typedef char gui_player_squad_iterator_size[(sizeof(squad_actor_iterator) == 0x10) ? 1 : -1];
typedef char gui_player_implement_offset[(sizeof(void *) != 4 || ((offsetof(c_gui_player_select_screen_widget, m_implement_index) == 0x1bac))) ? 1 : -1];
typedef char gui_player_second_index_offset[
    (sizeof(void *) != 4 || ((offsetof(c_gui_player_select_screen_widget, m_second_player_index) == 0x1bb8))) ? 1 : -1];
typedef char gui_player_medals_size[(sizeof(void *) != 4 || ((sizeof(c_player_select_medals_datasource) == 0x1f8) )) ? 1 : -1];
typedef char gui_player_rematch_value_offset[(offsetof(s_life_cycle_matchmaking_progress, countdown_timer) == 0x54) ? 1 : -1];
