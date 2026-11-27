// interfacelibxbox/gui_screen_scoreboard.obj: from the records 0x8b1e0 0x8b288 0x8b8e8 0x18eba8 0x1e2f00 0x244198 0x482b68 0x48e200 0x48e250 0x48e260 0x48e268 0x48e420 0x48e488 0x48ecd8 0x48eeb0 0x48f040 0x48f198 0x48f250 0x48f2b8 0x48f4c8 0x48f4d8 0x48f600 0x48f6c8 0x48f730 0x48f9b8 0x48fab8 0x490108 0x490508 0x490910 0x4909d0 0x490b20 0x490c08 0x490ed0 0x490f48 0x491048
// @flags /TP /O2 /Oc
// @address 0x4ec80 ??_7c_load_scoreboard_screen_message@@6B@
// @address 0x4c4bb8 ??_Gc_load_scoreboard_screen_message@@UAAPAXI@Z
// @address 0x4c4bb8 ??_Ec_load_scoreboard_screen_message@@UAAPAXI@Z
// @address 0x48eeb0 ?get_column_names@c_gui_scoreboard_data@@UAAXQAJPAJ@Z
// @address 0x48f040 ?get_integer_value@c_gui_scoreboard_data@@UAA_NJJPAJ@Z
// @address 0x48f198 ?get_text_value@c_gui_scoreboard_data@@UAA_NJJPAV?$c_static_wchar_string@$0EAA@@@@Z
// @address 0x48f250 ?get_player_appearance@c_gui_scoreboard_data@@UAA_NJPAUs_player_appearance@@@Z
// @address 0x48f4c8 gui_scoreboard_data_compare_names
// @address 0x48f6c8 ?set_scoreboard_mode@c_gui_screen_scoreboard@@AAAXW4e_scoreboard_mode@1@@Z
// @address 0x490ed0 gui_screen_scoreboard_is_starting_up
// @address 0x482b68 ?user_interface_messaging_post@@YAXPBVc_message@@@Z
// @address 0x48e200 gui_screen_scoreboard_is_interactive
// @address 0x8c4948 g_user_interface_network_search_started

#include "cseries/cseries.h"
#include "cseries/language.h"
#include "interface/user_interface_allocation.h"
#include "text/unicode.h"
#include "interface/gui_screen_scoreboard_externs.h"
#include <stddef.h>
#include <stdlib.h>
#include "math/real_math_types.h"
#include "memory/data_types.h"
#include "memory/static_arrays.h"
#include "tag_files/tag_types.h"
#include "interface/c_deletable.h"
#include "shell/shell.h"
#include "game/game_globals.h"
#include "game/game_globals_storage.h"
#include "game/player_types.h"
#include "interface/gui_animation_transform.h"
#include "interface/user_interface_messages.h"
#include <ppcintrinsics.h>
#include <string.h>
#include "game/game.h"
#include "game/game_engine.h"
#include "interface/gui_screens/scoreboard/gui_screen_scoreboard.h"
#include "interface/user_interface_memory.h"
#include "interface/user_interface_session.h"
#include "simulation/simulation.h"
#include "game/game_engine_display.h"
#include "game/game_engine_scoring.h"
#include "game/game_engine_team.h"
#include "interface/user_interface_utilities.h"
#include "networking/online/voice_xenon.h"
#include "networking/online/online_globals_types.h"
#include "interface/user_interface_sound_effect_types.h"
#include "game/game_engine_default.h"
#include "tag_files/string_ids.h"
#include "saved_games/saved_film.h"
#include "game/players.h"
#include "interface/user_interface.h"
#include "interface/c_gui_list_widget.h"
#include "cache/cache_files.h"
#include "interface/c_gui_widget.h"
#include "networking/online/voice_interface_types.h"
#include "interface/gui_data.h"
#include "interface/c_gui_screen_widget.h"
#include "memory/data.h"
#include "text/text_group.h"
#include "memory/tls_globals.h"
#include "interface/gui_screens/scoreboard/gui_screen_scoreboard_types.h"
#include "interface/gui_widget.h"
#include "interface/c_gui_text_widget.h"
#include "tag_files/tag_groups.h"
#include <wchar.h>
#include "interface/user_interface_message_globals.h"
#include <new>
#include "networking/session/network_session.h"
#include "interface/user_interface_window_manager.h"
#include "interface/chud/chud.h"
#include "saved_games/saved_film_scoreboard.h"
#include "interface/gui_screens/player_select/player_select_screen_message.h"
#include "interface/controller_types.h"
#include "interface/c_gui_list_item_widget.h"
#include "interface/c_gui_bitmap_widget.h"
#include "bitmaps/bitmap_group_definitions.h"
#include "interface/gui_widget_tree.h"
#include "interface/gui_screen_widget_text.h"
#include "game/game_engine_scoring_globals.h"
#include "game/game_engine_settings_definitions.h"
#include "game/game_engine_list.h"
#include "scenario/scenario.h"
#include "cseries/crt_xdk.h"
#include "networking/logic/network_search_globals.h"





extern "C" {
unsigned char g_user_interface_network_search_started;
}

struct s_scoreboard_round_limit { long value; };

class c_load_scoreboard_screen_message : public c_load_screen_message
{
public:
    __forceinline c_load_scoreboard_screen_message(long screen_name, e_controller_index controller, long layered_position, bool interactive)
        : c_load_screen_message(screen_name, controller, (e_window_index)player_get_user_from_controller(controller), layered_position)
    {
        m_interactive = interactive;
    }
    virtual void apply_initial_state(c_gui_screen_widget *screen) const;

    bool m_interactive;
};


// 0x482b68
void user_interface_messaging_post(class c_message const * message)
{
  bool queued = false;
  bool deferred = false;
  game_globals_storage *state = tls_game_globals();
  if (state && state->game_in_progress && !state->initializing && state->map_active)
    deferred = true;
  if (deferred) {
    c_message_globals &queue = g_message_globals;
    c_message_globals::s_message_queue_node *slot = queue.m_next_write;
    if (!slot->m_message) {
      slot->m_message = message;
      queued = true;
      queue.m_next_write = queue.m_next_write->m_next;
    }
  }
  if (!queued) {
    ((c_deletable *)message)->~c_deletable();
    user_interface_free(((c_deletable *)message));
  }
}

// @address 0x48e250 ?apply_initial_state@c_load_scoreboard_screen_message@@UBAXPAVc_gui_screen_widget@@@Z
void c_load_scoreboard_screen_message::apply_initial_state(c_gui_screen_widget *screen) const
{
    static_cast<c_gui_screen_scoreboard *>(screen)->m_interactive = m_interactive;
}



// @address 0x244198 ?set@?$c_static_wchar_string@$0DA@@@QAAXPB_W@Z
template<> void c_static_wchar_string<48>::set(const wchar_t *source)
{
 ustrnzcpy_inline(m_string, source, 48);
}

// @address 0x48e260 ?get_current_item_count_internal@c_gui_scoreboard_data@@UAAJXZ
long c_gui_scoreboard_data::get_current_item_count_internal()
{
 return count;
}

// @address 0x48e268 gui_scoreboard_data_add_row
extern "C" bool gui_scoreboard_data_add_row(c_gui_scoreboard_data *data, long type, long player_index, long team_index,
    const s_player_appearance *block, const wchar_t *text_2a, const wchar_t *text_8a, long rank, long score, unsigned char flag,
    long controller, long voice, long rating, const wchar_t *text_104, const wchar_t *text_164, const wchar_t *text_1c4,
    unsigned char dead, unsigned char alternate_state)
{
 bool added = data->count < 25;
 if (added) {
  data->rows[data->count].type = type;
  data->rows[data->count].player_index = player_index;
  data->rows[data->count].session_player_index = team_index;
  memcpy(&data->rows[data->count].appearance, block, sizeof(*block));
  data->rows[data->count].text_2a.set(text_2a);
  data->rows[data->count].text_8a.set(text_8a);
  data->rows[data->count].rank = rank;
  data->rows[data->count].team_index = score;
  data->rows[data->count].flag = flag;
  data->rows[data->count].controller = controller;
  data->rows[data->count].voice = voice;
  data->rows[data->count].rating = rating;
  data->rows[data->count].text_104.set(text_104);
  data->rows[data->count].text_164.set(text_164);
  data->rows[data->count].text_1c4.set(text_1c4);
  data->rows[data->count].dead = dead;
  data->rows[data->count].alternate_state = alternate_state;
  ++data->count;
 }
 return added;
}

