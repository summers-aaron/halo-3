// interfacelibxbox/gui_pregame_setup_manager.obj: from the records 0x19f8a0 0x4790c8 0x4a1bb0 0x4a1e00 0x4a2a20 0x4a2b10 0x4a2d18 0x4a2f48 0x4c73d8 0x70a6e0 0x4a1f10
// @compiler xdk6130
// @flags /TP /O2
// @address 0x19f8a0 ?global_preferences_set_last_game_setup@@YAXPBUs_gui_game_setup_storage@@@Z
// @address 0x19f3b0 ??0c_thread_globals_lock@@QAA@XZ
// @address 0x19f408 ??1c_thread_globals_lock@@QAA@XZ

#include "main/levels_map_datum_types.h"
#include "networking/online/online_cached_user.h"
#include "saved_games/saved_film_description.h"
#include "interface/gui_network_session_advertisement_types.h"
#include "interface/gui_pregame_setup_dependencies.h"
#include "interface/hopper_information_types.h"
#include "multithreading/thread_tls.h"
#include <stddef.h>
#include "networking/session/network_session_interface_types.h"
#include "game/game_types.h"
#include "shell/shell.h"
#include "game/player_types.h"
#include "interface/c_deletable.h"
#include "memory/static_arrays.h"
#include "interface/user_interface_messages.h"
#include "cseries/bit_test.h"
#include <string.h>
#include <stdlib.h>
#include "interface/gui_screens/scoreboard/gui_screen_scoreboard.h"
#include "interface/gui_screens/pregame_lobby/gui_screen_pregame_lobby.h"
#include "interface/user_interface.h"
#include "interface/user_interface_memory.h"
#include "interface/user_interface_session.h"
#include "networking/session/network_game_start_mode_types.h"
#include "networking/logic/network_session_interface.h"
#include "text/unicode.h"
#include "game/game_engine_variant.h"
#include "interface/gui_pregame_setup_manager.h"
#include "interface/user_interface_matchmaking.h"
#include "networking/logic/network_life_cycle.h"
#include "networking/online/online_xenon.h"
#include "interface/user_interface_networking.h"
#include "main/game_preferences.h"
#include "saved_games/saved_film.h"
#include "saved_games/saved_game_files.h"
#include "game/map_variant.h"
#include "saved_games/game_state.h"
#include "interface/user_interface_error_manager.h"
#include "networking/session/network_session_class.h"
#include "multithreading/threads.h"
#include "interface/user_interface_controller.h"
#include "interface/controller_inline.h"
#include "multithreading/synchronized_value.h"
#include "memory/data_types.h"
#include "interface/user_interface_sound_effect_types.h"
#include "interface/c_gui_screen_widget.h"
#include "interface/gui_game_mode_types.h"
#include "interface/user_interface_sound.h"
#include "networking/logic/network_session_game_setup.h"
#include "interface/user_interface_session_game_setup.h"
#include "interface/gui_screen_pregame_lobby_mapeditor_types.h"
#include "main/thread_globals_lock.h"
#include "main/game_preferences_types.h"
#include "main/game_preferences_defaults.h"
#include "main/thread_globals_lock_inlines.h"
#include "interface/gui_selected_items.h"
#include "interface/gui_screens/game_browser/gui_game_browser.h"
#include "main/levels_globals.h"
#include "interface/gui_selected_items_difficulty.h"
#include "interface/gui_selected_items_map.h"
#include "interface/gui_selected_items_level.h"
#include "interface/gui_selected_items_hopper.h"
#include "interface/gui_selected_items_network_mode.h"
#include <new>
#include "interface/user_interface_window_manager.h"
#include "interface/gui_pregame_setup_manager_types.h"
#include "interface/controller_types.h"
#include "game/game_options.h"
#include "interface/gui_selected_items_game_variant.h"
#include "interface/gui_selected_items_saved_film.h"


struct s_saved_film_description;

class c_gui_selected_item;

struct s_user_interface_session_privacy_request;

struct s_map_variant_validation;


struct s_pregame_stored_matchmaking_setup { unsigned short valid; unsigned short hopper_identifier; };

union u_options_buffer {
    char options_storage[sizeof(game_options)];
    __forceinline game_options &options() { return *(game_options *)options_storage; }
    char map_variant_space[63504];
};

struct s_pregame_setup_work {
    char map_name[256];                                      // +0
    union {
        char variant_storage[sizeof(c_game_variant)];          // +256
        levels_map_datum level;
    };
    __forceinline c_game_variant &variant() { return *(c_game_variant *)variant_storage; }
    u_options_buffer options_buffer;                         // +1120
    int request_mode;                                        // +64624
    u_pregame_setup_data request_setup;                      // +64628
};

struct s_pregame_stored_campaign_setup
{
    long valid;
    long campaign_id;
    long map_id;
    long difficulty;
    // Preferences serialize all seven words; the final three are copied intact and never interpreted here.
    long pad10;
    long pad14;
    long pad18;
};

struct s_pregame_stored_setup_layout
{
    s_pregame_stored_campaign_setup campaign;
    s_pregame_stored_matchmaking_setup matchmaking;
    s_custom_setup_data custom;
    s_mapeditor_setup_data mapeditor;
    s_theater_setup_data theater;
};

class c_gui_pregame_selected_item_calls
{
public:
    virtual ~c_gui_pregame_selected_item_calls();
    virtual long get_location() const;
    virtual void get_name(char *buffer, long buffer_size) const;
    virtual const wchar_t *get_description() const;
};

struct s_pregame_setup_item_source
{
    bool valid;
    long location;
    s_player_identifier owner;
    char name[256];
};

struct s_pregame_setup_map
{
    char variant[0xe090];
    s_pregame_setup_item_source source;
};

struct s_pregame_setup_custom
{
    long valid;
    c_game_variant variant;
    s_pregame_setup_item_source variant_source;
    s_pregame_setup_map map;
};

struct s_pregame_setup_mapeditor
{
    long valid;
    long location;
    s_pregame_setup_map map;
};

struct s_pregame_setup_theater
{
    long valid;
    long location;
    s_player_identifier owner;
    long film_category;
    long campaign_id;
    long map_id;
    long difficulty;
    char name[256];
    wchar_t description[128];
    long controller_index;
    long length_seconds;
    game_options options;
    long game_engine_index;
    long game_variant_index;
};

struct s_pregame_setup_storage
{
    s_pregame_stored_campaign_setup campaign;
    s_pregame_stored_matchmaking_setup matchmaking;
    s_pregame_setup_custom custom;
    s_pregame_setup_mapeditor mapeditor;
    s_pregame_setup_theater theater;
};

