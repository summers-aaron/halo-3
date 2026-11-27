// interfacelibxbox/gui_screen_boot_betrayer.obj: from the records 0x491250 0x4912a0 0x491318 0x4913d8 0x491460 0x491520 0x4915e0 0x491818
// @compiler xdk6130
// @flags /TP /O2 /Oc
#include "interface/gui_screen_widget_legacy.h"
#include "simulation/simulation_queue_event_types.h"
#include "cseries/crt_xdk.h"
#include "game/game_grief.h"
#include "memory/tls_data_arrays.h"
#include "memory/tls_globals.h"
#include "memory/data_types.h"
#include <stddef.h>
#include "game/player_types.h"
#include "shell/shell.h"
#include "game/game_globals_storage.h"
#include "interface/user_interface_messages.h"
#include <string.h>
#include "game/game.h"
#include "interface/gui_screens/scoreboard/gui_screen_scoreboard.h"
#include "interface/user_interface_memory.h"
#include "interface/user_interface_session.h"
#include "game/game_engine.h"
#include "main/game_preferences.h"
#include "interface/gui_screens/boot_betrayer/gui_screen_boot_betrayer.h"
#include "saved_games/content_catalogue.h"
#include "interface/game_tag_parser.h"
#include "simulation/simulation_globals.h"
#include "interface/ui_allocation_marker_types.h"
#include "networking/network_configuration.h"
#include "interface/user_interface_allocation.h"
#include "simulation/simulation_event_handler_types.h"
#include "interface/gui_widget.h"
#include "interface/c_gui_screen_widget.h"
#include "networking/network_configuration_types.h"
#include "game/player_in_game_iterator.h"
#include "memory/data_typed_access.h"
#include <new>
#include "interface/gui_screen_construction.h"
#include "text/unicode.h"


#include "game/game_grief_types.h"


#include "simulation/simulation_world_types.h"
#include "simulation/simulation_distributed_world_types.h"
#include "networking/simulation/simulation_accessors.h"

// @address 0x7e86cc g_game_grief_globals
// @address 0x491250 ?apply_initial_state@c_load_boot_betrayer_screen_message@@UBAXPAVc_gui_screen_widget@@@Z
void c_load_boot_betrayer_screen_message::apply_initial_state(c_gui_screen_widget *screen) const {
    c_gui_screen_boot_betrayer *boot_screen = (c_gui_screen_boot_betrayer *)screen;
    boot_screen->set_current_player_id(&m_current_player);
    boot_screen->set_target_player_id(&m_target_player);
}

// @address 0x4ecc0 ??_7c_gui_screen_boot_betrayer@@6B@
// @address 0x4912a0 gui_screen_boot_betrayer_construct
c_gui_screen_widget *gui_screen_boot_betrayer_construct(c_gui_screen_widget *screen, long name)
{
    c_gui_screen_boot_betrayer *boot_screen = static_cast<c_gui_screen_boot_betrayer *>(screen);
    boot_screen->c_gui_screen_boot_betrayer::c_gui_screen_boot_betrayer(name);
    return boot_screen;
}




// Short tag strings are zero-padded to 4 bytes.
// @address 0x4ec90 k_boot_betrayer_griefer_tag
// @rdata 0x8204ec90..0x8204ecb0
extern "C" {
const wchar_t k_boot_betrayer_griefer_tag[] = {
    '<', 'b', 'e', 't', 'r', 'a', 'y', 'e', 'r', '-', 'n', 'a', 'm', 'e', 0, 0
};
}
// @rdata-end

// @address 0x484e90 ?user_interface_malloc_tracked@@YAPAXIPBDJ@Z

// @address 0x491318 ?initialize@c_gui_screen_boot_betrayer@@UAAXXZ
void c_gui_screen_boot_betrayer::initialize()
{
    gui_screen_widget_initialize(this);
    add_game_tag_parser(new(0, 0, _ui_allocation_marker_dummy) c_magic_string_game_tag_parser(k_boot_betrayer_griefer_tag, this, parse_griefer_name));
    e_controller_index controller = get_any_responding_controller();
    if (!game_is_cooperative()) {
        g_game_grief_globals.player_betrayed[controller] = true;
        g_game_grief_globals.reprieve_time = 0;
    }
}
              // player_data

