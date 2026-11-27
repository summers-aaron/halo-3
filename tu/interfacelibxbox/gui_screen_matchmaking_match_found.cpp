// interfacelibxbox/gui_screen_matchmaking_match_found.obj: from the records 0x4db980 0x4dba00 0x4dba88 0x4dbb18 0x4dbba8 0x4dbcd8 0x4dbd70 0x4dbe00 0x4dbe88 0x4dbf58 0x4dc000 0x4dc3a8 0x4dc3e0 0x4dc590 0x4dca88 0x4dcbe8 0x4dcbf0
#include "interface/user_interface_controller_globals.h"
#include "interface/user_interface_matchmaking_progress.h"
#include "interface/gui_roster_data.h"
#include "interface/list_widget_definition.h"
#include "interface/gui_screens/scoreboard/gui_screen_scoreboard.h"
#include "cseries/placement_new.h"
#include "interface/c_gui_list_item_widget.h"
#include "interface/gui_screens/player_select/player_select_screen_message.h"
// @flags /TP /O2 /Oc
#include "math/real_math_types.h"
#include "interface/gui_screens/matchmaking/matchmaking_screen_literals.h"
#include "interface/gui_widget_hierarchy.h"
#include "interface/custom_map_image_types.h"
#include "math/integer_math.h"
#include <stddef.h>
#include "game/game_types.h"
#include "game/player_configuration.h"
#include "shell/shell.h"
#include "interface/gui_widget_types.h"
#include "memory/static_arrays.h"
#include "interface/user_interface_messages.h"
#include "interface/user_interface_memory.h"
#include "interface/user_interface_session.h"
#include "text/unicode.h"
#include "networking/logic/network_life_cycle.h"
#include "main/levels.h"
#include "interface/gui_screens/pregame_lobby/gui_screen_pregame_lobby.h"
#include "interface/c_gui_bitmap_widget.h"
#include "interface/c_gui_text_widget.h"
#include "interface/ui_allocation_marker_types.h"
#include "interface/user_interface_allocation.h"
#include "interface/user_interface_utilities.h"
#include "interface/user_interface_controller.h"
#include "interface/user_interface_window_manager.h"

enum e_user_interface_sound_effect;
struct s_data_array;
#include "main/levels_globals.h"
extern "C" {
}


struct s_player_configuration;



#include "networking/logic/life_cycle/life_cycle_matchmaking_progress.h"



class c_game_tag_parser;

extern "C" {


}

#include "interface/game_tag_parser.h"






class c_gui_text_widget;
class c_gui_bitmap_widget;
class c_gui_screen_widget;
struct s_text_widget_block;

#include "interface/gui_widget.h"



#include "interface/gui_custom_bitmap_widget.h"

class c_controller_input_message;
#include "interface/c_gui_screen_widget.h"

// The base owner still defines this entry point with C linkage.



class c_message;

#include "interface/gui_roster_list_externs.h"

enum
{
    k_match_found_sound = 17,
    k_veto_added_sound = 5,
    k_map_load_progress_animation = 30
};

#include "interface/gui_screens/matchmaking/gui_screen_matchmaking_match_found.h"



// @address 0x4db980 ?parse_xml_playlist_name@@YA_NPAXPA_WJ@Z
bool parse_xml_playlist_name(void *context, wchar_t *buffer, long count)
{
    bool success = false;
    if (context != 0)
    {
        wchar_t string[256];
        s_life_cycle_matchmaking_progress *block = &((c_gui_screen_matchmaking_match_found *)context)->m_matchmaking_progress;
        string[0] = 0;
        if (block != 0)
            ((c_static_wchar_string<256> *)string)->set((const wchar_t *)block->hopper_name);
        if (((const c_static_wchar_string<256> *)string)->length() > 0)
        {
            unsigned long size = count;
            if (size > 256)
                size = 256;
            ustrnzcpy(buffer, string, size);
            success = true;
        }
    }
    return success;
}

