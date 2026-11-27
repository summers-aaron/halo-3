// blamlibxbox/director.obj: from the records 0xd5c20 0x1123d0 0x12a3a8 0x12a4a0 0x12aa90 0x12ab00 0x12abb8 0x12ac98 0x1906a0 0x1e3138 0x200d88 0x200e38 0x2010d0 0x2010d8 0x2011f0 0x2012b8 0x201398 0x201618 0x201690 0x202308 0x202530 0x2025d0 0x202608 0x2026a8 0x2026e0 0x2412c0 0x282348 0x282500 0x294150 0x298158 0x298990 0x4470c0 0x447b98 0x447c38 0x447c50 0x707e20 0x707e30 0x709fb8 0x70b9c8 0x70b9e0
// @address 0x2251c k_real_zero_storage
// @address 0x11df680 game_globals
// @address 0x11401e8 g_controller_globals
// @address 0x11df8cc g_observer_globals
// @address 0x3aa90 k_director_zero_vector
// @address 0x282418 players_is_player_local
// @address 0x10b0 k_font_cache_vtable
// @address 0x73a248 ?render_globals@c_rasterizer@@2Us_rasterizer_render_globals@@A
// @address 0x119a9c0 ?saved@?1??camera_globals_initialize_from_tags@@YAXXZ@4Us_camera_globals@@A
// @address 0x8c42a0 ?g_camera_globals@@3Us_camera_globals@@A
// @flags /TP /O2
#include "game/player_control_globals.h"
// @address 0x94e3b0 g_saved_film_timer_globals
#include "first_person/first_person_weapon_visibility.h"
#include "text/font_id_types.h"
#include "text/font_cache_lock.h"
#include "rasterizer/rasterizer.h"
#include "camera/camera_globals.h"
#include "game/game_globals.h"
#include "math/matrix_math_types.h"
#include "game/game_globals_storage.h"
#include "math/real_math_types.h"
#include "memory/data_types.h"
#include "memory/datum_index.h"
#include "tag_files/tag_groups.h"
#include "shell/shell.h"
#include "game/player_types.h"
#include "game/player_mapping_types.h"
#include "memory/tls_globals.h"
#include "cache/restricted_memory.h"
#include "cache/restricted_memory_registration.h"
// @address 0xd5c20 ?director_get_perspective@@YA?AW4e_director_perspective@@JJ@Z
// @address 0x201618 director_get_user_unit
// @address 0x2026e0 ?set_target@c_camera@@UAAXJ@Z
// @address 0x4470c0 ?__tls_set_g_director_camera_scripted_allocator@@YAXPAX@Z
// @address 0x1d6bb0 ?restricted_region_free_member@@YAXJJ@Z
// @address 0x200d68 ?__tls_set_g_director_globals_allocator@@YAXPAX@Z
// @address 0x200d78 ?__tls_set_g_director_camera_scripted_flag_allocator@@YAXPAX@Z
// @address 0x293278 ?reserve_memory@?$t_restricted_allocation_manager@$02$0A@$0A@$1?__tls_set_g_camera_script_globals_allocator@@YAXPAX@Z@@QAAPAXPBD0IJ@Z
// @address 0x1e3138 ?director_set_authored_cameras@@YAXXZ
// @address 0x282500 director_try_and_get_user_player
#include "cache/restricted_memory_regions.h"
#include "game/game.h"
#include "camera/camera_scripting.h"
#include "camera/scripted_camera.h"
#include "camera/director.h"
#include "camera/director_runtime_symbols.h"
#include "camera/camera_script_globals_types.h"
#include "camera/observer.h"
#include "cutscene/cinematics.h"
#include "math/color_math.h"
#include "rasterizer/rasterizer_hue_saturation.h"
#include "rasterizer/rasterizer_main.h"
#include "text/draw_string.h"
#include "units/vehicle_type_component.h"
#include "visibility/visibility_portal_activation.h"
#include "game/player_control.h"
#include "game/players.h"
#include "game/player_access.h"
#include "interface/user_interface_controller.h"
#include "memory/data.h"
#include "hs/object_lists.h"
#include "camera/game_director.h"
#include "objects/objects.h"
#include "scenario/scenario_tag.h"
#include "objects/object_reference_types.h"
#include "rasterizer/rasterizer_video_mode_types.h"
#include "interface/chud/utf32_types.h"
#include "scenario/scenario.h"
#include "rasterizer/rasterizer_render_globals_types.h"

#include "math/real_math.h"
#include "camera/orbiting_camera.h"
#include "camera/following_camera.h"
#include "units/biped_types.h"
enum e_director_perspective;

// 0x1123d0
#include "camera/debug_director_internals.h"

#include "camera/camera_mode_types.h"
#include "camera/observer_command.h"
#include "camera/director_globals_types.h"
#include "camera/saved_film_director_internals.h"
#include "camera/saved_film_director_storage.h"
#include "game/player_mapping_inline.h"
#include "interface/c_gui_screen_widget.h"
#include "interface/controller_types.h"
#include "cache/restricted_memory_layout.h"
struct s_dircam_camera_definition { s_game_globals_camera camera; s_camera_globals options; };
#include "cache/restricted_memory_layout.h"
#include "multithreading/thread_tls.h"
#include "cache/restricted_memory_regions_inline.h"
#include <stdlib.h>
// match: 0x200e38: the saved authored camera copies its zero facing out of a three-float temporary.
struct s_director_zero_facing { float unused; real_euler_angles2d facing; };
#include "camera/director_notification_types.h"
#include <new>
#include "camera/first_person_camera_types.h"
#include "camera/dead_camera.h"
#include "camera/static_camera.h"
#include "camera/authored_camera.h"

#include "camera/game_director_internals.h"
#include "camera/editor_director.h"
#include "cseries/read_write_barrier.h"
#include "camera/observer_globals_types.h"
#include "saved_games/saved_film_globals.h"
#include "saved_games/saved_film_timer_storage.h"
#include "game/player_in_game_iterator.h"
#include "scenario/scenario_cutscene_flag.h"
#include "cutscene/cinematic_new_globals_types.h"
#include <string.h>
#include "rasterizer/rasterizer.h"
#include "text/font_cache_lock.h"
#include "text/font_character_types.h"
#include "text/font_cache_types.h"
#include "text/draw_string_types.h"
#include "text/rasterizer_draw_string_types.h"
// Thread-local player-control flags gating the director camera debug readout.
#include "game/player_control_types.h"

