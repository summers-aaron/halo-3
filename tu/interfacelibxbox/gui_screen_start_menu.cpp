// interfacelibxbox/gui_screen_start_menu.obj: from the records 0x4974c0 0x4975f8 0x4976c8 0x497708 0x4977d0 0x4978a8 0x497a20 0x497aa8 0x497b58 0x497c28 0x497da8 0x498038 0x4982b8 0x4984b0 0x498590 0x498698 0x4987e0 0x498880 0x4989e8 0x498b10 0x498c20 0x498da0 0x498ef0 0x499030
// @address 0x94e3b0 g_saved_film_timer_globals
// @address 0xf283c8 g_network_session_interface_local_users
// @compiler xdk6130
// @flags /TP /O2 /Oc
// Inherited inline targets used by this unit's start-menu vtable.
// @address 0x1c1258 ?get_gui_location@c_gui_screen_widget@@UBA?AW4e_gui_location@@XZ
// @address 0x5bec98 ?update_render@c_gui_screen_widget@@UAAXXZ
// @address 0x42d298 ?get_emblem_info_for_widget@c_gui_screen_widget@@UAA_NPAVc_gui_widget@@PAUs_emblem_info@@@Z
#include "game/game_simulation_predicates.h"
#include "saved_games/saved_film_timing.h"
#include "interface/controller_inline.h"
#include "memory/tls_globals.h"
#include "objects/object_reference_types.h"
#include "interface/gui_file_share_selection.h"
#include "interface/gui_start_menu_externs.h"
#include "interface/gui_data.h"
#include "interface/gui_screens/start_menu/start_menu_pane_screen_widget.h"
#include "networking/session/network_session_interface_user_externs.h"
#include "interface/gui_widget_tree.h"
#include "shell/shell.h"
#include "game/player_types.h"
#include "game/game_globals_storage.h"
#include "interface/gui_widget_types.h"
#include "memory/static_arrays.h"
#include "interface/c_deletable.h"
#include "interface/user_interface_messages.h"
#include <string.h>
#include "game/game.h"
#include "interface/gui_screens/scoreboard/gui_screen_scoreboard.h"
#include "interface/user_interface_memory.h"
#include "simulation/simulation.h"
#include "game/game_engine_sandbox.h"
#include "game/game_time.h"
#include "networking/logic/network_session_interface.h"
#include "saved_games/saved_film.h"
#include "interface/c_player_profile.h"
#include "interface/user_interface_utilities.h"
#include "interface/gui_screens/start_menu/gui_screen_start_menu.h"
#include "interface/user_interface_window_manager.h"
#include "interface/user_interface_error_manager.h"
#include "networking/network_configuration.h"
#include "interface/user_interface.h"
#include "interface/c_gui_list_widget.h"
#include "interface/c_gui_screen_widget.h"
#include "interface/c_gui_text_widget.h"
#include "interface/c_gui_bitmap_widget.h"
#include <new>
#include "interface/controller_types.h"

enum { k_start_menu_sidebar_focus_animation = 31 };

#include "networking/network_configuration_types.h"
#include "interface/c_window_manager.h"
#include "game/game_state_accessors.h"
#include "interface/gui_screens/start_menu/start_menu_string_ids.h"

class c_message;
class c_gui_data;
class c_gui_screen_widget;
class c_controller_input_message;
class c_load_start_menu_screen_message : public c_load_screen_message {
    s_start_menu_breadcrumb m_breadcrumbs[8];
    long m_breadcrumb_count;
    s_player_identifier m_target_player_id;
    unsigned __int64 m_target_player_xuid;
    s_service_record_identity m_service_record_identity;
public:
    c_load_start_menu_screen_message(e_controller_index controller, e_window_index window, long layered_position,
        const s_player_identifier *identifier, const unsigned __int64 *xuid, const s_service_record_identity *identity,
        const s_start_menu_breadcrumb *breadcrumbs, long breadcrumb_count);
    virtual void apply_initial_state(c_gui_screen_widget *screen_widget) const;
};
class c_start_menu_screen_widget_sidebar_items_datasource : public c_gui_ordered_data {
public:
    virtual void get_column_names(long * const column_names, long *column_count);
    virtual bool get_string_id_value(long element_handle, long value_name, long *value);
protected:
    virtual long get_current_item_count_internal() { return k_start_menu_sidebar_item_count; }
};
// @address 0x4fe98 ??_7c_load_start_menu_pane_message@@6B@
#include "interface/gui_screens/start_menu/load_start_menu_pane_message.h"