void c_gui_scoreboard_data::get_column_names(long *const columns, long *count) {
 *count = 0;
 columns[*count] = _scoreboard_column_type;
 *count = *count + 1;
 columns[*count] = _scoreboard_column_player_index;
 *count = *count + 1;
 columns[*count] = _scoreboard_column_session_player_index;
 *count = *count + 1;
 columns[*count] = _scoreboard_column_team;
 *count = *count + 1;
 columns[*count] = _scoreboard_column_has_teams;
 *count = *count + 1;
 columns[*count] = _scoreboard_column_base_color;
 *count = *count + 1;
 columns[*count] = _scoreboard_column_controller;
 *count = *count + 1;
 columns[*count] = _scoreboard_column_voice;
 *count = *count + 1;
 columns[*count] = _scoreboard_column_rating;
 *count = *count + 1;
 columns[*count] = _scoreboard_column_name;
 *count = *count + 1;
 columns[*count] = _scoreboard_column_place;
 *count = *count + 1;
 columns[*count] = _scoreboard_column_score;
 *count = *count + 1;
 columns[*count] = _scoreboard_column_round_score;
 *count = *count + 1;
 columns[*count] = _scoreboard_column_service_tag;
 *count = *count + 1;
}

bool c_gui_scoreboard_data::get_integer_value(long index, long column, long *value) {
 bool result=false;
 if(index>=0 && index<count) {
  const s_gui_scoreboard_player_row *row=&rows[index];
  switch(column) {
   case _scoreboard_column_type: *value=row->type; result=true; break;
   case _scoreboard_column_team: *value=row->team_index; result=true; break;
   case _scoreboard_column_has_teams: *value=row->flag; result=true; break;
   case _scoreboard_column_base_color: *value=row->rank; result=true; break;
   case _scoreboard_column_player_index: *value=row->player_index; result=true; break;
   case _scoreboard_column_dead: *value=row->dead; result=true; break;
   case _scoreboard_column_alternate_state: *value=row->alternate_state; result=true; break;
   case _scoreboard_column_session_player_index: *value=row->session_player_index; result=true; break;
   case _scoreboard_column_controller: *value=row->controller; result=true; break;
   case _scoreboard_column_voice: *value=row->voice; result=true; break;
   case _scoreboard_column_rating: *value=row->rating; result=true; break;
  }
 }
 return result;
}

bool c_gui_scoreboard_data::get_text_value(long index, long column, c_static_wchar_string<1024> *string) {
 bool result=false;
 if(index>=0 && index<count) {
  const s_gui_scoreboard_player_row *row=&rows[index];
  switch(column) {
   case _scoreboard_column_name: string->set(row->text_2a); result=true; break;
   case _scoreboard_column_service_tag: string->set(row->text_8a); result=true; break;
   case _scoreboard_column_place: string->set(row->text_104); result=true; break;
   case _scoreboard_column_score: string->set(row->text_164); result=true; break;
   case _scoreboard_column_round_score: string->set(row->text_1c4); result=true; break;
  }
 }
 return result;
}

bool c_gui_scoreboard_data::get_player_appearance(long element_handle, s_player_appearance *appearance) {
 bool result = false;
 if (element_handle >= 0 && element_handle < count) {
  const s_gui_scoreboard_player_row *row = &rows[element_handle];
  if (row->type == 0) {
   *appearance = row->appearance;
   result = true;
  }
 }
 return result;
}

// @address 0x48f2b8 ?scoreboard_sort_proc@c_gui_scoreboard_data@@SAHPBX0@Z
int c_gui_scoreboard_data::scoreboard_sort_proc(const void *a, const void *b)
{
 long result = 0;
 const s_gui_scoreboard_player_row *player_a = (const s_gui_scoreboard_player_row *)a;
 const s_gui_scoreboard_player_row *player_b = (const s_gui_scoreboard_player_row *)b;
 bool has_teams = game_engine_has_teams();
 bool a_valid;
 if (has_teams && player_a->team_index >= 0 && player_a->team_index < 8) a_valid = true; else a_valid = false;
 bool b_valid;
 if (has_teams && player_b->team_index >= 0 && player_b->team_index < 8) b_valid = true; else b_valid = false;
 long a_team;
 long b_team;
 if (player_a->type == 1 && player_b->type == 1) {
  if (a_valid && b_valid)
   result = game_engine_compare_teams(player_a->team_index, player_b->team_index, true);
  if (!result) result = player_a->team_index - player_b->team_index;
 }
 else if (player_a->type == 1 && player_b->type == 0) {
  a_team = player_a->team_index;
  b_team = player_b->team_index;
  if (a_team == b_team)
   result = -1;
  else {
   if (a_valid && b_valid)
    result = game_engine_compare_teams(a_team, b_team, true);
   if (!result) result = a_team - b_team;
  }
 }
 else if (player_a->type == 0 && player_b->type == 1) {
  a_team = player_a->team_index;
  b_team = player_b->team_index;
  if (a_team == b_team)
   result = 1;
  else {
   if (a_valid && b_valid)
    result = game_engine_compare_teams(a_team, b_team, true);
   if (!result) result = a_team - b_team;
  }
 }
 else if (has_teams && ((a_team = player_a->team_index) != (b_team = player_b->team_index))) {
  if (a_valid && b_valid)
   result = game_engine_compare_teams(a_team, b_team, true);
  if (!result) result = a_team - b_team;
 }
 else if (player_a->player_index == -1 && player_b->player_index == -1)
  result = player_b->session_player_index - player_a->session_player_index;
 else if (player_a->player_index == -1 && player_b->player_index != -1)
  result = 1;
 else if (player_a->player_index != -1 && player_b->player_index == -1)
  result = -1;
 else {
  long a_player = player_a->player_index;
  long b_player = player_b->player_index;
  result = game_engine_compare_players(a_player, b_player, true);
  if (!result) {
   if (player_a->team_index != player_b->team_index)
    result = player_a->team_index - player_b->team_index;
   else if (a_player != b_player)
    result = a_player - b_player;
  }
 }
 return result;
}

extern "C" int gui_scoreboard_data_compare_names(const s_gui_scoreboard_player_row *a, const s_gui_scoreboard_player_row *b) {
 return wcscmp(a->text_2a.get_string(), b->text_2a.get_string());
}

// @address 0x48f4d8 ?initialize_datasource@c_gui_screen_scoreboard@@UAAXXZ
void c_gui_screen_scoreboard::initialize_datasource()
{
    c_gui_screen_widget::initialize_datasource();
    c_gui_scoreboard_data *child = new (static_cast<const char *>(0), 0, _ui_allocation_marker_dummy) c_gui_scoreboard_data(get_driving_controller());
    if (child) {
        memset(child->rows, 0, sizeof(child->rows));
        if (child->initialize(_string_id_scoreboard)) {
            add_datasource(child);
        } else {
            ui_track_delete<c_gui_data>(child);
        }
    }
}

// @address 0x48f600 ?post_initialize@c_gui_screen_scoreboard@@UAAXXZ
void c_gui_screen_scoreboard::post_initialize()
{
 c_gui_screen_widget::post_initialize();
 bool in_progress = false;
 game_globals_storage *globals = tls_game_globals();
 if (globals && globals->game_in_progress && !globals->initializing && globals->map_active)
  in_progress = true;
 if (in_progress) {
  if (globals->options.game_mode == 2)
   set_scoreboard_mode((e_scoreboard_mode)(((unsigned char)game_is_cooperative() != 0) + 1));
  else
   set_scoreboard_mode(_scoreboard_mode_session_playing);
 }
 set_suppress_focus(true);
 set_responds_to_controller_events(false);
}