struct s_pregame_setup_selection
{
    long mode;
    union
    {
        s_pregame_stored_campaign_setup campaign;
        s_pregame_stored_matchmaking_setup matchmaking;
        char custom_storage[sizeof(s_pregame_setup_custom)];
        s_pregame_setup_mapeditor mapeditor;
        char theater_storage[sizeof(s_pregame_setup_theater)];
    };
    __forceinline s_pregame_setup_custom &custom() { return *(s_pregame_setup_custom *)custom_storage; }
    __forceinline s_pregame_setup_theater &theater() { return *(s_pregame_setup_theater *)theater_storage; }
};

typedef char theater_setup_size[(sizeof(s_theater_setup_data) == 64064) ? 1 : -1];

typedef char theater_setup_film_offset[(offsetof(s_theater_setup_data, film) == 0x10) ? 1 : -1];

typedef char pregame_campaign_setup_size[(sizeof(s_pregame_stored_campaign_setup) == 0x1c) ? 1 : -1];

typedef char pregame_storage_custom_offset[(offsetof(s_pregame_stored_setup_layout, custom) == 0x20) ? 1 : -1];

typedef char pregame_selected_item_type_offset[(sizeof(void *) != 4 || ((offsetof(c_gui_simple_selected_item, m_type) == 0x8))) ? 1 : -1];

typedef char pregame_selected_item_metadata_offset[(sizeof(void *) != 4 || ((offsetof(c_gui_simple_selected_item, m_metadata) == 0x18))) ? 1 : -1];

typedef char pregame_selected_item_value_offset[(sizeof(void *) != 4 || ((offsetof(c_gui_level_selected_item, m_sort_key) == 0x190))) ? 1 : -1];

typedef char pregame_selected_item_map_offset[(sizeof(void *) != 4 || ((offsetof(c_gui_level_selected_item, m_map_id) == 0x194))) ? 1 : -1];

typedef char pregame_selected_item_insertion_offset[(sizeof(void *) != 4 || ((offsetof(c_gui_level_selected_item, m_extra) == 0x19a))) ? 1 : -1];

typedef char pregame_selected_item_choice_offset[(sizeof(void *) != 4 || ((offsetof(c_gui_network_mode_selected_item, m_choice) == 0x194))) ? 1 : -1];

typedef char pregame_selected_item_hopper_offset[
    (sizeof(void *) != 4 || ((offsetof(c_gui_hopper_selected_item, m_information) + offsetof(s_hopper_information, id) == 0x1d0))) ? 1 : -1];

typedef char pregame_selected_item_film_category_offset[
    (sizeof(void *) != 4 || ((offsetof(c_gui_saved_film_selected_item, m_saved_film_category) == 0x2b0))) ? 1 : -1];

typedef char pregame_campaign_id_offset[(offsetof(s_campaign_setup_data, campaign_id) == 4) ? 1 : -1];

typedef char pregame_campaign_size[(sizeof(s_campaign_setup_data) == 0x10) ? 1 : -1];

typedef char pregame_matchmaking_hopper_offset[(offsetof(s_matchmaking_setup_data, hopper) == 2) ? 1 : -1];

typedef char pregame_matchmaking_size[(sizeof(s_matchmaking_setup_data) == 4) ? 1 : -1];

typedef char pregame_custom_variant_offset[(offsetof(s_custom_setup_data, variant) == 4) ? 1 : -1];

typedef char pregame_custom_selection_offset[(offsetof(s_custom_setup_data, selection) == 0x26c) ? 1 : -1];

typedef char pregame_custom_selection1_offset[(offsetof(s_custom_setup_data, selection1) == 0xe40c) ? 1 : -1];

typedef char pregame_custom_size[(sizeof(s_custom_setup_data) == 58648) ? 1 : -1];

typedef char pregame_mapeditor_selection_offset[(offsetof(s_mapeditor_setup_data, selection) == 0xe09c) ? 1 : -1];

typedef char pregame_mapeditor_size[(sizeof(s_mapeditor_setup_data) == 57768) ? 1 : -1];

typedef char pregame_manager_size[(sizeof(c_gui_pregame_setup_manager) == 0xfa54) ? 1 : -1];

void global_preferences_set_last_game_setup(const s_gui_game_setup_storage *last_game_setup) {
 if(!global_preferences_should_use_defaults()) {
  c_thread_globals_lock lock;
  s_global_preferences *preferences=thread_global_preferences;
  memcpy(&preferences->primary_file.data.mode_settings,last_game_setup,sizeof(*last_game_setup));
  preferences->dirty=true;
 }
}

// @address 0x4a1bb0 ?get_selected_item_title@c_gui_pregame_setup_manager@@SA_NPA_WJW4e_gui_selected_item_type@@@Z
bool c_gui_pregame_setup_manager::get_selected_item_title(wchar_t *title, long title_length, e_gui_selected_item_type type)
{
    c_static_wchar_string<1024> string;
    e_campaign_id campaign_id;
    e_map_id map_id;
    s_hopper_information hopper_information;
    levels_map_datum level;
    switch (type) {
    case _gui_selected_item_map: {
        const c_map_variant *map_variant =
            (const c_map_variant *)user_interface_game_settings_try_and_get_map_variant();
        if (map_variant != 0 && wcslen(map_variant->m_metadata.name) != 0)
            string.set(map_variant->m_metadata.name);
        else if (network_life_cycle_session_get_map(&campaign_id, &map_id)
                 && levels_try_and_get_datum(g_level_globals.multiplayer_levels, map_id, (s_datum *)&level))
            string.set(level.display_name);
        break;
    }
    case _gui_selected_item_campaign: {
        if (network_life_cycle_session_get_map(&campaign_id, &map_id)
            && levels_try_and_get_datum(g_level_globals.map_levels, map_id, (s_datum *)&level))
            string.set(level.display_name);
        break;
    }
    case _gui_selected_item_difficulty: {
        c_static_wchar_string<1024> title_string;
        c_static_wchar_string<1024> description;
        if (c_gui_difficulty_selected_item::find_title_and_description(
                user_interface_game_settings_get_campaign_difficulty(), &title_string, &description))
            string.set(title_string.m_string);
        break;
    }
    case _gui_selected_item_game_variant: {
        const c_game_variant *game_variant = user_interface_game_settings_get_game_variant();
        if (game_variant != 0)
            string.set(game_variant->get_name());
        break;
    }
    case _gui_selected_item_hopper: {
        unsigned short hopper = network_squad_session_get_hopper();
        if (hopper != 0xffff && ::user_interface_matchmaking_get_hopper_information(hopper, &hopper_information))
            string.set(reinterpret_cast<const wchar_t *>(hopper_information.name));
        break;
    }
    case _gui_selected_item_advertisement: {
        c_static_wchar_string<1024> advertisement_string;
        int string_id;
        switch (user_interface_networking_get_session_advertisement()) {
        case _network_advertise_xbox_live_open: string_id = _pregame_lobby_string_network_mode_xbox_live_advertise_anyone; break;
        case _network_advertise_xbox_live_friends_only: string_id = _pregame_lobby_string_network_mode_xbox_live_advertise_friends; break;
        case _network_advertise_xbox_live_invite_only: string_id = _pregame_lobby_string_network_mode_xbox_live_advertise_invite_only; break;
        case _network_advertise_system_link: string_id = _pregame_lobby_string_network_mode_system_link_advertise; break;
        case _network_advertise_offline: string_id = _pregame_lobby_string_network_mode_offline; break;
        // match: 0x4a1bb0: unreachable hint omits dead branch check
        default: __assume(0);
        }
        if (user_interface_global_string_get(string_id, (unsigned short *)advertisement_string.m_string))
            string.set(advertisement_string.get_string());
        break;
    }
    case _gui_selected_item_film: {
        s_saved_film_description *film = ((s_saved_film_description *)::user_interface_game_settings_get_film());
        if (film != 0 && film->film_type != 0 && film->film_type != 6)
            string.set(film->name);
        break;
    }
    default:
        // match: 0x4a1bb0: unreachable hint omits dead branch check
        __assume(0);
    }
    string.copy_to(title, title_length);
    return true;
}