// @address 0x4dba00 ?parse_xml_current_players@@YA_NPAXPA_WJ@Z
static bool parse_xml_current_players(void *context, wchar_t *buffer, long count)
{
    bool success = false;
    if (context != 0)
    {
        c_static_wchar_string<256> string;
        s_life_cycle_matchmaking_progress *block = &((c_gui_screen_matchmaking_match_found *)context)->m_matchmaking_progress;
        string.clear();
        if (block != 0)
            string.print(L"%d", block->group_player_count);
        if (string.length() > 0)
        {
            string.copy_to(buffer, count);
            success = true;
        }
    }
    return success;
}

// @address 0x4dba88 ?parse_xml_current_veto_count@@YA_NPAXPA_WJ@Z
bool parse_xml_current_veto_count(void *context, wchar_t *buffer, long count)
{
    bool success = false;
    if (context != 0)
    {
        c_static_wchar_string<256> string;
        c_gui_screen_matchmaking_match_found *veto = (c_gui_screen_matchmaking_match_found *)context;
        string.clear();
        if (veto->m_matchmaking_progress.type == 4)
            string.print(L"%d", veto->m_matchmaking_progress.configuring_match.current_veto_count);
        if (string.length() > 0)
        {
            string.copy_to(buffer, count);
            success = true;
        }
    }
    return success;
}

// @address 0x4dbb18 ?parse_xml_max_veto_count@@YA_NPAXPA_WJ@Z
bool parse_xml_max_veto_count(void *context, wchar_t *buffer, long count)
{
    bool success = false;
    if (context != 0)
    {
        c_static_wchar_string<256> string;
        c_gui_screen_matchmaking_match_found *veto = (c_gui_screen_matchmaking_match_found *)context;
        string.clear();
        if (veto->m_matchmaking_progress.type == 4)
            string.print(L"%d", veto->m_matchmaking_progress.configuring_match.max_veto_count);
        if (string.length() > 0)
        {
            string.copy_to(buffer, count);
            success = true;
        }
    }
    return success;
}

// @address 0x4dbba8 ?parse_xml_local_veto_indicators@@YA_NPAXPA_WJ@Z
bool parse_xml_local_veto_indicators(void *context, wchar_t *buffer, long count)
{
    bool success = false;
    if (context != 0)
    {
        wchar_t string[256];
        c_gui_screen_matchmaking_match_found *veto = (c_gui_screen_matchmaking_match_found *)context;
        unsigned long *flags = &veto->m_matchmaking_progress.ready_player_mask;
        string[0] = 0;
        if (veto->m_matchmaking_progress.type == 4)
        {
            c_static_wchar_string<1024> tag_string;
            if (((c_gui_screen_widget *)context)->get_string_by_string_id(13, &tag_string))
            {
                for (long player_index = 0; player_index < 16; player_index++)
                {
                    if (user_interface_group_is_player_valid(player_index) && user_interface_group_is_local_player(player_index) &&
                        (long)((1 << player_index) & *flags) != 0)
                    {
                        ::ustrnzcat((wchar_t *)(string), (wchar_t const *)(tag_string.m_string), 0x100);
                        string[255] = 0;
                    }
                }
            }
            if (((const c_static_wchar_string<256> *)string)->length() == 0)
            {
                ::ustrnzcat((wchar_t *)(string), L" ", 0x100);
                string[255] = 0;
            }
        }
        if (((const c_static_wchar_string<256> *)string)->length() > 0)
        {
            unsigned long size = count;
            if (size > 256)
                size = 256;
            ustrnzcpy(buffer, string, size);
            success = true;
        }
    }
    return success;
}

// @address 0x4dbcd8 ?parse_xml_game_start_countdown_seconds@@YA_NPAXPA_WJ@Z
bool parse_xml_game_start_countdown_seconds(void *context, wchar_t *buffer, long count)
{
    bool success = false;
    if (context != 0)
    {
        c_static_wchar_string<256> string;
        c_gui_screen_matchmaking_match_found *veto = (c_gui_screen_matchmaking_match_found *)context;
        string.clear();
        if (veto->m_matchmaking_progress.type == 4)
        {
            long seconds = veto->m_matchmaking_progress.configuring_match.countdown_timer - 1 > 0
                ? veto->m_matchmaking_progress.configuring_match.countdown_timer - 1 : 0;
            string.print(L"%d", seconds);
        }
        if (string.length() > 0)
        {
            string.copy_to(buffer, count);
            success = true;
        }
    }
    return success;
}

