// interfacelibxbox/start_menu_game_multiplayer.obj: from the records 0x4aa568 0x4aa840 0x4aaac0 0x4aacd0 0x4aae38 0x4ab0e0 0x4ab160 0x4ab1d8 0x4ab428 0x4ab5f8 0x4ab708 0x4ab740
// @flags /TP /O2 /Oc
// @address 0x483310 ??0c_load_dialog_screen_message@@QAA@W4e_controller_index@@W4e_window_index@@JJJ@Z
// @address 0x484e90 ?user_interface_malloc_tracked@@YAPAXIPBDJ@Z
// @address 0x486578 ?get_render_window@c_gui_screen_widget@@QAA?AW4e_window_index@@XZ
// @address 0x4e8a28 ?close_start_menu@c_start_menu_pane_screen_widget@@QAAXXZ
// @address 0x4e8c08 ?handle_list_item_chosen@c_start_menu_pane_screen_widget@@UAA_NPBVc_controller_input_message@@JPAVc_gui_list_item_widget@@PAVc_gui_data@@@Z
// @address 0x1b7030 ?in_use@c_controller_interface@@QBA_NXZ
// @address 0x8a4c8 ?is_signed_in_to_machine@c_controller_interface@@QBA_NXZ
// @address 0x489948 ?get_any_responding_controller@c_gui_widget@@QBA?AW4e_controller_index@@XZ
// @address 0x1e4640 ?main_menu_launch@@YAXXZ
// @address 0x3b08f8 ?network_life_cycle_in_matchmaking@@YA_NXZ
// @address 0x495440 ?network_squad_session_get_hopper@@YAGXZ
// @address 0x3b3110 ?network_squad_session_get_session_class@@YA?AW4e_network_session_class@@XZ
// @address 0x1d9b70 ?player_try_and_get_player_index_from_absolute_player_index@@YAJJ@Z
// @address 0x47bd98 ?set_as_unsigned_in_user@c_controller_interface@@QAAX_N@Z
// @address 0x46fc30 ?user_interface_controller_update_network_properties@@YAXW4e_controller_index@@@Z
// @address 0x48bfa0 ?get_child_widget@c_gui_widget@@QAAPAV1@W4e_gui_widget_type@@J@Z
// @address 0x3b4e98 ?network_life_cycle_session_get_map@@YA_NPAW4e_campaign_id@@PAW4e_map_id@@@Z
// @address 0x3b5178 ?user_interface_game_settings_get_game_variant@@YAPBVc_game_variant@@XZ
// @address 0x4927d8 ?set_map_image@c_gui_custom_bitmap_widget@@QAAXW4e_custom_map_image_type@@W4e_map_id@@_N@Z
// @address 0x4e8828 ?update@c_start_menu_pane_screen_widget@@UAAXK@Z
// @address 0x46b890 ?user_interface_squad_get_player_count@@YAJXZ