// Find the next set bit among the four mask bits; -1 starts at the first bit.
enum e_director_perspective director_get_perspective(long datum_index, long previous_bit)
{
  long result = -1;
  unsigned char mask = (*(s_user_globals **)&player_mapping_globals)->player_masks.masks[datum_index_to_absolute_index(datum_index)];
  if (mask) {
    long first_bit = previous_bit == -1 ? 0 : previous_bit + 1;
    for (long bit_index = first_bit; bit_index < 4; bit_index++) {
      if ((1 << bit_index) & mask) {
        result = bit_index;
        break;
      }
    }
  }
  return (e_director_perspective)(result);
}

// @address 0x1123d0 ?handle_deleted_object@c_director@@UAAXJ@Z
void c_director::handle_deleted_object(long object_index)
{
    get_camera()->handle_deleted_object(object_index);
}

// 0x12ac98: observer command passed to camera update and stored on the director
// Per-user observer submission state behind g_observer_globals (0x3c8 stride); the
// command slot header sits 4 bytes into each entry.
// @address 0x12a3a8 ?director_update@@YAXM@Z
void director_update(float dt)
{
    float maximum_dt = 0.06666667f;
    dt = dt > maximum_dt ? maximum_dt : dt;
    g_director_globals->dt = dt;
    if (g_director_globals->transition > k_real_zero_storage)
        g_director_globals->transition
            = (g_director_globals->transition - dt < k_real_zero_storage
                ? k_real_zero_storage
                : g_director_globals->transition - dt);
    else if (g_director_globals->transition < k_real_zero_storage)
        g_director_globals->transition
            = (g_director_globals->transition + dt < k_real_zero_storage
                ? g_director_globals->transition + dt
                : k_real_zero_storage);
    for (long user = 0; user < 4; user++) {
        if (players_user_is_active(user)) {
            c_director *director = director_get(user);
            director->update(dt);
        }
    }
}

// 0x12ab00
// @address 0x12ab00 director_get_effective_perspective
int director_get_effective_perspective(c_director *object)
{
    int result = 3;
    bool active = false;
    game_globals_storage *globals = game_globals;
    if (globals != 0 && globals->game_in_progress && !globals->initializing && globals->map_active)
        active = true;
    if (active) {
        result = ((c_camera *)&object->m_camera)->get_perspective();
        if (result == 0 && object->blend > k_real_zero_storage)
            result = 1;
        if (game_globals->options.game_mode == 3)
            result = 3;
    }
    return result;
}

// 0x12abb8


// @address 0x12abb8 director_dispose_controller_screen
bool director_dispose_controller_screen(c_director *director)
{
    long controller_index = user_interface_controller_get_rumble_enabled(director->m_user_index);
    bool result = false;
    if (controller_index != -1) {
        c_gui_screen_widget *screen = director_get_scoreboard_screen(((e_window_index *)g_controller_globals.controller_windows)[controller_index]);
        if (screen) {
            screen->transition_out_with_transition_type(0, _screen_transition_normal);
            result = true;
        }
    }
    return result;
}

// 0x2026e0
void c_camera::set_target(long target)
{
    m_target = target;
}

// @address 0x298990 ?handle_deleted_object@c_camera@@UAAXJ@Z
void c_camera::handle_deleted_object(long object_index)
{
    if (m_target == object_index)
    {
        m_target = -1;
    }
}


#define DIRCAM_DEFAULT(x) if (g_camera_globals.x == saved.x) g_camera_globals.x = options->x;

// 0x298158
// @address 0x298158 ?camera_globals_initialize_from_tags@@YAXXZ
void camera_globals_initialize_from_tags(void)
{
    static s_camera_globals saved = g_camera_globals;
    s_camera_globals *options = &TAG_BLOCK_GET_ELEMENT(&global_game_globals->camera, 0, s_dircam_camera_definition)->options;
    DIRCAM_DEFAULT(field_of_view_scale)
    DIRCAM_DEFAULT(yaw_scale)
    DIRCAM_DEFAULT(pitch_scale)
    DIRCAM_DEFAULT(forward_scale)
    DIRCAM_DEFAULT(side_scale)
    DIRCAM_DEFAULT(up_scale)
    DIRCAM_DEFAULT(dead_camera_transition_time)
    DIRCAM_DEFAULT(falling_death_transition_time)
    DIRCAM_DEFAULT(dead_camera_initial_focus_distance)
    DIRCAM_DEFAULT(dead_camera_focus_distance)
    DIRCAM_DEFAULT(dead_camera_height)
    DIRCAM_DEFAULT(dead_camera_maximum_pitch_down)
    DIRCAM_DEFAULT(dead_camera_minimum_transition_time)
    DIRCAM_DEFAULT(director_idle_time)
    DIRCAM_DEFAULT(director_idle_speed)
    DIRCAM_DEFAULT(boost_scale)
    DIRCAM_DEFAULT(boost_time)
    DIRCAM_DEFAULT(boost_function)
    DIRCAM_DEFAULT(flying_camera_field_of_view)
    DIRCAM_DEFAULT(flying_camera_look_scale)
    DIRCAM_DEFAULT(collision_radius)
    DIRCAM_DEFAULT(flying_camera_transition_time)
    DIRCAM_DEFAULT(flying_camera_parameter_time)
    DIRCAM_DEFAULT(vertical_boost_time)
    DIRCAM_DEFAULT(boost_function)
    DIRCAM_DEFAULT(orbiting_camera_minimum_distance)
    DIRCAM_DEFAULT(orbiting_camera_maximum_distance)
    DIRCAM_DEFAULT(orbiting_camera_transition_time)
    DIRCAM_DEFAULT(orbiting_camera_height)
    DIRCAM_DEFAULT(orbiting_camera_maximum_pitch)
    DIRCAM_DEFAULT(orbiting_camera_minimum_pitch)
    DIRCAM_DEFAULT(saved_film_maximum_playback_speed)
    DIRCAM_DEFAULT(saved_film_revert_transition_time)
    DIRCAM_DEFAULT(saved_film_secondary_playback_speed)
    DIRCAM_DEFAULT(following_camera_transition_time)
    DIRCAM_DEFAULT(orbiting_camera_switch_transition_time)
    saved = *options;
}


// @address 0x11df81c g_screenshot_hue_saturation
__declspec(thread) s_render_postprocess_color_tweaking *g_screenshot_hue_saturation;

void __tls_set_g_director_camera_scripted_allocator(void *allocator)
{
    g_screenshot_hue_saturation = static_cast<s_render_postprocess_color_tweaking *>(allocator);
}

// @address 0x11df85c g_director_camera_scripted
__declspec(thread) bool *g_director_camera_scripted;

// @address 0x11df70c g_director_globals
__declspec(thread) s_controllers *g_director_globals;