void c_gui_screen_scoreboard::set_scoreboard_mode(e_scoreboard_mode scoreboard_mode) {
 c_gui_data *data = 0;
 for (long test_index = 0; test_index < m_datasource_count; ++test_index) {
  if (m_datasource[test_index]->get_name() == _string_id_scoreboard) {
   data = m_datasource[test_index];
   break;
  }
 }
 if (data != 0) {
  m_scoreboard_mode = scoreboard_mode;
  ((c_gui_scoreboard_data *)data)->m_scoreboard_mode = scoreboard_mode;
 }
}

// @address 0x490910 ?translate_widget_recursive@c_gui_screen_scoreboard@@AAAXPAVc_gui_widget@@JJ@Z
void c_gui_screen_scoreboard::translate_widget_recursive(c_gui_widget *gui_widget, long x, long y)
{
 s_animation_transform transform = *gui_widget->get_animated_state();
 transform.position.x += (float)x;
 transform.position.y += (float)y;
 gui_widget->set_full_animation_state(&transform, false);
 for (c_gui_widget *child = gui_widget->get_children(); child != 0; child = child->get_next())
  translate_widget_recursive(child, x, y);
}

extern "C" bool gui_screen_scoreboard_is_starting_up(long mode) {
 bool campaign = mode == 1 || mode == 2;
 bool starting_up = campaign && simulation_starting_up() && tls_game_globals()->options.game_mode == 2;
 return starting_up;
}


// 0x48e200
extern "C" bool gui_screen_scoreboard_is_interactive(c_gui_screen_scoreboard *screen)
{
    c_gui_screen_widget *node = (c_gui_screen_widget *)screen;
    return screen->m_interactive && !node->transitioning_out();
}
static __forceinline c_gui_screen_widget *scoreboard_table_last(c_window_manager *table, long kind)
{
    c_gui_screen_widget *result = 0;
    long count = table->m_screen_count[kind].peek();
    if (count > 0) {
        long last = table->m_screen_count[kind].peek();
        result = table->m_screens[kind][last - 1];
    }
    return result;
}
// @address 0x8c4934 ?m_scoreboard_alpha@c_gui_screen_scoreboard@@2PAMA
float c_gui_screen_scoreboard::m_scoreboard_alpha[k_number_of_controllers];
// @address 0x8c4944 ?m_any_controller_scoreboard_alpha@c_gui_screen_scoreboard@@2MA
float c_gui_screen_scoreboard::m_any_controller_scoreboard_alpha;

// @address 0x8b1e0 gui_screen_scoreboard_save_text_alpha
extern "C" void gui_screen_scoreboard_save_text_alpha(int index)
{
 float value = 0.0f;
 if ((index >= 0 && index < 4) || index == k_any_controller) {
  c_gui_screen_scoreboard *screen = c_gui_screen_scoreboard::get_scoreboard_screen((e_controller_index)index);
  if (screen != 0) {
   c_gui_widget *widget = ((c_gui_widget *)screen)->get_child_widget(_gui_text_widget, _string_id_title);
   if (widget != 0) {
    s_animation_transform *state = widget->get_animated_state();
    if (state != 0)
     value = state->color.alpha;
   }
  }
  if (index == k_any_controller)
   c_gui_screen_scoreboard::m_any_controller_scoreboard_alpha = value;
  else
   c_gui_screen_scoreboard::m_scoreboard_alpha[index] = value;
 }
}

// @address 0x8b288 ?get_scoreboard_screen@c_gui_screen_scoreboard@@SAPAV1@W4e_controller_index@@@Z
c_gui_screen_scoreboard *c_gui_screen_scoreboard::get_scoreboard_screen(e_controller_index index)
{
    long kind = player_get_user_from_controller(index);
    c_gui_screen_widget *result = 0;
    c_gui_screen_widget *node = scoreboard_table_last(&g_window_manager, kind);
    while (node != 0) {
        if (node->m_name == _string_id_scoreboard || node->m_name == _string_id_scoreboard_half) {
            result = node;
            if (!node->transitioning_out())
                return (c_gui_screen_scoreboard *)node;
        }
        node = (c_gui_screen_widget *)g_window_manager.get_screen_below((e_window_index)kind, (c_gui_screen_widget *)node);
    }
    return (c_gui_screen_scoreboard *)result;
}
// @address 0x8b8e8 ?hide_scoreboard@c_gui_screen_scoreboard@@SAXW4e_controller_index@@@Z
void c_gui_screen_scoreboard::hide_scoreboard(e_controller_index index)
{
    c_gui_screen_widget *node = (c_gui_screen_widget *)get_scoreboard_screen(index);
    if (node != 0 && !node->transitioning_out())
        node->transition_out_with_transition_type(0, _screen_transition_normal);
}


// @address 0x18eba8 ?is_scoreboard_displayed@c_gui_screen_scoreboard@@SA_NW4e_controller_index@@@Z
bool c_gui_screen_scoreboard::is_scoreboard_displayed(e_controller_index controller_index)
{
    bool result = false;
    e_window_index window_index = (e_window_index)player_get_user_from_controller(controller_index);
    c_gui_screen_widget *screen = (c_gui_screen_widget *)get_scoreboard_screen(controller_index);
    result |= screen && !screen->transitioning_out();
    result |= c_window_manager::is_screen_queued_to_be_loaded(_string_id_scoreboard_half, window_index);
    result |= c_window_manager::is_screen_queued_to_be_loaded(_string_id_scoreboard, window_index);
    return result;
}

static __forceinline bool gui_screen_scoreboard_coop_message_wanted(void)
{
    bool result = false;
    if (game_is_cooperative()) {
        long limit = saved_film_get_target_tick();
        if (limit != -1 && saved_film_scoreboard_playback_tick() < limit)
            result = true;
    }
    return result;
}

// @address 0x1e2f00 gui_screen_scoreboard_update_coop_message
extern "C" void gui_screen_scoreboard_update_coop_message(void)
{
    bool show = gui_screen_scoreboard_coop_message_wanted();
    if (!show && saved_film_scoreboard_message_shown()) {
        c_gui_screen_widget *screen = (c_gui_screen_widget *)g_window_manager.get_screen_by_name(_console_window, _string_id_in_progress_mini_me);
        if (screen != 0)
            screen->transition_out_with_transition_type(0, _screen_transition_normal);
        saved_film_scoreboard_message_shown() = false;
    } else if (show && !saved_film_scoreboard_message_shown()) {
        if (g_window_manager.get_screen_by_name(_console_window, _string_id_in_progress_mini_me) == 0) {
            void *memory = user_interface_malloc_tracked(sizeof(c_load_in_progress_screen_message), 0, 0);
            c_load_in_progress_screen_message *message = memory
                ? new (memory) c_load_in_progress_screen_message(_string_id_in_progress_mini_me, k_any_controller,
                    _console_window, _scoreboard_string_film_snippet_preparing_title,
                    _scoreboard_string_film_snippet_preparing_message, 0, 1)
                : 0;
            if (message != 0) {
                ::user_interface_messaging_post((c_message const *)(c_deletable *)message);
                saved_film_scoreboard_message_shown() = true;
            }
        }
    }
}

// @address 0x4ec80 k_load_scoreboard_message_vtable

// @address 0x48f9b8 ?update_score_message@c_gui_screen_scoreboard@@AAAXXZ
void c_gui_screen_scoreboard::update_score_message()
{
    if (!gui_screen_scoreboard_is_starting_up(m_scoreboard_mode))
        return;

    c_gui_widget *text_widget = this->get_child_widget(_gui_text_widget, _scoreboard_string_message);
    if (text_widget == 0)
        return;

    int user_index;
    if (this->get_driving_controller() != k_any_controller) {
        long controller_index = this->get_any_responding_controller();
        user_index = g_controller_globals.controllers[controller_index].user_index;
        if (user_index == -1)
            return;
    } else {
        for (long controller = 0; controller < 4; controller++) {
            user_index = g_controller_globals.controllers[controller].user_index;
            if (user_index != -1)
                break;
        }
        if (user_index == -1)
            return;
    }

    wchar_t message[256];
    if (game_engine_hud_get_state_message(user_index, message, 0x100, false))
        ((c_gui_text_widget *)text_widget)->set_text(message);
}

