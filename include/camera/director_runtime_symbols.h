#ifndef CAMERA_DIRECTOR_RUNTIME_SYMBOLS_H
#define CAMERA_DIRECTOR_RUNTIME_SYMBOLS_H
#include "game/player_control_globals.h"
#include "math/real_math_types.h"
// Legacy storage declarations shared with the director implementation.
#include "cutscene/cinematic_new_globals_types.h"
extern "C" {
bool players_is_player_local(long player_index);
}


struct s_player_globals;
extern "C" {
extern const real_vector3d k_director_zero_vector;

}
extern __declspec(thread) s_player_globals *g_observer_globals;

#include "cache/restricted_memory_regions.h"
#include "camera/director.h"
template<> void t_restricted_allocation_manager<3, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_director_globals_allocator>::deallocate(void *memory);
template<> void t_restricted_allocation_manager<3, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_director_camera_scripted_flag_allocator>::deallocate(void *memory);

#endif