// @address 0x202530 ?reserve_memory@?$t_restricted_allocation_manager@$02$0A@$0A@$1?__tls_set_g_director_globals_allocator@@YAXPAX@Z@@QAAPAXPBD0IJ@Z
template<>
void *t_restricted_allocation_manager<3, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_director_globals_allocator>::reserve_memory(
    const char *name, const char *type, unsigned int size, long alignment_bits)
{
  int member_index = restricted_memory_register_member(&g_restricted_regions[3], name, type, size, 0, __tls_set_g_director_globals_allocator,
      0, 0);
  unsigned int offset = g_restricted_regions[3].get_member_offset(member_index);
  void *address = g_restricted_address[3] + offset;
  int owner = get_current_thread_index();
  m_thread_id = owner;
  m_member_index = member_index;
  g_director_globals = (s_controllers *)address;
  return g_restricted_address[3] + g_restricted_regions[3].get_member_offset(member_index);
}

// @address 0x202608 ?reserve_memory@?$t_restricted_allocation_manager@$02$0A@$0A@$1?__tls_set_g_director_camera_scripted_flag_allocator@@YAXPAX@Z@@QAAPAXPBD0IJ@Z
template<>
void *t_restricted_allocation_manager<3, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_director_camera_scripted_flag_allocator>::reserve_memory(
    const char *name, const char *type, unsigned int size, long alignment_bits)
{
  int member_index = restricted_memory_register_member(&g_restricted_regions[3], name, type, size, 0,
      __tls_set_g_director_camera_scripted_flag_allocator, 0, 0);
  unsigned int offset = g_restricted_regions[3].get_member_offset(member_index);
  void *address = g_restricted_address[3] + offset;
  int owner = get_current_thread_index();
  m_thread_id = owner;
  m_member_index = member_index;
  g_director_camera_scripted = (bool *)address;
  return g_restricted_address[3] + g_restricted_regions[3].get_member_offset(member_index);
}

// @address 0x2026a8 ?allocate@?$t_restricted_allocation_manager@$02$0A@$0A@$1?__tls_set_g_director_camera_scripted_flag_allocator@@YAXPAX@Z@@UAAPAXIPBD@Z
template<>
void *t_restricted_allocation_manager<3, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_director_camera_scripted_flag_allocator>::allocate(unsigned int size, const char *name)
{
    return reserve_memory(name, "", size, 0);
}

// @address 0x2025d0 ?allocate@?$t_restricted_allocation_manager@$02$0A@$0A@$1?__tls_set_g_director_globals_allocator@@YAXPAX@Z@@UAAPAXIPBD@Z
template<>
void *t_restricted_allocation_manager<3, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_director_globals_allocator>::allocate(unsigned int size, const char *name)
{
    return reserve_memory(name, "", size, 0);
}

// @address 0x200d88 ?director_initialize@@YAXXZ
void director_initialize(void)
{
#ifdef HALO3_64BIT
    g_director_globals = (s_controllers *)g_director_globals_allocator.reserve_memory("director globals", 0, sizeof(s_controllers), 3);
#else
    g_director_globals = (s_controllers *)g_director_globals_allocator.reserve_memory("director globals", 0, 0x5c0, 0);
#endif
    g_director_camera_scripted = (bool *)g_director_camera_scripted_flag_allocator.reserve_memory(
        "director scripting", 0, 1, 0);
    *(unsigned char *)g_director_camera_scripted = 0;
    g_camera_script_globals = (s_camera_script_globals *)g_camera_script_globals_allocator.reserve_memory(
        "scripted camera globals", 0, 0xb8, 0);
}

// @address 0x5bec98 ?director_dispose@@YAXXZ
void director_dispose(void)
{
}

// 0x447b98
// @address 0x447b98 ?reserve_memory@?$t_restricted_allocation_manager@$02$0A@$0A@$1?__tls_set_g_director_camera_scripted_allocator@@YAXPAX@Z@@QAAPAXPBD0IJ@Z
template<>
void *t_restricted_allocation_manager<3, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_director_camera_scripted_allocator>::reserve_memory(
    const char *name, const char *type, unsigned int size, long alignment_bits) {
    // match: 0x447b98: value range assumption guides branch lowering
    __assume(alignment_bits == 0);
    m_member_index = restricted_region_register_member(3, name, type, size, alignment_bits,
        __tls_set_g_director_camera_scripted_allocator, 0, 0);
    m_thread_id = get_current_thread_index();
    return restricted_region_get_member_address(3, m_member_index);
}

// 0x447c38
// @address 0x447c38 ?allocate@?$t_restricted_allocation_manager@$02$0A@$0A@$1?__tls_set_g_director_camera_scripted_allocator@@YAXPAX@Z@@UAAPAXIPBD@Z
template<>
void *t_restricted_allocation_manager<3, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_director_camera_scripted_allocator>::allocate(unsigned int size, const char *name)
{
    return reserve_memory(name, "", size, 0);
}

// Clears the thread's director allocator, then restricted_region_free_member(3) with the object's word 4.
// @address 0x447c50 ?deallocate@?$t_restricted_allocation_manager@$02$0A@$0A@$1?__tls_set_g_director_camera_scripted_allocator@@YAXPAX@Z@@UAAXPAX@Z
template<> void t_restricted_allocation_manager<3, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_director_camera_scripted_allocator>::deallocate(void *memory)
{
    g_screenshot_hue_saturation = 0;
    restricted_region_free_member(3, m_member_index);
}

extern "C" {
// @address 0x40f80 ??_7c_authored_camera@@6B@
// @address 0x4313c8 ?get_type@c_authored_camera@@UBA?AW4e_camera_mode@@XZ
// @address 0x5bec98 ?set_target@c_authored_camera@@UAAXJ@Z
// @address 0x5bec98 ?post_update@c_camera@@UAAXPAUs_observer_command@@@Z
// @address 0x5bec98 ?handle_deleted_player@c_camera@@UAAXJ@Z
// @address 0x40dd0 ??_7c_scripted_camera@@6B@
// @address 0x4311b0 ?get_type@c_scripted_camera@@UBA?AW4e_camera_mode@@XZ
// @address 0x86ce0 ?get_target@c_camera@@UBAJXZ
// @address 0x2026e0 ?set_target@c_camera@@UAAXJ@Z
// @address 0x40f4c ??_7c_static_camera@@6B@
// @address 0x428ad0 ?get_type@c_static_camera@@UBA?AW4e_camera_mode@@XZ
// @address 0x45fc28 ?get_perspective@c_static_camera@@UBA?AW4e_director_perspective@@XZ
// @address 0x40f18 ??_7c_dead_camera@@6B@
// @address 0x3fc550 ?get_type@c_dead_camera@@UBA?AW4e_camera_mode@@XZ
// @address 0x42d380 ?get_perspective@c_dead_camera@@UBA?AW4e_director_perspective@@XZ
// @address 0x1494 ??_7c_first_person_camera@@6B@
// @address 0x45fc28 ?get_type@c_first_person_camera@@UBA?AW4e_camera_mode@@XZ
// @address 0x42d298 ?get_perspective@c_first_person_camera@@UBA?AW4e_director_perspective@@XZ

// @address 0x40eb0 ??_7c_orbiting_camera@@6B@
// @address 0x40e7c ??_7c_following_camera@@6B@
// @address 0x42d380 ?get_perspective@c_following_camera@@UBA?AW4e_director_perspective@@XZ
// @address 0x42d380 ?get_perspective@c_orbiting_camera@@UBA?AW4e_director_perspective@@XZ
// @address 0x42d380 ?get_type@c_orbiting_camera@@UBA?AW4e_camera_mode@@XZ
}