// @address 0x4a1e00 ?get_selected_item_category_id@c_gui_pregame_setup_manager@@QAAJW4e_gui_selected_item_type@@@Z
long c_gui_pregame_setup_manager::get_selected_item_category_id(e_gui_selected_item_type type)
{
    long result = -1;
    s_hopper_information hopper_information;
    if (m_item_locked[type] == 0) {
        switch (type) {
        case _gui_selected_item_map: {
            e_campaign_id campaign;
            e_map_id map;
            if (network_life_cycle_session_get_map(&campaign, &map)) result = map;
            break;
        }
        case _gui_selected_item_film:
            break;
        case _gui_selected_item_campaign: {
            e_map_id map;
            e_campaign_id campaign;
            if (network_life_cycle_session_get_map(&campaign, &map)) result = campaign;
            break;
        }
        case _gui_selected_item_difficulty:
            result = user_interface_game_settings_get_campaign_difficulty();
            break;
        case _gui_selected_item_game_variant: {
            const c_game_variant *variant = user_interface_game_settings_get_game_variant();
            if (variant != 0) result = variant->get_game_engine_index();
            break;
        }
        case _gui_selected_item_hopper: {
            unsigned short hopper;
            result = -1;
            hopper = network_squad_session_get_hopper();
            if (hopper != 0xffff) {
                if (::user_interface_matchmaking_get_hopper_information(hopper, &hopper_information))
                    result = hopper_information.value;
            }
            break;
        }
        case _gui_selected_item_advertisement: {
            e_gui_network_session_advertisement_mode mode = user_interface_networking_get_session_advertisement();
            switch ((unsigned int)mode) {
            case _network_advertise_xbox_live_open:
            case _network_advertise_xbox_live_friends_only:
            case _network_advertise_xbox_live_invite_only: result = 0; break;
            case _network_advertise_system_link: result = 1; break;
            case _network_advertise_offline: result = 2; break;
            // match: 0x4a1e00: unreachable hint omits dead branch check
            default: __assume(0);
            }
            break;
        }
        default:
            // match: 0x4a1e00: unreachable hint omits dead branch check
            __assume(0);
        }
    }
    return result;
}

// @address 0x4a2a20 ?change_advertisement_mode@c_gui_pregame_setup_manager@@AAAXW4e_gui_network_session_advertisement_mode@@W4e_network_session_class@@1J@Z
void c_gui_pregame_setup_manager::change_advertisement_mode(e_gui_network_session_advertisement_mode advertisement_mode,
    e_network_session_class session_class, e_network_session_class session_class2, long parent_screen_index)
{
    if (session_class != session_class2 && network_squad_session_get_peer_count() > 1) {
        void *memory = user_interface_malloc_tracked(sizeof(c_load_dialog_screen_message), 0, 0);
        if (memory == 0) return;
        c_load_dialog_screen_message *message = new (memory) c_load_dialog_screen_message(k_any_controller, _console_window, 0x1000c, 0x30006, 0x10022);
        if (message == 0) return;
        message->set_dispose_on_success_screen_index(parent_screen_index);
        m_advertisement_mode = advertisement_mode;
        ::user_interface_messaging_post((c_message const *)(c_deletable *)message);
        return;
    }
    user_interface_networking_set_session_advertisement(advertisement_mode);
    if (advertisement_mode == 0 || advertisement_mode == 1 || advertisement_mode == 2)
        global_preferences_set_xbox_live_private_privacy_setting(advertisement_mode);
    if (parent_screen_index != -1) {
        c_gui_screen_widget *screen = g_window_manager.get_screen_by_screen_index(parent_screen_index);
        if (screen) {
            screen->transition_out_with_transition_type(0, _screen_transition_normal);
        }
    }
}

static __forceinline bool gui_pregame_setup_manager_get_local_user_identifier(long index, s_player_identifier *key, bool any_state)
{
    s_player_slot *slots = g_network_session_interface_local_users;
    bool result = any_state ? slots[index].state != 0 : slots[index].state == 1;
    if (result)
        memcpy(key, slots[index].key, sizeof(slots[index].key));
    return result;
}

static __forceinline bool gui_pregame_setup_manager_is_local_user_online_enabled(long index)
{
    return g_network_session_interface_local_users[index].online_enabled;
}

static __forceinline bool gui_pregame_setup_manager_is_everyone_online_enabled()
{
    bool result = true;
    for (long index = 0; index < 4; index++)
    {
        s_player_identifier identifier;
        if (gui_pregame_setup_manager_get_local_user_identifier(index, &identifier, false) &&
            !gui_pregame_setup_manager_is_local_user_online_enabled(index))
        {
            result = false;
            break;
        }
    }
    return result;
}

// @address 0x4a2b10 ?set_default_privacy@c_gui_pregame_setup_manager@@CAXW4e_gui_game_mode@@_N@Z
void c_gui_pregame_setup_manager::set_default_privacy(e_gui_game_mode, bool is_new_session)
{
    bool everyone_online = gui_pregame_setup_manager_is_everyone_online_enabled() && online_local_xuid_is_online_enabled();
    int mode = 4;
    if (is_new_session) {
        if (everyone_online) mode = saved_film_play();
    } else {
        e_network_session_class session_class = network_squad_session_get_session_class();
        if (session_class == _network_session_class_xbox_live) mode = saved_film_play();
        else if (session_class == _network_session_class_system_link) mode = 3;
    }
    if (user_interface_networking_get_session_advertisement() != mode) user_interface_networking_set_session_advertisement(mode);
}