#include "interface/gui_screens/boot_betrayer/gui_screen_boot_betrayer.h"
#include "interface/gui_network_session_advertisement_types.h"
#include "interface/user_interface_squad_session.h"
#include "interface/start_menu_game_multiplayer_strings.h"
#include "interface/custom_map_image_types.h"
#include "memory/tls_globals.h"
#include "shell/shell.h"
#include <stddef.h>
#include "game/game_globals_storage.h"
#include "game/game_types.h"
#include "interface/gui_widget_types.h"
#include "interface/gui_widget.h"
#include <ppcintrinsics.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include "game/map_variant.h"
#include "interface/user_interface_messages.h"
#include "game/players.h"
#include "interface/gui_screens/scoreboard/gui_screen_scoreboard.h"
#include "interface/gui_game_mode_types.h"
#include "game/game_engine_scoring_globals.h"
#include "game/game_engine_default.h"
#ifdef HALO3_64BIT
#include "game/game_engine_sandbox.h"
#endif
#include "interface/user_interface_memory.h"
#include "interface/user_interface_session.h"
#include "networking/logic/network_session_interface.h"
#include "text/unicode.h"
#include "game/multiplayer_game_hopper.h"
#include "interface/user_interface_matchmaking.h"
#include "main/main_game.h"
#include "networking/logic/network_life_cycle.h"
#include "game/game_engine_team.h"
#include "interface/gui_screens/start_menu/panes/game_multiplayer/start_menu_game_multiplayer.h"
#include "interface/user_interface_networking.h"
#include "main/game_preferences.h"
#include "saved_games/saved_film.h"
#include "saved_games/content_catalogue.h"
#include "interface/game_tag_parser.h"
#include "interface/utf8_types.h"
#include "interface/user_interface_allocation.h"
#include "interface/controller_inline.h"
#include "game/player_configuration.h"
#include "interface/hopper_information_types.h"
#include "interface/controller_types.h"
#include "interface/c_gui_list_item_widget.h"
#include "interface/gui_data.h"
#include "interface/c_gui_screen_widget.h"
#include "interface/gui_screens/start_menu/start_menu_pane_screen_widget.h"
#include "interface/c_gui_list_widget.h"
#include "interface/gui_custom_bitmap_widget.h"
#include "networking/session/network_session_class.h"
#include "saved_games/saved_film_playback.h"
#include "game/game_engine_list.h"
#include "game/game_engine_util.h"
#include "interface/user_interface_controller.h"
#include <new>
#include "main/game_preferences_game_setup.h"
#include "main/game_preferences_types.h"
#include "game/game_state_accessors.h"
#include "interface/gui_screens/start_menu/start_menu_string_ids.h"
#include "tag_files/string_ids.h"

struct utf8;


// @address 0x8c29c0 g_saved_film_playback


static __forceinline short start_menu_game_multiplayer_submenu_user_index(long controller)
{
    c_controller_interface *c = controller_get((e_controller_index)controller);
    return c->get_user_index();
}


static __forceinline int start_menu_game_multiplayer_range_1_10(int x)
{
    if ((x < 1)) return 1;
    if ((x > 10)) return 1;
    return 0;
}

// @address 0x4aa840 ?initialize_datasource@c_start_menu_game_multiplayer@@UAAXXZ
void c_start_menu_game_multiplayer::initialize_datasource()
{
    int item_count;
    c_gui_data *item;
    int controller;
    int local_player_count;
    int game_index;
    int result;
    unsigned char in_game;
    int index;
    c_gui_data **item_slot;
    const c_game_engine *game_engine;
    c_gui_screen_widget *view = this;
    c_gui_screen_widget::initialize_datasource();
    item_count = view->m_datasource_count;
    index = 0;
    if (item_count > 0)
    {
        item_slot = view->m_datasource;
        do
        {
            if ((*item_slot)->get_name() == _start_menu_string_start_menu_game_items)
            {
                item = view->m_datasource[index];
                if (item)
                {
                    item->clear_disabled_elements();
                    controller = (int)this->get_any_responding_controller();
                    if (!is_change_teams_allowed((e_controller_index)controller)) {
                        item->set_disabled_element(_start_menu_string_name, _start_menu_string_change_teams);
                    }
                    if (network_life_cycle_squad_local_peer_is_leader() && !network_life_cycle_in_matchmaking()) {
                        local_player_count = ::user_interface_squad_get_local_player_count();
                        if (user_interface_squad_get_player_count() <= local_player_count) {
                            item->set_disabled_element(_start_menu_string_name, _start_menu_string_leave_game);
                        }
                    } else {
                        item->set_disabled_element(_start_menu_string_name, _start_menu_string_change_game);
                        item->set_disabled_element(_start_menu_string_name, _start_menu_string_end_game);
                    }
                    item->set_disabled_element(_start_menu_string_name, _start_menu_string_change_game);
                    if (!network_life_cycle_squad_local_peer_is_leader()) {
                        item->set_disabled_element(_start_menu_string_name, _start_menu_string_lobby_privacy);
                    }
                    if (!((network_squad_session_get_session_class() == 2 && ((long)::network_life_cycle_squad_session_get_ui_game_mode()) != 1))) {
                        item->set_disabled_element(_start_menu_string_name, _start_menu_string_lobby_privacy);
                    }
                    in_game = 0;
                    game_index = tls_game_engine_globals()->game_engine_index;
                    if (!(start_menu_game_multiplayer_range_1_10(game_index))) {
                        game_engine = game_engines[game_index];
                    } else {
                        game_engine = 0;
                    }
                    result = game_engine->get_type();
                    if (result == _game_engine_sandbox) {
                        in_game = 1;
                    }
                    if (!in_game) {
                        item->set_disabled_element(_start_menu_string_name, _start_menu_string_save_map);
                        item->set_disabled_element(_start_menu_string_name, _start_menu_string_save_map_as);
                    }
                }
                break;
            }
            ++index;
            ++item_slot;
        } while (index < item_count);
    }
    return;
}