// @address 0x4dbd70 ?parse_xml_game_start_map_load_progress_percentage@@YA_NPAXPA_WJ@Z
bool parse_xml_game_start_map_load_progress_percentage(void *context, wchar_t *buffer, long count)
{
    bool success = false;
    if (context != 0)
    {
        c_static_wchar_string<256> string;
        c_gui_screen_matchmaking_match_found *veto = (c_gui_screen_matchmaking_match_found *)context;
        string.clear();
        if (veto->m_matchmaking_progress.type == 4)
            string.print(L"%d", veto->m_matchmaking_progress.configuring_match.map_load_progress_percentage);
        if (string.length() > 0)
        {
            string.copy_to(buffer, count);
            success = true;
        }
    }
    return success;
}

// @address 0x4dbe00 ?parse_xml_game_variant_name@@YA_NPAXPA_WJ@Z
bool parse_xml_game_variant_name(void *context, wchar_t *buffer, long count)
{
    bool success = false;
    if (context != 0)
    {
        wchar_t string[256];
        s_life_cycle_matchmaking_progress *block = &((c_gui_screen_matchmaking_match_found *)context)->m_matchmaking_progress;
        string[0] = 0;
        if (block->type == 4)
            ((c_static_wchar_string<256> *)string)->set(block->game_variant_name);
        if (((const c_static_wchar_string<256> *)string)->length() > 0)
        {
            unsigned long size = count;
            if (size > 256)
                size = 256;
            ustrnzcpy(buffer, string, size);
            success = true;
        }
    }
    return success;
}

// @address 0x4dbe88 ?parse_xml_map_variant_name@@YA_NPAXPA_WJ@Z
bool parse_xml_map_variant_name(void *context, wchar_t *buffer, long count)
{
    bool success = false;
    if (context != 0)
    {
        wchar_t string[256];
        s_life_cycle_matchmaking_progress *block = &((c_gui_screen_matchmaking_match_found *)context)->m_matchmaking_progress;
        string[0] = 0;
        if (block->type == 4)
        {
            if (block->map_variant_name[0] != 0)
                ((c_static_wchar_string<256> *)string)->set(block->map_variant_name);
            else
            {
                s_datum level;
                if (levels_try_and_get_datum(g_level_globals.multiplayer_levels, block->lobby_type, &level))
                    ((c_static_wchar_string<256> *)string)->set((const wchar_t *)level.display_name);
                else
                    string[0] = 0;
            }
        }
        if (((const c_static_wchar_string<256> *)string)->length() > 0)
        {
            unsigned long size = count;
            if (size > 256)
                size = 256;
            ustrnzcpy(buffer, string, size);
            success = true;
        }
    }
    return success;
}

// @address 0x4dbf58 ?parse_xml_party_leader@@YA_NPAXPA_WJ@Z
static bool parse_xml_party_leader(void *context, wchar_t *buffer, long count)
{
    bool success = false;
    if (context != 0)
    {
        c_static_wchar_string<256> string;
        string.clear();
        if (user_interface_squad_exists())
        {
            long leader_player_index = user_interface_squad_get_leader_player_index();
            if (leader_player_index != -1)
            {
                const s_player_configuration *leader = user_interface_squad_get_player_data(leader_player_index);
                if (leader != 0)
                {
                    ustrnzcpy(buffer, leader->host.player_name, count);
                    success = true;
                }
            }
        }
        if (string.length() > 0)
        {
            string.copy_to(buffer, count);
            success = true;
        }
    }
    return success;
}

