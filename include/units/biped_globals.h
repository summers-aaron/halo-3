#ifndef UNITS_BIPED_GLOBALS_H
#define UNITS_BIPED_GLOBALS_H

#include "game/player_control_constants.h"
#include "physics/collision_test_flags.h"

extern float global_lean_turn_angle;
extern float global_lean_angle;
extern float global_lean_pivot_offset;

// Pooled constants read by the biped surface and camera paths.
extern const float k_biped_aiming_vector_minimum_magnitude;

static __forceinline float biped_full_turn_angle() { return k_player_control_full_turn; }
static __forceinline float biped_half_turn_angle() { return k_player_control_half_turn; }
#endif // UNITS_BIPED_GLOBALS_H