// @address 0x1440 ??_7c_game_director@@6B@
// @address 0x40b50 ??_7c_saved_film_director@@6B@
// @address 0x40e30 ??_7c_observer_director@@6B@
// @address 0x3fc598 ?get_type@c_observer_director@@UBA?AW4e_director_mode@@XZ
// @address 0x40e0c ??_7c_debug_director@@6B@

// @address 0x40e58 ??_7c_editor_director@@6B@
// @address 0x42d298 ?get_type@c_game_director@@UBA?AW4e_director_mode@@XZ
// @address 0x42d380 ?should_draw_hud_saved_film@c_game_director@@UBA_NXZ
// @address 0x45fc28 ?get_type@c_debug_director@@UBA?AW4e_director_mode@@XZ
// @address 0x42d380 ?get_type@c_saved_film_director@@UBA?AW4e_director_mode@@XZ
// @address 0x42d380 ?should_draw_hud@c_director@@UBA_NXZ
// @address 0x42d380 ?should_draw_hud_saved_film@c_director@@UBA_NXZ

#define TABLE g_director_globals
#define EP ((c_director *)&((char *)TABLE->directors)[_rotl(i*sizeof(s_director_storage),32)])
#ifdef HALO3_64BIT
#define CAM(i) ((s_camera_interface *)&((char *)TABLE->directors)[(size_t)(i) * sizeof(s_director_storage) + sizeof(void *)])
#else
#define CAM(i) ((s_camera_interface *)&((char *)TABLE->directors)[_rotr(i*sizeof(s_director_storage),32)+4])
#endif
// Rebuilds the camera in place over its saved fields; the sub-camera restarts untargeted.
__forceinline c_authored_camera::c_authored_camera() :
    m_sub_camera_active(false)
{
    m_camera.m_field_of_view = k_real_zero_storage;
    m_camera.m_target = -1;
    m_camera.m_camera_flags.clear();
    m_camera.m_camera_state = 0;
    s_director_zero_facing zero;
    zero.facing.yaw = k_real_zero_storage;
    zero.facing.pitch = k_real_zero_storage;
    m_camera.m_facing = zero.facing;
    m_camera.m_orientation_enabled = false;
    m_sub_camera_following = false;
}

// @address 0x200e38 director_reset_from_saved_slots
void director_reset_from_saved_slots(void)
{
    for (long i = 0; i < 4; i++) {
        switch (TABLE->slots[i].director_type) {
        case _director_mode_game: new (EP) c_game_director; break;
        case _director_mode_debug: new (EP) c_debug_director; break;
        case _director_mode_saved_film: new (EP) c_saved_film_director; break;
        case _director_mode_observer: new (EP) c_observer_director; break;
        case _director_mode_editor: new (EP) c_editor_director; break;
        // match: 0x200e38: unreachable hint omits dead branch check
        default: __assume(0);
        }
        s_camera_interface *camera=CAM(i);
        switch(TABLE->slots[i].camera_type) {
        case _camera_mode_following: new (camera) c_following_camera; break;
        case _camera_mode_orbiting: new (camera) c_orbiting_camera; break;
        case _camera_mode_flying: new (camera) c_flying_camera; break;
        case _camera_mode_first_person: new (camera) c_first_person_camera; break;
        case _camera_mode_dead: new (camera) c_dead_camera; break;
        case _camera_mode_static: new (camera) c_static_camera; break;
        case _camera_mode_scripted: new (camera) c_scripted_camera; break;
        case _camera_mode_authored: new (camera) c_authored_camera; break;
        // match: 0x200e38: unreachable hint omits dead branch check
        default: __assume(0);
        }
    }
}

// @address 0x2010d0 ?director_reset_all_directors@@YAXXZ
void director_reset_all_directors(void)
{
    director_reset_from_saved_slots();
}

// 0x201618
#define DIRECTOR_NONE (-1)
// Single-return funnel: every path joins at `done` so no early site
// rematerializes -1; the provably-redundant retest and the second TLS
// base load survive as written with no fence.
extern "C" long director_get_user_unit(int user_index)
{
  long player_index;
  long unit_index = DIRECTOR_NONE;

  if (user_index != DIRECTOR_NONE) {
    unit_index = ((s_user_globals *)*(void **)&player_mapping_globals)->unit_indices[user_index];
  }
  _ReadWriteBarrier();
  do {
    if (unit_index != DIRECTOR_NONE) {
      break;
    }
    if (user_index == DIRECTOR_NONE) {
      break;
    }
    player_index = ((s_user_globals *)*(void **)&player_mapping_globals)->player_indices[user_index];
    if (player_index == DIRECTOR_NONE) {
      break;
    }
    unit_index = ((player_datum *)((s_data_array *)*(void **)&player_data)->data)[(unsigned short)player_index].dead_unit_index;
  } while (0);
  return unit_index;
}
// 0x201690
// @address 0x201690 ?set_camera_mode@c_director@@QAA_NW4e_camera_mode@@ME@Z

static __forceinline s_observer_result *u_director_observer_get_camera(long user_index)
{
    return &g_observer_globals->blocks[user_index].camera_result;
}

// The dead, static and authored cameras are built in place by c_director::set_camera_mode.
__forceinline c_dead_camera::c_dead_camera(long user_index)
{
    m_user_index = (unsigned char)user_index;
    long player_index = -1;
    if (user_index != -1)
        player_index = ((s_user_globals *)*(void **)&player_mapping_globals)->player_indices[user_index];
    m_player_index = player_index;
    if (player_index == -1)
        return;
    player_datum *follow = &((player_datum *)((s_data_array *)*(void **)&player_data)->data)[(unsigned short)player_index];
    long unit_index = follow->unit_index;
    if (unit_index != -1) {
        c_dead_camera::set_target(unit_index);
        return;
    }
    unit_index = follow->dead_unit_index;
    if (unit_index == -1)
        return;
    c_dead_camera::set_target(unit_index);
}