static __forceinline bool pregame_setup_controller_in_use(const c_controller_interface *controller)
{
    return ((const c_controller_interface *)controller)->is_signed_in_to_machine() || _rotl(controller->flags, 30) & 1;
}
// @address 0x19a840 ??0game_options@@QAA@XZ
// @address 0x19c310 ?game_state_read_game_options_from_persistent_storage_blocking@@YA_NW4e_controller_index@@PAUgame_options@@@Z
// XDK C++03 unions cannot contain game_options or c_game_variant constructors.
// These overlapping buffers are filled by the storage and setup APIs.

#ifdef HALO3_64BIT
extern "C" bool saved_game_files_read_game_options_for_player_count(
    e_controller_index controller_index, long player_count, game_options *options);
#endif

// @address 0x4a2f48 gui_pregame_setup_manager_apply_pending_setup
extern "C" void gui_pregame_setup_manager_apply_pending_setup(c_gui_pregame_setup_manager *manager, s_pregame_setup_params *setup)
{
    unsigned char *item_locked = manager->m_item_locked;
    s_pregame_setup_work work_data;
    char (&map_name)[256] = work_data.map_name;
    c_game_variant &variant = work_data.variant();
    levels_map_datum &level = work_data.level;
    u_options_buffer &options_buffer = work_data.options_buffer;
    int &request_mode = work_data.request_mode;
    u_pregame_setup_data &request_setup = work_data.request_setup;
    for (unsigned int remaining = 8; remaining != 0; --remaining)
        *item_locked++ = false;
    if (!network_squad_session_set_ui_game_mode((e_gui_game_mode)setup->mode))
        return;
    request_mode = setup->mode;
    switch (request_mode) {
    case _gui_game_mode_campaign: {
        s_campaign_setup_data *campaign = &setup->data.campaign;
        long campaign_id = 1;
        long map_id = 0xbbd;
        long difficulty = -1;
        long primary_skulls = 0;
        long secondary_skulls = 0;
        long metagame_scoring = 0;
        unsigned short insertion_point = 0;
        bool setup_valid = false;
#ifdef HALO3_64BIT
        long target_players = 0;
        if (user_interface_squad_exists()) {
            long total = user_interface_squad_get_player_count();
            if (total >= 1 && total <= 4 && user_interface_squad_get_local_player_count() == total)
                target_players = total;
        } else {
            for (long i = 0; i < 4; ++i) {
                if (pregame_setup_controller_in_use(controller_get((e_controller_index)i)))
                    ++target_players;
            }
        }
        if (target_players >= 1 && target_players <= 4) {
            for (long controller_index = 0; controller_index < 4; controller_index++) {
                c_controller_interface *controller = controller_get((e_controller_index)controller_index);
                if (pregame_setup_controller_in_use(controller)) {
                    new (&options_buffer) game_options;
                    if (saved_game_files_read_game_options_for_player_count(
                            (e_controller_index)controller_index, target_players, (game_options *)&options_buffer))
                    {
                        setup_valid = true;
                        campaign_id = options_buffer.options().campaign_id;
                        map_id = options_buffer.options().map_id;
                        difficulty = options_buffer.options().campaign_difficulty;
                        primary_skulls = options_buffer.options().primary_skulls;
                        secondary_skulls = options_buffer.options().secondary_skulls;
                        metagame_scoring = options_buffer.options().campaign_metagame_scoring;
                        insertion_point = options_buffer.options().campaign_insertion_point;
                        break;
                    }
                }
            }
        }
#else
        if (user_interface_squad_get_player_count() == 1) {
            for (long controller_index = 0; controller_index < 4; controller_index++) {
                c_controller_interface *controller = controller_get((e_controller_index)controller_index);
                if (pregame_setup_controller_in_use(controller)) {
                    new (&options_buffer) game_options;
                    if (game_state_read_game_options_from_persistent_storage_blocking((e_controller_index)controller_index, (game_options *)&options_buffer)) {
                        setup_valid = true;
                        campaign_id = options_buffer.options().campaign_id;
                        map_id = options_buffer.options().map_id;
                        difficulty = options_buffer.options().campaign_difficulty;
                        primary_skulls = options_buffer.options().primary_skulls;
                        secondary_skulls = options_buffer.options().secondary_skulls;
                        insertion_point = options_buffer.options().campaign_insertion_point;
                    }
                    else {
                        saved_game_files_delete_personal_storage(controller_index);
                    }
                    break;
                }
            }
        }
#endif
        if (!setup_valid) {
            if (!(campaign->valid && campaign->campaign_id >= 0
                && campaign->difficulty >= 0 && campaign->difficulty < 4 && campaign->map_id >= 0)) {
                request_setup.campaign.campaign_id = _campaign_id_pre_game_fallback;
                request_setup.campaign.map_id = _map_id_pre_game_fallback;
                request_setup.campaign.difficulty = _campaign_difficulty_level_normal;
                request_setup.campaign.valid = 1;
                global_preferences_set_mode_setup(&request_mode);
                campaign = &request_setup.campaign;
            }
            difficulty = campaign->difficulty;
            if (campaign->campaign_id == 1 && campaign->map_id == 0xbbd) {
                campaign_id = 1;
                map_id = 0xbbd;
            }
            else if (levels_try_and_get_datum(g_level_globals.map_levels, campaign->map_id, (s_datum *)&level)
                && (bool)((level.flags >> 2) & 1)) {
                for (long controller_index = 0; controller_index < 4; controller_index++) {
                    c_controller_interface *controller = controller_get((e_controller_index)controller_index);
                    if (controller->in_use()) {
                        short insertion = levels_get_campaign_map_index(campaign->campaign_id, campaign->map_id);
                        bool usable = false;
                        if (insertion >= 0 && insertion < 32)
                            usable = (((1 << (insertion & 31)) & ((unsigned long *)controller)[(insertion >> 5) + 41]) != 0);
                        if (usable) {
                            campaign_id = campaign->campaign_id;
                            map_id = campaign->map_id;
                            break;
                        }
                    }
                }
            }
        }
        user_interface_create_new_squad(_network_session_class_offline);
        bool map_set = false;
        levels_get_map_scenario_path(campaign_id, map_id, map_name, 256);
        if (map_name[0])
            map_set = network_squad_session_set_map((e_campaign_id)campaign_id, (e_map_id)map_id, map_name);
        if (!map_set) {
            levels_get_map_scenario_path(_campaign_id_pre_game_fallback, _map_id_pre_game_fallback, map_name, 256);
            if (map_name[0])
                network_squad_session_set_map(_campaign_id_pre_game_fallback, _map_id_pre_game_fallback, map_name);
        }
        network_squad_session_set_campaign_difficulty((e_campaign_difficulty_level)difficulty);
        network_squad_session_set_campaign_metagame_scoring(metagame_scoring);
        network_squad_session_set_primary_skulls(primary_skulls);
        network_squad_session_set_secondary_skulls(secondary_skulls);
        network_squad_session_set_campaign_insertion_point(insertion_point);
        break;
    }
    case _gui_game_mode_multiplayer: {
        s_custom_setup_data *custom = &setup->data.custom();
        bool valid = true;
        valid = valid && custom->valid;
        if (valid && custom->selection_valid)
            valid = valid && (custom->selection == -1 || (custom->selection >= 0 && custom->selection < 3));
        valid = valid && game_engine_variant_is_valid(&custom->variant);
        if (valid && custom->selection1_valid)
            valid = valid && (custom->selection1 == -1 || (custom->selection1 >= 0 && custom->selection1 < 3));
        valid = valid && map_variant_is_valid((const s_map_variant_validation *)custom->map_variant_data);
        if (!valid) {
            if (c_gui_game_variant_selected_item::get_game_engine_item_count(_game_engine_slayer) > 0)
                game_engine_tag_defined_variant_get_built_in_variant(_game_engine_slayer, 0, &request_setup.custom().variant);
            else
                game_engine_variant_initialize_default(&request_setup.custom().variant, _game_engine_slayer);
            memset(&request_setup.custom().selection_valid, 0, sizeof(s_pregame_setup_item_source));
            request_setup.custom().selection_valid = 1;
            request_setup.custom().selection = 0;
            long map = levels_get_default_multiplayer_map_id();

            ((c_map_variant *)request_setup.custom().map_variant_data)->create_default((e_map_id)map);
            memset(&request_setup.custom().selection1_valid, 0, sizeof(s_pregame_setup_item_source));
            request_setup.custom().selection1_valid = 1;
            request_setup.custom().selection1 = -1;
            request_setup.custom().valid = 1;
            global_preferences_set_mode_setup(&request_mode);
            custom = &request_setup.custom();
        }
        user_interface_create_new_squad(_network_session_class_system_link);
        if (!user_interface_squad_set_game_variant_with_verify(&custom->variant)) {
            variant.engine_type = 0;
            memset(variant.variant_storage, 0, sizeof(variant.variant_storage));
            memset(variant.variant_storage, 0, sizeof(variant.variant_storage));
            if (c_gui_game_variant_selected_item::get_game_engine_item_count(_game_engine_slayer) > 0)
                game_engine_tag_defined_variant_get_built_in_variant(_game_engine_slayer, 0, &variant);
            else
                game_engine_variant_initialize_default(&variant, _game_engine_slayer);
            user_interface_squad_set_game_variant_with_verify(&variant);
        }
        if (!user_interface_squad_set_map_variant_with_verify((s_user_interface_session_privacy_request *)custom->map_variant_data)) {
            new (options_buffer.map_variant_space) c_map_variant();
            long map = levels_get_default_multiplayer_map_id();
            ((c_map_variant *)options_buffer.map_variant_space)->create_default((e_map_id)map);
            user_interface_squad_set_map_variant_with_verify((s_user_interface_session_privacy_request *)options_buffer.map_variant_space);
        }
        break;
    }
    case _gui_game_mode_matchmaking: {
        s_matchmaking_setup_data *matchmaking = &setup->data.matchmaking;
        if (!matchmaking->valid) {
            request_setup.matchmaking.valid = 1;
            request_setup.matchmaking.hopper = 0xFFFF;
            global_preferences_set_mode_setup(&request_mode);
            matchmaking = &request_setup.matchmaking;
        }
        long controller = 0;
        long next;
        const wchar_t *name;
        do {
            name = user_interface_controller_get_player_profile_name((e_controller_index)controller);
            if (name && wcslen(name)) {
                user_interface_session_set_name(name, name);
                break;
            }
            switch (controller + 1) {
            case 0: next = 0; break;
            case 1: next = 1; break;
            case 2: next = 2; break;
            case 3: next = 3; break;
            default: next = -1; break;
            }
            controller = next;
        } while (next != -1);
        ::user_interface_squad_set_start_mode(_network_game_start_mode_matchmaking);
        user_interface_matchmaking_set_hopper((short)matchmaking->hopper);
        break;
    }
    case _gui_game_mode_mapeditor: {
        s_mapeditor_setup_data *editor = &setup->data.mapeditor;
        bool valid = true;
        valid = valid && editor->valid;
        if (valid && editor->selection_valid)
            valid = valid && (editor->selection == -1 || (editor->selection >= 0 && editor->selection < 3));
        valid = valid && map_variant_is_valid((const s_map_variant_validation *)editor->map_variant_data);
        if (!valid) {
            long map = levels_get_default_multiplayer_map_id();
            ((c_map_variant *)request_setup.mapeditor.map_variant_data)->create_default((e_map_id)map);
            memset(&request_setup.mapeditor.selection_valid, 0, sizeof(s_pregame_setup_item_source));
            request_setup.mapeditor.selection_valid = 1;
            request_setup.mapeditor.selection = -1;
            request_setup.mapeditor.valid = 1;
            global_preferences_set_mode_setup(&request_mode);
            editor = &request_setup.mapeditor;
        }
        variant.engine_type = 0;
        memset(variant.variant_storage, 0, sizeof(variant.variant_storage));
        memset(variant.variant_storage, 0, sizeof(variant.variant_storage));
        user_interface_create_new_squad(_network_session_class_system_link);
        if (!user_interface_squad_set_map_variant_with_verify((s_user_interface_session_privacy_request *)editor->map_variant_data)) {
            new (options_buffer.map_variant_space) c_map_variant();
            long map = levels_get_default_multiplayer_map_id();
            ((c_map_variant *)options_buffer.map_variant_space)->create_default((e_map_id)map);
            user_interface_squad_set_map_variant_with_verify((s_user_interface_session_privacy_request *)options_buffer.map_variant_space);
        }
        if (c_gui_game_variant_selected_item::get_game_engine_item_count(_game_engine_sandbox) > 0) {
            game_engine_tag_defined_variant_get_built_in_variant(_game_engine_sandbox, 0, &variant);
            user_interface_squad_set_game_variant_with_verify(&variant);
        }
        else {
            game_engine_variant_initialize_default(&variant, _game_engine_sandbox);
            user_interface_squad_set_game_variant_with_verify(&variant);
        }
        break;
    }
    case _gui_game_mode_theater: {
        memset(&request_setup.theater.film, 0, sizeof(request_setup.theater.film));
        request_setup.theater.film.film_type = 0;
        request_setup.theater.film.campaign_id = -1;
        request_setup.theater.film.map_id = -1;
        request_setup.theater.film.difficulty = _campaign_difficulty_level_normal;
        request_setup.theater.film.controller_index = -1;
        request_setup.theater.valid = 1;
        global_preferences_set_mode_setup(&request_mode);
        long controller = 0;
        long next;
        const wchar_t *name;
        do {
            name = user_interface_controller_get_player_profile_name((e_controller_index)controller);
            if (name && wcslen(name)) {
                user_interface_session_set_name(name, name);
                break;
            }
            switch (controller + 1) {
            case 0: next = 0; break;
            case 1: next = 1; break;
            case 2: next = 2; break;
            case 3: next = 3; break;
            default: next = -1; break;
            }
            controller = next;
        } while (next != -1);
        ::user_interface_squad_set_start_mode(_network_game_start_mode_film);
        e_network_session_class session_class = network_squad_session_get_session_class();
        user_interface_squad_set_film(&request_setup.theater.film.film_type, session_class == _network_session_class_xbox_live);
        break;
    }
    // match: 0x4a2f48: unreachable hint omits dead branch check
    default: __assume(0);
    }
    for (long controller = 0; controller < 4; controller++)
        user_interface_controller_update_network_properties((e_controller_index)controller);
}