static __forceinline bool start_menu_game_multiplayer_update_game_in_progress()
{
    game_globals_storage *globals = tls_game_globals();
    return globals && (globals->initializing || globals->map_active);
}


// @address 0x4aaac0 ?update@c_start_menu_game_multiplayer@@UAAXK@Z
void c_start_menu_game_multiplayer::update(unsigned long current_milliseconds)
{
    // match: 0x4aaac0: pointer __restrict qualifier prevents alias reload
    c_gui_data *__restrict data;
    c_gui_widget *widget;
    long a;
    long b;
    long element;
    unsigned short mask;

    c_start_menu_pane_screen_widget::update(current_milliseconds);
    data = 0;
    for (long i = 0; i < m_datasource_count; i++)
    {
        if (m_datasource[i]->get_name() == 0x10017)
        {
            data = m_datasource[i];
            break;
        }
    }
    widget = this->get_child_widget(_gui_bitmap_widget, _string_id_map_image);
    if (data)
    {
        const c_game_variant *variant = user_interface_game_settings_get_game_variant();
        // a and b are the campaign and map ids out of the session call; b is reused for the team index below
        if (network_life_cycle_session_get_map((e_campaign_id *)&a, (e_map_id *)&b) && variant)
        {
            data->clear_disabled_elements();
            mask = game_engine_get_active_team_mask();
            for (element = data->get_first_element_handle(); element != -1; element = data->get_next_element_handle(element))
            {
                if (data->get_integer_value(element, 266, &b) && ((1 << b) & mask) == 0)
                {
                    a = -1;
                    if (data->get_string_id_value(element, 668, &a))
                        data->set_disabled_element(668, a);
                }
            }
        }
    }
    if (widget && start_menu_game_multiplayer_update_game_in_progress())
        ((c_gui_custom_bitmap_widget *)widget)->set_map_image(_custom_map_image_type_normal, (e_map_id)tls_game_globals()->options.map_id, true);
}


// @address 0x4ab0e0 start_menu_game_multiplayer_set_player_team
extern "C" void start_menu_game_multiplayer_set_player_team(long controller_index, long value)
{
    if ((value >= 0 && value < 8) || value == -1)
    {
        long player_index = ::user_interface_squad_get_player_at_local_controller((e_controller_index)controller_index);
        if (player_index != -1 && ::user_interface_squad_get_player_data(player_index)->host.team_index != value)
        {
            network_session_interface_set_controller_desired_team(controller_index, value);
            user_interface_controller_update_network_properties((e_controller_index)controller_index);
        }
    }
}

// @address 0x4ab160 ?submenu_invoked@c_start_menu_game_multiplayer@@UAAXPAVc_gui_list_widget@@@Z
void c_start_menu_game_multiplayer::submenu_invoked(c_gui_list_widget *list)
{
    long value = -1;
    switch (list->m_name)
    {
    case _start_menu_string_change_teams:
    {
        long user = start_menu_game_multiplayer_submenu_user_index(
            this->get_any_responding_controller());
        if (user != -1)
            value = network_session_interface_get_user_team_index(user);
        break;
    }
    }
    if (value != -1)
        list->set_focused_datasource_element_from_value(266, value, false);
}