void c_gui_screen_matchmaking_match_found::initialize()
{
    c_gui_screen_widget::initialize();
    add_game_tag_parser(new(0, 0, _ui_allocation_marker_dummy) c_magic_string_game_tag_parser(L"<playlist-name", this, parse_xml_playlist_name));
    add_game_tag_parser(new(0, 0, _ui_allocation_marker_dummy) c_magic_string_game_tag_parser(L"<current-players", this, parse_xml_current_players));
    add_game_tag_parser(new (0, 0, _ui_allocation_marker_dummy)
            c_magic_string_game_tag_parser(L"<veto-countdown-seconds", this, parse_xml_game_start_countdown_seconds));
    add_game_tag_parser(new (0, 0, _ui_allocation_marker_dummy)
            c_magic_string_game_tag_parser(L"<current-veto-count", this, parse_xml_current_veto_count));
    add_game_tag_parser(new(0, 0, _ui_allocation_marker_dummy) c_magic_string_game_tag_parser(L"<max-veto-count", this, parse_xml_max_veto_count));
    add_game_tag_parser(new (0, 0, _ui_allocation_marker_dummy)
            c_magic_string_game_tag_parser(L"<local-veto-indicators", this, parse_xml_local_veto_indicators));
    add_game_tag_parser(new (0, 0, _ui_allocation_marker_dummy)
            c_magic_string_game_tag_parser(L"<game-start-countdown-seconds", this, parse_xml_game_start_countdown_seconds));
    add_game_tag_parser(new (0, 0, _ui_allocation_marker_dummy)
            c_magic_string_game_tag_parser(L"<map-load-progress-percentage", this, parse_xml_game_start_map_load_progress_percentage));
    add_game_tag_parser(new (0, 0, _ui_allocation_marker_dummy)
            c_magic_string_game_tag_parser(L"<game-variant-name", this, parse_xml_game_variant_name));
    add_game_tag_parser(new(0, 0, _ui_allocation_marker_dummy) c_magic_string_game_tag_parser(L"<map-variant-name", this, parse_xml_map_variant_name));
    add_game_tag_parser(new(0, 0, _ui_allocation_marker_dummy) c_magic_string_game_tag_parser(L"<party-leader", this, parse_xml_party_leader));
}

// @address 0x4dc3a8 ?post_initialize@c_gui_screen_matchmaking_match_found@@UAAXXZ
void c_gui_screen_matchmaking_match_found::post_initialize()
{
    c_gui_screen_widget::post_initialize();
    ::user_interface_play_sound((e_user_interface_sound_effect)k_match_found_sound, m_core_definition.sound_override_index);
}

// @address 0x4dc3e0 ?update@c_gui_screen_matchmaking_match_found@@UAAXK@Z
void c_gui_screen_matchmaking_match_found::update(unsigned long current_milliseconds)
{
    c_gui_screen_matchmaking_match_found *veto = (c_gui_screen_matchmaking_match_found *)this;
    s_life_cycle_matchmaking_progress progress;
    c_gui_widget *match_list = get_child_widget(_gui_list_widget, 0x103a7);
    network_life_cycle_get_matchmaking_progress(&progress);
    *(s_life_cycle_matchmaking_progress *)((char *)&g_user_interface_matchmaking_progress) = progress;
    if (progress.type == 4)
        veto->m_matchmaking_progress = progress;
    if (match_list != 0)
        ((c_gui_roster_list_widget *)match_list)->m_show_party_indicators = true;
    update_status();
    update_countdown();
    c_gui_widget *progress_list = get_child_widget(_gui_group_widget, 0x10041);
    if (progress_list != 0)
    {
        long load_progress = 0;
        if (veto->m_matchmaking_progress.type == 4)
        {
            load_progress = veto->m_matchmaking_progress.configuring_match.map_load_progress_percentage;
            if (load_progress != 0)
                progress_list->start_animation_at_time((e_animation_state)k_map_load_progress_animation,
                    current_milliseconds - load_progress * 10, true);
        }
        progress_list->set_visible(load_progress != 0);
    }
    if (veto->m_matchmaking_progress.type == 4)
    {
        long veto_count = count_bits<unsigned long>(veto->m_matchmaking_progress.ready_player_mask);
        if (veto_count > veto->previous_veto_count)
            ::user_interface_play_sound((e_user_interface_sound_effect)k_veto_added_sound, m_core_definition.sound_override_index);
        veto->previous_veto_count = veto_count;
    }
    else
    {
        veto->previous_veto_count = 0;
    }
    bool show_first;
    if (user_interface_squad_get_player_count() > 1)
        show_first = true;
    else
        show_first = false;
    c_gui_widget *first_widget = get_child_widget(_gui_text_widget, 0x1033f);
    if (first_widget != 0)
        first_widget->set_visible(show_first);
    bool show_second;
    if (user_interface_squad_get_player_count() > 1)
        show_second = true;
    else
        show_second = false;
    c_gui_widget *second_widget = get_child_widget(_gui_bitmap_widget, 0x10340);
    if (second_widget != 0)
        second_widget->set_visible(show_second);
    c_gui_screen_widget::update(current_milliseconds);
}