// @address 0x4913d8 ?dispose@c_gui_screen_boot_betrayer@@UAAXXZ
void c_gui_screen_boot_betrayer::dispose()
{
    if (!m_player_booted && !game_is_cooperative())
        g_game_grief_globals.reprieve_time = GetTickCount();
    e_controller_index controller = get_any_responding_controller();
    if (!game_is_cooperative())
        g_game_grief_globals.player_betrayed[controller] = false;
    c_gui_screen_widget::dispose();
}
// @address 0x491520 ?handle_dialog_result@c_gui_screen_boot_betrayer@@UAA_NPBVc_dialog_result_message@@@Z
bool c_gui_screen_boot_betrayer::handle_dialog_result(const c_dialog_result_message *dialog_result_message)
{
    bool handled = false;
    if (dialog_result_message->get_dialog_name() == 0x30004) {
        if (dialog_result_message->get_dialog_result() == 0) {
            boot_player();
            transition_out_with_transition_type(0, _screen_transition_normal);
        }
        handled = true;
    }
    if ((unsigned char)handled == 0) {
        handled = false;
        if (dialog_result_message->get_dialog_name() == 0x3001e) {
            if (dialog_result_message->get_dialog_result() == 0)
                content_catalogue_display_device_selection_guide_interface(dialog_result_message->get_controller());
            handled = true;
        }
    }
    return handled;
}
// @address 0x491818 ?parse_griefer_name@@YA_NPAXPA_WJ@Z
bool parse_griefer_name(void *context, wchar_t *buffer, long buffer_length)
{
    c_gui_screen_boot_betrayer *screen = (c_gui_screen_boot_betrayer *)context;
    bool success = false;
    c_player_in_game_iterator iterator;
    iterator.begin();
    while (!success && iterator.next()) {
        player_datum *player = iterator.get_datum();
        if (memcmp(&player->player_identifier, screen->get_target_player_id(), sizeof(s_player_identifier)) == 0) {
            ustrnzcpy_inline(buffer, player->configuration.host.name, buffer_length);
            success = true;
        }
    }
    return success;
}
// @address 0x487450 ?parse_tag@c_magic_string_game_tag_parser@@UAA_NPB_WPAJPA_WJ1@Z
// @address 0x4874c0 ?get_magic_string@c_magic_string_game_tag_parser@@UAA_NPA_WJ@Z
// @address 0x486f08 ?add_game_tag_parser@c_gui_screen_widget@@IAAXPAVc_game_tag_parser@@@Z

// @address 0x491460 ?handle_controller_input_message@c_gui_screen_boot_betrayer@@UAA_NPBVc_controller_input_message@@@Z
bool c_gui_screen_boot_betrayer::handle_controller_input_message(const c_controller_input_message *message)
{
    bool handled = false;
    if (message->get_event_type() == 13 && message->get_component() == 2) {
        void *memory = user_interface_malloc_tracked(sizeof(c_load_dialog_screen_message), 0, 0);
        if (memory) {
            long value = m_name;
            e_controller_index controller = message->get_controller();
            c_load_dialog_screen_message *dialog = new(memory) c_load_dialog_screen_message(controller, get_render_window(), 0x1000c, 0x30004, value);
            if (dialog) {
                dialog->set_parent_screen_index(m_screen_index);
                ::user_interface_messaging_post(dialog);
            }
        }
        handled = true;
    }
    if (!handled)
        handled = c_gui_screen_widget::handle_controller_input_message(message);
    return handled;
}


// @address 0x223098 game_grief_can_boot_betrayer
static __forceinline player_datum *gui_screen_boot_betrayer_try_get_player(long absolute_index)
{
    player_datum *player;
    if (absolute_index != -1) {
        s_data_array *data = tls_player_data();
        player_datum *entry = 0;
        if (absolute_index >= 0 && absolute_index < data->first_unallocated) {
            player_datum *candidate = static_cast<player_datum *>(datum_get_absolute(data, absolute_index));
            if (candidate->identifier != 0)
                entry = candidate;
        }
        player = entry;
    } else {
        player = 0;
    }
    return player;
}
static __forceinline long gui_screen_boot_betrayer_player_index(player_datum *player, long absolute_index)
{
    return player ? (static_cast<unsigned short>(player->identifier) << 16) | absolute_index : -1;
}
static __forceinline void gui_screen_boot_betrayer_record_ejection()
{
    if (!game_is_cooperative()) {
        long count = global_preferences_get_ejection_count();
        g_game_grief_globals.quit_decay_time = GetTickCount();
        global_preferences_set_ejection_count(count + 1);
    }
}
static __forceinline bool gui_screen_boot_betrayer_is_predicted()
{
    bool result = false;
    if (tls_game_globals()->options.simulation_type == 4)
        result = true;
    return result;
}
static __forceinline bool gui_screen_boot_betrayer_is_distributed(c_simulation_world *world)
{
    bool result;
    if (world->m_world_type == 7 || world->m_world_type == 8)
        result = true;
    else
        result = false;
    return result;
}
// @address 0x4915e0 ?boot_player@c_gui_screen_boot_betrayer@@AAAXXZ
void c_gui_screen_boot_betrayer::boot_player()
{
    long current_absolute = user_interface_session_player_get_player_index(&m_current_player);
    long target_absolute = user_interface_session_player_get_player_index(&m_target_player);
    player_datum *current = gui_screen_boot_betrayer_try_get_player(current_absolute);
    player_datum *target = gui_screen_boot_betrayer_try_get_player(target_absolute);
    long current_index = gui_screen_boot_betrayer_player_index(current, current_absolute);
    long target_index = gui_screen_boot_betrayer_player_index(target, target_absolute);
    if (current_index != -1 && target_index != -1 && game_grief_can_boot_betrayer(current_index)) {
        gui_screen_boot_betrayer_record_ejection();
        if (gui_screen_boot_betrayer_is_predicted()) {
            long request[2] = {0};
            request[0] = target_index & 0xffff;
            request[1] = current_index & 0xffff;
            c_simulation_world *world = simulation_get_world();
            if (gui_screen_boot_betrayer_is_distributed(world)) {
                // match: 0x4915e0: pointer __restrict qualifier prevents alias reload
                c_simulation_event_handler * __restrict event_handler = &world->m_distributed_world->event_handler;
                event_handler->send_event(_simulation_event_type_game_engine_request_boot_player, 0, 0, ~0ul, sizeof(request), request,
                    g_network_configuration.event_relevance[_simulation_event_type_game_engine_request_boot_player].delay_msec);
            }
        } else {
            game_engine_boot_player(target_index);
        }
        m_player_booted = true;
    }
}