__forceinline c_static_camera::c_static_camera(long user_index)
{
    s_observer_result *observer = u_director_observer_get_camera(user_index);
    m_position = observer->position;
    m_forward = observer->forward;
    m_up = observer->up;
    m_field_of_view = observer->horizontal_field_of_view;
}

__forceinline c_authored_camera::c_authored_camera(long user_index)
{
    m_camera = c_first_person_camera(-1);
    s_player_globals *observer_globals = g_observer_globals;
    m_position = observer_globals->blocks[user_index].camera_result.position;
    m_forward = observer_globals->blocks[user_index].camera_result.forward;
    m_sub_camera_active = false;
    m_sub_camera_following = false;
}

bool c_director::set_camera_mode(e_camera_mode camera_mode, float blend, unsigned char force)
{
    bool changed = camera_mode != ((c_camera *)&this->m_camera)->get_type();
    c_camera *camera = (c_camera *)&this->m_camera;
    if (changed || force) {
        switch (camera_mode) {
        case _camera_mode_following: {
            c_following_camera *following = (c_following_camera *)camera;
            if (following != 0) {
                new (following) c_following_camera(director_get_user_unit(this->m_user_index));
            }
            break;
        }
        case _camera_mode_orbiting: {
            c_orbiting_camera *orbiting = (c_orbiting_camera *)camera;
            if (orbiting != 0) {
                new (orbiting) c_orbiting_camera(director_get_user_unit(this->m_user_index));
            }
            break;
        }
        case _camera_mode_flying:
            if (camera != 0) {
                new (camera) c_flying_camera(this->m_user_index);
            }
            break;
        case _camera_mode_first_person: {
            c_first_person_camera *first_person = (c_first_person_camera *)camera;
            if (first_person != 0) {
                new (first_person) c_first_person_camera(director_get_user_unit(this->m_user_index));
            }
            break;
        }
        case _camera_mode_dead: {
            c_dead_camera *dead = (c_dead_camera *)camera;
            if (dead != 0) {
                new (dead) c_dead_camera(this->m_user_index);
            }
            break;
        }
        case _camera_mode_static: {
            c_static_camera *fixed = (c_static_camera *)camera;
            if (fixed != 0) {
                new (fixed) c_static_camera(this->m_user_index);
            }
            break;
        }
        case _camera_mode_scripted: {
            c_camera *scripted = (c_camera *)camera;
            if (scripted != 0) {
                new (scripted) c_scripted_camera;
            }
            break;
        }
        case _camera_mode_authored: {
            c_authored_camera *authored = (c_authored_camera *)camera;
            if (authored != 0) {
                new (authored) c_authored_camera(this->m_user_index);
            }
            break;
        }
        default:
            // match: 0x201690: unreachable hint omits dead branch check
            __assume(0);
        }
        this->blend = blend;
    }
    int screen = director_get_effective_perspective(this);
    if (g_director_globals->slots[this->m_user_index].perspective != screen
        || g_director_globals->slots[this->m_user_index].camera_type != camera_mode) {
        g_director_globals->slots[this->m_user_index].perspective = screen;
        g_director_globals->slots[this->m_user_index].camera_type = camera_mode;
        long user_index = this->m_user_index;
        long slot_index = 0;
        do {
            first_person_weapon_update_visibility(user_index, slot_index);
            slot_index++;
        } while (slot_index < 2);
    }
    return changed || force;
}

// 0x1e3138
// Put every active player's orbiting director into the authored camera and
// record it in the global camera-mode-changed bit mask.
void director_set_authored_cameras(void)
{
    int bit;
    int user_index;
    unsigned char active;
    int camera_mode;
    if (g_saved_film_timer_globals.timer_target == -1)
        return;
    float zero = k_real_zero_storage;
    user_index = 0;
    do {
        active = 0;
        if (user_index != -1)
            active = ((*(s_user_globals **)&player_mapping_globals)->player_indices[user_index] + 1) != 0;
        if (active != 0) {
            s_director_storage *directors = g_director_globals->directors;
            camera_mode = directors[user_index].get_director()->get_type();
            if (camera_mode == 1) {
                bit = 1 << (user_index & 31);
                if ((g_saved_film_timer_globals.authored_camera_users.m_storage[user_index >> 5] & bit) == 0) {
                    directors = g_director_globals->directors;
                    directors[user_index].get_director()->set_camera_mode(_camera_mode_authored, zero, 0);
                    g_saved_film_timer_globals.authored_camera_users.m_storage[user_index >> 5]
                        = g_saved_film_timer_globals.authored_camera_users.m_storage[user_index >> 5] | bit;
                }
            }
        }
        user_index = user_index + 1;
    } while (user_index < 4);
}

// @address 0x201398 ?director_script_camera@@YAX_N@Z
void director_script_camera(bool scripted)
{
    unsigned char scripted_byte = (unsigned char)scripted;
    if (*(unsigned char *)g_director_camera_scripted != scripted_byte) {
        *(unsigned char *)g_director_camera_scripted = scripted;
        long user_index = 0;
        long director_offset = 0;
        do {
            if (scripted_byte != 0) {
                ((c_director *)((char *)g_director_globals + director_offset))->set_camera_mode(_camera_mode_scripted, 0.0f, 0);
            } else {
                director_set_mode(user_index, (e_director_mode)(game_is_cooperative() != 0));
            }
            director_offset += (long)sizeof(s_director_storage);
            user_index++;
        } while (director_offset < 4 * (long)sizeof(s_director_storage));
    }
}

// @address 0x12a4a0 ?director_set_mode@@YAXJW4e_director_mode@@@Z
void director_set_mode(long user_index, e_director_mode mode)
{
    long index = user_index;
    e_director_mode director_mode = mode;
    switch (director_mode) {
    case _director_mode_game:
        {
        c_director *director = (c_director *)((char *)g_director_globals + index * (long)sizeof(s_director_storage));
        if (director != 0)
            new (director) c_game_director(user_index);
        }
        break;
    case _director_mode_debug:
        {
        c_director *director = (c_director *)((char *)g_director_globals + index * (long)sizeof(s_director_storage));
        if (director != 0)
            new (director) c_debug_director(user_index);
        }
        break;
    case _director_mode_saved_film:
        {
        c_director *director = (c_director *)((char *)g_director_globals + index * (long)sizeof(s_director_storage));
        if (director != 0)
            new (director) c_saved_film_director(user_index);
        }
        break;
    case _director_mode_observer:
        {
        c_director *director = (c_director *)((char *)g_director_globals + index * (long)sizeof(s_director_storage));
        if (director != 0)
            new (director) c_observer_director(user_index);
        }
        break;
    case _director_mode_editor:
        {
        c_director *director = (c_director *)((char *)g_director_globals + index * (long)sizeof(s_director_storage));
        if (director != 0)
            new (director) c_editor_director(user_index);
        }
        break;
    default:
        // match: 0x12a4a0: unreachable hint omits dead branch check
        __assume(0);
    }
    g_director_globals->slots[index].director_type = director_mode;
}