// @address 0x4dc590 ?update_status@c_gui_screen_matchmaking_match_found@@AAAXXZ
void c_gui_screen_matchmaking_match_found::update_status()
{
    c_gui_screen_matchmaking_match_found *veto = (c_gui_screen_matchmaking_match_found *)this;

    c_gui_text_widget *text_a = get_child_text_widget(0x10410);
    c_gui_text_widget *text_b = get_child_text_widget(0x10411);
    c_gui_text_widget *text_c = get_child_text_widget(0x10412);
    c_gui_text_widget *text_d = get_child_text_widget(0x10413);
    c_gui_text_widget *text_e = get_child_text_widget(0x29e);
    c_gui_text_widget *text_f = get_child_text_widget(0x10414);
    c_gui_bitmap_widget *bitmap_a = get_child_bitmap_widget(0x10416);
    c_gui_text_widget *text_g = get_child_text_widget(0x1002a);
    c_gui_bitmap_widget *bitmap_b = get_child_bitmap_widget(0x10328);
    c_gui_custom_bitmap_widget *map_image = (c_gui_custom_bitmap_widget *)get_child_bitmap_widget(0x1032a);

    set_child_visible(_gui_text_widget, 0x10411, false);
    set_child_visible(_gui_text_widget, 0x10412, false);
    set_child_visible(_gui_text_widget, 0x10413, false);
    set_child_visible(_gui_text_widget, 0x29e, false);
    set_child_visible(_gui_text_widget, 0x10414, false);
    set_child_visible(_gui_bitmap_widget, 0x10416, false);

    if (text_a == 0 || text_b == 0 || text_e == 0 || text_f == 0 || text_c == 0 || text_d == 0
        || bitmap_a == 0 || text_g == 0 || bitmap_b == 0 || map_image == 0)
        return;
    if (veto->m_matchmaking_progress.type != 4)
        return;

    bitmap_b->set_sprite_frame(veto->m_matchmaking_progress.game_engine_index);
    map_image->set_map_image(_custom_map_image_type_normal, (e_map_id)veto->m_matchmaking_progress.lobby_type, false);

    switch (veto->m_matchmaking_progress.configuring_match.state)
    {
    case 0:
        text_a->set_text_from_string_id(this, 0x10405);
        break;
    case 3:
        text_a->set_text_from_string_id(this, 0x1040c);
        text_b->set_text_from_string_id(this, 0x1040d);
        text_b->set_visible(true);
        return;
    case 1:
        {
            bool all_local_players_vetoed = true;
            for (long player_index = 0; player_index < 16; player_index++)
            {
                if (user_interface_group_is_player_valid(player_index) && user_interface_group_is_local_player(player_index))
                    all_local_players_vetoed = all_local_players_vetoed & ((veto->m_matchmaking_progress.ready_player_mask & (1 << player_index)) != 0);
            }
            text_a->set_text_from_string_id(this, 0x10406);
            text_b->set_text_from_string_id(this, 0x10407);
            text_b->set_visible(true);
            text_c->set_text_from_string_id(this, 0x10408);
            text_c->set_visible(true);
            text_d->set_text_from_string_id(this, 0x10409);
            text_d->set_visible(true);
            text_f->set_text_from_string_id(this, all_local_players_vetoed ? 0x10415 : 0x10414);
            gui_widget_set_alternate_state(text_f, all_local_players_vetoed == 0);
            text_f->set_visible(true);
            bitmap_a->set_visible(true);
            text_e->set_text_from_string_id(this, 0x1040f);
            text_e->set_visible(true);
        }
        return;
    case 4:
        text_a->set_text_from_string_id(this, 0x1040b);
        break;
    case 2:
        text_a->set_text_from_string_id(this, 0x1040a);
        text_b->set_text_from_string_id(this, 0x10405);
        text_b->set_visible(true);
        return;
    case 5:
        text_a->set_text_from_string_id(this, 0x1040c);
        break;
    default:
        break;
    }
}