// @address 0x491048 ?show_scoreboard@c_gui_screen_scoreboard@@SAXW4e_controller_index@@_N@Z
void c_gui_screen_scoreboard::show_scoreboard(e_controller_index controller_index, bool interactive)
{
    if (is_scoreboard_displayed(controller_index)) {
        c_gui_screen_scoreboard *screen = get_scoreboard_screen(controller_index);
        if (screen != 0)
            screen->m_interactive = interactive;
    } else {
        bool single_window = false;
        if (controller_index == k_any_controller) {
            single_window = true;
        } else {
            short user_index = g_controller_globals.controllers[controller_index].user_index;
            if (user_index != -1) {
                long window_count = 0;
                long player_window_count = 0;
                user_interface_get_number_of_render_windows(user_index, &window_count, &player_window_count);
                single_window = player_window_count == 1;
            }
        }
        c_load_scoreboard_screen_message *message = (c_load_scoreboard_screen_message *)user_interface_malloc_tracked(
            sizeof(c_load_scoreboard_screen_message), 0, 0);
        if (message != 0) {
            long screen_name = single_window ? _string_id_scoreboard : _string_id_scoreboard_half;
            new (message) c_load_scoreboard_screen_message(screen_name, controller_index, _string_id_top_most, interactive);
            ::user_interface_messaging_post(message);
        }
    }
}

// @address 0x490b20 ?select_player_in_scoreboard@c_gui_screen_scoreboard@@AAA_NW4e_controller_index@@PBUs_player_identifier@@@Z
bool c_gui_screen_scoreboard::select_player_in_scoreboard(e_controller_index controller_index, const s_player_identifier *player_identifier)
{
    bool result = false;
    long local_player_index = ::user_interface_session_get_player_at_local_controller(controller_index);
    if (local_player_index != -1) {
        const s_player_identifier *source_player = user_interface_session_is_local_player(local_player_index);
        if (source_player != 0 && player_identifier != 0) {
            long screen_name = players_get_active_user_count() > 1
                ? _string_id_scoreboard_player_select_splitscreen : _scoreboard_string_scoreboard_player_select;
            void *memory = user_interface_malloc_tracked(sizeof(c_load_player_select_screen_message), 0, 0);
            if (memory != 0) {
                c_gui_screen_scoreboard *self = this;
                long layered_position = self->m_name;
                c_load_player_select_screen_message *message = new (memory) c_load_player_select_screen_message(
                    screen_name, controller_index, get_render_window(), layered_position, source_player, player_identifier, true);
                if (message != 0) {
                    message->set_parent_screen_index(self->m_screen_index);
                    ::user_interface_messaging_post((c_message const *)(c_deletable *)message);
                    result = true;
                }
            }
        } else {
            ::user_interface_play_sound(_ui_sound_error, m_core_definition.sound_override_index);
        }
    }
    return result;
}

// @address 0x4909d0 ?handle_list_item_chosen@c_gui_screen_scoreboard@@UAA_NPBVc_controller_input_message@@JPAVc_gui_list_item_widget@@PAVc_gui_data@@@Z
bool c_gui_screen_scoreboard::handle_list_item_chosen(const c_controller_input_message *message, long list_name, c_gui_list_item_widget *item, c_gui_data *data)
{
    bool handled;
    handled = false;
    bool result;
    if (list_name == _string_id_scoreboard) {
        long player_index;
        player_index = -1;
        long team_index;
        team_index = -1;
        if (data->get_integer_value(item->get_element_handle(), _scoreboard_column_player_index, &player_index) && player_index != -1) {
            const s_player_identifier *identifier =
                &datum_get<player_datum>(tls_player_data(), player_index)->player_identifier;
            handled = select_player_in_scoreboard(message->get_controller(), identifier);
        } else {
            if (data->get_integer_value(item->get_element_handle(), _scoreboard_column_session_player_index, &team_index) && team_index != -1) {
                const s_player_identifier *identifier = user_interface_session_is_local_player(team_index);
                if (identifier != 0) {
                    int controller = message->get_controller();
                    handled = select_player_in_scoreboard((e_controller_index)controller, identifier);
                }
            }
        }

    }
    result = false;
    if (handled) {
        result = handled;
    }
    return result;
}

static __forceinline game_options *gui_screen_scoreboard_get_options()
{
    game_options *options = &tls_game_globals()->options;
    return options;
}
static __forceinline c_game_variant *gui_screen_scoreboard_get_multiplayer_variant()
{
    c_game_variant *variant = 0;
    game_options *options = gui_screen_scoreboard_get_options();
    if (options->game_mode == 2)
        variant = &options->game_variant;
    return variant;
}
static __forceinline s_scoreboard_round_limit gui_screen_scoreboard_current_round_limit()
{
    game_options *options = gui_screen_scoreboard_get_options();
    c_game_variant *variant = &options->game_variant;
    if (options->game_mode != 2)
        variant = 0;
    s_scoreboard_round_limit rounds = { variant->round_limit };
    return rounds;
}
static __forceinline bool gui_screen_scoreboard_has_multiple_rounds()
{
    return gui_screen_scoreboard_get_multiplayer_variant() != 0 &&
        gui_screen_scoreboard_current_round_limit().value > 1;
}
// @address 0x490f48 gui_screen_scoreboard_is_rounds_visible
extern "C" bool gui_screen_scoreboard_is_rounds_visible(c_gui_screen_scoreboard *screen, e_controller_index controller, long mode)
{
    bool show = gui_screen_scoreboard_has_multiple_rounds() && gui_screen_scoreboard_is_starting_up(mode);
    if ((unsigned long)controller <= 3) {
        short user_index = g_controller_globals.controllers[controller].user_index;
        if (user_index != -1) {
            long window_count = 0;
            long player_window_count = 0;
            user_interface_get_number_of_render_windows(user_index, &window_count, &player_window_count);
            if (window_count > 1)
                show = false;
        }
    }
    return show;
}

static __forceinline long gui_screen_scoreboard_voice_session()
{
    long session = 0;
    if (g_voice_interface_globals.initialized)
        session = g_voice_interface_globals.matchmaking_state;
    return session;
}
static __forceinline bool gui_screen_scoreboard_voice_private_chat(long player_index)
{
    bool away = false;
    if (g_voice_interface_globals.initialized) {
        unsigned long flags = 0;
        if (g_voice_interface_globals.player_states[player_index].status_flags.get_unsafe() & (1 << _voice_player_in_session))
            flags = g_voice_interface_globals.player_states[player_index].voice_flags;
        away = (flags & 8) != 0;
    }
    return away;
}
static __forceinline bool gui_screen_scoreboard_voice_has_voice(long player_index)
{
    bool voice = false;
    if (g_voice_interface_globals.initialized)
        voice = (g_voice_interface_globals.player_states[player_index].status_flags.get_unsafe() & (1 << _voice_player_has_voice)) != 0;
    return voice;
}
static __forceinline long gui_screen_scoreboard_voice_controller_player(e_controller_index controller)
{
    long player = -1;
    if (g_voice_interface_globals.initialized &&
        (g_voice_interface_globals.controller_states[controller].status_flags.get_unsafe() & (1 << _voice_controller_valid)))
        player = g_voice_interface_globals.controller_states[controller].player_index;
    return player;
}
static __forceinline bool gui_screen_scoreboard_voice_can_communicate(e_controller_index const &controller, long player_index)
{
    bool communication = false;
    if ((controller == k_any_controller && voice_any_local_player_can_communicate(player_index)) ||
        (controller >= 0 && controller < 4 && voice_controller_can_communicate(controller, player_index)))
        communication = true;
    return communication;
}

