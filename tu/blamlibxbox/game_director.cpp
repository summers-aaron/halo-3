// @address 0x8c42a0 ?g_camera_globals@@3Us_camera_globals@@A
// blamlibxbox/game_director.obj: from the records 0x9a3c8 0x12a728 0x12ac48 0x201b88 0x201c60 0x201e60 0x2020d8 0x202280 0x293668
// @flags /TP /O2 /Oc
#include "simulation/simulation_queue_player_event_data_types.h"
#include "camera/camera_globals.h"
#include "memory/data_types.h"
#include "units/unit_definitions.h"
#include "input/input_abstraction.h"
#include "tag_files/tag_types.h"
#include "tag_files/string_ids.h"
#include "items/weapon_definitions.h"
#include "shell/shell.h"
#include "game/game_options.h"
#include "game/game_globals_storage.h"
#include "interface/gui_widget.h"
#include "interface/user_interface_messages.h"
#include "game/game.h"
#include "game/players.h"
#include "interface/gui_screens/scoreboard/gui_screen_scoreboard.h"
#include "interface/user_interface_memory.h"
#include "objects/objects.h"
#include "simulation/game_interface/simulation_game_action.h"
#include "motor/mover.h"
#include "camera/director.h"
#include "camera/game_director.h"
#include "interface/user_interface_controller.h"
#include "camera/observer_director.h"
#include "objects/object_datum.h"
#include "camera/camera.h"
#include "interface/controller_types.h"
#include "interface/c_window_manager.h"
#include "simulation/simulation_queue_player_event_types.h"
#include "cache/cache_files.h"
#include "simulation/simulation_queue_global_events.h"
#include "interface/c_deletable.h"
#include "input/input_abstraction_types.h"
class c_message;
class c_gui_list_widget;
#include "interface/c_gui_screen_widget.h"
#include "interface/gui_widget_hierarchy.h"

#include <string.h>
#include <new>
#include "interface/load_object_property_panel_screen_message.h"
#include "camera/camera_mode_types.h"
#include "objects/object_reference_types.h"
#include "math/matrix_math.h"
#include "cseries/cseries.h"
#include "camera/orbiting_camera.h"
#include "camera/game_director_internals.h"
#include "memory/datum_index.h"
enum e_director_perspective;
enum e_seat_state;
// @address 0x9a3c8 ?director_game_desired_perspective@@YA?AW4e_director_perspective@@JPAW4e_seat_state@@@Z
// @address 0x12ac48 director_get_scoreboard_screen
// @address 0x201b88 ?player_set_desired_respawn_player@@YAXJJ@Z


#include "objects/object_header.h"
#include "units/unit_types.h"
#include "tag_files/tag_groups.h"
#include "animations/animation_manager.h"
#include "game/player_mapping_types.h"
#include "motor/motor_animation_state.h"
#include "interface/user_interface_window_manager.h"
#include "camera/saved_film_director_storage.h"
// @address 0x11df85c g_director_camera_scripted
enum { k_scoreboard_multiplayer_animation = 30, k_scoreboard_cooperative_animation = 31 };

__forceinline bool weapon_uses_3rd_person_camera(long i)
{
    weapon_definition *d = (weapon_definition *)tag_get(0, object_get(i)->definition);
    return (bool)((d->flags >> 21) & 1);
}






// match: 0x9a3c8: seat_state stores keep global_tag_instances in r11; removal emits an extra lwz reload of it
enum e_director_perspective director_game_desired_perspective(long unit_index, enum e_seat_state * __restrict seat_state) {
 long perspective=0;
 *((long *)seat_state)=0;
 if(unit_index!=-1) {
  unit_datum *unit=(unit_datum *)object_get(unit_index);
  if(unit->object.parent!=-1) {
   object_datum *parent=object_get(unit->object.parent);
   long animation_state=motor_animation_get_state(unit_index);
   bool special_seat=false;
   if(((1<<parent->type)&4099) && unit->unit.parent_seat_index!=-1) {
    unit_definition *definition=(unit_definition *)tag_get(0, parent->definition);
    unit_seat *s=&definition->seats[unit->unit.parent_seat_index];
    special_seat=(bool)((s->flags>>6)&1);
    if((bool)((s->flags>>4)&1)) perspective=1;
   }
   if(special_seat && mover_is_in_entering_state(unit_index)) *((long *)seat_state)=1;
   else if(special_seat && (animation_state==85 || animation_state==1190)) *((long *)seat_state)=3;
   else *((long *)seat_state)=2;
  }
  short slot=((unit_datum *)object_get(unit_index))->unit.current_weapon_index;
  long weapon=-1;
  if(slot!=-1) weapon=((unit_datum *)object_get(unit_index))->unit.weapon_object_indices[slot];
  if(weapon!=-1 && weapon_uses_3rd_person_camera(weapon)) *((long *)seat_state)=4;
 }
 if(*((long *)seat_state)==1 || *((long *)seat_state)==3 || *((long *)seat_state)==4) perspective=1;
 return (e_director_perspective)perspective;
}

