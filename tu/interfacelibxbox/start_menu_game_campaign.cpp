// interfacelibxbox/start_menu_game_campaign.obj: from the records 0x484a18 0x4a9b18 0x4a9c40 0x4a9d08 0x4a9ee8 0x4a9fc8 0x4aa1a8 0x4aa498 0x4aa500 0x4ae0f8 0x4ae760
// @flags /TP /O2 /Oc
#include "interface/gui_screens/start_menu/start_menu_string_ids.h"
#include "tag_files/string_ids.h"
#include "interface/gui_screen_widget_legacy.h"
#include "interface/marketplace_ui.h"
#include "interface/gui_screens/boot_betrayer/gui_screen_boot_betrayer.h"
#include "interface/gui_network_session_advertisement_types.h"
#include "memory/static_arrays.h"
#include "shell/shell.h"
#include "interface/gui_data.h"
#include <ppcintrinsics.h>
#include <math.h>
#include <stdlib.h>
#include "interface/user_interface_memory.h"
#include "interface/user_interface_session.h"
#include "networking/logic/network_session_interface.h"
#include "text/unicode.h"
#include "interface/user_interface_networking.h"
#include "interface/user_interface.h"
#include "saved_games/saved_game_files.h"
#include "interface/gui_screens/start_menu/panes/settings/start_menu_settings.h"
#include "interface/user_interface_messages.h"
#include "interface/gui_screens/scoreboard/gui_screen_scoreboard.h"
#include "interface/gui_screens/start_menu/start_menu_pane_screen_widget.h"
#include "interface/c_gui_bitmap_widget.h"
#include "interface/gui_custom_bitmap_widget.h"
#include <new>
#include "saved_games/content_catalogue.h"
#include "saved_games/marketplace_asset_types.h"
#include "interface/ui_allocation_marker_types.h"
#include "interface/user_interface_allocation.h"
#include "simulation/simulation_queue_global_events.h"
enum e_network_session_class;
class c_game_tag_parser;
class c_gui_screen_widget;
#include "interface/game_tag_parser.h"
#include "interface/gui_screens/start_menu/panes/game_multiplayer/start_menu_game_multiplayer.h"
#include "interface/gui_screens/start_menu/panes/game_campaign/start_menu_game_campaign.h"
#include "interface/user_interface_window_manager.h"
#include "interface/gui_screens/start_menu/panes/hq_service_record/gui_start_menu_hq_service_record.h"
#include "interface/c_player_marketplace.h"
#include "interface/user_interface_objectives.h"
class c_dialog_result_message;
class c_gui_list_item_widget;
#include "interface/gui_widget.h"
#include "interface/c_gui_list_item_widget.h"
#include "interface/c_start_menu_item_gui_data.h"
#include "interface/controller_inline.h"
#include "interface/gui_custom_bitmap_widget.h"
#include "interface/user_interface_device_selection.h"
#include "networking/logic/network_life_cycle.h"
#include "game/game.h"
#include "game/game_globals_storage.h"
#ifdef HALO3_64BIT
#include "game/game_simulation_types.h"
#endif
#include "game/game_time_types.h"
#include "memory/tls_globals.h"
#include "saved_games/content_catalogue_types.h"
#include "saved_games/content_catalogue_externs.h"

// @address 0x4ab428 ?handle_leave_game_response@c_start_menu_game_multiplayer@@SA_NPAVc_start_menu_pane_screen_widget@@PBVc_dialog_result_message@@@Z
// @address 0x59ffa8 XShowMarketplaceUI_thunk


struct s_start_menu_game_campaign_marketplace_binding
{
    e_controller_index controller_index;
    s_player_marketplace_user *check;
};

// @address 0x8c7e8 ?user_interface_squad_exists@@YA_NXZ
// @address 0x3b0b28 ?network_squad_session_get_maximum_player_count@@YAJXZ
// @address 0x3b2110 ?user_interface_networking_get_closed@@YA_NXZ