static __forceinline c_gui_data *gui_screen_scoreboard_find_data(c_gui_screen_widget *screen, long name)
{
    c_gui_data *result = 0;
    for (long index = 0; index < screen->m_datasource_count; ++index) {
        if (screen->m_datasource[index]->m_name == name) {
            result = screen->m_datasource[index];
            break;
        }
    }
    return result;
}
static __forceinline void gui_screen_scoreboard_set_frame(c_gui_widget *widget, long frame)
{
    ((c_gui_bitmap_widget *)widget)->m_bitmap_override = frame;
}
static __forceinline short gui_screen_scoreboard_frame_count(long bitmap_index)
{
    short count = 0;
    const bitmap_group *bitmap = TAG_GET('bitm', const bitmap_group, bitmap_index);
    if (bitmap) {
        if (bitmap->sequences.count)
            count = (short)((const bitmap_sequence *)bitmap->sequences.address)->sprites.count;
        else
            count = (short)bitmap->bitmaps.count;
    }
    return count;
}

static __forceinline const long &scoreboard_minimum(const long &a, const long &b)
{
    return a > b ? b : a;
}
// @address 0x48fab8 ?update_player_list@c_gui_screen_scoreboard@@AAAXXZ
void c_gui_screen_scoreboard::update_player_list()
{
    c_gui_widget *list = this->get_child_widget(_gui_list_widget, _string_id_scoreboard);
    c_gui_data *data = gui_screen_scoreboard_find_data(this, _string_id_scoreboard);
    if (list && data) {
        bool teams = game_engine_has_teams();
        for (c_gui_widget *item = gui_widget_find_child_by_name(list, _gui_list_item_widget); item; item = gui_widget_get_next_list_item(item, true)) {
            c_gui_widget *score = item->get_child_widget(_gui_text_widget, _scoreboard_column_place);
            c_gui_widget *name = item->get_child_widget(_gui_text_widget, _string_id_name);
            c_gui_widget *team_name = item->get_child_widget(_gui_text_widget, _scoreboard_string_team_bar);
            c_gui_widget *service_tag = item->get_child_widget(_gui_text_widget, _scoreboard_column_score);
            c_gui_widget *voice = item->get_child_widget(_gui_bitmap_widget, _scoreboard_string_ring_of_light);
            c_gui_widget *rank = item->get_child_widget(_gui_bitmap_widget, _scoreboard_column_rating);
            c_gui_widget *party = item->get_child_widget(_gui_bitmap_widget, _scoreboard_string_player_emblem);
            c_gui_widget *player_color = item->get_child_widget(_gui_bitmap_widget, 69);
            c_gui_widget *base_color = item->get_child_widget(_gui_bitmap_widget, _scoreboard_column_base_color);
            c_gui_widget *unknown = item->get_child_widget(_gui_bitmap_widget, _scoreboard_string_observer);
            c_gui_widget *team_color = item->get_child_widget(_gui_bitmap_widget, _scoreboard_string_team_bar);
            long element = item->get_element_handle();
            long row_type = 0;
            long voice_frame = -1;
            long voice_state = 0;
            long rank_frame = -1;
            long team = 0;
            long in_party = 0;
            long alternate_state = 0;
            if (name && team_name && score && service_tag && voice && rank && party && player_color && unknown && team_color &&
                data->get_integer_value(element, _scoreboard_column_type, &row_type) &&
                data->get_integer_value(element, _scoreboard_column_controller, &voice_frame) &&
                data->get_integer_value(element, _scoreboard_column_voice, &voice_state) &&
                data->get_integer_value(element, _scoreboard_column_rating, &rank_frame) &&
                data->get_integer_value(element, _scoreboard_column_team, &team) &&
                data->get_integer_value(element, _scoreboard_column_dead, &in_party) &&
                data->get_integer_value(element, _scoreboard_column_alternate_state, &alternate_state)) {
                bool player_row = row_type == 0;
                bool team_row = row_type == 1;
                long state = voice_state;
                bool is_party = in_party != 0;
                item->set_enabled(player_row);
                name->set_visible(player_row);
                team_name->set_visible(team_row);
                base_color->set_visible(player_row);
                unsigned char score_visible = teams ? team_row : player_row;
                score->set_visible(score_visible != 0);
                service_tag->set_visible(true);
                bool show_rounds = (unsigned char)gui_screen_scoreboard_is_rounds_visible(
                                       this, this->get_driving_controller(), m_scoreboard_mode) != 0;
                c_gui_widget *rounds = item->get_child_widget(_gui_text_widget, _scoreboard_column_round_score);
                if (rounds)
                    rounds->set_visible(show_rounds);
                party->set_visible(player_row && !is_party);
                player_color->set_visible(player_row && is_party);
                unknown->set_visible(false);
                team_color->set_visible(team_row);
                gui_widget_set_alternate_state(item, player_row && alternate_state != 0);
                rank->set_visible(rank_frame != -1);
                long last_frame = gui_screen_scoreboard_frame_count(((c_gui_bitmap_widget *)rank)->m_definition.bitmap_definition_index) - 1;
                gui_screen_scoreboard_set_frame(rank, scoreboard_minimum(rank_frame, last_frame));
                if (state == 2) {
                    voice->set_visible(true);
                    gui_screen_scoreboard_set_frame(voice, 5);
                } else if (state == 3) {
                    voice->set_visible(true);
                    gui_screen_scoreboard_set_frame(voice, 6);
                } else if (voice_frame != -1) {
                    voice->set_visible(true);
                    gui_screen_scoreboard_set_frame(voice, voice_frame);
                } else if (state == 1) {
                    voice->set_visible(true);
                    gui_screen_scoreboard_set_frame(voice, 4);
                } else if (state == 4) {
                    voice->set_visible(true);
                    gui_screen_scoreboard_set_frame(voice, 7);
                } else {
                    voice->set_visible(false);
                }
            }
        }
    }
}

static __forceinline player_datum *gui_screen_scoreboard_get_player(long player_index)
{
    s_data_array *array = (s_data_array *)tls_player_data();
    return datum_get<player_datum>(array, player_index);
}
static __forceinline bool gui_screen_scoreboard_xuid_is_guest(unsigned __int64 xuid)
{
    bool guest = false;
    if (xuid != 0) {
        unsigned __int64 flags = xuid >> 48;
        if ((flags & 15) == 9 && (flags & 192) != 0)
            guest = true;
    }
    return guest;
}

static __forceinline long gui_screen_scoreboard_pixel_coordinate(float value)
{
    return (long)value;
}

