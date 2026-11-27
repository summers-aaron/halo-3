// interfacelibxbox/user_interface_messages.obj: from the records 0x89778 0x89828 0x8cc70 0x482b08 0x482c40 0x482ce8 0x483130 0x4831c0 0x483240 0x4832e8 0x483310 0x4833d0 0x483428 0x4834a0 0x483510 0x70cc68
// @compiler xdk6130
// @flags /TP /O2 /Oc
#include <xtl.h>
#include "game/game_time_types.h"
#include "networking/logic/network_session_interface.h"
// @address 0x70d344 XNotifyGetNext
// @address 0x4e49c ??_7c_load_alert_screen_message@@6B@
// @address 0x4c4bb8 ??_Gc_load_alert_screen_message@@UAAPAXI@Z
// @address 0x4c4bb8 ??_Ec_load_alert_screen_message@@UAAPAXI@Z
#include "networking/session/session_network_message_types.h"
#include "memory/tls_globals.h"
#include <stddef.h>
#include "shell/shell.h"
#include "game/game_globals_storage.h"
#include "interface/user_interface_messages.h"
#include "interface/c_player_marketplace.h"

#include <string.h>
#include "interface/gui_screens/scoreboard/gui_screen_scoreboard.h"
#include "interface/user_interface.h"
#include "interface/user_interface_memory.h"
#include "networking/online/online_guide_xenon.h"
#include "networking/logic/network_join.h"
#include "saved_games/content_catalogue.h"
#include "networking/online/voice_xenon.h"
#include "networking/transport/transport_security.h"
#include "networking/transport/transport_security_types.h"

#include "interface/c_gui_screen_widget.h"
#include "interface/gui_screens/dialog/gui_screen_dialog.h"


#include "game/game_time_inline.h"
#include "networking/online/voice_interface_types.h"
#include "game/game_state_accessors.h"

#include "interface/user_interface_message_inlines.h"
#include "interface/gui_dialog_definition.h"
#include "interface/user_interface_shared_globals.h"
#include "interface/user_interface_message_globals.h"
#include "interface/gui_screens/alert/gui_screen_alert.h"
#include "interface/gui_screens/gui_screen_in_progress.h"
#include "interface/c_player_marketplace.h"
#include "interface/c_window_manager.h"
#include "interface/user_interface_error_manager.h"
#include "interface/user_interface_window_manager.h"
#include "interface/user_interface_networking.h"
#include "networking/session/network_session_class.h"
#include "interface/user_interface_allocation.h"
#include "interface/gui_screens/start_menu/start_menu_string_ids.h"

long c_load_dialog_screen_message::get_dialog_screen_name(long dialog_name)
{
    long screen_name=-1;
    const s_user_interface_shared_globals *shared = ::user_interface_shared_tag_globals_try_and_get();
    if (shared) {
        for (long i=0;i<shared->dialogs.count;++i) {
            s_gui_dialog_definition *dialog = &((s_gui_dialog_definition *)shared->dialogs.address)[i];
            if (dialog->name==dialog_name) {
                screen_name=dialog->choices[2]==-1 ? 0x100a1 : 0x100a2;
                break;
            }
        }
    }
    return screen_name;
}

static __forceinline c_message_globals *message_globals_get()
{
    return &g_message_globals;
}

// @address 0x89778 ?user_interface_messaging_get_next_message@@YA_NJW4e_controller_index@@W4e_window_index@@PAPBVc_message@@@Z
bool user_interface_messaging_get_next_message(long screen_name, e_controller_index controller, e_window_index window, const c_message **message_reference)
{
    // Callers always ask for any controller.
    // match: 0x89778: value range assumption guides branch lowering
    __assume(controller == k_any_controller);
    bool result = false;
    if (game_in_progress())
    {
        message_globals_get()->get_next_message(screen_name, controller, window, message_reference);
        result = *message_reference != 0;
    }
    return result;
}