// @address 0x4ab1d8 start_menu_game_multiplayer_handle_leave_game
extern "C" void start_menu_game_multiplayer_handle_leave_game(c_gui_screen_widget *screen, long controller_index)
{
    long dialog = -1;
    bool single_player = ::user_interface_squad_get_local_player_count() == 1 || ::user_interface_squad_get_local_player_count() == 0;
    bool unsigned_in_user = !controller_get((e_controller_index)controller_index)->is_signed_in_to_machine();
    long mode = ((long)::network_life_cycle_squad_session_get_ui_game_mode());
#ifdef HALO3_64BIT
    // A directly launched Forge map can be active before a squad mode exists.
    if (screen->get_name() == _start_menu_string_start_menu_game_editor && game_engine_sandbox_is_active())
        mode = _gui_game_mode_mapeditor;
#endif
    switch (mode)
    {
    default:
        if (single_player) dialog = _start_menu_string_leave_game_single_player;
        else if (unsigned_in_user) dialog = _start_menu_string_leave_game_unsigned_user;
        else dialog = _start_menu_string_leave_game_multiplayer;
        break;
    case _gui_game_mode_theater:
        if (g_saved_film_playback && g_saved_film_playback->state) dialog = _start_menu_string_leave_film_upload;
        else if (single_player) dialog = _start_menu_string_leave_theater_single_player;
        else if (unsigned_in_user) dialog = _start_menu_string_leave_theater_unsigned_user;
        else dialog = _start_menu_string_leave_theater_multiplayer;
        break;
    case _gui_game_mode_mapeditor:
        if (single_player) dialog = _start_menu_string_leave_editor_single_player;
        else if (unsigned_in_user) dialog = _start_menu_string_leave_editor_unsigned_user;
        else dialog = _start_menu_string_leave_editor_multiplayer;
        break;
    case _gui_game_mode_matchmaking:
    {
        s_hopper_information hopper_information;
        if (::user_interface_matchmaking_get_hopper_information(network_squad_session_get_hopper(), &hopper_information))
        {
            if (hopper_information.ranked && hopper_information.b3)
                dialog = single_player ? _start_menu_string_leave_ranked_matchmaking_penalty_single_player
                    : _start_menu_string_leave_ranked_matchmaking_penalty_multiplayer;
            else if (hopper_information.ranked)
                dialog = single_player ? _start_menu_string_leave_ranked_matchmaking_single_player : _start_menu_string_leave_ranked_matchmaking_multiplayer;
            else if (hopper_information.b3)
                dialog = single_player ? _start_menu_string_leave_matchmaking_penalty_single_player : _start_menu_string_leave_matchmaking_penalty_multiplayer;
            else
                dialog = single_player ? _start_menu_string_leave_matchmaking_single_player : _start_menu_string_leave_matchmaking_multiplayer;
        }
        break;
    }
    }
    if (dialog != -1)
    {
        void *memory = user_interface_malloc_tracked(sizeof(c_load_dialog_screen_message), 0, 0);
        if (memory)
        {
            c_load_dialog_screen_message *message = new (memory) c_load_dialog_screen_message((e_controller_index)controller_index,
                screen->get_render_window(), screen->get_name(), dialog, screen->get_name());
            if (message) ::user_interface_messaging_post(message);
        }
    }
}