// @address 0x4974c0 ??0c_load_start_menu_screen_message@@QAA@W4e_controller_index@@W4e_window_index@@JPBUs_player_identifier@@PB_KPBUs_service_record_identity@@PBUs_start_menu_breadcrumb@@J@Z
c_load_start_menu_screen_message::c_load_start_menu_screen_message(e_controller_index controller, e_window_index window,
    long layered_position, const s_player_identifier *identifier, const unsigned __int64 *xuid,
    const s_service_record_identity *identity, const s_start_menu_breadcrumb *breadcrumbs, long breadcrumb_count)
    : c_load_screen_message(_start_menu_string_start_menu, controller,
        // match: 0x4974c0: value range assumption guides branch lowering
        (__assume(window == _console_window), window),
        // match: 0x4974c0: value range assumption guides branch lowering
        (__assume(layered_position == 0x1000c), layered_position))
{
    m_breadcrumb_count = breadcrumb_count;
    if (breadcrumbs) {
        memcpy(m_breadcrumbs, breadcrumbs, breadcrumb_count * sizeof(s_start_menu_breadcrumb));
    }
    if (identifier) {
        m_target_player_id = *identifier;
    } else {
        memset(&m_target_player_id, 0, sizeof(m_target_player_id));
    }
    if (xuid) {
        m_target_player_xuid = *xuid;
    } else {
        memset(&m_target_player_xuid, 0, sizeof(m_target_player_xuid));
    }
    if (identity) {
        m_service_record_identity = *identity;
    } else {
        memset(&m_service_record_identity, 0, sizeof(m_service_record_identity));
    }
}

void c_load_start_menu_screen_message::apply_initial_state(c_gui_screen_widget *screen_widget) const {
    c_start_menu_screen_widget *screen = static_cast<c_start_menu_screen_widget *>(screen_widget);
    screen->set_target_player_id(&m_target_player_id);
    screen->set_target_player_xuid(&m_target_player_xuid);
    screen->set_service_record_identity(&m_service_record_identity);
    for (long index = 0; index < m_breadcrumb_count; ++index) {
        screen->add_breadcrumb(&m_breadcrumbs[index]);
    }
}
void c_start_menu_screen_widget_sidebar_items_datasource::get_column_names(long * const column_names, long *column_count) {
    *column_count = 0;
    column_names[(*column_count)++] = _start_menu_string_screen_name;
    column_names[(*column_count)++] = _start_menu_string_display_name;
}

// @address 0x4ff80 ??_7c_start_menu_screen_widget_sidebar_items_datasource@@6B@


// @address 0x497b58 ?initialize_datasource@c_start_menu_screen_widget@@UAAXXZ
void c_start_menu_screen_widget::initialize_datasource()
{
    c_gui_screen_widget::initialize_datasource();
    void *memory = user_interface_malloc_tracked(sizeof(c_start_menu_screen_widget_sidebar_items_datasource), 0, 0);
    if (memory != 0) {
        c_start_menu_screen_widget_sidebar_items_datasource *child =
            new (memory) c_start_menu_screen_widget_sidebar_items_datasource();
        if (child->initialize(_start_menu_string_start_menu_sidebar)) {
            m_datasource[m_datasource_count] = child;
            m_datasource_count++;
            return;
        }
        child->~c_start_menu_screen_widget_sidebar_items_datasource();
        user_interface_free(child);
    }
}


// 0x497aa8
// @address 0x497aa8 ?post_initialize@c_start_menu_screen_widget@@UAAXXZ
void c_start_menu_screen_widget::post_initialize()
{
    c_controller_interface *controller = controller_get(get_any_responding_controller());
    c_gui_text_widget *text = get_child_text_widget(0x2ae);
    c_gui_bitmap_widget *bitmap = (c_gui_bitmap_widget *)get_child_widget(_gui_bitmap_widget, 0x1000b);
    c_gui_screen_widget::post_initialize();
    update_pane();
    if (text)
        text->set_text(controller->display_name);
    if (bitmap)
        bitmap->set_sprite_frame(controller->get_controller_index());
}

