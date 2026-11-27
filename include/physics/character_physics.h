#ifndef PHYSICS_CHARACTER_PHYSICS_H
#define PHYSICS_CHARACTER_PHYSICS_H

#include "physics/havok_group_types.h"

// Functions defined in tu/blamlibxbox/character_physics.cpp that other units call.

class c_character_physics_component;
union real_point3d;
union real_vector3d;
struct s_character_physics_definition;
struct s_character_physics_move_input_datum;
struct s_character_physics_update_input_datum;
class c_animation_manager;
class phys_shape;

void character_physics_move_input_datum_initialize(s_character_physics_move_input_datum *input, c_character_physics_component *component,
    const s_character_physics_definition *definition, long object_index, long rigid_body_index, bool at_rest, bool airborne,
    bool disable_ground, bool recently_jumped, bool reset_ground, bool force_ground, bool ignore_climbable_surfaces, bool unused,
    const real_point3d *position, const real_vector3d *velocity, const real_vector3d *forward, const real_vector3d *up);
void character_physics_update_input_datum_initialize_ground(s_character_physics_update_input_datum *input, c_character_physics_component *component,
    bool disable_player_control, bool sprinting, bool ground_contact, float friction, float crouch);
void character_physics_update_input_datum_initialize_flying(long node_count, s_character_physics_update_input_datum *input,
    c_character_physics_component *component, float friction, float boost);
extern "C" void character_physics_update_input_datum_initialize(long object_index, s_character_physics_update_input_datum *input,
    c_character_physics_component *component,
    const s_character_physics_definition *definition, long havok_component_index, const c_animation_manager *animation_manager,
    float scale, e_havok_group havok_group, bool connected, bool facing_constrained, bool facing_locked, bool animation_disabled,
    bool at_rest, bool use_actor_gravity, bool reduced_adhesion, bool component_flag, bool stun_active, bool snap_to_ground,
    bool reference_relative, bool limit_upward,
    float movement_scale, float acceleration_scale, const real_vector3d *movement, const real_point3d *position,
    const real_vector3d *forward, const real_vector3d *up, const real_vector3d *facing, const real_vector3d *aiming,
    const real_vector3d *external_velocity, const phys_shape *shape);

extern "C" bool g_character_physics_flying_noise;

#endif // PHYSICS_CHARACTER_PHYSICS_H