inline void c_message_globals::get_next_message(long screen_name, e_controller_index controller, e_window_index window, const c_message **message_reference)
{
    // The global queue is always read for any controller.
    // match: 0x89828: known any-controller folds the cmpwi-255 re-check and the r31 spill frame
    __assume(controller == k_any_controller);
    const c_message *previous_message = *message_reference;
    const c_message *result = 0;
    s_message_queue_node *next_node = 0;
    if (!previous_message)
        next_node = message_globals_get()->m_next_read;
    else
    {
        for (s_message_queue_node *test_node = message_globals_get()->m_next_read;
            test_node && test_node != message_globals_get()->m_next_write; test_node = test_node->m_next)
        {
            if (test_node->m_message && test_node->m_message == previous_message)
            {
                next_node = test_node->m_next;
                break;
            }
        }
    }
    while (next_node && next_node != message_globals_get()->m_next_write)
    {
        if (next_node->m_message && message_match(next_node->m_message, screen_name, controller, window))
        {
            result = next_node->m_message;
            break;
        }
        else
            next_node = next_node->m_next;
    }
    *message_reference = result;
}

bool user_interface_message_queue_is_empty()
{
    bool result = true;
    if (game_in_progress())
        result = !message_globals_get()->can_read();
    return result;
}

const c_message *c_message_globals::dequeue_node(s_message_queue_node *node, bool unlink)
{
    // The node is always unlinked from the global queue.
    const c_message *result = node->m_message;
    {
        if (node == message_globals_get()->m_next_read)
            message_globals_get()->m_next_read = message_globals_get()->m_next_read->m_next;
        s_message_queue_node *next = node->m_next;
        s_message_queue_node *previous = node->m_previous;
        next->m_previous = node->m_previous;
        previous->m_next = node->m_next;
        node->m_message = 0;
        s_message_queue_node *free_next = message_globals_get()->m_next_write->m_next;
        free_next->m_previous = node;
        message_globals_get()->m_next_write->m_next = node;
        node->m_previous = message_globals_get()->m_next_write;
        node->m_next = free_next;
    }
    return result;
}

__forceinline void c_message_globals::dequeue(const c_message *message)
{
    for (s_message_queue_node *node=m_next_read; node && node!=m_next_write; node=node->m_next) {
        if (node->m_message && node->m_message==message) {
            dequeue_node(node,true);
            break;
        }
    }
}
// @address 0x482c40 ?user_interface_messaging_pop@@YAXPBVc_message@@@Z
void user_interface_messaging_pop(const c_message *message)
{
    if (game_in_progress()) message_globals_get()->dequeue(message);
}

// @address 0x483130 ??0c_controller_input_message@@QAA@JW4e_controller_index@@W4e_window_index@@W4e_event_type@@W4e_controller_component@@J@Z
c_controller_input_message::c_controller_input_message(long screen_name, e_controller_index controller, e_window_index window,
    e_event_type event_type, e_controller_component component, long event_value)
    : c_message(_ui_controller_input, screen_name, controller, window)
{
    m_event_type = event_type;
    m_component = component;
    m_event_value = event_value;
    m_handled = false;
}

// @address 0x4831c0 ??0c_xenon_message@@QAA@W4e_controller_index@@W4e_xenon_message_type@0@J@Z
c_xenon_message::c_xenon_message(e_controller_index controller, e_xenon_message_type xenon_message_type, long event_value)
    : c_message(_ui_xenon, -1, _controller_none, _window_none)
{
    m_xenon_message_type = xenon_message_type;
    m_event_value = event_value;
}

c_load_screen_message::c_load_screen_message(long screen_name, e_controller_index controller, e_window_index window, long layered_position)
    : c_message(_ui_load_screen, screen_name, controller, window)
{
    m_transition_type = _screen_transition_normal;
    m_respond_to_controller_events = true;
    m_focus_on_load_list_name = -1;
    m_focus_on_load_element_handle = -1;
    m_focus_on_load_column_name = -1;
    m_focus_on_load_column_value = -1;
    m_parent_screen_index = -1;
    m_layered_position = layered_position;
    m_applies_even_to_codeless_screens = false;
}