// @address 0x490c08 ?handle_controller_input_message@c_gui_screen_scoreboard@@UAA_NPBVc_controller_input_message@@@Z
bool c_gui_screen_scoreboard::handle_controller_input_message(const c_controller_input_message *message)
{
    bool handled = false;
    bool suppress = false;
    switch (message->get_event_type()) {
    case _event_type_button_press:
        if (message->get_component() != _controller_component_button_x) {
            if ((unsigned long)(message->get_component() - 4) <= 1) {
            if (m_scoreboard_mode == 2 || m_scoreboard_mode == _scoreboard_mode_session_film) {
                set_scoreboard_mode((e_scoreboard_mode)(m_scoreboard_mode == 2 ? _scoreboard_mode_session_film : 2));
                handled = true;
            }
        }
        } else {
            if (m_scoreboard_mode == 1 || m_scoreboard_mode == _scoreboard_mode_session_playing ||
                m_scoreboard_mode == _scoreboard_mode_session_film) {
                long local_player = ::user_interface_session_get_player_at_local_controller(message->get_controller());
                c_gui_list_widget *list = (c_gui_list_widget *)this->get_child_widget(_gui_list_widget, _string_id_scoreboard);
                c_gui_data *data = list ? (list)->get_data() : 0;
                if (local_player != -1 && list && data) {
                    long player = -1;
                    if (data->get_integer_value((list)->get_focused_element_handle(), _scoreboard_column_session_player_index, &player) && player != -1) {
                        const long selected_player = player;
                        bool local = user_interface_session_is_local_peer_player(selected_player);
                        bool same_player = local_player == selected_player;
                        unsigned __int64 xuid = user_interface_session_get_player_xuid_if_usable(selected_player);
                        unsigned long flags = (unsigned long)(xuid >> 48);
                        bool guest = (flags & 15) == 9 && (flags & 192) != 0;
                        if (!same_player && !local && !guest) {
                            voice_set_mute(local_player, selected_player, !voice_get_mute(local_player, selected_player));
                            handled = true;
                        }
                    }
                }
            }
        }
        break;
    case _event_type_tab_up:
    case _event_type_tab_left:
    case _event_type_tab_down:
    case _event_type_tab_right:
        suppress = get_render_window() != 4;
        handled = true;
        break;
    case _event_type_alt_tab_up:
    case _event_type_alt_tab_down:
        {
            e_event_type event = message->get_event_type() == _event_type_alt_tab_up ? _event_type_tab_up : _event_type_tab_down;
            c_controller_input_message translated(message->get_screen_name(), message->get_controller(),
                (e_window_index)message->get_window(), event, _controller_component_right_stick, message->get_event_value());
            handled = c_gui_screen_widget::handle_controller_input_message(&translated);
        }
        break;
    }
    if (!handled && !suppress)
        handled = c_gui_screen_widget::handle_controller_input_message(message);
    return handled;
}
// @address 0x48ecd8 ?get_voice_for_player@c_gui_scoreboard_data@@QAA?AW4e_voice_talking_state@1@J@Z
c_gui_scoreboard_data::e_voice_talking_state c_gui_scoreboard_data::get_voice_for_player(long player_index)
{
    e_voice_talking_state state = _voice_none;
    if (gui_screen_scoreboard_voice_session() == 1 || gui_screen_scoreboard_voice_session() == 2) {
        bool audible;
        {
            c_gui_scoreboard_data *self = this;
            audible = gui_screen_scoreboard_voice_can_communicate(self->m_controller_index, player_index);
        }
        if (audible) {
            if (gui_screen_scoreboard_voice_private_chat(player_index))
                state = _voice_private_chat;
            if (gui_screen_scoreboard_voice_has_voice(player_index)) {
                if (state == _voice_none)
                    state = (voice_player_is_talking(player_index) ? _voice_talking : _voice_available);
                c_gui_scoreboard_data *self = this;
                e_controller_index const &controller = self->m_controller_index;
                if (controller == k_any_controller && voice_any_local_player_muting(player_index))
                    state = _voice_muted;
                else if (controller >= 0 && controller < 4) {
                    long local_player = gui_screen_scoreboard_voice_controller_player(controller);
                    if (local_player != -1 && voice_get_mute(local_player, player_index))
                        state = _voice_muted;
                }
            }
        }
    }
    return state;
}

// @address 0x490508 ?update_render_state@c_gui_screen_scoreboard@@UAAXK@Z
void c_gui_screen_scoreboard::update_render_state(unsigned long current_milliseconds)
{
    c_gui_widget *list = this->get_child_widget(_gui_list_widget, _string_id_scoreboard);
    c_gui_data *data = gui_screen_scoreboard_find_data(this, _string_id_scoreboard);
    this->c_gui_widget::update_render_state(current_milliseconds);
    if (list && data) {
        for (c_gui_widget *item = gui_widget_find_child_by_name(list, _gui_list_item_widget); item; item = gui_widget_get_next_list_item(item, true)) {
            c_gui_widget *base_color = item->get_child_widget(_gui_bitmap_widget, _scoreboard_column_base_color);
            c_gui_widget *name = item->get_child_widget(_gui_text_widget, _string_id_name);
            c_gui_widget *team_name = item->get_child_widget(_gui_text_widget, _scoreboard_string_team_bar);
            c_gui_widget *service_tag = item->get_child_widget(_gui_text_widget, _scoreboard_column_service_tag);
            c_gui_widget *score = item->get_child_widget(_gui_text_widget, _scoreboard_column_place);
            c_gui_widget *round_score = item->get_child_widget(_gui_text_widget, _scoreboard_column_score);
            c_gui_widget *rounds = item->get_child_widget(_gui_text_widget, _scoreboard_column_round_score);
            c_gui_widget *team_color = item->get_child_widget(_gui_bitmap_widget, _scoreboard_string_team_bar);
            long element = item->get_element_handle();
            long row_type = 0;
            long team = -1;
            long use_team_color = 0;
            long color;
            if (data->get_integer_value(element, _scoreboard_column_type, &row_type) &&
                data->get_integer_value(element, _scoreboard_column_base_color, &color)) {
                data->get_integer_value(item->get_element_handle(), _scoreboard_column_team, &team);
            }
            if (data->get_integer_value(element, _scoreboard_column_has_teams, &use_team_color)) {
                bool teams = game_engine_has_teams() && use_team_color != 0;
                long tint = team;
                if (!teams) tint = color;
                tint_widget_to_change_color(base_color, tint, teams);
                tint_widget_to_change_color(name, tint, teams);
                tint_widget_to_change_color(team_name, tint, teams);
                tint_widget_to_change_color(service_tag, tint, teams);
                tint_widget_to_change_color(score, tint, teams);
                tint_widget_to_change_color(round_score, tint, teams);
                tint_widget_to_change_color(rounds, tint, teams);
                tint_widget_to_change_color(team_color, tint, true);
            }
        }
    }
    c_gui_widget *player_list = this->get_child_widget(_gui_list_widget, _string_id_scoreboard);
    c_gui_widget *footer = gui_widget_find_child_by_name((c_gui_widget *)this, _gui_button_key_widget);
    if (player_list && footer) {
        long last_valid_bottom = 0;
        c_gui_widget *item = gui_widget_find_child_by_name(player_list, _gui_list_item_widget);
        if (item) {
        long last_bottom;
        do {
            real_rectangle2d bounds;
            item->get_current_bounds(&bounds);
            if (item->get_element_handle() != -1) {
                last_valid_bottom = (long)bounds.y0;
                last_bottom = (long)bounds.y0;
            }
            else
                last_bottom = gui_screen_scoreboard_pixel_coordinate(bounds.y0);
            item = gui_widget_get_next_list_item(item, false);
        } while (item);
        if (last_valid_bottom != last_bottom)
            translate_widget_recursive(footer, 0, last_valid_bottom - last_bottom);
        }
    }
}

extern "C" {
}
static __forceinline s_game_engine_globals *scoreboard_game_engine_globals()
{
    return (s_game_engine_globals *)tls_game_engine_globals();
}


static __forceinline bool scoreboard_session_established(const c_network_session *session)
{
    return (unsigned long)session->m_local_state >= 4 && (unsigned long)session->m_local_state < 10;
}

static __forceinline bool scoreboard_get_interactive_session(c_network_session **session)
{
    return user_interface_get_interactive_session(session) && scoreboard_session_established(*session);
}

static __forceinline long scoreboard_get_session_player_index(const s_player_identifier *identifier)
{
    c_network_session *session = 0;
    long session_player_index = -1;
    if (scoreboard_get_interactive_session(&session))
        session_player_index = user_interface_session_get_player_index(identifier, session);
    return session_player_index;
}

static __forceinline bool scoreboard_game_is_finished(void)
{
    return tls_game_globals()->game_finished;
}

static __forceinline c_game_engine *scoreboard_current_game_engine(void)
{
    long index = scoreboard_game_engine_globals()->game_engine_index;
    if (index < 1)
        return 0;
    if (index > 10)
        return 0;
    return game_engines[index];
}

static __forceinline c_game_statborg *scoreboard_statborg_in_range(void)
{
    return scoreboard_current_game_engine() ? &scoreboard_game_engine_globals()->statborg : 0;
}