// @address 0x484a18 parse_lobby_privacy
bool parse_lobby_privacy(void *context, wchar_t *output, long capacity)
{
    bool copied;
    int max_players;
    int campaign_mode;
    int string_id;
    unsigned int copy_length;
    copied = false;
    if (user_interface_squad_exists()) {
        c_static_wchar_string<1024> buffer;
        max_players = network_squad_session_get_maximum_player_count();
        if (::user_interface_squad_get_player_count() >= max_players) {
            string_id = _string_id_privacy_full;
        } else if (::network_squad_session_get_session_class() == 1) {
            string_id = _string_id_privacy_system_link;
        } else if (::network_squad_session_get_session_class() == 0) {
            string_id = _string_id_privacy_offline;
        } else if (user_interface_networking_get_closed()) {
            string_id = _string_id_privacy_closed;
        } else {
            campaign_mode = user_interface_squad_get_join_mode();
            switch (campaign_mode) {
            case 0: string_id = _string_id_privacy_open_to_public; break;
            case 1: string_id = _string_id_privacy_open_to_friends; break;
            case 2: string_id = _string_id_privacy_invite_only; break;
            case 3: string_id = _string_id_privacy_closed; break;
            // match: 0x484a18: assume keeps string_id in a register; without it string_id spills to stack and the frame grows
            default: __assume(0);
            }
        }
        if (static_cast<c_gui_screen_widget *>(context)->get_string_by_string_id(string_id, &buffer)) {
            copy_length = (unsigned int)capacity;
            if (copy_length > 0x400) copy_length = 0x400;
            ustrnzcpy(output, buffer.get_string(), copy_length);
            copied = true;
        }
    }
    return copied;
}

// @address 0x4aa498 ?parse_xml_primary_mission_objectives@@YA_NPAXPA_WJ@Z
bool parse_xml_primary_mission_objectives(void *context, wchar_t *output, long capacity)
{
    c_static_wchar_string<256> text;
    user_interface_build_primary_objectives_text(&text);
    ustrnzcpy(output, text.get_string(), capacity);
    return *output != 0;
}

// @address 0x4aa500 ?parse_xml_secondary_mission_objectives@@YA_NPAXPA_WJ@Z
bool parse_xml_secondary_mission_objectives(void *context, wchar_t *output, long capacity)
{
    c_static_wchar_string<256> text;
    user_interface_build_secondary_objectives_text(&text);
    ustrnzcpy(output, text.get_string(), capacity);
    return *output != 0;
}

// @address 0x4e8a28 ?close_start_menu@c_start_menu_pane_screen_widget@@QAAXXZ

// @address 0x4a9b18 ?initialize@c_start_menu_game_campaign@@UAAXXZ
void c_start_menu_game_campaign::initialize()
{
    gui_screen_widget_initialize(this);
    add_game_tag_parser(new(0, 0, _ui_allocation_marker_dummy)
        c_magic_string_game_tag_parser(L"<primary-objectives", this, parse_xml_primary_mission_objectives));
    add_game_tag_parser(new(0, 0, _ui_allocation_marker_dummy)
        c_magic_string_game_tag_parser(L"<secondary-objectives", this, parse_xml_secondary_mission_objectives));
    add_game_tag_parser(new(0, 0, _ui_allocation_marker_dummy) c_magic_string_game_tag_parser(L"<lobby-privacy", this, parse_lobby_privacy));
}

// @address 0x4a9c40 ?post_initialize@c_start_menu_game_campaign@@UAAXXZ
void c_start_menu_game_campaign::post_initialize()
{
    int driving_controller;
    c_controller_interface *controller_interface;
    driving_controller = (int)get_driving_controller();
    if ((unsigned int)driving_controller <= 3)
    {
        controller_interface = controller_get((e_controller_index)driving_controller);
        if (controller_interface->is_signed_in_to_machine() &&
            !content_catalogue_has_storage_device(&g_content_catalogue_globals.catalogues[controller_interface->get_controller_index()]) &&
            !controller_interface->is_guest() && !controller_interface->is_unsigned_in_user())
        {
            user_interface_show_device_selection_dialog((e_controller_index)driving_controller, m_name, m_screen_index);
        }
    }
    c_start_menu_pane_screen_widget::post_initialize();
    return;
}