extern "C" {
c_gui_screen_widget *director_get_scoreboard_screen(e_window_index window_index)
{
    c_gui_screen_widget *screen = g_window_manager.get_screen_by_name(window_index, _string_id_observer_camera_list_screen);
    if (!screen)
        screen = g_window_manager.get_screen_by_name(window_index, _string_id_observer_camera_list_splitscreen);
    return screen;
}
}

static __forceinline bool game_director_is_multiplayer() { return tls_game_globals()->options.game_mode==2; }
static __forceinline bool game_director_is_networked()
{
    bool result = false;
    switch (tls_game_globals()->options.game_simulation)
    {
    case 4:
    case 5:
        result = true;
        break;
    }
    return result;
}
static __forceinline bool game_director_is_predicted()
{
    bool result = false;
    if (tls_game_globals()->options.game_simulation == 4)
        result = true;
    return result;
}
void player_set_desired_respawn_player(long player_index, long target_index) {
 if(game_director_is_multiplayer() && !game_is_cooperative() && game_director_is_networked()) {
  if(game_director_is_predicted()) ::simulation_request_player_respawn_target(player_index, target_index);
  else {
   s_simulation_queue_player_event_data data;
   data.active = false;
   memset(&data.respawn_target_absolute_player_index, 0, sizeof(data.respawn_target_absolute_player_index));
   if (target_index != -1)
       data.respawn_target_absolute_player_index = datum_index_to_absolute_index(target_index);
   else
       data.respawn_target_absolute_player_index = -1;
   simulation_queue_player_event_insert(_simulation_queue_player_event_respawn_target,player_index,&data);
  }
 }
}

// @address 0x201e60 director_update_scoreboard_focus
extern "C" void director_update_scoreboard_focus(c_director *director)
{
    long user_index = director->m_user_index;
    long controller_index = user_interface_controller_get_rumble_enabled(user_index);
    if (controller_index == -1)
        return;
    c_gui_screen_widget *screen = director_get_scoreboard_screen(
        (e_window_index)controller_globals().controller_windows[controller_index]);
    if (!screen || screen->m_suppress_focus)
        return;
    c_gui_widget *focused_widget = screen->m_focused_widget;
    if (!focused_widget || focused_widget->get_type() != _gui_list_item_widget)
        return;
    c_gui_data *datasource = 0;
    for (long index = 0; index < screen->m_datasource_count; index++)
    {
        if (screen->m_datasource[index]->get_name() == 0x100ce)
        {
            datasource = screen->m_datasource[index];
            break;
        }
    }
    bool already_focused = false;
    long player_query_result[2] = { -1, 2 };
    if (datasource->get_element(focused_widget->get_element_handle(), player_query_result, sizeof(player_query_result)))
    {
        long current_player;
        if (director->get_camera()->get_type() == _camera_mode_authored)
        {
            current_player = -1;
            if (user_index != -1)
                current_player = tls_player_mapping_globals()->player_indices[user_index];
        }
        else
            current_player = director->watched_player_index;
        if (player_query_result[1] == 2 && player_query_result[0] == current_player)
            already_focused = true;
    }
    if (already_focused)
        return;
    bool focus_updated = false;
    long element_handle = datasource->get_first_element_handle();
    while (element_handle != -1)
    {
        if (focus_updated)
            return;
        datasource->get_element(element_handle, player_query_result, sizeof(player_query_result));
        long wanted_player;
        if (director->get_camera()->get_type() == _camera_mode_authored)
            wanted_player = player_index_from_user_index(user_index);
        else
            wanted_player = director->watched_player_index;
        if (player_query_result[1] == 2 && player_query_result[0] != -1 && player_query_result[0] == wanted_player)
        {
            focused_widget->get_parent_screen()->transfer_focus_to_list(
                gui_widget_get_parent_list_widget(focused_widget), element_handle, false, false);
            focus_updated = true;
        }
        element_handle = datasource->get_next_element_handle(element_handle);
    }
}