// 0x497c28
// @address 0x497c28 ?reconstruct@c_start_menu_screen_widget@@UAAXXZ
void c_start_menu_screen_widget::reconstruct()
{
    m_pane = -1;
    m_return_breadcrumb.screen = -1;
    m_return_breadcrumb.index = -1;
    m_returning = false;
    m_breadcrumbs.clear();
    c_gui_screen_widget::reconstruct();
}


// 0x4982b8
// @address 0x4982b8 ?update_pane@c_start_menu_screen_widget@@AAAXXZ
void c_start_menu_screen_widget::update_pane()
{
    c_gui_screen_widget *pane = 0;
    if (m_pane != -1)
        pane = (c_gui_screen_widget *)get_child_widget(_gui_screen_widget, m_pane);
    if (m_pane == -1) {
        long pane_name;
        if (m_breadcrumbs.count() == 0) {
            c_gui_widget *sidebar = get_child_widget(_gui_list_widget, _start_menu_string_start_menu_sidebar);
            m_root_pane_shown = true;
            if (!sidebar)
                return;
            c_gui_data *data = ((c_gui_data *)((c_gui_list_widget *)sidebar)->get_data());
            long handle = ((c_gui_list_widget *)sidebar)->get_focused_element_handle();
            long value = -1;
            if (!data)
                return;
            if (!data->get_string_id_value(handle, _start_menu_string_screen_name, &value))
                return;
            if (value == -1 || value == 0)
                return;
            pane_name = value;
        } else {
            pane_name = m_breadcrumbs.get_top()->screen;
            m_breadcrumbs.pop();
        }
        if (pane_name == -1)
            return;
        float fade_in = pane && start_menu_pane_is_valid_type(pane)
            ? ((c_start_menu_pane_screen_widget *)pane)->m_translation : 0.0f;
        float fade_out = pane && start_menu_pane_is_valid_type(pane)
            ? ((c_start_menu_pane_screen_widget *)pane)->m_rotation : 0.0f;
        load_pane(pane_name, false, _screen_transition_normal, -1, -1, fade_in, fade_out);
    } else if (pane && pane->get_focused_widget() && m_returning) {
        c_gui_widget *focused = pane->get_focused_widget();
        if (focused->m_type == _gui_list_item_widget) {
            c_gui_widget *parent = gui_widget_get_parent_list_widget(focused);
            if (parent->m_name == _start_menu_string_start_menu_items)
                focused->start_animation_at_time(static_cast<e_animation_state>(k_start_menu_sidebar_focus_animation), (unsigned long)-1, true);
        }
        m_return_breadcrumb.screen = -1;
        m_return_breadcrumb.index = -1;
        m_returning = false;
    }
}

// 0x4984b0

// @address 0x4984b0 ?clean_up_old_panes@c_start_menu_screen_widget@@AAAXXZ
void c_start_menu_screen_widget::clean_up_old_panes(void)
{
    long focused_handle = m_pane;
    if (focused_handle != -1) {
        c_gui_widget *focused = get_child_widget(_gui_screen_widget, focused_handle);
        if (focused && !static_cast<c_gui_screen_widget *>(focused)->transitioning_out()) {
            for (c_gui_widget *widget = gui_widget_find_child_by_name(this, _gui_screen_widget); widget;
                widget = gui_widget_get_next_sibling_of_type(widget, _gui_screen_widget))
            {
                if (widget != focused && !static_cast<c_gui_screen_widget *>(widget)->transitioning_out())
                    static_cast<c_gui_screen_widget *>(widget)->transition_out_with_transition_type(0, _screen_transition_start_menu_back);
            }
        }
    }
}


