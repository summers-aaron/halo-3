#ifndef MOTOR_WEAPON_ACTIONS_H
#define MOTOR_WEAPON_ACTIONS_H

struct s_weapon_action_block {
    long flags;
    // Filler ahead of grenade_throw_state; never referenced through the weapon view (weapon_actions.cpp is the sole user).
    unsigned short pad;
    signed char grenade_throw_state;
    bool immediate;
    signed char throw_ticks;
    signed char release_ticks;
    signed char duration_ticks;
    long grenade_index;
    long grenade_type;
};

// Functions defined in tu/objectslibxbox/weapon_actions.cpp.

union real_point3d;
union real_vector3d;
enum e_unit_messages;

void unit_action_send_message_to_first_person(long unit_index, e_unit_messages message);

bool unit_weapon_in_overheated_loop(long unit_index, long weapon_slot);
void unit_action_weapon_put_away_end(long unit_index, bool primary);
void unit_action_weapon_set_unarmed_animation(long unit_index, bool force);
bool unit_get_inherited_velocity(long unit_index, bool inherit, const real_point3d *position, const real_vector3d *direction, real_vector3d *result);
bool unit_action_weapon_stow_immediate(long unit_index, bool immediate);
void unit_action_weapon_ready(long unit_index, long animation, bool immediate, bool force, bool primary);
extern "C" bool unit_throw_grenade_create(long unit_index, long projectile_definition_index, long *projectile_index);
extern "C" long unit_throw_grenade(long unit_index, const real_point3d *position, const real_vector3d *direction,
    const real_point3d *origin, const real_vector3d *forward, const real_vector3d *up, real_vector3d *velocity, bool primary, bool keep);
extern "C" bool unit_action_weapon_put_away_submit(long unit_index, long action, bool immediate, bool simulated, bool flags, bool primary);
void unit_throw_grenade_release(long unit_index, bool release, const real_point3d *position, const real_vector3d *direction);

#endif // MOTOR_WEAPON_ACTIONS_H