// @address 0x7c6004 g_director_camera_init_pending
bool g_director_camera_init_pending = true;

// @address 0x2011f0 director_initialize_for_new_map
void director_initialize_for_new_map(void)
{
    if (g_director_camera_init_pending) {
        camera_globals_initialize_from_tags();
        g_director_camera_init_pending = false;
    }
    *(unsigned char *)g_director_camera_scripted = 0;
    int i = 0;
    do {
        c_director *entry = g_director_globals->directors[i].get_director();
        if (entry != 0)
            new (entry) c_game_director(i);
        g_director_globals->slots[i].director_type = _director_mode_game;
        i++;
    } while (i < 4);
    director_reset();
}

// @address 0x5bec98 ?director_dispose_from_old_map@@YAXXZ
void director_dispose_from_old_map(void)
{
}
// Head of the camera script globals; camera_scripting.cpp owns the full layout.

// @address 0x7e7bfc g_director_watched_players
long g_director_watched_players[4];

// @address 0x12aa90 director_set_watched_player
extern "C" void director_set_watched_player(c_director *director, long player_index)
{
    long user_index = director->m_user_index;
    director->watched_player_index = player_index;
    if (player_index != -1) {
        if (game_is_cooperative())
            players_set_local_player(user_index, player_index);
        g_director_watched_players[user_index] = director->watched_player_index;
    }
}

// Every director vtable has get_type() in slot 0.
static __forceinline e_director_mode get_director_mode(c_director *director)
{
    return ((c_game_director *)director)->get_type();
}

// @address 0x202308 ?player_get_next_player_with_a_unit@c_director@@SAJJJJJ_N00PAJ@Z
long c_director::player_get_next_player_with_a_unit(long reference_player_index, long current_player_index,
    long preferred_player_index, long direction, bool same_team_only, bool live_unit_only, bool wrap,
    long *out_unit_index)
{
    long result = -1;
    long result_unit = -1;
    long team_index = -1;
    long count = 0;
    long match_index = -1;

    if (same_team_only) {
        long reference = current_player_index == -1 ? reference_player_index : current_player_index;
        if (reference != -1)
            team_index = player_get_team_index((unsigned int)reference);
    }

    long candidate_offset = 0;
    long candidate_players[16];
    long candidate_units[16];

    c_player_in_game_iterator iterator;
    iterator.begin();
    if (iterator.next()) {
        do {
            long unit_index = iterator.get_datum()->unit_index;
            long candidate_unit = unit_index != -1 ? unit_index : iterator.get_datum()->dead_unit_index;
            long player_index = iterator.get_index();
            bool accept = false;
            if (candidate_unit != -1) {
#ifdef HALO3_64BIT
                s_object_header_datum *header = &((s_object_header_datum *)object_header_data->data)[candidate_unit & 0xffff];
#else
                char *headers = *(char **)((char *)*(void **)&object_header_data + 68);
                s_object_header_datum *header = (s_object_header_datum *)(headers + ((candidate_unit << 4) & 0xffff0));
#endif
                unsigned short object_flags = ((const biped_datum *)header->datum)->biped.flags;
                if (header->object_type != 0 || (bool)((object_flags >> 4) & 1) == 0) {
                    if (player_index == preferred_player_index)
                        accept = true;
                    else if (player_index != reference_player_index && (!live_unit_only || unit_index != -1) &&
                        (!same_team_only || player_get_team_index((unsigned int)player_index) == team_index))
                        accept = true;
                }
            }
            if (player_index == current_player_index) {
                if (direction < 0)
                    match_index = count - 1;
                else if (direction > 0)
                    match_index = count + (accept ? 1 : 0);
                else
                    match_index = count;
            }
            if (accept) {
                count++;
                *(long *)((char *)candidate_players + candidate_offset) = player_index;
                *(long *)((char *)candidate_units + candidate_offset) = candidate_unit;
                candidate_offset += 4;
            }
        } while (iterator.next());
    }
    if (match_index == -1)
        match_index = direction > 0 ? 0 : count - 1;
    if (wrap) {
        if (match_index < 0)
            match_index = count - 1;
        else if (match_index >= count)
            match_index = 0;
    }
    if (match_index >= 0 && match_index < count) {
        result = candidate_players[match_index];
        result_unit = candidate_units[match_index];
    }
    if (out_unit_index != 0)
        *out_unit_index = result_unit;
    return result;
}

// @address 0x2012b8 director_reset
void director_reset(void)
{
    for (long i = 0; i < 4; i++)
        g_director_watched_players[i] = -1;
    for (long user_index = 0; user_index < 4; user_index++) {
        c_director *director = director_get(user_index);
        if (get_director_mode(director) != _director_mode_debug)
            director_set_mode(user_index, (e_director_mode)(game_is_cooperative() != 0));
    }
    g_director_globals->transition = 0.0f;
    g_director_globals->transition_remaining = 0.0f;
    g_camera_script_globals->mode = 0;
    g_camera_script_globals->object_index = -1;
    unsigned char scripted = *(unsigned char *)g_director_camera_scripted;
    director_script_camera(false);
    if (scripted != 0)
        director_script_camera(true);
}



// @address 0x2010d8 director_set_saved_film_modes
extern "C" void director_set_saved_film_modes(void)
{
    if (game_is_cooperative()) {
        director_script_camera(false);
        director_script_camera(*(bool *)g_director_camera_scripted);
        for (long user_index = 0; user_index < 4; user_index++) {
            long player_index = g_director_watched_players[user_index];
            s_controllers *globals = g_director_globals;
            c_director *director = globals->directors[user_index].get_director();
            if (get_director_mode(director) != _director_mode_saved_film)
                director_set_mode(user_index, _director_mode_saved_film);
            if (player_index != -1) {
                s_data_array *players = *(s_data_array **)&player_data;
                long absolute_index = player_index & 0xffff;
                if (absolute_index < players->first_unallocated) {
                    player_datum *player = (player_datum *)(players->data + players->size * absolute_index);
                    if (player->identifier != 0 && player->identifier == (player_index >> 16) && (player->flags & 2) == 0)
                        director_set_watched_player(director, player_index);
                }
            }
        }
    } else {
        director_reset();
    }
}


enum e_scene_object_type;



extern "C" {

}

static __forceinline s_cinematics_name_entry *cinematic_new_object_entry(long index)
{
    return &cinematic_new_globals->entries[index];
}