void c_load_screen_message::apply_initial_state(c_gui_screen_widget *screen) const
{
}


c_load_alert_screen_message::c_load_alert_screen_message(
    long screen_name, e_controller_index controller, e_window_index window, const c_gui_queued_error *error)
    : c_load_screen_message(screen_name, controller, window, _string_id_top_most), m_error(*error)
{
}

// @address 0x4832e8 ?apply_initial_state@c_load_alert_screen_message@@UBAXPAVc_gui_screen_widget@@@Z
void c_load_alert_screen_message::apply_initial_state(c_gui_screen_widget *screen) const
{
    c_gui_alert_screen_widget *alert_screen = static_cast<c_gui_alert_screen_widget *>(screen);
    if (alert_screen->running_in_codeless_mode())
        return;
    // match: 0x4832e8: taking the source address first puts this+60 in r11 before the destination in r3
    const c_gui_queued_error *error = &m_error;
    memcpy(&alert_screen->m_error, error, sizeof(alert_screen->m_error));
}

// @address 0x483310 ??0c_load_dialog_screen_message@@QAA@W4e_controller_index@@W4e_window_index@@JJJ@Z
c_load_dialog_screen_message::c_load_dialog_screen_message(
    e_controller_index controller, e_window_index window, long layered_position, long dialog_name, long dialog_invoker)
    : c_load_screen_message(get_dialog_screen_name(dialog_name),controller,window,layered_position)
{
    m_dialog_name=dialog_name;
    m_dialog_invoker=dialog_invoker;
    m_test_mode=false;
    m_dispose_on_success_screen_index=-1;
}

void c_load_dialog_screen_message::apply_initial_state(c_gui_screen_widget *screen) const
{
    if (!screen->running_in_codeless_mode()) {
        c_gui_dialog_screen_widget *dialog_screen = static_cast<c_gui_dialog_screen_widget *>(screen);
        dialog_screen->set_dialog_name(m_dialog_name);
        dialog_screen->set_dialog_invoker(m_dialog_invoker);
        dialog_screen->set_test_mode(m_test_mode);
        dialog_screen->set_dispose_on_success_screen_index(m_dispose_on_success_screen_index);
    }
}


// @address 0x483428 ??0c_load_in_progress_screen_message@@QAA@W4e_controller_index@@W4e_window_index@@JJ@Z
c_load_in_progress_screen_message::c_load_in_progress_screen_message(e_controller_index controller, e_window_index window, long title, long message)
    : c_load_screen_message(0x100b3, controller, window, 0x1000c)
{
    m_title = title;
    m_message = message;
    m_respond_to_controller_events = true;
    m_show_progress = false;
    m_applies_even_to_codeless_screens = true;
}

// @address 0x4834a0 ??0c_load_in_progress_screen_message@@QAA@JW4e_controller_index@@W4e_window_index@@JJJJ@Z
c_load_in_progress_screen_message::c_load_in_progress_screen_message(long screen_name, e_controller_index controller, e_window_index window,
    long title, long message, long respond_to_controller_events, long show_progress)
    : c_load_screen_message(screen_name, controller,
        // match: 0x4834a0: value range assumption guides branch lowering
        (__assume(window == _console_window), window), 0x1000c)
{
    // The screen always shows on the console window, ignoring input, with the progress shown.
    // match: 0x4834a0: value range assumption guides branch lowering
    __assume(respond_to_controller_events == 0);
    // match: 0x4834a0: value range assumption guides branch lowering
    __assume(show_progress == 1);
    m_title = title;
    m_message = message;
    m_respond_to_controller_events = respond_to_controller_events;
    m_show_progress = show_progress;
    m_applies_even_to_codeless_screens = true;
}