// 0x498698
// @address 0x498698 ?load_pane@c_start_menu_screen_widget@@QAAXJ_NW4e_screen_transition_type@@JJMM@Z
void c_start_menu_screen_widget::load_pane(long pane, bool clear, e_screen_transition_type transition,
    long focus, long item, float translation, float rotation)
{
    c_gui_screen_widget *current = 0;
    if (m_pane != -1)
        current = (c_gui_screen_widget *)get_child_widget(_gui_screen_widget, m_pane);
    if (m_breadcrumbs.count() < 8 && pane != m_pane)
    {
        s_start_menu_breadcrumb crumb;
        crumb.screen = pane;
        crumb.index = -1;
        if (clear)
        {
            m_breadcrumbs.clear();
        }
        else if (m_breadcrumbs.count() > 0)
        {
            s_start_menu_breadcrumb *top = m_breadcrumbs.get_top();
            long index = -1;
            if (current)
            {
                c_gui_widget *focused = current->get_focused_widget();
                if (focused)
                {
                    c_gui_widget *list_item = focused->m_type == _gui_list_item_widget ? focused : gui_widget_get_parent_list_item_widget(focused);
                    index = list_item ? list_item->get_element_handle() : -1;
                }
            }
            top->index = index;
        }
        m_breadcrumbs.push_back(crumb);
        load_pane_direct(pane, transition, -1, -1, translation, rotation);
    }
}


// 0x498880
// @address 0x498880 ?load_pane_direct@c_start_menu_screen_widget@@QAAXJW4e_screen_transition_type@@JJMM@Z
void c_start_menu_screen_widget::load_pane_direct(long name, e_screen_transition_type transition,
    long focus, long item, float translation, float rotation)
{
    c_gui_widget *old_pane = 0;
    if (m_pane != -1)
        old_pane = get_child_widget(_gui_screen_widget, m_pane);
    m_pane = name;
    c_load_start_menu_pane_message *message = (c_load_start_menu_pane_message *)user_interface_malloc_tracked(sizeof(c_load_start_menu_pane_message), 0, 0);
    if (message)
    {
        long owner = m_name;
        e_controller_index controller = get_any_responding_controller();
        e_window_index window = get_render_window();
        new (message) c_load_start_menu_pane_message(name, controller, window, owner,
            &m_owner_player_identifier, &m_target_player_identifier, &m_target_xuid,
            &m_service_record_identity, translation, rotation);
        if (old_pane)
            static_cast<c_gui_screen_widget *>(old_pane)->transition_out_with_transition_type(0, transition);
        message->set_transition_type(transition);
        message->set_parent_screen_index(m_screen_index);
        if (focus != -1)
        {
            message->set_focus_on_load(focus, item);
        }
        ::user_interface_messaging_post((c_message const *)(c_deletable *)message);
    }
}



static __forceinline bool gui_screen_start_menu_get_alpha_is_locked_down()
{
    return g_user_interface_is_alpha && (g_network_configuration.alpha_locked_down == 0 || !g_eula_accepted);
}

static __forceinline bool game_time_get_paused()
{
    bool result;
    if (tls_game_time_globals()->initialized)
        result = (tls_game_time_globals()->flags & 4) != 0;
    else
        result = false;
    return result;
}





// @address 0x4987e0 ?transition_out_with_transition_type@c_start_menu_screen_widget@@UAAXJW4e_screen_transition_type@@@Z
void c_start_menu_screen_widget::transition_out_with_transition_type(long state, e_screen_transition_type message)
{
    for (c_gui_widget *widget = gui_widget_find_child_by_name(this, _gui_screen_widget); widget;
        widget = gui_widget_get_next_sibling_of_type(widget, _gui_screen_widget))
    {
        if (!static_cast<c_gui_screen_widget *>(widget)->transitioning_out())
            static_cast<c_gui_screen_widget *>(widget)->transition_out_with_transition_type(state, (e_screen_transition_type)message);
    }
    window_manager_close_screen_animated(&g_window_manager, this, message);
}

// @address 0x498b10 start_menu_get_game_screen_name
extern "C" long start_menu_get_game_screen_name(void)
{
    long name = -1;
    if (game_in_progress())
    {
        if (game_is_cooperative())
            name = _start_menu_string_start_menu_game_cooperative;
        else
        {
            long game_mode = tls_game_globals()->options.game_mode;
            if (game_mode == 1)
                name = _start_menu_string_start_menu_game_campaign;
            else if (game_engine_sandbox_is_active())
                name = _start_menu_string_start_menu_game_editor;
            else if (game_mode == 2)
                name = _start_menu_string_start_menu_game_multiplayer;
        }
    }
    return name;
}