// @address 0x4aae38 ?handle_list_item_chosen@c_start_menu_game_multiplayer@@UAA_NPBVc_controller_input_message@@JPAVc_gui_list_item_widget@@PAVc_gui_data@@@Z
bool c_start_menu_game_multiplayer::handle_list_item_chosen(
    const c_controller_input_message *message, long list_name, c_gui_list_item_widget *item, c_gui_data *data)
{
    bool handled = false;
    long choice;
    if (list_name == _start_menu_string_change_teams || list_name == _start_menu_string_start_menu_items)
    {
        if (list_name != _start_menu_string_change_teams)
        {
            choice = -1;
            if (data->get_string_id_value(item->get_element_handle(), _start_menu_string_name, &choice))
            {
                if (choice <= _start_menu_string_save_map_as)
                {
                    if (choice < _start_menu_string_save_map)
                    {
                        if (choice != _start_menu_string_leave_game)
                        {
                            if (choice == _start_menu_string_end_game)
                            {
                                void *memory = user_interface_malloc_tracked(sizeof(c_load_dialog_screen_message), 0, 0);
                                c_load_dialog_screen_message *dialog = memory
                                    ? new (memory) c_load_dialog_screen_message(
                                          message->get_controller(), this->get_render_window(), this->get_name(),
                                          _start_menu_string_end_game_confirmation, this->get_name())
                                    : 0;
                                if (dialog) ::user_interface_messaging_post(dialog);
                                handled = true;
                            }
                        }
                        else
                        {
                            start_menu_game_multiplayer_handle_leave_game(this, message->get_controller());
                            handled = true;
                        }
                    }
                    else
                    {
                        void *memory = user_interface_malloc_tracked(sizeof(c_load_dialog_screen_message), 0, 0);
                        c_load_dialog_screen_message *dialog = memory
                            ? new (memory) c_load_dialog_screen_message(
                                  message->get_controller(), this->get_render_window(), this->get_name(),
                                  _start_menu_string_save_map_confirmation, this->get_name())
                            : 0;
                        if (dialog) ::user_interface_messaging_post(dialog);
                        handled = true;
                    }
                }
                else if (choice == _start_menu_string_lobby_privacy)
                {
                    void *memory = user_interface_malloc_tracked(sizeof(c_load_dialog_screen_message), 0, 0);
                    c_load_dialog_screen_message *dialog = memory ? new (memory) c_load_dialog_screen_message(message->get_controller(),
                                                                        this->get_render_window(), this->get_name(),
                                                                        _start_menu_string_lobby_privacy, this->get_name())
                                                                  : 0;
                    if (dialog)
                    {
                        dialog->set_parent_screen_index(this->get_screen_index());
                        ::user_interface_messaging_post(dialog);
                    }
                    handled = true;
                }
            }
        }
        choice = -1;
        if (data->get_integer_value(item->get_element_handle(), _start_menu_string_team, &choice))
        {
            if ((choice >= 0 && choice < 16) || choice == -1)
            {
                long controller = this->get_any_responding_controller();
                start_menu_game_multiplayer_set_player_team(controller, choice);
                this->close_start_menu();
            }
        }
    }
    if (!handled) handled = this->c_start_menu_pane_screen_widget::handle_list_item_chosen(message, list_name, item, data);
    return handled;
}

// @address 0x4ab428 ?handle_leave_game_response@c_start_menu_game_multiplayer@@SA_NPAVc_start_menu_pane_screen_widget@@PBVc_dialog_result_message@@@Z
bool c_start_menu_game_multiplayer::handle_leave_game_response(c_start_menu_pane_screen_widget *screen, const c_dialog_result_message *message)
{
    bool handled = false;
    bool leave_squad = false;
    switch (message->get_dialog_name())
    {
    case _start_menu_string_leave_game_single_player:
    case _start_menu_string_leave_matchmaking_single_player:
    case _start_menu_string_leave_ranked_matchmaking_single_player:
    case _start_menu_string_leave_ranked_matchmaking_penalty_single_player:
    case _start_menu_string_leave_matchmaking_penalty_single_player:
    case _start_menu_string_leave_editor_single_player:
    case _start_menu_string_leave_theater_single_player:
        if (message->get_dialog_result() == 0)
        {
            if (::user_interface_squad_get_local_player_count() == 0)
                main_menu_launch();
            else
                leave_squad = true;
        }
        handled = true;
        break;
    case _start_menu_string_leave_game_multiplayer:
    case _start_menu_string_leave_matchmaking_multiplayer:
    case _start_menu_string_leave_ranked_matchmaking_multiplayer:
    case _start_menu_string_leave_ranked_matchmaking_penalty_multiplayer:
    case _start_menu_string_leave_matchmaking_penalty_multiplayer:
    case _start_menu_string_leave_editor_multiplayer:
    case _start_menu_string_leave_theater_multiplayer:
        if (message->get_dialog_result() == 0)
            leave_squad = true;
        handled = true;
        break;
    case _start_menu_string_leave_game_unsigned_user:
    case _start_menu_string_leave_editor_unsigned_user:
    case _start_menu_string_leave_theater_unsigned_user:
        if (message->get_dialog_result() == 0)
            leave_squad = true;
        else if (message->get_dialog_result() == 1)
            controller_get(message->get_controller())->set_as_unsigned_in_user(false);
        handled = true;
        break;
    case _start_menu_string_leave_film_upload:
        if (message->get_dialog_result() == 0)
        {
            saved_film_reset_playback_upload(g_saved_film_playback);
            start_menu_game_multiplayer_handle_leave_game(screen, message->get_controller());
        }
        handled = true;
        break;
    }
    if (leave_squad)
    {
        user_interface_leave_squad(0);
        screen->close_start_menu();
    }
    return handled;
}