// @address 0x202280 director_show_scoreboard_screen
extern "C" void director_show_scoreboard_screen(c_director *director)
{
    long controller_index = user_interface_controller_get_rumble_enabled(director->m_user_index);
    if (controller_index == -1)
        return;
    e_window_index window_index = (e_window_index)controller_globals().controller_windows[controller_index];
    c_gui_screen_widget *screen = director_get_scoreboard_screen(window_index);
    if (!screen || game_is_cooperative())
        return;
    gui_widget_set_alternate_state(screen, true);
    screen->start_animation_at_time((e_animation_state)k_scoreboard_multiplayer_animation, -1, true);
}

// 0x2020d8
// @address 0x2020d8 director_update_scoreboard_screen
extern "C" void director_update_scoreboard_screen(c_director *director)
{
    if (tls_game_globals()->options.game_mode != 2 && tls_game_globals()->game_lost_state != 0) {
        director_dispose_controller_screen(director);
        return;
    }
    long user_index = director->m_user_index;
    long controller_index = user_interface_controller_get_rumble_enabled(user_index);
    if (controller_index == -1)
        return;
    int screen_slot = controller_globals().controller_windows[controller_index];
    c_gui_screen_widget *screen = director_get_scoreboard_screen((e_window_index)screen_slot);
    if (!screen) {
        long list_name;
        if (players_get_active_user_count() > 1)
            list_name = _string_id_observer_camera_list_splitscreen;
        else
            list_name = _string_id_observer_camera_list_screen;
        long controller = user_interface_controller_get_rumble_enabled(user_index);
        if (controller != -1) {
#ifdef HALO3_64BIT
            void *memory = ::user_interface_malloc_tracked(sizeof(c_load_object_property_panel_screen_message), (char const *)(0), 0);
#else
            void *memory = ::user_interface_malloc_tracked(64, (char const *)(0), 0);
#endif
            if (memory) {
                c_load_object_property_panel_screen_message *message = new (memory) c_load_object_property_panel_screen_message(
                    list_name, (e_controller_index)controller,
                    (e_window_index)controller_globals().controller_windows[controller], _string_id_bottom_most, user_index);
                ::user_interface_messaging_post((c_message const *)(c_deletable *)message);
            }
        }
        director->scoreboard_animation_started = 0;
        return;
    }
    if (director->scoreboard_animation_started == 0) {
        bool done = false;
        long controller2 = user_interface_controller_get_rumble_enabled(user_index);
        if (controller2 != -1) {
            int screen2_slot = controller_globals().controller_windows[controller2];
            c_gui_screen_widget *screen2 = director_get_scoreboard_screen((e_window_index)screen2_slot);
            if (screen2) {
                long animation;
                if (game_is_cooperative()) {
                    gui_widget_set_alternate_state(screen2, 0);
                    animation = k_scoreboard_cooperative_animation;
                } else {
                    gui_widget_set_alternate_state(screen2, 1);
                    animation = k_scoreboard_multiplayer_animation;
                }
                screen2->start_animation_at_time((e_animation_state)animation, 0, true);
                done = true;
            }
        }
        director->scoreboard_animation_started = done;
    } else {
        director_update_scoreboard_focus(director);
    }
}


// @address 0x201c60 director_update_watched_player
extern "C" unsigned char director_update_watched_player(c_director *director)
{
    long user_index = director->m_user_index;
    long controller = user_interface_controller_get_rumble_enabled(user_index);
    bool result = false;
    if (controller != -1)
    {
        long selected_player = director->watched_player_index;
        long local_player = -1;
        if (!game_is_cooperative())
        {
            long player = -1;
            if (user_index != -1)
                player = tls_player_mapping_globals()->player_user_mapping[user_index];
            local_player = player;
        }
        if (!camera_responding_gate((e_controller_index)controller))
        {
            bool same_team = true;
            s_game_input_state *input_state = &input_abstraction_globals.input_states[controller];
            if (game_is_cooperative())
                same_team = false;
            if (input_state->buttons[2].down_frames() == 1)
            {
                selected_player = c_director::player_get_next_player_with_a_unit(-1,
                    selected_player, local_player, -1, same_team, false, true, 0);
                director_show_scoreboard_screen(director);
            }
            else if (input_state->buttons[3].down_frames() == 1)
            {
                selected_player = c_director::player_get_next_player_with_a_unit(-1,
                    selected_player, local_player, 1, same_team, false, true, 0);
                director_show_scoreboard_screen(director);
            }
        }
        if (selected_player != -1 && selected_player != director->watched_player_index)
        {
            player_datum *player_entry = &((player_datum *)tls_player_data()->data)[datum_index_to_absolute_index(selected_player)];
            long unit_index = player_entry->unit_index != -1 ? player_entry->unit_index : player_entry->dead_unit_index;
            director_set_watched_player(director, selected_player);
            if (unit_index != -1)
            {
                c_camera *camera = director->get_camera();
                s_object_header *object_data = ((s_object_header *)tls_object_header_data()->data);
                long root;
                long parent = unit_index;
                do
                {
                    root = parent;
                    parent = object_data[datum_index_to_absolute_index(root)].datum->parent;
                } while (parent != -1);
                camera->set_target(root);
                camera->m_camera_state = 5;
                camera->m_camera_flags.set(0, true);
            }
            if (local_player != -1 && selected_player != local_player && player_entry->unit_index != -1)
                player_set_desired_respawn_player(local_player, selected_player);
            result = true;
        }
    }
    return result;
}




