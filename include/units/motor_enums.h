#ifndef UNITS_MOTOR_ENUMS_H
#define UNITS_MOTOR_ENUMS_H

// Actions available to the unit action system.
enum e_action {
    _action_none = -1,

    _action_death_ping = 0,
    _action_custom_animation,
    _action_stop_custom_animation,
    _action_get_infected,
    _action_hostile_morph,
    _action_resurrect,
    _action_hard_ping,
    _action_ping,
    _action_morph,
    _action_morph_arrival,
    _action_infect,    // 10
    _action_posture,
    _action_posture_exit,
    _action_vehicle_exit,
    _action_vehicle_exit_and_detach,
    _action_vehicle_ejection,
    _action_vehicle_board,
    _action_vehicle_enter,
    _action_open,
    _action_close,
    _action_vehicle_exit_immediate,    // 20
    _action_vehicle_flip,
    _action_climb_attach,
    _action_climb_detach,
    _action_wall_lean,
    _action_wall_lean_open,
    _action_token,    // the table spells it "action_token"
    _action_takeoff,
    _action_perch,
    _action_perch_jump,
    _action_land,    // 30
    _action_jump,
    _action_leap_anticipation,
    _action_leap,
    _action_leap_recovery,
    _action_emit_ai,
    _action_berserk,
    _action_flurry,
    _action_deploy_turret,
    _action_smash,
    _action_vault,    // 40
    _action_hoist,
    _action_ai_impulse,
    _action_melee_ai,
    _action_weapon_pickup,
    _action_weapon_trade,
    _action_primary_weapon_drop,
    _action_secondary_weapon_drop,
    _action_surge,
    _action_phase,
    _action_stow,    // 50
    _action_draw,
    _action_infection_spew,
    _action_kamikaze,
    _action_primary_weapon_switch,
    _action_secondary_weapon_switch,
    _action_grenade,
    _action_equipment,
    _action_equipment_pickup,
    _action_equipment_drop,
    _action_melee_player,    // 60
    _action_primary_weapon_primary_reload,
    _action_primary_weapon_secondary_reload,
    _action_secondary_weapon_primary_reload,
    _action_secondary_weapon_secondary_reload,
    _action_soft_ping,
    _action_primary_weapon_primary_recoil,
    _action_primary_weapon_secondary_recoil,
    _action_primary_weapon_primary_chamber,
    _action_primary_weapon_secondary_chamber,
    _action_primary_weapon_primary_charged,    // 70
    _action_primary_weapon_secondary_charged,
    _action_secondary_weapon_primary_recoil,
    _action_secondary_weapon_secondary_recoil,
    _action_secondary_weapon_primary_chamber,
    _action_secondary_weapon_secondary_chamber,
    _action_secondary_weapon_primary_charged,
    _action_secondary_weapon_secondary_charged,
    _action_primary_weapon_overheat_enter,
    _action_primary_weapon_overheat_loop,
    _action_primary_weapon_overheat_exit,    // 80
    _action_secondary_weapon_overheat_enter,
    _action_secondary_weapon_overheat_loop,
    _action_secondary_weapon_overheat_exit,

    k_action_count                              // 84
};

// Animation names weapon_actions.cpp plays for the primary and secondary overheat loops.
enum e_weapon_overheat_loop_animation_name {
    _primary_weapon_overheat_loop_animation_name = 106,
    _secondary_weapon_overheat_loop_animation_name = 666
};

enum e_motor_program {
    _motor_program_none = -1,
    _motor_program_ground = 0,
    _motor_program_airborne = 1,
    // Values 2, 3, 9 from March g_motor_program_names ("biped_dead", "biped_vehicle", "biped_posture");
    // 3 keeps its biped_ prefix to distinguish it from 10 (_motor_program_vehicle, the vehicle itself).
    _motor_program_dead = 2,
    _motor_program_biped_vehicle = 3,
    _motor_program_flying = 4,
    _motor_program_perch = 5,
    _motor_program_bunker = 6,
    _motor_program_climb = 7,
    _motor_program_leap = 8,
    _motor_program_posture = 9,
    _motor_program_vehicle = 10,
    _motor_program_custom_animation = 11,
    _motor_program_phase = 12,
    _motor_program_creature_ground = 13,
    _motor_program_creature_flying = 14,
    _motor_program_effect_scenery = 15
};

enum e_motor_task {
    _motor_task_none = -1,
    _motor_task_default = 0,
    _motor_task_jump = 1,
    _motor_task_stun = 2,
    _motor_task_accelerate = 3,
    _motor_task_dash = 4,
    _motor_task_notify_physics_built = 5,
    _motor_task_handle_simulation_warp = 6,
    _motor_task_force_airborne = 7,
    _motor_task_force_relaxation_state = 8,
    _motor_task_force_heavy = 9,
    _motor_task_board = 10,
    _motor_task_ejection = 11,
    _motor_task_takeoff = 12,
    _motor_task_perch = 13,
    _motor_task_land = 14,
    _motor_task_attach = 15,
    _motor_task_detach = 16,
    _motor_task_bunker_open = 17,
    _motor_task_bunker_close = 18,
    _motor_task_melee = 19,
    _motor_task_resurrect = 20,
    _motor_task_open = 23,
    _motor_task_close = 24,
    _motor_task_enter = 26,
    _motor_task_weapon_animation = 27,
    _motor_task_weapon_stow = 28,
    _motor_task_weapon_draw = 29,
    _motor_task_grenade = 30,
    _motor_task_equipment = 31,
    _motor_task_exit = 32,
    _motor_task_ping = 33,
    _motor_task_die = 34,
    _motor_task_animation = 35,
    _motor_task_posture = 36,
    _motor_task_transient_posture = 37,
    _motor_task_hoist = 38,
    _motor_task_custom_animation = 39,
    _motor_task_stop_animation = 40,
    _motor_task_leap_anticipation = 41,
    _motor_task_leap_recovery = 43,
    _motor_task_surge = 44,
    _motor_task_phase = 45
};

enum e_motor_result {
    _motor_result_none = 0,
    _motor_result_failed = 0,
    _motor_result_failure = 0,
    _motor_result_pending = 1,
    _motor_result_success = 2
};

enum e_motor_resource {
    _motor_resource_none = -1,
    _motor_resource_0 = 0,
    _motor_resource_base = 0,
    _motor_resource_1 = 1,
    _motor_resource_overlay = 1,
    _motor_resource_2 = 2,
    _units_primary_weapon_resource = 2,
    _motor_resource_3 = 3,
    _units_secondary_weapon_resource = 3
};

typedef char motor_enums_size_action[(sizeof(e_action) == 4) ? 1 : -1];
typedef char motor_enums_size_program[(sizeof(e_motor_program) == 4) ? 1 : -1];
typedef char motor_enums_size_task[(sizeof(e_motor_task) == 4) ? 1 : -1];
typedef char motor_enums_size_result[(sizeof(e_motor_result) == 4) ? 1 : -1];
typedef char motor_enums_size_resource[(sizeof(e_motor_resource) == 4) ? 1 : -1];

#endif // UNITS_MOTOR_ENUMS_H