// @address 0x4dca88 ?update_countdown@c_gui_screen_matchmaking_match_found@@AAAXXZ
void c_gui_screen_matchmaking_match_found::update_countdown()
{
    c_gui_screen_matchmaking_match_found *veto = (c_gui_screen_matchmaking_match_found *)this;
    bool counting = false;
    bool starting = false;
    long state = veto->m_matchmaking_progress.configuring_match.state;
    long countdown = veto->m_matchmaking_progress.configuring_match.countdown_timer;

    if (state == 1)
        counting = true;
    else if ((unsigned long)(state - 3) <= 2)
    {
        starting = true;
        counting = true;
    }

    if (counting && countdown != veto->previous_countdown)
    {
        long start_threshold = get_constants_datasource_integer(0x10023);
        if (countdown <= get_constants_datasource_integer(60) + 1 && countdown != -1 && countdown != 0)
        {
            if (starting)
                ::user_interface_play_sound((e_user_interface_sound_effect)((countdown == 1) + 13), m_core_definition.sound_override_index);
            else
                ::user_interface_play_sound((e_user_interface_sound_effect)((countdown == 1) + 15), m_core_definition.sound_override_index);
        }
        if (starting)
        {
            if (!veto->countdown_started || countdown <= start_threshold)
            {
                if (countdown == start_threshold)
                {
                    g_window_manager.m_fade_in = true;
                    veto->countdown_started = true;
                }
            }
            else
            {
                g_window_manager.m_fade_in = false;
                veto->countdown_started = false;
            }
        }
        else
        {
            g_window_manager.m_fade_in = false;
            veto->countdown_started = false;
        }
        veto->previous_countdown = countdown;
    }
    else if (!counting && veto->countdown_started)
    {
        g_window_manager.m_fade_in = false;
        veto->previous_countdown = 0;
        veto->countdown_started = false;
    }
}

bool c_gui_screen_matchmaking_match_found::process_message(const c_message *message)
{
    return c_gui_screen_widget::process_message(message);
}


static __forceinline void match_found_set_desires_veto(e_controller_index controller_index, s_user_interface_controller *controller)
{
    bool changed = controller->desires_veto != 1;
    controller->desires_veto = true;
    if (changed) user_interface_controller_update_network_properties(controller_index);
}

// @address 0x4dcbf0 ?handle_controller_input_message@c_gui_screen_matchmaking_match_found@@UAA_NPBVc_controller_input_message@@@Z
bool c_gui_screen_matchmaking_match_found::handle_controller_input_message(const c_controller_input_message *message)
{
    c_gui_screen_matchmaking_match_found *veto = (c_gui_screen_matchmaking_match_found *)this;
    if (message->get_event_type() == 13 && message->get_component() == 2
        && veto->m_matchmaking_progress.type == 4 && veto->m_matchmaking_progress.configuring_match.state == 1)
    {
        e_controller_index controller_index = message->get_controller();
        match_found_set_desires_veto(controller_index, &g_user_interface_controller_globals.controllers[controller_index]);
    }
    return c_gui_screen_widget::handle_controller_input_message(message);
}