// @address 0x497708 ?get_string_id_value@c_start_menu_screen_widget_sidebar_items_datasource@@UAA_NJJPAJ@Z
bool c_start_menu_screen_widget_sidebar_items_datasource::get_string_id_value(long element_handle, long value_name, long *value)
{
    bool result = false;
    if (element_handle != -1 && (value_name == _start_menu_string_screen_name || value_name == _start_menu_string_display_name)) {
        if ((unsigned char)simulation_starting_up() != 0 && tls_game_globals()->options.game_mode == 3) {
            switch (element_handle) {
            case _start_menu_sidebar_game:
                *value = _start_menu_string_start_menu_game_saved_film;
                result = true;
                break;
            case _start_menu_sidebar_hq:
                *value = _start_menu_string_start_menu_hq;
                result = true;
                break;
            }
        } else {
            switch (element_handle) {
            case _start_menu_sidebar_game:
                {
                int name = start_menu_get_game_screen_name();
                if (name != -1) {
                    *value = name;
                    result = true;
                }
                break;
                }
            case _start_menu_sidebar_hq:
                *value = _start_menu_string_start_menu_hq;
                result = true;
                break;
            }
        }
    }
    return result;
}
// @address 0x498da0 ?load_start_menu_internal@c_start_menu_screen_widget@@CA_NW4e_controller_index@@PBUs_player_identifier@@PB_KPBUs_service_record_identity@@PBUs_start_menu_breadcrumb@@J_N5@Z
bool c_start_menu_screen_widget::load_start_menu_internal(e_controller_index controller,
    const s_player_identifier *identifier, const unsigned __int64 *xuid,
    const s_service_record_identity *identity, const s_start_menu_breadcrumb *breadcrumbs,
    long breadcrumb_count, bool force_reload, bool immediate)
{
    bool loaded = false;
    if (!gui_screen_start_menu_get_alpha_is_locked_down())
    {
        e_window_index window = _console_window;
        c_gui_screen_widget *screen = g_window_manager.get_screen_by_name(window, _start_menu_string_start_menu);
        bool already_loaded = screen != 0 && screen->get_any_responding_controller() == controller;
        if (!already_loaded || force_reload)
        {
            if (screen)
                screen->transition_out_with_transition_type(0, _screen_transition_normal);
            void *storage = user_interface_malloc_tracked(sizeof(c_load_start_menu_screen_message), 0, 0);
            c_load_start_menu_screen_message *message = storage ? new (storage) c_load_start_menu_screen_message(
                controller, window, 0x1000c, identifier, xuid, identity, breadcrumbs, breadcrumb_count) : 0;
            if (message)
                ::user_interface_messaging_post(message);
            loaded = true;
        }
    }
    return loaded;
}

// @address 0x494790 ?post_error_with_custom_message@c_gui_error_manager@@QAAXJPB_W0W4e_controller_index@@_N2@Z

namespace {
static __forceinline bool gui_screen_start_menu_does_xuid_point_to_a_bungie_file_share(unsigned __int64 xuid)
{
    return xuid >= (unsigned __int64)-255 && xuid <= (unsigned __int64)-2;
}
}

