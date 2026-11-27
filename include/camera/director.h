#ifndef CAMERA_DIRECTOR_H
#define CAMERA_DIRECTOR_H

#include "camera/camera_mode_types.h"
#include "camera/observer_director.h"
#include "cache/restricted_memory_regions.h"

// Functions defined in tu/blamlibxbox/director.cpp.

void director_update(float dt);
void director_reset_all_directors(void);
extern "C" void director_set_saved_film_modes(void);
void __tls_set_g_director_camera_scripted_allocator(void *allocator);
extern t_restricted_allocation_manager<3, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_director_camera_scripted_allocator> g_director_camera_scripted_allocator;
void director_script_camera(bool scripted);
void director_setup_flying_camera_at_scenario_point(const char *name, long flag_index);

class c_director;

void director_set_authored_cameras(void);
extern "C" void director_set_watched_player(c_director *director, long player_index);
extern "C" void director_draw_fade_and_camera_debug(void);
extern "C" void director_reset(void);

extern "C" int director_get_effective_perspective(c_director *object);
extern "C" bool director_dispose_controller_screen(c_director *director);
extern "C" int director_new_portal_bitvectors(void *object, int a, int b);
extern "C" int director_new_camera_scripted(void *object, int a, int b);
extern "C" void director_initialize_table_reset(void);
extern "C" void director_initialize_portal_bitvectors_reset(void);

// The debug camera speed scale (placed by the // @address lines in director.cpp and flying_camera.cpp).
extern "C" float g_debug_camera_speed;
extern "C" void director_reset_from_saved_slots(void);
extern "C" long director_get_user_unit(int user_index);
extern "C" void director_initialize_for_new_map(void);
extern "C" bool director_try_and_get_user_player(long user_index, long *out_player_index);

// Shared declarations used by director.cpp.
template<long, void (*)(void *), void (*)(void *), void (*)(void *)>
class t_restricted_allocation_manager;
enum e_director_perspective;
enum e_director_mode;
e_director_perspective director_get_perspective(long datum_index, long previous_bit);
void director_set_mode(long user_index, e_director_mode mode);
void __tls_set_g_director_globals_allocator(void *address);
void __tls_set_g_director_camera_scripted_flag_allocator(void *allocator);
extern t_restricted_allocation_manager<3, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_director_globals_allocator> g_director_globals_allocator;
extern t_restricted_allocation_manager<3, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_director_camera_scripted_flag_allocator> g_director_camera_scripted_flag_allocator;
extern "C" bool g_director_camera_init_pending;
extern "C" long g_director_watched_players[4];

void director_initialize(void);
void director_dispose(void);
void director_dispose_from_old_map(void);

#endif // CAMERA_DIRECTOR_H