// @address 0x4c73d8 gui_pregame_setup_manager_get_film_map_title
extern "C" bool gui_pregame_setup_manager_get_film_map_title(void *context, wchar_t *output, long capacity)
{
    bool result = false;
    s_saved_film_description *film = ((s_saved_film_description *)::user_interface_game_settings_get_film());
    if (film)
    {
        if (film->film_type != 0 && film->film_type != 6)
        {
            result = c_gui_pregame_setup_manager::get_selected_item_title(output, capacity, _gui_selected_item_map);
        }
    }
    return result;
}

// @address 0x8a3c0 ?ustrnzcpy@@YAPA_WPA_WPB_WJ@Z
// @address 0x4eb608 ?find_title_and_description@c_gui_difficulty_selected_item@@SA_NW4e_campaign_difficulty_level@@PAV?$c_static_wchar_string@$0EAA@@@1@Z
// @address 0x3b4e98 ?network_life_cycle_session_get_map@@YA_NPAW4e_campaign_id@@PAW4e_map_id@@@Z
// @address 0x495440 ?network_squad_session_get_hopper@@YAGXZ
// @address 0x46d740 ?user_interface_game_settings_get_campaign_difficulty@@YA?AW4e_campaign_difficulty_level@@XZ
// @address 0x3b5178 ?user_interface_game_settings_get_game_variant@@YAPBVc_game_variant@@XZ
// @address 0x3b52f8 user_interface_game_settings_try_and_get_map_variant
// @address 0x1e7238 levels_try_and_get_datum
// @address 0x1bfeb8 ?set@?$c_static_wchar_string@$0EAA@@@QAAXPB_W@Z
// @address 0x4835a0 user_interface_global_string_get
// @address 0x4795f8 ?user_interface_networking_get_session_advertisement@@YA?AW4e_gui_network_session_advertisement_mode@@XZ