// @address 0x4ab5f8 ?is_change_teams_allowed@@YA_NW4e_controller_index@@@Z
bool is_change_teams_allowed(e_controller_index controller_index)
{
    bool allowed = false;
    if (game_in_progress() &&
        (tls_game_globals()->options.game_mode == 2) &&
        !network_life_cycle_in_matchmaking() &&
        (unsigned long)controller_index <= 3 &&
        controller_get((e_controller_index)controller_index)->in_use() &&
        controller_get((e_controller_index)controller_index)->get_user_index() >= 0 &&
        controller_get((e_controller_index)controller_index)->get_user_index() < 4)
    {
        long absolute_player_index = ::user_interface_squad_get_player_at_local_controller(controller_index);
        long player_index = ((unsigned long)absolute_player_index <= 15 && absolute_player_index != -1)
            ? player_try_and_get_player_index_from_absolute_player_index(absolute_player_index)
            : -1;
        allowed = (player_index != -1) ? game_engine_teams_player_can_change_teams(player_index) : false;
    }
    return allowed;
}

// @address 0x4ab708 start_menu_game_multiplayer_parse_hopper_name
extern "C" bool start_menu_game_multiplayer_parse_hopper_name(void *context, wchar_t *output, long capacity)
{
    const wchar_t *source = ((c_start_menu_game_multiplayer *)context)->m_hopper_name;
    if (source != 0)
    {
        ustrnzcpy(output, source, capacity);
    }
    return true;
}

// @address 0x4ab740 start_menu_game_multiplayer_parse_hopper_description
extern "C" bool start_menu_game_multiplayer_parse_hopper_description(void *context, wchar_t *output, long capacity)
{
    const wchar_t *source = ((c_start_menu_game_multiplayer *)context)->m_hopper_description;
    if (source != 0)
    {
        ustrnzcpy(output, source, capacity);
    }
    return true;
}

// @address 0x1d4038 ?save_to@c_map_variant@@QBAXPAV1@@Z

// @address 0x282ce8 ?content_catalogue_display_device_selection_guide_interface@@YAXW4e_controller_index@@@Z
// @address 0x4795f8 ?user_interface_networking_get_session_advertisement@@YA?AW4e_gui_network_session_advertisement_mode@@XZ

// 0x4aacd0: the dialog-result handler the start menu calls for its multiplayer dialogs
// @address 0x4aacd0 ?handle_dialog_result@c_start_menu_game_multiplayer@@UAA_NPBVc_dialog_result_message@@@Z
bool c_start_menu_game_multiplayer::handle_dialog_result(const c_dialog_result_message *message)
{
    bool handled = c_start_menu_game_multiplayer::handle_leave_game_response(this, message);
    if (!handled)
    {
        long dialog_index = message->get_dialog_name() - _start_menu_string_end_game_confirmation;
        if (message->get_dialog_name() - _start_menu_string_end_game_confirmation != 0)
        {
            if ((unsigned long)dialog_index != 2)
            {
                if ((unsigned long)dialog_index == 63)
                {
                    e_gui_network_session_advertisement_mode advertisement = user_interface_networking_get_session_advertisement();
                    switch (message->get_dialog_result())
                    {
                    case 1:
                        user_interface_networking_set_session_advertisement(1);
                        break;
                    case 2:
                        user_interface_networking_set_session_advertisement(2);
                        break;
                    case 0:
                        advertisement = _network_advertise_xbox_live_open;
                        // fall through
                    default:
                        user_interface_networking_set_session_advertisement(advertisement);
                        break;
                    }
                    handled = true;
                }
            }
            else
            {
                if (message->get_dialog_result() == 0)
                {
                    struct s_map_save
                    {
                        int a;
                        int b[16017];
                    } obj;
                    const s_game_preferences_mode_settings *settings = game_preferences_get_mode_settings();
                    memcpy(obj.b, settings->map_editor_settings, sizeof(settings->map_editor_settings));
                    tls_game_engine_globals()->map_info.save_to((c_map_variant *)&obj.b[2]);
                    global_preferences_set_mode_setup(&obj.a);
                    close_start_menu();
                }
                handled = true;
            }
        }
        else
        {
            if (message->get_dialog_result() == 0)
            {
                if (network_life_cycle_squad_local_peer_is_leader())
                    network_squad_session_end_game();
                close_start_menu();
            }
            handled = true;
        }
    }
    if (!handled)
    {
        handled = false;
        if (message->get_dialog_name() == _start_menu_string_device_selection)
        {
            if (message->get_dialog_result() == 0)
                content_catalogue_display_device_selection_guide_interface(message->get_controller());
            handled = true;
        }
    }
    return handled;
}


