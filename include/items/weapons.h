#ifndef ITEMS_WEAPONS_H
#define ITEMS_WEAPONS_H

// Functions defined in tu/objectslibxbox/weapons.cpp.

union real_point3d;
union real_vector3d;
struct s_aim_assist_targeting_result;
struct s_predicted_weapon_fire_data;
struct s_scenario_weapon;

float weapon_get_pitch_throttling(long weapon_index);
float weapon_compute_movement_penalty(long weapon_index, bool forward, bool moving);
bool weapon_is_melee_only(long weapon_index);
float weapon_get_zoom_magnification(long weapon_index, short zoom_level);
bool weapon_is_tracking(long object_index);
bool weapon_reloading(long weapon_index, long magazine_index, short *rounds_reloaded);
float weapon_estimate_time_to_target(long weapon_index, short barrel_index, float distance);
bool weapon_is_support_weapon_or_must_be_readied(long weapon_index);
void weapon_barrel_recover(long weapon_index, short barrel_index, bool flag);
long weapon_get_owner_unit_inventory_index(long weapon_index);
short weapon_rotate_zoom_level(long weapon_index, short zoom_level);
bool weapons_aim_assist_targeting_is_valid_for_plasma_pistol_lock(const s_aim_assist_targeting_result *reference);
long weapon_get_rounds_inventory(long datum_index, long slot, bool exclude_linked);
bool weapon_destroy_barrel(long weapon_index, long barrel_index);
bool weapon_damage_barrel(long weapon_index, long barrel_index);
bool weapon_is_zoomable(long index);
bool weapon_counts_towards_inventory_limit(long weapon_index);
bool weapon_uses_charging_melee(long weapon_index);
bool weapon_can_be_dual_wielded(long weapon_index);
void weapon_set_initial_rounds(long weapon_index, short magazine_index, short loaded_rounds, short total_rounds);
bool weapon_get_trigger_position(long weapon_index, short barrel_index, real_point3d *gun_position);
bool weapon_is_age_based(long definition_index);
void weapon_age(long weapon_index, float age_delta);
long weapon_effect_new(long weapon_index, long old_effect_index, long definition_index, float scale_a, float scale_b);
bool weapon_can_be_stowed(long weapon_index, long unit_index);
void weapon_stow(long weapon_index, long unit_index);
bool weapon_has_infinite_ammo(long weapon_index);
bool weapon_setup_first_person_emulation(long weapon_index, long animation_name);
long weapon_place(long weapon_index, const s_scenario_weapon *scenario_weapon);
void weapon_set_initial_rounds_from_scenario_weapon(long weapon_index, const s_scenario_weapon *scenario_weapon);
void weapons_fire_barrels();
void weapon_fire_barrels(long weapon_index, const s_predicted_weapon_fire_data *predicted_fire_data, long parent_unit_index,
    bool force, long forced_barrel_index);
void weapon_barrel_add_fire_error(long weapon_index, long barrel_index);
void weapon_ready(long weapon_index, bool skip_effects, bool instant);
bool weapon_put_away(long weapon_index, bool unused, bool force);
bool weapon_can_be_fired(long weapon_index);
bool weapon_reload(long weapon_index, long magazine_index);
void weapon_steal_goodies(long weapon_index, long victim_weapon_index);
void weapon_set_total_rounds(long weapon_index, short *rounds_array);
void weapon_set_current_amount(long weapon_index, float amount);
void weapon_report_kill(long weapon_index, long victim_index);
void weapon_reset(long weapon_index);
bool weapon_set_state(long weapon_index, long state, bool force);
bool weapon_aim(long weapon_index, short barrel_index, const real_point3d *origin, const real_point3d *target, float *maximum_time_scale, bool high_arc,
    real_vector3d *aim_vector, float *unused, float *result_distance, float *result_speed, bool *linear);