// @address 0x483510 ?apply_initial_state@c_load_in_progress_screen_message@@UBAXPAVc_gui_screen_widget@@@Z
void c_load_in_progress_screen_message::apply_initial_state(c_gui_screen_widget *screen) const
{
    c_gui_screen_in_progress *in_progress_screen = static_cast<c_gui_screen_in_progress *>(screen);
    in_progress_screen->setup(m_title, m_message);
    if (!m_respond_to_controller_events)
        screen->set_responds_to_controller_events(false);
    in_progress_screen->set_show_progress(m_show_progress);
}

// Unexpected notifications only log in debug builds ("ui: XNotifyGetNext() unexpected ...").
// match: 0x482ce8: the call in the default case keeps the switch lowering of the listed notifications
static __forceinline void user_interface_messages_ignore_notification(long) {}
static __forceinline c_xenon_message *user_interface_messages_make_xenon(long type, long value)
{
    return new(0, 0, _ui_allocation_marker_dummy) c_xenon_message(_controller_none, (c_xenon_message::e_xenon_message_type)type, value);
}
static __forceinline void user_interface_messages_content_changed(c_window_manager *window_manager)
{
    content_catalogue_dispose_all_on_content_change();
    c_gui_screen_widget *screen = window_manager->get_screen_by_name(_console_window, 0x100ac);
    if (screen)
        screen->transition_out_with_transition_type(0, _screen_transition_normal);
}
// PC (port/GAME-CHANGES.md, "Game invites and lobby codes"): the console's XAM delivers invites only to Xbox Live
// sessions (an online peer XNKID), which the invite joins as Xbox Live; the port's lobby codes also carry System Link
// lobbies (a System Link XNKID, XNetXnKidIsSystemLink), which join as System Link, the class the game browser joins
// them with (port/sdk/online/invite.h).
static e_network_session_class user_interface_messages_invite_session_class(const XINVITE_INFO *invite)
{
    return XNetXnKidIsSystemLink(&invite->hostInfo.sessionID) ? _network_session_class_system_link
                                                              : _network_session_class_xbox_live;
}

// PC (port/GAME-CHANGES.md, "Game invites and lobby codes"): the squad session's lobby, reported to the port's invites
// once per UI update (port/probe/extra/invite_lobby.cpp): what a lobby code or a Live invite names.
extern "C" void invite_lobby_update(void);

// PC (port/GAME-CHANGES.md, "The Guide's menu sounds and hold-to-repeat"): the Guide's menu sounds played and its
// repeat timing read, once per UI update (port/probe/extra/guide_menu.cpp), after the game's own menus' input.
extern "C" void guide_menu_update(void);

