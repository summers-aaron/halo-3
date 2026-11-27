#ifndef CAMERA_OBSERVER_H
#define CAMERA_OBSERVER_H

#include "objects/object_type_enums.h"
#include "math/real_constant_pool.h"

extern "C" const float k_observer_default_aspect_ratio;
#include "physics/collision_test_flags.h"

// Functions defined in tu/blamlibxbox/observer.cpp.

union real_point3d;
struct s_observer;
struct s_observer_command;
struct s_observer_result;

const s_observer_result *observer_try_and_get_camera(long user_index);
bool valid_world_real_point3d(real_point3d *point);
void observer_adopt_global_update_list();
void observer_initialize_for_saved_game(long game_state);
void observer_initialize_after_load_saved_game(long game_state);
void __tls_set_g_observer_gamestate_globals_allocator(void *allocator);
void __tls_set_g_observer_globals_allocator(void *allocator);
void observer_validate_camera_command(s_observer_command *command);
void observer_clear(s_observer *observer);
void observer_update(float elapsed);

// The field of view cameras use when nothing narrows it (March: observer_suggested_field_of_view() returns it).
extern float g_observer_default_field_of_view;

union real_vector3d;

extern "C" void __tls_set_g_observer_globals(void *address);
void observer_fix_focus_position_to_safe_position(long object_index, union real_point3d * point, long first_ignore,
    long second_ignore, union real_point3d const * origin);
extern "C" void observer_fix_point_for_collision(real_point3d *point, const real_vector3d *direction, const real_point3d *start, float distance, float radius);
void observer_update_polynomial(long user_index);
void observer_update_velocities(long user_index);
extern "C" void observer_update_interpolation(long user_index);
extern "C" void observer_update_result(long user_index, float *effect);
extern "C" void observer_update_collision(long user_index, float *effect, float elapsed);

// Named allocator objects owned by this unit.
template<long, void (*)(void *), void (*)(void *), void (*)(void *)>
class t_restricted_allocation_manager;
extern t_restricted_allocation_manager<3, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_observer_globals_allocator> g_observer_globals_allocator;
extern t_restricted_allocation_manager<3, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_observer_gamestate_globals_allocator> g_observer_gamestate_globals_allocator;

class c_map_variant;
struct s_variant_multiplayer_object_properties_definition;

s_collision_test_flags observer_get_collision_flags(int index);
extern "C" long map_variant_new_object(c_map_variant *map_variant, long definition_index, long count, long placement_index,
    const real_point3d *position, const real_vector3d *forward, const real_vector3d *up, long scenario_index,
    e_object_type object_type, const s_variant_multiplayer_object_properties_definition *properties, short owner_index);

struct s_observer_update;

extern s_observer_update global_observer_update_list[4];
extern "C" float k_observer_field_of_view_minimum;
extern "C" float k_observer_field_of_view_maximum;
extern "C" float k_observer_command_focus_distance_max;

void observer_initialize();
void observer_dispose();
void observer_initialize_for_new_map();
void observer_dispose_from_old_map();
void observer_initialize_for_new_structure_bsp(unsigned long structure_bsp_mask);
void observer_dispose_from_old_structure_bsp(unsigned long structure_bsp_mask);
void observer_initialize_before_saved_game(long game_state);

#endif // CAMERA_OBSERVER_H