// @address 0x2412c0 ?director_setup_flying_camera_at_scenario_point@@YAXPBDJ@Z
void director_setup_flying_camera_at_scenario_point(const char *name, long flag_index)
{
    if (flag_index != -1) {
        scenario_cutscene_flag *flag = &global_scenario->cutscene_flags[flag_index];
        s_game_globals *globals =
            (s_game_globals *)global_tag_instances[datum_index_to_absolute_index(global_scenario_game_globals_index)].definition;
        s_tag_reference *camera = globals->cinematic_camera.s_tag_block::count ? (s_tag_reference *)globals->cinematic_camera.address : 0;
        long object_index = cinematic_object_create(name, 0, camera->index, (e_scene_object_type)'scen');
        if (object_index != -1 && cinematic_new_object_entry(object_index)->object_index != -1) {
            real_vector3d forward;
            real_vector3d up;
            vectors3d_from_euler_angles2d(&forward, &up, &flag->facing);
            object_set_position_direct(cinematic_new_object_entry(object_index)->object_index, &flag->position, &forward, &up, 0, false);
        }
    }
}

// @address 0x294150 ?find_any_player@c_observer_director@@UAAXXZ
void c_observer_director::find_any_player()
{
    long unit_index = -1;
    long player_index = c_director::player_get_next_player_with_a_unit(-1, -1, -1, 1, false, true, false, &unit_index);
    if (player_index != -1)
    {
        director_set_watched_player(this, player_index);
        get_camera()->set_target(unit_index);
    }
}


// @address 0x282348 ?find_any_player@c_saved_film_director@@UAAXXZ
void c_saved_film_director::find_any_player()
{
    long player_index = watched_player_index;
    director_try_and_get_user_player(m_user_index, &player_index);
    if (player_index != -1)
    {
        player_datum *player = player_get(player_index);
        long unit_index;
        if (player->unit_index != -1)
            unit_index = player->unit_index;
        else
            unit_index = player->dead_unit_index;
        director_set_watched_player(this, player_index);
        get_camera()->set_target(unit_index);
    }
    if (m_follow_any_player && get_camera()->get_target() == -1)
        c_observer_director::find_any_player();
}

// 0x282500: resolve the player index for a user, preferring the user's mapped
// player, then the controller-identified player, then a machine-local player.
struct s_data_array;

extern "C" bool director_try_and_get_user_player(long user_index, long *out_player_index)
{
    long found_index = DIRECTOR_NONE;
    bool result = false;
    long player_index = DIRECTOR_NONE;
    unsigned __int64 identifier;
    c_data_iterator<player_datum> player_iterator;
    s_machine_identifier machine;
    if (user_index != DIRECTOR_NONE)
        player_index = ((s_user_globals *)*(void **)&player_mapping_globals)->player_indices[user_index];
    if (players_is_player_local(player_index)) {
        found_index = player_index;
        result = true;
    }
    if (!result) {
        bool got_identifier = false;
        long controller_index = user_interface_controller_get_rumble_enabled(user_index);
        if (controller_index != DIRECTOR_NONE) {
            c_controller_interface *controller = &g_controller_globals.controllers[controller_index];
            ((const c_controller_interface *)controller)->get_player_identifier((s_player_identifier *)&identifier);
            got_identifier = true;
        }
        if (got_identifier) {
            bool identifier_is_zero = (identifier == 0);
            if (!identifier_is_zero) {
                player_iterator.begin((s_data_array *)*(void **)&player_data);
                if (player_iterator.next()) {
                    game_options *options = &game_globals->options;
                    // match: 0x282500: pinned intermediate keeps the two-step address form
                    __assume(options != 0);
                    const s_machine_identifier *local_machine = &options->local_machine;
                    do {
                        memcpy(&machine, local_machine, sizeof(machine));
                        player_datum *player = player_get(player_iterator.m_iterator.index);
                        if (memcmp(&player->machine, &machine, sizeof(machine)) == 0
                            && memcmp(&player->player_identifier, &identifier, sizeof(identifier)) == 0) {
                            found_index = player_iterator.m_iterator.index;
                            result = true;
                        }
                    } while (player_iterator.next());
                }
            }
        }
    }
    if (!result) {
        long match_count = 0;
        player_iterator.begin((s_data_array *)*(void **)&player_data);
        if (player_iterator.next()) {
            game_options *options = &game_globals->options;
            // match: 0x282500: pinned intermediate keeps the two-step address form
            __assume(options != 0);
            const s_machine_identifier *local_machine = &options->local_machine;
            do {
                memcpy(&machine, local_machine, sizeof(machine));
                player_datum *player = player_get(player_iterator.m_iterator.index);
                bool matches = memcmp(&player->machine, &machine, sizeof(machine)) == 0;
                if (matches && (found_index == DIRECTOR_NONE || match_count <= user_index)) {
                    found_index = player_iterator.m_iterator.index;
                    result = matches;
                    match_count++;
                }
            } while (player_iterator.next());
        }
    }
    if (!result) {
        long match_count = 0;
        player_iterator.begin((s_data_array *)*(void **)&player_data);
        while (player_iterator.next()) {
            if (found_index == DIRECTOR_NONE || match_count <= user_index) {
                found_index = player_iterator.m_iterator.index;
                match_count++;
            }
        }
    }
    *out_player_index = found_index;
    return result;
}

static __forceinline float director_update_cooperative_scale()
{
    return g_saved_film_timer_globals.playback_speed;
}
static __forceinline real_vector3d *director_update_scale_vector3d(const real_vector3d *vector, float scale, real_vector3d *result)
{
    float x = vector->i * scale;
    float y = vector->j * scale;
    float z = vector->k * scale;
    result->i = x;
    result->j = y;
    result->k = z;
    return result;
}
static __forceinline void director_update_observer_set_camera(long user_index, s_observer_command *command)
{
    s_player_block *observer = &g_observer_globals->blocks[user_index];
    observer_validate_camera_command(command);
    observer->command = command;
    bool was_pending = observer->pending;
    observer->updated = false;
    if (!was_pending) {
        observer->pending = true;
        command->timer = 0.0f;
        observer->command->flags |= 8;
        long *values = (long *)observer->command->parameter_timers;
        long count = 6;
        do {
            *values++ = 0;
        } while (--count != 0);
    }
}