// @address 0x4790c8 gui_pregame_setup_manager_initialize_squad_for_mode
bool gui_pregame_setup_manager_initialize_squad_for_mode(long game_mode)
{
    bool success = true;
    bool is_new_session = false;
    if (!user_interface_squad_exists())
    {
        success = user_interface_squad_restart(
            gui_pregame_setup_manager_is_everyone_online_enabled() && online_local_xuid_is_online_enabled() ? 2 : 0);
        is_new_session = true;
    }
    if (success)
    {
        success = false;
        if (network_squad_session_set_ui_game_mode((e_gui_game_mode)game_mode))
        {
            global_preferences_get_mode_setup(&g_pregame_setup_manager, game_mode);
            if (user_interface_squad_exists())
            {
                c_gui_pregame_setup_manager::set_default_privacy((e_gui_game_mode)game_mode, is_new_session);
                success = true;
            }
        }
        g_pregame_setup_manager.m_pending_setup_valid = false;
    }
    return success;
}

// @address 0x4a2d18 gui_pregame_setup_manager_save_current_setup
extern "C" void gui_pregame_setup_manager_save_current_setup(long mode, const void *stored_setup, s_pregame_setup_params *storage)
{
    const s_pregame_stored_setup_layout *source = static_cast<const s_pregame_stored_setup_layout *>(stored_setup);
    switch (mode)
    {
    case _gui_game_mode_campaign:
    {
        s_campaign_setup_data *campaign = &storage->data.campaign;
        network_life_cycle_session_get_map((e_campaign_id *)&campaign->campaign_id, (e_map_id *)&campaign->map_id);
        campaign->difficulty = user_interface_game_settings_get_campaign_difficulty();
        campaign->valid = 1;
        break;
    }
    case _gui_game_mode_multiplayer:
    {
        s_custom_setup_data *custom = &storage->data.custom();
        const c_game_variant *game_variant = user_interface_game_settings_get_game_variant();
        const c_map_variant *map_variant = (const c_map_variant *)user_interface_game_settings_try_and_get_map_variant();
        custom->selection_valid = 0;
        if (game_variant != 0)
        {
            custom->variant.copy_from_and_validate(game_variant);
            if (source->custom.variant.is_equal_to(game_variant))
                memcpy(&custom->selection_valid, &source->custom.selection_valid, sizeof(s_pregame_setup_item_source));
        }
        custom->selection1_valid = 0;
        if (map_variant != 0)
        {
            memcpy(custom->map_variant_data, map_variant, sizeof(*map_variant));
            if (map_variant_test_equal(source->custom.map_variant_data, map_variant))
                memcpy(&custom->selection1_valid, &source->custom.selection1_valid, sizeof(s_pregame_setup_item_source));
        }
        else
        {
            ((c_map_variant *)custom->map_variant_data)->create_default((e_map_id)levels_get_default_multiplayer_map_id());
        }
        custom->valid = 1;
        break;
    }
    case _gui_game_mode_matchmaking:
    {
        s_matchmaking_setup_data *matchmaking = &storage->data.matchmaking;
        matchmaking->hopper = network_squad_session_get_hopper();
        matchmaking->valid = 1;
        break;
    }
    case _gui_game_mode_mapeditor:
    {
        s_mapeditor_setup_data *editor = &storage->data.mapeditor;
        const c_map_variant *map_variant = (const c_map_variant *)user_interface_game_settings_try_and_get_map_variant();
        editor->selection_valid = 0;
        if (map_variant != 0)
        {
            memcpy(editor->map_variant_data, map_variant, sizeof(*map_variant));
            if (map_variant_test_equal(source->mapeditor.map_variant_data, map_variant))
                memcpy(&editor->selection_valid, &source->mapeditor.selection_valid, sizeof(s_pregame_setup_item_source));
        }
        else
        {
            ((c_map_variant *)editor->map_variant_data)->create_default((e_map_id)levels_get_default_multiplayer_map_id());
        }
        editor->valid = 1;
        break;
    }
    case _gui_game_mode_theater:
    {
        s_theater_setup_data *theater = &storage->data.theater;
        const s_saved_film_description *film = ((s_saved_film_description *)::user_interface_game_settings_get_film());
        if (film != 0)
        {
            memcpy(&theater->film, film, sizeof(*film));
            theater->valid = 1;
        }
        break;
    }
    }
}