// @address 0x4a9d08 ?initialize_datasource@c_start_menu_game_campaign@@UAAXXZ
void c_start_menu_game_campaign::initialize_datasource()
{
    c_start_menu_pane_screen_widget::initialize_datasource();
    long datasource_count = m_datasource_count;
    long datasource_index = 0;
    if (datasource_count > 0)
    {
        c_gui_data **datasource = m_datasource;
        do
        {
            if ((*datasource)->get_name() == 0x1010f)
            {
                c_start_menu_item_gui_data *item = (c_start_menu_item_gui_data *)m_datasource[datasource_index];
                if (item != 0)
                {
                    item->clear_disabled_elements();
#ifdef HALO3_64BIT
                    // Directly launched offline Campaign has no hosted squad.
                    const bool local_campaign = tls_game_globals()->options.game_simulation == _game_simulation_local;
                    if (!local_campaign && !network_life_cycle_is_squad_host())
#else
                    if (!network_life_cycle_is_squad_host())
#endif
                    {
                        item->set_disabled_element(0x29c, 0x11f);
                        item->set_disabled_element(0x29c, 0x120);
                    }
                    else
                    {
                        game_globals_storage *globals = tls_game_globals();
                        if (globals->options.game_mode == 1 && !game_campaign_is_multiplayer() &&
                            (((unsigned int)(globals->options.game_mode == 1 ? globals->options.primary_skulls : 0) & 1u) != 0))
                            item->set_disabled_element(0x29c, 0x11f);
                    }
                    if (user_interface_squad_exists() && user_interface_squad_get_player_count() != 1)
                        item->set_disabled_element(0x29c, 0x121);
                    else
                        item->set_disabled_element(0x29c, 0x106);
                    if (!network_life_cycle_squad_local_peer_is_leader())
                        item->set_disabled_element(0x29c, 0x30053);
                    if (network_squad_session_get_session_class() != 2 || game_campaign_is_multiplayer())
                        item->set_disabled_element(0x29c, 0x30053);
                }
                break;
            }
            datasource_index++;
            datasource++;
        } while (datasource_index < datasource_count);
    }
}

// @address 0x4a9ee8 ?update@c_start_menu_game_campaign@@UAAXK@Z
void c_start_menu_game_campaign::update(unsigned long current_milliseconds)
{
    c_start_menu_pane_screen_widget::update(current_milliseconds);
    c_gui_custom_bitmap_widget *map_bitmap = (c_gui_custom_bitmap_widget *)get_child_widget(_gui_bitmap_widget, _string_id_map_image);
    bool show_widget;
    if (game_time_globals->initialized) {
        show_widget = (bool)((game_time_globals->flags >> 2) & 1);
    } else {
        show_widget = false;
    }
    c_gui_widget *widget = get_child_widget(_gui_group_widget, 0x341);
    if (widget) {
        widget->set_visible(show_widget);
    }
    if (map_bitmap) {
        game_globals_storage *globals = tls_game_globals();
        bool in_game = globals && (globals->initializing || globals->map_active);
        if (in_game) {
            map_bitmap->set_map_image(_custom_map_image_type_normal, globals->options.map_id, true);
        }
    }
}

// @address 0x4795f8 ?user_interface_networking_get_session_advertisement@@YA?AW4e_gui_network_session_advertisement_mode@@XZ
// @address 0x282ce8 ?content_catalogue_display_device_selection_guide_interface@@YAXW4e_controller_index@@@Z