// @address 0x50e04 k_magic_string_hopper_name
// @address 0x50e18 k_magic_string_hopper_description
// @rdata 0x82050e04..0x82050e44
extern "C" {
const wchar_t k_magic_string_hopper_name[] = {
    '<', 'g', 'a', 'm', 'e', 't', 'y', 'p', 'e', 0
};
const wchar_t k_magic_string_hopper_description[] = {
    '<', 'g', 'a', 'm', 'e', 't', 'y', 'p', 'e', '-', 'd', 'e', 's', 'c', 'r', 'i', 'p', 't', 'i', 'o', 'n', 0
};
}
// @rdata-end


static __forceinline void start_menu_game_multiplayer_ustrncpy(wchar_t *dest, const wchar_t *source, long count)
{
    for (long index = 0; index <= count; index++) {
        if (!(dest[index] = *source))
            break;
        ++source;
    }
}

// @address 0x4aa568 ?initialize@c_start_menu_game_multiplayer@@UAAXXZ
void c_start_menu_game_multiplayer::initialize()
{
    unsigned short hopper = network_squad_session_get_hopper();
    c_gui_screen_widget::initialize();
    add_game_tag_parser(new (0, 0, _ui_allocation_marker_dummy)
            c_magic_string_game_tag_parser(k_magic_string_hopper_name, this, start_menu_game_multiplayer_parse_hopper_name));
    add_game_tag_parser(new (0, 0, _ui_allocation_marker_dummy)
            c_magic_string_game_tag_parser(k_magic_string_hopper_description, this, start_menu_game_multiplayer_parse_hopper_description));
    add_game_tag_parser(new(0, 0, _ui_allocation_marker_dummy) c_magic_string_game_tag_parser(L"<lobby-privacy", this, parse_lobby_privacy));
    ustrnzcpy_inline(m_hopper_name, L"", 32);
    ustrnzcpy_inline(m_hopper_description, L"", 256);
    if (((long)::network_life_cycle_squad_session_get_ui_game_mode()) == 1 && hopper != 0xffff)
    {
        s_hopper_information information;
        if (::user_interface_matchmaking_get_hopper_information(hopper, &information))
        {
            start_menu_game_multiplayer_ustrncpy(m_hopper_name, (const wchar_t *)information.name, 32);
            bool converted = false;
            const utf8 *description = multiplayer_game_hopper_get_description(information.id);
            wchar_t buffer[256];
            if (description != 0)
            {
                ::utf8_string_to_wchar_string((utf8 const *)(const unsigned char *)description, (wchar_t *)((unsigned short *)buffer), 0x100, (long *)(0));
                converted = true;
            }
            if (converted)
                start_menu_game_multiplayer_ustrncpy(m_hopper_description, buffer, 0x100);
        }
    }
}

// @address 0x4d7500 ?create_bitmap_widget@c_start_menu_game_multiplayer@@UAAPAVc_gui_bitmap_widget@@PBUs_runtime_bitmap_widget_definition@@@Z
c_gui_bitmap_widget *c_start_menu_game_multiplayer::create_bitmap_widget(const s_runtime_bitmap_widget_definition *definition)
{
    if (definition->name == _string_id_map_image)
        return new (0, 0, _ui_allocation_marker_dummy) c_gui_custom_bitmap_widget();
    return new (0, 0, _ui_allocation_marker_dummy) c_gui_bitmap_widget();
}