// @address 0x499030 ?load_start_menu_for_file_share@c_start_menu_screen_widget@@SA_NW4e_controller_index@@PBUs_player_identifier@@PB_KPBUs_service_record_identity@@J_N@Z
bool c_start_menu_screen_widget::load_start_menu_for_file_share(e_controller_index controller,
    const s_player_identifier *identifier, const unsigned __int64 *xuid,
    const s_service_record_identity *identity, long slot_index, bool)
{
    bool result = false;
    if (game_in_progress() && game_is_ui_shell())
    {
        s_start_menu_breadcrumb breadcrumbs[8];
        long count = 0;
        g_file_share_selected_slot = -1;
        g_file_share_selected_slot = slot_index;
        bool is_file_share = gui_screen_start_menu_does_xuid_point_to_a_bungie_file_share(*xuid);
        s_start_menu_breadcrumb breadcrumb;
        breadcrumb.index = slot_index;
        breadcrumb.screen = is_file_share ? _start_menu_string_start_menu_bungie_file_share : _start_menu_string_start_menu_file_share;
        breadcrumbs[count++] = breadcrumb;
        result = load_start_menu_internal(controller, identifier, xuid, identity, breadcrumbs, count, true, true);
    }
    else
    {
        g_gui_error_manager.post_error_with_custom_message(0x20017, 0, 0, controller, false, true);
    }
    return result;
}
static __forceinline unsigned __int64 gui_screen_start_menu_get_player_xuid(const c_controller_interface *controller)
{
    short index = controller->get_user_index();
    unsigned __int64 result = 0;
    if (index >= 0 && index < 4)
        result = g_network_session_interface_local_users[index].xuid;
    return result;
}
// @address 0x98818 ?game_campaign_is_multiplayer@@YA_NXZ
// @address 0x4978a8 ?initialize@c_start_menu_screen_widget@@UAAXXZ
void c_start_menu_screen_widget::initialize()
{
    c_start_menu_screen_widget *state = this;
    c_controller_interface *controller = controller_get(get_any_responding_controller());
    c_gui_screen_widget::initialize();
    ((const c_controller_interface *)controller)
        ->get_player_identifier(&state->m_owner_player_identifier);
    bool target_missing = state->m_owner_player_identifier.is_empty();
    if (target_missing)
        transition_out_with_transition_type(0, _screen_transition_normal);
    bool secondary_missing = state->m_target_player_identifier.is_empty();
    if (secondary_missing)
        ((const c_controller_interface *)controller)
            ->get_player_identifier(&state->m_target_player_identifier);
    if (state->m_target_xuid == 0)
    {
        unsigned __int64 id = 0;
        short slot = controller->user_index;
        if (slot >= 0 && slot < 4)
            id = g_network_session_interface_local_users[slot].xuid;
        state->m_target_xuid = id;
    }
    if (game_is_cooperative())
    {
        state->m_saved_film_playback_speed = g_saved_film_timer_globals.playback_speed;
        saved_film_set_playback_game_speed(0.0f);
    }
    if (tls_game_globals()->options.game_mode == 1)
        state->m_pause_game = !(game_campaign_is_multiplayer() && network_squad_session_get_peer_count() > 1);
}

// 0x498ef0
// @address 0x498ef0 ?load_start_menu_for_service_record@c_start_menu_screen_widget@@SA_NW4e_controller_index@@PBUs_player_identifier@@PB_KPBUs_service_record_identity@@_N@Z
bool c_start_menu_screen_widget::load_start_menu_for_service_record(e_controller_index controller,
    const s_player_identifier *identifier, const unsigned __int64 *xuid,
    const s_service_record_identity *identity, bool)
{
    bool result = false;
    if (game_in_progress() && game_is_ui_shell())
    {
        s_start_menu_breadcrumb breadcrumbs[8];
        long count = 0;
        g_file_share_selected_slot = -1;
        s_start_menu_breadcrumb breadcrumb(_start_menu_string_start_menu_service_record, -1);
        breadcrumbs[count++] = breadcrumb;
        result = load_start_menu_internal(controller, identifier, xuid, identity, breadcrumbs, count, true, true);
    }
    else
    {
        g_gui_error_manager.post_error_with_custom_message(0x20018, 0, 0, controller, false, true);
    }
    return result;
}

// 0x497a20
// @address 0x497a20 ?dispose@c_start_menu_screen_widget@@UAAXXZ
void c_start_menu_screen_widget::dispose()
{
    c_gui_screen_widget::dispose();
    if (game_is_cooperative())
        saved_film_set_playback_game_speed(m_saved_film_playback_speed);
    unsigned char paused;
    if (tls_game_time_globals()->initialized)
        paused = (tls_game_time_globals()->flags >> 2) & 1;
    else
        paused = 0;
    if (paused != 0)
        game_time_set_pause_reason(0, 2);
}

// 0x4977d0
// @address 0x4977d0 ??0c_start_menu_screen_widget@@QAA@J@Z
c_start_menu_screen_widget::c_start_menu_screen_widget(long name)
    : c_gui_screen_widget(name), m_breadcrumbs(), m_pane(-1), m_returning(false),
      m_return_breadcrumb(), m_root_pane_shown(false), m_pause_game(false)
{
    m_return_breadcrumb.reset();
    memset(&m_owner_player_identifier, 0, sizeof(m_owner_player_identifier));
    memset(&m_target_player_identifier, 0, sizeof(m_target_player_identifier));
    m_target_xuid = 0;
}