static __forceinline int scoreboard_team_out_of_range(long team_index)
{
    if (team_index < 0)
        return 1;
    if (team_index >= 8)
        return 1;
    return 0;
}

static __forceinline bool scoreboard_game_engine_has_teams_inline(void)
{
    return scoreboard_current_game_engine() && TEST_BIT(scoreboard_game_engine_globals()->flags, 3);
}

static __forceinline bool scoreboard_team_is_ever_active(long team_index)
{
    return scoreboard_game_engine_has_teams_inline() && !scoreboard_team_out_of_range(team_index) &&
        TEST_BIT(scoreboard_game_engine_globals()->ever_active_teams, team_index);
}

static __forceinline long scoreboard_get_current_language(void)
{
    if (g_current_language == -1)
        g_current_language = get_e_language_from_xbox_language(XTLGetLanguage());
    return g_current_language;
}

static __forceinline void scoreboard_string_list_get_normal_string(long tag_index, long string_id, c_static_wchar_string<256> *string)
{
    long language = scoreboard_get_current_language();
    string_list_get_normal_string_by_language(tag_index, string_id, language, string);
}

static __forceinline void scoreboard_get_team_name(long team_index, c_static_wchar_string<256> *team_name)
{
    s_game_globals *game_globals = global_game_globals;
    const s_multiplayer_globals_definition *multiplayer =
        TAG_GET('mulg', const s_multiplayer_globals_definition, game_globals->multiplayer_globals_index);
    team_name->clear();
    if (multiplayer->universal.count > 0) {
        const s_multiplayer_universal_globals_definition *universal =
            (const s_multiplayer_universal_globals_definition *)multiplayer->universal.address;
        if (universal->team_names.index != -1) {
            long team_string_id = string_id_number(team_index);
            scoreboard_string_list_get_normal_string(universal->team_names.index, team_string_id, team_name);
        }
    }
}

// @address 0x48e488 ?update_for_scoreboard_mode@c_gui_scoreboard_data@@QAAX_N0@Z
void c_gui_scoreboard_data::update_for_scoreboard_mode(bool teams, bool round)
{
    bool has_teams = round && game_engine_has_teams();
    unsigned long active_team_mask = 0;
    c_static_wchar_string<256> place_string(L" ");
    c_static_wchar_string<256> score_string(L" ");
    c_static_wchar_string<256> round_score_string(L" ");

    count = 0;
    memset(rows, 0, sizeof(rows));

    if (teams) {
        for (long session_player_index = 0; session_player_index < 16; ++session_player_index) {
            if (user_interface_session_is_player_valid(session_player_index)) {
                const s_player_configuration *player_data = user_interface_session_get_player_data(session_player_index);
                if (player_data->host.team_index != -1) {
                    long rating = game_engine_get_player_rating(session_player_index);
                    long controller_index = user_interface_session_is_local_peer_player(session_player_index)
                        ? user_interface_session_get_controller_index(session_player_index)
                        : -1;
                    long color = player_data->client.appearance.primary_color;
                    gui_scoreboard_data_add_row(
                        this,
                        0,
                        -1,
                        session_player_index,
                        &player_data->client.appearance,
                        player_data->host.name,
                        player_data->client.appearance.service_tag,
                        color,
                        -1,
                        0,
                        controller_index,
                        get_voice_for_player(session_player_index),
                        rating,
                        place_string.get_string(),
                        score_string.get_string(),
                        round_score_string.get_string(),
                        0,
                        0);
                }
            }
        }
    } else {
        c_data_iterator<player_datum> iterator;
        iterator.begin((s_data_array *)tls_player_data());
        while (iterator.next()) {
            const player_datum *player = iterator.get_datum();
            const s_player_configuration *player_data = &player->configuration;
            long session_player_index = scoreboard_get_session_player_index(&player->player_identifier);
            if (player_data->host.team_index != -1) {
                bool is_dead = !TEST_BIT(player->flags, 3) &&
                    player->unit_index == -1 &&
                    simulation_starting_up() &&
                    !scoreboard_game_is_finished() &&
                    !simulation_is_pending_start() &&
                    game_engine_in_round();
                long player_index = iterator.get_index();
                if (round) {
                    if (has_teams) {
                        long compound_rank = 0;
                        if (scoreboard_statborg_in_range())
                            compound_rank = game_engine_scoring_get_team_compound_rank(
                                player_data->host.team_index, scoreboard_game_is_finished() && game_engine_get_statborg()->m_finalized);
                        game_engine_get_place_string(compound_rank / 2, &place_string);
                    } else {
                        long compound_rank = 0;
                        if (scoreboard_statborg_in_range())
                            compound_rank = game_engine_scoring_get_player_compound_rank(
                                player_index, scoreboard_game_is_finished() && game_engine_get_statborg()->m_finalized);
                        game_engine_get_place_string(compound_rank / 2, &place_string);
                    }
                    game_engine_get_score_string(game_engine_get_player_score(player_index, true), &score_string);
                    game_engine_get_score_string(game_engine_get_player_score(player_index, false), &round_score_string);
                }
                if (has_teams)
                    active_team_mask |= 1 << player_data->host.team_index;
                if (session_player_index != -1) {
                    long rating = game_engine_get_player_rating(session_player_index);
                    long controller_index = user_interface_session_is_local_peer_player(session_player_index)
                        ? user_interface_session_get_controller_index(session_player_index)
                        : -1;
                    long color = player_data->client.appearance.primary_color;
                    gui_scoreboard_data_add_row(
                        this,
                        0,
                        player_index,
                        session_player_index,
                        &player_data->client.appearance,
                        player_data->host.name,
                        player_data->client.appearance.service_tag,
                        color,
                        player_data->host.team_index,
                        game_engine_has_teams(),
                        controller_index,
                        get_voice_for_player(session_player_index),
                        rating,
                        place_string.get_string(),
                        score_string.get_string(),
                        round_score_string.get_string(),
                        is_dead,
                        0);
                } else {
                    gui_scoreboard_data_add_row(
                        this,
                        0,
                        player_index,
                        -1,
                        &player_data->client.appearance,
                        player_data->host.name,
                        player_data->client.appearance.service_tag,
                        player_data->client.appearance.primary_color,
                        player_data->host.team_index,
                        game_engine_has_teams(),
                        -1,
                        0,
                        -1,
                        place_string.get_string(),
                        score_string.get_string(),
                        round_score_string.get_string(),
                        is_dead,
                        TEST_BIT(player->flags, 1));
                }
            }
        }
    }

    if (has_teams) {
        c_static_wchar_string<256> team_name;
        s_player_appearance empty_appearance;
        memset(&empty_appearance, 0, sizeof(empty_appearance));
        for (long team_index = 0; team_index < 8; ++team_index) {
            if (scoreboard_team_is_ever_active(team_index) || TEST_BIT(active_team_mask, team_index)) {
                if (round) {
                    long team_place = 0;
                    long team_score = 0;
                    long team_round_score = 0;
                    if (scoreboard_team_is_ever_active(team_index)) {
                        team_place = game_engine_find_team_place(team_index);
                        team_score = game_engine_get_scores_player_score(team_index, true);
                        team_round_score = game_engine_get_scores_player_score(team_index, false);
                    }
                    game_engine_get_place_string(team_place, &place_string);
                    game_engine_get_score_string(team_score, &score_string);
                    game_engine_get_score_string(team_round_score, &round_score_string);
                }
                scoreboard_get_team_name(team_index, &team_name);
                gui_scoreboard_data_add_row(
                    this,
                    1,
                    -1,
                    -1,
                    (const s_player_appearance *)&empty_appearance,
                    team_name.get_string(),
                    L" ",
                    -1,
                    team_index,
                    1,
                    -1,
                    0,
                    -1,
                    place_string.get_string(),
                    score_string.get_string(),
                    round_score_string.get_string(),
                    0,
                    0);
            }
        }
    }

    if (teams)
        qsort(rows, count, sizeof(rows[0]), (int (*)(const void *, const void *))gui_scoreboard_data_compare_names);
    else
        qsort(rows, count, sizeof(rows[0]), scoreboard_sort_proc);
}