// Facing follows the player's look override; a user with no player is held.
// @address 0x293668 ?inhibits_facing@c_game_director@@UBA_NXZ
bool c_game_director::inhibits_facing() const
{
    bool result = true;
    long user_index = m_user_index;
    if (user_index != -1) {
        long player_index = tls_player_mapping_globals()->player_indices[user_index];
        if (player_index != -1)
            result = ((player_datum *)tls_player_data()->data)[datum_index_to_absolute_index(player_index)].look_override;
    }
    return result;
}

// @address 0x12a728 ?update@c_game_director@@UAAXM@Z
void c_game_director::update(float dt)
{
    long user_index = this->m_user_index;
    long player_index = -1;
    if (user_index != -1)
        player_index = tls_player_mapping_globals()->player_indices[user_index];
    long unit_index = -1;
    if (user_index != -1)
        unit_index = tls_player_mapping_globals()->unit_indices[user_index];

    if (player_index != -1)
        game_engine_is_player_in_edit_mode(player_index, 0, 0);

    if (!*g_director_camera_scripted && tls_game_globals()->options.game_mode != 3)
    {

        if (unit_index == -1)
        {
            unit_index = get_camera()->get_target();
            c_camera *camera = get_camera();
            bool dead = false;
            if (unit_index != -1)
            {
                s_location target_location;
                object_get_location(unit_index, &target_location);
                if ((unsigned char)target_location.cluster_reference.bsp_index == 255)
                    dead = true;
            }
            if (unit_index == -1 || dead)
            {
                long new_target;
                player_get_next_player_with_a_unit(player_index, -1, -1, 1, 1, 1, 0, &new_target);
                camera->set_target(new_target);
                unit_index = new_target;
                camera->m_camera_state = 5;
                camera->m_camera_flags.set(0, true);
            }
            if (unit_index != -1 &&
                ((1 << ((s_object_header *)tls_object_header_data()->data)[
                    datum_index_to_absolute_index(unit_index)].object_type) & 0x1003) != 0)
            {
                e_camera_mode mode;
                if (
                    ((unit_datum *)((s_object_header *)tls_object_header_data()->data)[
                        datum_index_to_absolute_index(unit_index)].datum)->unit.last_active_time != -1)
                    mode = (e_camera_mode)observer_director_get_dead_camera_mode(unit_index, user_index);
                else
                    mode = _camera_mode_orbiting;
                if (set_camera_mode((e_camera_mode)mode, 0.0f, 0))
                {
                    if (mode != 1)
                    {
                        camera->set_target(unit_index);
                        camera->set_forward(&get_last_observer_command()->forward);
                    }
                    else
                    {
                        camera->set_target(unit_index);
                        camera->set_forward(&get_last_observer_command()->forward);
                        ((c_orbiting_camera *)camera)->set_distance(get_last_observer_command()->focus_distance);
                    }
                }
            }
            else
            {
                set_camera_mode(_camera_mode_dead, 0.0f, 0);
            }
            director_update_watched_player(this);
            director_update_scoreboard_screen(this);
        }
        else
        {
            e_seat_state seat_state;
            enum e_director_perspective perspective = director_game_desired_perspective(unit_index, &seat_state);
            bool changed = false;
            director_set_watched_player(this, player_index);
            c_camera *camera = get_camera();
            if (get_camera()->get_target() != unit_index)
            {
                camera->set_target(unit_index);
                changed = true;
            }
            if (perspective == 0)
                set_camera_mode(_camera_mode_first_person, changed ? 0.0f : g_camera_globals.orbiting_camera_switch_transition_time, 0);
            else
                set_camera_mode(_camera_mode_following, changed ? 0.0f : g_camera_globals.following_camera_transition_time, 0);
            director_dispose_controller_screen(this);
        }

    }
    c_director::update(dt);
}