// 0x4989e8
// @address 0x4989e8 ?back_out_current_pane@c_start_menu_screen_widget@@QAAXXZ
void c_start_menu_screen_widget::back_out_current_pane()
{
    if (m_breadcrumbs.count() <= 1) {
        transition_out_with_transition_type(1, _screen_transition_normal);
        return;
    }
    c_gui_screen_widget *pane = 0;
    if (m_pane != -1) {
        pane = (c_gui_screen_widget *)get_child_widget(_gui_screen_widget, m_pane);
        if (pane)
            pane->transition_out_with_transition_type(0, _screen_transition_start_menu_back);
    }
    m_breadcrumbs.pop();
    s_start_menu_breadcrumb *breadcrumb = m_breadcrumbs.get_top();
    m_return_breadcrumb = *breadcrumb;
    m_returning = true;
    float fade_in = pane && start_menu_pane_is_valid_type(pane)
        ? ((c_start_menu_pane_screen_widget *)pane)->m_translation : 0.0f;
    float fade_out = pane && start_menu_pane_is_valid_type(pane)
        ? ((c_start_menu_pane_screen_widget *)pane)->m_rotation : 0.0f;
    load_pane_direct(m_return_breadcrumb.screen, _screen_transition_start_menu_back,
        _start_menu_string_start_menu_items, m_return_breadcrumb.index, fade_in, fade_out);
}


static __forceinline bool gui_screen_start_menu_root_pane(bool root, long count)
{
    return root && count == 1;
}

// @address 0x498c20 ?handle_global_start_button_press@c_start_menu_screen_widget@@SA_NPBVc_controller_input_message@@@Z
bool c_start_menu_screen_widget::handle_global_start_button_press(const c_controller_input_message *controller_input_message)
{
    bool result = false;
    if (controller_input_message->get_event_type() == _event_type_button_press && controller_input_message->get_component() == 12)
    {
        c_controller_interface *controller = controller_get(controller_input_message->get_controller());
        if (!simulation_is_pending_start() && controller->in_use() && controller->get_user_index() != -1)
        {
            e_controller_index controller_index = controller_input_message->get_controller();
            s_player_identifier identifier;
            unsigned __int64 xuid;
            c_controller_interface *player_controller = controller_get(controller_index);
            player_controller->get_player_identifier(&identifier);
            xuid = gui_screen_start_menu_get_player_xuid(player_controller);
            g_file_share_selected_slot = -1;
            result = load_start_menu_internal(controller_input_message->get_controller(),
                &identifier, &xuid, 0, 0, 0, false, true);
        }
    }
    return result;
}


static __forceinline bool gui_screen_start_menu_can_pause_game()
{
    bool result;
    if (game_campaign_has_multiple_players() && (!game_simulation::game_is_server()
        || network_squad_session_get_peer_count() > 1))
        result = false;
    else
        result = true;
    return result;
}

// @address 0x497da8 ?update@c_start_menu_screen_widget@@UAAXK@Z
void c_start_menu_screen_widget::update(unsigned long current_milliseconds)
{
    if (gui_screen_start_menu_get_alpha_is_locked_down()) {
        void *storage = ::user_interface_malloc_tracked(sizeof(c_load_screen_message), 0, 0);
        if (storage) {
            c_load_screen_message *message = new (storage) c_load_screen_message(
                _start_menu_string_alpha_locked_down, k_any_controller, _console_window, 0x1000c);
            if (message)
                ::user_interface_messaging_post(message);
        }
        transition_out_with_transition_type(0, _screen_transition_normal);
    } else {
        c_controller_interface *controller = controller_get(get_any_responding_controller());
        s_player_identifier player;
        controller->get_player_identifier(&player);
        if (!controller->in_use()) {
            transition_out_with_transition_type(0, _screen_transition_normal);
        } else if (memcmp(&player, &m_owner_player_identifier, sizeof(player))) {
            transition_out_with_transition_type(0, _screen_transition_normal);
        } else {
            update_pane();
            clean_up_old_panes();
        }
    }
    if (m_pause_game && !transitioning_in() && !transitioning_out() &&
        !game_time_get_paused() && gui_screen_start_menu_can_pause_game())
    {
        game_time_set_pause_reason(true, 2);
        m_pause_game = false;
    }
    c_gui_screen_widget::update(current_milliseconds);
}