__forceinline e_gui_selected_item_type c_gui_selected_item::get_selection_type() const
{
    return m_type;
}


static __forceinline bool gui_pregame_setup_manager_any_online_user()
{
    bool result = false;
    for (long user_index = 0; user_index < 4; user_index++)
    {
        if (online_cached_user_is_silver_or_gold(user_index))
        {
            result = true;
            break;
        }
    }
    return result;
}


// @address 0x4a1f10 ?set_selected_item@c_gui_pregame_setup_manager@@QAAXW4e_controller_index@@PBVc_gui_selected_item@@_NJ@Z
void c_gui_pregame_setup_manager::set_selected_item(
    e_controller_index controller_index, const c_gui_selected_item *selected_item, bool locked, long parent_screen_index)
{
    const c_gui_simple_selected_item *selection = (const c_gui_simple_selected_item *)selected_item;
    const c_gui_pregame_selected_item_calls *calls = (const c_gui_pregame_selected_item_calls *)selected_item;
    const s_pregame_setup_storage *last_setup = (const s_pregame_setup_storage *)global_preferences_get_game_setup_storage();
    bool setup_changed = false;
    s_pregame_setup_selection setup;
    setup.mode = network_life_cycle_squad_session_get_ui_game_mode();
    switch (selected_item->get_selection_type())
    {
    case _gui_selected_item_campaign:
    {
        levels_map_datum level;
        if (levels_try_and_get_datum(g_level_globals.map_levels, ((const c_gui_level_selected_item *)selected_item)->m_map_id, (s_datum *)&level)
            && test_bit(level.flags, 2)
           
                && user_interface_squad_set_map(
                    (e_campaign_id)((const c_gui_level_selected_item *)selected_item)->m_sort_key,
                    (e_map_id)((const c_gui_level_selected_item *)selected_item)->m_map_id))
        {
            setup.campaign = last_setup->campaign;
            setup_changed = true;
            setup.campaign.campaign_id = ((const c_gui_level_selected_item *)selected_item)->m_sort_key;
            setup.campaign.map_id = ((const c_gui_level_selected_item *)selected_item)->m_map_id;
            m_item_locked[selected_item->get_selection_type()] = locked;
            network_squad_session_set_campaign_insertion_point(((const c_gui_level_selected_item *)selected_item)->m_extra);
        }
        else
        {
            user_interface_play_sound(_ui_sound_error, -1);
        }
        break;
    }
    case _gui_selected_item_difficulty:
        if (network_squad_session_set_campaign_difficulty((e_campaign_difficulty_level)((const c_gui_difficulty_selected_item *)selected_item)->m_difficulty))
        {
            setup.campaign = last_setup->campaign;
            setup_changed = true;
            setup.campaign.difficulty = ((const c_gui_difficulty_selected_item *)selected_item)->m_difficulty;
            m_item_locked[selected_item->get_selection_type()] = locked;
        }
        else
        {
            user_interface_play_sound(_ui_sound_error, -1);
        }
        break;
    case _gui_selected_item_map:
    {
        s_pregame_setup_map *map_setup;
        if (setup.mode == _gui_game_mode_multiplayer)
        {
            memcpy(&setup.custom(), &last_setup->custom, sizeof(setup.custom()));
            map_setup = &setup.custom().map;
        }
        else
        {
            memcpy(&setup.mapeditor, &last_setup->mapeditor, sizeof(setup.mapeditor));
            map_setup = &setup.mapeditor.map;
        }
        c_map_variant *map_variant = (c_map_variant *)map_setup->variant;
        setup_changed = true;
        memset(&map_setup->source, 0, sizeof(map_setup->source));
        map_setup->source.valid = true;
        map_setup->source.location = calls->get_location();
        switch (map_setup->source.location)
        {
        case _gui_stored_item_location_saved_game_file:
        case _gui_stored_item_location_autosave_queue:
            ((const c_gui_map_selected_item *)selected_item)->get_map_variant(map_variant);
            calls->get_name(map_setup->source.name, 256);
            break;
        default:
            map_variant->create_default((e_map_id)selection->m_metadata.map_id);
            map_setup->source.name[0] = 0;
            break;
        }
        controller_get((e_controller_index)controller_index)->get_player_identifier(&map_setup->source.owner);
        if (map_variant_is_valid((const s_map_variant_validation *)map_variant))
        {
            if (user_interface_squad_set_map_variant_with_verify((s_user_interface_session_privacy_request *)map_variant))
            {
                m_item_locked[selected_item->get_selection_type()] = locked;
            }
            else
            {
                user_interface_play_sound(_ui_sound_error, -1);
            }
            break;
        }
        g_gui_error_manager.post_error(0x2004e, k_any_controller, false);
        parent_screen_index = -1;
        break;
    }
    case _gui_selected_item_game_variant:
    {
        c_game_variant game_variant;
        if (!((const c_gui_game_variant_selected_item *)selected_item)->get_game_variant(&game_variant))
            break;
        if (network_life_cycle_squad_local_peer_is_leader())
        {
            if (game_engine_variant_is_valid(&game_variant))
            {
                if (user_interface_squad_set_game_variant_with_verify(&game_variant))
                {
                    setup.custom() = last_setup->custom;
                    setup_changed = true;
                    memset(&setup.custom().variant_source, 0, sizeof(setup.custom().variant_source));
                    setup.custom().variant_source.valid = true;
                    game_variant_set_from(&setup.custom().variant, &game_variant);
                    setup.custom().variant_source.location = calls->get_location();
                    calls->get_name(setup.custom().variant_source.name, 256);
                    controller_get((e_controller_index)controller_index)->get_player_identifier(&setup.custom().variant_source.owner);
                    m_item_locked[selected_item->get_selection_type()] = locked;
                }
                else
                {
                    user_interface_play_sound(_ui_sound_error, -1);
                }
                break;
            }
            g_gui_error_manager.post_error(0x2004f, k_any_controller, false);
            parent_screen_index = -1;
            break;
        }
        user_interface_play_sound(_ui_sound_error, -1);
        break;
    }
    case _gui_selected_item_advertisement:
    {
        e_network_session_class session_class = network_squad_session_get_session_class();
        switch (((const c_gui_network_mode_selected_item *)selected_item)->m_choice)
        {
        case _network_mode_choice_xbox_live_advertise_anyone:
            if (gui_pregame_setup_manager_any_online_user() && !online_local_xuid_is_online_enabled())
            {
                g_gui_error_manager.post_error(0x20067, controller_index, false);
                parent_screen_index = -1;
                break;
            }
            change_advertisement_mode(_network_advertise_xbox_live_open, session_class, _network_session_class_xbox_live, parent_screen_index);
            parent_screen_index = -1;
            break;
        case _network_mode_choice_xbox_live_advertise_friends:
            if (gui_pregame_setup_manager_any_online_user() && !online_local_xuid_is_online_enabled())
            {
                g_gui_error_manager.post_error(0x20067, controller_index, false);
                parent_screen_index = -1;
                break;
            }
            change_advertisement_mode(_network_advertise_xbox_live_friends_only, session_class, _network_session_class_xbox_live, parent_screen_index);
            parent_screen_index = -1;
            break;
        case _network_mode_choice_xbox_live_advertise_invite_only:
            if (gui_pregame_setup_manager_any_online_user() && !online_local_xuid_is_online_enabled())
            {
                g_gui_error_manager.post_error(0x20067, controller_index, false);
                parent_screen_index = -1;
                break;
            }
            change_advertisement_mode(_network_advertise_xbox_live_invite_only, session_class, _network_session_class_xbox_live, parent_screen_index);
            parent_screen_index = -1;
            break;
        case _network_mode_choice_system_link_advertise:
            if (get_alpha_is_internal_beta())
            {
                g_gui_error_manager.post_error(0x20001, controller_index, false);
                parent_screen_index = -1;
                break;
            }
            change_advertisement_mode(_network_advertise_system_link, session_class, _network_session_class_system_link, parent_screen_index);
            parent_screen_index = -1;
            break;
        case 5:
            if (get_alpha_is_internal_beta())
            {
                g_gui_error_manager.post_error(0x20001, controller_index, false);
                parent_screen_index = -1;
                break;
            }
            change_advertisement_mode(_network_advertise_offline, session_class, _network_session_class_offline, parent_screen_index);
            parent_screen_index = -1;
            break;
        case _network_mode_choice_system_link_browse:
        {
            if (get_alpha_is_internal_beta())
            {
                g_gui_error_manager.post_error(0x20001, controller_index, false);
                parent_screen_index = -1;
                break;
            }
            void *memory = user_interface_malloc_tracked(sizeof(c_load_game_browser_screen_message), 0, 0);
            if (memory)
            {
                c_load_game_browser_screen_message *message = new (memory) c_load_game_browser_screen_message(0x10334,
                    controller_index, _console_window, 0x1000c, 0, _browser_type_default);
                user_interface_messaging_post(message);
            }
            break;
        }
        }
        break;
    }
    case _gui_selected_item_hopper:
        if (user_interface_matchmaking_set_hopper(((const c_gui_hopper_selected_item *)selected_item)->m_information.id))
        {
            setup_changed = true;
            setup.matchmaking = last_setup->matchmaking;
            user_interface_matchmaking_squad_set_search_preference(0);
            setup.matchmaking.hopper_identifier = ((const c_gui_hopper_selected_item *)selected_item)->m_information.id;
            m_item_locked[selected_item->get_selection_type()] = locked;
        }
        else
        {
            user_interface_play_sound(_ui_sound_error, -1);
        }
        break;
    case _gui_selected_item_film:
    {
        bool film_valid = true;
        setup.theater() = last_setup->theater;
        setup_changed = true;
        if (((const c_gui_saved_film_selected_item *)selected_item)
                ->get_variants(&setup.theater().options, (c_game_variant *)&setup.theater().game_engine_index,
                    (c_map_variant *)&setup.theater().game_variant_index))
        {
            setup.theater().location = calls->get_location();
            controller_get((e_controller_index)controller_index)->get_player_identifier(&setup.theater().owner);
            setup.theater().controller_index = controller_index;
            setup.theater().film_category = ((const c_gui_saved_film_selected_item *)selected_item)->get_saved_film_category();
            setup.theater().campaign_id = selection->m_metadata.campaign_id;
            setup.theater().difficulty = selection->m_metadata.campaign_difficulty;
            setup.theater().map_id = selection->m_metadata.map_id;
            setup.theater().length_seconds = selection->m_metadata.length_seconds;
            memset(setup.theater().name, 0, sizeof(setup.theater().name));
            calls->get_name(setup.theater().name, 256);
            memset(setup.theater().description, 0, sizeof(setup.theater().description));
            wchar_t *description = setup.theater().description;
            ustrnzcpy_inline(description, calls->get_description(), 128);
            m_item_locked[selected_item->get_selection_type()] = locked;
            if (saved_film_can_load(controller_index, setup.theater().name))
            {
                user_interface_squad_set_film(&setup.theater().film_category, network_squad_session_get_session_class() == 2);
                network_squad_session_set_game_options(setup.theater().game_engine_index, setup.theater().game_variant_index, &setup.theater().options);
                if (setup.theater().options.campaign_id == -1)
                {
                    user_interface_squad_set_game_variant_with_verify(&setup.theater().options.game_variant);
                    if (setup.theater().options.map_variant.m_map_id != _map_id_none)
                    {
                        user_interface_squad_set_map_variant_with_verify((s_user_interface_session_privacy_request *)&setup.theater().options.map_variant);
                    }
                    else if (setup.theater().options.map_id != _map_id_none)
                    {
                        c_map_variant map_variant;
                        map_variant.create_default(setup.theater().options.map_id);
                        user_interface_squad_set_map_variant_with_verify((s_user_interface_session_privacy_request *)&map_variant);
                    }
                    else
                    {
                        film_valid = false;
                    }
                }
                else
                {
                    user_interface_squad_set_map((e_campaign_id)setup.theater().options.campaign_id, setup.theater().options.map_id);
                    network_squad_session_set_campaign_difficulty((e_campaign_difficulty_level)setup.theater().options.campaign_difficulty);
                    network_squad_session_set_primary_skulls(setup.theater().options.primary_skulls);
                    network_squad_session_set_secondary_skulls(setup.theater().options.secondary_skulls);
                    network_squad_session_set_campaign_metagame_scoring(setup.theater().options.campaign_metagame_scoring);
                    network_squad_session_set_campaign_insertion_point(setup.theater().options.campaign_insertion_point);
                }
            }
            else
            {
                film_valid = false;
            }
        }
        else
        {
            film_valid = false;
        }
        if (!film_valid)
        {
            g_gui_error_manager.post_error(0x20050, k_any_controller, false);
            parent_screen_index = -1;
        }
        break;
    }
    default:
        // match: 0x4a1f10: unreachable hint omits dead branch check
        __assume(0);
    }
    if (setup_changed)
        global_preferences_set_mode_setup((int *)&setup);
    if (parent_screen_index != -1)
    {
        c_gui_screen_widget *screen = g_window_manager.get_screen_by_screen_index(parent_screen_index);
        if (screen)
        {
            screen->transition_out_with_transition_type(0, _screen_transition_normal);
        }
    }
}

c_gui_pregame_setup_manager g_pregame_setup_manager;