// @address 0x4a9fc8 ?handle_dialog_result@c_start_menu_game_campaign@@UAA_NPBVc_dialog_result_message@@@Z
bool c_start_menu_game_campaign::handle_dialog_result(const c_dialog_result_message *message)
{
    bool handled = c_start_menu_game_multiplayer::handle_leave_game_response(this, message);
    if (!handled)
    {
        switch (message->get_dialog_name())
        {
        case _start_menu_string_in_campaign_save_and_quit:
        case _start_menu_string_in_coop_end_game:
            if (message->get_dialog_result() == 0)
            {
                c_window_manager *manager = &g_window_manager;
                manager->m_fade_in = 1;
                manager->m_fade_held = 1;
                close_start_menu();
            }
            handled = true;
            break;
        case _start_menu_string_in_campaign_revert_to_last_save:
            if (message->get_dialog_result() == 0)
            {
                ::simulation_queue_game_global_event_insert(_simulation_queue_game_global_event_main_revert_map);
                close_start_menu();
            }
            handled = true;
            break;
        case _start_menu_string_in_campaign_restart_level:
            if (message->get_dialog_result() == 0)
            {
                ::simulation_queue_game_global_event_insert(_simulation_queue_game_global_event_main_save_map);
                close_start_menu();
            }
            handled = true;
            break;
        case _start_menu_string_lobby_privacy:
        {
            long advertisement = user_interface_networking_get_session_advertisement();
            switch ((unsigned long)message->get_dialog_result())
            {
            case 0: advertisement = 0; break;
            case 1: advertisement = 1; break;
            case 2: advertisement = 2; break;
            }
            user_interface_networking_set_session_advertisement(advertisement);
            handled = true;
            break;
        }
        default:
            break;
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

// @address 0x4ae0f8 ?initialize@c_gui_start_menu_hq_service_record@@UAAXXZ
void c_gui_start_menu_hq_service_record::initialize()
{
    gui_screen_widget_initialize(this);
    this->add_game_tag_parser(new(0, 0, _ui_allocation_marker_dummy)
        c_magic_string_game_tag_parser(L"<bnet-pro-button", this, parse_xml_look_sensitivity));
    this->add_game_tag_parser(new(0, 0, _ui_allocation_marker_dummy)
        c_magic_string_game_tag_parser(L"<bnet-pro-message", this, parse_xml_look_centering));
}

static __forceinline s_start_menu_game_campaign_marketplace_binding start_menu_game_campaign_bind_marketplace(e_controller_index controller_index)
{
    s_start_menu_game_campaign_marketplace_binding binding;
    binding.controller_index = controller_index;
    binding.check = &g_player_marketplace_users[controller_index];
    return binding;
}

// @address 0x4ae760 ?handle_list_item_chosen@c_gui_start_menu_hq_service_record@@UAA_NPBVc_controller_input_message@@JPAVc_gui_list_item_widget@@PAVc_gui_data@@@Z
bool c_gui_start_menu_hq_service_record::handle_list_item_chosen(const c_controller_input_message *message,
    long list, c_gui_list_item_widget *item, c_gui_data *data)
{
    bool handled = false;
    if (list == _start_menu_string_start_menu_items)
    {
        long value = -1;
        const bool &has_value = data->get_string_id_value(item->get_element_handle(), _start_menu_string_name, &value);
        if (has_value)
        {
            switch (value)
            {
            case _start_menu_string_marketplace:
            {
                e_controller_index controller = this->get_any_responding_controller();
                marketplace_show_ui(controller, 0, 0, -1);
                break;
            }
            case _start_menu_string_file_share_bungie:
                if (get_alpha_is_external_beta())
                    handled = true;
                break;
            case _start_menu_string_bnet_pro:
            {
                const s_start_menu_game_campaign_marketplace_binding &binding = start_menu_game_campaign_bind_marketplace(
                    this->get_any_responding_controller());
                binding.check->controller_index = binding.controller_index;
                s_player_marketplace_user *user = binding.check;
                if (user->pending)
                {
                    if (!user->consume_pending)
                        marketplace_consume_assets(user, user->offer_id);
                    else
#if defined(__clang__)
                        marketplace_consume_assets(user, 0);
#else
                        // match: 0x4ae760: identity rotate emits the else-path mr r3,r11 copy; without it the copy is missing
                        marketplace_consume_assets((s_player_marketplace_user *)_rotl((unsigned long)user, 32), 0);
#endif
                }
                break;
            }
            }
        }
    }
    if (!handled)
        handled = c_start_menu_pane_screen_widget::handle_list_item_chosen(message, list, item, data);
    return handled;
}

// @address 0x4aa1a8 ?handle_list_item_chosen@c_start_menu_game_campaign@@UAA_NPBVc_controller_input_message@@JPAVc_gui_list_item_widget@@PAVc_gui_data@@@Z
bool c_start_menu_game_campaign::handle_list_item_chosen(const c_controller_input_message *message,
    long list, c_gui_list_item_widget *item, c_gui_data *data)
{
    bool handled = false;
    if (list == _start_menu_string_start_menu_items)
    {
        long choice = -1;
        if (data->get_string_id_value(item->get_element_handle(), _start_menu_string_name, &choice))
        {
            switch (choice)
            {
            case _start_menu_string_restart_level:
            {
                void *memory = user_interface_malloc_tracked(sizeof(c_load_dialog_screen_message), 0, 0);
                c_load_dialog_screen_message *dialog = memory
                    ? new (memory)
                          c_load_dialog_screen_message(message->get_controller(), get_render_window(), get_name(),
                              _start_menu_string_in_campaign_restart_level, get_name())
                    : 0;
                if (dialog) ::user_interface_messaging_post(dialog);
                handled = true;
                break;
            }
            case _start_menu_string_revert_to_last_save:
            {
                void *memory = user_interface_malloc_tracked(sizeof(c_load_dialog_screen_message), 0, 0);
                c_load_dialog_screen_message *dialog = memory
                    ? new (memory)
                          c_load_dialog_screen_message(message->get_controller(), get_render_window(), get_name(),
                              _start_menu_string_in_campaign_revert_to_last_save, get_name())
                    : 0;
                if (dialog) ::user_interface_messaging_post(dialog);
                handled = true;
                break;
            }
            case _start_menu_string_leave_game:
            {
                void *memory = user_interface_malloc_tracked(sizeof(c_load_dialog_screen_message), 0, 0);
                c_load_dialog_screen_message *dialog = memory
                    ? new (memory)
                          c_load_dialog_screen_message(message->get_controller(), get_render_window(), get_name(),
                              _start_menu_string_in_coop_end_game, get_name())
                    : 0;
                if (dialog) ::user_interface_messaging_post(dialog);
                handled = true;
                break;
            }
            case _start_menu_string_save_and_quit:
            {
                void *memory = user_interface_malloc_tracked(sizeof(c_load_dialog_screen_message), 0, 0);
                c_load_dialog_screen_message *dialog = memory
                    ? new (memory)
                          c_load_dialog_screen_message(message->get_controller(), get_render_window(), get_name(),
                              _start_menu_string_in_campaign_save_and_quit, get_name())
                    : 0;
                if (dialog) ::user_interface_messaging_post(dialog);
                handled = true;
                break;
            }
            case _start_menu_string_resume:
                close_start_menu();
                handled = true;
                break;
            case _start_menu_string_lobby_privacy:
            {
                void *memory = user_interface_malloc_tracked(sizeof(c_load_dialog_screen_message), 0, 0);
                c_load_dialog_screen_message *dialog = memory
                    ? new (memory)
                          c_load_dialog_screen_message(message->get_controller(), get_render_window(), get_name(), _start_menu_string_lobby_privacy, get_name())
                    : 0;
                if (dialog)
                {
                    dialog->set_parent_screen_index(get_screen_index());
                    ::user_interface_messaging_post(dialog);
                }
                handled = true;
                break;
            }
            }
        }
    }
    if (!handled)
        handled = c_start_menu_pane_screen_widget::handle_list_item_chosen(message, list, item, data);
    return handled;
}

// @address 0x4d7500 ?create_bitmap_widget@c_start_menu_game_campaign@@UAAPAVc_gui_bitmap_widget@@PBUs_runtime_bitmap_widget_definition@@@Z
c_gui_bitmap_widget *c_start_menu_game_campaign::create_bitmap_widget(const s_runtime_bitmap_widget_definition *definition)
{
    if (definition->name == _string_id_map_image)
        return new (0, 0, _ui_allocation_marker_dummy) c_gui_custom_bitmap_widget();
    return new (0, 0, _ui_allocation_marker_dummy) c_gui_bitmap_widget();
}