// @address 0x48e420 ?update@c_gui_scoreboard_data@@UAAXXZ
void c_gui_scoreboard_data::update()
{
    long mode = m_scoreboard_mode;
    if (mode == 1 || mode == 2)
        update_for_scoreboard_mode(false, gui_screen_scoreboard_is_starting_up(mode));
    else if (mode == c_gui_screen_scoreboard::_scoreboard_mode_session_playing || mode == c_gui_screen_scoreboard::_scoreboard_mode_session_film)
        update_for_scoreboard_mode(true, false);
}

static __forceinline void gui_screen_scoreboard_set_child_visible(c_gui_widget *widget, e_gui_widget_type type, long name, bool visible)
{
    c_gui_widget *child = widget->get_child_widget(type, name);
    if (child)
        child->set_visible(visible);
}

// @address 0x48f730 ?update@c_gui_screen_scoreboard@@UAAXK@Z
void c_gui_screen_scoreboard::update(unsigned long current_milliseconds)
{
    c_gui_screen_widget::update(current_milliseconds);
    set_suppress_focus(!get_is_interactive());
    set_responds_to_controller_events(get_is_interactive());
    update_score_message();
    update_player_list();
    update_button_key();

    if (gui_screen_scoreboard_is_rounds_visible(this, get_driving_controller(), m_scoreboard_mode))
        gui_screen_widget_set_display_group(this, 0, _scoreboard_column_round_score);
    else
        gui_screen_widget_set_display_group(this, 0, _scoreboard_column_score);

    c_gui_widget *list = get_child_widget(_gui_list_widget, _string_id_scoreboard);
    if (list && !list->within_focus_chain())
        transfer_focus_to_list((c_gui_list_widget *)list, 0, false, false);

    gui_screen_scoreboard_set_child_visible(this, _gui_text_widget, _scoreboard_column_score, gui_screen_scoreboard_is_starting_up(m_scoreboard_mode));
    gui_screen_scoreboard_set_child_visible(this, _gui_text_widget, _scoreboard_column_round_score,
        gui_screen_scoreboard_is_rounds_visible(this, get_driving_controller(), m_scoreboard_mode));
    gui_screen_scoreboard_set_child_visible(this, _gui_text_widget, _scoreboard_string_message, gui_screen_scoreboard_is_starting_up(m_scoreboard_mode));

    long text_id;
    long mode = m_scoreboard_mode;
    if (mode == 1)
        text_id = _scoreboard_string_game;
    else if (mode == 2)
        text_id = gui_screen_scoreboard_is_interactive(this) ? _scoreboard_string_game_film : _scoreboard_string_game;
    else if (mode == _scoreboard_mode_session_playing)
        text_id = _scoreboard_string_session;
    else if (mode == _scoreboard_mode_session_film)
        text_id = gui_screen_scoreboard_is_interactive(this) ? _scoreboard_string_session_film : _scoreboard_string_session;
    else
        return;
    set_text_widget_string_id(_string_id_title, text_id);
}

static __forceinline bool gui_screen_scoreboard_guest_account(unsigned __int64 xuid)
{
    unsigned long account_flags = (unsigned long)(xuid >> 48);
    return (account_flags & 15) == 9 && (account_flags & 192) != 0;
}



static __forceinline bool scoreboard_session_mode(long mode)
{
    return mode == 1 || mode == c_gui_screen_scoreboard::_scoreboard_mode_session_playing ||
        mode == c_gui_screen_scoreboard::_scoreboard_mode_session_film;
}

// @address 0x490108 ?update_button_key@c_gui_screen_scoreboard@@AAAXXZ
void c_gui_screen_scoreboard::update_button_key()
{
    c_gui_screen_scoreboard *screen = this;
    long pause_reason = 0;
    long default_button_key;
    if (game_is_scoreboard_blocked(&pause_reason)) {
        default_button_key = _scoreboard_string_button_key_empty;
        switch (pause_reason) {
        case 1:
            gui_screen_widget_set_display_group(screen, 1, _scoreboard_string_scoreboard_busy_start_game);
            return;
        case 3:
            gui_screen_widget_set_display_group(screen, 1, _scoreboard_string_scoreboard_busy_selecting_new_host);
            return;
        case 4:
            gui_screen_widget_set_display_group(screen, 1, _scoreboard_string_scoreboard_busy_connecting_new_host);
            return;
        case 2:
        case 5:
            gui_screen_widget_set_display_group(screen, 1, _scoreboard_string_scoreboard_busy_end_game);
            return;
        default:
            break;
        }
    } else if (gui_screen_scoreboard_is_interactive(screen)) {
        c_gui_widget *widget = (c_gui_widget *)screen;
        c_gui_list_widget *list = (c_gui_list_widget *)widget->get_child_widget(_gui_list_widget, _string_id_scoreboard);
        c_gui_data *data = list ? list->get_data() : 0;
        if (list && data) {
            bool updated = false;
            long player_index = -1;
            long session_player_index = -1;
            if (!data->get_integer_value(list->get_focused_element_handle(), _scoreboard_column_player_index, &player_index))
                player_index = -1;
            if (!data->get_integer_value(list->get_focused_element_handle(), _scoreboard_column_session_player_index, &session_player_index))
                session_player_index = -1;
            if (player_index != -1 || session_player_index != -1) {
                player_datum *player = player_index != -1 ? gui_screen_scoreboard_get_player(player_index) : 0;
                unsigned __int64 xuid = player ? user_interface_session_find_player_xuid(&player->player_identifier)
                                               : user_interface_session_player_get_usable_player_xuid(session_player_index);
                bool guest = xuid != 0 && gui_screen_scoreboard_guest_account(xuid);
                e_controller_index controller = widget->get_driving_controller();
                long local_player_index = VALID_INDEX(controller, 4) && user_interface_session_get_player_at_local_controller(controller);
                if (scoreboard_session_mode(screen->m_scoreboard_mode) &&
                    local_player_index != -1 && session_player_index != -1) {
                    bool same_player = local_player_index == session_player_index;
                    bool local_player = user_interface_session_is_local_peer_player(session_player_index);
                    const s_player_identifier *source = user_interface_session_is_local_player(local_player_index);
                    const s_player_identifier *destination = user_interface_session_is_local_player(session_player_index);
                    if (VALID_INDEX(controller, 4) && source && destination) {
                        if (same_player || guest || local_player)
                            gui_screen_widget_set_display_group(screen, 1, _scoreboard_string_button_key_a_select);
                        else if (voice_get_mute(local_player_index, session_player_index))
                            gui_screen_widget_set_display_group(screen, 1, _scoreboard_string_button_key_x_unmute_a_select);
                        else
                            gui_screen_widget_set_display_group(screen, 1, _scoreboard_string_button_key_x_mute_a_select);
                    } else {
                        gui_screen_widget_set_display_group(screen, 1, _scoreboard_string_button_key_empty);
                    }
                    updated = true;
                } else {
                    if (xuid != 0 && !guest && VALID_INDEX(controller, 4) &&
                        ((c_controller_interface *)&g_controller_globals.controllers[controller])->is_signed_in_to_machine())
                        gui_screen_widget_set_display_group(screen, 1, _scoreboard_string_button_key_a_select);
                    else
                        gui_screen_widget_set_display_group(screen, 1, _scoreboard_string_button_key_empty);
                    updated = true;
                }
            }
            if (!updated)
                gui_screen_widget_set_display_group(screen, 1, _scoreboard_string_button_key_empty);
        }
        return;
    } else {
        default_button_key = _scoreboard_string_back;
    }
    gui_screen_widget_set_display_group(screen, 1, default_button_key);
}

typedef char scoreboard_row_value_offset[(offsetof(s_gui_scoreboard_player_row, controller) == 0xf8) ? 1 : -1];
typedef char scoreboard_row_size[(sizeof(s_gui_scoreboard_player_row) == 0x228) ? 1 : -1];