// @address 0x12ac98 ?update@c_director@@UAAXM@Z
void c_director::update(float dt)
{
    s_observer_command command;
    memset(&command, 0, sizeof(command));
    c_camera *camera = (c_camera *)&this->m_camera;
    (void)camera->get_target();
    ((c_camera *)&this->m_camera)->update(m_user_index, dt, &command);
    if (camera->m_camera_flags.test(0)) {
        command.timer = 0.0f;
        if (--camera->m_camera_state == 0)
            camera->m_camera_flags.set(0, false);
    }
    if (watched_player_index == -1) {
        long player_index = g_director_watched_players[m_user_index];
        if (player_index != -1) {
            void *player = datum_try_and_get((struct s_data_array *)*(void **)&player_data, player_index);
            if (player != 0 && ((((player_datum *)player)->flags) & 2) == 0)
                director_set_watched_player(this, player_index);
        }
    }
    if (watched_player_index == -1) {
        long player_index = -1;
        if (m_user_index != -1)
            player_index = ((s_user_globals *)*(void **)&player_mapping_globals)->player_indices[m_user_index];
        director_set_watched_player(this, player_index);
    }
    if ((command.flags & 1) != 0) {
        if (blend > 0.0f) {
            if (camera->get_type() == _camera_mode_first_person && blend < 0.2f) {
                blend = 0.0f;
                command.parameter_timers[0] = 0.0f;
                command.parameter_timers[3] = 0.0f;
                command.parameter_flags[0] = 3;
                command.parameter_flags[3] = 3;
            } else {
                if (!(command.timer > blend - dt))
                    command.timer = blend - dt;
                if (camera->get_type() == _camera_mode_first_person)
                    command.flags |= 0x40;
            }
            float new_blend = blend - dt;
            if (new_blend < 0.0f)
                new_blend = 0.0f;
            blend = new_blend;
        }
        if (game_is_cooperative()) {
            director_update_scale_vector3d(&command.velocity, director_update_cooperative_scale(), &command.velocity);
        }
        memcpy(&stored_command, &command, sizeof(command));
    } else {
        stored_command.flags &= ~1UL;
    }
    director_update_observer_set_camera(m_user_index, &stored_command);
}

static __forceinline float director_camera_debug_add_revolution(float yaw) { return yaw + 6.2831855f; }
// @address 0x744608 g_debug_camera_speed
float g_debug_camera_speed = 1.0f;

// 0x1906a0
// Fades a fullscreen letterbox quad during director transitions in co-op games,
// then draws the debug camera position/yaw/pitch string for the first active user.
struct s_font_header;
static const char *const director_camera_debug_format = "%.3f %.3f %.3f / %.2f %.2f / %.3f [%s]";
// @rdata 0x8202867c
static const float director_camera_debug_degrees_per_radian = 57.29578f;
static __forceinline rectangle2d *director_camera_debug_set_rectangle(rectangle2d *rectangle, short x0, short y0, short x1, short y1)
{
    rectangle->x0 = x0;
    rectangle->y0 = y0;
    rectangle->x1 = x1;
    rectangle->y1 = y1;
    return rectangle;
}
// @address 0x1906a0 director_draw_fade_and_camera_debug
extern "C" void director_draw_fade_and_camera_debug(void)
{
    if (game_is_cooperative()) {
        short width;
        short height;
        if (rasterizer_main_get_video_mode()->fIsWideScreen != 0) {
            width = (short)c_rasterizer::render_globals.width;
            height = (short)c_rasterizer::render_globals.height;
        } else {
            width = 640;
            height = 480;
        }
        rectangle2d letterbox;
        director_camera_debug_set_rectangle(&letterbox, 0, 0, (short)width, (short)height);
        real_argb_color fade_color;
        fade_color.rgb = objects_get_default_argb_white()[2].rgb;
        float transition = g_director_globals->transition;
        if (transition >= 0.0f && g_director_globals->transition_remaining > 0.0f) {
            fade_color.alpha = 1.0f - transition / g_director_globals->transition_remaining;
            draw_quad(&letterbox, real_argb_color_to_pixel32(&fade_color));
        } else if (transition < 0.0f) {
            fade_color.alpha = transition / g_director_globals->transition_remaining;
            draw_quad(&letterbox, real_argb_color_to_pixel32(&fade_color));
        }
    }
    unsigned char show_camera_debug = (unsigned char)(player_control_can_use_debug_controls()
        && ((s_player_control_globals *)player_control_globals)->thread_active != 0
        && ((s_player_control_globals *)player_control_globals)->thread_unknown58a != 0);
    if (show_camera_debug == 0)
        return;
    long found_user = -1;
    long user_index = 0;
    long *player_indices = (*(s_user_globals **)&player_mapping_globals)->player_indices;
    do {
        if (*player_indices != -1) {
            found_user = user_index;
            break;
        }
        user_index++;
        player_indices++;
    } while (user_index < 4);
    const s_observer_result *camera = observer_try_and_get_camera(found_user);
    if (camera != 0) {
        c_rasterizer_draw_string draw_string;
        c_font_cache font_cache;
        real_euler_angles2d angles;
        const s_observer_result *debug_camera = camera;
        euler_angles2d_from_vector3d(&angles, &debug_camera->forward);
        if (angles.yaw < 0.0f)
            angles.yaw = director_camera_debug_add_revolution(angles.yaw);
        const char *mode_label = player_control_is_old_controls_active() ? "pan-cam" : "normal";
        char text[256];
        csnzprintf(text, 256, director_camera_debug_format,
            debug_camera->position.x, debug_camera->position.y, debug_camera->position.z,
            angles.yaw * director_camera_debug_degrees_per_radian, angles.pitch * director_camera_debug_degrees_per_radian,
            g_debug_camera_speed, mode_label);
        rectangle2d bounds;
        if (rasterizer_main_get_video_mode()->fIsWideScreen != 0) {
            c_rasterizer::get_fullscreen_render_title_safe_pixel_bounds(&bounds);
        } else if (rasterizer_main_get_video_mode()->fIsHiDef != 0) {
            director_camera_debug_set_rectangle(&bounds, 32, 24, 608, 456);
        } else {
            director_camera_debug_set_rectangle(&bounds, 48, 36, 592, 444);
        }
        bounds.y0 += 80;
        draw_string.m_font_id = _large_body_font;
        draw_string.m_font_header = font_get_header(_large_body_font);
        draw_string.m_style = _text_style_none;
        draw_string.m_justification = _text_justification_left;
        ((c_draw_string *)&draw_string)->set_bounds(&bounds);
        ((c_draw_string *)&draw_string)->set_color_deprecated(objects_get_default_argb_white(), true);
        ((c_draw_string *)&draw_string)->c_draw_string::draw(&font_cache, text);
    }
}

t_restricted_allocation_manager<3, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_director_globals_allocator> g_director_globals_allocator;
t_restricted_allocation_manager<3, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_director_camera_scripted_flag_allocator> g_director_camera_scripted_flag_allocator;
// @address 0x7bb56c ?g_director_camera_scripted_allocator@@3V?$t_restricted_allocation_manager@$02$0A@$0A@$1?__tls_set_g_director_camera_scripted_allocator@@YAXPAX@Z@@A
t_restricted_allocation_manager<3, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_director_camera_scripted_allocator> g_director_camera_scripted_allocator;