// @address 0x498590 ?update_render_state@c_start_menu_screen_widget@@UAAXK@Z
void c_start_menu_screen_widget::update_render_state(unsigned long current_milliseconds)
{
    e_controller_index controller = get_any_responding_controller();
    c_gui_widget *header = get_child_widget(_gui_text_widget, 686);
    long color = controller_get(controller)->get_player_profile_interface()->get_primary_change_color();
    c_gui_widget::update_render_state(current_milliseconds);
    if (header)
        tint_widget_to_change_color(header, color, false);
    for (c_gui_widget *child = get_children(); child; child = child->get_next())
    {
        c_gui_widget *widget = child->get_child_widget(_gui_bitmap_widget, _start_menu_string_base_color);
        if (widget)
        {
            for (;;)
            {
                tint_widget_to_change_color(widget, color, false);
                c_gui_widget *next = gui_widget_get_next_sibling_of_type(widget, _gui_bitmap_widget);
                while (next && next->m_name != _start_menu_string_base_color)
                    next = gui_widget_get_next_sibling_of_type(next, _gui_bitmap_widget);
                if (!next)
                    break;
                widget = next;
            }
        }
    }
}

// @address 0x498038 ?handle_controller_input_message@c_start_menu_screen_widget@@UAA_NPBVc_controller_input_message@@@Z
bool c_start_menu_screen_widget::handle_controller_input_message(const c_controller_input_message *message)
{
    bool handled = false;
    long event = message->get_event_type();
    long component = message->get_component();
    c_gui_widget *focused = get_focused_widget();
    if (event == _event_type_button_press && component == 12)
    {
        if (!transitioning_out())
            transition_out_with_transition_type(1, _screen_transition_normal);
        handled = true;
    }
    else if (event == _event_type_button_press && component == 1)
    {
        if (m_breadcrumbs.count() == 1)
            transition_out_with_transition_type(1, _screen_transition_normal);
        else
            back_out_current_pane();
        handled = true;
    }
    else if (gui_screen_start_menu_root_pane(m_root_pane_shown, m_breadcrumbs.count()) && (event == 2 || event == 4))
    {
        long pane_name = -1;
        c_gui_list_widget *list = (c_gui_list_widget *)gui_widget_get_parent_list_widget(focused);
        long element = ((c_gui_list_widget *)list)->get_focused_element_handle();
        e_screen_transition_type transition;
        if (event == 4)
        {
            c_gui_data *data = ((c_gui_data *)((c_gui_list_widget *)list)->get_data());
            element = data->get_next_element_handle(element);
            transition = _screen_transition_start_menu_next;
        }
        else
        {
            c_gui_data *data = ((c_gui_data *)((c_gui_list_widget *)list)->get_data());
            element = data->get_previous_element_handle(element);
            transition = _screen_transition_start_menu_previous;
        }
        c_gui_data *data = ((c_gui_data *)((c_gui_list_widget *)list)->get_data());
        if (data->get_string_id_value(element, _start_menu_string_screen_name, &pane_name) && pane_name != -1 && pane_name != 0)
        {
            c_gui_screen_widget *pane = 0;
            if (m_pane != -1)
                pane = (c_gui_screen_widget *)get_child_widget(_gui_screen_widget, m_pane);
            float translation = pane && start_menu_pane_is_valid_type(pane)
                ? ((c_start_menu_pane_screen_widget *)pane)->m_translation : 0.0f;
            float rotation = pane && start_menu_pane_is_valid_type(pane)
                ? ((c_start_menu_pane_screen_widget *)pane)->m_rotation : 0.0f;
            load_pane(pane_name, true, transition, -1, -1, translation, rotation);
        }
    }
    if (!handled)
        handled = c_gui_screen_widget::handle_controller_input_message(message);
    return handled;
}