bool weapon_compute_function_value(long object_index, long function_name, long argument, float *value, bool *active, bool *deterministic);
void weapon_barrel_fire(long weapon_index, short barrel_index, bool force);
extern "C" void weapon_interrupt_reload(long weapon_index, bool reload_magazines);
extern "C" void weapon_interrupt_reload_and_clear_timer(long weapon_index, long unused, bool clear_timer);
extern "C" long weapon_replace_with_replacement(long weapon_index);
bool weapon_is_dual_wielded(long object_index);
extern "C" void weapon_set_state_update_timer(long weapon_index);
long weapon_get_owner_unit_index(long weapon_index);
long weapon_get_effect_object_index(long object_index);
void weapon_barrel_clear_empty_click_bit(long weapon_index, short barrel_index);
long weapon_get_projectile_owner_unit_index(long weapon_index);
float weapon_trigger_get_charged_fraction(long object_index, short timer_index);
// match: 0x70e558: __restrict keeps the shipped callee mangling (PIA_N) and 0xa8 body; plain prototype emits PA_N with an 848B body and shifts .text +0x400
bool weapon_barrel_can_fire_this_tick(long weapon_index, short barrel_index, bool *__restrict fire, bool *__restrict exhausted, bool *__restrict blocked);
void weapon_magazine_finish_reload(long weapon_index, short magazine_index, bool interrupted);
extern "C" bool weapon_try_and_get_magazine_value(long object_index, short slot_index, short *value);
bool weapon_is_charged_plasma_pistol(long weapon_index, bool any_state);
float weapon_get_age(long object_index);
bool weapon_trigger_can_begin_charging(long weapon_index, short trigger_index);
void weapon_barrel_idle(long weapon_index, short barrel_index);
void weapon_barrel_finish_recover(long weapon_index, short barrel_index);
void weapon_barrel_start_ejection_port(long weapon_index, short barrel_index, bool flag);
void weapon_magazine_update_reload(long weapon_index, short magazine_index);
extern "C" void weapon_stop_first_person_sound(long weapon_index, long animation_name, bool unknown);
bool weapon_update_first_person_emulation(long weapon_index);
bool weapon_barrel_check_for_reload(long weapon_index, long barrel_index);
bool weapon_magazine_start_or_continue_reload(long weapon_index, short magazine_index);
void weapon_magazine_start_chamber(long weapon_index, short magazine_index);
void weapon_trigger_fully_charged(long weapon_index, short trigger_index);
void weapon_trigger_release_charge(long weapon_index, short trigger_index);
void weapon_trigger_recover(long weapon_index, short trigger_index, bool force);
void weapon_trigger_begin_tracking(long weapon_index, short trigger_index);
void weapon_trigger_begin_locked(long weapon_index, short trigger_index);
void weapon_trigger_begin_locking(long weapon_index, short trigger_index);
void weapon_trigger_begin_charging(long weapon_index, short trigger_index);
void weapon_magazine_set_state(long weapon_index, short magazine_index, short state);
extern "C" bool weapon_submit_reload_action(long weapon_index, short magazine_index, bool forced);
bool weapon_magazine_update(long weapon_index, long magazine_index);
void weapon_detonate(long weapon_index);
bool weapon_barrel_can_fire(long weapon_index, short barrel_index, bool ignore_reload);
void weapon_barrel_create_projectiles(long weapon_index, short barrel_index, const s_predicted_weapon_fire_data *predicted_fire_data,
    long predicted_fire_count, bool force);

struct weapon_interface_state;

void weapon_build_weapon_interface_state(long weapon_index, weapon_interface_state *state);
bool weapon_handle_potential_inventory_item(long weapon_index, long item_index, long unit_index, long player_index, short *rounds_picked_up);
extern "C" bool weapon_barrel_is_ready_to_fire(long weapon_index, short barrel_index, long *shots, long *recovery_ticks,
    long *recovery_duration, long *firing_ticks, long *firing_duration);
void weapon_barrel_set_fire_bit(long weapon_index, short barrel_index, bool set);

enum e_unit_messages;

void weapon_send_message_to_unit(long weapon_index, e_unit_messages message);
void weapon_trigger_determine_analog_autofire_behavior(long weapon_index, long trigger_index, bool *fire, bool *alt, bool *clear);
bool weapon_tracked_on_something_trackable(long weapon_index, long *target_index, char *model_target_index);
bool weapon_owner_autoaimed_on_lockable_target(long weapon_index, long *target_index, char *model_target_index);

#include "cseries/cseries_wrapped.h"
enum e_weapon_control_flags;
void weapon_owner_update(long weapon_index, c_flags<e_weapon_control_flags, unsigned short, 8> flags, float trigger);

#endif // ITEMS_WEAPONS_H