// @address 0x482ce8 ?update_xenon_messages@@YAXXZ
void update_xenon_messages()
{
    invite_lobby_update();
    guide_menu_update();
    c_window_manager *window_manager = &g_window_manager;
    c_message_globals *globals = message_globals_get();
    void *notification = globals->notification_handle;
    if (notification && notification != INVALID_HANDLE_VALUE)
    {
        unsigned long id;
#ifdef HALO3_64BIT
        ULONG_PTR parameter;
#else
        long parameter;
#endif
        while (XNotifyGetNext(notification, 0, &id, (PULONG_PTR)&parameter))
        {
            c_message *message = 0;
            switch (id)
            {
            case XN_SYS_UI:
                if (parameter)
                {
                    globals->flag = true;
                    network_session_interface_handle_message(_session_network_message_guide_opened);
                }
                else
                {
                    globals->flag = false;
                    network_session_interface_handle_message(_session_network_message_guide_closed);
                }
                break;
            case XN_SYS_SIGNINCHANGED:
                message = user_interface_messages_make_xenon(4, (long)parameter);
                break;
            case XN_SYS_PROFILESETTINGCHANGED:
                message = user_interface_messages_make_xenon(5, (long)parameter);
                break;
            case XN_SYS_INPUTDEVICESCHANGED:
                message = user_interface_messages_make_xenon(6, -1);
                break;
            case XN_LIVE_CONNECTIONCHANGED:
                switch ((long)parameter)
                {
                case XONLINE_S_LOGON_CONNECTION_ESTABLISHED:
                    network_session_interface_handle_message(_session_network_message_online_connected);
                    break;
                case XONLINE_S_LOGON_DISCONNECTED:
                    network_session_interface_handle_message(_session_network_message_online_disconnected);
                    break;
                case XONLINE_E_LOGON_CONNECTION_LOST:
                    network_session_interface_handle_message(_session_network_message_online_duplicate_logon);
                    break;
                case XONLINE_E_LOGON_KICKED_BY_DUPLICATE_LOGON:
                    g_gui_error_manager.post_error_with_custom_message(0x2002f, 0, 0, k_any_controller, false, true);
                    break;
                }
                break;
            case XN_LIVE_INVITE_ACCEPTED:
                if (get_alpha_is_locked_down() && network_join_is_eula_accepted())
                {
                    message = new(0, 0, _ui_allocation_marker_dummy) c_load_screen_message(
                        _start_menu_string_alpha_locked_down, k_any_controller, _console_window, 0x1000c);
                }
                else
                {
                    XINVITE_INFO invite = {0};
                    if (XInviteGetAcceptedInfo(parameter, &invite) == ERROR_SUCCESS)
                        user_interface_join_remote_session(!invite.fFromGameInvite,
                            user_interface_messages_invite_session_class(&invite),
                            (const s_transport_secure_identifier *)&invite.hostInfo.sessionID,
                            (const s_transport_secure_address *)&invite.hostInfo.hostAddress,
                            (const s_transport_secure_key *)&invite.hostInfo.keyExchangeKey);
                }
                break;
            case XN_LIVE_CONTENT_INSTALLED:
                for (long controller = 0; controller < k_number_of_controllers; controller++)
                {
                    g_player_marketplace_users[controller].controller_index = controller;
                    g_player_marketplace_users[controller].refresh_requested = true;
                }
                user_interface_messages_content_changed(window_manager);
                break;
            case XN_SYS_STORAGEDEVICESCHANGED:
                user_interface_messages_content_changed(window_manager);
                break;
            case XN_LIVE_VOICECHAT_AWAY:
                g_voice_interface_globals.voice_chat_away = parameter != 0;
                break;
            case XN_SYS_MUTELISTCHANGED:
                break;
            case XN_LIVE_LINK_STATE_CHANGED:
                break;
            case XN_LIVE_MEMBERSHIP_PURCHASED:
                break;
            case XN_LIVE_PRESENCE_CHANGED:
                break;
            case XN_FRIENDS_PRESENCE_CHANGED:
                break;
            case XN_FRIENDS_FRIEND_ADDED:
                break;
            case XN_FRIENDS_FRIEND_REMOVED:
                break;
            case XN_CUSTOM_GAMEBANNERPRESSED:
                break;
            case XN_XMP_STATECHANGED:
                break;
            case XN_XMP_PLAYBACKBEHAVIORCHANGED:
                break;
            case XN_XMP_PLAYBACKCONTROLLERCHANGED:
                break;
            case XN_CUSTOM_ACTIONPRESSED:
                online_guide_handle_dynamic_action();
                break;
            case XN_CUSTOM_GAMERCARD:
                online_guide_handle_custom_gamercard(parameter != 0);
                break;
            default:
                user_interface_messages_ignore_notification(id);
                break;
            }
            if (message)
                ::user_interface_messaging_post(message);
        }
    }
}

// @address 0x1142a68 ?g_message_globals@@3Vc_message_globals@@A
c_message_globals g_message_globals;