// @address 0x4cbb48 ?create_list_widget@c_gui_screen_matchmaking_match_found@@UAAPAVc_gui_list_widget@@PBUs_list_widget_block@@@Z
c_gui_list_widget *c_gui_screen_matchmaking_match_found::create_list_widget(const s_list_widget_block *definition)
{
    if (definition->definition.core.name == _pregame_lobby_string_roster) {
        void *memory = user_interface_malloc_tracked(sizeof(c_gui_roster_list_widget), 0, 0);
        if (memory)
            return new (memory) c_gui_roster_list_widget(true, false);
    } else {
        void *memory = user_interface_malloc_tracked(sizeof(c_gui_list_widget), 0, 0);
        if (memory)
            return new (memory) c_gui_list_widget;
    }
    return 0;
}

// @address 0x4d7500 ?create_bitmap_widget@c_gui_screen_matchmaking_match_found@@UAAPAVc_gui_bitmap_widget@@PBUs_runtime_bitmap_widget_definition@@@Z
c_gui_bitmap_widget *c_gui_screen_matchmaking_match_found::create_bitmap_widget(const s_runtime_bitmap_widget_definition *definition)
{
    if (definition->name == _pregame_lobby_string_map_image) {
        return new(0, 0, _ui_allocation_marker_dummy) c_gui_custom_bitmap_widget();
    }
    return new(0, 0, _ui_allocation_marker_dummy) c_gui_bitmap_widget();
}

// @address 0x4da478 ?initialize_datasource@c_gui_screen_matchmaking_match_found@@UAAXXZ
void c_gui_screen_matchmaking_match_found::initialize_datasource()
{
    c_gui_screen_widget::initialize_datasource();
    void *memory = ::user_interface_malloc_tracked(sizeof(c_gui_squad_roster_data), (char const *)(0), 0);
    c_gui_data *data = memory != 0 ? new(memory) c_gui_squad_roster_data(get_driving_controller()) : 0;
    if (data != 0) {
        if (data->initialize(_pregame_lobby_string_roster))
            add_datasource(data);
        else
            {
            data->~c_gui_data();
            user_interface_free(data);
        }
    }
}

// @address 0x4cc598 ?handle_list_item_chosen@c_gui_screen_matchmaking_match_found@@UAA_NPBVc_controller_input_message@@JPAVc_gui_list_item_widget@@PAVc_gui_data@@@Z
bool c_gui_screen_matchmaking_match_found::handle_list_item_chosen(
    const c_controller_input_message *message, long list_name, c_gui_list_item_widget *list_item, c_gui_data *data)
{
 bool handled = false;
 if (list_name == _pregame_lobby_string_roster) {
  long selected = -1;
  if (data->get_integer_value(list_item->get_element_handle(), _pregame_lobby_string_player_index, &selected)) {
   long player = ::user_interface_session_get_player_at_local_controller(message->get_controller());
   if (player != -1) {
    const s_player_identifier *local_id = user_interface_session_is_local_player(player);
    const s_player_identifier *selected_id = user_interface_session_is_local_player(selected);
    if (local_id != 0 && selected_id != 0) {
     c_load_player_select_screen_message *load = (c_load_player_select_screen_message *)user_interface_malloc_tracked(sizeof(c_load_player_select_screen_message), 0, 0);
     if (load) {
      long layer = this->m_name;
      long controller = message->get_controller();
      c_load_player_select_screen_message *posted = new (load) c_load_player_select_screen_message(
          _pregame_lobby_string_player_select, (e_controller_index)controller, this->get_render_window(), layer, local_id, selected_id, true);
      if (posted) {
       posted->set_parent_screen_index(this->m_screen_index);
       ::user_interface_messaging_post(posted);
      }
     }
    } else {
     ::user_interface_play_sound(_ui_sound_error, this->m_core_definition.sound_override_index);
    }
   }
  }
  handled = true;
 }
 if (!handled) handled = c_gui_screen_widget::handle_list_item_chosen(message, list_name, list_item, data);
 return handled;
}
