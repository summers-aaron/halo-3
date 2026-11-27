#ifndef UNITS_UNIT_ANIMATION_H
#define UNITS_UNIT_ANIMATION_H

// Functions defined in tu/objectslibxbox/unit_animation.cpp.

#include "units/unit_animation_key_frame_status_types.h"

class c_animation_channel;
e_unit_animation_key_frame_status unit_animation_get_key_frame_status(long unit_index, const c_animation_channel *channel, bool primary);

struct real_matrix4x3;

long unit_animation_get_action_name(long unit_index);
bool unit_animation_weapon_raised(long unit_index);
void unit_set_emotional_state(long unit_index, long emotion, float value, float rate);
long unit_animation_get_desired_seat(long object_index, bool, bool);
void unit_animation_locate_auxilary_animations(long unit_index);
void unit_animation_locate_constant_overlay(long unit_index);
void unit_animation_reset(long unit_index);
bool unit_animation_build_root_matrix_by_seat_and_state(long unit_index, long seat, long state, bool armed, long flags, real_matrix4x3 *matrix);
bool unit_animation_test_weapon(long unit_index, long label, long weapon_class, long weapon_type, bool must_be_readied);
bool unit_animation_set_animation_time(long unit_index, float time);
bool unit_animation_set_weapon(long unit_index, long weapon_class, long weapon_type, bool force);
float unit_animation_compute_interpolation_time(bool unit_is_player, bool force_standard, long current_state, long desired_state,
    float minimum_interpolation_time);
void unit_animation_raise_weapon(long unit_index, float duration);
void unit_animation_lower_weapon(long unit_index, float duration);
bool unit_animation_allows_gunner_control(long unit_index);
void unit_animation_update_mixing_board(long unit_index);
extern "C" bool unit_animation_allows_weapon_ik(long unit_index);
extern "C" unsigned char unit_animation_is_running_damage_overlay(unsigned int unit_index);
extern "C" void unit_animation_snap_mixing_sliders(long unit_index);
void unit_animation_reset_interpolators(long unit_index);

class c_animation_manager;

void unit_animation_setup_mixing_board(c_animation_manager *animation_manager);
void unit_animation_setup_mixing_board_channels(c_animation_manager *animation_manager);
extern "C" void unit_animation_setup_weapon_aux_channels(long unit_index);

// Data defined in tu/objectslibxbox/unit_animation.cpp that other units use.
extern bool enable_player_transitions;

#endif // UNITS_UNIT_ANIMATION_H
